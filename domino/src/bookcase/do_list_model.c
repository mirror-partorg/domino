
#include "do_list_model.h"
#include "do_client.h"

// boring declarations of local functions
static void do_list_model_init(DoListModel *pkg_tree);
static void do_list_model_class_init(DoListModelClass *klass);
static void do_list_model_tree_model_init(GtkTreeModelIface *iface);
static void do_list_model_finalize(GObject *object);

static gint do_list_model_get_n_columns(GtkTreeModel *tree_model);
static GType do_list_model_get_column_type(GtkTreeModel *tree_model, gint index);
static gboolean do_list_model_get_iter(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path);
static GtkTreePath *do_list_model_get_path(GtkTreeModel *tree_model, GtkTreeIter *iter);
static void do_list_model_get_value(GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value);
static gboolean do_list_model_iter_next(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean do_list_model_iter_children(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent);
static gboolean do_list_model_iter_has_child(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gint do_list_model_iter_n_children(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean do_list_model_iter_nth_child(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n);
static gboolean do_list_model_iter_parent(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child);
//static gboolean do_list_model_get_last_value(DoListModel *model);
static gboolean do_list_model_fill(DoListModel *model);

static void do_list_model_record_update(JsonNode *node, DoListModel *model);
static void do_list_model_fill_keys(JsonNode *node, DoListModel *model);
typedef struct _DoListModelRecord DoListModelRecord;
typedef struct _DoListModelPrivate  DoListModelPrivate;

// Load

//static void do_list_model_index_rebuild(DoListModel *model);


struct _DoListModelRecord
{
    gchar    *key;
    gchar    *sort;
    gchar    *code;
    gint      index;
};

struct _DoListModelPrivate
{
    //GHashTable *records;
    DoClient           *client;
    DoListModelRecord **records;
    guint               n_records;
    gchar              *name;
    gchar              *fields;
    gint                stamp;
    guint               n_columns;
};

enum
{
	PROP_0,
	PROP_NAME,
	PROP_FIELDS,
	PROP_CLIENT,
};

G_DEFINE_TYPE_WITH_CODE (DoListModel, do_list_model, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL,
						do_list_model_tree_model_init)
             G_ADD_PRIVATE(DoListModel)
						);

#define DO_LIST_MODEL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_LIST_MODEL_TYPE,  DoListModelPrivate))


static void do_list_model_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_NAME:
            if ( !priv->name )
                priv->name = g_value_dup_string(value);
			break;
		case PROP_FIELDS:
            if ( !priv->fields ) {
                priv->fields = g_value_dup_string(value);
                priv->n_columns = strlen(priv->fields) + DO_LIST_MODEL_N_KEYS;
            }
			break;
		case PROP_CLIENT:
            if ( !priv->client ) {
                priv->client = g_value_get_pointer(value);
                g_object_ref(priv->client);
            }
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_list_model_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_NAME:
            g_value_set_string(value, priv->name);
            break;
        case PROP_FIELDS:
            g_value_set_string(value, priv->fields);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}
static GObject *do_list_model_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	//DoListModelPrivate *priv;
	GObject            *object;
	DoListModel        *model;

	object = G_OBJECT_CLASS (do_list_model_parent_class)->constructor(type, n_construct_properties, construct_params);
	model = (DoListModel*)object;

    //priv = DO_LIST_MODEL_GET_PRIVATE(object);

    g_return_val_if_fail (do_list_model_fill(model), object);

	return object;

}
static void do_list_model_class_init(DoListModelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_list_model_constructor;
	object_class->finalize     = do_list_model_finalize;
	object_class->get_property = do_list_model_get_property;
	object_class->set_property = do_list_model_set_property;

    g_object_class_install_property (object_class,
				   PROP_NAME,
				   g_param_spec_string ("name",
							"Имя",
							"Имя",
							NULL,
							G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (object_class,
				   PROP_FIELDS,
				   g_param_spec_string ("fields",
							"Имя",
							"Имя",
							NULL,
							G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (object_class,
				   PROP_CLIENT,
				   g_param_spec_pointer("client",
							"Клиент запросов",
							"Клиент запросов",
							G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void do_list_model_tree_model_init (GtkTreeModelIface *iface)
{
    iface->get_n_columns   = do_list_model_get_n_columns;
    iface->get_column_type = do_list_model_get_column_type;
    iface->get_iter        = do_list_model_get_iter;
    iface->get_path        = do_list_model_get_path;
    iface->get_value       = do_list_model_get_value;
    iface->iter_next       = do_list_model_iter_next;
    iface->iter_children   = do_list_model_iter_children;
    iface->iter_has_child  = do_list_model_iter_has_child;
    iface->iter_n_children = do_list_model_iter_n_children;
    iface->iter_nth_child  = do_list_model_iter_nth_child;
    iface->iter_parent     = do_list_model_iter_parent;
}

static void do_list_model_init (DoListModel *do_list_model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);
    priv->stamp = g_random_int(); // Random int to check whether iters belong to out model

}
static void do_list_model_record_free(DoListModelRecord *record)
{
    g_free(record->code);
    g_free(record->sort);
    g_free(record->key);
    g_free(record);
}

static void do_list_model_finalize(GObject *object)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(object);

    /*if ( priv->load_timer != 0 ) {
        g_source_remove (priv->load_timer);
        priv->load_timer = 0;
        if ( priv->load_path )
            gtk_tree_path_free( priv->load_path );
    }*/
    gint i;
    for ( i = 0; i < priv->n_records; i++ )
        do_list_model_record_free(priv->records[i]);
    g_free(priv->records);
    g_free(priv->name);
    g_free(priv->fields);
    g_object_unref(priv->client);

    G_OBJECT_CLASS (do_list_model_parent_class)->finalize (object);
}

static gint do_list_model_get_n_columns(GtkTreeModel *tree_model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);
    g_return_val_if_fail (DO_IS_LIST_MODEL(tree_model), 0);

    return priv->n_columns;
}

static GType do_list_model_get_column_type(GtkTreeModel *tree_model, gint index)
{
    g_return_val_if_fail (DO_IS_LIST_MODEL(tree_model), G_TYPE_INVALID);
    return G_TYPE_STRING;
}

static gboolean do_list_model_get_iter (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);
    DoListModelRecord *record;
    gint *indices, n, depth;


    g_assert(DO_IS_LIST_MODEL(tree_model));
    g_assert(path != NULL);

    indices = gtk_tree_path_get_indices(path);
    depth   = gtk_tree_path_get_depth(path);

    g_assert(depth == 1); // depth 1 = top level; a model only has top level nodes and no children

    n = indices[0]; // the n-th top level row

    if ( n >=  priv->n_records || n < 0 )
        return FALSE;

    record = priv->records[n];

    g_assert(record != NULL);

    iter->stamp      = priv->stamp;
    iter->user_data  = record;
    iter->user_data2 = NULL;
    iter->user_data3 = NULL;

    return TRUE;
}

static GtkTreePath *do_list_model_get_path_by_record(DoListModel *model, DoListModelRecord *record)
{
    //DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
    GtkTreePath  *path = NULL;

    path = gtk_tree_path_new();
    gtk_tree_path_append_index(path, record->index);

    return path;
}

static GtkTreePath *do_list_model_get_path(GtkTreeModel *tree_model, GtkTreeIter  *iter)
{
    GtkTreePath  *path;
    DoListModelRecord *record;
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (DO_IS_LIST_MODEL(tree_model), NULL);
    g_return_val_if_fail (iter != NULL,               NULL);
    g_return_val_if_fail (iter->user_data != NULL,    NULL);

    if ( iter->stamp != priv->stamp ) {
        g_error("iter is invalid\n");
        return NULL;
    }

    g_return_val_if_fail (iter->stamp == priv->stamp, NULL);

    record = (DoListModelRecord*) iter->user_data;
    path =  do_list_model_get_path_by_record(DO_LIST_MODEL(tree_model), record);

    return path;
}

static void do_list_model_get_value(GtkTreeModel *tree_model, GtkTreeIter  *iter, gint column, GValue *value)
{
    DoListModelRecord  *record;
    //DoListModel   *do_list_model;
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);

    g_return_if_fail (DO_IS_LIST_MODEL (tree_model));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (column < priv->n_columns);

    g_value_init (value, do_list_model_get_column_type(tree_model, column));

    //do_list_model = DO_LIST_MODEL(tree_model);

    record = (DoListModelRecord*) iter->user_data;

    g_return_if_fail ( record != NULL );

    if ( column < DO_LIST_MODEL_N_KEYS ) {
        switch (column) {
            case 0:
                g_value_set_string(value, record->key);
                break;
            case 1:
                g_value_set_string(value, record->sort);
                break;
            default:
                g_value_set_string(value, record->code);
        }
        return;
    }

    JsonNode *node;
    JsonObject *obj;
    JsonArray *values;
    gchar *cache_key;

    cache_key = g_strdup_printf("RECORD.%s.%s", priv->name, record->key);


    node = do_client_request_get_async(priv->client, "GetRecord", cache_key, (GFunc)do_list_model_record_update, tree_model,
                                      "key", record->key, "fields", priv->fields, NULL);
    g_free(cache_key);
    if ( !node ) {
        g_value_set_string(value, "");
        return;
    }

    obj = json_node_get_object(node);
    values = json_object_get_array_member(obj, "values");

    if ( json_array_get_length(values) > column - DO_LIST_MODEL_N_KEYS ) {
        g_value_set_string(value,json_array_get_string_element(values, column - DO_LIST_MODEL_N_KEYS));
    }
    else
        g_value_set_string(value, "");

}

static gboolean do_list_model_iter_next(GtkTreeModel  *tree_model, GtkTreeIter   *iter)
{
    DoListModelRecord  *record, *nextrecord;
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (DO_IS_LIST_MODEL (tree_model), FALSE);

    if (iter == NULL || iter->user_data == NULL)
        return FALSE;

    record = (DoListModelRecord *) iter->user_data;

    if ( priv->n_records <= record->index + 1 )
        return FALSE;

    nextrecord = priv->records[record->index + 1];

    iter->stamp     = priv->stamp;
    iter->user_data = nextrecord;

    return TRUE;
}

static gboolean do_list_model_iter_children(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (parent == NULL || parent->user_data != NULL, FALSE);

    if (parent)
        return FALSE;

    g_return_val_if_fail (DO_IS_LIST_MODEL (tree_model), FALSE);

    if (priv->n_records == 0)
            return FALSE;

    iter->user_data = priv->records[0];
    iter->stamp     = priv->stamp;

    return TRUE;
}

static gboolean do_list_model_iter_has_child (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    return FALSE;
}

static gint do_list_model_iter_n_children (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (DO_IS_LIST_MODEL (tree_model), -1);
    g_return_val_if_fail (iter == NULL || iter->user_data != NULL, FALSE);

    if ( !iter )
        return priv->n_records;
    return 0;
}

static gboolean do_list_model_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n)
{
    DoListModelRecord  *record;
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (DO_IS_LIST_MODEL (tree_model), FALSE);

    if(parent)
        return FALSE;

    if( n >= priv->n_records )
        return FALSE;

    record = priv->records[n];

    if (record == NULL) {
        return FALSE;
    }

    iter->stamp = priv->stamp;
    iter->user_data = record;


    return TRUE;
}

static gboolean do_list_model_iter_parent (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child)
{
    return FALSE;
}

GtkTreeModel *do_list_model_new(const gchar *name, const gchar *fields, GObject *client)
{
    GtkTreeModel *model;

    model = (GtkTreeModel*)g_object_new (DO_LIST_MODEL_TYPE,
                                    "name", name,
                                    "fields", fields,
                                    "client", client,
                                    NULL);

    g_assert( model != NULL );

    return model;
}
static gboolean do_list_model_fill(DoListModel *model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
    JsonNode *node;

    node = do_client_request_async(priv->client, "GET", "GetList", priv->name, TRUE, FALSE,
    		                    (GFunc) do_list_model_fill_keys, model,
								"name", priv->name,
								// ""
								 NULL);
    if ( node ) {
        do_list_model_fill_keys(node, model);
    }
    else
        return FALSE;
    return TRUE;
}
static void do_list_model_record_update(JsonNode *node, DoListModel *model)
{
    // to do
}
static void do_list_model_fill_keys(JsonNode *node, DoListModel *model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
    if ( priv->n_records ) { // to do
        return;
    }

	JsonObject *obj;
	JsonArray *array;
	if ( !node )
		return;
	obj = json_node_get_object(node);
	if ( obj ) {
        gint i;
		array = json_object_get_array_member(obj, "items");
		priv->n_records = json_array_get_length(array);
		priv->records = (DoListModelRecord**) g_new0(gpointer, priv->n_records);
		for ( i = 0; i < priv->n_records; i++ ) {
            DoListModelRecord *record;
            JsonObject *obj;
            record = g_new0(DoListModelRecord, 1);
            priv->records[i] = record;
            record->index = i;
            obj = json_array_get_object_element(array, i);
            g_assert(obj != NULL);
            if ( json_object_has_member(obj,"key") )
                record->key = g_strdup((gchar*)json_object_get_string_member(obj, "key"));
            if ( json_object_has_member(obj,"sort") )
                record->sort = g_strdup((gchar*)json_object_get_string_member(obj, "sort"));
            if ( json_object_has_member(obj,"code") )
                record->code = g_strdup((gchar*)json_object_get_string_member(obj, "code"));
		}
	}
}