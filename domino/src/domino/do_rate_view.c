
#include "do_rate_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_rate_dialog.h"
#include "domino.h"
#include "do_tree_view.h"
#include <domino.h>
#include "hig.h"
#include "do_icons.h"
#include <gdk/gdk.h>
#include <time.h>

#define DO_RATE_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_RATE_VIEW, DoRateViewPrivate))

#define OBJECT_ROOT_PATH "RateView"
#define DEFAULT_COL_PACK_FORM_WIDTH 400
#define DEFAULT_COL_DEFAULT_WIDTH 60

// View interface
static void do_rate_view_view_init(DoViewIface *iface);

static gboolean    do_rate_view_view_do_grab_focus(DoView *view);
static void	       do_rate_view_view_do_close(DoView *view);
static const char *do_rate_view_view_get_title(DoView *view);
static GdkPixbuf  *do_rate_view_view_get_icon(DoView *view);
static void		   do_rate_view_view_do_edit(DoView *view, const gchar *tab);
static gboolean	   do_rate_view_view_can_do_edit(DoView *view, const gchar *tab);
static gboolean	   do_rate_view_view_can_do_delete(DoView *view);
static void		   do_rate_view_view_do_delete(DoView *view);
static void		   do_rate_view_view_do_insert(DoView *view);
static gboolean    do_rate_view_view_can_do_insert(DoView *view);

enum
{
	PROP_0,
	PROP_ALIAS,
};

enum
{
    COLUMN_TIME = 0,
    COLUMN_RUB = 1,

};

struct _DoRateViewPrivate
{
    GtkListStore        *model;
    GtkTreeView         *tree_view;
    GtkTreeViewColumn   **columns;
    GtkTreeSelection    *selection;
    GdkPixbuf           *icon;
    gchar               *alias_name;
};

G_DEFINE_TYPE_WITH_CODE (DoRateView, do_rate_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_rate_view_view_init)
                                    );

static void do_rate_view_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_rate_view_view_do_grab_focus;
    iface->do_close      = do_rate_view_view_do_close;
    iface->get_title     = do_rate_view_view_get_title;
    iface->get_icon      = do_rate_view_view_get_icon;
    iface->do_edit       = do_rate_view_view_do_edit;
    iface->can_do_edit   = do_rate_view_view_can_do_edit;
    iface->do_delete       = do_rate_view_view_do_delete;
    iface->do_insert       = do_rate_view_view_do_insert;
    iface->can_do_delete   = do_rate_view_view_can_do_delete;
    iface->can_do_insert   = do_rate_view_view_can_do_insert;
}

static void do_rate_view_init(DoRateView *view)
{
	DoRateViewPrivate *priv = DO_RATE_VIEW_GET_PRIVATE (view);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_INFO, DO_VIEW_ICON_SIZE, 0, NULL);
}

