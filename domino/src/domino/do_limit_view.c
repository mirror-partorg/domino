
#include "do_limit_view.h"
#include "do_product_edit.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_sale_dialog.h"
#include "domino.h"
#include "do_tree_view.h"
#include <domino_action.h>
#include "do_entry_date_box.h"
#include "../misc/template.h"
#include <domino.h>
#include "hig.h"
#include "do_icons.h"
#include "do_utilx.h"
#include <gdk/gdk.h>
#include <time.h>

#define DO_LIMIT_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_LIMIT_VIEW, DoLimitViewPrivate))

#define OBJECT_ROOT_PATH "LimitView"
#define DEFAULT_COL_PACK_FORM_WIDTH 400
#define DEFAULT_COL_DEFAULT_WIDTH 60

#define MAX_STR_COUNT 4

// View interface
static void do_limit_view_view_init(DoViewIface *iface);

static gboolean    do_limit_view_view_do_grab_focus(DoView *view);
static void	       do_limit_view_view_do_close(DoView *view);
static const char *do_limit_view_view_get_title(DoView *view);
static GdkPixbuf  *do_limit_view_view_get_icon(DoView *view);
static gboolean    do_limit_view_fill_model(DoLimitView *view);
static gboolean    do_limit_view_can_do_print(DoView *view);
static void        do_limit_view_do_print(DoView *view);
static void        do_limit_view_do_print_now(DoView *view);
static void		do_limit_view_view_do_edit(DoView *view, const gchar *tab);
static gboolean	do_limit_view_view_can_do_edit(DoView *view, const gchar *tab);
static void		do_limit_view_view_do_popup(DoView *view, GdkEventButton* event);

static void do_limit_view_row_expanded(GtkTreeView *tree_view, GtkTreeIter* iter, GtkTreePath *path, DoLimitView *view);

void do_limit_view_data_func_description(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         DoLimitView *view);

enum
{
	PROP_0,
	PROP_ALIAS,
};

/*
enum
{
    COLUMN_DESC = 0,
    COLUMN_PURCHASE,
    COLUMN_SALE,
    COLUMN_PROFIT,
    COLUMN_PROFIT_PURCHASE,
    COLUMN_PROFIT_SALE,
    COLUMN_CHECK,
    COLUMN_NO ,

};
*/

struct _DoLimitViewPrivate
{
    GtkTreeModel        *model;
    GtkTreeView         *tree_view;
    GtkTreeViewColumn   **columns;
    GtkTreeViewColumn   *col_rest;
    GtkTreeViewColumn   *col_sale;
    GtkTreeSelection    *selection;
    GtkWidget           *date1;
    GtkWidget           *date2;
    GtkWidget           *check_[2];
    GtkWidget           *filter;
    gint                postponement;
    GtkWidget           *footer;
    GtkWidget           *header;
    GdkPixbuf           *icon;
//    gchar               *unit;
    gchar               *alias_name;


};

G_DEFINE_TYPE_WITH_CODE (DoLimitView, do_limit_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_limit_view_view_init)
                                    );

static void do_limit_view_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_limit_view_view_do_grab_focus;
    iface->do_close      = do_limit_view_view_do_close;
    iface->get_title     = do_limit_view_view_get_title;
    iface->get_icon      = do_limit_view_view_get_icon;
    iface->do_print      = do_limit_view_do_print;
    iface->do_print_now      = do_limit_view_do_print_now;
    iface->can_do_print  = do_limit_view_can_do_print;

    iface->do_edit       = do_limit_view_view_do_edit;
    iface->can_do_edit   = do_limit_view_view_can_do_edit;
    iface->do_popup      = do_limit_view_view_do_popup;
    //iface->do_delete       = do_limit_view_view_do_delete;
    //iface->do_insert       = do_limit_view_view_do_insert;
    //iface->can_do_delete   = do_limit_view_view_can_do_delete;
    //iface->can_do_insert   = do_limit_view_view_can_do_insert;
}

static void do_limit_view_init(DoLimitView *view)
{
	//!!DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE (view);
    //!!GtkIconTheme *icon_theme;
    //!!icon_theme = gtk_icon_theme_get_default();
    //!!priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_FILE, DO_VIEW_ICON_SIZE, 0, NULL);
}

static GtkTreeModel *do_limit_view_create_model(DoLimitView *embed)
{
    gint count =  18;

    GtkTreeModel *model;
    GType *types;

    types = g_new(GType, count);
    types[0] = G_TYPE_INT;
    types[1] = G_TYPE_STRING;
    types[2] = G_TYPE_STRING;
    types[3] = G_TYPE_DOUBLE;
    types[4] = G_TYPE_STRING;
    types[5] = G_TYPE_STRING;
    types[6] = G_TYPE_DOUBLE;
    types[7] = G_TYPE_DOUBLE;
    types[8] = G_TYPE_STRING;
    types[9] = G_TYPE_STRING;
    types[10] = G_TYPE_STRING;
    types[11] = G_TYPE_STRING;
    types[12] = G_TYPE_STRING;
    types[13] = G_TYPE_STRING;
    types[14] = G_TYPE_DOUBLE;
    types[15] = G_TYPE_STRING;
    types[16] = G_TYPE_STRING;
    types[17] = G_TYPE_STRING;

    model = GTK_TREE_MODEL(gtk_tree_store_newv(count, types));
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(model), 2, GTK_SORT_ASCENDING);


    return model;
}
static void do_limit_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoLimitView *view)
{
	DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE (view);
    GtkTreeIter iter;
    GValue value = {0,};
    domino_marked_t marked;
    const gchar *code;
    struct tm tm1;
    struct tm tm2;
    struct tm tm_mark;
    time_t now;

    gtk_tree_model_get_iter(priv->model, &iter, path);
    gtk_tree_model_get_value(priv->model, &iter, 1, &value);
    code = g_value_get_string(&value);
    if ( !code )
        return;

    now = time(NULL);
    tm1 = *localtime(&now);
    tm2 = tm1;
    tm1.tm_mday-= DOMINO_MARKED_INTERVAL;
    mktime(&tm1);



    if ( !domino_order_get_marked_(priv->alias_name, domino_unit(), code, &tm1, &tm2, &marked, &tm_mark) )
        return;

    switch (marked) {
        case DOMINO_MARKED_AND_ACCEPT:
            marked = DOMINO_MARKED_AND_REJECT;
            break;
        case DOMINO_MARKED_UNMARK:
        case DOMINO_MARKED:
            marked = DOMINO_MARKED_AND_ACCEPT;
            break;
        case DOMINO_MARKED_AND_REJECT:
            if ( gtk_tree_path_get_depth(path) > 1 )
                marked = DOMINO_MARKED_UNMARK;
            else
                marked = DOMINO_MARKED;
            break;
    }
    if ( !domino_order_set_marked_(priv->alias_name, domino_unit(), code, &tm1, &tm2, marked) )
        return;

    gtk_tree_store_set(GTK_TREE_STORE(priv->model), &iter, 0, marked, -1);
}

