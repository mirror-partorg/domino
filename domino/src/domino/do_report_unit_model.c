
#include "do_report_unit_model.h"
#include <domino.h>
#include "domino.h"
#include "do_utilx.h"
#include "do_print.h"
#include "../misc/template.h"
#include <string.h>



#define DO_REPORT_UNIT_MODEL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_REPORT_UNIT_MODEL_TYPE, DoReportUnitModelPrivate))
#define DO_TYPE_REPORT_UNIT_MODEL_FLAGS do_report_unit_model_flags_get_type()


// boring declarations of local functions
static void do_report_unit_model_init(DoReportUnitModel *pkg_tree);
static void do_report_unit_model_class_init(DoReportUnitModelClass *klass);
static void do_report_unit_model_tree_model_init(GtkTreeModelIface *iface);
static void do_report_unit_model_finalize(GObject *object);
static GtkTreeModelFlags do_report_unit_model_get_flags(GtkTreeModel *tree_model);
static gint do_report_unit_model_get_n_columns(GtkTreeModel *tree_model);
static GType do_report_unit_model_get_column_type(GtkTreeModel *tree_model, gint index);
static gboolean do_report_unit_model_get_iter(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path);
static GtkTreePath *do_report_unit_model_get_path(GtkTreeModel *tree_model, GtkTreeIter *iter);
static void do_report_unit_model_get_value(GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value);
static gboolean do_report_unit_model_iter_next(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean do_report_unit_model_iter_children(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent);
static gboolean do_report_unit_model_iter_has_child(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gint do_report_unit_model_iter_n_children(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean do_report_unit_model_iter_nth_child(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n);
static gboolean do_report_unit_model_iter_parent(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child);

// print iface
static void   do_report_unit_model_print_init(DoPrintIface *iface);
static gchar *do_report_unit_model_get_html(DoPrint *print);

typedef struct _DoReportUnitModelPrivate  DoReportUnitModelPrivate;
typedef struct _DoReportUnitModelRecord  DoReportUnitModelRecord;

GType do_report_unit_model_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_report_unit_model_flags_values[] = {
         { DO_REPORT_UNIT_MODEL_FLAG_TOTAL_COLUMN, "DO_REPORT_UNIT_MODEL_FLAG_TOTAL_COLUMN", "total-column" },
         { DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW, "DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW", "total-row" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoReportUnitModelFlags", _do_report_unit_model_flags_values);
    }
    return type;
}
struct _DoReportUnitModelRecord
{
    gchar    *code;
    gchar    *desc;
    gdouble  *values;
    guint     indx;
    gboolean  no_total;
    gboolean  grey;
};

struct _DoReportUnitModelPrivate
{
    gint                    n_units;
    gint                    n_values;
    do_list_t              *values;
    DoReportUnitModelFlags  flags;
    DoReportUnitModelRecord total;
    gint                    stamp;
    gchar                  *title;
    gchar                  *footer;
    DoReportValueType      *value_types;
    gchar                 **header_titles;
    guint                   header_deep;
    guint                  *header_cols;
    gchar                 **unit_titles;
    gboolean                not_print_unit;
};

enum
{
	PROP_0,
	PROP_FLAGS,
	PROP_N_UNITS,
	PROP_N_VALUES,
};

static GObjectClass *parent_class = NULL;  /* GObject stuff - nothing to worry about */

G_DEFINE_TYPE_WITH_CODE (DoReportUnitModel, do_report_unit_model, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL,
						do_report_unit_model_tree_model_init)
			 G_IMPLEMENT_INTERFACE (DO_TYPE_PRINT,
						do_report_unit_model_print_init))

static void do_report_unit_model_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE (object);
	switch (prop_id)
	{
		case PROP_FLAGS:
            priv->flags = g_value_get_flags(value);
			break;
		case PROP_N_UNITS:
            priv->n_units = g_value_get_int(value);
            break;
		case PROP_N_VALUES:
            priv->n_values = g_value_get_int(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_report_unit_model_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE (object);
    switch (prop_id)
    {
		case PROP_FLAGS:
			g_value_set_flags (value, priv->flags);
			break;
        case PROP_N_UNITS:
            g_value_set_int(value, priv->n_units);
            break;
        case PROP_N_VALUES:
            g_value_set_int(value, priv->n_values);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static GObject *do_report_unit_model_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	DoReportUnitModelPrivate *priv;
	GObject                  *object;
	gint i;

	object = G_OBJECT_CLASS (do_report_unit_model_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(object);


    priv->values = do_list_new(TRUE);
    priv->value_types = g_malloc0(sizeof(DoReportValueType)*priv->n_values);
    priv->header_deep = 1;
    priv->header_cols = g_malloc0(sizeof(guint)*priv->n_values);
    priv->header_titles = g_malloc0(sizeof(gpointer)*priv->n_values*priv->n_values);
    for ( i = 0; i < priv->n_values*priv->n_values; i++ )
        priv->header_titles[i] = g_strdup("");
    priv->unit_titles = g_malloc0(sizeof(gpointer)*priv->n_units);
    for ( i = 0; i < priv->n_units; i++ )
        priv->unit_titles[i] = g_strdup("");

    if ( priv->flags & DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW ) {
        priv->total.code = do_strdup("");
        priv->total.desc = do_strdup("Итого");
        priv->total.indx = 0;
        priv->total.values = g_malloc0(sizeof(gdouble) * priv->n_units*priv->n_values);
    }
	return object;

}
static void do_report_unit_model_class_init(DoReportUnitModelClass *klass)
{
    GObjectClass *object_class;

    parent_class = (GObjectClass*) g_type_class_peek_parent (klass);
    object_class = (GObjectClass*) klass;

	object_class->constructor  = do_report_unit_model_constructor;
    object_class->finalize = do_report_unit_model_finalize;

	object_class->get_property = do_report_unit_model_get_property;
	object_class->set_property = do_report_unit_model_set_property;

	g_type_class_add_private (object_class, sizeof (DoReportUnitModelPrivate));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_REPORT_UNIT_MODEL_FLAGS,
				     0,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property
        (object_class,
				   PROP_N_UNITS,
				   g_param_spec_int("count-unit",
							"Количество подразделений",
							"Количество подразделений",
							1,10000,1,
							G_PARAM_READABLE |
							G_PARAM_WRITABLE |
							G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
                            G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property
        (object_class,
				   PROP_N_VALUES,
				   g_param_spec_int("count-value",
							"Количество параметров",
							"Количество параметров",
							1,10000,1,
							G_PARAM_READABLE |
							G_PARAM_WRITABLE |
							G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
                            G_PARAM_CONSTRUCT_ONLY));

}

static void do_report_unit_model_tree_model_init (GtkTreeModelIface *iface)
{
    iface->get_flags       = do_report_unit_model_get_flags;
    iface->get_n_columns   = do_report_unit_model_get_n_columns;
    iface->get_column_type = do_report_unit_model_get_column_type;
    iface->get_iter        = do_report_unit_model_get_iter;
    iface->get_path        = do_report_unit_model_get_path;
    iface->get_value       = do_report_unit_model_get_value;
    iface->iter_next       = do_report_unit_model_iter_next;
    iface->iter_children   = do_report_unit_model_iter_children;
    iface->iter_has_child  = do_report_unit_model_iter_has_child;
    iface->iter_n_children = do_report_unit_model_iter_n_children;
    iface->iter_nth_child  = do_report_unit_model_iter_nth_child;
    iface->iter_parent     = do_report_unit_model_iter_parent;
}
static void do_report_unit_model_print_init(DoPrintIface *iface)
{
    iface->get_html        = do_report_unit_model_get_html;
}

static void do_report_unit_model_init (DoReportUnitModel *do_report_unit_model)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(do_report_unit_model);
    priv->stamp = g_random_int(); // Random int to check whether iters belong to out model
}
static int do_report_unit_record_free(DoReportUnitModelRecord *rec)
{
    g_free(rec->code);
    g_free(rec->desc);
    g_free(rec->values);
    return TRUE;
}
static void do_report_unit_model_finalize(GObject *object)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(object);
    gint i;
    do_list_foreach(priv->values, (do_list_foreach_func)do_report_unit_record_free, NULL);
    do_list_free(priv->values);
    g_free(priv->value_types);
    for ( i = 0; i < priv->n_values*priv->n_values; i++ ) {
        if ( priv->header_titles[i] )
            g_free(priv->header_titles[i]);
    }
    g_free(priv->header_cols);
    g_free(priv->header_titles);
    if ( priv->flags & DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW )
        do_report_unit_record_free(&priv->total);
    (* parent_class->finalize) (object);
}

static GtkTreeModelFlags do_report_unit_model_get_flags(GtkTreeModel *tree_model)
{
    g_return_val_if_fail (DO_IS_REPORT_UNIT_MODEL(tree_model), (GtkTreeModelFlags)0);

    return (GTK_TREE_MODEL_LIST_ONLY | GTK_TREE_MODEL_ITERS_PERSIST);
}
static inline gint do_report_unit_model_count_column(DoReportUnitModel *model)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    gint n_columns;
    n_columns = 2;
    n_columns += priv->n_units*priv->n_values;
    if ( priv->flags & DO_REPORT_UNIT_MODEL_FLAG_TOTAL_COLUMN )
        n_columns += priv->n_values;
    return n_columns;
}
static gint do_report_unit_model_get_n_columns(GtkTreeModel *tree_model)
{
    g_return_val_if_fail (DO_IS_REPORT_UNIT_MODEL(tree_model), 0);
    return do_report_unit_model_count_column(DO_REPORT_UNIT_MODEL(tree_model));
}

static GType do_report_unit_model_get_column_type(GtkTreeModel *tree_model, gint index)
{
    g_return_val_if_fail (DO_IS_REPORT_UNIT_MODEL(tree_model), G_TYPE_INVALID);
    g_return_val_if_fail (index < do_report_unit_model_count_column(DO_REPORT_UNIT_MODEL(tree_model)) && index >= 0, G_TYPE_INVALID);

    switch ( index ) {
        case DO_REPORT_UNIT_MODEL_COL_CODE:
            return G_TYPE_STRING;
        case DO_REPORT_UNIT_MODEL_COL_DESC:
            return G_TYPE_STRING;
        default:
            return G_TYPE_DOUBLE;
    }
}

static gboolean do_report_unit_model_get_iter (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(tree_model);
    DoReportUnitModelRecord  *record;
    gint          *indices, n, depth;


    g_assert(DO_IS_REPORT_UNIT_MODEL(tree_model));
    g_assert(path != NULL);

    indices = gtk_tree_path_get_indices(path);
    depth   = gtk_tree_path_get_depth(path);

    g_assert(depth == 1); // depth 1 = top level; a model only has top level nodes and no children

    n = indices[0]; // the n-th top level row


    if ( n >= priv->values->count + 1 || n < 0 )
        return FALSE;
    if ( n == priv->values->count ) {
        if ( priv->flags & DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW )
            record = &priv->total;
        else
            return FALSE;
    }
    else
        record = priv->values->list[n];

    g_assert(record != NULL);

    iter->stamp      = priv->stamp;
    iter->user_data  = record;
    iter->user_data2 = NULL;
    iter->user_data3 = NULL;

    return TRUE;
}

static GtkTreePath *do_report_unit_model_get_path_by_record(DoReportUnitModel *model, DoReportUnitModelRecord *record)
{
    //DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    GtkTreePath  *path = NULL;

    path = gtk_tree_path_new();
    gtk_tree_path_append_index(path, record->indx);
    return path;
}

static GtkTreePath *do_report_unit_model_get_path(GtkTreeModel *tree_model, GtkTreeIter  *iter)
{
    GtkTreePath  *path;
    DoReportUnitModelRecord *record;

    g_return_val_if_fail (DO_IS_REPORT_UNIT_MODEL(tree_model), NULL);
    g_return_val_if_fail (iter != NULL,               NULL);
    g_return_val_if_fail (iter->user_data != NULL,    NULL);

    record = (DoReportUnitModelRecord*) iter->user_data;
    path =  do_report_unit_model_get_path_by_record(DO_REPORT_UNIT_MODEL(tree_model), record);

    return path;
}
static gdouble do_report_unit_model_get_column_total_value(DoReportUnitModel *model, DoReportUnitModelRecord *record, gint column)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    gdouble retval;
    gint i;

    if ( record->no_total )
        return 0;

    retval = 0;

    for ( i = 0; i < priv->n_units; i++ )
        retval += record->values[ i*priv->n_values + column];

    return retval;
}
static void do_report_unit_model_get_value(GtkTreeModel *tree_model, GtkTreeIter  *iter, gint column, GValue *value)
{
    DoReportUnitModelRecord  *record;
    DoReportUnitModel   *model = DO_REPORT_UNIT_MODEL(tree_model);
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(tree_model);

    g_return_if_fail (DO_IS_REPORT_UNIT_MODEL (tree_model));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (column < do_report_unit_model_count_column(model));

    g_value_init (value, do_report_unit_model_get_column_type(tree_model, column));

    record = (DoReportUnitModelRecord*) iter->user_data;

    g_return_if_fail ( record != NULL );

    switch ( column ) {
        case DO_REPORT_UNIT_MODEL_COL_CODE:
            g_value_set_string(value, record->code);
            break;
        case DO_REPORT_UNIT_MODEL_COL_DESC:
            g_value_set_string(value, record->desc);
            break;
        default:
            if ( column < priv->n_units*priv->n_values + DO_REPORT_UNIT_MODEL_N_COL)
                g_value_set_double(value, record->values[column - DO_REPORT_UNIT_MODEL_N_COL]);
            else
                g_value_set_double(value,
                    do_report_unit_model_get_column_total_value(model, record,
                                                                column - DO_REPORT_UNIT_MODEL_N_COL - priv->n_units*priv->n_values));

    }
}

static gboolean do_report_unit_model_iter_next(GtkTreeModel  *tree_model, GtkTreeIter   *iter)
{
    DoReportUnitModelRecord  *record, *nextrecord;
    //DoReportUnitModel    *model;
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(tree_model);
    gint indx;

    g_return_val_if_fail (DO_IS_REPORT_UNIT_MODEL (tree_model), FALSE);

    if (iter == NULL || iter->user_data == NULL)
        return FALSE;

    //model = DO_REPORT_UNIT_MODEL(tree_model);

    record = iter->user_data;
    indx = record->indx + 1;

    if ( priv->flags & DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW ) {
        if ( indx >= priv->values->count + 1 )
            return FALSE;
        if ( indx == priv->values->count )
            nextrecord = &priv->total;
        else
            nextrecord = priv->values->list[indx];
    }
    else {
        if ( indx >= priv->values->count )
            return FALSE;
        nextrecord = priv->values->list[indx];
    }

    iter->stamp     = priv->stamp;
    iter->user_data = nextrecord;

    return TRUE;
}

static gboolean do_report_unit_model_iter_children(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (parent == NULL || parent->user_data != NULL, FALSE);

    if ( parent )
        return FALSE;

    g_return_val_if_fail (DO_IS_REPORT_UNIT_MODEL (tree_model), FALSE);

    if ( priv->values->count == 0 ) {
        return FALSE;
    }

    iter->stamp     = priv->stamp;
    iter->user_data = priv->values->list[0];

    return TRUE;
}

static gboolean do_report_unit_model_iter_has_child (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    return FALSE;
}

static gint do_report_unit_model_iter_n_children (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (DO_IS_REPORT_UNIT_MODEL (tree_model), -1);
    g_return_val_if_fail (iter == NULL || iter->user_data != NULL, FALSE);

    if ( !iter ) {
        if ( priv->flags & DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW )
            return priv->values->count + 1;
        else
            return priv->values->count;
    }
    return 0;
}

static gboolean do_report_unit_model_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n)
{
    DoReportUnitModelRecord  *record;
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (DO_IS_REPORT_UNIT_MODEL (tree_model), FALSE);

    if ( parent )
        return FALSE;

    if ( priv->flags & DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW ) {
        if ( n >= priv->values->count + 1 )
            return FALSE;
        if ( n == priv->values->count )
            record = &priv->total;
        else
            record = priv->values->list[n];
    }
    else {
        if ( n >= priv->values->count )
            return FALSE;
        record = priv->values->list[n];
    }

    if ( record == NULL ) {
        return FALSE;
    }

    iter->stamp = priv->stamp;
    iter->user_data = record;

    return TRUE;
}

static gboolean do_report_unit_model_iter_parent (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child)
{
    return FALSE;
}

GtkTreeModel *do_report_unit_model_new(gint n_units, gint n_values, DoReportUnitModelFlags flags)
{
    GtkTreeModel *model;

    model = (GtkTreeModel*)g_object_new (DO_REPORT_UNIT_MODEL_TYPE,
                                    "flags", flags,
                                    "count-unit", n_units,
                                    "count-value", n_values,
                                    NULL);
    g_assert( model != NULL );
    return model;
}

void do_report_unit_model_append(DoReportUnitModel *model,
                                             const gchar  *code,
                                             const gchar  *desc,
                                             GtkTreeIter *iter)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    DoReportUnitModelRecord *record;
    GtkTreePath *path;
    record = g_malloc0(sizeof(DoReportUnitModelRecord));
    record->code = g_strdup(code);
    record->desc = g_strdup(desc);
    record->indx = priv->values->count;
    record->values = g_malloc0(sizeof(gdouble) * priv->n_units*priv->n_values);
    do_list_add(priv->values, record);
    priv->total.indx = priv->values->count;
    iter->stamp      = priv->stamp;
    iter->user_data  = record;
    iter->user_data2 = NULL;
    iter->user_data3 = NULL;

    path =  do_report_unit_model_get_path_by_record(DO_REPORT_UNIT_MODEL(model), record);
    gtk_tree_model_row_inserted(GTK_TREE_MODEL(model), path, iter);
    gtk_tree_path_free(path);

}
void do_report_unit_model_delete(DoReportUnitModel *model,
                                             GtkTreeIter *iter)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    DoReportUnitModelRecord *record;

    g_return_if_fail(iter->user_data != NULL && priv->stamp == iter->stamp);

    record = iter->user_data;
    do_list_delete(priv->values, record->indx);
    priv->total.indx = priv->values->count;

    GtkTreePath *path;
    path =  do_report_unit_model_get_path_by_record(DO_REPORT_UNIT_MODEL(model), record);
    gtk_tree_model_row_deleted(GTK_TREE_MODEL(model), path);
    gtk_tree_path_free(path);

    //do_report_unit_record_free(record);
}

