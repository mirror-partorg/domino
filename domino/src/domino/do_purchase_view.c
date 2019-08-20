
#include "do_purchase_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_purchase_dialog.h"
#include "domino.h"
#include "do_tree_view.h"
#include "../misc/template.h"
#include <domino.h>
#include "hig.h"
#include "do_icons.h"
#include <gdk/gdk.h>
#include <time.h>

#define DO_PURCHASE_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_PURCHASE_VIEW, DoPurchaseViewPrivate))

#define OBJECT_ROOT_PATH "PurchaseView"
#define DEFAULT_COL_PACK_FORM_WIDTH 400
#define DEFAULT_COL_DEFAULT_WIDTH 60

// View interface
static void do_purchase_view_view_init(DoViewIface *iface);

static gboolean    do_purchase_view_view_do_grab_focus(DoView *view);
static void	       do_purchase_view_view_do_close(DoView *view);
static const char *do_purchase_view_view_get_title(DoView *view);
static GdkPixbuf  *do_purchase_view_view_get_icon(DoView *view);
static gboolean    do_purchase_view_fill_model(DoPurchaseView *view);
static gboolean    do_purchase_view_can_do_print(DoView *view);
static void        do_purchase_view_do_print(DoView *view);
static void        do_purchase_view_do_print_now(DoView *view);
void do_purchase_view_data_func_description(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         DoPurchaseView *view);

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
    COLUMN_TOTAL,
    COLUMN_QUANT,
    COLUMN_PURCHASE,
    COLUMN_RETAIL,
    COLUMN_PERCENT,
    COLUMN_NO ,

};

struct _DoPurchaseViewPrivate
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

G_DEFINE_TYPE_WITH_CODE (DoPurchaseView, do_purchase_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_purchase_view_view_init)
                                    );

static void do_purchase_view_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_purchase_view_view_do_grab_focus;
    iface->do_close      = do_purchase_view_view_do_close;
    iface->get_title     = do_purchase_view_view_get_title;
    iface->get_icon      = do_purchase_view_view_get_icon;
    iface->do_print      = do_purchase_view_do_print;
    iface->do_print_now      = do_purchase_view_do_print_now;
    iface->can_do_print  = do_purchase_view_can_do_print;

    //iface->do_edit       = do_purchase_view_view_do_edit;
    //iface->can_do_edit   = do_purchase_view_view_can_do_edit;
    //iface->do_delete       = do_purchase_view_view_do_delete;
    //iface->do_insert       = do_purchase_view_view_do_insert;
    //iface->can_do_delete   = do_purchase_view_view_can_do_delete;
    //iface->can_do_insert   = do_purchase_view_view_can_do_insert;
}

static void do_purchase_view_init(DoPurchaseView *view)
{
#if GTK_MAJOR_VERSION == 2
	DoPurchaseViewPrivate *priv = DO_PURCHASE_VIEW_GET_PRIVATE (view);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_FILE, DO_VIEW_ICON_SIZE, 0, NULL);
#endif
}

static GtkTreeStore *do_purchase_view_create_model(DoPurchaseView *embed)
{

    gint count =  6;

    GtkTreeStore  *model;
    GType *types;

    types = g_new(GType, count);
/*    types[0] = GTK_TYPE_STRING;
    types[1] = GTK_TYPE_INT;
    types[2] = GTK_TYPE_INT;
    types[3] = GTK_TYPE_DOUBLE;
    types[4] = GTK_TYPE_DOUBLE;
    types[5] = GTK_TYPE_DOUBLE;*/
    types[0] = G_TYPE_STRING;
    types[1] = G_TYPE_INT;
    types[2] = G_TYPE_INT;
    types[3] = G_TYPE_DOUBLE;
    types[4] = G_TYPE_DOUBLE;
    types[5] = G_TYPE_DOUBLE;

    model = gtk_tree_store_newv(count, types);

    return model;
}
static void do_purchase_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoPurchaseView *view)
{
    if ( gtk_tree_view_row_expanded(tree_view, path) )
        gtk_tree_view_collapse_row(tree_view, path);
    else
        gtk_tree_view_expand_row(tree_view, path, FALSE);
}

