
#include "do_rz_select.h"
#include "do_view.h"
#include "do_utilx.h"
#include "domino.h"
#include <domino.h>
#include <dolib.h>
#include "hig.h"
#include "do_tree_view.h"
#include "do_icons.h"
#include <gdk/gdk.h>
#include <time.h>

#define DO_RZ_SELECT_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_RZ_SELECT, DoRzSelectPrivate))

enum
{
	PROP_0,
	PROP_ALIAS,
	PROP_BARCODE,
	PROP_RZ_MAIN,
};

enum
{
    COLUMN_MNN = 0,
    COLUMN_TRADE_NAME,
    COLUMN_PACK_FORM,
    COLUMN_PACK_QUANT,
    COLUMN_UNPACK,
    COLUMN_PRICE,
    COLUMN_MANUFACTOR,
    N_COLUMNS,
};

struct _DoRzSelectPrivate
{
    GtkListStore        *model;
    GtkTreeView         *tree_view;
    GtkTreeViewColumn   *columns[N_COLUMNS];
    GtkTreeSelection    *selection;
    gchar               *alias_name;
    gchar               *barcode;
    rz_info_t           *rz_main;
};

G_DEFINE_TYPE (DoRzSelect, do_rz_select, DO_TYPE_TREE_VIEW)

static void do_rz_select_init(DoRzSelect *view)
{
}
#define DEFAULT_COL_DEFAULT_WIDTH 100
#define OBJECT_ROOT_PATH "RzSelect"

static GtkListStore *do_rz_select_create_model(DoRzSelect *embed)
{
	DoRzSelectPrivate *priv = DO_RZ_SELECT_GET_PRIVATE(embed);
    GtkListStore      *model;
    do_list_t            *list;
    do_alias_t        *alias;
    int                i;

    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_END))
        return NULL;

    model = priv->model = gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_BOOLEAN);

    list = do_rz_info_list_from_barcode(alias, priv->barcode);

    for (i = 0; i < list->count; i++) {
        rz_info_t *rz;
        GtkTreeIter iter;

        rz = list->list[i];
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model, &iter,
                    0, rz,
                    1, rz->unpack,
                     -1);
    }
    do_list_free(list);
    do_alias_free(alias);
    return model;
}
static void do_rz_select_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoRzSelect *view)
{
    //!!do_rz_select_view_do_edit(DO_VIEW(view), NULL);
}
typedef struct {
    gint col_n;
    DoRzSelectPrivate *priv;
} DoRzSelectCellDataFuncArg;

static void do_rz_select_cell_data_func (GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    gint col_n;
    GValue value = {0,};
    gchar *buf, *buf1;
    rz_info_t *rz;
    DoValutaInfo *valuta;
    DoRzSelectPrivate *priv;
    DoRzSelectCellDataFuncArg *arg;

    arg = data;

	priv = arg->priv;
    col_n = arg->col_n;

    gtk_tree_model_get_value(tree_model, iter, 0, &value);
    rz = g_value_get_pointer(&value);

    g_object_set(cell, "text", "~", NULL);
    switch (col_n) {
        case COLUMN_MNN:
            if (!priv->rz_main || strcmp(priv->rz_main->mnn, rz->mnn))
                g_object_set(cell, "text", rz->mnn, NULL);
            break;
        case COLUMN_TRADE_NAME:
            if (!priv->rz_main || strcmp(priv->rz_main->trade_name, rz->trade_name))
                g_object_set(cell, "text", rz->trade_name, NULL);
            break;
        case COLUMN_PACK_FORM:
            if (!priv->rz_main || strcmp(priv->rz_main->pack_form, rz->pack_form))
                g_object_set(cell, "text", rz->pack_form, NULL);
            break;
        case COLUMN_MANUFACTOR:
            if (!priv->rz_main || strcmp(priv->rz_main->manufactor, rz->manufactor))
                g_object_set(cell, "text", rz->manufactor, NULL);
            break;
        case COLUMN_PACK_QUANT:
            if (!priv->rz_main || priv->rz_main->pack_quant != rz->pack_quant) {
                buf = g_strdup_printf("%d", rz->pack_quant);
                g_object_set(cell, "text", buf, NULL);
                g_free(buf);
            }
            break;
        case COLUMN_PRICE:
            if (!priv->rz_main || priv->rz_main->valuta != rz->valuta || priv->rz_main->valuta != rz->valuta) {
                valuta = do_utilx_get_valuta_info(rz->valuta);
                buf = do_money_format(rz->price, 0);
                buf1 = g_strdup_printf("%s%s", buf, valuta->sign);
                g_object_set(cell, "text", buf1, NULL);
                g_free(buf);g_free(buf1);
            }
            break;
    }
}
static void do_rz_select_cell_data_destroy_notify(gpointer data)
{
    g_free(data);
}

