
#include "do_product_model.h"
#include <domino.h>
#include "domino.h"
#include "do_utilx.h"
#include <dolib.h>
#include "../misc/send_error.h"
#include "do_marshal.h"
#include <string.h>
#include <domino_action.h>
#include <limits.h>


#define HANDBOOK_DB  DO_DB_SKLAD, DO_DB_PRODUCT, DO_DB_BARCODE, DO_DB_STOCK, DO_DB_PROTOCOL, DO_DB_DOCUMENT_ORDER, DO_DB_PRODUCT_VIEW, DO_DB_PRODUCT_DATA, DO_DB_PARTNER, DO_DB_SKLAD, DO_DB_END

#define DO_PRODUCT_MODEL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_PRODUCT_MODEL_TYPE, DoProductModelPrivate))

#define UPDATE_TIME 10
#define WAIT_LOAD_TIME 60*6
#define MAX_LOAD_IN_THREAD 100

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

// boring declarations of local functions
static void do_product_model_init(DoProductModel *pkg_tree);
static void do_product_model_class_init(DoProductModelClass *klass);
static void do_product_model_tree_model_init(GtkTreeModelIface *iface);
static void do_product_model_finalize(GObject *object);
static GtkTreeModelFlags do_product_model_get_flags(GtkTreeModel *tree_model);
static gint do_product_model_get_n_columns(GtkTreeModel *tree_model);
static GType do_product_model_get_column_type(GtkTreeModel *tree_model, gint index);
static gboolean do_product_model_get_iter(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path);
static GtkTreePath *do_product_model_get_path(GtkTreeModel *tree_model, GtkTreeIter *iter);
static void do_product_model_get_value(GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value);
static gboolean do_product_model_iter_next(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean do_product_model_iter_children(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent);
static gboolean do_product_model_iter_has_child(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gint do_product_model_iter_n_children(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean do_product_model_iter_nth_child(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n);
static gboolean do_product_model_iter_parent(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child);
static gboolean do_product_model_get_last_value(DoProductModel *model);

/*!!static void do_product_model_add_price(DoProductModel *model, do_sort_list_t *list, const char *base_code, const char *code,
                                       double price, double price_full, double stock, int unit_indx, int uniq_price);*/

typedef struct _DoProductRecord DoProductRecord;
typedef struct _DoProductPrice DoProductPrice;
typedef struct _DoProductIndex DoProductIndex;
typedef struct _DoProductModelPrivate  DoProductModelPrivate;

// Load

static void do_product_model_index_rebuild(DoProductModel *model);

//static gboolean do_product_model_load_prices_stocks(DoProductModel *do_product_model, do_sort_list_t *prices, const char *basecode);
static gboolean do_product_model_load_prices_stocks(DoProductModel *do_product_model, DoProductRecord *record);
static gboolean do_product_model_load_products(DoProductModel *do_product_model);
static gboolean do_product_model_load_filter_products(DoProductModel *do_product_model);
static void set_load_status (DoProductModel *model, gboolean status);
static void set_load_progress (DoProductModel *model, gint progress);

static gboolean do_product_model_get_index_for_code(DoProductModel *model, gint *indx, const gchar *code) G_GNUC_WARN_UNUSED_RESULT;

// Real Time update
static gboolean do_product_model_read_protocol(DoProductModel *model);

typedef struct {
    gchar *region;
    gint code;
    gchar *name;
} manufactor_rec_t;


typedef struct {
    gchar *code;
    gchar *name;
} inn_rec_t;


struct _DoProductRecord
{
    gchar    *code;
    gchar    *name;
    gdouble   coef;
    gchar    *manufactor_region;
    gint      manufactor_code;
//    guint     list;
    domino_marked_t marked;
    guint     indx;
    gchar    *ads;
    do_list_t *stock_price_list;

    time_t    update_time;
};
struct _DoProductIndex
{
    gchar    *code;
    guint     indx;
};

struct _DoProductModelPrivate
{
    do_alias_t     *alias;

    do_sort_list_t    *products;
    do_sort_list_t    *index;
    do_sort_list_t    *manufactors;


    guint           load_timer;
    GtkTreePath    *load_path;
    guint           realtime_timer;
    gint            last_value;

    gint            stamp;
    gchar          *alias_name;
    gboolean        multi_price;
    gboolean        realtime;
    gboolean        is_loading : 1;
    gint            progress;

    gchar            *error;
    do_string_list_t *units_code;
    do_list_t        *units_rec;
    do_sort_list_t   *list_list;

    gint            nprice;

    struct {
        GHashTable     *h;
        gint            size;
    } invalidated_rows;
    //protocol_key0_t zero_point;
    do_string_list_t *filter_codes;
    do_string_list_t *filter_lexs;
    do_string_list_t *filter_nums;
    gchar            *filter_inn;
    do_sort_list_t   *filter_inns;

};

enum
{
	PROP_0,
	PROP_ALIAS,
	PROP_UNITS,
	PROP_LOAD_STATUS,
	PROP_LOAD_PROGRESS,
	PROP_REALTIME,
	PROP_MULTI_PRICE,
	PROP_ERROR,
};

enum
{
    REFRESH_RECORD_SYGNAL,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

static GObjectClass *parent_class = NULL;  /* GObject stuff - nothing to worry about */

G_DEFINE_TYPE_WITH_CODE (DoProductModel, do_product_model, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL,
						do_product_model_tree_model_init));
enum {
    DO_PRODUCT_MODEL_ERROR_LOAD = 0,
    DO_PRODUCT_MODEL_ERROR_UPDATE,
    DO_PRODUCT_MODEL_ERROR_NO,
};

static void do_product_model_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string(value);
			break;
		case PROP_UNITS: {
            priv->units_code = g_value_get_pointer(value);
            if ( priv->units_rec )
                do_list_clear(priv->units_rec);
            else
                priv->units_rec = do_list_new(TRUE);
            int indx;
            if ( !priv->alias ) {
                priv->alias = domino_alias_new(priv->alias_name);
                if ( priv->alias )
                    g_return_if_fail (do_alias_open(priv->alias, 0, HANDBOOK_DB));
            };
            if ( !priv->alias )
                break;
            sklad_key0_t sklad_key0;
            sklad_rec_t sklad;
            for ( indx = 0; indx < priv->units_code->count; indx++ ) {
                do_text_set(priv->alias, sklad_key0.code, priv->units_code->list[indx]);
                if ( do_sklad_get0(priv->alias, &sklad, &sklad_key0, DO_GET_EQUAL) == DO_OK ) {
                    do_list_add_alloc(priv->units_rec, &sklad, sizeof(sklad));
                }
            }
			break;
        }
        case PROP_REALTIME:
            priv->realtime = g_value_get_boolean (value);
            break;
        case PROP_MULTI_PRICE:
            priv->multi_price = g_value_get_boolean (value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_product_model_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_ALIAS:
            g_value_set_string(value, priv->alias_name);
            break;
        case PROP_UNITS:
            g_value_set_pointer(value, priv->units_code);
            break;
        case PROP_MULTI_PRICE:
            g_value_set_boolean (value, priv->multi_price);
            break;
        case PROP_REALTIME:
            g_value_set_boolean (value, priv->realtime);
            break;
        case PROP_LOAD_STATUS:
            g_value_set_boolean(value, priv->is_loading || g_hash_table_size(priv->invalidated_rows.h) > 0);
            break;
        case PROP_LOAD_PROGRESS:
            g_value_set_int(value, priv->progress);
            break;
        case PROP_ERROR:
            g_value_set_string(value, priv->error);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}
static GObject *do_product_model_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	DoProductModelPrivate *priv;
	GObject               *object;
	DoProductModel        *model;

	object = G_OBJECT_CLASS (do_product_model_parent_class)->constructor(type, n_construct_properties, construct_params);
	model = (DoProductModel*)object;

    priv = DO_PRODUCT_MODEL_GET_PRIVATE(object);

    priv->invalidated_rows.h = g_hash_table_new(NULL, NULL);

    priv->list_list = do_property_list_create("Список");
    if ( ! priv->alias ) {
        priv->alias = domino_alias_new(priv->alias_name);
        g_return_val_if_fail (priv->alias, object);
        g_return_val_if_fail (do_alias_open(priv->alias, 0, HANDBOOK_DB), object);
    }

    set_load_progress(DO_PRODUCT_MODEL(object), 0);

    g_return_val_if_fail (do_product_model_load_products(model), object);
    priv->nprice = DO_CONST_PRICE_RETAIL;

    if ( priv->realtime ) {
        do_product_model_get_last_value(DO_PRODUCT_MODEL(object));

        priv->realtime_timer =
            g_timeout_add_full(G_PRIORITY_LOW, 5000,
                                       (GSourceFunc) do_product_model_read_protocol, object, NULL);
    }

    set_load_progress(DO_PRODUCT_MODEL(object), 100);
	return object;

}
static void do_product_model_class_init(DoProductModelClass *klass)
{
    GObjectClass *object_class;

    parent_class = (GObjectClass*) g_type_class_peek_parent (klass);
    object_class = (GObjectClass*) klass;

	object_class->constructor  = do_product_model_constructor;
    object_class->finalize = do_product_model_finalize;

	object_class->get_property = do_product_model_get_property;
	object_class->set_property = do_product_model_set_property;

	g_type_class_add_private (object_class, sizeof (DoProductModelPrivate));

    g_object_class_install_property (object_class,
				   PROP_ALIAS,
				   g_param_spec_string ("alias-name",
							"Алиас БД",
							"Алиас БД",
							NULL,
							G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (object_class,
				   PROP_UNITS,
				   g_param_spec_pointer("units",
							"Подразделения",
							"Подразделения цен и остатков",
							G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (object_class,
				   PROP_LOAD_STATUS,
				   g_param_spec_boolean("load-status",
							"Статус загрузки",
							"Справочник загружается",
							TRUE,
							G_PARAM_READABLE));

    g_object_class_install_property (object_class,
				   PROP_LOAD_PROGRESS,
				   g_param_spec_int("load-progress",
							"Статус загрузки",
							"Справочник загружается",
							0,100,0,
							G_PARAM_READABLE));


    g_object_class_install_property (object_class,
				   PROP_MULTI_PRICE,
				   g_param_spec_boolean("multi-price",
							"Отображать разные цены",
							"Отображать разные цены",
							FALSE,
							G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (object_class,
				   PROP_ERROR,
				   g_param_spec_string("error",
							"Описание ошибки",
							"Описание ошибки",
							NULL,
							G_PARAM_READABLE));


    g_object_class_install_property (object_class,
				   PROP_REALTIME,
				   g_param_spec_boolean("realtime",
							"Обновление данных во время просмотра",
							"Обновление данных во время просмотра",
							FALSE,
							G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    signals[REFRESH_RECORD_SYGNAL] =
        g_signal_new ("refresh-record",
                G_OBJECT_CLASS_TYPE (object_class),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (DoProductModelClass, refresh_record),
                NULL, NULL,
                 g_cclosure_marshal_VOID__BOXED,
                 G_TYPE_NONE,
                 1,
                 GTK_TYPE_TREE_ITER);
}

static void do_product_model_tree_model_init (GtkTreeModelIface *iface)
{
    iface->get_flags       = do_product_model_get_flags;
    iface->get_n_columns   = do_product_model_get_n_columns;
    iface->get_column_type = do_product_model_get_column_type;
    iface->get_iter        = do_product_model_get_iter;
    iface->get_path        = do_product_model_get_path;
    iface->get_value       = do_product_model_get_value;
    iface->iter_next       = do_product_model_iter_next;
    iface->iter_children   = do_product_model_iter_children;
    iface->iter_has_child  = do_product_model_iter_has_child;
    iface->iter_n_children = do_product_model_iter_n_children;
    iface->iter_nth_child  = do_product_model_iter_nth_child;
    iface->iter_parent     = do_product_model_iter_parent;
}

static int inn_cmp_record(void *rec, void *rec1, void *user_data)
{
    return strcmp(((inn_rec_t*)rec)->code, ((inn_rec_t*)rec1)->code);
}
static int inn_free_record(void *rec, void *data)
{
    do_free(((inn_rec_t*)rec)->code);
    do_free(((inn_rec_t*)rec)->name);
    return !data || *((gboolean*)data) != TRUE;
}


static int cmp_record(void *rec, void *rec1, void *user_data)
{
    int i = strcmp(((DoProductRecord*)rec)->name, ((DoProductRecord*)rec1)->name);
    if (i)
        return i;
    return strcmp(((DoProductRecord*)rec)->code, ((DoProductRecord*)rec1)->code);
}
static int cmpn_record(void *rec, void *rec1, void *user_data)
{
    char *name = ((DoProductRecord*)rec)->name;
    char *name1 = ((DoProductRecord*)rec1)->name;
    int len = strlen(name), len1 = strlen(name1);
    int result = strncmp(name, name1, (len > len1) ? len1 : len);
    return ((len < len1) && !result) ? -1 : result;
}
static int cmpi_record(void *rec, void *rec1, void *user_data)
{
    return strcmp(((DoProductIndex*)rec)->code, ((DoProductIndex*)rec1)->code);
}
static int cmp_manufactor(void *rec, void *rec1, void *user_data)
{
    int res;
    res = strcmp(((manufactor_rec_t*)rec)->region, ((manufactor_rec_t*)rec1)->region);
    if ( !res )
        res = ((manufactor_rec_t*)rec)->code - ((manufactor_rec_t*)rec1)->code;
    return res;
}

static void do_product_model_init (DoProductModel *do_product_model)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);
    priv->is_loading = TRUE;
    priv->products = do_sort_list_new(0, 1, cmp_record, NULL);
    priv->index = do_sort_list_new(0, 1, cmpi_record, NULL);
    priv->manufactors = do_sort_list_new(0, 1, cmp_manufactor, NULL);
    //!!priv->prices = do_sort_list_new(0, 1, cmpp_record, NULL);
    do_sort_list_set_cmpn_proc(priv->products, cmpn_record);
    //!!do_sort_list_set_cmpn_proc(priv->prices, cmppn_record);
    priv->stamp = g_random_int(); // Random int to check whether iters belong to out model
    priv->filter_lexs = do_string_list_new();
    priv->filter_nums = do_string_list_new();
    priv->filter_inns = do_sort_list_new(TRUE,TRUE,inn_cmp_record,NULL);

}
static int do_product_model_record_free(DoProductRecord *record, void *data)
{
    //do_log(LOG_INFO, "free record '%s'",record->code ? record->code : "");
    if (record->code)
        do_free(record->code);
    if (record->name)
        do_free(record->name);
    if (record->manufactor_region)
        do_free(record->manufactor_region);
    if (record->ads)
        do_free(record->ads);
    if ( record->stock_price_list )
        do_list_free(record->stock_price_list);
    record->stock_price_list=NULL;
    do_free(record);
    return !data || *((gboolean*)data) != TRUE;
}
static int do_product_model_index_free(DoProductIndex *index, void *data)
{
    if (index->code)
        do_free(index->code);
    do_free(index);
    return !data || *((gboolean*)data) != TRUE;
}
static int do_product_model_manufactor_free(manufactor_rec_t *r, void *data)
{
    if (r->region)
        do_free(r->region);
    if (r->name)
        do_free(r->name);
    do_free(r);
    return !data || *((gboolean*)data) != TRUE;
}

static void do_product_model_finalize(GObject *object)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(object);

    do_property_list_free(priv->list_list);
    g_hash_table_destroy(priv->invalidated_rows.h);
    if ( priv->load_timer != 0 ) {
        g_source_remove (priv->load_timer);
        priv->load_timer = 0;
        if ( priv->load_path )
            gtk_tree_path_free( priv->load_path );
    }
    if ( priv->realtime_timer != 0 ) {
        g_source_remove (priv->realtime_timer);
        priv->realtime_timer = 0;
    }

    do_alias_close(priv->alias);
    do_alias_free(priv->alias);
	if (priv->alias_name)
		g_free(priv->alias_name);
    if ( priv->filter_lexs ) do_string_list_free(priv->filter_lexs);
    if ( priv->filter_nums ) do_string_list_free(priv->filter_nums);
    if ( priv->filter_inns ) {
        do_list_foreach(priv->filter_inns, inn_free_record, NULL);
        do_sort_list_free(priv->filter_inns);
    }
    if ( priv->filter_codes ) do_string_list_free(priv->filter_codes);
    if ( priv->filter_inn ) g_free(priv->filter_inn);

    do_list_foreach((do_list_t*)priv->products, (do_list_foreach_func)do_product_model_record_free, NULL);
    do_sort_list_free(priv->products);
    do_list_foreach((do_list_t*)priv->index, (do_list_foreach_func)do_product_model_index_free, NULL);
    do_sort_list_free(priv->index);
    do_list_foreach((do_list_t*)priv->manufactors, (do_list_foreach_func)do_product_model_manufactor_free, NULL);
    do_sort_list_free(priv->manufactors);
    if ( priv->units_rec )
        do_list_free(priv->units_rec);
    if ( priv->units_code )
        do_list_free(priv->units_code);


    (* parent_class->finalize) (object);
}

static GtkTreeModelFlags do_product_model_get_flags(GtkTreeModel *tree_model)
{
    g_return_val_if_fail (DO_IS_PRODUCT_MODEL(tree_model), (GtkTreeModelFlags)0);

    return (GTK_TREE_MODEL_LIST_ONLY | GTK_TREE_MODEL_ITERS_PERSIST);
}

static gint do_product_model_get_n_columns(GtkTreeModel *tree_model)
{
    g_return_val_if_fail (DO_IS_PRODUCT_MODEL(tree_model), 0);

    return DO_PRODUCT_MODEL_N_COLUMNS;
}

static GType do_product_model_get_column_type(GtkTreeModel *tree_model, gint index)
{
    g_return_val_if_fail (DO_IS_PRODUCT_MODEL(tree_model), G_TYPE_INVALID);
    g_return_val_if_fail (index < DO_PRODUCT_MODEL_N_COLUMNS && index >= 0, G_TYPE_INVALID);
    switch (index) {
        case DO_PRODUCT_MODEL_COL_RECORD:
            return G_TYPE_POINTER;
        case DO_PRODUCT_MODEL_COL_CODE:
            return G_TYPE_STRING;
        case DO_PRODUCT_MODEL_COL_NAME:
            return G_TYPE_STRING;
        case DO_PRODUCT_MODEL_COL_COEF:
            return G_TYPE_DOUBLE;
        case DO_PRODUCT_MODEL_COL_MARKED:
            return G_TYPE_INT;
        case DO_PRODUCT_MODEL_COL_ADS:
            return G_TYPE_STRING;
        case DO_PRODUCT_MODEL_COL_PACK:
            return G_TYPE_STRING;
        case DO_PRODUCT_MODEL_COL_PRICE:
            return G_TYPE_STRING;
        case DO_PRODUCT_MODEL_COL_PRICE_MIN:
            return G_TYPE_STRING;
        case DO_PRODUCT_MODEL_COL_STOCK:
            return G_TYPE_STRING;
        case DO_PRODUCT_MODEL_COL_MANUFACTOR:
            return G_TYPE_STRING;
        case DO_PRODUCT_MODEL_COL_STOCK_MIN:
            return G_TYPE_STRING;
        default:
            return G_TYPE_INVALID;
    }
}

static gboolean do_product_model_get_iter (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(tree_model);
    DoProductRecord  *record;
    gint          *indices, n, depth;


    g_assert(DO_IS_PRODUCT_MODEL(tree_model));
    g_assert(path != NULL);

    indices = gtk_tree_path_get_indices(path);
    depth   = gtk_tree_path_get_depth(path);

    g_assert(depth == 1); // depth 1 = top level; a model only has top level nodes and no children

    n = indices[0]; // the n-th top level row

    if ( priv->filter_codes ) {

        if ( n >= priv->filter_codes->count || n < 0 )
            return FALSE;
        const char *code;
        code = priv->filter_codes->list[n];
        if ( !do_product_model_get_index_for_code(DO_PRODUCT_MODEL(tree_model), &n ,code) )
            return FALSE;
    }

    if ( n >= priv->products->count || n < 0 )
        return FALSE;

    record = priv->products->list[n];

    g_assert(record != NULL);

    iter->stamp      = priv->stamp;
    iter->user_data  = record;
    iter->user_data2 = NULL;
    iter->user_data3 = NULL;

    return TRUE;
}

static gboolean do_product_model_get_index_for_code(DoProductModel *model, gint *indx, const gchar *code)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(model);
    DoProductIndex index;
    gboolean retval;
    int i;

    index.code = (char*)code;
    retval = do_sort_list_find(priv->index, &i, &index);
    if ( retval )
        *indx = ((DoProductIndex*)priv->index->list[i])->indx;

    return retval;
}

static GtkTreePath *do_product_model_get_path_by_record(DoProductModel *model, DoProductRecord *record)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(model);
    GtkTreePath  *path = NULL;

    path = gtk_tree_path_new();
    if ( priv->filter_codes )  {
        int indx;
        if ( do_string_list_find(priv->filter_codes, &indx, record->code) )
            gtk_tree_path_append_index(path, indx);
        else {
            gtk_tree_path_free(path);
            return NULL;
        }
    }
    else
        gtk_tree_path_append_index(path, record->indx);

    return path;
}

static GtkTreePath *do_product_model_get_path(GtkTreeModel *tree_model, GtkTreeIter  *iter)
{
    GtkTreePath  *path;
    DoProductRecord *record;
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (DO_IS_PRODUCT_MODEL(tree_model), NULL);
    g_return_val_if_fail (iter != NULL,               NULL);
    g_return_val_if_fail (iter->user_data != NULL,    NULL);

    if ( iter->stamp != priv->stamp ) {
        do_log(LOG_ERR, "iter is invalid");
        return NULL;
    }

    g_return_val_if_fail (iter->stamp == priv->stamp, NULL);

    record = (DoProductRecord*) iter->user_data;
    path =  do_product_model_get_path_by_record(DO_PRODUCT_MODEL(tree_model), record);

    return path;
}
static gboolean price_cmp(double p1, double p2)
{
    return ( p1 < p2-0.005 || p1 > p2+0.005 );
}
static gchar *do_product_model_get_price(DoProductModel *tree_model, DoProductRecord *record, gboolean full)
{


    int indx, more = FALSE;
    double stock_max = 0, price = 0;
    gchar *retval, *val;

    if ( !record->stock_price_list )
        do_product_model_load_prices_stocks(tree_model, record);
    for ( indx = 0; indx < record->stock_price_list->count; indx++ ) {
        do_rest_price_life_t *item;
        item = record->stock_price_list->list[indx];
        if ( price && price_cmp(price, item->price) )
            more = TRUE;
        if ( !price || stock_max < item->rest ) {
            stock_max = item->rest;
            price = item->price;
        }
    }

    val = do_money_format((full) ? price : price/record->coef, TRUE);
    if ( more ) {
        retval = g_strdup_printf("%s...",val);
        g_free(val);
    }
    else
        retval = val;
    return retval;

}
static gchar *do_product_model_get_stock(DoProductModel *tree_model, DoProductRecord *record, gboolean full)
{
    int indx;
    double stock = 0;
    gchar *retval;

    if ( !record->stock_price_list )
        do_product_model_load_prices_stocks(tree_model, record);
    for ( indx = 0; indx < record->stock_price_list->count; indx++ ) {
        do_rest_price_life_t *item;
        item = record->stock_price_list->list[indx];
        stock += item->rest;
    }

    retval = do_rest_format((full) ? stock : stock*record->coef);
    return retval;

}
static gchar *do_product_model_get_manufactor(DoProductModel *tree_model, DoProductRecord *record)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(tree_model);
    int indx;
    manufactor_rec_t m;
    if ( !record->manufactor_region ) return "";
    m.region = record->manufactor_region;
    m.code = record->manufactor_code;
    if ( !do_sort_list_find(priv->manufactors, &indx, &m) ) return "";
    return ((manufactor_rec_t*)priv->manufactors->list[indx])->name;
}

typedef struct {
    GtkTreeModel *model;
    GtkTreePath *path;
    gint new_order;
    gboolean *no_update_flag;
} DoProductModelRowsReordered;

static gboolean do_product_model_update_record(DoProductModel *tree_model, DoProductRecord *record, GtkTreeIter *iter)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(tree_model);
    int indx;
    gboolean retval;
    product_rec_t product;
    product_key0_t product_key0;
    manufactor_rec_t manufactor;

    //do_log(LOG_INFO, "update record '%s'",record->code?record->code:"");

    do_text_set(priv->alias, product_key0.code, record->code);
    product_key0.base_parcel = 0;
    switch (do_product_get0(priv->alias, &product, &product_key0, DO_GET_EQUAL)) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            return TRUE;
        default:
            return FALSE;
    }
    if (record->name)
        g_free(record->name);

    manufactor.region = do_text(priv->alias, product.data.mfr_region);
    manufactor.code = product.data.mfr_code;
    if ( !record->manufactor_region ||
          strcmp(record->manufactor_region, manufactor.region) ||
          record->manufactor_code != manufactor.code )  {
        if ( record->manufactor_region )
            g_free(record->manufactor_region);
        record->manufactor_region = do_strdup(manufactor.region);
        record->manufactor_code = manufactor.code;
        if ( !do_sort_list_find(priv->manufactors, &indx, &manufactor) ) {
            partner_rec_t partner;
            partner_key0_t partner_key0;
            do_text_set(priv->alias, partner_key0.g_code, record->manufactor_region);
            partner_key0.code = record->manufactor_code;
            do_partner_get0(priv->alias, &partner, &partner_key0, DO_GET_EQUAL);
            manufactor.name = do_text(priv->alias, partner.data.name);
            do_sort_list_insert_new(priv->manufactors, indx, &manufactor, sizeof(manufactor));
        } else
            do_free(manufactor.region);
    } else
        do_free(manufactor.region);

    record->name = do_text(priv->alias, product.data.name);
    record->coef = do_pdecimal(product.data.tech_unit_rate, 4, 3);
    record->ads = do_product_param(priv->alias, &product, "MR");
    //record->ads = (record->ads == 2) || (record->ads == 3);
    if (!record->coef)
         record->coef = 1;
    //do_log(LOG_INFO, "read price & stock record '%s'",record->code?record->code:"");
    retval = do_product_model_load_prices_stocks(tree_model, record);
    return retval;
}
static void do_product_model_get_value(GtkTreeModel *tree_model, GtkTreeIter  *iter, gint column, GValue *value)
{
    DoProductRecord  *record;
    DoProductModel   *do_product_model;
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(tree_model);
    gchar            *str;
    time_t            now;
    int indx;
    //char *buf;

    g_return_if_fail (DO_IS_PRODUCT_MODEL (tree_model));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (column < DO_PRODUCT_MODEL_N_COLUMNS);

    g_value_init (value, do_product_model_get_column_type(tree_model, column));

    do_product_model = DO_PRODUCT_MODEL(tree_model);

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
                set_load_progress(do_product_model, pr);
        }
        if (g_hash_table_size(priv->invalidated_rows.h) == 0 ||
            (record->update_time && now - record->update_time > WAIT_LOAD_TIME)
            ) {
            g_object_notify (G_OBJECT (tree_model), "load-status");
            set_load_progress(do_product_model, 100);
            g_hash_table_remove_all(priv->invalidated_rows.h);
            set_load_status(DO_PRODUCT_MODEL(tree_model), FALSE);
        }
    }
    //else
    if (now - record->update_time > UPDATE_TIME) {
        if (do_product_model_update_record(DO_PRODUCT_MODEL(tree_model), record, iter))
            record->update_time = now;
        g_signal_emit (tree_model, signals[REFRESH_RECORD_SYGNAL], 0, iter);

    }

    switch(column)    {
        case DO_PRODUCT_MODEL_COL_RECORD:
            g_value_set_pointer(value, record);
            break;
        case DO_PRODUCT_MODEL_COL_CODE:
            g_value_set_string(value, record->code);
            break;
        case DO_PRODUCT_MODEL_COL_NAME:
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
        case DO_PRODUCT_MODEL_COL_COEF:
            g_value_set_double(value, record->coef);
            break;
        case DO_PRODUCT_MODEL_COL_MARKED:
            g_value_set_int(value, record->marked);
            break;
        case DO_PRODUCT_MODEL_COL_ADS:
            g_value_set_string(value, record->ads);
            break;
        case DO_PRODUCT_MODEL_COL_PACK: {
            gchar pack[100];
            int coef = record->coef;
            if (coef > 1) {
                sprintf(pack, "1*%d", coef);
                g_value_set_string(value, pack);
            }
            break;
        }
        case DO_PRODUCT_MODEL_COL_PRICE:
            str = do_product_model_get_price(DO_PRODUCT_MODEL(tree_model), record, TRUE);
            g_value_set_string(value, str);
            g_free(str);
            break;
        case DO_PRODUCT_MODEL_COL_PRICE_MIN:
            if ( record->coef > 1 ) {
                str = do_product_model_get_price(DO_PRODUCT_MODEL(tree_model), record, FALSE);
                g_value_set_string(value, str);
                g_free(str);
            }
            break;
        case DO_PRODUCT_MODEL_COL_STOCK:
            str = do_product_model_get_stock(DO_PRODUCT_MODEL(tree_model), record, TRUE);
            g_value_set_string(value, str);
            g_free(str);
            break;
        case DO_PRODUCT_MODEL_COL_MANUFACTOR:
            str = do_product_model_get_manufactor(DO_PRODUCT_MODEL(tree_model), record);
            g_value_set_string(value, str);
            break;
        case DO_PRODUCT_MODEL_COL_STOCK_MIN:
            if ( record->coef > 1 ) {
                str = do_product_model_get_stock(DO_PRODUCT_MODEL(tree_model), record, FALSE);
                g_value_set_string(value, str);
                g_free(str);
            }
            break;
    }
}

