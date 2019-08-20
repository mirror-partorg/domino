
#include "do_discount_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_discount_dialog.h"
#include "domino.h"
#include "do_tree_view.h"
#include <domino.h>
#include "hig.h"
#include "do_icons.h"
#include <gdk/gdk.h>
#include <time.h>

#define DO_DISCOUNT_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_DISCOUNT_VIEW, DoDiscountViewPrivate))

#define OBJECT_ROOT_PATH "DiscountView"
#define DEFAULT_COL_PACK_FORM_WIDTH 400
#define DEFAULT_COL_DEFAULT_WIDTH 60

// View interface
static void do_discount_view_view_init(DoViewIface *iface);

static gboolean    do_discount_view_view_do_grab_focus(DoView *view);
static void	       do_discount_view_view_do_close(DoView *view);
static const char *do_discount_view_view_get_title(DoView *view);
static GdkPixbuf  *do_discount_view_view_get_icon(DoView *view);
static gboolean    do_discount_view_fill_model(DoDiscountView *view);
void do_discount_view_data_func_description(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         DoDiscountView *view);

enum
{
	PROP_0,
	PROP_UNITS,
	PROP_DATE1,
	PROP_DATE2,
	PROP_DISCOUNT_TYPES_NAME,
	PROP_DISCOUNT_GROUP,
};

enum
{
    COLUMN_DESC = 0,
    COLUMN_TOTAL = 1,
    COLUMN_SUM = 2,
    COLUMN_DISCOUNT = 3,
    COLUMN_CARD = 4,
    COLUMN_PERCENT = 5,
    COLUMN_NO =6,

};

struct _DoDiscountViewPrivate
{
    GtkTreeStore        *model;
    GtkTreeView         *tree_view;
    GtkTreeViewColumn   **columns;
    GtkTreeSelection    *selection;
    GdkPixbuf           *icon;
    gchar               *units;
    GDate               date1;
    GDate               date2;
    do_list_t           *range;
    GtkWidget           *footer;
    GtkWidget           *header;
    gint                 group;
    gchar               *range_name;
};

G_DEFINE_TYPE_WITH_CODE (DoDiscountView, do_discount_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_discount_view_view_init)
                                    );

static void do_discount_view_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_discount_view_view_do_grab_focus;
    iface->do_close      = do_discount_view_view_do_close;
    iface->get_title     = do_discount_view_view_get_title;
    iface->get_icon      = do_discount_view_view_get_icon;
    //iface->do_edit       = do_discount_view_view_do_edit;
    //iface->can_do_edit   = do_discount_view_view_can_do_edit;
    //iface->do_delete       = do_discount_view_view_do_delete;
    //iface->do_insert       = do_discount_view_view_do_insert;
    //iface->can_do_delete   = do_discount_view_view_can_do_delete;
    //iface->can_do_insert   = do_discount_view_view_can_do_insert;
}

static void do_discount_view_init(DoDiscountView *view)
{
	DoDiscountViewPrivate *priv = DO_DISCOUNT_VIEW_GET_PRIVATE (view);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
#if GTK_MAJOR_VERSION > 2
    priv->icon = gtk_icon_theme_load_icon(icon_theme, "folder-documents-symbolic", DO_VIEW_ICON_SIZE, 0, NULL);
#else
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_FILE, DO_VIEW_ICON_SIZE, 0, NULL);
#endif
}

static GtkTreeStore *do_discount_view_create_model(DoDiscountView *embed)
{
	//DoDiscountViewPrivate *priv = DO_DISCOUNT_VIEW_GET_PRIVATE(embed);
    GtkTreeStore    *model;
    model = gtk_tree_store_new(COLUMN_NO,
                               G_TYPE_STRING, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_INT, G_TYPE_DOUBLE);
    return model;
}
static void do_discount_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoDiscountView *view)
{
    if ( gtk_tree_view_row_expanded(tree_view, path) )
        gtk_tree_view_collapse_row(tree_view, path);
    else
        gtk_tree_view_expand_row(tree_view, path, FALSE);
}