void do_report_unit_model_set_code(DoReportUnitModel   *model,
                                             const gchar  *code,
                                             const gchar  *desc,
                                             GtkTreeIter *iter)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    DoReportUnitModelRecord *record;

    g_return_if_fail(iter->user_data != NULL && priv->stamp == iter->stamp);
    record = iter->user_data;
    record->code = g_strdup(code);
    record->desc = g_strdup(desc);

}

#define VALID_ITER(iter, list_store) ((iter)!= NULL && (iter)->user_data != NULL && list_store->stamp == (iter)->stamp && !g_sequence_iter_is_end ((iter)->user_data) && g_sequence_iter_get_sequence ((iter)->user_data) == list_store->seq)

static void do_report_unit_model_total_update(DoReportUnitModel *model, gint i_unit, gint i_value)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);

    if ( priv->flags & DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW ) {
        gdouble sum;
        gint i;
        sum = 0;
        GtkTreePath *path;
        GtkTreeIter iter;
        for ( i = 0; i < priv->values->count; i++ ) {
            DoReportUnitModelRecord *record;
            record = priv->values->list[i];
            sum += record->values[i_unit*priv->n_values + i_value];
        }
        priv->total.values[i_unit*priv->n_values + i_value] = sum;

        path =  do_report_unit_model_get_path_by_record(DO_REPORT_UNIT_MODEL(model), &priv->total);
        do_report_unit_model_get_iter(GTK_TREE_MODEL(model), &iter, path);
        gtk_tree_model_row_changed(GTK_TREE_MODEL(model), path, &iter);
        gtk_tree_path_free(path);
    }
}
void do_report_unit_model_add_unit_value (DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint i_unit,
                                             gint i_value,
                                             gdouble value)
{
    do_report_unit_model_set_unit_value (model,
                                             iter,
                                             i_unit,
                                             i_value,
                                             value+
            do_report_unit_model_get_unit_value (model,
                                             iter,
                                             i_unit,
                                             i_value));
}

