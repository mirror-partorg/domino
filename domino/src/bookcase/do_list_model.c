
#include "do_list_model.h"
#include "do_application.h"

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
static gboolean do_list_model_get_last_value(DoListModel *model);
static gboolean do_list_model_fill(DoListModel *model);

typedef struct _DoListModelRecord DoListModelRecord;
typedef struct _DoListModelPrivate  DoListModelPrivate;

// Load

static void do_list_model_index_rebuild(DoListModel *model);

struct _DoListModelRecord
{
    gchar    *key;
    gchar    *sort;
    gchar    *code;
    gchar    *fields;
    gint      index;
};

struct _DoListModelPrivate
{
    //GHashTable *records;
    DoListModelRecord **records;
    guint               n_records;
    gchar              *name;
    gchar              *fields;
    gint                stamp;
};

enum
{
	PROP_0,
	PROP_NAME,
	PROP_FIELDS,
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
            if ( !priv->fields )
                priv->fields = g_value_dup_string(value);
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

    G_OBJECT_CLASS (do_list_model_parent_class)->finalize (object);
}

static gint do_list_model_get_n_columns(GtkTreeModel *tree_model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);
    g_return_val_if_fail (DO_IS_LIST_MODEL(tree_model), 0);

    return strlen(priv->fields) + 3;
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
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
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
    DoListModel   *do_list_model;
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);
    gchar            *str;
    time_t            now;
    int indx;
    //char *buf;

    g_return_if_fail (DO_IS_LIST_MODEL (tree_model));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (column < DO_LIST_MODEL_N_COLUMNS);

    g_value_init (value, do_list_model_get_column_type(tree_model, column));

    do_list_model = DO_LIST_MODEL(tree_model);

    record = (DoProductRecord*) iter->user_data;
    if ( !record ) {
        now = time(NULL);

    }


    g_return_if_fail ( record != NULL );

    now = time(NULL);
    if ( g_hash_table_size(priv->invalidated_rows.h) ) {
        gint pr;
        if (g_hash_table_lookup(priv->invalidated_rows.h, record->code) != NULL)
            g_hash_table_remove(priv->invalidated_rows.h, record->code);

        if (priv->invalidated_rows.size) {
            pr = ((priv->invalidated_rows.size - g_hash_table_size(priv->invalidated_rows.h))
                / (priv->invalidated_rows.size*1.))*100;
            if (priv->progress < pr)
                set_load_progress(do_list_model, pr);
        }
        if (g_hash_table_size(priv->invalidated_rows.h) == 0 ||
            (record->update_time && now - record->update_time > WAIT_LOAD_TIME)
            ) {
            g_object_notify (G_OBJECT (tree_model), "load-status");
            set_load_progress(do_list_model, 100);
            g_hash_table_remove_all(priv->invalidated_rows.h);
            set_load_status(DO_LIST_MODEL(tree_model), FALSE);
        }
    }
    //else
    if (now - record->update_time > UPDATE_TIME) {
        if (do_list_model_update_record(DO_LIST_MODEL(tree_model), record, iter))
            record->update_time = now;
        g_signal_emit (tree_model, signals[REFRESH_RECORD_SYGNAL], 0, iter);

    }

    switch(column)    {
        case DO_LIST_MODEL_COL_RECORD:
            g_value_set_pointer(value, record);
            break;
        case DO_LIST_MODEL_COL_CODE:
            g_value_set_string(value, record->code);
            break;
        case DO_LIST_MODEL_COL_NAME:
            //do_log(LOG_INFO, "get name '%s' '%s'",record->code,record->name);
            if ( priv->filter_inn ) {
                inn_rec_t rec;
                rec.code = record->code;
                if ( do_sort_list_find(priv->filter_inns, &indx, &rec) ) {
                    char *buf;
                    buf = g_strdup_printf("%s\n%s", record->name, ((inn_rec_t*)priv->filter_inns->list[indx])->name);
                    g_value_set_string(value, buf);
                }
                else
                    g_value_set_string(value, record->name);
            }
            else
                g_value_set_string(value, record->name);
            break;
        case DO_LIST_MODEL_COL_COEF:
            g_value_set_double(value, record->coef);
            break;
        case DO_LIST_MODEL_COL_MARKED:
            g_value_set_int(value, record->marked);
            break;
        case DO_LIST_MODEL_COL_ADS:
            g_value_set_string(value, record->ads);
            break;
        case DO_LIST_MODEL_COL_PACK: {
            gchar pack[100];
            int coef = record->coef;
            if (coef > 1) {
                sprintf(pack, "1*%d", coef);
                g_value_set_string(value, pack);
            }
            break;
        }
        case DO_LIST_MODEL_COL_PRICE:
            str = do_list_model_get_price(DO_LIST_MODEL(tree_model), record, TRUE);
            g_value_set_string(value, str);
            g_free(str);
            break;
        case DO_LIST_MODEL_COL_PRICE_MIN:
            if ( record->coef > 1 ) {
                str = do_list_model_get_price(DO_LIST_MODEL(tree_model), record, FALSE);
                g_value_set_string(value, str);
                g_free(str);
            }
            break;
        case DO_LIST_MODEL_COL_STOCK:
            str = do_list_model_get_stock(DO_LIST_MODEL(tree_model), record, TRUE);
            g_value_set_string(value, str);
            g_free(str);
            break;
        case DO_LIST_MODEL_COL_MANUFACTOR:
            str = do_list_model_get_manufactor(DO_LIST_MODEL(tree_model), record);
            g_value_set_string(value, str);
            break;
        case DO_LIST_MODEL_COL_STOCK_MIN:
            if ( record->coef > 1 ) {
                str = do_list_model_get_stock(DO_LIST_MODEL(tree_model), record, FALSE);
                g_value_set_string(value, str);
                g_free(str);
            }
            break;
    }
}