static GObject *do_discount_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkTreeStore        *model;
    DoTreeView          *view;
	DoDiscountView      *embed;
	GtkTreeView         *tree_view;
	DoDiscountViewPrivate   *priv;

	object = G_OBJECT_CLASS (do_discount_view_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_DISCOUNT_VIEW_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_DISCOUNT_VIEW(object);
	g_object_get(object, "tree-view", &tree_view, NULL);

    g_signal_connect (tree_view, "row-activated", G_CALLBACK(do_discount_view_row_activated), object);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(tree_view), FALSE);

	priv->tree_view = tree_view;
    priv->selection = gtk_tree_view_get_selection(tree_view);

	priv->model = model = do_discount_view_create_model(embed);
	if (model)
        g_object_set(view, "model", model,
//                           "fixed-height-mode", FALSE,
                           NULL);

/*    col = do_tree_view_add_column(view, "Card", "Карта", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_CARD);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_CARD);
*/

    col = do_tree_view_add_column(view, "Desc", "Наименование", 300);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_DESC);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_DESC);
    /*gtk_tree_view_column_set_cell_data_func(col ,renderer,
             (GtkTreeCellDataFunc)do_discount_view_data_func_description, object, NULL);*/

    col = do_tree_view_add_column(view, "Total", "Количество", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_TOTAL);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_TOTAL);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_rest, GINT_TO_POINTER(COLUMN_TOTAL), NULL);

    col = do_tree_view_add_column(view, "Sum", "Сумма", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_SUM);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_SUM);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_money, GINT_TO_POINTER(COLUMN_SUM), NULL);

    col = do_tree_view_add_column(view, "Discount", "Скидка", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_DISCOUNT);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_DISCOUNT);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_money, GINT_TO_POINTER(COLUMN_DISCOUNT), NULL);

    col = do_tree_view_add_column(view, "Percent", "Процент", 100);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_PERCENT);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_PERCENT);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_percent, GINT_TO_POINTER(COLUMN_PERCENT), NULL);

    priv->header = gtk_label_new("");
    g_object_set(priv->header, "xalign", 0.0, NULL);
    do_tree_view_pack_header(view, GTK_WIDGET(priv->header));
    gtk_widget_show(GTK_WIDGET(priv->header));

    priv->footer = gtk_label_new("");
    g_object_set(priv->footer, "xalign", 0.01, NULL);
    do_tree_view_pack_footer(view, GTK_WIDGET(priv->footer));
    gtk_widget_show(GTK_WIDGET(priv->footer));


    gtk_tree_view_set_search_column(tree_view, 0);
	gtk_widget_show(GTK_WIDGET(object));

    do_discount_view_fill_model(embed);

	return object;
}

static void do_discount_view_finalize (GObject *object)
{
    DoDiscountViewPrivate *priv;
	priv = DO_DISCOUNT_VIEW_GET_PRIVATE (object);
	if ( priv->units )
		g_free(priv->units);
    if ( priv->range_name )
        g_free(priv->range_name);
    if ( priv->range )
        do_list_free(priv->range);

	G_OBJECT_CLASS (do_discount_view_parent_class)->finalize (object);
}

static void do_discount_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoDiscountViewPrivate *priv = DO_DISCOUNT_VIEW_GET_PRIVATE (object);

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
        case PROP_DISCOUNT_TYPES_NAME:
            g_value_set_string(value, priv->range_name);
            break;
        case PROP_DISCOUNT_GROUP:
            g_value_set_int(value, priv->group);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
	/* no readable properties */
	g_assert_not_reached ();
}
typedef struct {
    int start;
    int end;
} interval_t;