static gboolean do_product_model_iter_next(GtkTreeModel  *tree_model, GtkTreeIter   *iter)
{
    DoProductRecord  *record, *nextrecord;
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(tree_model);
    gint indx;

    g_return_val_if_fail (DO_IS_PRODUCT_MODEL (tree_model), FALSE);

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
        if ( !do_product_model_get_index_for_code(DO_PRODUCT_MODEL(tree_model), &indx ,code) )
            return FALSE;
        nextrecord = priv->products->list[indx];
    }
    else  {
        if (!do_product_model_get_index_for_code(DO_PRODUCT_MODEL(tree_model), &indx, record->code))
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

static gboolean do_product_model_iter_children(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (parent == NULL || parent->user_data != NULL, FALSE);

    if (parent)
        return FALSE;

    g_return_val_if_fail (DO_IS_PRODUCT_MODEL (tree_model), FALSE);

    if ( priv->filter_codes ) {

        if ( priv->filter_codes->count == 0 )
            return FALSE;

        const char *code;
        int indx;
        code = priv->filter_codes->list[0];
        if ( !do_product_model_get_index_for_code(DO_PRODUCT_MODEL(tree_model), &indx ,code) )
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

static gboolean do_product_model_iter_has_child (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    return FALSE;
}

static gint do_product_model_iter_n_children (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (DO_IS_PRODUCT_MODEL (tree_model), -1);
    g_return_val_if_fail (iter == NULL || iter->user_data != NULL, FALSE);

    if ( !iter ) {
        return priv->filter_codes ? priv->filter_codes->count : priv->products->count;
    }
    return 0;
}

static gboolean do_product_model_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n)
{
    DoProductRecord  *record;
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (DO_IS_PRODUCT_MODEL (tree_model), FALSE);

    if(parent)
        return FALSE;

    if ( priv->filter_codes ) {
        if( n >= priv->filter_codes->count )
            return FALSE;
        const char *code;
        int indx;
        code = priv->filter_codes->list[n];
        if ( !do_product_model_get_index_for_code(DO_PRODUCT_MODEL(tree_model), &indx ,code) )
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

static gboolean do_product_model_iter_parent (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child)
{
    return FALSE;
}

GtkTreeModel *do_product_model_new(const gchar *alias_name, do_string_list_t *units, gboolean start_thread, gboolean realtime, gboolean multiprice)
{
    GtkTreeModel *model;

    model = (GtkTreeModel*)g_object_new (DO_PRODUCT_MODEL_TYPE,
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
set_load_status (DoProductModel *model, gboolean status)
{
  DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(model);
  guint is_loading;

  is_loading = status != FALSE;

  if (is_loading != priv->is_loading) {
    priv->is_loading = is_loading;
    g_object_notify (G_OBJECT (model), "load-status");
  }
}

static void
set_load_progress (DoProductModel *model, gint progress)
{
  DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(model);
  if (progress != priv->progress) {
    priv->progress = progress;
    g_object_notify (G_OBJECT (model), "load-progress");
  }
  if (progress == 100) {
    priv->is_loading = FALSE;
    g_object_notify (G_OBJECT (model), "load-status");
  }
}
static gboolean do_product_model_load_prices_stocks(DoProductModel *do_product_model, DoProductRecord *record)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);
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

static gboolean do_product_model_load_products(DoProductModel *do_product_model)
{
    do_alias_t *alias;
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);
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

    do_product_model_index_rebuild(do_product_model);

    if (priv->multi_price) {
        priv->invalidated_rows.size = priv->products->count;
        for (i = 0; i < priv->products->count; i++)
            g_hash_table_insert(priv->invalidated_rows.h,
            ((DoProductRecord*)priv->products->list[i])->code, GUINT_TO_POINTER(1));
    }
    return TRUE;
}
gboolean do_product_model_find_record_by_name(DoProductModel *do_product_model, const char *text, gint *indx)
{
    DoProductRecord record;
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);
    record.name = (char*)text;
    if ( priv->filter_codes ) {
        for ( *indx = 0; *indx < priv->filter_codes->count; (*indx)++ ) {
            int n;
            if ( !do_product_model_get_index_for_code(do_product_model, &n, priv->filter_codes->list[*indx]) )
                return FALSE;
            if ( !strncmp(((DoProductRecord*)priv->products->list[n])->name, text, strlen(text)) )
                return TRUE;
        }
        return TRUE;
    }
    else
        return do_sort_list_findn(priv->products, indx, &record);
}
gboolean do_product_model_find_record_by_code(DoProductModel *do_product_model, const char *text, gint *indx)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);
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
const gchar *do_product_model_find_name_by_code(DoProductModel *do_product_model, const char *code)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);
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
gboolean do_product_model_find_record_by_bcode(DoProductModel *do_product_model, const char *text, gint *indx)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);
    barcode_key0_t barcode_key0;
    barcode_rec_t  barcode;
    do_text_set(priv->alias, barcode_key0.barcode, text);
    gboolean res = FALSE;
    if ( do_barcode_get0(priv->alias, &barcode, &barcode_key0, DO_GET_EQUAL) == DO_OK ) {
        char *code = do_text(priv->alias, barcode.data.code);
        res = do_product_model_get_index_for_code(do_product_model, indx, code);
        do_free (code);
    }
    return res;
}
GtkTreePath *do_product_model_get_path_from_code(DoProductModel *model, const gchar *code)
{
    DoProductModelPrivate *priv  = DO_PRODUCT_MODEL_GET_PRIVATE(model);
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

GtkTreePath *do_product_model_get_path_from_context(DoProductModel *model, DoContext *context)
{
    DoProductModelPrivate *priv  = DO_PRODUCT_MODEL_GET_PRIVATE(model);
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
static void do_product_model_index_rebuild(DoProductModel *model)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(model);
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
            do_product_model_index_free(((DoProductIndex*)priv->index->list[i]), NULL);
            do_sort_list_delete(priv->index, i);
        }
        else
            i++;
}
static gboolean do_product_model_record_insert(DoProductModel *model, const gchar *code, const gchar *name, double coef)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(model);
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
    do_product_model_index_rebuild(model);

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
static gboolean do_product_model_record_delete(DoProductModel *tree_model, const gchar *code)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(tree_model);
    DoProductRecord *rec;
    GtkTreePath *path;
    int indx;

    //do_log(LOG_INFO, "delete record '%s'", code);
    if (do_product_model_get_index_for_code(tree_model, &indx, code)) {
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
        do_product_model_record_free(rec, NULL);
        do_sort_list_delete(priv->products, indx);

        do_product_model_index_rebuild(tree_model);
    }
    return TRUE;
}
static gboolean do_product_model_record_reorder(DoProductModel *model, const gchar *new_name, gint old_indx)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(model);
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

    do_product_model_index_rebuild(model);

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