void do_report_unit_model_set_unit_value (DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint i_unit,
                                             gint i_value,
                                             gdouble value)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    DoReportUnitModelRecord  *record;
    GtkTreePath *path;

    g_return_if_fail(iter->user_data != NULL && priv->stamp == iter->stamp);
    g_return_if_fail(i_unit >= 0 && i_unit < priv->n_units);
    g_return_if_fail(i_value >= 0 && i_value < priv->n_values);

    record = iter->user_data;

    record->values[i_unit*priv->n_values + i_value] = value;
    do_report_unit_model_total_update(model, i_unit, i_value);

    path =  do_report_unit_model_get_path_by_record(DO_REPORT_UNIT_MODEL(model), record);
    gtk_tree_model_row_changed(GTK_TREE_MODEL(model), path, iter);
    gtk_tree_path_free(path);

}
void  do_report_unit_model_set_no_total (DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint i_value)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    DoReportUnitModelRecord  *record;
    GtkTreePath *path;

    g_return_if_fail(iter->user_data != NULL && priv->stamp == iter->stamp);
    g_return_if_fail(i_value >= 0 && i_value < priv->n_values);

    record = iter->user_data;

    record->no_total = TRUE;
    path =  do_report_unit_model_get_path_by_record(DO_REPORT_UNIT_MODEL(model), record);
    gtk_tree_model_row_changed(GTK_TREE_MODEL(model), path, iter);
    gtk_tree_path_free(path);

}
void  do_report_unit_model_set_grey (DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint i_value)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    DoReportUnitModelRecord  *record;

    g_return_if_fail(iter->user_data != NULL && priv->stamp == iter->stamp);
    g_return_if_fail(i_value >= 0 && i_value < priv->n_values);

    record = iter->user_data;

    record->grey = TRUE;

}


