
#include "do_client.h"
#include "config.h"
#include <libsoup/soup.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <zlib.h>

#define DO_CLIENT_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_CLIENT, DoClientPrivate))
//#define DO_CLIENT_GET_PRIVATE(object)do_client_get_instance_private (object)
typedef struct _DoValue	DoValue;
typedef struct _DoQueue	DoQueue;
#

enum
{
	PROP_0,
	PROP_URL,
	PROP_STORE,
	PROP_CACHED,
};

struct _DoValue
{
	GDateTime  *time;
	JsonParser *parser;
	JsonNode   *node;
#ifdef DEBUG
    gchar      *key;
#endif // DEBUG
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
#ifdef DEBUG
    g_print("Clear cache \"%s\"\n", value->key);
    g_free(value->key);
#endif
    if ( value->time ) {
        g_date_time_unref(value->time);
    }
	if ( value->parser ) {
        g_object_unref(value->parser);
    }
	if ( value->node ) {
        json_node_free(value->node);
    }
    value->parser = NULL;
    value->node = NULL;
    value->time = NULL;
}
#ifdef DEBUG
static void do_value_set_parser(DoValue *value, GDateTime *time, JsonParser *parser, const gchar *key)
#else
static void do_value_set_parser(DoValue *value, GDateTime *time, JsonParser *parser)
#endif
{
    do_value_free(value);
	value->time = time;
	value->parser = parser;
#ifdef DEBUG
    value->key = g_strdup(key);
#endif // DEBUG
}
#ifdef DEBUG
static void do_value_set_node(DoValue *value, GDateTime *time, JsonNode *node, const gchar *key)
#else
static void do_value_set_node(DoValue *value, GDateTime *time, JsonNode *node)
#endif
{
    do_value_free(value);
	value->time = time;
	//if ( JSON_NODE_TYPE(node) != JSON_TYPE_OBJECT )
    //    g_warning(json_node_get_object(node));
	//else
    value->node = json_node_copy(node);
#ifdef DEBUG
    value->key = g_strdup(key);
#endif // DEBUG
}
#ifdef DEBUG
static DoValue *do_value_new_from_parser(GDateTime *time, JsonParser *parser, const gchar *key)
#else
static DoValue *do_value_new_from_parser(GDateTime *time, JsonParser *parser)
#endif
{
    DoValue *value;
    value = g_new0(DoValue,1);
	value->time = time;
	value->parser = parser;
#ifdef DEBUG
    value->key = g_strdup(key);
#endif // DEBUG
	return value;
}
#ifdef DEBUG
static DoValue *do_value_new_from_node(GDateTime *time, JsonNode *node, const gchar *key)
#else
static DoValue *do_value_new_from_node(GDateTime *time, JsonNode *node)
#endif
{
    DoValue *value;
    value = g_new0(DoValue,1);
	value->time = time;
	//if ( JSON_NODE_TYPE(node) != JSON_TYPE_OBJECT )
    //    g_warning(json_node_get_object(node));
	//else
    value->node = json_node_copy(node);
#ifdef DEBUG
    value->key = g_strdup(key);
#endif // DEBUG
	return value;
}
static JsonNode *do_value_get_node(DoValue *value)
{
	return value->parser ? json_parser_get_root(value->parser) : value->node;
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
	guint        source;
};

G_DEFINE_TYPE_WITH_CODE (DoClient, do_client, G_TYPE_OBJECT, G_ADD_PRIVATE(DoClient))

static JsonNode *do_client_request_valist_(DoClient *client, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache, GFunc callback, gpointer data, va_list args);
static JsonNode *do_client_proccess_message(DoClient *client, SoupMessage *msg, const gchar *key, gboolean archive, GFunc callback, gpointer data, JsonNode *cache);
static void do_client_update_cache_store_url(DoClient *client);
static gboolean do_client_cleaning(DoClient *client);

static void do_client_init(DoClient *temp)
{

}
static int do_client_read_cache_callback(void *client, int argc, char **argv, char **azColName)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE(client);
    const gchar *key;
    const gchar *data;
    GDateTime *time;
    //JsonNode *node;
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
    value = g_hash_table_lookup(priv->hash, key);
    if ( value )
#ifdef DEBUG
            do_value_set_parser(value, time, parser, key);
#else
            do_value_set_parser(value, time, parser);