static gboolean do_product_model_record_check(DoProductModel *model, do_alias_t *alias, const gchar *code)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(model);
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
    if ( do_product_model_get_index_for_code(model, &indx, code) )
        record = priv->products->list[indx];

    if ( status == DO_OK ) {
        if ( record == NULL ) {
            name = do_text(alias, product.data.name);
            coef = do_product_coef(alias, &product);
            retval = do_product_model_record_insert(model, code, name, coef);
        }
        else {
            name = do_text(alias, product.data.name);
            if ( !strcmp(name, record->name) )
                record->update_time = 0;
            else
                retval = do_product_model_record_reorder(model, name, indx);
        }
    }
    else {
        if ( record != NULL )
            retval = do_product_model_record_delete(model, record->code);
    }
    if ( name )
        g_free(name);
    return retval;
}

static gboolean do_product_model_get_last_value(DoProductModel *model)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(model);
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
static gboolean do_product_model_read_protocol(DoProductModel *model)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(model);
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
        do_product_model_get_last_value(model);

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
void do_product_model_set_marked(DoProductModel *model, const char *product_code, DoMarked marked)
{
	DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(model);
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
gboolean do_product_model_set_filter(DoProductModel *do_product_model, do_string_list_t *lexs, do_string_list_t *nums, const gchar* inn)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);

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
        do_product_model_load_filter_products(do_product_model);
    return res;
}
/*
gboolean do_product_model_set_filter_exact(DoProductModel *do_product_model, int type, const char *text)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);
    do_string_list_clear(priv->filter_lexs);
    do_product_model_load_filter_products(do_product_model, type, text);
    return TRUE;
}
*/
static int cmp_func(char *code1, char *code2, DoProductModel *do_product_model)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);
    int indx1=0, indx2=0;
    if ( !do_product_model_get_index_for_code(do_product_model, &indx1, code1) )
        return 0;
    if ( !do_product_model_get_index_for_code(do_product_model, &indx2, code2) )
        return 0;
    return strcmp(((DoProductRecord*)priv->products->list[indx1])->name,((DoProductRecord*)priv->products->list[indx2])->name);

}