static gboolean do_list_model_iter_next(GtkTreeModel  *tree_model, GtkTreeIter   *iter)
{
    DoProductRecord  *record, *nextrecord;
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);
    gint indx;

    g_return_val_if_fail (DO_IS_LIST_MODEL (tree_model), FALSE);

    if (iter == NULL || iter->user_data == NULL)
        return FALSE;

    //do_log(LOG_INFO, "1");
    record = (DoProductRecord *) iter->user_data;

    if ( priv->filter_codes ) {

        if ( !do_string_list_find(priv->filter_codes, &indx, record->code) ) {
            for ( ; indx < priv->filter_codes->count; indx++ ) {
                if ( strcmp(priv->filter_codes->list[indx], record->code) < 0 ) {
                    indx--;
                    break;
                }
            }
            //do_log(LOG_INFO, "record \"%s\"not found in filters", record->code);
            //return FALSE;
        }

        if ((indx + 1) >= priv->filter_codes->count) {
            do_log(LOG_INFO, "index out off filters", record->code);
            return FALSE;
        }

        const char *code;
        code = priv->filter_codes->list[indx+1];
        if ( !do_list_model_get_index_for_code(DO_LIST_MODEL(tree_model), &indx ,code) )
            return FALSE;
        nextrecord = priv->products->list[indx];
    }
    else  {
        if (!do_list_model_get_index_for_code(DO_LIST_MODEL(tree_model), &indx, record->code))
            return FALSE;
        if ((indx + 1) >= priv->products->count)
            return FALSE;
        nextrecord = priv->products->list[indx + 1];
    }

    //do_log(LOG_INFO, "2");
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

    if ( priv->filter_codes ) {

        if ( priv->filter_codes->count == 0 )
            return FALSE;

        const char *code;
        int indx;
        code = priv->filter_codes->list[0];
        if ( !do_list_model_get_index_for_code(DO_LIST_MODEL(tree_model), &indx ,code) )
            return FALSE;
        iter->user_data = priv->products->list[indx];
    }
    else {
        if (priv->products->count == 0) {
            return FALSE;
        }
        iter->user_data = priv->products->list[0];
    }

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

    if ( !iter ) {
        return priv->filter_codes ? priv->filter_codes->count : priv->products->count;
    }
    return 0;
}

