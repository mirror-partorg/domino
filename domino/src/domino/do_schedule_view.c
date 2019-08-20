
#include "do_schedule_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_schedule_dialog.h"
#include "domino.h"
#include "do_tree_view.h"
#include "../misc/template.h"
#include <domino.h>
#include "hig.h"
#include "do_icons.h"
#include <gdk/gdk.h>
#include <time.h>

#define DO_SCHEDULE_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_SCHEDULE_VIEW, DoScheduleViewPrivate))

#define OBJECT_ROOT_PATH "ScheduleView"
#define DEFAULT_COL_PACK_FORM_WIDTH 400
#define DEFAULT_COL_DEFAULT_WIDTH 60

// View interface
static void do_schedule_view_view_init(DoViewIface *iface);

static gboolean    do_schedule_view_view_do_grab_focus(DoView *view);
static void	       do_schedule_view_view_do_close(DoView *view);
static const char *do_schedule_view_view_get_title(DoView *view);
static GdkPixbuf  *do_schedule_view_view_get_icon(DoView *view);
static gboolean    do_schedule_view_fill_model(DoScheduleView *view);
static gboolean    do_schedule_view_can_do_print(DoView *view);
static void        do_schedule_view_do_print(DoView *view);
static void        do_schedule_view_do_print_now(DoView *view);
void do_schedule_view_data_func_description(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         DoScheduleView *view);

enum
{
	PROP_0,
	PROP_ALIASES,
	PROP_DATE1,
	PROP_DATE2,
	PROP_SCHEDULE_POSTPONEMENT,
	PROP_SUPPLIER_IGNORE,
};

enum
{
    COLUMN_DESC = 0,
    COLUMN_DEBT,
    COLUMN_NO ,

};

struct _DoScheduleViewPrivate
{
    GtkTreeStore        *model;
    GtkTreeView         *tree_view;
    GtkTreeViewColumn   **columns;
    GtkTreeSelection    *selection;
    gchar               *aliases;
    GDate               date1;
    GDate               date2;
    gint                postponement;
    gchar               *ignore;
    //GtkWidget           *footer;
    GtkWidget           *header;
    GdkPixbuf           *icon;
    gchar               *firms;
};

G_DEFINE_TYPE_WITH_CODE (DoScheduleView, do_schedule_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_schedule_view_view_init)
                                    );

static void do_schedule_view_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_schedule_view_view_do_grab_focus;
    iface->do_close      = do_schedule_view_view_do_close;
    iface->get_title     = do_schedule_view_view_get_title;
    iface->get_icon      = do_schedule_view_view_get_icon;
    iface->do_print      = do_schedule_view_do_print;
    iface->do_print_now      = do_schedule_view_do_print_now;
    iface->can_do_print  = do_schedule_view_can_do_print;

    //iface->do_edit       = do_schedule_view_view_do_edit;
    //iface->can_do_edit   = do_schedule_view_view_can_do_edit;
    //iface->do_delete       = do_schedule_view_view_do_delete;
    //iface->do_insert       = do_schedule_view_view_do_insert;
    //iface->can_do_delete   = do_schedule_view_view_can_do_delete;
    //iface->can_do_insert   = do_schedule_view_view_can_do_insert;
}

static void do_schedule_view_init(DoScheduleView *view)
{
#if GTK_MAJOR_VERSION == 2
	DoScheduleViewPrivate *priv = DO_SCHEDULE_VIEW_GET_PRIVATE (view);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_FILE, DO_VIEW_ICON_SIZE, 0, NULL);
#endif
}

static GtkTreeStore *do_schedule_view_create_model(DoScheduleView *embed)
{
	DoScheduleViewPrivate *priv = DO_SCHEDULE_VIEW_GET_PRIVATE(embed);

    gint count =  g_date_days_between(&priv->date1, &priv->date2) + 2;

    GtkTreeStore  *model;
    GType *types;
    int j;

    types = g_new(GType, count+2);
    types[0] = G_TYPE_STRING;

    for (j = 1; j <= count+1; j++)
        types[j] = G_TYPE_DOUBLE;

    model = gtk_tree_store_newv(count + 2, types);

    return model;
}
static void do_schedule_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoScheduleView *view)
{
    if ( gtk_tree_view_row_expanded(tree_view, path) )
        gtk_tree_view_collapse_row(tree_view, path);
    else
        gtk_tree_view_expand_row(tree_view, path, FALSE);
}