gdouble do_report_unit_model_get_unit_value (DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint i_unit,
                                             gint i_value)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    DoReportUnitModelRecord  *record;

    g_return_val_if_fail(iter->user_data != NULL && priv->stamp == iter->stamp, 0);
    g_return_val_if_fail(i_unit >= 0 && i_unit < priv->n_units, 0);
    g_return_val_if_fail(i_value >= 0 && i_value < priv->n_values, 0);

    record = iter->user_data;

    return record->values[i_unit*priv->n_values + i_value];
}
static gboolean do_report_unit_model_is_total(DoReportUnitModel *model, GtkTreeIter *iter, gint column)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    DoReportUnitModelRecord *record;

    g_return_val_if_fail (iter != NULL,               FALSE);
    g_return_val_if_fail (iter->user_data != NULL,    FALSE);
    g_return_val_if_fail (column < do_report_unit_model_count_column(model), FALSE);

    record = (DoReportUnitModelRecord*) iter->user_data;
    return (column >= priv->n_units*priv->n_values + DO_REPORT_UNIT_MODEL_N_COL) ||
           (record->indx >= priv->values->count);
}
void do_report_unit_model_set_value(DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint column,
                                             gdouble value)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    gint i_unit;
    gint i_value;
    if ( column < DO_REPORT_UNIT_MODEL_N_COL ||
         do_report_unit_model_is_total(model, iter, column) )
        return;
    i_value = (column - DO_REPORT_UNIT_MODEL_N_COL) % priv->n_values;
    i_unit = (column - DO_REPORT_UNIT_MODEL_N_COL - i_value) / priv->n_values;
    do_report_unit_model_set_unit_value (model, iter, i_unit, i_value, value);
}