static gboolean do_list_model_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n)
{
    DoProductRecord  *record;
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (DO_IS_LIST_MODEL (tree_model), FALSE);

    if(parent)
        return FALSE;

    if ( priv->filter_codes ) {
        if( n >= priv->filter_codes->count )
            return FALSE;
        const char *code;
        int indx;
        code = priv->filter_codes->list[n];
        if ( !do_list_model_get_index_for_code(DO_LIST_MODEL(tree_model), &indx ,code) )
            return FALSE;
        record = priv->products->list[indx];
    }
    else {
        if( n >= priv->products->count )
            return FALSE;
        record = priv->products->list[n];
    }

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

GtkTreeModel *do_list_model_new(const gchar *alias_name, do_string_list_t *units, gboolean start_thread, gboolean realtime, gboolean multiprice)
{
    GtkTreeModel *model;

    model = (GtkTreeModel*)g_object_new (DO_LIST_MODEL_TYPE,
                                    "alias-name", alias_name,
                                    "units", units,
                                    //"start-in-thread", start_thread,
                                    "realtime", realtime,
                                    "multi-price", multiprice,
                                    NULL);

    g_assert( model != NULL );

    return model;
}
static void
set_load_status (DoListModel *model, gboolean status)
{
  DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
  guint is_loading;

  is_loading = status != FALSE;

  if (is_loading != priv->is_loading) {
    priv->is_loading = is_loading;
    g_object_notify (G_OBJECT (model), "load-status");
  }
}

static void
set_load_progress (DoListModel *model, gint progress)
{
  DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
  if (progress != priv->progress) {
    priv->progress = progress;
    g_object_notify (G_OBJECT (model), "load-progress");
  }
  if (progress == 100) {
    priv->is_loading = FALSE;
    g_object_notify (G_OBJECT (model), "load-status");
  }
}
static gboolean do_list_model_load_prices_stocks(DoListModel *do_list_model, DoProductRecord *record)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);
    int indx;
    gboolean retval = TRUE;
    if ( !strcmp(record->code,"33426") ) {
        indx=1;
    }
    if ( record->stock_price_list ) {
        if ( record->stock_price_list->count < 0 ||  record->stock_price_list->count > 100 ) {
            do_log(LOG_ERR, "dead stock_price_list");
        }
        do_list_clear(record->stock_price_list);
    }
    else
        record->stock_price_list = do_list_new(TRUE);

    for ( indx = 0 ; indx < priv->units_rec->count; indx++ ) {
        sklad_rec_t *sklad;
        sklad=priv->units_rec->list[indx];
        retval = do_get_rest_and_price(priv->alias, record->code, sklad, record->stock_price_list, FALSE);
        if ( !retval )
            break;

    }
    return retval;
}

static gboolean do_list_model_load_products(DoListModel *do_list_model)
{
    do_alias_t *alias;
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);
    alias = priv->alias;

    do_ret_list_t       *retval;
    product_view_key0_t  product_view_key0;
    do_extended_t       *extended;
    int i;

    do_text_set_empty(product_view_key0.code);
    do_text_set_empty(product_view_key0.sort);
    do_text_set_empty(product_view_key0.sklad);
    product_view_key0.type = 1;

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_PRODUCT_VIEW);
    do_extended_set_key(extended, 0, &product_view_key0);
    do_extended_set_filter(extended, FALSE, 1, sizeof(product_view_key0.type));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(product_view_struct_t, type),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &product_view_key0.type);
    do_extended_set_fields(extended, 2,
                           do_size_and_offset(product_view_struct_t, code),
                           do_size_and_offset(product_view_struct_t, sort)
                           );
    retval = do_extended_get(extended, -1);
    do_extended_free(extended);


    for (i = 0; i < do_ret_list_count(retval); i++) {
        DoProductRecord *record;
        struct PACK_STRUCT {
            BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
            BTI_CHAR sort[40];
        } *rec;

        rec = do_ret_list_item_data(retval, i);
        record = (DoProductRecord*)g_malloc0(sizeof(DoProductRecord));
        record->code = do_text(alias, rec->code);
        record->name = do_text(alias, rec->sort);
        do_sort_list_add(priv->products, record);
    }
    do_ret_list_free(retval);

    do_list_model_index_rebuild(do_list_model);

    if (priv->multi_price) {
        priv->invalidated_rows.size = priv->products->count;
        for (i = 0; i < priv->products->count; i++)
            g_hash_table_insert(priv->invalidated_rows.h,
            ((DoProductRecord*)priv->products->list[i])->code, GUINT_TO_POINTER(1));
    }
    return TRUE;
}
gboolean do_list_model_find_record_by_name(DoListModel *do_list_model, const char *text, gint *indx)
{
    DoProductRecord record;
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);
    record.name = (char*)text;
    if ( priv->filter_codes ) {
        for ( *indx = 0; *indx < priv->filter_codes->count; (*indx)++ ) {
            int n;
            if ( !do_list_model_get_index_for_code(do_list_model, &n, priv->filter_codes->list[*indx]) )
                return FALSE;
            if ( !strncmp(((DoProductRecord*)priv->products->list[n])->name, text, strlen(text)) )
                return TRUE;
        }
        return TRUE;
    }
    else
        return do_sort_list_findn(priv->products, indx, &record);
}
gboolean do_list_model_find_record_by_code(DoListModel *do_list_model, const char *text, gint *indx)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);
    int i, len = strlen(text);
    if ( priv->filter_codes ) {
        for ( i = 0; i < priv->filter_codes->count; i++ )
            if( !strncmp(priv->filter_codes->list[i], text, len) ) {
                *indx = i;
                return TRUE;
            }
    }
    else {
        for (i =0; i < priv->products->count; i++)
            if ( !strncmp(((DoProductRecord*)(priv->products->list[i]))->code, text, len) ) {
                *indx = i;
                return TRUE;
            }
    }
    return FALSE;
}
const gchar *do_list_model_find_name_by_code(DoListModel *do_list_model, const char *code)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);
    int i, len = strlen(code);
    if ( priv->filter_codes ) {
        for ( i = 0; i < priv->filter_codes->count; i++ )
            if( !strncmp(priv->filter_codes->list[i], code, len) ) {
                return ((DoProductRecord*)priv->products->list[i])->name;
            }
    }
    else {
        for (i =0; i < priv->products->count; i++)
            if ( !strncmp(((DoProductRecord*)(priv->products->list[i]))->code, code, len) ) {
                return ((DoProductRecord*)(priv->products->list[i]))->name;
            }
    }
    return NULL;
}
gboolean do_list_model_find_record_by_bcode(DoListModel *do_list_model, const char *text, gint *indx)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);
    barcode_key0_t barcode_key0;
    barcode_rec_t  barcode;
    do_text_set(priv->alias, barcode_key0.barcode, text);
    gboolean res = FALSE;
    if ( do_barcode_get0(priv->alias, &barcode, &barcode_key0, DO_GET_EQUAL) == DO_OK ) {
        char *code = do_text(priv->alias, barcode.data.code);
        res = do_list_model_get_index_for_code(do_list_model, indx, code);
        do_free (code);
    }
    return res;
}
GtkTreePath *do_list_model_get_path_from_code(DoListModel *model, const gchar *code)
{
    DoListModelPrivate *priv  = DO_LIST_MODEL_GET_PRIVATE(model);
    gint indx;
    DoProductIndex i;
    GtkTreePath *path = NULL;
    i.code = (char*)code;
    if (do_sort_list_find(priv->index, &indx, &i)) {
        DoProductIndex *index = priv->index->list[indx];
        path = gtk_tree_path_new();
        gtk_tree_path_append_index(path, index->indx);
    }
    return path;

}

