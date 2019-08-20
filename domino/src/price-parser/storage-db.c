#include "storage-db.h"
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <libpq-fe.h>

#define LOG_DOMAIN "storage-db"

static void storage_db_get_property  (GObject              *object,
					     guint                 param_id,
					     GValue               *value,
					     GParamSpec           *pspec);
static void storage_db_set_property  (GObject              *object,
					     guint                 param_id,
					     const GValue         *value,
					     GParamSpec           *pspec);

static void storage_db_finalize (GObject          *object);

#define STORAGE_DB_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), STORAGE_TYPE_DB, StorageDbPrivate))


typedef struct _StorageDbPrivate StorageDbPrivate;

#define STORAGE_TYPE_DB_LEXEM_TYPE storage_db_lexem_get_type()


struct _StorageDbPrivate
{
    gchar *connection_str;
    PGconn *conn;
    gboolean get_error_replace_prepared;
};


enum {
    PROP_0,
    PROP_CONNECTION_INFO,

};

G_DEFINE_TYPE (StorageDb, storage_db, G_TYPE_OBJECT)
static void storage_db_close(StorageDb *storage);


static void storage_db_init (StorageDb *db)
{
 //   StorageDbPrivate *priv = STORAGE_DB_GET_PRIVATE (db);
	//memset(priv, 0, sizeof(StorageDbPrivate));
}

static GObject *storage_db_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
    GObject *object;
    StorageDbPrivate *priv;
    StorageDb *db;

    object = G_OBJECT_CLASS (storage_db_parent_class)->constructor(type, n_construct_properties, construct_params);
    db = STORAGE_DB(object);
    priv = STORAGE_DB_GET_PRIVATE(db);

	return object;
}

static void storage_db_class_init (StorageDbClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->constructor  = storage_db_constructor;
    object_class->get_property = storage_db_get_property;
    object_class->set_property = storage_db_set_property;
    object_class->finalize     = storage_db_finalize;

// signal
// property

    g_object_class_install_property (object_class,
				   PROP_CONNECTION_INFO,
				   g_param_spec_string ("connection-info",
							"Connection info string",
							"Connection info string",
							NULL,
							G_PARAM_READWRITE | G_PARAM_CONSTRUCT));


    g_type_class_add_private (object_class, sizeof (StorageDbPrivate));
}

static void storage_db_get_property (GObject     *object,
				guint        param_id,
				GValue      *value,
				GParamSpec  *pspec)
{
    StorageDbPrivate *priv = STORAGE_DB_GET_PRIVATE (object);

    switch (param_id) {
        case PROP_CONNECTION_INFO:
            g_value_set_string (value, priv->connection_str);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
            break;
    }
}

static void
storage_db_set_property (GObject      *object,
				guint         param_id,
				const GValue *value,
				GParamSpec   *pspec)
{
    StorageDbPrivate *priv = STORAGE_DB_GET_PRIVATE (object);

    switch ( param_id ) {
        case PROP_CONNECTION_INFO:
            g_free(priv->connection_str);
            priv->connection_str = g_value_dup_string(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
            break;
    }
}
static void storage_db_finalize (GObject          *object)
{
    StorageDbPrivate *priv = STORAGE_DB_GET_PRIVATE (object);
    g_free(priv->connection_str);
    storage_db_close(STORAGE_DB(object));

    G_OBJECT_CLASS (storage_db_parent_class)->finalize (object);
}
StorageDb *storage_db_new(const gchar *connection_str)
{
    return g_object_new (STORAGE_TYPE_DB, "connection-info", connection_str, NULL);
}
static void storage_db_close(StorageDb *storage)
{
    StorageDbPrivate *priv = STORAGE_DB_GET_PRIVATE (storage);
    if ( priv->conn )
        PQfinish(priv->conn);
}
static void pg_log(PGconn *conn, const char *msg)
{
    g_log(LOG_DOMAIN, G_LOG_LEVEL_ERROR, "%s.%s", msg, PQerrorMessage(conn));
}

gboolean storage_db_connect(StorageDb *storage)
{
    StorageDbPrivate *priv = STORAGE_DB_GET_PRIVATE (storage);

    priv->conn = PQconnectdb(priv->connection_str);
    if ( PQstatus(priv->conn) != CONNECTION_OK ) {
        pg_log(priv->conn, "Connect");
        return FALSE;
    }
    return TRUE;
}
static gboolean storage_db_get_error_replace_prepare(StorageDb *storage)
{
    StorageDbPrivate *priv = STORAGE_DB_GET_PRIVATE (storage);
    PGresult *res;
    //Oid pg_prepare_params[1];

    //pg_prepare_params[0] = TIMESTAMPOID;
    res = PQprepare(priv->conn, "get_error_replace",
                    "SELECT replace FROM errors WHERE name = $1",
                    1, NULL);//pg_prepare_params);
    if ( PQresultStatus(res) != PGRES_COMMAND_OK ) {
        pg_log(priv->conn, "Prepare ");
        priv->get_error_replace_prepared = FALSE;
        return FALSE;

    }
    PQclear(res);
    priv->get_error_replace_prepared = TRUE;
    return TRUE;
}
gchar *storage_db_get_error_replace(StorageDb *storage, const gchar *name)
{
    StorageDbPrivate *priv = STORAGE_DB_GET_PRIVATE (storage);
    PGresult *res;

    if ( !priv->get_error_replace_prepared )
        if ( !storage_db_get_error_replace_prepare(storage) )
            return NULL;

    const char *values[1];
    int         lengths[1];
    int         formats[1];

    values[0] = (char *)name;
    lengths[0] = strlen(name);
    formats[0] = 1;

    res = PQexecPrepared(priv->conn, "get_error_replace", 1, values, lengths, formats, 1);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        pg_log(priv->conn, "Exec");
        return NULL;
    }
    gchar *retval = NULL;
    int nrows;
    nrows = PQntuples(res);
    if ( nrows > 0 )
        retval = g_strdup(PQgetvalue(res, 0, 0));

    PQclear(res);

    return retval;


}

