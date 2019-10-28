

#include "do_client.h"
#include "config.h"
#include <libsoup/soup.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sqlite3.h>

#define DO_CLIENT_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_CLIENT, DoClientPrivate))
//#define DO_CLIENT_GET_PRIVATE(object)do_client_get_instance_private (object)
typedef struct _DoValue	DoValue;
typedef struct _DoQueue	DoQueue;

enum
{
	PROP_0,
	PROP_URL,
	PROP_STORE,
	PROP_CACHED,
};

struct _DoValue
{
	GDateTime *time;
	JsonNode  *node;
};

struct _DoQueue
{
	DoClient    *client;
	guint64      index;
	GFunc        callback;
	gpointer     data;
	SoupSession *session;
	SoupMessage *msg;
	gboolean     archive;
	gchar       *key;
	gboolean     canceled;
	JsonNode    *cache;
};

static void do_value_free(DoValue *value)
{
	g_object_unref(value->time);
	json_node_free(value->node);
}

struct _DoClientPrivate
{
	gchar       *url;
	gboolean     cached;
	GHashTable  *hash;
	gchar       *store;
	SoupSession *session;
	//guint64      queue_index;
	GSList      *queue_message;
	gchar       *filename;
	sqlite3     *conn;
	gchar       *cache_url;
	gchar       *cache_store;
};

G_DEFINE_TYPE_WITH_CODE (DoClient, do_client, G_TYPE_OBJECT, G_ADD_PRIVATE(DoClient))

static JsonNode *do_client_request_valist_(DoClient *client, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache, GFunc callback, gpointer data, va_list args);
static JsonNode *do_client_proccess_message(DoClient *client, SoupMessage *msg, const gchar *key, gboolean archive, GFunc callback, gpointer data, JsonNode *cache);
static void do_client_update_cache_store_url(DoClient *client);

static void do_client_init(DoClient *temp)
{

}
static int do_client_read_cache_callback(void *client, int argc, char **argv, char **azColName)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE(client);
    const gchar *key;
    const gchar *data;
    GDateTime *time;
    JsonNode *node;
    DoValue *value;
    GError *error = NULL;
	JsonParser *parser;

    key = argv[0];
    data = argv[2];
    if ( (!data) || (data[0] == '\0') ) {
    	g_warning("empty cache %s %s\n",argv[0],argv[1]);
    	return 0;
    }
    time = do_client_strptime(argv[1]);
    if ( !time )
    	time = g_date_time_new_local(1970,1,1,0,0,0);
    if ( !g_strcmp0(key, "CACHE_STORE") ) {
    	priv->cache_store = g_strdup(data);
    	return 0;
    }
    if ( !g_strcmp0(key, "CACHE_URL") ) {
    	priv->cache_url = g_strdup(data);
    	return 0;
    }

	parser = json_parser_new();
	json_parser_load_from_data(parser, data, -1, &error);

	if ( error ) {
		g_error ("Unable to parse %s\n", error->message);
		g_error_free (error);
	}
#ifdef JSON12
	node = json_parser_get_root(parser);
	json_node_ref(node);
#else
	node = json_node_copy(json_parser_get_root(parser));
#endif
	g_object_unref (parser);
	if ( !node )
		g_warning("key empty %s\n", key); // fix me
    value = g_hash_table_lookup(priv->hash, key);
    if ( value ) {
    	g_date_time_unref(value->time);
#ifdef JSON12
	    json_node_unref(value->node);
#else
        json_node_free(value->node);
#endif
	    value->time = time;
	    value->node = node;
	}
	else {
		value = g_new(DoValue,1);
		value->time = time;
		value->node = node;
		g_hash_table_insert(priv->hash, g_strdup(key), value);
	}
    return 0;
}
static gboolean do_client_open_db(DoClient *client)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (client);
	gchar *error;

	priv->filename = g_strdup_printf("%s/%s",g_get_user_data_dir(),DB_FILENAME);

	if ( sqlite3_open(priv->filename, &priv->conn) != SQLITE_OK ) {
		g_error("Ошибка открытия/создания БД: %s\n", sqlite3_errmsg(priv->conn));
		priv->conn = NULL;
	}
	if ( priv->conn && sqlite3_exec(priv->conn,
			"CREATE TABLE IF NOT EXISTS cache (key CHAR(100) NOT NULL,time CHARE(23) NOT NULL,data TEXT)",
			NULL, NULL, &error) != SQLITE_OK ) {
		g_error("Ошибка создания таблицы cache %s\n", error ? error : "");
		priv->conn = NULL;
		sqlite3_free(error);
	}
	if ( priv->conn && sqlite3_exec(priv->conn,
			"CREATE UNIQUE INDEX IF NOT EXISTS idx_key_cache ON cache (key)",
			NULL, NULL, &error) != SQLITE_OK ) {
		g_error("Ошибка создания индекса таблицы cache %s\n", error ? error : "");
		priv->conn = NULL;
		sqlite3_free(error);
	}
	if ( priv->conn && sqlite3_exec(priv->conn,
			"SELECT key,time,data FROM cache",
			do_client_read_cache_callback, client, &error) != SQLITE_OK ) {
		g_error("Ошибка выполнения запроса %s\n", error ? error : "");
		priv->conn = NULL;
		sqlite3_free(error);
	}
	return priv->conn != NULL;
}