GtkTreePath *do_list_model_get_path_from_context(DoListModel *model, DoContext *context)
{
    DoListModelPrivate *priv  = DO_LIST_MODEL_GET_PRIVATE(model);
    if (do_context_get_kind(context) != DO_CONTEXT_KIND_PRODUCT)
        return NULL;

    gint indx;
    DoProductIndex i;
    GtkTreePath *path = NULL;
    g_object_get(context, "product-code", &i.code, NULL);
    if (do_sort_list_find(priv->index, &indx, &i)) {
        DoProductIndex *index = priv->index->list[indx];
        path = gtk_tree_path_new();
        gtk_tree_path_append_index(path, index->indx);
    }
    g_free(i.code);
    return path;
}
static void do_list_model_index_rebuild(DoListModel *model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
    gint i, indx;
    DoProductIndex index, *n_index;
    DoProductRecord *record;

    for (i = 0; i < priv->index->count; i++)
        ((DoProductIndex*)priv->index->list[i])->indx = -1;

    for (i = 0; i < priv->products->count; i++) {
         record = priv->products->list[i];
         record->indx = i;

         index.code = record->code;
         if (!do_sort_list_find(priv->index, &indx, &index)) {
             n_index = g_malloc(sizeof(DoProductIndex));
             n_index->code = g_strdup(record->code);
             do_sort_list_add(priv->index, n_index);
         }
         else
            n_index = priv->index->list[indx];
         n_index->indx = i;
    }
    for (i = 0; i < priv->index->count; )
        if (((DoProductIndex*)priv->index->list[i])->indx == -1) {
            do_list_model_index_free(((DoProductIndex*)priv->index->list[i]), NULL);
            do_sort_list_delete(priv->index, i);
        }
        else
            i++;
}
static gboolean do_list_model_record_insert(DoListModel *model, const gchar *code, const gchar *name, double coef)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
    gint indx;
    GtkTreePath    *path;
    GtkTreeIter     iter;
    DoProductRecord record;
    DoProductIndex  index;

    memset(&record, 0, sizeof(record));
    record.code = g_strdup(code);
    record.name = g_strdup(name);
    record.coef = coef;
    record.marked = DOMINO_MARKED_UNMARK;
    record.update_time = 0;
    record.manufactor_region = NULL;
    record.manufactor_code = 0;

    //do_log(LOG_INFO, "insert record '%s' '%s'", code, name);

    indx = do_sort_list_add_alloc(priv->products, &record, sizeof(record));
    index.code = g_strdup(record.code);
    index.indx = indx;
    do_sort_list_add_alloc(priv->index, &index, sizeof(index));
    do_list_model_index_rebuild(model);

    iter.stamp      = priv->stamp;
    iter.user_data  = &record;
    iter.user_data2 = NULL;
    iter.user_data3 = NULL;
    if ( !priv->filter_codes ) {
        path = gtk_tree_path_new();
        gtk_tree_path_append_index(path, indx);
        gtk_tree_model_row_inserted(GTK_TREE_MODEL(model), path, &iter);
        gtk_tree_path_free(path);
    }

    return TRUE;
}
static gboolean do_list_model_record_delete(DoListModel *tree_model, const gchar *code)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(tree_model);
    DoProductRecord *rec;
    GtkTreePath *path;
    int indx;

    //do_log(LOG_INFO, "delete record '%s'", code);
    if (do_list_model_get_index_for_code(tree_model, &indx, code)) {
        int n = 0;
        if ( priv->filter_codes ) {
            if ( !do_string_list_find(priv->filter_codes, &n , code) )
                n = 0;
        }
        else
            n = indx;
        if ( n ) {
            path = gtk_tree_path_new();
            gtk_tree_path_append_index(path, n);
            gtk_tree_model_row_deleted(GTK_TREE_MODEL(tree_model), path);
            gtk_tree_path_free(path);
        }


        rec = priv->products->list[indx];
        if (g_hash_table_lookup(priv->invalidated_rows.h, rec->code))
            g_hash_table_remove(priv->invalidated_rows.h, rec->code);
        do_list_model_record_free(rec, NULL);
        do_sort_list_delete(priv->products, indx);

        do_list_model_index_rebuild(tree_model);
    }
    return TRUE;
}
static gboolean do_list_model_record_reorder(DoListModel *model, const gchar *new_name, gint old_indx)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
    DoProductRecord *record;
    GtkTreePath *path;
    GtkTreeIter iter;
    gint new_indx;
    gint start, end, i;



    g_return_val_if_fail (old_indx < priv->products->count, FALSE);

    record = priv->products->list[old_indx];
    //do_log(LOG_INFO, "reorder record '%s' '%s'", record->name ? record->name : "", new_name );
    if (record->name)
        g_free(record->name);
    record->name = g_strdup(new_name);
    record->update_time = 0;
    do_sort_list_delete(priv->products, old_indx);
    new_indx = do_sort_list_add(priv->products, record);

    if (old_indx == new_indx)
        return TRUE;

    do_list_model_index_rebuild(model);

    start = min(old_indx, new_indx);
    end = max(old_indx, new_indx);
    path = gtk_tree_path_new();
    gtk_tree_path_append_index(path, start);
    priv->invalidated_rows.size = end - start + 1;
    g_hash_table_remove_all(priv->invalidated_rows.h);
    set_load_status(model, TRUE);
    priv->progress = 0;

    for (i = start;
         i <=end && gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path);
         i++, gtk_tree_path_next(path)) {

        g_hash_table_insert(priv->invalidated_rows.h, ((DoProductRecord*)iter.user_data)->code, GUINT_TO_POINTER(1));
        if ( !priv->filter_codes )
            gtk_tree_model_row_changed(GTK_TREE_MODEL(model), path, &iter);
    }
    //gtk_tree_model_rows_reordered(info->model, info->path, NULL, &info->new_order);
    gtk_tree_path_free(path);

    return TRUE;
}