static GtkWidget *do_limit_view_create_header(DoLimitView *view);
static void cell_received(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, 0, &value);

#if GTK_MAJOR_VERSION > 2
    switch ( g_value_get_int(&value) ) {
        case DO_MARKED_AND_ACCEPT:
            g_object_set(cell, "icon-name", "object-select-symbolic", NULL);
            break;
        case DO_MARKED_AND_REJECT:
            g_object_set(cell, "icon-name", "edit-delete-symbolic", NULL);
            break;
        default:
            g_object_set(cell, "icon-name", NULL, NULL);
    }
#else
    switch ( g_value_get_int(&value) ) {
        case DO_MARKED_AND_ACCEPT:
            g_object_set(cell, "icon-name", GTK_STOCK_OK, NULL);
            break;
        case DO_MARKED_AND_REJECT:
            g_object_set(cell, "icon-name", GTK_STOCK_CANCEL, NULL);
            break;
        default:
            g_object_set(cell, "icon-name", NULL, NULL);
    }
#endif

}
#define DO_LIMIT_VIEW_POPUP "limit-view-popup"

static gchar *popup_ui =
    "  <popup name='" DO_LIMIT_VIEW_POPUP "'>\n"
    "    <menuitem action='ProductEditStock'/>\n"
    "  </popup>\n";