static GObject *do_client_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;

	//DoClient *temp;
	DoClientPrivate *priv;

	object = G_OBJECT_CLASS (do_client_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv = DO_CLIENT_GET_PRIVATE(object);
	priv->hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)do_value_free);
	priv->cached = TRUE;
	do_client_open_db(DO_CLIENT(object));
	return object;
}
static void do_client_clear_cache(GObject *object, const gchar *key)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (DO_CLIENT(object));
	gchar *error = NULL;
	gchar *sql_text;
	if ( !priv->hash )
        return;
	if ( !key ) {
		g_hash_table_remove_all(priv->hash);
		sql_text = sqlite3_mprintf("DELETE FROM cache");
	}
	else {
		g_hash_table_remove(priv->hash, key);
		sql_text = sqlite3_mprintf("DELETE FROM cache WHERE key = %Q", key);
	}
	if ( priv->conn && sqlite3_exec(priv->conn,
			sql_text,
			NULL, NULL, &error) != SQLITE_OK ) {
		g_error("Ошибка удаления записей cache %s\n", error ? error : "");
		priv->conn = NULL;
		sqlite3_free(error);
	}
	sqlite3_free(sql_text);
	if ( priv->conn )
		sqlite3_get_autocommit(priv->conn);
}
static void do_client_finalize (GObject *object)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (object);
	do_client_clear_cache(object,NULL);
	g_free(priv->url);
	g_free(priv->store);
	if ( priv->conn )
		sqlite3_close(priv->conn);

	G_OBJECT_CLASS (do_client_parent_class)->finalize (object);
}

static void do_client_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_URL:
			g_value_set_string(value, priv->url);
	        break;
		case PROP_STORE:
			g_value_set_string(value, priv->store);
	        break;
	    case PROP_CACHED:
			g_value_set_boolean(value, priv->cached);
	        break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
}

static void do_client_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE(object);

	switch (prop_id)
	{
		case PROP_URL: {
			const gchar *url;
			url = g_value_get_string(value);
			if ( g_strcmp0(priv->url, url) ) {
				g_assert(url != NULL);
				if ( g_strcmp0(priv->cache_url, url) ) {
					g_free(priv->cache_url);
					priv->cache_url = g_strdup(url);
					do_client_clear_cache(object, NULL);
					do_client_update_cache_store_url(DO_CLIENT(object));
				}
				g_free(priv->url);
				priv->url = g_strdup(url);
			}
			break;
		}
		case PROP_STORE: {
			const gchar *store;
			store = g_value_get_string(value);
			if ( g_strcmp0(priv->store, store) ) {
				g_assert(store != NULL);
				if ( g_strcmp0(priv->cache_store, store) ) {
					g_free(priv->cache_store);
					priv->cache_store = g_strdup(store);
					do_client_clear_cache(object, NULL);
					do_client_update_cache_store_url(DO_CLIENT(object));
				}
				g_free(priv->store);
				priv->store = g_strdup(store);
			}
			break;
		}
		case PROP_CACHED:
			priv->cached = g_value_get_boolean(value);
			break;
	}
}