static gboolean do_list_model_record_check(DoListModel *model, do_alias_t *alias, const gchar *code)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
    DoProductRecord *record = NULL;
    gchar *name = NULL;
    gdouble coef;
    product_rec_t product;
    product_key4_t product_key4;
    int status;
    gint indx;
    gboolean retval;

    do_text_set(alias, product_key4.code, code);
    status = do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL);

    if ( status == DO_ERROR ) {
        do_log(LOG_ERR, "Ошибка чтения товаров");
        return FALSE;
    }
    retval = TRUE;
    //do_log(LOG_INFO, "record check '%s'", code);
    if ( do_list_model_get_index_for_code(model, &indx, code) )
        record = priv->products->list[indx];

    if ( status == DO_OK ) {
        if ( record == NULL ) {
            name = do_text(alias, product.data.name);
            coef = do_product_coef(alias, &product);
            retval = do_list_model_record_insert(model, code, name, coef);
        }
        else {
            name = do_text(alias, product.data.name);
            if ( !strcmp(name, record->name) )
                record->update_time = 0;
            else
                retval = do_list_model_record_reorder(model, name, indx);
        }
    }
    else {
        if ( record != NULL )
            retval = do_list_model_record_delete(model, record->code);
    }
    if ( name )
        g_free(name);
    return retval;
}