static GObject *do_rz_select_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkListStore        *model;
    DoTreeView          *view;
	DoRzSelect          *embed;
	GtkTreeView         *tree_view;
	DoRzSelectPrivate   *priv;
	DoRzSelectCellDataFuncArg *arg[N_COLUMNS];
	gint i;

	object = G_OBJECT_CLASS (do_rz_select_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_RZ_SELECT_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_RZ_SELECT(object);
	g_object_get(object, "tree-view", &tree_view, NULL);

    g_signal_connect (tree_view, "row-activated", G_CALLBACK(do_rz_select_row_activated), object);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(tree_view), FALSE);

	priv->tree_view = tree_view;
    priv->selection = gtk_tree_view_get_selection(tree_view);

	priv->model = model = do_rz_select_create_model(embed);
	if (model)
        g_object_set(view, "model", model,
                           NULL);

    for (i = 0; i < N_COLUMNS; i++) {
        arg[i] = g_new(DoRzSelectCellDataFuncArg, 1);
        arg[i]->priv = priv;
        arg[i]->col_n = i;
    }
    col = priv->columns[COLUMN_MNN]
        = do_tree_view_add_column(view, "MNN", "МНН", DEFAULT_COL_DEFAULT_WIDTH);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_rz_select_cell_data_func, arg[COLUMN_MNN], do_rz_select_cell_data_destroy_notify);

    col = priv->columns[COLUMN_TRADE_NAME]
        = do_tree_view_add_column(view, "TradeName", "Торговое наименование", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_rz_select_cell_data_func, arg[COLUMN_TRADE_NAME], do_rz_select_cell_data_destroy_notify);

    col = priv->columns[COLUMN_PACK_FORM]
        = do_tree_view_add_column(view, "PackForm", "Форма выпуска", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_rz_select_cell_data_func, arg[COLUMN_PACK_FORM], do_rz_select_cell_data_destroy_notify);

    col = priv->columns[COLUMN_PACK_QUANT]
        = do_tree_view_add_column(view, "PackQuant", "Количество", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_rz_select_cell_data_func, arg[COLUMN_PACK_QUANT], do_rz_select_cell_data_destroy_notify);

    col = priv->columns[COLUMN_UNPACK]
        = do_tree_view_add_column(view, "Unpack", "Цена перв.уп.", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_pixbuf_warning, GINT_TO_POINTER(1), NULL);

    col = priv->columns[COLUMN_PRICE]
        = do_tree_view_add_column(view, "Price", "Цена", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_rz_select_cell_data_func, arg[COLUMN_PRICE], do_rz_select_cell_data_destroy_notify);

    col = priv->columns[COLUMN_MANUFACTOR]
        = do_tree_view_add_column(view, "Manufactor", "Производитель", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_rz_select_cell_data_func, arg[COLUMN_MANUFACTOR], do_rz_select_cell_data_destroy_notify);

	gtk_widget_show(GTK_WIDGET(object));

	return object;
}
static gboolean do_rz_select_free_rz(GtkTreeModel *model,
                                            GtkTreePath *path,
                                            GtkTreeIter *iter,
                                            gpointer data)
{
    GValue rz = {0,};
    gtk_tree_model_get_value(model, iter, 0, &rz);
    do_rz_info_free(g_value_get_pointer(&rz));
    return FALSE;
}
static void do_rz_select_finalize (GObject *object)
{
    DoRzSelectPrivate *priv;
	priv = DO_RZ_SELECT_GET_PRIVATE (object);
	if (priv->alias_name)
		g_free(priv->alias_name);
	if (priv->barcode)
		g_free(priv->barcode);

    gtk_tree_model_foreach(GTK_TREE_MODEL(priv->model), do_rz_select_free_rz, NULL);

	G_OBJECT_CLASS (do_rz_select_parent_class)->finalize (object);
}

static void do_rz_select_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoRzSelectPrivate *priv = DO_RZ_SELECT_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            g_value_set_string (value, priv->alias_name);
			break;
		case PROP_RZ_MAIN:
            g_value_set_object (value, priv->rz_main);
			break;
		case PROP_BARCODE:
            g_value_set_string (value, priv->barcode);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
	/* no readable properties */
	g_assert_not_reached ();
}

static void do_rz_select_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoRzSelectPrivate *priv = DO_RZ_SELECT_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string (value);
			break;
		case PROP_RZ_MAIN:
            priv->rz_main = g_value_get_pointer(value);
            break;
		case PROP_BARCODE:
            if (!priv->barcode)
                priv->barcode = g_value_dup_string (value);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
}

static void do_rz_select_class_init (DoRzSelectClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_rz_select_constructor;
	o_class->finalize     = do_rz_select_finalize;
	o_class->get_property = do_rz_select_get_property;
	o_class->set_property = do_rz_select_set_property;

	g_type_class_add_private (o_class, sizeof (DoRzSelectPrivate));

    g_object_class_install_property
        (o_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property
        (o_class,
         PROP_BARCODE,
         g_param_spec_string("barcode",
                      "Штрихкод",
                      "Штрихкод",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property
        (o_class,
         PROP_RZ_MAIN,
         g_param_spec_pointer("rz-main",
                      "rz-main",
                      "rz-main",
                       G_PARAM_READWRITE));

}

GtkWidget *do_rz_select_new (const gchar *alias_name, const gchar *barcode)
{
	return g_object_new (DO_TYPE_RZ_SELECT,
			     "alias-name", alias_name,
			     "object-root-path", OBJECT_ROOT_PATH,
			     "barcode", barcode,
			     (gpointer) NULL);
}
rz_info_t  *do_rz_select_get_select (DoRzSelect *select)
{
	DoRzSelectPrivate *priv = DO_RZ_SELECT_GET_PRIVATE(select);
	rz_info_t *retval;
	GList *list;

	retval = NULL;
    list = gtk_tree_selection_get_selected_rows(priv->selection, (GtkTreeModel**)&priv->model);
    if (list) {
        GtkTreeIter iter;
        GValue rz = {0,};

        gtk_tree_model_get_iter((GtkTreeModel*)priv->model, &iter, list->data);
        gtk_tree_model_get_value((GtkTreeModel*)priv->model, &iter, 0, &rz);
        retval = g_value_get_pointer(&rz);

        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    return retval;
}