static GObject *do_schedule_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkTreeStore        *model;
    DoTreeView          *view;
	DoScheduleView      *embed;
	GtkTreeView         *tree_view;
	DoScheduleViewPrivate   *priv;

	object = G_OBJECT_CLASS (do_schedule_view_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_SCHEDULE_VIEW_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_SCHEDULE_VIEW(object);
	g_object_get(object, "tree-view", &tree_view, NULL);

    g_signal_connect (tree_view, "row-activated", G_CALLBACK(do_schedule_view_row_activated), object);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(tree_view), FALSE);

	priv->tree_view = tree_view;
    priv->selection = gtk_tree_view_get_selection(tree_view);

	priv->model = model = do_schedule_view_create_model(embed);
	if (model)
        g_object_set(view, "model", model,
//                           "fixed-height-mode", FALSE,
                           NULL);

    col = do_tree_view_add_column(view, "Desc", "Наименование", 300);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_DESC);
    gtk_tree_view_column_add_attribute (col, renderer, "markup", COLUMN_DESC);
    /*gtk_tree_view_column_set_cell_data_func(col ,renderer,
             (GtkTreeCellDataFunc)do_schedule_view_data_func_description, object, NULL);*/
    gint count_date;
    gint i;
    count_date =  g_date_days_between(&priv->date1, &priv->date2) + 1;
    col = do_tree_view_add_column(view, "Debt", "Долг", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_DEBT);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_DEBT);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_money_no_negavite, GINT_TO_POINTER(COLUMN_DEBT), NULL);

    for ( i = 0; i < count_date; i++ ) {
        gchar *name, title[256];
        GDate date;
        date = priv->date1;
        name = g_strdup_printf("Date%d", i);
        g_date_add_days(&date, i);
        g_date_strftime(title, 255, "%d %b,%a", &date);
        col = do_tree_view_add_column(view, name, title, 100);
        g_free(name);
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_column_pack_start (col, renderer, TRUE);
        g_object_set(renderer, "xalign" , 1., NULL);

        gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_NO+i);
        gtk_tree_view_column_set_sort_column_id (col, COLUMN_NO+i);
        gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_money_no_negavite, GINT_TO_POINTER(COLUMN_NO+i), NULL);
    };
    col = do_tree_view_add_column(view, "Total", "Итого", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_NO+count_date);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_NO+count_date);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_money_bold_no_negative, GINT_TO_POINTER(COLUMN_NO+count_date), NULL);

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

    do_schedule_view_fill_model(embed);

	return object;
}

static void do_schedule_view_finalize (GObject *object)
{
    DoScheduleViewPrivate *priv;
	priv = DO_SCHEDULE_VIEW_GET_PRIVATE (object);
    g_free(priv->aliases);
    g_free(priv->ignore);

	G_OBJECT_CLASS (do_schedule_view_parent_class)->finalize (object);
}

static void do_schedule_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoScheduleViewPrivate *priv = DO_SCHEDULE_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIASES:
            g_value_set_string (value, priv->aliases);
			break;
        case PROP_DATE1:
            g_value_set_boxed(value, &priv->date1);
            break;
        case PROP_DATE2:
            g_value_set_boxed(value, &priv->date2);
            break;
        case PROP_SCHEDULE_POSTPONEMENT:
            g_value_set_int(value, priv->postponement);
            break;
        case PROP_SUPPLIER_IGNORE:
            g_value_set_string(value, priv->ignore);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
	/* no readable properties */
	g_assert_not_reached ();
}
static void do_schedule_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoScheduleViewPrivate *priv = DO_SCHEDULE_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIASES:
            if (!priv->aliases)
                priv->aliases = g_value_dup_string (value);
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
        case PROP_SCHEDULE_POSTPONEMENT:
            priv->postponement = g_value_get_int(value);
            break;

        case PROP_SUPPLIER_IGNORE:
            priv->ignore = g_value_dup_string(value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
}