static gboolean do_list_model_get_last_value(DoListModel *model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
    do_alias_t *alias;
    int status;
    product_data_key0_t product_data_key0;
    alias = priv->alias;

    do_text_set(alias, product_data_key0.code, "00001");
    product_data_key0.type = 77;
    product_data_key0.number = INT_MAX;
    status = do_product_data_key0(alias, &product_data_key0, DO_GET_LE);
    switch ( status ) {
        case DO_ERROR:
            do_log(LOG_ERR, "Ошибка чтения данных товара");
            return FALSE;
        case DO_OK:
            break;
        default:
            return TRUE;
    }
    priv->last_value = product_data_key0.number;
    return TRUE;
}
static gboolean do_list_model_read_protocol(DoListModel *model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
    do_alias_t *alias;
    int status;
    gboolean retval;
    product_data_key0_t product_data_key0, key;
    product_data_rec_t product_data;
    alias = domino_alias_new(do_alias_get_name(priv->alias));
    if ( !alias || ! do_alias_open(alias, TRUE)) {
        if (alias)
            do_alias_free(alias);
        return FALSE;
    }

    do_text_set(alias, product_data_key0.code, "00001");
    product_data_key0.type = 77;
    if ( !priv->last_value )
        do_list_model_get_last_value(model);

    product_data_key0.number = priv->last_value;
    do_cpy(key.code, product_data_key0.code);
    key.type = product_data_key0.type;
    retval = TRUE;

    status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_GT);
    while ( status == DO_OK ) {
        if ( do_cmp(key.code, product_data_key0.code) ||
              key.type != product_data_key0.type ) break;
        priv->last_value = product_data_key0.number;

        if ( do_product_data_param_int(alias, &product_data, "1") == 1) {
            gchar *code;
            code = do_product_data_param(alias, &product_data, "2");
            code[5] = '\0';

            g_free(code);
        }
        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        do_log(LOG_ERR, "Ошибка чтения данных товара");
        retval=FALSE;
    }
    do_alias_close(alias);
    do_alias_free(alias);

    return retval;
}
void do_list_model_set_marked(DoListModel *model, const char *product_code, DoMarked marked)
{
	DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);
    DoProductRecord  *record;

    //do_log(LOG_INFO ,"record mark '%s' %d", product_code, marked);
    int i;
    for (i =0; i < priv->products->count; i++) {
        record = priv->products->list[i];
        if ( !strcmp(record->code, product_code) ) {
            if ( marked != (int)record->marked ) {
                GtkTreePath *path;
                GtkTreeIter iter;
                record->marked = marked;
                path = gtk_tree_path_new();
                gtk_tree_path_append_index(path, record->indx);
                gtk_tree_model_row_changed(GTK_TREE_MODEL(model), path, &iter);
                gtk_tree_path_free(path);
            }
        }
    }
}
gboolean do_list_model_set_filter(DoListModel *do_list_model, do_string_list_t *lexs, do_string_list_t *nums, const gchar* inn)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);

    //do_log(LOG_INFO, "filter %d '%s' %d", lexs ? lexs->count : 0, inn ? inn : "(empty)", priv->filter_codes ? priv->filter_codes->count : priv->products->count);
    gboolean res = FALSE, empty1=!inn || inn[0]=='\0',empty2=!priv->filter_inn || priv->filter_inn[0]=='\0';
    int i;

    if ( lexs && lexs->count ) {
        if ( priv->filter_lexs->count != lexs->count )
            res = TRUE;
        else
            for ( i = 0; i < lexs->count; i++ )
                if ( strcmp(lexs->list[i], priv->filter_lexs->list[i]) ) {
                    res = TRUE;
                    break;
                }
        if ( res )  {
            do_string_list_free(priv->filter_lexs);
            priv->filter_lexs = do_string_list_dup(lexs);
        }
    }
    else {
        if ( priv->filter_lexs->count ) {
            res = TRUE;
            do_string_list_clear(priv->filter_lexs);
        }
    }
    if ( empty1 != empty2 || g_strcmp0(inn, priv->filter_inn) ) {
        res = TRUE;
        if (priv->filter_inn) g_free(priv->filter_inn);
        if ( inn && inn[0] != '\0')
            priv->filter_inn = strdup(inn);
        else
            priv->filter_inn = NULL;
    }
    /*
    if ( nums && nums->count ) {
        if ( priv->filter_nums->count != nums->count )
            res = TRUE;
        else
            for ( i = 0; i < nums->count; i++ )
                if ( strcmp(nums->list[i], priv->filter_nums->list[i]) ) {
                    res = TRUE;
                    break;
                }
        if ( res )  {
            do_string_list_free(priv->filter_nums);
            priv->filter_nums = do_string_list_dup(nums);
        }
    }
    else {
        if ( priv->filter_nums->count ) {
            res = TRUE;
            do_string_list_clear(priv->filter_nums);
        }
    }*/
    if ( res )
        do_list_model_load_filter_products(do_list_model);
    return res;
}
/*
gboolean do_list_model_set_filter_exact(DoListModel *do_list_model, int type, const char *text)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);
    do_string_list_clear(priv->filter_lexs);
    do_list_model_load_filter_products(do_list_model, type, text);
    return TRUE;
}
*/
static int cmp_func(char *code1, char *code2, DoListModel *do_list_model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);
    int indx1=0, indx2=0;
    if ( !do_list_model_get_index_for_code(do_list_model, &indx1, code1) )
        return 0;
    if ( !do_list_model_get_index_for_code(do_list_model, &indx2, code2) )
        return 0;
    return strcmp(((DoProductRecord*)priv->products->list[indx1])->name,((DoProductRecord*)priv->products->list[indx2])->name);

}