static GtkListStore *do_rate_view_create_model(DoRateView *embed)
{
	DoRateViewPrivate *priv = DO_RATE_VIEW_GET_PRIVATE(embed);
    GtkListStore    *model;
    do_alias_t      *alias;
    do_ret_list_t   *list;
    do_extended_t   *extended;
    GtkTreeIter      iter;
    GType           *types;
    DoValutaInfo    *valuta;
    int j, indx, k;
    gint n_columns;

    document_order_key0_t document_order_key0;
    struct PACK_STRUCT {
        BTI_LONG line;
        BTI_REAL price[8];
        BTI_SINT nprice[8];
    } *ret_;
    time_t time;
    struct tm tm;
    GList  *l;
    double price;

    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_END))
        return NULL;

    n_columns = g_list_length(do_utilx_get_valuta_info_list())*2 + 1;
    types = (GType*)g_malloc(n_columns*sizeof(GType*));
    types[0] = GTK_TYPE_LONG;
    for (j = 1; j < n_columns; j+=2 ) {
        types[j] = GTK_TYPE_DOUBLE;
        types[j+1] = GTK_TYPE_INT;
    }

    model = gtk_list_store_newv(n_columns, types);

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);
    do_extended_set_key(extended, 0, &document_order_key0);
    do_extended_set_filter(extended, TRUE, 3, sizeof(document_order_key0.dtype),
                                              sizeof(document_order_key0.sklad),
                                              sizeof(document_order_key0.document));

    do_text_set(alias, document_order_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_RATE));
    do_text_set(alias, document_order_key0.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    do_text_set(alias, document_order_key0.document, DO_CONST_DEFAULT_DOCUMENT);
    document_order_key0.line = 0;

    do_extended_set_filter_term(extended, 0, DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, dtype), DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND, &document_order_key0.dtype);
    do_extended_set_filter_term(extended, 1, DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, sklad), DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND, &document_order_key0.sklad);
    do_extended_set_filter_term(extended, 2, DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, document), DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO, &document_order_key0.document);

    do_extended_set_fields(extended, 3,
                           do_size_and_offset(document_order_struct_t, line),
                           do_size_and_offset(document_order_struct_t, price),
                           do_size_and_offset(document_order_struct_t, nprice)
                           );
    while ( ((list = do_extended_get(extended, 100)) != NULL) &&
            do_ret_list_count(list) ) {
        for (indx = 0; indx < do_ret_list_count(list); indx++) {
            ret_ = do_ret_list_item_data(list, indx);

            time = do_date(ret_->line, &tm);
            gtk_list_store_append(model, &iter);
            gtk_list_store_set(model, &iter, 0, time, -1);
            for (l = do_utilx_get_valuta_info_list(),
                 j = 1; l ; l = l->next, j +=2) {
                valuta = l->data;
                price = 0;
                for (k = 0; k < 8; k++)
                    if (ret_->nprice[k] == valuta->code) {
                        price = ret_->price[k];
                        break;
                    }
                gtk_list_store_set(model, &iter,
                    j, price,
                    j + 1, valuta->code,
                     -1);
            }
        }
        do_ret_list_free(list);
    }
    if (list)
        do_ret_list_free(list);
    do_extended_free(extended);

    do_alias_free(alias);
    return model;
}
static void do_rate_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoRateView *view)
{
    do_rate_view_view_do_edit(DO_VIEW(view), NULL);
}

static GObject *do_rate_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkListStore        *model;
    DoTreeView          *view;
	DoRateView          *embed;
	GtkTreeView         *tree_view;
	DoRateViewPrivate   *priv;
	gint                 n_valuta;
	DoValutaInfo        *valuta;
	GList               *l;
	gint                 j;

	object = G_OBJECT_CLASS (do_rate_view_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_RATE_VIEW_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_RATE_VIEW(object);
	g_object_get(object, "tree-view", &tree_view, NULL);

    g_signal_connect (tree_view, "row-activated", G_CALLBACK(do_rate_view_row_activated), object);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(tree_view), FALSE);

	priv->tree_view = tree_view;
    priv->selection = gtk_tree_view_get_selection(tree_view);

	priv->model = model = do_rate_view_create_model(embed);
	if (model)
        g_object_set(view, "model", model,
//                           "fixed-height-mode", FALSE,
                           NULL);
    n_valuta = g_list_length(do_utilx_get_valuta_info_list());

    col = do_tree_view_add_column(view, "", "Дата", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, 0);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_date_time_date_only, GINT_TO_POINTER(0), NULL);
     gtk_tree_view_set_search_equal_func(tree_view,
             do_tree_view_search_equal_func_date_time, GINT_TO_POINTER(0), NULL);

    for (l = do_utilx_get_valuta_info_list(), j = 1; l; l = l->next, j += 2) {
        valuta = l->data;
        col = do_tree_view_add_column(view, "", valuta->name, DEFAULT_COL_DEFAULT_WIDTH);
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_column_pack_start (col, renderer, TRUE);
        gtk_tree_view_column_add_attribute (col, renderer, "text", j);
        gtk_tree_view_column_set_sort_column_id (col, j);
        gtk_tree_view_column_set_cell_data_func(col ,renderer,
                do_tree_cell_data_func_money_with_sign, GINT_TO_POINTER(j), NULL);
        gtk_tree_view_column_set_visible(col, valuta->code != 1);
    }
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(priv->model), 0, GTK_SORT_DESCENDING);
    gtk_tree_view_set_search_column(tree_view, 0);
	gtk_widget_show(GTK_WIDGET(object));

	return object;
}