static GObject *do_purchase_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkTreeStore        *model;
    DoTreeView          *view;
	DoPurchaseView      *embed;
	GtkTreeView         *tree_view;
	DoPurchaseViewPrivate   *priv;

	object = G_OBJECT_CLASS (do_purchase_view_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_PURCHASE_VIEW_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_PURCHASE_VIEW(object);
	g_object_get(object, "tree-view", &tree_view, NULL);

    g_signal_connect (tree_view, "row-activated", G_CALLBACK(do_purchase_view_row_activated), object);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(tree_view), FALSE);

	priv->tree_view = tree_view;
    priv->selection = gtk_tree_view_get_selection(tree_view);

	priv->model = model = do_purchase_view_create_model(embed);
	if (model)
        g_object_set(view, "model", model,
//                           "fixed-height-mode", FALSE,
                           NULL);

    col = do_tree_view_add_column(view, "Desc", "Наименование", 300);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_DESC);
    gtk_tree_view_column_add_attribute (col, renderer, "markup", COLUMN_DESC);

    col = do_tree_view_add_column(view, "Total", "Приходов", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_TOTAL);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_TOTAL);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_big_int, GINT_TO_POINTER(COLUMN_TOTAL), NULL);


    col = do_tree_view_add_column(view, "Quant", "Единиц", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_QUANT);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_QUANT);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_big_int, GINT_TO_POINTER(COLUMN_QUANT), NULL);

    col = do_tree_view_add_column(view, "Retail", "Розница", 100);


    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_RETAIL);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_RETAIL);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_money, GINT_TO_POINTER(COLUMN_RETAIL), NULL);

    col = do_tree_view_add_column(view, "Percent", "Наценка", 100);


    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_PERCENT);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_PERCENT);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_percent, GINT_TO_POINTER(COLUMN_PERCENT), NULL);

    col = do_tree_view_add_column(view, "Purchase", "Закуп", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_PURCHASE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_PURCHASE);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_money_bold, GINT_TO_POINTER(COLUMN_PURCHASE), NULL);



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

    do_purchase_view_fill_model(embed);

	return object;
}

static void do_purchase_view_finalize (GObject *object)
{
    DoPurchaseViewPrivate *priv;
	priv = DO_PURCHASE_VIEW_GET_PRIVATE (object);
    g_free(priv->units);

	G_OBJECT_CLASS (do_purchase_view_parent_class)->finalize (object);
}

static void do_purchase_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoPurchaseViewPrivate *priv = DO_PURCHASE_VIEW_GET_PRIVATE (object);

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
static void do_purchase_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoPurchaseViewPrivate *priv = DO_PURCHASE_VIEW_GET_PRIVATE (object);

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