void do_report_unit_model_cell_data_func_product_name(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_product_name_format(g_value_get_string(&value));
    if ( !do_report_unit_model_is_total(DO_REPORT_UNIT_MODEL(tree_model), iter, GPOINTER_TO_INT(data)) )
        g_object_set(cell, "text", buf, NULL);
    else {
        gchar *markup;
        markup = g_strdup_printf("<b>%s</b>", buf);
        g_object_set(cell, "markup", markup, NULL);
        g_free(markup);
    }
    g_free (buf);
}
void do_report_unit_model_cell_data_func_rest(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_rest_format(g_value_get_double(&value));
    if ( !do_report_unit_model_is_total(DO_REPORT_UNIT_MODEL(tree_model), iter, GPOINTER_TO_INT(data)) )
        g_object_set(cell, "text", buf, NULL);
    else {
        gchar *markup;
        markup = g_strdup_printf("<b>%s</b>", buf);
        g_object_set(cell, "markup", markup, NULL);
        g_free(markup);
    }
    g_free (buf);
}
void do_report_unit_model_cell_data_func_money(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_money_format(g_value_get_double(&value), TRUE);
    if ( !do_report_unit_model_is_total(DO_REPORT_UNIT_MODEL(tree_model), iter, GPOINTER_TO_INT(data)) )
        g_object_set(cell, "text", buf, NULL);
    else {
        gchar *markup;
        markup = g_strdup_printf("<b>%s</b>", buf);
        g_object_set(cell, "markup", markup, NULL);
        g_free(markup);
    }
    g_free (buf);
}
static gchar *get_desc(const gchar *value)
{
    GString *res;
    gchar *str;
    gchar *ch, *head;
    res = g_string_new("");
    str = g_strdup(value);
    ch = str;
    head = ch;
    while ( ch[0] ) {
        if ( ch[0] == '\t' ) {
            ch[0] = '\0';
            g_string_append_printf(res, "%s&nbsp;&nbsp;",head);
            head = ch+1;
        }
        ch++;
    }
    if ( head[0] )
        g_string_append(res, head);
    g_free(str);
    return g_string_free(res, FALSE);
}
static gchar *do_report_unit_model_get_html(DoPrint *print)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(print);
    do_template_t *templ;
    gchar *title;
    gchar *footer;
    gchar *value;
    gint i,indx,j, k;
    GString *string;

    templ = do_template_new(TEMPLATE_REPORT_DEFAULT, TRUE);
    if ( !templ )
        return NULL;
    do_template_cursor_t *cursor;
    do_template_cursor_t *line;
    if ( priv->title )
        title = g_strdup(priv->title);
    else
        title = g_strdup("");
    if ( priv->footer )
        footer = g_strdup(priv->footer);
    else
        footer = g_strdup("");

    cursor = do_template_cursor(templ);
    line   = do_template_section(templ, "Line");
    if ( !line ) {
        do_log(LOG_ERR, "Неправильный шаблон \"%s\"", TEMPLATE_REPORT_DEFAULT);
        do_template_free(templ);
        return NULL;
    }

    string = g_string_new("<tr align=\"center\">");
    g_string_append_printf(string, "<td rowspan=\"%d\">%s</td>", priv->header_deep, "№");
    g_string_append_printf(string, "<td rowspan=\"%d\">%s</td>", priv->header_deep, "Наименование");
    indx = 0;
    for ( j = 0; j < priv->header_deep; j++ ) {
        for ( i = 0; i < priv->n_units + 1; i++ ) {
            if ( i == priv->n_units && !priv->flags & DO_REPORT_UNIT_MODEL_FLAG_TOTAL_COLUMN )
                break;
            if ( priv->not_print_unit && i < priv->n_units )
                continue;
            for ( k = 0; k < priv->header_cols[j]; k++ ) {
                if ( j )
                    g_string_append_printf(string, "<td colspan=\"%d\">%s</td>",
                                priv->n_values/priv->header_cols[j], priv->header_titles[indx + k]);
                else {
                    if ( i == priv->n_units )
                        g_string_append_printf(string, "<td colspan=\"%d\">Итого</td>",
                                priv->n_values/priv->header_cols[j]);
                    else {
                        g_string_append_printf(string, "<td colspan=\"%d\">%s %s</td>",
                                priv->n_values/priv->header_cols[j], priv->header_titles[indx + k],
                                priv->unit_titles[i]);
                    }
                }
            }
        }
        indx += priv->header_cols[j];
        if ( j < priv->header_deep -1 )
            g_string_append(string, "</tr><tr align=\"center\">");
    }
    g_string_append(string, "</tr>");
    value = g_string_free(string, FALSE);
    do_template_set(templ,
                    "Title", title,
                    "Footer", footer,
                    "Header", value, NULL);
    g_free(value);
    do_template_cursor_run_step_for(cursor, "Line");
    for ( indx = 0; indx < priv->values->count + 1; indx++ ) {
        DoReportUnitModelRecord *record;

        if ( indx == priv->values->count ) {
            if ( priv->flags & DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW ) {
                record = &priv->total;
            }
            else
                break;
        }
        else
            record = priv->values->list[indx];
        if ( record->grey )
            string = g_string_new("<tr bgcolor=\"#858585\" align=\"center\">");
        else
            string = g_string_new("<tr align=\"center\">");
        value = get_desc(record->desc);
        if ( indx == priv->values->count )
            g_string_append_printf(string, "<td colspan=\"2\" align=\"left\">%s</td>", value);
        else {
            g_string_append_printf(string, "<td>%d</td>", indx + 1);
            g_string_append_printf(string, "<td align=\"left\">%s</td>", value);
        }
        g_free(value);
        for ( i = 0; i < priv->n_units + 1; i++ ) {
            double value;
            gchar *str;
            if ( i == priv->n_units &&
                 !priv->flags & DO_REPORT_UNIT_MODEL_FLAG_TOTAL_COLUMN )
                 break;
            if ( priv->not_print_unit && i < priv->n_units )
                continue;

            for ( j = 0; j < priv->n_values; j++ ) {
                if ( i < priv->n_units )
                    value = record->values[i*priv->n_values + j];
                else
                    value = do_report_unit_model_get_column_total_value(DO_REPORT_UNIT_MODEL(print),
                                                                                record, j);
                switch ( priv->value_types[j] ) {
                    case DO_REPORT_VALUE_TYPE_DOUBLE:
                        g_string_append_printf(string, "<td NOWRAP>%.2f</td>", value);
                        break;
                    case DO_REPORT_VALUE_TYPE_MONEY:
                        str = do_money_format(value, TRUE);
                        g_string_append_printf(string, "<td NOWRAP>%s</td>", str);
                        g_free(str);
                        break;
                    case DO_REPORT_VALUE_TYPE_REST:
                        str = do_rest_format(value);
                        g_string_append_printf(string, "<td NOWRAP>%s</td>", str);
                        g_free(str);
                        break;
                }
            }

        }
        g_string_append(string, "</tr>");
        value = g_string_free(string, FALSE);
        do_template_set(templ, "Line", value, NULL);
        g_free(value);
        do_template_cursor_first_run(line);
    }
    do_template_cursor_skip(cursor);
    do_template_cursor_run(cursor);
    value = do_template_text(templ);
    do_template_free(templ);
    g_free(title);
    g_free(footer);

    return value;

}
int do_report_unit_model_is_empty(DoReportUnitModel   *model,
                                             GtkTreeIter *iter)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    DoReportUnitModelRecord  *record;

    g_return_val_if_fail(iter->user_data != NULL && priv->stamp == iter->stamp, FALSE);
    int i,j;

    record = (DoReportUnitModelRecord*) iter->user_data;
    for ( i = 0; i < priv->n_units; i++ )
        for ( j = 0; j < priv->n_values; j++ )
            if ( record->values[i*priv->n_values + j] )
                return FALSE;
    return TRUE;

}

