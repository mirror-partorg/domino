
#include "do_sale_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_sale_dialog.h"
#include "domino.h"
#include "do_tree_view.h"
#include "../misc/template.h"
#include <domino.h>
#include "hig.h"
#include "do_icons.h"
#include "do_utilx.h"
#include <gdk/gdk.h>
#include <time.h>

#define DO_SALE_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_SALE_VIEW, DoSaleViewPrivate))

#define OBJECT_ROOT_PATH "SaleView"
#define DEFAULT_COL_PACK_FORM_WIDTH 400
#define DEFAULT_COL_DEFAULT_WIDTH 60

// View interface
static void do_sale_view_view_init(DoViewIface *iface);

static gboolean    do_sale_view_view_do_grab_focus(DoView *view);
static void	       do_sale_view_view_do_close(DoView *view);
static const char *do_sale_view_view_get_title(DoView *view);
static GdkPixbuf  *do_sale_view_view_get_icon(DoView *view);
static gboolean    do_sale_view_fill_model(DoSaleView *view);
static gboolean    do_sale_view_can_do_print(DoView *view);
static void        do_sale_view_do_print(DoView *view);
static void        do_sale_view_do_print_now(DoView *view);
void do_sale_view_data_func_description(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         DoSaleView *view);

enum
{
	PROP_0,
	PROP_UNITS,
	PROP_DATE1,
	PROP_DATE2,
};

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

struct _DoSaleViewPrivate
{
    GtkTreeStore        *model;
    GtkTreeView         *tree_view;
    GtkTreeViewColumn   **columns;
    GtkTreeSelection    *selection;
    GDate               date1;
    GDate               date2;
    gint                postponement;
    //GtkWidget           *footer;
    GtkWidget           *header;
    GdkPixbuf           *icon;
    gchar               *units;
};

G_DEFINE_TYPE_WITH_CODE (DoSaleView, do_sale_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_sale_view_view_init)
                                    );

static void do_sale_view_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_sale_view_view_do_grab_focus;
    iface->do_close      = do_sale_view_view_do_close;
    iface->get_title     = do_sale_view_view_get_title;
    iface->get_icon      = do_sale_view_view_get_icon;
    iface->do_print      = do_sale_view_do_print;
    iface->do_print_now      = do_sale_view_do_print_now;
    iface->can_do_print  = do_sale_view_can_do_print;

    //iface->do_edit       = do_sale_view_view_do_edit;
    //iface->can_do_edit   = do_sale_view_view_can_do_edit;
    //iface->do_delete       = do_sale_view_view_do_delete;
    //iface->do_insert       = do_sale_view_view_do_insert;
    //iface->can_do_delete   = do_sale_view_view_can_do_delete;
    //iface->can_do_insert   = do_sale_view_view_can_do_insert;
}

static void do_sale_view_init(DoSaleView *view)
{
#if GTK_MAJOR_VERSION == 2
	DoSaleViewPrivate *priv = DO_SALE_VIEW_GET_PRIVATE (view);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_FILE, DO_VIEW_ICON_SIZE, 0, NULL);
#endif
}

static GtkTreeStore *do_sale_view_create_model(DoSaleView *embed)
{

    gint count =  7;

    GtkTreeStore *model;
    GType *types;

    types = g_new(GType, count);
    types[0] = G_TYPE_STRING;
    types[1] = G_TYPE_DOUBLE;
    types[2] = G_TYPE_DOUBLE;
    types[3] = G_TYPE_DOUBLE;
    types[4] = G_TYPE_DOUBLE;
    types[5] = G_TYPE_DOUBLE;
    types[6] = G_TYPE_INT;


    model = gtk_tree_store_newv(count, types);

    return model;
}
static void do_sale_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoSaleView *view)
{
    if ( gtk_tree_view_row_expanded(tree_view, path) )
        gtk_tree_view_collapse_row(tree_view, path);
    else
        gtk_tree_view_expand_row(tree_view, path, FALSE);
}