static void do_purchase_view_class_init (DoPurchaseViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_purchase_view_constructor;
	o_class->finalize     = do_purchase_view_finalize;
	o_class->get_property = do_purchase_view_get_property;
	o_class->set_property = do_purchase_view_set_property;

	g_type_class_add_private (o_class, sizeof (DoPurchaseViewPrivate));

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

GtkWidget *do_purchase_view_new (const gchar *units, GDate *date1, GDate *date2)
{
	return g_object_new (DO_TYPE_PURCHASE_VIEW,
			     "units", units,
			     "date1", date1,
			     "date2", date2,
			     "object-root-path", OBJECT_ROOT_PATH,
			     (gpointer) NULL);
}
static gboolean do_purchase_view_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void	do_purchase_view_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_purchase_view_view_get_title(DoView *view)
{
    DoPurchaseViewPrivate *priv = DO_PURCHASE_VIEW_GET_PRIVATE(view);
    gchar *retval;
    gchar buf[256],buf1[256];
    gchar *v1,*v2;
    g_date_strftime(buf, 255, "%d/%m/%Y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%Y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL , NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL , NULL);
    retval = g_strdup_printf("Приходы за период с %s по %s", v1, v2);
    g_free(v1);g_free(v2);
    return retval;
}
static GdkPixbuf  *do_purchase_view_view_get_icon(DoView *view)
{
    DoPurchaseViewPrivate *priv = DO_PURCHASE_VIEW_GET_PRIVATE(view);
    return priv->icon;
}
typedef struct {
    gchar *name;
    gint    quant;
    gdouble purchase;
    gdouble retail;
} obt_doc_t;

typedef struct {
    partner_key0_t partner_key0;
    gchar *name;
    GSList *docs;
} obt_t;

static gboolean do_purchase_view_get_obt(do_alias_period_t *alias_period, GSList **obt, const gchar *unit);

static gboolean do_purchase_view_fill_model(DoPurchaseView *view)
{
    DoPurchaseViewPrivate *priv = DO_PURCHASE_VIEW_GET_PRIVATE(view);
    gint i;
    do_string_list_t *units;
    gboolean retval = TRUE;
    GSList *obts = NULL;
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
            retval = do_purchase_view_get_obt(p, &obts, units->list[i]);
        }

    }
    do_string_list_free(units);
    domino_meta_alias_free(meta);

    g_date_strftime(buf, 255, "%d/%m/%y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL, NULL);

    do_alias_t *alias;
    alias = domino_alias_new(NULL);
    if ( !do_alias_open(alias, TRUE) )  {
        do_alias_free(alias);
        return FALSE;
    }

    header_mark = g_strdup_printf("Приходы за период с <b>%s</b> по <b>%s</b>, Аптеки: <b>%s</b>",
                                  v1, v2, do_get_unit_desc(alias, priv->units) );
    do_alias_free(alias);
    g_free(v1);g_free(v2);
    gtk_label_set_markup(GTK_LABEL(priv->header), header_mark);

    do_end_long_operation(GTK_WIDGET(view));
    if ( retval ) {
        GSList *l;
        gint total_root;
        gint quant_root;
        gdouble purchase_root;
        gdouble retail_root;

        total_root = 0;
        quant_root = 0;
        purchase_root = 0;
        retail_root = 0;

        GtkTreeIter iter_root;
        gtk_tree_store_clear(priv->model);

        gtk_tree_store_append(priv->model, &iter_root, NULL);
        gtk_tree_store_set(priv->model, &iter_root,
                           COLUMN_DESC, "<b>Итого</b>",
                           -1
                           );
        for ( l = obts; l; l=l->next ) {
            GtkTreeIter iter_partner;
            obt_t *obt;
            GSList *l_doc;

            gint total_partner;
            gint quant_partner;
            gdouble purchase_partner;
            gdouble retail_partner;

            total_partner = 0;
            quant_partner = 0;
            purchase_partner = 0;
            retail_partner = 0;

            obt = l->data;
            gtk_tree_store_append(priv->model, &iter_partner, &iter_root);
            gtk_tree_store_set(priv->model, &iter_partner,
                               COLUMN_DESC, obt->name,
                               -1);

            for ( l_doc = obt->docs; l_doc; l_doc=l_doc->next ) {
                GtkTreeIter iter;
                obt_doc_t *doc;

                doc = l_doc->data;

                gtk_tree_store_append(priv->model, &iter, &iter_partner);
                gtk_tree_store_set(priv->model, &iter,
                                   COLUMN_DESC, doc->name,
                                   COLUMN_TOTAL, 1,
                                   COLUMN_QUANT, doc->quant,
                                   COLUMN_PURCHASE, doc->purchase,
                                   COLUMN_PERCENT, (doc->retail/doc->purchase-1),
                                   COLUMN_RETAIL, doc->retail,
                                   -1
                                   );
                total_partner++;
                quant_partner+=doc->quant;
                purchase_partner+=doc->purchase;
                retail_partner+=doc->retail;
            }
            gtk_tree_store_set(priv->model, &iter_partner,
                               COLUMN_TOTAL, total_partner,
                               COLUMN_QUANT, quant_partner,
                               COLUMN_PURCHASE, purchase_partner,
                               COLUMN_PERCENT, (retail_partner/ purchase_partner-1),
                               COLUMN_RETAIL, retail_partner,
                               -1
                               );
            total_root+=total_partner;
            quant_root+=quant_partner;
            purchase_root+=purchase_partner;
            retail_root+=retail_partner;
        }
        gtk_tree_store_set(priv->model, &iter_root,
                           COLUMN_TOTAL, total_root,
                           COLUMN_QUANT, quant_root,
                           COLUMN_PURCHASE, purchase_root,
                           COLUMN_PERCENT, (retail_root/ purchase_root-1),
                           COLUMN_RETAIL, retail_root,
                           -1
                           );

    }
    if ( retval ) {
        GtkTreePath *path;
        path = gtk_tree_path_new_first();
        gtk_tree_view_expand_row(priv->tree_view, path, FALSE);
    }

    return retval;
}
static gboolean do_purchase_view_get_obt(do_alias_period_t *alias_period, GSList **obts, const gchar *unit)
{

    do_alias_t *alias = alias_period->alias;

    document_key3_t document_key3, key;
    document_rec_t document;
    document_type_key0_t document_type_key0;
    int status;

    do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
    do_text_set(alias, document_key3.sklad, unit);
    do_date_set(&document_key3.date, alias_period->date_b);
    do_date_set(&key.date, alias_period->date_e);
    document_key3.time = 0;

    do_cpy(key.dtype, document_key3.dtype);
    do_cpy(key.sklad, document_key3.sklad);

    do_text_set(alias, document_type_key0.dtype1, "01");

    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);

    while ( status == DO_OK ) {
        if (
             do_cmp(key.dtype, document_key3.dtype) ||
             do_cmp(key.sklad, document_key3.sklad) ||
             document_key3.date > key.date
            ) break;

        if ( !do_cmp(document.data.dtype1, document_type_key0.dtype1) &&
             document.data.accepted ) {
            obt_t *item;
            GSList *l;
            item = NULL;

            for ( l = *obts; l; l=l->next )
                if (
                     !do_cmp(document.data.p_g_code, ((obt_t*)l->data)->partner_key0.g_code) &&
                       document.data.p_code ==  ((obt_t*)l->data)->partner_key0.code ) {
                    item = l->data;
                    break;
                }
            if ( !item ) {
                partner_rec_t partner;
                item = g_new0(obt_t, 1);
                do_cpy(item->partner_key0.g_code, document.data.p_g_code);
                item->partner_key0.code = document.data.p_code;
                status = do_partner_get0(alias, &partner, &item->partner_key0, DO_GET_EQUAL);
                if ( status != DO_OK ) {
                    if ( status == DO_KEY_NOT_FOUND ) {
                        do_log(LOG_ERR, "partner \"%s\" not found", do_util_partner_key0_to_str(alias, &item->partner_key0));
                        status = DO_ERROR;
                        break;
                    }
                }
                item->name = do_text(alias, partner.data.name);
                item->docs = NULL; // just in case
                *obts = g_slist_append(*obts, item);
            }
            obt_doc_t *doc;
            struct tm date;
            doc = g_new0(obt_doc_t, 1);
            item->docs = g_slist_append(item->docs, doc);
            do_date(document.data.date, &date);
            gchar *v1, *v2;

            v1 = do_text(alias, document.data.p_doc);
            v2 = do_datetoa(date);

            doc->name = g_strdup_printf("%s от %s", v1, v2);
            g_free(v1);g_free(v2);

            doc->quant = do_document_param_int(alias, &document, "616");
            doc->purchase = do_document_sum(alias, &document, 2);
            doc->retail = do_document_sum(alias, &document, 4);

        }
        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }

    if ( status == DO_ERROR )
        return FALSE;


    return TRUE;
}
static void do_print_item(do_template_t *templ, do_template_cursor_t *line, GtkTreeModel *model, GtkTreeView *view, GtkTreePath *path);
static gchar *do_print_get_html(DoPurchaseView *view)
{
    DoPurchaseViewPrivate *priv = DO_PURCHASE_VIEW_GET_PRIVATE(view);
    do_template_t *templ;
    gchar *title;
    gchar *value;

    templ = do_template_new("purchase", TRUE);
    if ( !templ )
        return NULL;
    do_template_cursor_t *cursor;
    do_template_cursor_t *line;
    char buf[256], buf1[256], *v1,*v2;
    g_date_strftime(buf, 255, "%d/%m/%y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL, NULL);
    title = g_strdup_printf("Приходы за период с <b>%s</b> по <b>%s</b>",
                                  v1, v2);
    g_free(v1);g_free(v2);
    cursor = do_template_cursor(templ);
    line   = do_template_section(templ, "Line");
    if ( !line ) {
        do_log(LOG_ERR, "Неправильный шаблон \"%s\"", "purchase");
        do_template_free(templ);
        return NULL;
    }

    do_alias_t *alias;
    alias = domino_alias_new(NULL);
    if ( !do_alias_open(alias, TRUE) )  {
        do_alias_free(alias);
        return FALSE;
    }
    do_template_set(templ,
                    "Title", title,
                    "Units", do_get_unit_desc(alias, priv->units), NULL);
    do_alias_free(alias);
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
static void  do_purchase_view_do_print(DoView *view)
{
    //DoPurchaseViewPrivate *priv = DO_PURCHASE_VIEW_GET_PRIVATE(view);
    gchar *html_string = do_print_get_html(DO_PURCHASE_VIEW(view));
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
static gboolean    do_purchase_view_can_do_print(DoView *view)
{
    return TRUE;
}
static void  do_purchase_view_do_print_now(DoView *view)
{
    //DoPurchaseViewPrivate *priv = DO_PURCHASE_VIEW_GET_PRIVATE(view);
    gchar *html_string = do_print_get_html(DO_PURCHASE_VIEW(view));
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
        GValue desc = {0,}, total = {0,}, quant = {0,}, purchase = {0,}, retail = {0,}, percent = {0,};
        GtkTreeIter iter;
        gtk_tree_model_get_iter(model, &iter, path);
        gtk_tree_model_get_value(model, &iter, COLUMN_DESC, &desc);
        gtk_tree_model_get_value(model, &iter, COLUMN_TOTAL, &total);
        gtk_tree_model_get_value(model, &iter, COLUMN_QUANT, &quant);
        gtk_tree_model_get_value(model, &iter, COLUMN_PURCHASE, &purchase);
        gtk_tree_model_get_value(model, &iter, COLUMN_RETAIL, &retail);
        gtk_tree_model_get_value(model, &iter, COLUMN_PERCENT, &percent);
        gchar *total_str,*quant_str, *purchase_str, *retail_str, *desc_str, *percent_str;
        if ( g_value_get_int(&total) )
            total_str = g_strdup_printf("%d",g_value_get_int(&total));
        else
            total_str = g_strdup("");
        quant_str = g_strdup_printf("%d", g_value_get_int(&quant));
        purchase_str = do_money_format(g_value_get_double(&purchase), FALSE);
        retail_str = do_money_format(g_value_get_double(&retail), FALSE);
        percent_str = do_percent_format(g_value_get_double(&percent)*100);
        desc_str = g_strdup_printf("<div style=\"margin-left:%dpx;\">%s</dev>",
                                   10*(gtk_tree_path_get_depth(path)-1), g_value_get_string(&desc));



        do_template_set(templ, "Desc", desc_str,
                               "Total", total_str,
                               "Quant", quant_str,
                               "Purchase", purchase_str,
                               "Percent", percent_str,//
                               "Retail", retail_str,
                               NULL);
        g_free(total_str);g_free(quant_str);g_free(purchase_str);g_free(retail_str);g_free(desc_str);

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