static void do_client_class_init (DoClientClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_client_constructor;
	object_class->finalize     = do_client_finalize;
	object_class->get_property = do_client_get_property;
	object_class->set_property = do_client_set_property;

	//g_type_class_add_private (object_class, sizeof (DoClientPrivate));


    g_object_class_install_property
        (object_class,
         PROP_URL,
         g_param_spec_string("url",
                      "Адрес запроса к серверу",
                      "Адрес запроса к серверу",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property
        (object_class,
         PROP_STORE,
         g_param_spec_string("store",
                      "Код аптеки",
                      "Код аптеки",
                      NULL,
                       G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
                                     PROP_CACHED,
                                     g_param_spec_boolean ("cached",
                                                          "Кэширование ответов",
                                                          "Кэширование ответов",
                                                           TRUE,
                                                           G_PARAM_READWRITE));
}

GObject *do_client_new (const gchar *url, const gchar *store)
{
	return g_object_new (DO_TYPE_CLIENT,
			     "url", url,
			     "store", store,
			     (gpointer) NULL);
}
static gboolean check_valid_cache(const gchar *key, GDateTime *time)
{
	GDateTime *now = g_date_time_new_now_local();
	gboolean res;
	if ( !g_strcmp0(key, "Goods") )
		res = g_date_time_difference(now, time) < CACHE_GOODLIST_VALID_IN_MICROSECONDS;
	else
		res = g_date_time_difference(now, time) < CACHE_OBJECT_VALID_IN_MICROSECONDS;
	g_date_time_unref(now);
	return res;
}
JsonNode *do_client_request(DoClient *client, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache,...)
{
	JsonNode *node;
	va_list args;

    va_start (args, nocache);

	node = do_client_request_valist_(client, method, func, key, archive, nocache, NULL, NULL, args);

    va_end (args);
    return node;
}
JsonNode *do_client_request_valist(DoClient *client, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache, va_list args)
{
	return do_client_request_valist_(client, method, func, key, archive, nocache, NULL, NULL, args);
}
JsonNode *do_client_request_async(DoClient *client, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache, GFunc callback, gpointer data, ...)
{
	JsonNode *node = NULL;
	va_list args;

    va_start (args, data);

	node = do_client_request_valist_(client, method, func, key, archive, nocache, callback, data, args);

    va_end (args);
    return node;
}
static void do_client_update_cache_store_url(DoClient *client)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (client);
	if ( priv->cached ) {
		gchar *timestr, *data, *sql_text, *error = NULL, *key;
		GDateTime *time = g_date_time_new_now_local();
		timestr = g_date_time_format(time, DATETIME_FORMAT);
		g_date_time_unref(time);
		if ( priv->cache_store ) {
			data = priv->cache_store;
			key = "CACHE_STORE";
			sql_text = sqlite3_mprintf("REPLACE INTO cache (key,time,data) VALUES (%Q,%Q,%Q)",
					 key,timestr, data);
			if ( priv->conn && sqlite3_exec(priv->conn,
					sql_text,
					NULL, NULL, &error) != SQLITE_OK ) {
				g_error("Ошибка обновление записей cache %s\n", error ? error : "");
				priv->conn = NULL;
				sqlite3_free(error);
			}
			sqlite3_free(sql_text);
		}
		if ( priv->cache_url ) {
			data = priv->cache_url;
			key = "CACHE_URL";
			sql_text = sqlite3_mprintf("REPLACE INTO cache (key,time,data) VALUES (%Q,%Q,%Q)",
					 key,timestr, data);
			if ( priv->conn && sqlite3_exec(priv->conn,
					sql_text,
					NULL, NULL, &error) != SQLITE_OK ) {
				g_error("Ошибка обновление записей cache %s\n", error ? error : "");
				priv->conn = NULL;
				sqlite3_free(error);
			}
			sqlite3_free(sql_text);
		}
		g_free(timestr);
		if ( priv->conn )
			sqlite3_get_autocommit(priv->conn);
	}
}
static void do_client_update_cache(DoClient *client, const gchar *key, JsonNode *node)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (client);
	DoValue *value;
	value = g_hash_table_lookup(priv->hash, key);
	if ( !node ) {
		g_warning("empty update key %s\n", key);//fix me
		return;
	}
	if ( value ) {
		g_date_time_unref(value->time);
#ifdef JSON12
		json_node_unref(value->node);
#else
		json_node_free(value->node);
#endif
		value->time = g_date_time_new_now_local();
		value->node = node;
	}
	else {
		value = g_new(DoValue,1);
		value->time = g_date_time_new_now_local();
		value->node = node;
		g_hash_table_insert(priv->hash, g_strdup(key), value);
	}
	if ( priv->cached ) {
		gchar *timestr, *data, *sql_text, *error = NULL;
		JsonGenerator *generator = json_generator_new();
		json_generator_set_root(generator, value->node);
		data = json_generator_to_data(generator, NULL);
		g_object_unref(generator);
		if ( (!data) || (data[0] == '\0') ) {
			g_warning("insert empty key %s\n", key);
			return;
		}
		timestr = g_date_time_format(value->time, DATETIME_FORMAT);
		sql_text = sqlite3_mprintf("REPLACE INTO cache (key,time,data) VALUES (%Q,%Q,%Q)",
				 key,timestr, data);
		g_free(timestr);
		g_free(data);
		if ( priv->conn && sqlite3_exec(priv->conn,
				sql_text,
				NULL, NULL, &error) != SQLITE_OK ) {
			g_error("Ошибка обновление записей cache %s\n", error ? error : "");
			priv->conn = NULL;
			sqlite3_free(error);
		}
		sqlite3_free(sql_text);
		if ( priv->conn )
			sqlite3_get_autocommit(priv->conn);
	}
}

JsonNode *do_client_request_valist_async(DoClient *client, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache, GFunc callback, gpointer data, va_list args)
{
	return do_client_request_valist_(client, method, func, key, archive, nocache, callback, data, args);
}
static void do_client_message_finished(SoupSession *session, SoupMessage *msg, gpointer data)
{
	DoQueue *queue = data;
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (queue->client);
	//if ( queue->canceled ) // debug it
	//	g_print("finished canceled\n");
	//else
	//	g_print("finished\n");
	priv->queue_message = g_slist_remove(priv->queue_message, data);
	if ( !queue->canceled )
		do_client_proccess_message(queue->client, msg, queue->key, queue->archive, queue->callback, queue->data, queue->cache);
	g_free(queue->key);
	g_free(queue);
}
gboolean do_client_cancel_request(DoClient *client, const gchar *key)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (client);
	GSList *node;
	DoQueue *data;
	g_print("cancel\n");
	for ( node = priv->queue_message; node; node = node->next ) { // to do not "equal" key
		data = node->data;
		if ( !g_strcmp0(data->key, key) ) {
			data->canceled = TRUE;
			//soup_message_finished(data->msg);
			//priv->queue_message = g_slist_remove(priv->queue_message, data);
			return TRUE;
		}
	}
	return FALSE;
}