static GObject *do_limit_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkTreeModel        *model;
    DoTreeView          *view;
	DoLimitView      *embed;
	GtkTreeView         *tree_view;
	DoLimitViewPrivate   *priv;

	object = G_OBJECT_CLASS (do_limit_view_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_LIMIT_VIEW_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_LIMIT_VIEW(object);
	g_object_get(object, "tree-view", &tree_view, NULL);

    g_signal_connect (tree_view, "row-activated", G_CALLBACK(do_limit_view_row_activated), object);
    g_signal_connect (tree_view, "row-expanded", G_CALLBACK(do_limit_view_row_expanded), object);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(tree_view), FALSE);

	priv->tree_view = tree_view;
    priv->selection = gtk_tree_view_get_selection(tree_view);

	priv->model = model = do_limit_view_create_model(embed);
	if (model)
        g_object_set(view, "model", model,
                           "fixed-height-mode", FALSE,
                           NULL);

	do_view_actions_add_ui_from_string(popup_ui, NULL);

    col = do_tree_view_add_column(view, "Mark", "*", -1);
    renderer = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, renderer, cell_received, object, NULL);
    g_object_set (renderer,
                    "xalign", (gfloat)0.5,
                    "yalign", (gfloat)0.5,
                    "mode", GTK_CELL_RENDERER_MODE_INERT,
                    NULL);
    gtk_tree_view_column_set_sort_column_id (col, 0);


    col = do_tree_view_add_column(view, "Name", "Наименование", 400);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    //gtk_tree_view_column_add_attribute (col, renderer, "text", 2);
    gtk_tree_view_column_set_cell_data_func(col, renderer, do_tree_cell_data_func_product_name, GINT_TO_POINTER(2), NULL);
    gtk_tree_view_column_set_sort_column_id (col, 2);
    g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);


    col = do_tree_view_add_column(view, "Manufactor", "Производитель", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", 15);


    col = do_tree_view_add_column(view, "Price", "Цена", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 0.5, NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", 7);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_money, GINT_TO_POINTER(7), NULL);

    priv->col_rest = col = do_tree_view_add_column(view, "Rest", "Остаток", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 0.5, NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", 3);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_rest, GINT_TO_POINTER(3), NULL);

    priv->col_sale = col = do_tree_view_add_column(view, "Sale", "Реализация", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 0.5, NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", 6);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_rest, GINT_TO_POINTER(6), NULL);

    col = do_tree_view_add_column(view, "Ads", "Указание", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    //g_object_set(renderer, "xalign" , 1.0, NULL);
    gtk_tree_view_column_set_sort_column_id (col, 16);
    gtk_tree_view_column_add_attribute (col, renderer, "text", 16);


    col = do_tree_view_add_column(view, "Purchase", "Закуп", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    //g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", 4);

    col = do_tree_view_add_column(view, "Purchase1", "", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 0.5, NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", 11);

    col = do_tree_view_add_column(view, "Purchase2", "", -1);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 0.1, "font", "bold", NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", 12);
    col = do_tree_view_add_column(view, "Purchase3", "", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    //g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", 13);

    col = do_tree_view_add_column(view, "Real", "Реализация", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    //g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", 5);

    col = do_tree_view_add_column(view, "Real1", "", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 0.1, "font", "bold", NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", 8);

    col = do_tree_view_add_column(view, "Real2", "", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xpad" , 4, NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", 9);

    col = do_tree_view_add_column(view, "Real3", "", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 0.1, NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", 10);


    col = do_tree_view_add_column(view, "Code", "Остаток в организации", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    //g_object_set(renderer, "xalign" , 1.0, NULL);

    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_rest, GINT_TO_POINTER(14), NULL);
    gtk_tree_view_column_set_sort_column_id (col, 14);


    gtk_tree_view_set_search_column(tree_view, 2);
    gtk_tree_view_set_search_equal_func(tree_view, gtk_tree_view_seach_by_product_name_func, NULL, NULL);



    priv->header = do_limit_view_create_header(embed);


    do_tree_view_pack_header(view, GTK_WIDGET(priv->header));
    gtk_widget_show_all(GTK_WIDGET(priv->header));

    priv->footer = gtk_label_new("");
    g_object_set(priv->footer, "xalign", 0.01, NULL);
    do_tree_view_pack_footer(view, GTK_WIDGET(priv->footer));
    gtk_widget_show(GTK_WIDGET(priv->footer));


	gtk_widget_show(GTK_WIDGET(object));

    //do_limit_view_fill_model(embed);

	return object;
}

static void do_limit_view_finalize (GObject *object)
{
    //DoLimitViewPrivate *priv;
	//priv = DO_LIMIT_VIEW_GET_PRIVATE (object);
    //g_free(priv->units);

	G_OBJECT_CLASS (do_limit_view_parent_class)->finalize (object);
}

static void do_limit_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE (object);

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
static void do_limit_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE (object);

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

static void do_limit_view_class_init (DoLimitViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_limit_view_constructor;
	o_class->finalize     = do_limit_view_finalize;
	o_class->get_property = do_limit_view_get_property;
	o_class->set_property = do_limit_view_set_property;


    g_object_class_install_property
        (o_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_type_class_add_private (o_class, sizeof (DoLimitViewPrivate));


}

GtkWidget *do_limit_view_new ()
{
	return g_object_new (DO_TYPE_LIMIT_VIEW,
			     "object-root-path", OBJECT_ROOT_PATH,
			     (gpointer) NULL);
}
static gboolean do_limit_view_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void	do_limit_view_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_limit_view_view_get_title(DoView *view)
{
//    DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE(view);
    return "Остатки меньше реализации";
}
static GdkPixbuf  *do_limit_view_view_get_icon(DoView *view)
{
    DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE(view);
    return priv->icon;
}

typedef struct {
    sklad_key0_t sklad_key0;
    gchar *name;
    gdouble purchase[3];
    gdouble sale[3];
    gint  checks;
    gchar *otdel_name;
} sale_t;

typedef struct {
    GtkTreeIter iter;
    sale_t sale;
} otdel_sale_t;

static void filter_clicked(GtkWidget *w, DoLimitView *view);
static void filter_changed(GtkComboBox *w, DoLimitView *view);

static GtkWidget *do_limit_view_create_header(DoLimitView *view)
{
    DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE(view);

    GtkWidget *retval;
    GtkWidget *d1, *d2;
    GtkWidget *hbox;
    GtkWidget *l;
    GtkWidget *e;
    GDate *date1;
    GDate *date2;
    GTimeVal today;
    //int i;

    g_get_current_time(&today);

    date1 = g_date_new();
    date2 = g_date_new();
    g_date_set_time_val(date1, &today);
    g_date_set_time_val(date2, &today);
    g_date_add_days (date1, -10);
    g_date_add_days (date2, -1);

#if GTK_MAJOR_VERSION >2
    retval = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
#else
    retval = gtk_vbox_new(FALSE, 6);
    hbox = gtk_hbox_new(FALSE, 6);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(retval), 3);
    gtk_box_pack_start(GTK_BOX(retval), hbox, FALSE, FALSE ,0);
    /*l = gtk_alignment_new( 0.0f, 0.0f, 0.0f, 0.0f );
    gtk_widget_set_size_request( l, 0u, 1u );*/

    priv->date1 = d1= do_entry_date_box_new(0);
    priv->date2 = d2= do_entry_date_box_new(0);
    g_object_set(d1, "year", g_date_get_year(date1), "month", g_date_get_month(date1), "day", g_date_get_day(date1), NULL);
    g_object_set(d2, "year", g_date_get_year(date2), "month", g_date_get_month(date2), "day", g_date_get_day(date2), NULL);
    //gtk_box_pack_start(GTK_BOX(hbox), l, FALSE, FALSE ,0);
    gtk_box_pack_start(GTK_BOX(hbox), d1, FALSE, FALSE ,0);
    gtk_box_pack_start(GTK_BOX(hbox), d2, FALSE, FALSE ,0);


    priv->check_[0] = l = gtk_check_button_new_with_label("заказаные");
    gtk_box_pack_start(GTK_BOX(hbox), l, FALSE, FALSE ,0);

    /*priv->check_[1] = l = gtk_check_button_new_with_label("дефектура");
    gtk_box_pack_start(GTK_BOX(hbox), l, FALSE, FALSE ,0);*/


    priv->filter = l = gtk_combo_box_text_new();
    gtk_box_pack_start(GTK_BOX(hbox), l, FALSE, FALSE ,0);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(l), "Остатки меньше реализации");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(l), "Нулевые остатки");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(l), "Отсутствие товара в аптеке");
    gtk_combo_box_set_active(GTK_COMBO_BOX(l), 0);
    g_signal_connect(l, "changed", G_CALLBACK(filter_changed), view);

    e = gtk_button_new_with_label("Выполнить");
    gtk_box_pack_start(GTK_BOX(hbox), e, FALSE, FALSE ,0);
    g_signal_connect(e, "clicked", G_CALLBACK(filter_clicked), view);



    //hbox = gtk_hbox_new(FALSE, 6);
    //gtk_box_pack_start(GTK_BOX(retval), hbox, FALSE, FALSE ,0);


    return retval;
}
static void filter_clicked(GtkWidget *w, DoLimitView *view)
{
    DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE(view);

    if ( !do_validate_valid(DO_VALIDATE(priv->date1)) ) {
        gtk_widget_grab_focus(priv->date1);
        return;
    }
    if ( !do_validate_valid(DO_VALIDATE(priv->date2)) ) {
        gtk_widget_grab_focus(priv->date2);
        return;
    }

    if ( do_limit_view_fill_model(view) )
        do_tree_view_grab_focus(DO_TREE_VIEW(view));

}



typedef struct {
   char code[6];
   double quant;
}
realization_t;

static inline int real_cmp(realization_t *r1, realization_t *r2)
{
    return strcmp(r1->code, r2->code);
}

static do_sort_list_t *get_real(do_alias_t *alias, struct tm *tm1, struct tm *tm2)
{
    do_sort_list_t *realization;
    realization = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)real_cmp, NULL);
    realization_t rec;
    document_key3_t document_key3, key;
    document_rec_t document;
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    int status, index;

    do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    do_text_set(alias, document_key3.sklad, domino_unit());
    do_date_set(&document_key3.date, *tm1);
    document_key3.time = 0;
    do_cpy(key.dtype, document_key3.dtype);
    do_cpy(key.sklad, document_key3.sklad);
    do_date_set(&key.date, *tm2);

    rec.code[5] = '\0';

    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
    while ( status ) {
        if ( key.date < document_key3.date ||
             do_cmp(key.sklad, document_key3.sklad) ||
             do_cmp(key.dtype, document_key3.dtype) ) break;

        if ( document.data.dtype1[0] == '0' && document.data.dtype1[1]>='1' && document.data.dtype1[1]<='4') {
            do_cpy(document_order_key0.dtype, document.data.dtype);
            do_cpy(document_order_key0.sklad, document.data.sklad);
            do_cpy(document_order_key0.document, document.data.document);
            document_order_key0.line = 0;

            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
            while ( status ) {
                if (
                     do_cmp(document_order_key0.document, document.data.document) ||
                     do_cmp(document_order_key0.sklad, document.data.sklad) ||
                     do_cmp(document_order_key0.dtype, document.data.dtype)
                ) break;

                if ( document_order.data.accepted ) {
                    strncpy(rec.code, document_order.data.code, 5);
                    if ( do_sort_list_find(realization, &index, &rec) ) {
                        ((realization_t*)realization->list[index])->quant +=
                        do_document_order_quant(alias, &document_order, 1);
                    }
                    else {
                        rec.quant = do_document_order_quant(alias, &document_order, 1);
                        do_sort_list_insert_new(realization, index, &rec, sizeof(rec));
                    }

                }

                status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
            }
            if ( status == DO_ERROR )
                break;
        }
        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }

    if ( status == DO_ERROR ) {
        do_sort_list_free(realization);
        return NULL;
    }
    return realization;

}
static do_sort_list_t *get_stock(do_alias_t *alias, struct tm *tm1, struct tm *tm2)
{

    do_sort_list_t *stocks, *all;
    do_sort_list_t *retval;

	realization_rec_t realization;
	realization_key4_t realization_key4, key;

    stocks = do_report_get_rest(alias, domino_unit());
    if ( !stocks )
        return NULL;
    all = do_report_get_rest(alias, NULL);
    if ( !all )
        return NULL;

    retval = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)real_cmp, NULL);
    int i;
    for ( i = 0; i < stocks->count; i++ ) {
        do_rest_t *rest;
        rest = stocks->list[i];
        if ( rest->quant[0] < 0.001 ) {
            realization_t crnt;
            crnt.code[5] = '\0';
            int indx;
            if ( !do_sort_list_find(all, &indx, rest) ) {
                fprintf(stderr, "Error !!\n");
                break;
            }
            if ( ((do_rest_t*)all->list[indx])->quant[0] > 0 ) {

                struct tm tm;
                time_t now;
                now = time(NULL);
                tm = *localtime(&now);

                do_text_set(alias, realization_key4.sklad, domino_unit());
                do_text_set(alias, realization_key4.code, rest->code);
                do_cpy(key.sklad, realization_key4.sklad);
                do_cpy(key.code, realization_key4.code);

                realization_key4.month = do_datetosql(tm);
                tm.tm_year--;
                mktime(&tm);
                key.month = do_datetosql(tm);

                if ( ( do_realization_get4(alias, &realization, &realization_key4, DO_GET_LE) == DO_KEY_NOT_FOUND) ||
                     do_cmp(key.sklad, realization_key4.sklad) ||
                     do_cmp(key.code, realization_key4.code) ||
                     key.month > realization_key4.month
                    )  {

                    strncpy(crnt.code, rest->code, 5);
                    crnt.quant = rest->quant[0];
                    do_sort_list_add_alloc(retval, &crnt, sizeof(crnt));
                }
            }
        }
    }
    do_rest_list_free(stocks);
    return retval;
}

static do_sort_list_t *get_stock1(do_alias_t *alias, struct tm *tm1, struct tm *tm2)
{

    do_sort_list_t *stocks;
    do_sort_list_t *retval;

	realization_rec_t realization;
	realization_key4_t realization_key4, key;

    stocks = do_report_get_rest(alias, domino_unit());
    if ( !stocks )
        return NULL;

    retval = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)real_cmp, NULL);
    int i;
    for ( i = 0; i < stocks->count; i++ ) {
        do_rest_t *rest;
        rest = stocks->list[i];
        if ( rest->quant[0] < 0.001 ) {

            struct tm tm;
            time_t now;
            now = time(NULL);
            tm = *localtime(&now);

            do_text_set(alias, realization_key4.sklad, domino_unit());
            do_text_set(alias, realization_key4.code, rest->code);
            do_cpy(key.sklad, realization_key4.sklad);
            do_cpy(key.code, realization_key4.code);

            realization_key4.month = do_datetosql(tm);
            tm.tm_year--;
            mktime(&tm);
            key.month = do_datetosql(tm);

            if ( do_realization_get4(alias, &realization, &realization_key4, DO_GET_LE) == DO_OK )
                continue;
            if ( do_cmp(key.sklad, realization_key4.sklad) ||
                 do_cmp(key.code, realization_key4.code) ||
                 key.month > realization_key4.month
                )  continue;

            realization_t crnt;
            crnt.code[5] = '\0';
            strncpy(crnt.code, rest->code, 5);
            crnt.quant = 0;
            do_sort_list_add_alloc(retval, &crnt, sizeof(crnt));
        }
    }
    do_rest_list_free(stocks);
    return retval;
}
static gchar *MonthShort[12] =
{"Янв","Фев","Мар","Апр","Май","Июн","Июл","Авг","Сен","Окт","Ноя","Дек"
};
static gchar *MonthFull[12] =
{"Январь","Февраль","Март","Апрель","Май","Июнь","Июль","Август","Сентябрь","Октябрь","Ноябрь","Декабрь"
};

static gboolean do_limit_view_fill_model(DoLimitView *view)
{
    DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE(view);

    gboolean skip_ordered;
    //gboolean skip_def;
    gint day, month, year;
    gboolean retval = TRUE;
    struct tm tm1;
    struct tm tm2;
    do_alias_t *alias;
    do_sort_list_t *realization;
    int count =0;
    int i;
    int kind;

    gtk_tree_store_clear(GTK_TREE_STORE(priv->model));
    alias = domino_alias_new(priv->alias_name);
    if ( !alias )
        return FALSE;
    if ( !do_alias_open(alias, TRUE) ) {
        do_alias_free(alias);
        return FALSE;
    }



    g_object_get(priv->date1, "day", &day, "month", &month, "year", &year, NULL);
    do_date_set_ymd(&tm1, year, month, day);
    g_object_get(priv->date2, "day", &day, "month", &month, "year", &year, NULL);
    do_date_set_ymd(&tm2, year, month, day);
    skip_ordered = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->check_[0]));
    //skip_def = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->check_[1]));
    kind = gtk_combo_box_get_active(GTK_COMBO_BOX(priv->filter));

    //gtk_tree_view_column_set_visible(priv->col_rest, !kind);
    gtk_tree_view_column_set_visible(priv->col_sale, !kind);

    switch ( kind ) {
        case 0:
            realization = get_real(alias, &tm1, &tm2);
            break;
        case 1:
            realization = get_stock1(alias, &tm1, &tm2);
            break;
        case 2:
            realization = get_stock(alias, &tm1, &tm2);
            break;
        default:
            break;
    }

    if ( !realization )
        return FALSE;
    struct tm tm1_;
    struct tm tm2_;
    struct tm tm_mark;
    time_t now;

    now = time(NULL);
    tm1_ = *localtime(&now);
    tm2_ = tm1_;
    tm1_.tm_mday-= DOMINO_MARKED_INTERVAL;
    mktime(&tm1_);


    int inx;
    int firm;
    firm = do_firm_get_firm_by_unit(alias, domino_unit());
    do_string_list_t *units = do_firm_get_units(alias, firm);

    for ( inx = 0; inx < realization->count; inx++ ) {
        realization_t *rec;
        rec = realization->list[inx];
        product_key0_t product_key0;
        product_rec_t product;
        domino_marked_t marked;
        GtkTreeIter iter;
        char *name;
        char *purchase[4];
        char *real[4];
        double rest,rest_all;
        realiz_t month[4];
        realiz_t season[4];
        //if ( strcmp(rec->code, "19179" ) ) continue;//


        if ( !domino_order_get_marked(alias, domino_unit(), rec->code, &tm1_, &tm2_, &marked, &tm_mark) ) {
            retval = FALSE;
            break;
        }


        if ( skip_ordered &&
            ( marked == DOMINO_MARKED_AND_ACCEPT || marked == DOMINO_MARKED_AND_REJECT) )
            continue;

        /*if ( skip_def && marked == DOMINO_MARKED_AND_REJECT )
            continue;*/

        if ( !kind ) {

            rest = do_get_rest(alias, rec->code, domino_unit());

            if ( rest >= rec->quant )
                continue;
        }
        else
            rest = rec->quant;

        count++;


        product_key0.base_parcel = 0;
        do_text_set(alias, product_key0.code, rec->code);
        if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
            do_log(LOG_ERR, "Can't get product \"%s\"", rec->code);
            retval = FALSE;
            break;
        }
        name = do_text(alias, product.data.name);
        //price = do_product_price(alias, &product, 4)*do_product_coef(alias, &product);
        purchase_t last[4];
        get_purchase(alias, &product, domino_unit(), last);

        get_realiz(alias, &product, domino_unit(), month, season);


        rest /=do_product_coef(alias, &product);

        partner_key0_t partner_key0;
        partner_rec_t partner;
        do_cpy(partner_key0.g_code, product.data.mfr_region);
        partner_key0.code=product.data.mfr_code;
        if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) != DO_OK ) {
            do_log(LOG_ERR, "Can't get partner \"%s.%d\"", do_text(alias,partner_key0.g_code), partner_key0.code);
            retval = FALSE;
            break;
        }
        gchar *manufactor=do_text(alias,partner.data.name);



        time_t now;
        int year;
        struct tm tm;
        now = time(NULL);
        tm = *localtime(&now);
        year = tm.tm_year;
        rest_all = rest;
        for ( i = 0; i < units->count; i++ )
            if ( strcmp(units->list[i], domino_unit()) )
                rest_all += do_get_rest(alias, rec->code, units->list[i])/do_product_coef(alias, &product);
        for ( i = 0; i < 4; i++ ) {
            real[i] = g_strdup("");
            purchase[i] = g_strdup("");
        }
        int diff;
        for ( i = 0; i < 4; i++ ) {
            gchar *buf;
            if ( month[i].quant > 0 ) {
                buf = real[0];
                if ( year == month[i].tm.tm_year )
                    real[0] = g_strdup_printf("%s%s\n",buf, MonthFull[month[i].tm.tm_mon]);
                else
                    real[0] = g_strdup_printf("%s%s %d\n",buf, MonthShort[month[i].tm.tm_mon], month[i].tm.tm_year-100);
                g_free(buf);
                buf = real[1];
                if ( month[i].quant < 2 ) {
                    char *str;
                    str = do_rest_format(month[i].quant);
                    real[1] = g_strdup_printf("%s%s\n",buf, str);
                    g_free(str);
                }
                else
                    real[1] = g_strdup_printf("%s%d\n",buf, (int)month[i].quant);
                g_free(buf);
            }
            if ( season[i].quant > 0 ) {

                buf = real[2];
                real[2] = g_strdup_printf("%s%s\n",buf, season_name(i));
                g_free(buf);
                buf = real[3];
                if ( season[i].quant < 2 ) {
                    char *str;
                    str = do_rest_format(season[i].quant);
                    real[3] = g_strdup_printf("%s%s\n",buf, str);
                    g_free(str);
                }
                else
                    real[3] = g_strdup_printf("%s%d\n",buf, (int)season[i].quant);
                g_free(buf);
            }
            if ( last[i].quant > 0 ) {
                gchar *str;
                char date[50];
                buf = purchase[0];
                purchase[0] = g_strdup_printf("%s%s\n",buf, last[i].name);
                if ( !i || strcmp(last[i-1].name,last[i].name) )
                    diff = i;
                g_free(buf);
                buf = purchase[1];
                str = do_money_format(last[i].price, TRUE);
                purchase[1] = g_strdup_printf("%s%s\n",buf, str);
                g_free(buf);g_free(str);
                buf = purchase[2];

                purchase[2] = g_strdup_printf("%s%d\n",buf, (int)last[i].quant);
                g_free(buf);
                buf = purchase[3];
                strftime(date, sizeof(date)-1, "%d/%m/%y", &last[i].tm);
                purchase[3] = g_strdup_printf("%s%s\n",buf, date);
                g_free(buf);
            }
        }
        for ( i = 0; i < 4; i++ ) {
            if ( strlen(real[i]) > 0 )
                real[i][strlen(real[i])-1]='\0';
            if ( strlen(purchase[i]) > 0 ) {
                if ( !i && !diff )  {
                    do_free(purchase[0]);
                    purchase[0] = g_strdup(last[0].name);
                }
                else
                    purchase[i][strlen(purchase[i])-1]='\0';

            }
        }
        char *ads_long, *ch;
        char *ads;
        ads = do_product_param(alias, &product, "480");
        ads_long = NULL;
        ch = strchr(ads, '-');
        if ( ch ) {
            *ch = '\0';
            ads_long = ch+1;
        }
        ch = strchr(ads, ',');
        while ( ch ) {
            *ch = '\n';
            ch = strchr(ads, ',');
        }

        gtk_tree_store_append(GTK_TREE_STORE(priv->model), &iter, NULL);
        gtk_tree_store_set(GTK_TREE_STORE(priv->model), &iter, //!!
                    0 , marked ? marked : DOMINO_MARKED,
                    1 , rec->code,
                    2 , name,
                    3 , rest,
                    4 , purchase[0],
                    6, rec->quant/do_product_coef(alias, &product),
                    14,rest_all,
                    11 , purchase[1],
                    12 , purchase[2],
                    13 , purchase[3],
                    5, real[0],
                    8,real[1],
                    9,real[2],
                    10,real[3],
                    7, do_product_price(alias, &product, 4)*do_product_coef(alias, &product),
                    15, manufactor,
                    16, ads,
                    17, ads_long,

                    -1);
        if ( ads ) do_free(ads);
        g_free(manufactor);
        GtkTreeIter child;
        gtk_tree_store_append(GTK_TREE_STORE(priv->model), &child, &iter);
        gtk_tree_store_set(GTK_TREE_STORE(priv->model), &child,
                    2 , "-", -1);
        do_free(name);
        for ( i = 0; i < 4; i++ ) {
            do_free(real[i]);
            do_free(purchase[i]);
        }


    }

    gchar *text;
    text = g_strdup_printf("%d: позиций", count);
    gtk_label_set_text(GTK_LABEL(priv->footer), text);
    g_free(text);
    do_alias_free(alias);
    return retval;
}
static void		do_limit_view_view_do_edit(DoView *view, const gchar *tab)
{
    DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE(view);
    GtkTreeIter iter;
    GValue value = {0,};
    const gchar *code;
    GtkTreePath *path;
    GtkTreeView *tree_view;

	g_object_get(view, "tree-view", &tree_view, NULL);

    GList *list = gtk_tree_selection_get_selected_rows(gtk_tree_view_get_selection(tree_view), NULL);
    if ( !list )
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(tree_view), &path, NULL);
    else {
        path = gtk_tree_path_copy(list->data);
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    if (!path)
        return;

    gtk_tree_model_get_iter(priv->model, &iter, path);
    gtk_tree_model_get_value(priv->model, &iter, 1, &value);
    code = g_value_get_string(&value);


    DoContext *context = do_context_new();
    do_context_set_kind(context, DO_CONTEXT_KIND_PRODUCT);
    if (code)
        g_object_set(context, "product-code", code, NULL);

    DoView *edit = (DoView*)do_product_edit_new(DO_VIEW(view), priv->alias_name,
                                            DO_PRODUCT_EDIT_FLAGS_READ_ONLY,
                                               DO_DIALOG_ACTION_EDIT, context, domino_unit());

    do_notebook_add_tab(DO_NOTEBOOK(do_window_get_notebook(
	                                 DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))))),
                        edit, -1, TRUE);
    g_object_set(edit, "view", view, "tab", tab, NULL);

}
static gboolean	do_limit_view_view_can_do_edit(DoView *view, const gchar *tab)
{
    return TRUE;
}