static void do_discount_view_set_range(DoDiscountView *view, const gchar *name)
{
	DoDiscountViewPrivate *priv = DO_DISCOUNT_VIEW_GET_PRIVATE (view);
    interval_t inter;
    char *value;
    do_string_list_t *sinter;
    int i;
    if ( priv->range_name )
        g_free(priv->range_name);
    priv->range_name = g_strdup(name);
    if ( priv->range )
        do_list_free(priv->range);

    priv->range = do_list_new(TRUE);

    DOMINO_COMMON_GET("DiscountTypes", name, &value, NULL);

    if ( !value )
        return;

    sinter = do_string_list_new_from_csv(value);

    for ( i = 0; i < sinter->count; i++ ) {
        char *ch, *val;
        val = sinter->list[i];
        ch = strchr(val, '-');
        if ( ch ) {
            ch[0] = '\0';
            ch++;
            if ( val[0] == '\0' )
                inter.start=-1;
            else
                inter.start=atoi(val);
            if ( ch[0] == '\0' )
                inter.end=-1;
            else
                inter.end=atoi(ch);
            do_list_add_alloc(priv->range, &inter, sizeof(inter));
        }
    }

    do_string_list_free(sinter);



}

static void do_discount_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoDiscountViewPrivate *priv = DO_DISCOUNT_VIEW_GET_PRIVATE (object);

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
        case PROP_DISCOUNT_GROUP:
            priv->group = g_value_get_int(value);
            break;
        case PROP_DISCOUNT_TYPES_NAME:
            do_discount_view_set_range(DO_DISCOUNT_VIEW(object), g_value_dup_string(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
}

static void do_discount_view_class_init (DoDiscountViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_discount_view_constructor;
	o_class->finalize     = do_discount_view_finalize;
	o_class->get_property = do_discount_view_get_property;
	o_class->set_property = do_discount_view_set_property;

	g_type_class_add_private (o_class, sizeof (DoDiscountViewPrivate));

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
    g_object_class_install_property (o_class,
				   PROP_DISCOUNT_TYPES_NAME,
				   g_param_spec_string("discount-range-name",
 						      "Диапазон карт наименование",
 						      "Диапазон карт наименование",
 						      NULL,
 						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY ));
    g_object_class_install_property (o_class,
				   PROP_DISCOUNT_GROUP,
				   g_param_spec_int ("discount-group",
 						      "Группировка карт",
 						      "Группировка карт",
 						      0,
 						      10000,
 						      0,
 						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY ));

}

GtkWidget *do_discount_view_new (const gchar *units, GDate *date1, GDate *date2, const gchar *range_name, gint group)
{
	return g_object_new (DO_TYPE_DISCOUNT_VIEW,
			     "units", units,
			     "date1", date1,
			     "date2", date2,
			     "discount-range-name", range_name,
			     "discount-group", group,
			     "object-root-path", OBJECT_ROOT_PATH,
			     (gpointer) NULL);
}
static gboolean do_discount_view_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void	do_discount_view_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_discount_view_view_get_title(DoView *view)
{
    DoDiscountViewPrivate *priv = DO_DISCOUNT_VIEW_GET_PRIVATE(view);
    gchar *retval;
    gchar buf[256],buf1[256];
    gchar *v1,*v2;
    g_date_strftime(buf, 255, "%d/%m/%Y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%Y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL , NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL , NULL);
    retval = g_strdup_printf("Отчет по скидкам с %s по %s", v1, v2);
    g_free(v1);g_free(v2);
    return retval;
}
static GdkPixbuf  *do_discount_view_view_get_icon(DoView *view)
{
    DoDiscountViewPrivate *priv = DO_DISCOUNT_VIEW_GET_PRIVATE(view);
    return priv->icon;
}
void do_discount_view_data_func_description(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         DoDiscountView *view)
{

}
static gboolean do_discount_view_get_for_unit(do_alias_period_t *p, const gchar *unit, do_sort_list_t *retval, do_list_t *range, gint group);


typedef struct {
    gchar *group_name;
    gint card;
    gchar *card_name;
    do_list_t *checks;
} sk_t;

typedef struct {
    GDateTime *time;
    gchar *unit;
    gint cash;
    gint shift;
    gint check;
    gint pos;
    gdouble sum;
    gdouble discount;
    do_list_t *lines;
} check_t;

typedef struct {
    char *desc;
    gdouble total;
    gdouble sum;
    gdouble discount;
} line_t;

static int do_discount_view_sk_cmp_func(sk_t *s1, sk_t  *s2)
{
    int i;
    if ( s1->group_name &&  s2->group_name) {
        i = strcmp(s1->group_name, s2->group_name);
        if ( i )
            return i;
    }
    return s1->card-s2->card;
}

static gboolean do_discount_view_fill_model(DoDiscountView *view)
{
    DoDiscountViewPrivate *priv = DO_DISCOUNT_VIEW_GET_PRIVATE(view);
    gint i, j, k;
    do_string_list_t *units;
    gboolean retval = TRUE;
    do_meta_alias_t *meta;
    struct tm bd;
    struct tm ed;
    do_sort_list_t *list;
    int total_check;
    double total_sum, total_discount;
    double group_sum, group_discount, group_total=0;

    do_start_long_operation(GTK_WIDGET(view));


    meta = domino_meta_alias_new();
    if ( !meta )
        return FALSE;
    g_date_to_struct_tm(&priv->date1, &bd);
    g_date_to_struct_tm(&priv->date2, &ed);

    list = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)do_discount_view_sk_cmp_func, NULL);


    units = do_string_list_new_from_csv(priv->units);
    for ( i = 0; retval && i < units->count; i++ ) {
        do_sort_list_t *aliases;
        aliases = domino_meta_alias_get_list_from_sklad(meta, units->list[i], &bd, &ed);
        for ( j = 0 ; retval && j < aliases->count; j++ ) {
            do_alias_period_t *p;
            p = aliases->list[j];
            retval = do_discount_view_get_for_unit(p, units->list[i], list, priv->range, priv->group);
        }
    }
    do_string_list_free(units);
    domino_meta_alias_free(meta);

    if ( retval ) {
        gtk_tree_store_clear(priv->model);
        char *crnt_group="";
        total_check = 0;
        total_sum = 0;
        total_discount = 0;
        GtkTreeIter root_iter, *group_iter=NULL;

        for ( i = 0; i < list->count; i++ ) {
            //int proc;
            sk_t *sk;
            double sum,discount, pp;
            GtkTreeIter card_iter;
            sk = list->list[i];
            if ( priv->group ) {
                if ( strcmp(crnt_group, sk->group_name) ) {
                    if ( group_iter ) {
                        gtk_tree_store_set(priv->model, group_iter,
                                           COLUMN_TOTAL, group_total,
                                           COLUMN_SUM, group_sum,
                                           COLUMN_DISCOUNT, group_discount,
                                           -1
                                           );

                    }
                    group_sum=0;group_discount=0;group_total=0;
                    crnt_group = g_strdup(sk->group_name);
                    gtk_tree_store_append(priv->model, &root_iter, NULL);
                    group_iter=&root_iter;
                    gtk_tree_store_set(priv->model, &root_iter,
                                       COLUMN_DESC, sk->group_name,
                                       //COLUMN_TOTAL, pp,
                                       //COLUMN_SUM, sum,
                                       //COLUMN_DISCOUNT, discount,
                                       //COLUMN_CARD, sk->card,
                                       -1
                                       );


                }

            }

            gtk_tree_store_append(priv->model, &card_iter, group_iter);
            sum=0;discount=0;
            for ( j = 0; j < sk->checks->count; j++ ) {
                GtkTreeIter iter;
                gchar *desc;
                check_t *check;
                gchar *str;

                check = sk->checks->list[j];
                str = g_date_time_format(check->time, "%d/%m/%y %H:%M:%S");

                desc = g_strdup_printf("Чек %s %d/%d/%d аптека %s", str, check->cash, check->shift, check->check, check->unit);
                gtk_tree_store_append(priv->model, &iter, &card_iter);
                pp=check->pos;

                gtk_tree_store_set(priv->model, &iter,
                                   COLUMN_DESC, desc,
                                   COLUMN_TOTAL, pp,
                                   COLUMN_SUM, check->sum,
                                   COLUMN_DISCOUNT, check->discount,
                                   //COLUMN_CARD, sk->card,
                                   -1
                                   );
                for ( k = 0; k < check->lines->count; k++ ) {
                    GtkTreeIter pos_iter;
                    line_t *line;
                    line = check->lines->list[k];
                    //proc=line->discount/(line->sum+line->discount)*100.;

                    gtk_tree_store_append(priv->model, &pos_iter, &iter);
                    pp=line->discount/(line->sum+line->discount);
                    gtk_tree_store_set(priv->model, &pos_iter,
                                       COLUMN_DESC, line->desc,
                                       COLUMN_TOTAL, line->total,
                                       COLUMN_SUM, line->sum,
                                       COLUMN_DISCOUNT, line->discount,
                                     //  COLUMN_CARD, sk->card,
                                       COLUMN_PERCENT, pp,
                                       -1
                                       );


                }
                sum+=check->sum;
                discount+=check->discount;
                g_free(desc);
            }
            pp=sk->checks->count;
            total_check += sk->checks->count;
            total_sum+=sum;
            total_discount+=discount;
            gtk_tree_store_set(priv->model, &card_iter,
                               COLUMN_DESC, sk->card_name,
                               COLUMN_TOTAL, pp,
                               COLUMN_SUM, sum,
                               COLUMN_DISCOUNT, discount,
                               COLUMN_CARD, sk->card,
                               -1
                               );
            group_sum+=sum;
            group_discount+=discount;
            group_total+=pp;

        }
        char *footer_mark, *header_mark;
        gchar buf[256],buf1[256];
        gchar *v1,*v2;
        g_date_strftime(buf, 255, "%d/%m/%y", &priv->date1);
        g_date_strftime(buf1, 255, "%d/%m/%y", &priv->date2);
        v1 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
        v2 = g_locale_to_utf8(buf1, -1, NULL, NULL, NULL);

        header_mark = g_strdup_printf("Отчет по скидкам за период с <b>%s</b> по <b>%s</b>, аптеки: <b>%s</b> карты: <b>%s</b>",
                                      v1, v2, priv->units, priv->range_name );
        g_free(v1);g_free(v2);
        footer_mark = g_strdup_printf("Итого чеков: <b>%d</b> на сумму: <b>%s</b>, суммарная скидка: <b>%s</b> ",
                                       total_check, do_money_format(total_sum, TRUE), do_money_format(total_discount, TRUE));

        gtk_label_set_markup(GTK_LABEL(priv->header), header_mark);

        gtk_label_set_markup(GTK_LABEL(priv->footer), footer_mark);

        g_free(footer_mark);
        if ( group_iter ) {
            gtk_tree_store_set(priv->model, group_iter,
                               COLUMN_TOTAL, group_total,
                               COLUMN_SUM, group_sum,
                               COLUMN_DISCOUNT, group_discount,
                               -1
                               );

        }

    }
    do_end_long_operation(GTK_WIDGET(view));
    return retval;
}