static JsonNode *do_client_proccess_message(DoClient *client, SoupMessage *msg, const gchar *key, gboolean archive, GFunc callback, gpointer data, JsonNode *cache)
{
	//g_print("process message key %s\n", key);//debug it
	JsonNode *res = NULL;
	if ( msg->status_code != 200 ) {
		g_warning("Error from %s status: %d %s\n", soup_uri_to_string(soup_message_get_uri(msg),TRUE), msg->status_code, msg->reason_phrase);
	}
	else {
		JsonParser *parser;
		GError *error = NULL;
		JsonObject *obj;
		gssize length;
		gchar *out = NULL;

		if ( archive ) {
			// to do
		}

		if ( !out ) {
			out = (gchar*)msg->response_body->data;
			length = msg->response_body->length;
			//g_print("%s\n", out);
		}
		parser = json_parser_new();
		json_parser_load_from_data(parser, out, length, &error);

		if ( error ) {
			g_error ("Unable to parse %s\n", error->message);
			g_error_free (error);
		}
		res = json_parser_get_root (parser);
		if ( res ) {
			obj = json_node_get_object(res);
			if ( obj ) {
				if ( json_object_has_member(obj, "cachevalid") &&
				     json_object_get_int_member(obj, "cachevalid") == 1 ) {
					g_object_unref (parser);
					if ( callback ) {
						callback(cache, data);
					}
                    return cache;
				}
			}
#ifdef JSON12
			json_node_ref(res);
#else
            res = json_node_copy(res);
#endif
			g_object_unref (parser);
			do_client_update_cache(client, key, res);
//			json_node_ref(res);
		}
	}
	if ( callback ) {
		callback(res, data);
	}
	return res;
}
static DoValue *do_client_get_cache_value(DoClient *client, const gchar *key)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (client);
	DoValue *value;
	value = g_hash_table_lookup(priv->hash, key);
	return value;
}