static void do_schedule_view_class_init (DoScheduleViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_schedule_view_constructor;
	o_class->finalize     = do_schedule_view_finalize;
	o_class->get_property = do_schedule_view_get_property;
	o_class->set_property = do_schedule_view_set_property;

	g_type_class_add_private (o_class, sizeof (DoScheduleViewPrivate));

    g_object_class_install_property
        (o_class,
         PROP_ALIASES,
         g_param_spec_string("aliases",
                      "Список алиасов",
                      "Список алиасов",
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
				   PROP_SCHEDULE_POSTPONEMENT,
				   g_param_spec_int ("schedule-postponement",
 						      "Отсрочка платежа",
 						      "Отстрочка платежа",
 						      0,
 						      10000,
 						      0,
 						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY ));
    g_object_class_install_property
        (o_class,
         PROP_SUPPLIER_IGNORE,
         g_param_spec_string("supplier-ignore",
                      "Список не расчитываемых поставщиков",
                      "Список не расчитываемых поставщиков",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY ));


}

GtkWidget *do_schedule_view_new (const gchar *aliases, GDate *date1, GDate *date2, gint postponement, gchar *ignore)
{
	return g_object_new (DO_TYPE_SCHEDULE_VIEW,
			     "aliases", aliases,
			     "date1", date1,
			     "date2", date2,
			     "schedule-postponement", postponement,
			     "supplier-ignore", ignore,
			     "object-root-path", OBJECT_ROOT_PATH,
			     (gpointer) NULL);
}
static gboolean do_schedule_view_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void	do_schedule_view_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_schedule_view_view_get_title(DoView *view)
{
    DoScheduleViewPrivate *priv = DO_SCHEDULE_VIEW_GET_PRIVATE(view);
    gchar *retval;
    gchar buf[256],buf1[256];
    gchar *v1,*v2;
    g_date_strftime(buf, 255, "%d/%m/%Y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%Y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL , NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL , NULL);
    retval = g_strdup_printf("График платежей с %s по %s", v1, v2);
    g_free(v1);g_free(v2);
    return retval;
}
static GdkPixbuf  *do_schedule_view_view_get_icon(DoView *view)
{
    DoScheduleViewPrivate *priv = DO_SCHEDULE_VIEW_GET_PRIVATE(view);
    return priv->icon;
}
typedef struct {
    partner_key0_t partner_key0;
    gchar *name;
    gdouble debt;
    gdouble *debt_day;
} debt_t;

static gboolean do_schedule_view_get_debt(DoScheduleView *view, do_alias_t *alias, GSList **debts, const gchar *firm_suf);
static gchar  *get_firm_name(do_alias_t *alias)
{
    otdel_key0_t otdel_key0;
    otdel_rec_t otdel;
    int status;

    do_text_set(alias, otdel_key0.code, domino_division());
    status = do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return NULL;
    if ( status == DO_KEY_NOT_FOUND ) {
        do_log(LOG_ERR, "Отдел не найден");
        return NULL;
    }
    return do_text(alias, otdel.data.name);
}
static gchar  *get_firm_suf(do_alias_t *alias)
{
    otdel_key0_t otdel_key0;
    otdel_rec_t otdel;
    int status;

    do_text_set(alias, otdel_key0.code, domino_division());
    status = do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return NULL;
    if ( status == DO_KEY_NOT_FOUND ) {
        do_log(LOG_ERR, "Отдел не найден");
        return NULL;
    }
    return do_otdel_param(alias, &otdel, "07");
}

static gboolean do_schedule_view_fill_model(DoScheduleView *view)
{
    DoScheduleViewPrivate *priv = DO_SCHEDULE_VIEW_GET_PRIVATE(view);
    gint i;
    do_string_list_t *aliases_name;
    gboolean retval = TRUE;
    GSList *debts = NULL;

    do_start_long_operation(GTK_WIDGET(view));

    aliases_name = do_string_list_new_from_csv(priv->aliases);

    char *header_mark;
    gchar buf[256],buf1[256];
    gchar *v1,*v2, *suf;


    for ( i = 0; retval && i < aliases_name->count; i++ ) {
        do_alias_t *alias;
        alias = domino_alias_new(aliases_name->list[i]);


        if ( !alias || !do_alias_open(alias, TRUE)) {
            retval = FALSE;
            break;
        }
        v1 = get_firm_name(alias);
        suf = get_firm_suf(alias);
        if ( priv->firms )
            priv->firms = g_strdup_printf("%s,%s", priv->firms, v1);
        else
            priv->firms = v1;
        retval = do_schedule_view_get_debt(view, alias, &debts, suf);
        do_alias_free(alias);
    }

    g_date_strftime(buf, 255, "%d/%m/%y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL, NULL);
    header_mark = g_strdup_printf("График платежей за период с <b>%s</b> по <b>%s</b>, организации: <b>%s</b>",
                                  v1, v2, priv->firms );
    if ( priv->postponement > 0 )
        header_mark = g_strdup_printf("%s, для постащиков с отстрочкой от <b>%d</b> дней", header_mark, priv->postponement);
    if ( priv->ignore && priv->ignore[0] != '\0' ) {
        do_alias_t *alias;
        alias = domino_alias_new(NULL);
        do_alias_open(alias, DO_DB_PARTNER, DO_DB_END);
        header_mark = g_strdup_printf("%s, без задожности по", header_mark);
        do_string_list_t *tmp;
        tmp = do_string_list_new_from_csv(priv->ignore);
        partner_key0_t partner_key0;
        partner_rec_t partner;
        do_text_set(alias, partner_key0.g_code, "03");
        int i;
        for ( i = 0; i < tmp->count; i++ ) {
            partner_key0.code = atoi(tmp->list[i]);
            if ( partner_key0.code ) {
                if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) == DO_OK )
                    header_mark = g_strdup_printf("%s %s", header_mark, do_text(alias, partner.data.name));
            }
        }
        do_alias_free(alias);
    }

    g_free(v1);g_free(v2);
    gtk_label_set_markup(GTK_LABEL(priv->header), header_mark);

    do_string_list_free(aliases_name);
    do_end_long_operation(GTK_WIDGET(view));
    if ( retval ) {
        GSList *l;
        gdouble debt;
        gdouble *debt_day;
        gint count =  g_date_days_between(&priv->date1, &priv->date2) + 1;
        gint i;

        debt_day = g_new0(gdouble, count+1);
        debt = 0;
        GtkTreeIter iter_root;
        gtk_tree_store_clear(priv->model);

        gtk_tree_store_append(priv->model, &iter_root, NULL);
        gtk_tree_store_set(priv->model, &iter_root,
                           COLUMN_DESC, "<b>Итого</b>",
                           //COLUMN_TOTAL, pp,
                           //COLUMN_SUM, sum,
                           //COLUMN_SCHEDULE, schedule,
                           //COLUMN_CARD, sk->card,
                           -1
                           );
        for ( l = debts; l; l=l->next ) {
            GtkTreeIter iter;
            debt_t *item;
            gdouble sum;
            item = l->data;
            debt += item->debt;
            //do_log(LOG_ERR,"%f %f",debt, item->debt);//!!
            gtk_tree_store_append(priv->model, &iter, &iter_root);
            sum = item->debt;
            gtk_tree_store_set(priv->model, &iter,
                               COLUMN_DESC, item->name,
                               COLUMN_DEBT, item->debt,
                               -1);

            for ( i = 0; i < count; i++ ) {
                debt_day[i] += item->debt_day[i];
                sum +=item->debt_day[i];
                gtk_tree_store_set(priv->model, &iter,
                                   COLUMN_NO + i, item->debt_day[i],
                                   -1
                                   );
            }
            gtk_tree_store_set(priv->model, &iter,
                               COLUMN_NO + count, sum,
                               -1
                               );
        }
        gdouble sum;
        gtk_tree_store_set(priv->model, &iter_root,
                           COLUMN_DEBT, debt,
                           -1
                           );
        sum = debt;
        for ( i = 0; i < count; i++ ) {
            sum += debt_day[i];
            gtk_tree_store_set(priv->model, &iter_root,
                               COLUMN_NO + i, debt_day[i],
                               -1
                               );
        }
        gtk_tree_store_set(priv->model, &iter_root,
                           COLUMN_NO + count, sum,
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
typedef struct {
    partner_key0_t partner_key0;
    gchar *name;
    gint postponement;
    gboolean date_in_prowod;
} cache_t;
typedef struct PACK_STRUCT {
	BTI_CHAR subaccount[20];
    BTI_CHAR p_g_code[3];
    BTI_LONG p_code;
    BTI_CHAR other[20];
    BTI_BYTE debit_sum[8];
    BTI_BYTE credit_sum[8];
} saldo_info_t;


static cache_t *get_payment_postponement(do_alias_t *alias, GSList **cache, partner_key0_t *partner_key0, const gchar *suf);
static gboolean get_payment_day(do_alias_t *alias, saldo_info_t *info, BTI_LONG *day, gint postponement, gboolean date_in_prowod);
static gboolean do_schedule_view_get_debt(DoScheduleView *view, do_alias_t *alias, GSList **debts, const gchar *firm_suf)
{
    DoScheduleViewPrivate *priv = DO_SCHEDULE_VIEW_GET_PRIVATE(view);
    struct tm bd;
    struct tm ed;
    BTI_LONG date1;
    BTI_LONG date2;
    GSList *cache = NULL;
    saldo_info_t *rec;
    do_list_t *ignore = do_list_new(FALSE);

    g_date_to_struct_tm(&priv->date1, &bd);
    g_date_to_struct_tm(&priv->date2, &ed);
    do_date_set(&date1, bd);
    do_date_set(&date2, ed);

    do_extended_t *extended;
    saldo_key0_t saldo_key0;
    //saldo_struct_t saldo_struct;


    do_list_add(ignore, GINT_TO_POINTER(219));//!!!
    if ( priv->ignore ) {
        do_string_list_t *tmp;
        tmp = do_string_list_new_from_csv(priv->ignore);
        int i;
        for ( i = 0; i < tmp->count; i++ ) {
            int v;
            v = atoi(tmp->list[i]);
            if ( v )
                do_list_add(ignore, GINT_TO_POINTER(v));
        }
    }

    do_ret_list_t *list;

    do_text_set(alias, saldo_key0.division, domino_division());
	do_text_set_empty(saldo_key0.layer);
	do_text_set(alias, saldo_key0.account, "60");
	saldo_key0.account_total = 0;
	do_text_set_empty(saldo_key0.subaccount);
	//do_text_set(alias, saldo_key0.subaccount, "01");
	saldo_key0.subaccount_total = 0;
	do_text_set_empty(saldo_key0.p_g_code);
	saldo_key0.p_code=0;
	do_text_set_empty(saldo_key0.other);

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_SALDO);
    do_extended_set_key(extended, 0, &saldo_key0);
    do_extended_set_filter(extended, FALSE, 1, sizeof(saldo_key0.account));//, sizeof(saldo_struct.p_code));//, sizeof(saldo_key0.subaccount));
    do_extended_set_filter_term(extended, 0, DO_EXTENDED_TYPE_STRING,
                                             offsetof(saldo_struct_t, account),
                                             DO_EXTENDED_COMP_EQUAL,
                                             DO_EXTENDED_CONNECTION_NO,
                                             &saldo_key0.account);
    /*saldo_struct.p_code = 285;
    do_extended_set_filter_term(extended, 1, DO_EXTENDED_TYPE_INTEGER,
                                             offsetof(saldo_struct_t, p_code),
                                             DO_EXTENDED_COMP_EQUAL,
                                             DO_EXTENDED_CONNECTION_NO,
                                             &saldo_struct.p_code);*/

    //do_extended_set_reject_func(extended, (do_extended_reject_func)do_rz_ch_reject_func, &key);
    do_extended_set_fields(extended, 6,
                                        do_size_and_offset(saldo_struct_t, subaccount),
                                        do_size_and_offset(saldo_struct_t, p_g_code),
                                        do_size_and_offset(saldo_struct_t, p_code),
                                        do_size_and_offset(saldo_struct_t, other),
                                        sizeof(rec->debit_sum), offsetof(saldo_struct_t, debit_sum),
                                        sizeof(rec->credit_sum), offsetof(saldo_struct_t, credit_sum)
                                        );

    list = do_extended_get(extended, -1);
    do_extended_free(extended);
    if ( !list ) {
        do_list_free(ignore);
        return FALSE;
    }
    int i, k;
    gboolean retval, ignore_;
    retval = TRUE;
    for ( i = 0; i < do_ret_list_count(list); i++ ) {
        rec = do_ret_list_item_data(list, i);
        ignore_ = FALSE;
        for ( k = 0; k < ignore->count; k++) {
            if ( rec->p_g_code[0] == '0' && rec->p_g_code[1] == '3' && rec->p_code == GPOINTER_TO_INT(ignore->list[k])) {
                ignore_ = TRUE;
                break;
            }
        }
        if ( ignore_ ) continue;

        //if (  rec->p_code != 9 ) //!!
        //    continue;

        if ( rec->p_g_code[0] != '0' || rec->p_g_code[1] != '3') //!! || rec->p_code != 2 ) //!!
            continue;

        double sum;
        gboolean pre;
        pre = rec->subaccount[0] == '0' && rec->subaccount[1] == '2'; //fix me !!!
        sum = do_pdecimal(rec->credit_sum, 8, 2) - do_pdecimal(rec->debit_sum, 8, 2);
        if ( sum  > 0.001 || sum < -0.001 ) {
            cache_t *item;
            BTI_LONG pay_day;
            partner_key0_t *partner_key0;
            partner_key0 = (partner_key0_t*)rec->p_g_code;
            GSList *l;
            item = get_payment_postponement(alias, &cache, partner_key0, firm_suf);
            if ( !item ) {
                retval = FALSE;
                break;
            }
            if ( item->postponement < priv->postponement )
                continue;
            if ( !pre && sum > 0.001 ) {
                if ( !get_payment_day(alias, rec, &pay_day, item->postponement, item->date_in_prowod) ) {
                    retval = FALSE;
                    break;
                }
                if ( pay_day > date2  )
                    continue;
            }
            for ( l = *debts; l; l=l->next ) {
                debt_t *debt;
                debt = l->data;
                if ( !do_cmp(debt->partner_key0, *partner_key0) ) {
                    if ( pay_day < date1 || sum < -0.001 || pre )
                        debt->debt += sum;
                    else
                        debt->debt_day[pay_day - date1] += sum;
                    break;

                }
            }
            if ( !l ) {
                debt_t *debt;
                debt = g_new0(debt_t, 1);
                debt->debt_day = g_new0(gdouble, date2-date1 + 1);
                debt->name = item->name;
                do_cpy(debt->partner_key0, *partner_key0);
                if ( pay_day < date1 || sum < -0.001 || pre )
                    debt->debt += sum;
                else
                    debt->debt_day[pay_day - date1] += sum;
                *debts = g_slist_append(*debts, debt);
            }
        }
    }

    do_ret_list_free(list);
    do_list_free(ignore);

    return retval;


}
static cache_t *get_payment_postponement(do_alias_t *alias, GSList **cache, partner_key0_t *partner_key0, const gchar *suf)
{
    cache_t *item;
    GSList *l;
    for ( l = *cache; l; l=l->next ) {
        item = l->data;
        if ( !do_cmp(item->partner_key0, partner_key0) )
            return item;
    }
    partner_rec_t partner;
    if ( do_partner_get0(alias, &partner, partner_key0, DO_GET_EQUAL) != DO_OK ) {
        do_log(LOG_ERR, "Неизвестный партнер \"%s.%d\"", do_text(alias, partner_key0->g_code), partner_key0->code);
        return NULL;
    }
    item = g_new0(cache_t, 1);
    do_cpy(item->partner_key0, *partner_key0);
    gchar *v1;
    if ( partner_key0->code == 6 )
        partner_key0->code = 6;

    v1 = g_strdup_printf("%s%s", "ОТСРОЧ", suf);
    item->postponement = do_partner_param_int(alias, &partner, v1);
    if ( !item->postponement )
        item->postponement = do_partner_param_int(alias, &partner, "ОТСРОЧ");

    item->name = do_text(alias, partner.data.name);
    item->date_in_prowod = do_partner_param_int(alias, &partner, "ОТСР_ДОК");
    *cache = g_slist_append(*cache, item);
    return item;
}

static gboolean get_payment_day(do_alias_t *alias, saldo_info_t *info, BTI_LONG *day, gint postponement, gboolean date_in_prowod)
{
    prowod_key3_t prowod_key3, key;
    prowod_rec_t prowod;
    do_text_set(alias, prowod_key3.division, domino_division());
	do_text_set(alias, prowod_key3.credit_account, "60");
	do_text_set(alias, prowod_key3.credit_subaccount, "01");
    do_cpy(prowod_key3.credit_p_g_code, info->p_g_code);
    prowod_key3.credit_p_code = info->p_code;
    do_cpy(prowod_key3.credit_other, info->other);
    prowod_key3.date = 0;

    do_cpy(key, prowod_key3);

    if ( (do_prowod_get3(alias, &prowod, &prowod_key3, DO_GET_GE) != DO_OK) ||
         (memcmp(&key, &prowod_key3, sizeof(key) - sizeof(key.date))) ) {
        partner_key0_t partner_key0;
        partner_rec_t partner;
        char *name;
        do_cpy(partner_key0.g_code, info->p_g_code);
        do_cpy(partner_key0.code, info->p_code);
        if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) != DO_OK )
            name = do_strdup("");
        else
            name = do_text(alias, partner.data.name);

        do_log(LOG_ERR, "Невозможно найти проводку для партнера \"%s.%d\" %s аналитика \"%s\" возможно неправильно закрыт возврат",
          do_text(alias, info->p_g_code), info->p_code, name, do_text(alias, info->other));
        return FALSE;
    }
    *day= do_prowod_param_int(alias, &prowod, "КОММ");
    if ( *day < 100 || !date_in_prowod ) //!!
        *day = prowod.data.date + postponement;
    //do_log(LOG_ERR, "%d", postponement);
    return TRUE;

}
static gchar *do_print_get_html(DoScheduleView *view)
{
    DoScheduleViewPrivate *priv = DO_SCHEDULE_VIEW_GET_PRIVATE(view);
    do_template_t *templ;
    gchar *title;
    gchar *value;
    gint j;
    GString *string;

    templ = do_template_new("schedule", TRUE);
    if ( !templ )
        return NULL;
    do_template_cursor_t *cursor;
    do_template_cursor_t *line;
    char buf[256], buf1[256], *v1,*v2;
    g_date_strftime(buf, 255, "%d/%m/%y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL, NULL);
    title = g_strdup_printf("График платежей за период с <b>%s</b> по <b>%s</b>",
                                  v1, v2);
    if ( title > 0 )
        title = g_strdup_printf("%s, для постащиков с отстрочкой от <b>%d</b> дней", title, priv->postponement);

    g_free(v1);g_free(v2);
    cursor = do_template_cursor(templ);
    line   = do_template_section(templ, "Line");
    if ( !line ) {
        do_log(LOG_ERR, "Неправильный шаблон \"%s\"", "schedule");
        do_template_free(templ);
        return NULL;
    }

    string = g_string_new("<tr align=\"center\">");
    //indx = 0;
    g_string_append_printf(string, "<td colspan=\"%d\">%s</td>", 1, "Описание");
    g_string_append_printf(string, "<td colspan=\"%d\">%s</td>", 1, "Долг");
    gint count =  g_date_days_between(&priv->date1, &priv->date2) + 1;
    for ( j = 0; j < count; j++ ) {
        GDate date;
        gchar title[256];
        date = priv->date1;
        g_date_add_days(&date, j);
        g_date_strftime(title, 255, "%d %b,%a", &date);
        g_string_append_printf(string, "<td colspan=\"%d\">%s</td>", 1, title);
    }
    g_string_append_printf(string, "<td colspan=\"%d\">%s</td>", 1, "Итого");
    g_string_append(string, "</tr>");

    value = g_string_free(string, FALSE);
    do_template_set(templ,
                    "Title", title,
                    "Firms", priv->firms,
//                    "Footer", footer,
                    "Header", value, NULL);
    g_free(value);
    do_template_cursor_run_step_for(cursor, "Line");
    GtkTreePath *path;
    GValue val = {0,}, vld = {0,}, vld1 = {0,};
    GtkTreeIter iter;
    path = gtk_tree_path_new_first();
    gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path);
    string = g_string_new("<tr align=\"center\">");
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, COLUMN_DESC, &val);
    g_string_append_printf(string, "<td>%s</td>", g_value_get_string(&val));
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, COLUMN_DEBT, &vld);
    g_string_append_printf(string, "<td nowrap><b>%s<b></td>", do_money_format(g_value_get_double(&vld), FALSE));
    for ( j = 0; j < count; j++ ) {
        GValue vld = {0,};
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, COLUMN_NO+j, &vld);
        g_string_append_printf(string, "<td nowrap><b>%s<b></td>", do_money_format(g_value_get_double(&vld), FALSE));
    }
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, COLUMN_NO+count, &vld1);
    g_string_append_printf(string, "<td nowrap><b>%s<b></td>", do_money_format(g_value_get_double(&vld1), FALSE));
    g_string_append(string, "</tr>");
    value = g_string_free(string, FALSE);
    do_template_set(templ, "Line", value, NULL);
    g_free(value);
    do_template_cursor_first_run(line);
    gtk_tree_path_down(path);
    while ( gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path) ) {
        GValue val = {0,}, vld = {0,}, vld1 = {0,};
        GtkTreeIter iter;
        gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path);
        string = g_string_new("<tr align=\"center\">");
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, COLUMN_DESC, &val);
        g_string_append_printf(string, "<td align=\"left\">%s</td>", g_value_get_string(&val));
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, COLUMN_DEBT, &vld);
        g_string_append_printf(string, "<td nowrap>%s</td>", do_money_format(g_value_get_double(&vld), FALSE));
        for ( j = 0; j < count; j++ ) {
            GValue vld = {0,};
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, COLUMN_NO+j, &vld);
            g_string_append_printf(string, "<td nowrap>%s</td>", do_money_format(g_value_get_double(&vld), FALSE));
        }
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, COLUMN_NO+count, &vld1);
        g_string_append_printf(string, "<td nowrap>%s</td>", do_money_format(g_value_get_double(&vld1), FALSE));
        g_string_append(string, "</tr>");
        value = g_string_free(string, FALSE);
        do_template_set(templ, "Line", value, NULL);
        g_free(value);
        do_template_cursor_first_run(line);

        gtk_tree_path_next(path);

    }
    do_template_cursor_skip(cursor);
    do_template_cursor_run(cursor);

    value = do_template_text(templ);
    do_template_free(templ);
    return value;

}
static void  do_schedule_view_do_print(DoView *view)
{
    //DoScheduleViewPrivate *priv = DO_SCHEDULE_VIEW_GET_PRIVATE(view);
    gchar *html_string = do_print_get_html(DO_SCHEDULE_VIEW(view));
    if (html_string) {
#ifndef MINIMAL_WIN
        GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(view));
        GtkWidget *html;
        GtkWidget *notebook;
        html = do_html_view_new();
        notebook = do_window_get_notebook(DO_WINDOW(window));
        do_notebook_add_tab(DO_NOTEBOOK(notebook), DO_VIEW(html), -1, TRUE);
        g_object_set(html, "html-string", html_string, "title", "Отчет", NULL);
#else
        do_show_html(html_string);
#endif
        g_free(html_string);
    }
}
static gboolean    do_schedule_view_can_do_print(DoView *view)
{
    return TRUE;
}
static void  do_schedule_view_do_print_now(DoView *view)
{
    //DoScheduleViewPrivate *priv = DO_SCHEDULE_VIEW_GET_PRIVATE(view);
    gchar *html_string = do_print_get_html(DO_SCHEDULE_VIEW(view));
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