static void do_rate_view_finalize (GObject *object)
{
    DoRateViewPrivate *priv;
	priv = DO_RATE_VIEW_GET_PRIVATE (object);
	if (priv->alias_name)
		g_free(priv->alias_name);

	G_OBJECT_CLASS (do_rate_view_parent_class)->finalize (object);
}

static void do_rate_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoRateViewPrivate *priv = DO_RATE_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            g_value_set_string (value, priv->alias_name);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
	/* no readable properties */
	g_assert_not_reached ();
}

static void do_rate_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoRateViewPrivate *priv = DO_RATE_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string (value);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
}

static void do_rate_view_class_init (DoRateViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);
    GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);

	o_class->constructor  = do_rate_view_constructor;
	o_class->finalize     = do_rate_view_finalize;
	o_class->get_property = do_rate_view_get_property;
	o_class->set_property = do_rate_view_set_property;

	g_type_class_add_private (object_class, sizeof (DoRateViewPrivate));

    g_object_class_install_property
        (o_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

}

GtkWidget *do_rate_view_new (const gchar *alias_name)
{
	return g_object_new (DO_TYPE_RATE_VIEW,
			     "alias-name", alias_name,
			     "object-root-path", OBJECT_ROOT_PATH,
			     (gpointer) NULL);
}
static gboolean do_rate_view_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void	do_rate_view_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_rate_view_view_get_title(DoView *view)
{
    return "Курсы валют ";
}
static GdkPixbuf  *do_rate_view_view_get_icon(DoView *view)
{
    DoRateViewPrivate *priv = DO_RATE_VIEW_GET_PRIVATE(view);
    return priv->icon;
}

static gboolean	do_rate_view_view_can_do_edit(DoView *view, const gchar *tab)
{
    if (tab && *tab != '\0')
        return FALSE;
    gboolean retval = FALSE;
	DoRateViewPrivate *priv = DO_RATE_VIEW_GET_PRIVATE(view);
    GList *list = gtk_tree_selection_get_selected_rows(priv->selection, (GtkTreeModel**)&priv->model);
    if (list) {
        retval = (g_list_length(list) == 1);
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    else
        retval = FALSE;
    return retval;
}
static gboolean	do_rate_view_view_can_do_delete(DoView *view)
{
    gboolean retval = FALSE;
	DoRateViewPrivate *priv = DO_RATE_VIEW_GET_PRIVATE(view);
    GList *list = gtk_tree_selection_get_selected_rows(priv->selection, (GtkTreeModel**)&priv->model);
    if (list) {
        retval = (g_list_length(list) == 1);
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    else
        retval = FALSE;
    return retval;
}
static gboolean do_rate_view_insert_from_context(DoRateView *view, DoContext *context, gboolean set_cursor)
{
	DoRateViewPrivate *priv = DO_RATE_VIEW_GET_PRIVATE(view);
	GtkTreeIter iter;
	document_order_rec_t document_order;
	document_order_key0_t document_order_key0;
    gchar *dtype;
    gchar *sklad;
    gchar *document;
    gint   line;
    do_alias_t *alias;
	struct tm tm;
	time_t time;
	GList *l;
	int j;
	DoValutaInfo *valuta;


    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_BARCODE, DO_DB_END)) {
        if (alias)
            do_alias_free(alias);
        return FALSE;
    }

    g_object_get(context, "dtype-code", &dtype, "sklad-code", &sklad, "document-number", &document, "line-number", &line, NULL);
    do_text_set(alias, document_order_key0.dtype, dtype);
    do_text_set(alias, document_order_key0.sklad, sklad);
    do_text_set(alias, document_order_key0.document, document);
    document_order_key0.line = line;
    if (do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_EQUAL) != DO_OK) {
        do_alias_free(alias);
        return FALSE;
    }
    time = do_date(document_order.data.line, &tm);
    gtk_list_store_append (priv->model, &iter);
    gtk_list_store_set(priv->model, &iter, 0, time, -1);
    for (l = do_utilx_get_valuta_info_list(),
         j = 1; l ; l = l->next, j +=2) {
        valuta = l->data;
        gtk_list_store_set(priv->model, &iter,
            j, do_document_order_price(alias, &document_order, valuta->code),
            j + 1, valuta->code,
             -1);
    }
    do_alias_free(alias);
    if (set_cursor) {
        GtkTreePath *path;
        path = gtk_tree_model_get_path(GTK_TREE_MODEL(priv->model), &iter);
        if (path) {
            //gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(priv->model), path, NULL,  TRUE, 0.5, 0.0);
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
            gtk_tree_path_free(path);
        }
    }
    return TRUE;
}