static JsonNode *do_client_request_valist_(DoClient *client, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache, GFunc callback, gpointer data, va_list args)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (client);
	JsonNode *res = NULL;
	GDateTime *cache_time = NULL;

	if ( priv->cached && !nocache ) {
		DoValue *value;
		value = do_client_get_cache_value(client, key);
		if ( value ) {
#ifdef JSON12
			json_node_ref(value->node);
#else
            json_node_free(value->node);
#endif
			cache_time = value->time;
			if ( check_valid_cache(key, cache_time) ) {
				if ( callback ) {
					callback(value->node, data);
					return NULL;
				}
				else {
					if ( !callback )
						return value->node;
					else {
						callback(value->node, data);
						return NULL;
					}
				}
			}
			res = value->node;
		}
	}

	gchar *url;
	url = g_strdup_printf("%s/%s?store=%s", priv->url, func, priv->store);
	if ( !g_strcmp0(method, "GET") ) {
		gchar *buf, *name, *value;
	    name = va_arg(args, gchar*);
		while ( name != NULL ) {
			value = va_arg(args, gchar*);
			buf = g_strdup_printf("%s&%s=%s", url, name, value);
			g_free(url);
			url = buf;
			if ( cache_time ) {
				value = do_client_strftime(cache_time);
				buf = g_strdup_printf("%s&cache=%s", url, value);
				g_free(url);
				g_free(value);
				url = buf;
			}
		    name = va_arg(args, gchar*);
		}
	}
	SoupSession *session;
	SoupMessage *msg;

	session = soup_session_new();
	msg = soup_message_new(method, url);
	//g_print("start message key %s\n", key);//debug it
	if ( !callback ) {
		soup_session_send_message(session, msg);
		res = do_client_proccess_message(client, msg, key, archive, callback, data, res);
	}
	else {
		DoQueue *queue;
		queue = g_new(DoQueue, 1);
		queue->callback = callback;
		queue->data = data;
		queue->client = client;
		//queue->index = priv->queue_index++;
		queue->session = session;
		queue->msg = msg;
		queue->archive = archive;
		queue->key = g_strdup(key);
		queue->canceled = FALSE;
		queue->cache = res;
		priv->queue_message = g_slist_append(priv->queue_message, queue);

		soup_session_queue_message(session, msg, do_client_message_finished, queue);
	}
	return res;
}
gchar *do_client_strftime(GDateTime *time)
{
	gchar *ret;
	ret = g_date_time_format(time, DATETIME_FORMAT);
	return ret;
}
GDateTime *do_client_strptime(const gchar* str)
{
	gchar **dt = NULL, **d = NULL, **t = NULL;
	dt = g_strsplit(str,"T",-1);
	if ( !dt || !dt[0] || !dt[1] ) {
		g_strfreev(dt);
		return NULL;
	}
	d = g_strsplit(dt[0],"-",-1);
	if ( !d || !d[0] || !d[1] || !d[2] ) {
		g_strfreev(dt);
		g_strfreev(d);
		return NULL;
	}
	t = g_strsplit(dt[1],":",-1);
	if ( !t || !t[0] || !t[1] || !t[2] ) {
		g_strfreev(dt);
		g_strfreev(t);
		g_strfreev(d);
		return NULL;
	}
	GDateTime *res;
	res = g_date_time_new_local(atoi(d[0]),atoi(d[1]),atoi(d[2]),atoi(t[0]),atoi(t[1]),atoi(t[2]));
	g_strfreev(dt);
	g_strfreev(t);
	g_strfreev(d);
	return res;
}
void do_client_set_cache(DoClient *client, const gchar *key, JsonNode *node)
{
	do_client_update_cache(client, key, node);
}
JsonNode  *do_client_get_cache(DoClient *client, const gchar *key)
{
	DoValue *value;
	value = do_client_get_cache_value(client, key);
	if ( value )
		return value->node;
	else
		return NULL;
}