static gboolean do_discount_view_check_range(do_list_t *range, gint cardno)
{
    gint i;
    for ( i = 0; i < range->count; i++ ) {
        interval_t *inter;
        inter = range->list[i];
        if ( ((inter->start <= cardno) || (inter->start == -1))
             &&
             ((inter->end >= cardno) || (inter->end == -1)) )
            return TRUE;
    }
    return FALSE;
}
typedef struct {
    gint start;
    gint end;
    gchar *desc;
} kind_t;

static gchar *do_discount_view_sk_get_card_name(do_alias_t *alias, int card, do_list_t *discount_kind)
{
    partner_key0_t partner_key0;
    partner_rec_t partner;
    int i;

    do_text_set(alias, partner_key0.g_code, "SK");
    partner_key0.code = card;

    if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) == DO_OK )
        return do_text(alias, partner.data.name);

    if ( card < 2000 ) {
        do_text_set(alias, partner_key0.g_code, "CAS");
        partner_key0.code = card - 1000;
        if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) == DO_OK )
            return do_text(alias, partner.data.name);
    }
    else {
        if ( (card < 20000) && (card > 10000) ) {
            do_text_set(alias, partner_key0.g_code, "CAS");
            partner_key0.code = card - 10000;
            if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) == DO_OK )
                return do_text(alias, partner.data.name);
        }
        else {
            /*if ( (card < 54321999) && (card > 54321000) ) {
                do_text_set(alias, partner_key0.g_code, "CAS");
                partner_key0.code = card - 54321000;
                if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) == DO_OK )
                    return do_text(alias, partner.data.name);
                else {
                    do_text_set(alias, partner_key0.g_code, "SK");
                    partner_key0.code = card - 54320000;
                    if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) == DO_OK )
                        return do_text(alias, partner.data.name);
                }
            }*/
        }
    }

    for ( i = discount_kind->count-1;i >=0;  i-- ) {
        kind_t *kind;
        kind = discount_kind->list[i];
        if ( (kind->start <= card) && (kind->end >= card) ) {
            return g_strdup_printf("%s (%d)", kind->desc, card);

        }
    }

    return g_strdup_printf("Карта %d", card);
}