static GObject *do_sale_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkTreeStore        *model;
    DoTreeView          *view;
	DoSaleView      *embed;
	GtkTreeView         *tree_view;
	DoSaleViewPrivate   *priv;

	object = G_OBJECT_CLASS (do_sale_view_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_SALE_VIEW_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_SALE_VIEW(object);
	g_object_get(object, "tree-view", &tree_view, NULL);

    g_signal_connect (tree_view, "row-activated", G_CALLBACK(do_sale_view_row_activated), object);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(tree_view), FALSE);

	priv->tree_view = tree_view;
    priv->selection = gtk_tree_view_get_selection(tree_view);

	priv->model = model = do_sale_view_create_model(embed);
	if (model)
        g_object_set(view, "model", model,
//                           "fixed-height-mode", FALSE,
                           NULL);

    col = do_tree_view_add_column(view, "Desc", "Аптека", 300);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_DESC);
    gtk_tree_view_column_add_attribute (col, renderer, "markup", COLUMN_DESC);
    /*gtk_tree_view_column_set_cell_data_func(col ,renderer,
             (GtkTreeCellDataFunc)do_sale_view_data_func_description, object, NULL);*/

    col = do_tree_view_add_column(view, "Purchase", "Закуп", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_PURCHASE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_PURCHASE);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_money_bold_depth1, GINT_TO_POINTER(COLUMN_PURCHASE), NULL);

    col = do_tree_view_add_column(view, "Sale", "Продажа", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_SALE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_SALE);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_money_bold_depth1, GINT_TO_POINTER(COLUMN_SALE), NULL);


    col = do_tree_view_add_column(view, "Profit", "Наценка", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_PROFIT);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_PROFIT);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_money_bold_depth1, GINT_TO_POINTER(COLUMN_PROFIT), NULL);

    col = do_tree_view_add_column(view, "ProfitPurchase", "Наценка\n%", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_PROFIT_PURCHASE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_PROFIT_PURCHASE);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_percent_bold_depth1, GINT_TO_POINTER(COLUMN_PROFIT_PURCHASE), NULL);


    col = do_tree_view_add_column(view, "ProfitSale", "Валовая\n%", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_PROFIT_SALE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_PROFIT_SALE);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_percent_bold_depth1, GINT_TO_POINTER(COLUMN_PROFIT_SALE), NULL);

    col = do_tree_view_add_column(view, "Checks", "Покупателей", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_CHECK);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_CHECK);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_big_int_bold_depth1, GINT_TO_POINTER(COLUMN_CHECK), NULL);


    priv->header = gtk_label_new("");
    g_object_set(priv->header, "xalign", 0.0, NULL);
    do_tree_view_pack_header(view, GTK_WIDGET(priv->header));
    gtk_widget_show(GTK_WIDGET(priv->header));

    /*priv->footer = gtk_label_new("");
    g_object_set(priv->footer, "xalign", 0.01, NULL);
    do_tree_view_pack_footer(view, GTK_WIDGET(priv->footer));
    gtk_widget_show(GTK_WIDGET(priv->footer));*/


    gtk_tree_view_set_search_column(tree_view, 0);
	gtk_widget_show(GTK_WIDGET(object));

    do_sale_view_fill_model(embed);

	return object;
}

static void do_sale_view_finalize (GObject *object)
{
    DoSaleViewPrivate *priv;
	priv = DO_SALE_VIEW_GET_PRIVATE (object);
    g_free(priv->units);

	G_OBJECT_CLASS (do_sale_view_parent_class)->finalize (object);
}