static void	 do_limit_view_view_do_popup(DoView *view, GdkEventButton* event)
{
    GtkWidget * menu = do_view_actions_action_get_widget("/" DO_LIMIT_VIEW_POPUP);
    do_tree_view_popup(DO_TREE_VIEW(view), event, menu);
}

static void filter_changed(GtkComboBox *w, DoLimitView *view)
{
}
static void do_print_item(do_template_t *templ, do_template_cursor_t *line, GtkTreeModel *model, GtkTreeView *view, GtkTreePath *path);
static gchar *do_print_get_html(DoLimitView *view)
{
    DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE(view);
    do_template_t *templ;
    gchar *title;
    gchar *value;

    templ = do_template_new("order", TRUE);
    if ( !templ )
        return NULL;
    do_template_cursor_t *cursor;
    do_template_cursor_t *line;
    char buf[256], buf1[256], *v1,*v2;
    GDate *date1,*date2;
    int day, month, year;

    g_object_get(priv->date1, "day", &day, "month", &month, "year", &year, NULL);
    date1 = g_date_new_dmy(day, month, year);
    g_object_get(priv->date2, "day", &day, "month", &month, "year", &year, NULL);
    date2 = g_date_new_dmy(day, month, year);


    g_date_strftime(buf, 255, "%d/%m/%y", date1);
    g_date_strftime(buf1, 255, "%d/%m/%y", date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL, NULL);
    title = g_strdup_printf("Остатки меньше реализации <b>%s</b>", v2);
    g_free(v1);g_free(v2);
    cursor = do_template_cursor(templ);
    line   = do_template_section(templ, "Line");
    if ( !line ) {
        do_log(LOG_ERR, "Неправильный шаблон \"%s\"", "sale");
        do_template_free(templ);
        return NULL;
    }

    do_template_set(templ,
                    "Title", title,
                     NULL);
    do_template_cursor_run_step_for(cursor, "Line");

    GtkTreePath *path;
    path = gtk_tree_path_new_first();
    do_print_item(templ, line, GTK_TREE_MODEL(priv->model), priv->tree_view, path);
    gtk_tree_path_free(path);

    do_template_cursor_skip(cursor);
    do_template_cursor_run(cursor);

    value = do_template_text(templ);
    do_template_free(templ);
    return value;

}
static void  do_limit_view_do_print(DoView *view)
{
    //DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE(view);
    gchar *html_string = do_print_get_html(DO_LIMIT_VIEW(view));
    if (html_string) {
#ifndef MINIMAL_WIN
        GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(view));
        GtkWidget *html;
        GtkWidget *notebook;
        html = do_html_view_new();
        notebook = do_window_get_notebook(DO_WINDOW(window));
        do_notebook_add_tab(DO_NOTEBOOK(notebook), DO_VIEW(html), -1, TRUE);
        g_object_set(html, "html-string", html_string, "title", "Отчет", NULL);
        g_free(html_string);
#else
        do_show_html(html_string);
#endif
    }
}
static gboolean    do_limit_view_can_do_print(DoView *view)
{
    return TRUE;
}