#endif
	else {
#ifdef DEBUG
		value = do_value_new_from_parser(time, parser, key);
#else
		value = do_value_new_from_parser(time, parser);
#endif
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
			"SELECT key,time,data FROM cache WHERE key IN ('CACHE_STORE','CACHE_URL')",
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
	priv->source = g_timeout_add(CACHE_LIVETIME_SECONDS*1000, (GSourceFunc)do_client_cleaning, object);
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
	if ( priv->conn ) {
		sqlite3_get_autocommit(priv->conn);
	}
}
static void do_client_finalize (GObject *object)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (object);
	do_client_clear_cache(object,NULL);
	g_free(priv->url);
	g_free(priv->store);
	if ( priv->conn )
		sqlite3_close(priv->conn);
    if ( priv->source )
        g_source_remove(priv->source);
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
				g_warning("Ошибка обновление записей cache %s\n", error ? error : "");
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
				g_warning("Ошибка обновление записей cache %s\n", error ? error : "");
				priv->conn = NULL;
				sqlite3_free(error);
			}
			sqlite3_free(sql_text);
		}
		g_free(timestr);
		if ( priv->conn ) {
			sqlite3_get_autocommit(priv->conn);
		}
	}
}
static void do_client_update_cache(DoClient *client, const gchar *key, JsonParser *parser, JsonNode *node, gchar *text, gint length)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (client);
	DoValue *value;
	GDateTime *time;
	JsonGenerator *generator = NULL;

	value = g_hash_table_lookup(priv->hash, key);
	time = g_date_time_new_now_local();
	if ( value ) {
        if ( parser )
#ifdef DEBUG
            do_value_set_parser(value, time, parser, key);
#else
            do_value_set_parser(value, time, parser);
#endif
        else
#ifdef DEBUG
            do_value_set_node(value, time, node, key);
#else
            do_value_set_node(value, time, node);
#endif
    }
	else {
        if ( parser )
#ifdef DEBUG
            value = do_value_new_from_parser(time, parser, key);
#else
            value = do_value_new_from_parser(time, parser);
#endif // DEBUG
        else
#ifdef DEBUG
            value = do_value_new_from_node(time, node, key);
#else
            value = do_value_new_from_node(time, node);
#endif
		g_hash_table_insert(priv->hash, g_strdup(key), value);
	}
	if ( priv->cached ) {
		gchar *timestr, *sql_text, *error = NULL, *data;
		if ( text ) {
            if ( length != -1 ) {
                data = g_malloc(length + 1);
                strncpy(data, text, length);
                data[length] = '\0';
            }
            else
                data = text;
        }
        else {
            generator = json_generator_new();
            json_generator_set_root(generator, do_value_get_node(value));
            data = json_generator_to_data(generator, NULL);
		}
		timestr = g_date_time_format(value->time, DATETIME_FORMAT);
		sql_text = sqlite3_mprintf("REPLACE INTO cache (key,time,data) VALUES (%Q,%Q,%Q)",
				 key,timestr, data);
		g_free(timestr);
		if ( priv->conn && sqlite3_exec(priv->conn,
				sql_text,
				NULL, NULL, &error) != SQLITE_OK ) {
			g_warning("Ошибка обновление записей cache %s\n", error ? error : "");
			priv->conn = NULL;
			sqlite3_free(error);
		}
		sqlite3_free(sql_text);
		if ( priv->conn ) {
			sqlite3_get_autocommit(priv->conn);
		}
		if ( !text || length != -1 )
            g_free(data);
        if ( generator )
            g_object_unref(generator);
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
#ifdef DEBUG
	if ( queue->canceled )
        g_print("finished canceled\n");
#endif // DEBUG
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
#ifdef DEBUG
    g_print("cancel request \"%s\"\n", key);
#endif
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
gchar *inf(gchar *source, size_t len, size_t *outlen)
{
    #define BUFFER_SIZE 64*1024*8
    gchar *crnt;
    int ret;
    unsigned have;
    z_stream strm;
    GSList *pieces = NULL;
    GSList *lens = NULL;

    unsigned char *out;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return NULL;
    crnt = source;
    do {

        strm.avail_in = BUFFER_SIZE < (len - (crnt - source)) ? BUFFER_SIZE : len - (crnt - source);
        strm.next_in = crnt;

        do {
            out = g_malloc(BUFFER_SIZE);

            strm.avail_out = BUFFER_SIZE;
            strm.next_out = out;

            ret = inflate(&strm, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)
                return NULL;
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return NULL;
            }
            have = BUFFER_SIZE - strm.avail_out;

            pieces = g_slist_append(pieces, out);
            lens  = g_slist_append(lens, GINT_TO_POINTER(have));

        } while (strm.avail_out == 0);

        crnt += BUFFER_SIZE;

    } while (ret != Z_STREAM_END && crnt < source + len);

    (void)inflateEnd(&strm);
    if ( ret == Z_STREAM_END ) {
        GSList *l, *p;
        size_t out_len = 0;
        for ( l = lens; l; l = l->next )
            out_len += GPOINTER_TO_INT(l->data);
        out = g_malloc(out_len+1);
        crnt = out;
        p = pieces;
        for ( l = lens; l ; l = l->next ) {
            size_t len1;
            len1 = GPOINTER_TO_INT(l->data);
            memcpy(crnt, p->data, len1);
            crnt = crnt + len1;
            p = p->next;
        }
        out[out_len] = '\0';
        *outlen = out_len;
        return out;

    }

    return NULL;
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
		gboolean needfree = FALSE;
		gchar *out = NULL;


		if ( archive ) {
            out = inf(msg->response_body->data, msg->response_body->length, &length);
            if ( !out )
                return NULL;
		}

		if ( !out ) {
			out = (gchar*)msg->response_body->data;
			length = msg->response_body->length;
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
					if ( needfree ) g_free(out);
                    return cache;
				}
			}
			do_client_update_cache(client, key, parser, NULL, out, length);
		}
        if ( needfree ) g_free(out);
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
	if ( !value && priv->conn ) {
        gchar *sql, *error = NULL;
        gint res;
#ifdef DEBUG
        g_print("read from local db \"%s\"\n", key);
#endif
        sql = g_strdup_printf("SELECT key,time,data FROM cache WHERE key = '%s'", key);
        res = sqlite3_exec(priv->conn, sql, do_client_read_cache_callback, client, &error);
        if ( res != SQLITE_OK ) {
            g_error("Ошибка выполнения запроса %s\n", error ? error : "");
            priv->conn = NULL;
            sqlite3_free(error);
        }
        g_free(sql);
        value = g_hash_table_lookup(priv->hash, key);
    }