static void	do_rate_view_show_record(DoView *view, DoDialogAction action)
{
	DoRateViewPrivate *priv = DO_RATE_VIEW_GET_PRIVATE(view);
    GtkTreePath *path;
    GtkTreeIter iter;
    GValue line = { 0, };
    gint response;

    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        return;

    if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {
        gtk_tree_path_free(path);
        return;
    }
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, 0, &line);
    gtk_tree_path_free(path);
    DoContext *context, *new_context;
    GtkWidget *dialog;
    struct tm tm;
    time_t time;
    int ln;
    BTI_LONG date;
    time = g_value_get_long(&line);
    tm = *localtime(&time);
    do_date_set(&date, tm);
    ln = date;

    context = do_context_new_with_attributes(DO_CONTEXT_KIND_LINE,
     "dtype-code", do_param(DO_PARAM_DOCUMENT_TYPE_RATE),
     "sklad-code", do_param(DO_PARAM_COMMON_SKLAD),
     "document-number", DO_CONST_DEFAULT_DOCUMENT,
     "line-number", ln,
     NULL);

    dialog = do_rate_dialog_new(gtk_widget_get_toplevel(GTK_WIDGET(view)), priv->alias_name, 0, action, context);

    response = do_rate_dialog_run((DoRateDialog*)dialog);

    if (response == GTK_RESPONSE_ACCEPT) {
        switch (action) {
            case DO_DIALOG_ACTION_DELETE:
                gtk_list_store_remove(priv->model, &iter);
                break;
            case DO_DIALOG_ACTION_INSERT:
                g_object_get(dialog, "context", &new_context, NULL);
                do_rate_view_insert_from_context(DO_RATE_VIEW(view), new_context, TRUE);
                break;
            case DO_DIALOG_ACTION_EDIT:
                gtk_list_store_remove(priv->model, &iter);
                g_object_get(dialog, "context", &new_context, NULL);
                do_rate_view_insert_from_context(DO_RATE_VIEW(view), new_context, TRUE);
            default:
                break;
        }
    }
    g_object_unref(context);
    gtk_widget_destroy(dialog);
}

static void	do_rate_view_view_do_edit(DoView *view, const gchar *tab)
{
    do_rate_view_show_record(view, DO_DIALOG_ACTION_EDIT);
}
static void	do_rate_view_view_do_delete(DoView *view)
{
    do_rate_view_show_record(view, DO_DIALOG_ACTION_DELETE);
}
static void	do_rate_view_view_do_insert(DoView *view)
{
    do_rate_view_show_record(view, DO_DIALOG_ACTION_INSERT);
}
static gboolean do_rate_view_view_can_do_insert(DoView *view)
{
    return TRUE;
}