static void do_sale_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoSaleViewPrivate *priv = DO_SALE_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_UNITS:
            g_value_set_string (value, priv->units);
			break;
        case PROP_DATE1:
            g_value_set_boxed(value, &priv->date1);
            break;
        case PROP_DATE2:
            g_value_set_boxed(value, &priv->date2);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
	/* no readable properties */
	g_assert_not_reached ();
}
static void do_sale_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoSaleViewPrivate *priv = DO_SALE_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_UNITS:
            if (!priv->units)
                priv->units = g_value_dup_string (value);
			break;
        case PROP_DATE1: {
            GDate *date;
            date = g_value_get_boxed(value);
            priv->date1 = *date;
            break;
        }
        case PROP_DATE2: {
            GDate *date;
            date = g_value_get_boxed(value);
            priv->date2 = *date;
            break;
        }

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
}

static void do_sale_view_class_init (DoSaleViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_sale_view_constructor;
	o_class->finalize     = do_sale_view_finalize;
	o_class->get_property = do_sale_view_get_property;
	o_class->set_property = do_sale_view_set_property;

	g_type_class_add_private (o_class, sizeof (DoSaleViewPrivate));

    g_object_class_install_property
        (o_class,
         PROP_UNITS,
         g_param_spec_string("units",
                      "Список подразделений",
                      "Список подразделений",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY ));

    g_object_class_install_property (o_class,
				   PROP_DATE1,
				   g_param_spec_boxed  ("date1",
 						      "Начальная дата",
 						      "Дата начальная",
 						      G_TYPE_DATE,
 						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY ));
    g_object_class_install_property (o_class,
				   PROP_DATE2,
				   g_param_spec_boxed  ("date2",
 						      "Конечная дата",
 						      "Дата конечная",
 						      G_TYPE_DATE,
 						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY ));

}