static void  do_limit_view_do_print_now(DoView *view)

{
    //DoLimitViewPrivate *priv = DO_ORDER_VIEW_GET_PRIVATE(view);
    gchar *html_string = do_print_get_html(DO_LIMIT_VIEW(view));
    if (html_string) {
#ifndef MINIMAL_WIN
        GtkWidget *html;
        html = do_html_view_new();
        g_object_set(html, "html-string", html_string, "title", "Отчет", NULL);
        g_free(html_string);
        do_html_view_command_do_print_now(DO_VIEW(html));
#else
        do_show_html(html_string);
#endif
    }
}
static void do_print_item(do_template_t *templ, do_template_cursor_t *line, GtkTreeModel *model, GtkTreeView *view, GtkTreePath *path)
{
    GtkTreeIter iter;

    while ( gtk_tree_model_get_iter(model, &iter, path) ) {
        GValue desc = {0,}, price = {0,}, stock = {0,},
        purchase = {0,}, sale = {0,}, code = {0,};
        GtkTreeIter iter;
        gtk_tree_model_get_iter(model, &iter, path);
        gtk_tree_model_get_value(model, &iter, 1, &code);
        gtk_tree_model_get_value(model, &iter, 2, &desc);
        gtk_tree_model_get_value(model, &iter, 4 ,&purchase);
        gtk_tree_model_get_value(model, &iter, 5, &sale);
        gtk_tree_model_get_value(model, &iter, 7, &price);
        gtk_tree_model_get_value(model, &iter, 3, &stock);

        gchar *desc_str, *purchase_str, *sale_str, *code_str, *price_str, *stock_str;
        char **values;

        desc_str = g_value_dup_string(&desc);
        code_str = g_value_dup_string(&code);
        price_str = do_money_format(g_value_get_double(&price), FALSE);


        stock_str = do_rest_format(g_value_get_double(&stock));
        //purchase_str = g_value_dup_string(&purchase);
        values = g_strsplit(g_value_get_string(&purchase), "\n", -1);
        purchase_str = g_strjoinv("<br>",values);
        g_strfreev(values);
        values = g_strsplit(g_value_get_string(&sale), "\n", -1);
        sale_str = g_strjoinv("<br>",values);
        g_strfreev(values);

        do_template_set(templ, "Code", code_str,
                               "Desc", desc_str,
                               "Price", price_str,
                               "Stock", stock_str,
                               "Purchase", purchase_str,
                               "Sale", sale_str,
                               NULL);
        g_free(desc_str);g_free(purchase_str);g_free(sale_str);g_free(price_str);g_free(code_str);
        g_free(stock_str);

        do_template_cursor_first_run(line);
        gtk_tree_path_next(path);
    }
}