do_list_t *do_discount_view_sk_get_type()
{
    GList *list, *l;
    do_list_t *retval;

    retval = do_list_new(TRUE);

    list = DOMINO_COMMON_PROPERTIES("DiscountTypes");
    for (l = list; l; l = l->next) {
        kind_t kind;
        gchar *value, *ch, *str;
        DOMINO_COMMON_GET("DiscountTypes", l->data, &value, NULL);
        str  = g_strdup(value);
        ch = strchr(str, '-');
        if ( ch ) {
            *ch = '\0';
            ch++;
            kind.start=atoi(str);
            kind.end=atoi(ch);
        }
        kind.desc = g_strdup(l->data);
        do_list_add_alloc(retval, &kind, sizeof(kind));
    }
    g_list_foreach(list, (GFunc)g_free, NULL);
    g_list_free(list);
    return retval;
}

static gboolean do_discount_view_get_for_unit(do_alias_period_t *p, const gchar *unit, do_sort_list_t *retval, do_list_t *range_, gint group)
{
    do_alias_t *alias = p->alias;
    document_key3_t document_key3, key;
    document_rec_t document;
    int status;

    if ( !do_alias_open(alias, TRUE) )
        return FALSE;

    do_list_t *discount_kind, *discount_type;

    do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_DISCOUNT));
    do_text_set(alias, document_key3.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    document_key3.date = 0;
    document_key3.time = 0;
    do_cpy(key.dtype, document_key3.dtype);
    do_cpy(key.sklad, document_key3.sklad);

    discount_type=do_discount_view_sk_get_type();

    discount_kind=do_list_new(TRUE);

    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);

    while ( status == DO_OK ) {
        kind_t kind;
        if ( do_cmp(key.dtype, document_key3.dtype) ||
             do_cmp(key.sklad, document_key3.sklad)
            ) break;

        kind.desc = do_document_param(alias, &document, "409");
        kind.start = document.data.date;
        kind.end = document.data.time;
        do_list_add_alloc(discount_kind, &kind, sizeof(kind));

        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }

    if ( status == DO_ERROR )
        return FALSE;



    do_text_set(alias, document_key3.dtype, "DK");
    do_text_set(alias, document_key3.sklad, unit);
    do_date_set(&document_key3.date, p->date_b);
    document_key3.time = 0;

    do_cpy(key.dtype, document_key3.dtype);
    do_cpy(key.sklad, document_key3.sklad);
    do_date_set(&key.date, p->date_e);

    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);

    while ( status == DO_OK ) {
        if ( do_cmp(key.dtype, document_key3.dtype) ||
             do_cmp(key.sklad, document_key3.sklad) ||
            key.date < document_key3.date
            ) break;
        sk_t sk;
        sk.card = do_document_param_int(alias, &document, "CARD");


        /*if ( sk.card > 443210000 ) {
            sk.card -= 488880000;
            do_document_param_int_set(alias, &document, "CARD", sk.card);
            do_document_update(alias, &document);
        }*/

        if ( do_discount_view_check_range(range_, sk.card) ) {
            int indx;
            check_t check;
            struct tm tm;
            sk_t *crnt;
            char *val, *ch;
            switch ( group ) {
                case 0:
                    sk.group_name = NULL;
                    break;
                case 1:
                    sk.group_name = do_strdup_printf("Аптека %3d", atoi(unit));
                    break;
                case 2: {
                    int i;
                    sk.group_name = NULL;
                    for ( i = discount_type->count-1; i >=0; i-- ) {
                        kind_t *kind;
                        kind = discount_type->list[i];
                        if ( (kind->start <= sk.card) && (kind->end >= sk.card) ) {
                            sk.group_name = g_strdup(kind->desc);
                            break;
                        }
                    }
                    if ( !sk.group_name )
                        sk.group_name = do_strdup("Неизвестно");
                    break;
                }
                case 3: {
                    int i;
                    sk.group_name = NULL;
                    for ( i = discount_kind->count-1; i >=0; i-- ) {
                        kind_t *kind;
                        kind = discount_kind->list[i];
                        if ( (kind->start <= sk.card) && (kind->end >= sk.card) ) {
                            sk.group_name = g_strdup(kind->desc);
                            break;
                        }
                    }
                    if ( !sk.group_name )
                        sk.group_name = do_strdup("Неизвестно");
                    break;
                }
                case 4:
                    sk.group_name = do_discount_view_sk_get_card_name(alias, sk.card, discount_kind);
;
                    break;

            }
            if ( !do_sort_list_find(retval, &indx, &sk) ) {
                sk.card_name = do_discount_view_sk_get_card_name(alias, sk.card, discount_kind);
                sk.checks = do_list_new(TRUE);
                do_sort_list_insert_new(retval, indx, &sk, sizeof(sk));
            }
            crnt = retval->list[indx];
            do_date(document.data.date, &tm);
            do_time(document.data.time, &tm);

            check.time = g_date_time_new_local(tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

            val = do_text(alias, document.data.p_doc);
            ch = val;
            ch[4] = '\0';
            check.cash = atoi(ch);
            ch[9] = '\0';
            check.shift = atoi(ch+5);
            check.check = atoi(ch+10);
            check.pos = 0;
            check.sum = 0;
            check.discount = 0;
            check.unit = g_strdup(unit);
            check.lines = do_list_new(TRUE);

            document_order_key0_t document_order_key0;
            document_order_rec_t document_order;
            do_cpy(document_order_key0.dtype, document.data.dtype);
            do_cpy(document_order_key0.sklad, document.data.sklad);
            do_cpy(document_order_key0.document, document.data.document);
            document_order_key0.line = 0;

            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
            while ( status == DO_OK ) {
                line_t line;
                product_key0_t product_key0;
                product_rec_t product;

                if ( do_cmp(document_order_key0.dtype, document.data.dtype) ||
                     do_cmp(document_order_key0.sklad, document.data.sklad) ||
                     do_cmp(document_order_key0.document, document.data.document) ) break;

                document_order.data.code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)] = '\0';
                do_text_set(alias, product_key0.code, document_order.data.code);
                product_key0.base_parcel = 0;

                if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
                    do_log(LOG_ERR, "product %s not found", document_order.data.code);
                }
                line.desc = do_product_name_format(do_text(alias, product.data.name));
                line.total = do_document_order_quant(alias, &document_order, 1)/do_product_coef(alias, &product);
                line.sum = do_document_order_price(alias, &document_order, 9)*do_document_order_quant(alias, &document_order, 1);
                line.discount = (do_document_order_price(alias, &document_order, 4)-do_document_order_price(alias, &document_order, 9))*do_document_order_quant(alias, &document_order, 1);

                check.pos++;
                check.sum+=line.sum;
                check.discount+=line.discount;
                do_list_add_alloc(check.lines, &line, sizeof(line));
                status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
            }
            if ( status == DO_ERROR )
                break;
            do_list_add_alloc(crnt->checks, &check, sizeof(check));

        }
        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }

    if ( status == DO_ERROR )
        return FALSE;

    return TRUE;
}