void do_report_unit_model_set_title (DoReportUnitModel   *model,
                                             const gchar *title)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    if ( priv->title )
        g_free(priv->title);
    priv->title = g_strdup(title);
}
void do_report_unit_model_set_footer (DoReportUnitModel   *model,
                                             const gchar *footer)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    if ( priv->footer )
        g_free(priv->footer);
    priv->footer = g_strdup(footer);
}
void do_report_unit_model_set_value_type (DoReportUnitModel   *model,
                                             ... )
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    va_list args;
    gint i;
    va_start (args, model);
    for ( i = 0; i < priv->n_values; i++ )
        priv->value_types[i] = va_arg (args, DoReportValueType);
    va_end (args);
}

void do_report_unit_model_set_header_title (DoReportUnitModel   *model,
                                            guint deep,
                                             ... )
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    va_list args;
    gint i, j, k = 0;
    priv->header_deep = deep;
    va_start (args, deep);
    if ( priv->header_deep > priv->n_values )
        priv->header_deep = priv->n_values;
    for ( i = 0; i < priv->header_deep; i++ ) {
        priv->header_cols[i] = va_arg (args, guint);
        for ( j = 0; j < priv->header_cols[i]; j++,k++ ) {
            if ( priv->header_titles[k] )
                g_free(priv->header_titles[k]);
            priv->header_titles[k] = g_strdup(va_arg (args, gchar *));
        }
    }
    va_end (args);
}
void do_report_unit_model_set_unit_title (DoReportUnitModel   *model,
                                                  GList *list)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    GList *l;
    gint i;
    for ( i = 0, l = list; i < priv->n_units && l; i++,l=l->next ) {
        if ( priv->unit_titles[i] )
            g_free(priv->unit_titles[i]);
        priv->unit_titles[i] = g_strdup(l->data);
    }
}
void do_report_unit_model_set_print_units(DoReportUnitModel   *model, gboolean print)
{
    DoReportUnitModelPrivate *priv = DO_REPORT_UNIT_MODEL_GET_PRIVATE(model);
    priv->not_print_unit = !print;
}