static gboolean do_product_model_load_filter_products(DoProductModel *do_product_model)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);

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
                gtk_tree_model_row_deleted(GTK_TREE_MODEL(do_product_model), path);
                gtk_tree_path_free(path);
            }
            for ( indx = 0; indx < priv->products->count; indx++ ) {
                path = gtk_tree_path_new();
                gtk_tree_path_append_index(path, indx);
                iter.user_data  = priv->products->list[indx];
                gtk_tree_model_row_inserted(GTK_TREE_MODEL(do_product_model), path, &iter);
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
        gtk_tree_model_row_deleted(GTK_TREE_MODEL(do_product_model), path);
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
    sort = do_sort_list_new(FALSE, FALSE, (do_list_cmp_func)cmp_func, do_product_model);
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
        if ( !do_product_model_get_index_for_code(do_product_model,&n, code) )
            return FALSE;
        iter.user_data = priv->products->list[n];
        gtk_tree_model_row_inserted(GTK_TREE_MODEL(do_product_model), path, &iter);
        gtk_tree_path_free(path);
    }
    return TRUE;
}
gboolean     do_product_model_is_filtered(DoProductModel *do_product_model)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);
    return priv->filter_codes != NULL;
}
gboolean    do_product_model_is_filtered_and_empty(DoProductModel *do_product_model)
{
    DoProductModelPrivate *priv = DO_PRODUCT_MODEL_GET_PRIVATE(do_product_model);
    return priv->filter_codes != NULL && priv->filter_codes->count == 0;
}