GtkWidget *do_sale_view_new (const gchar *units, GDate *date1, GDate *date2)
{
	return g_object_new (DO_TYPE_SALE_VIEW,
			     "units", units,
			     "date1", date1,
			     "date2", date2,
			     "object-root-path", OBJECT_ROOT_PATH,
			     (gpointer) NULL);
}
static gboolean do_sale_view_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void	do_sale_view_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_sale_view_view_get_title(DoView *view)
{
    DoSaleViewPrivate *priv = DO_SALE_VIEW_GET_PRIVATE(view);
    gchar *retval;
    gchar buf[256],buf1[256];
    gchar *v1,*v2;
    g_date_strftime(buf, 255, "%d/%m/%Y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%Y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL , NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL , NULL);
    retval = g_strdup_printf("Выручка за период с %s по %s", v1, v2);
    g_free(v1);g_free(v2);
    return retval;
}
static GdkPixbuf  *do_sale_view_view_get_icon(DoView *view)
{
    DoSaleViewPrivate *priv = DO_SALE_VIEW_GET_PRIVATE(view);
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

static gboolean do_sale_view_get_sale(do_alias_period_t *alias_period, GSList **sale, const gchar *unit);

static const gchar *desc_name[3] = { "НДС 0%", "НДС 10%", "НДС 20%" };

typedef struct {
    GtkTreeIter iter;
    sale_t sale;
} otdel_sale_t;

static gboolean do_sale_view_fill_model(DoSaleView *view)
{
    DoSaleViewPrivate *priv = DO_SALE_VIEW_GET_PRIVATE(view);
    gint i;
    do_string_list_t *units;
    gboolean retval = TRUE;
    GSList *sales = NULL;
    struct tm bd;
    struct tm ed;

    do_start_long_operation(GTK_WIDGET(view));

    char *header_mark;
    gchar buf[256],buf1[256];
    gchar *v1,*v2;

    do_meta_alias_t *meta;

    meta = domino_meta_alias_new();
    if ( !meta ) {
        do_end_long_operation(GTK_WIDGET(view));
        return FALSE;
    }

    g_date_to_struct_tm(&priv->date1, &bd);
    g_date_to_struct_tm(&priv->date2, &ed);

    units = do_string_list_new_from_csv(priv->units);
    for ( i = 0; retval && i < units->count; i++ ) {
        do_sort_list_t *aliases;
        int j;
        aliases = domino_meta_alias_get_list_from_sklad(meta, units->list[i], &bd, &ed);
        for ( j = 0 ; retval && j < aliases->count; j++ ) {
            do_alias_period_t *p;
            p = aliases->list[j];
            if ( (retval = do_alias_open(p->alias, TRUE)) != TRUE )
                break;
            retval = do_sale_view_get_sale(p, &sales, units->list[i]);
        }

    }
    do_string_list_free(units);
    domino_meta_alias_free(meta);

    g_date_strftime(buf, 255, "%d/%m/%y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL, NULL);


    header_mark = g_strdup_printf("Выручка за период с <b>%s</b> по <b>%s</b>, Аптеки: <b>%s</b>",
                                  v1, v2, do_get_unit_desc(NULL,priv->units) );
    g_free(v1);g_free(v2);
    gtk_label_set_markup(GTK_LABEL(priv->header), header_mark);

    do_end_long_operation(GTK_WIDGET(view));
    if ( retval ) {
        GSList *l;
        sale_t root;
        gdouble profit;
        gdouble profit_purchase;
        gdouble profit_sale;
        int i;


        memset(&root, 0, sizeof(root));

        GtkTreeIter iter_root;
        gtk_tree_store_clear(priv->model);


        GList *otdels = NULL, *o;

        for ( l = sales; l; l=l->next ) {
            otdel_sale_t *otd;
            gboolean new_;
            sale_t *sale;
            sale = l->data;
            new_ = TRUE;
            for ( o = otdels; o; o=o->next ) {
                otd = o->data;
                if ( !strcmp(otd->sale.name, sale->otdel_name) ) {
                    for ( i = 0; i < 3; i++ ) {
                        otd->sale.sale[i]+=sale->sale[i];
                        otd->sale.purchase[i]+=sale->purchase[i];
                    }
                        otd->sale.checks += sale->checks;
                    new_ = FALSE;
                    break;
                }
            }
            if ( new_ ) {
               otd = g_new0(otdel_sale_t, 1);
               otdels = g_list_append(otdels, otd);
               otd->sale.name = sale->otdel_name;
                for ( i = 0; i < 3; i++ ) {
                    otd->sale.sale[i]=sale->sale[i];
                    otd->sale.purchase[i]=sale->purchase[i];
                }
               otd->sale.checks = sale->checks;
            }
        }
        for ( o = otdels; o; o=o->next ) {
            otdel_sale_t *otd;
            sale_t *sale;

            otd = o->data;
            sale = &otd->sale;

            gtk_tree_store_append(priv->model, &otd->iter, NULL);
            profit = (sale->sale[0]+sale->sale[1]+sale->sale[2])-
                     (sale->purchase[0]+sale->purchase[1]+sale->purchase[2]);

            profit_sale = profit/(sale->sale[0]+sale->sale[1]+sale->sale[2]);
            profit_purchase = profit/(sale->purchase[0]+sale->purchase[1]+sale->purchase[2]);

            gtk_tree_store_set(priv->model, &otd->iter,
                               COLUMN_DESC, sale->name,
                               COLUMN_PURCHASE, (sale->purchase[0]+sale->purchase[1]+sale->purchase[2]),
                               COLUMN_SALE, (sale->sale[0]+sale->sale[1]+sale->sale[2]),
                               COLUMN_PROFIT, profit,
                               COLUMN_PROFIT_PURCHASE, profit_purchase,
                               COLUMN_PROFIT_SALE, profit_sale,
                               COLUMN_CHECK, sale->checks,
                               -1);

        }
        for ( l = sales; l; l=l->next ) {
            GtkTreeIter iter_unit;
            sale_t *sale;
            otdel_sale_t *otd;

            sale = l->data;
            otd = NULL;

            for ( o = otdels; o; o=o->next ) {
                otd = o->data;
                if ( !strcmp(otd->sale.name, sale->otdel_name) ) {
                    break;
                }
            }

            gtk_tree_store_append(priv->model, &iter_unit, otd ? &otd->iter : NULL);
            profit = (sale->sale[0]+sale->sale[1]+sale->sale[2])-
                     (sale->purchase[0]+sale->purchase[1]+sale->purchase[2]);

            profit_sale = profit/(sale->sale[0]+sale->sale[1]+sale->sale[2]);
            profit_purchase = profit/(sale->purchase[0]+sale->purchase[1]+sale->purchase[2]);

            gtk_tree_store_set(priv->model, &iter_unit,
                               COLUMN_DESC, sale->name,
                               COLUMN_PURCHASE, (sale->purchase[0]+sale->purchase[1]+sale->purchase[2]),
                               COLUMN_SALE, (sale->sale[0]+sale->sale[1]+sale->sale[2]),
                               COLUMN_PROFIT, profit,
                               COLUMN_PROFIT_PURCHASE, profit_purchase,
                               COLUMN_PROFIT_SALE, profit_sale,
                               COLUMN_CHECK, sale->checks,
                               -1);

            for ( i = 0; i < 3; i++ ) {
                GtkTreeIter iter;
                profit = (sale->sale[i])-
                         (sale->purchase[i]);

                profit_sale = profit/sale->sale[i];
                profit_purchase = profit/sale->purchase[i];

                gtk_tree_store_append(priv->model,&iter, &iter_unit);
                gtk_tree_store_set(priv->model, &iter,
                                   COLUMN_DESC, desc_name[i],
                                   COLUMN_PURCHASE, sale->purchase[i],
                                   COLUMN_SALE, sale->sale[i],
                                   COLUMN_PROFIT, profit,
                                   COLUMN_PROFIT_PURCHASE, profit_purchase,
                                   COLUMN_PROFIT_SALE, profit_sale,
                                   -1
                                   );
                root.purchase[i] += sale->purchase[i];
                root.sale[i] += sale->sale[i];
            }
            root.checks += sale->checks;
        }
        profit = (root.sale[0]+root.sale[1]+root.sale[2])-
                 (root.purchase[0]+root.purchase[1]+root.purchase[2]);

        profit_sale = profit/(root.sale[0]+root.sale[1]+root.sale[2]);
        profit_purchase = profit/(root.purchase[0]+root.purchase[1]+root.purchase[2]);

        gtk_tree_store_append(priv->model, &iter_root, NULL);
        gtk_tree_store_set(priv->model, &iter_root,
                               COLUMN_DESC, "<b>По всем аптекам</b>",
                               COLUMN_PURCHASE, (root.purchase[0]+root.purchase[1]+root.purchase[2]),
                               COLUMN_SALE, (root.sale[0]+root.sale[1]+root.sale[2]),
                               COLUMN_PROFIT, profit,
                               COLUMN_PROFIT_PURCHASE, profit_purchase,
                               COLUMN_PROFIT_SALE, profit_sale,
                               COLUMN_CHECK, root.checks,
                           -1
                           );
        for ( i = 0; i < 3; i++ ) {
            GtkTreeIter iter;
            profit = (root.sale[i])-
                     (root.purchase[i]);

            profit_sale = profit/root.sale[i];
            profit_purchase = profit/root.purchase[i];


            gtk_tree_store_append(priv->model, &iter, &iter_root);
            gtk_tree_store_set(priv->model, &iter,
                               COLUMN_DESC, desc_name[i],
                               COLUMN_PURCHASE, root.purchase[i],
                               COLUMN_SALE, root.sale[i],
                               COLUMN_PROFIT, profit,
                               COLUMN_PROFIT_PURCHASE, profit_purchase,
                               COLUMN_PROFIT_SALE, profit_sale,
                               -1
                               );
        }

        for ( o = otdels; o; o=o->next ) {
            otdel_sale_t *otd;
            otd = o->data;
            GtkTreePath *path;
            path = gtk_tree_model_get_path(GTK_TREE_MODEL(priv->model), &otd->iter);
            gtk_tree_view_expand_row(priv->tree_view, path, FALSE);
        }
    }

    if ( retval ) {
        /*GtkTreePath *path;
        path = gtk_tree_path_new_first();
        gtk_tree_view_expand_row(priv->tree_view, path, FALSE);*/
    }

    return retval;
}

static gboolean do_sale_view_get_sale(do_alias_period_t *alias_period, GSList **sales, const gchar *unit)
{

    do_alias_t *alias = alias_period->alias;

    document_key3_t document_key3, key;
    document_rec_t document;
    int status;
    int indx;
    otdel_rec_t otdel;
    otdel_key0_t otdel_key0;
    do_text_set_empty(otdel_key0.code);
    status = do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return FALSE;
    char *otdel_name;
    otdel_name = do_text(alias, otdel.data.name);

    for ( indx = 1; indx >= 0; indx-- ) {

        if ( !indx )
            do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
        else
            do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));

        do_text_set(alias, document_key3.sklad, unit);
        do_date_set(&document_key3.date, alias_period->date_b);
        do_date_set(&key.date, alias_period->date_e);
        document_key3.time = 0;

        do_cpy(key.dtype, document_key3.dtype);
        do_cpy(key.sklad, document_key3.sklad);

        status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);

        while ( status == DO_OK ) {
            int is_need;
            if (
                 do_cmp(key.dtype, document_key3.dtype) ||
                 do_cmp(key.sklad, document_key3.sklad) ||
                 document_key3.date > key.date
                ) break;

            is_need = FALSE;
            if ( !indx )
                is_need = do_text_in_list(alias, document.data.dtype1, "20", NULL);
            else
                is_need = do_text_in_list(alias, document.data.dtype1, "01","02","03", "04", NULL);

            if ( is_need &&
                 document.data.accepted ) {
                sale_t *item;
                GSList *l;
                item = NULL;

                for ( l = *sales; l; l=l->next )
                    if ( !do_cmp(document.data.sklad, ((sale_t*)l->data)->sklad_key0.code)
                ) {
                        item = l->data;
                        break;
                    }
                if ( !item ) {
                    sklad_rec_t sklad;
                    item = g_new0(sale_t, 1);
                    item->otdel_name = do_strdup(otdel_name);
                    do_cpy(item->sklad_key0.code, document.data.sklad);
                    status = do_sklad_get0(alias, &sklad, &item->sklad_key0, DO_GET_EQUAL);
                    if ( status != DO_OK ) {
                        if ( status == DO_KEY_NOT_FOUND ) {
                            do_log(LOG_ERR, "sklad \"%s\" not found", unit);
                            status = DO_ERROR;
                            break;
                        }
                    }
                    item->name = do_text(alias, sklad.data.name);
                    *sales = g_slist_append(*sales, item);

                }
                if ( !indx )
                {
                    item->purchase[0] -= do_document_sum(alias, &document, 11000);
                    item->purchase[2] -= do_document_sum(alias, &document, 11020);
                    item->purchase[1] -= do_document_sum(alias, &document, 11)
                               - (do_document_sum(alias, &document, 11000)+
                                  do_document_sum(alias, &document, 11020));
                }
                else {
                    item->purchase[0] += do_document_sum(alias, &document, 11000);
                    item->purchase[2] += do_document_sum(alias, &document, 11020);
                    item->purchase[1] += do_document_sum(alias, &document, 11)
                                      - (do_document_sum(alias, &document, 11000)+do_document_sum(alias, &document, 11020));


                    item->sale[0] += do_document_sum(alias, &document, 9000);
                    item->sale[1] += do_document_param_float(alias, &document, "214") -(do_document_sum(alias, &document, 9000)+do_document_sum(alias, &document, 9020));
                    item->sale[2] += do_document_sum(alias, &document, 9020);
                    int checks;
                    checks = do_document_param_int(alias, &document, "215");

                    item->checks += checks;
                }

            }
            status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
        }

        if ( status == DO_ERROR )
            return FALSE;
    }
    do_free(otdel_name);
    return TRUE;
}
static void do_print_item(do_template_t *templ, do_template_cursor_t *line, GtkTreeModel *model, GtkTreeView *view, GtkTreePath *path);
static gchar *do_print_get_html(DoSaleView *view)
{
    DoSaleViewPrivate *priv = DO_SALE_VIEW_GET_PRIVATE(view);
    do_template_t *templ;
    gchar *title;
    gchar *value;

    templ = do_template_new("sale", TRUE);
    if ( !templ )
        return NULL;
    do_template_cursor_t *cursor;
    do_template_cursor_t *line;
    char buf[256], buf1[256], *v1,*v2;
    g_date_strftime(buf, 255, "%d/%m/%y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL, NULL);
    title = g_strdup_printf("Выручка за период с <b>%s</b> по <b>%s</b>",
                                  v1, v2);
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
                    "Units", do_get_unit_desc(NULL,priv->units), NULL);
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
static void  do_sale_view_do_print(DoView *view)
{
    //DoSaleViewPrivate *priv = DO_SALE_VIEW_GET_PRIVATE(view);
    gchar *html_string = do_print_get_html(DO_SALE_VIEW(view));
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
static gboolean    do_sale_view_can_do_print(DoView *view)
{
    return TRUE;
}
static void  do_sale_view_do_print_now(DoView *view)
{
    //DoSaleViewPrivate *priv = DO_SALE_VIEW_GET_PRIVATE(view);
    gchar *html_string = do_print_get_html(DO_SALE_VIEW(view));
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
        GValue desc = {0,}, purchase = {0,}, sale = {0,}, profit = {0,},
               profit_purchase = {0,}, profit_sale = {0,}, checks = {0,};
        GtkTreeIter iter;
        gtk_tree_model_get_iter(model, &iter, path);
        gtk_tree_model_get_value(model, &iter, COLUMN_DESC, &desc);
        gtk_tree_model_get_value(model, &iter, COLUMN_PURCHASE, &purchase);
        gtk_tree_model_get_value(model, &iter, COLUMN_SALE, &sale);
        gtk_tree_model_get_value(model, &iter, COLUMN_PROFIT, &profit);
        gtk_tree_model_get_value(model, &iter, COLUMN_PROFIT_PURCHASE, &profit_purchase);
        gtk_tree_model_get_value(model, &iter, COLUMN_PROFIT_SALE, &profit_sale);
        gtk_tree_model_get_value(model, &iter, COLUMN_CHECK, &checks);

        gchar *desc_str, *purchase_str, *sale_str, *profit_str, *profit_purchase_str, *profit_sale_str, *checks_str;

        desc_str = g_value_dup_string(&desc);
        purchase_str = do_money_format(g_value_get_double(&purchase), FALSE);
        sale_str = do_money_format(g_value_get_double(&sale), FALSE);
        profit_str = do_money_format(g_value_get_double(&profit), FALSE);
        profit_purchase_str = do_percent_format(g_value_get_double(&profit_purchase)*100);
        profit_sale_str = do_percent_format(g_value_get_double(&profit_sale)*100);
        checks_str = do_big_int_format(g_value_get_int(&checks));

        do_template_set(templ, "Desc", desc_str,
                               "Purchase", purchase_str,
                               "Sale", sale_str,
                               "Profit", profit_str,
                               "ProfitPurchase", profit_purchase_str,
                               "ProfitSale", profit_sale_str,
                               "Checks",checks_str,
                               NULL);
        g_free(desc_str);g_free(purchase_str);g_free(sale_str);g_free(profit_str);g_free(profit_purchase_str);
        g_free(profit_sale_str); g_free(checks_str);

        do_template_cursor_first_run(line);
        if ( gtk_tree_view_row_expanded(view, path) ) {
            GtkTreePath *new_path;
            new_path = gtk_tree_path_copy(path);
            gtk_tree_path_down(new_path);
            do_print_item(templ, line, model, view, new_path);
            gtk_tree_path_free(new_path);
        }
        gtk_tree_path_next(path);
    }
}