static void do_limit_view_row_expanded(GtkTreeView *tree_view, GtkTreeIter* iter, GtkTreePath *path, DoLimitView *view)
{
	DoLimitViewPrivate *priv = DO_LIMIT_VIEW_GET_PRIVATE (view);
    GValue value = {0,};
    GValue value_code = {0,};
    GValue value_name = {0,};
    GtkTreePath *path1;
    GtkTreeIter iter1;
    const gchar *code;
    const gchar *name;
    gchar *word;

    path1 = gtk_tree_path_copy(path);

    gtk_tree_path_down(path1);

    if ( !gtk_tree_model_get_iter(priv->model, &iter1, path1) ) {
        gtk_tree_path_free(path1);
        return;
    }
    gtk_tree_path_free(path1);

    gtk_tree_model_get_value(priv->model, &iter1, 1, &value);
    code = g_value_get_string(&value);
    if ( code != NULL )
        return;

    gtk_tree_model_get_value(priv->model, iter, 2, &value_name);
    gtk_tree_model_get_value(priv->model, iter, 1, &value_code);
    name = g_value_get_string(&value_name);

    gunichar  out;
    gchar *p;
    for (p = (gchar*)name; p && *p !='\0'; p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        if ( g_unichar_isspace(out) )
            break;
    }
    word = g_strdup(name);
    word[p-name] = '\0';

    product_view_key0_t product_view_key0,key;
    product_rec_t product;
    product_key0_t product_key0;
    int status;
    int len;
    do_alias_t *alias;

    alias = domino_alias_new(priv->alias_name);
    if ( !alias )
        return;
    if ( !do_alias_open(alias, TRUE) ) {
        do_alias_free(alias);
        return;
    }

    do_text_set_empty(product_view_key0.sklad);
    do_text_set_empty(product_view_key0.code);
    product_view_key0.type = 1;
    do_text_set(alias, product_view_key0.sort, word);
    do_cpy(key.sort, product_view_key0.sort);

    for ( len = 0; len < sizeof(key.sort); len++ )
        if ( key.sort[len] == ' ' ) break;

    status = do_product_view_key0(alias, &product_view_key0, DO_GET_GE);


    struct tm tm1;
    struct tm tm2;
    struct tm tm_mark;
    int day, month, year;

    g_object_get(priv->date1, "day", &day, "month", &month, "year", &year, NULL);
    do_date_set_ymd(&tm1, year, month, day);
    g_object_get(priv->date2, "day", &day, "month", &month, "year", &year, NULL);
    do_date_set_ymd(&tm2, year, month, day);

    int firm;
    gboolean first = TRUE;
    firm = do_firm_get_firm_by_unit(alias, domino_unit());
    do_string_list_t *units = do_firm_get_units(alias, firm);

    while ( status == DO_OK ) {
        if ( strncmp(key.sort, product_view_key0.sort, len) || product_view_key0.type != 1 ) break;

        product_key0.base_parcel = 0;
        do_cpy(product_key0.code, product_view_key0.code);

        if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
            do_log(LOG_ERR, "Товар \"%s\" не найден", do_text(alias, product_key0.code));
            break;
        }
        gchar *code;
        gchar *name;
        double rest_all,rest;
        double price;
        double coef;
        //int i;
        domino_marked_t marked;

        coef = do_product_coef(alias, &product);
        price = do_product_price(alias, &product, 4);
        rest_all = 0;
        rest = 0;
        code = do_text(alias, product.data.code);
        if ( strcmp(code, g_value_get_string(&value_code)) ) {

            name = do_text(alias, product.data.name);

            rest = do_get_rest(alias, code, domino_unit());
            /*for ( i = 0; i < units->count; i++ )
                if ( strcmp(units->list[i], domino_unit()) )
                    rest = do_get_rest(alias, code, units->list[i]);
                rest_all += do_get_rest(alias, code, units->list[i]);*/


            domino_order_get_marked_(priv->alias_name, domino_unit(), code, &tm1, &tm2, &marked, &tm_mark);
            if ( first )
                first = FALSE;
            else
                gtk_tree_store_append(GTK_TREE_STORE(priv->model), &iter1, iter);
            gtk_tree_store_set(GTK_TREE_STORE(priv->model), &iter1,
                        0 , marked,
                        1 , code,
                        14 , rest_all/coef,
                        2 , name,
                        3 , rest/coef,
                        7, price*coef,
                        -1);
            do_free(name);
        }
        do_free(code);
        status = do_product_view_key0(alias, &product_view_key0, DO_GET_NEXT);
    }
    if ( first )
        gtk_tree_store_remove(GTK_TREE_STORE(priv->model), &iter1);
    do_string_list_free(units);
    do_alias_free(alias);

}