static gboolean do_list_model_load_filter_products(DoListModel *do_list_model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);

    GtkTreePath *path;
    GtkTreeIter     iter;
    int indx;
    iter.stamp      = priv->stamp;
    iter.user_data2 = NULL;
    iter.user_data3 = NULL;
    do_list_foreach(priv->filter_inns,inn_free_record, NULL);
    do_sort_list_clear(priv->filter_inns);
    if ( !priv->filter_lexs->count && !priv->filter_inn ) {
        if ( priv->filter_codes ) {
            for ( indx = priv->filter_codes->count-1; indx >=0; indx-- ) {
                path = gtk_tree_path_new();
                gtk_tree_path_append_index(path, indx);
                gtk_tree_model_row_deleted(GTK_TREE_MODEL(do_list_model), path);
                gtk_tree_path_free(path);
            }
            for ( indx = 0; indx < priv->products->count; indx++ ) {
                path = gtk_tree_path_new();
                gtk_tree_path_append_index(path, indx);
                iter.user_data  = priv->products->list[indx];
                gtk_tree_model_row_inserted(GTK_TREE_MODEL(do_list_model), path, &iter);
                gtk_tree_path_free(path);
            }
            do_string_list_free(priv->filter_codes);
            priv->filter_codes = NULL;
        }
        return TRUE;
    }

    do_alias_t *alias;
    alias = priv->alias;
    product_view_key0_t  product_view_key0, key;
    barcode_key0_t barcode_key0;
    barcode_rec_t barcode;
    int len, status, n;
    char *code;
    int old_count = priv->filter_codes ? priv->filter_codes->count : priv->products->count;

    for ( indx = old_count-1; indx >=0; indx-- ) {
        path = gtk_tree_path_new();
        gtk_tree_path_append_index(path, indx);
        gtk_tree_model_row_deleted(GTK_TREE_MODEL(do_list_model), path);
        gtk_tree_path_free(path);
    }
    if ( priv->filter_codes )
        do_string_list_clear(priv->filter_codes);
    else
        priv->filter_codes = do_string_list_new();

    do_string_list_t *codes;
    for ( indx = 0; indx < priv->filter_lexs->count; indx++ ) {
        codes = do_string_list_new();
        product_view_key0.type = 4;

        do_text_set_empty(product_view_key0.sklad);
        do_text_set_empty(product_view_key0.code);
        do_text_set(alias, product_view_key0.sort, priv->filter_lexs->list[indx]);
        do_cpy(key.sort, product_view_key0.sort);
        for ( len = sizeof(product_view_key0.sort) - 1; len >= 0; len-- )
            if ( product_view_key0.sort[len] != ' ' ) break;
        len++;
        if ( !len ) continue;
        status = do_product_view_key0(alias, &product_view_key0, DO_GET_GE);
        while ( status == DO_OK ) {
            if ( strncmp(key.sort, product_view_key0.sort, len) || product_view_key0.type != 4) break;

            code = do_text(alias, product_view_key0.code);
            if ( !indx || do_string_list_find(priv->filter_codes, &n, code) ) {
                if ( !do_string_list_find(codes, &n, code) )
                    do_string_list_add_alloc(codes, code);
            }
            do_free(code);
            status = do_product_view_key0(alias, &product_view_key0, DO_GET_NEXT);
        }
        do_text_set(alias, barcode_key0.barcode, priv->filter_lexs->list[indx]);
        status = do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL);
        if ( status == DO_OK ) {
            code = do_text(alias, barcode.data.code);
            if ( !indx || do_string_list_find(priv->filter_codes, &n, code) ) {
                if ( !do_string_list_find(codes, &n, code) )
                    do_string_list_add_alloc(codes, code);
            }
            do_free(code);
        }
        do_string_list_free(priv->filter_codes);
        priv->filter_codes=codes;
    }

    if ( priv->filter_inn ) { // МНН
        product_view_key0.type = 2;
        do_text_set_empty(product_view_key0.sklad);
        do_text_set_empty(product_view_key0.code);
        do_text_set(alias, product_view_key0.sort, priv->filter_inn);
        do_cpy(key.sort, product_view_key0.sort);
        for ( len = sizeof(product_view_key0.sort) - 1; len >= 0; len-- )
            if ( product_view_key0.sort[len] != ' ' ) break;
        len++;
        if ( len ) {
            status = do_product_view_key0(alias, &product_view_key0, DO_GET_GE);
            while ( status == DO_OK ) {
                if ( strncmp(key.sort, product_view_key0.sort, len) || product_view_key0.type != 2) break;
                inn_rec_t rec;
                code = do_text(alias, product_view_key0.code);
                rec.code = do_text(alias, product_view_key0.code);
                rec.name = do_text(alias, product_view_key0.sort);

                do_sort_list_add_alloc(priv->filter_inns, &rec, sizeof(rec));

                if ( !do_string_list_find(priv->filter_codes, &n, code) ) {
                    do_string_list_add_alloc(priv->filter_codes, code);
                }
                do_free(code);
                status = do_product_view_key0(alias, &product_view_key0, DO_GET_NEXT);
            }
        }
    }
    do_sort_list_t *sort;
    sort = do_sort_list_new(FALSE, FALSE, (do_list_cmp_func)cmp_func, do_list_model);
    for ( indx = 0; indx < priv->filter_codes->count; indx++ )
        do_sort_list_add(sort, priv->filter_codes->list[indx]);
    codes = do_string_list_new();
    for ( indx = 0; indx < priv->filter_codes->count; indx++ )
        do_string_list_add_alloc(codes,sort->list[indx]);
    do_string_list_free(priv->filter_codes);
    do_sort_list_free(sort);
    priv->filter_codes = codes;
    iter.stamp = g_random_int();
    for ( indx = 0; indx < priv->filter_codes->count; indx++ ) {
        code = priv->filter_codes->list[indx];
        path = gtk_tree_path_new();
        gtk_tree_path_append_index(path, indx);
        if ( !do_list_model_get_index_for_code(do_list_model,&n, code) )
            return FALSE;
        iter.user_data = priv->products->list[n];
        gtk_tree_model_row_inserted(GTK_TREE_MODEL(do_list_model), path, &iter);
        gtk_tree_path_free(path);
    }
    return TRUE;
}
gboolean     do_list_model_is_filtered(DoListModel *do_list_model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);
    return priv->filter_codes != NULL;
}
gboolean    do_list_model_is_filtered_and_empty(DoListModel *do_list_model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(do_list_model);
    return priv->filter_codes != NULL && priv->filter_codes->count == 0;
}
static gboolean do_list_model_fill(DoListModel *model)
{
    DoListModelPrivate *priv = DO_LIST_MODEL_GET_PRIVATE(model);

    node = do_application_request_async(DO_APPLICATION(app), "GET", "GetList", priv->name, archived, FALSE,
    		                    (GFunc)model_fill, view,
								"name", priv->name,
								// ""
								 NULL);
    if ( node ) {
        do_list_view_set_load_status(view, "Обновление данных");
    	do_list_view_model_fill(view, node);
    }
    else {
        do_list_view_set_load_status(view, "Запрос данных");
    }
}