#ifdef DEBUG
    if ( !value )
        g_print("Not found in cache \"%s\"\n", key);
    else
        g_print("In cache \"%s\"\n", key);
#endif // DEBUG
	return value;
}

static JsonNode *do_client_request_valist_(DoClient *client, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache, GFunc callback, gpointer data, va_list args)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (client);
	JsonNode *res = NULL;
	GDateTime *cache_time = NULL;

	if ( priv->cached && !nocache && FALSE) { // fix me
		DoValue *value;
		value = do_client_get_cache_value(client, key);
		if ( value ) {
            res = do_value_get_node(value);
			cache_time = value->time;
			if ( check_valid_cache(key, cache_time) ) {
				if ( callback ) {
					callback(res, data);
					return NULL;
				}
				else
                    return res;
			}
		}
	}

	gchar *url;
    url = g_strdup_printf("%s/%s?store=%s", priv->url, func, priv->store);
	if ( archive ) {
        gchar *buf;
        buf = g_strdup_printf("%s&zip=1", url);
        g_free(url);
        url = buf;
    }
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
void do_client_set_cache(DoClient *client, const gchar *key, JsonNode *node, gchar *text, gint length)
{
	do_client_update_cache(client, key, NULL, node, text, length);
}
JsonNode  *do_client_get_cache(DoClient *client, const gchar *key)
{
	DoValue *value;
	value = do_client_get_cache_value(client, key);
	if ( value )
		return do_value_get_node(value);
	else
		return NULL;
}
static void cleaning_cache(gchar *key, DoValue *value, GSList **list)
{
	GDateTime *now = g_date_time_new_now_local();
    if ( g_date_time_difference(now, value->time) > CACHE_LIVETIME_SECONDS * MICRO ) {
        *list = g_slist_append(*list, key);
    }
	g_date_time_unref(now);
}

static gboolean do_client_cleaning(DoClient *client)
{
	DoClientPrivate *priv = DO_CLIENT_GET_PRIVATE (client);
    GSList *clean = NULL, *l;
#ifdef DEBUG
    g_print("Cleaning caches\n");
#endif // DEBUG
	g_hash_table_foreach(priv->hash, (GHFunc)cleaning_cache, &clean);
	for ( l = clean; l; l = l->next ) {
        gchar *key;
        key = l->data;
#ifdef DEBUG
        g_print("Cleaning cache \"%s\"\n", key);
#endif // DEBUG
        g_hash_table_remove(priv->hash, key);
	}
	return TRUE;
}
