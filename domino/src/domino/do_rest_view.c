
#include "do_rest_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "domino.h"
#include "do_tree_view.h"
#include "../misc/template.h"
#include <domino.h>
#include "hig.h"
#include "do_icons.h"
#include "do_utilx.h"
#include <gdk/gdk.h>
#include <time.h>

#define DO_REST_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_REST_VIEW, DoRestViewPrivate))

#define OBJECT_ROOT_PATH "RestView"
#define DEFAULT_COL_PACK_FORM_WIDTH 400
#define DEFAULT_COL_DEFAULT_WIDTH 60

// View interface
static void do_rest_view_view_init(DoViewIface *iface);

static gboolean    do_rest_view_view_do_grab_focus(DoView *view);
static void	       do_rest_view_view_do_close(DoView *view);
static const char *do_rest_view_view_get_title(DoView *view);
static GdkPixbuf  *do_rest_view_view_get_icon(DoView *view);
static gboolean    do_rest_view_fill_model(DoRestView *view);
static gboolean    do_rest_view_can_do_print(DoView *view);
static void        do_rest_view_do_print(DoView *view);
static void        do_rest_view_do_print_now(DoView *view);
void do_rest_view_data_func_description(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         DoRestView *view);

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
    COLUMN_IN,
    COLUMN_OUT,
    COLUMN_NO ,

};

struct _DoRestViewPrivate
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

G_DEFINE_TYPE_WITH_CODE (DoRestView, do_rest_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_rest_view_view_init)
                                    );

static void do_rest_view_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_rest_view_view_do_grab_focus;
    iface->do_close      = do_rest_view_view_do_close;
    iface->get_title     = do_rest_view_view_get_title;
    iface->get_icon      = do_rest_view_view_get_icon;
    iface->do_print      = do_rest_view_do_print;
    iface->do_print_now      = do_rest_view_do_print_now;
    iface->can_do_print  = do_rest_view_can_do_print;

    //iface->do_edit       = do_rest_view_view_do_edit;
    //iface->can_do_edit   = do_rest_view_view_can_do_edit;
    //iface->do_delete       = do_rest_view_view_do_delete;
    //iface->do_insert       = do_rest_view_view_do_insert;
    //iface->can_do_delete   = do_rest_view_view_can_do_delete;
    //iface->can_do_insert   = do_rest_view_view_can_do_insert;
}

static void do_rest_view_init(DoRestView *view)
{
#if GTK_MAJOR_VERSION == 2
	DoRestViewPrivate *priv = DO_REST_VIEW_GET_PRIVATE (view);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_FILE, DO_VIEW_ICON_SIZE, 0, NULL);
#endif
}

static GtkTreeStore *do_rest_view_create_model(DoRestView *embed)
{

    gint count =  3;

    GtkTreeStore *model;
    GType *types;

    types = g_new(GType, count);
    types[0] = G_TYPE_STRING;
    types[1] = G_TYPE_DOUBLE;
    types[2] = G_TYPE_DOUBLE;


    model = gtk_tree_store_newv(count, types);

    return model;
}
static void do_rest_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoRestView *view)
{
    if ( gtk_tree_view_row_expanded(tree_view, path) )
        gtk_tree_view_collapse_row(tree_view, path);
    else
        gtk_tree_view_expand_row(tree_view, path, FALSE);
}

static GObject *do_rest_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkTreeStore        *model;
    DoTreeView          *view;
	DoRestView      *embed;
	GtkTreeView         *tree_view;
	DoRestViewPrivate   *priv;

	object = G_OBJECT_CLASS (do_rest_view_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_REST_VIEW_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_REST_VIEW(object);
	g_object_get(object, "tree-view", &tree_view, NULL);

    g_signal_connect (tree_view, "row-activated", G_CALLBACK(do_rest_view_row_activated), object);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(tree_view), FALSE);

	priv->tree_view = tree_view;
    priv->selection = gtk_tree_view_get_selection(tree_view);

	priv->model = model = do_rest_view_create_model(embed);
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
             (GtkTreeCellDataFunc)do_rest_view_data_func_description, object, NULL);*/

    col = do_tree_view_add_column(view, "In", "Входящий остаток", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_IN);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_IN);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_money_bold_total, GINT_TO_POINTER(COLUMN_IN), NULL);

    col = do_tree_view_add_column(view, "Out", "Исходящий остаток", 100);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    g_object_set(renderer, "xalign" , 1., NULL);

    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_OUT);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_OUT);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
         do_tree_cell_data_func_money_bold_total, GINT_TO_POINTER(COLUMN_OUT), NULL);

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

    do_rest_view_fill_model(embed);

	return object;
}

static void do_rest_view_finalize (GObject *object)
{
    DoRestViewPrivate *priv;
	priv = DO_REST_VIEW_GET_PRIVATE (object);
    g_free(priv->units);

	G_OBJECT_CLASS (do_rest_view_parent_class)->finalize (object);
}

static void do_rest_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoRestViewPrivate *priv = DO_REST_VIEW_GET_PRIVATE (object);

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
static void do_rest_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoRestViewPrivate *priv = DO_REST_VIEW_GET_PRIVATE (object);

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

static void do_rest_view_class_init (DoRestViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_rest_view_constructor;
	o_class->finalize     = do_rest_view_finalize;
	o_class->get_property = do_rest_view_get_property;
	o_class->set_property = do_rest_view_set_property;

	g_type_class_add_private (o_class, sizeof (DoRestViewPrivate));

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

GtkWidget *do_rest_view_new (const gchar *units, GDate *date1, GDate *date2)
{
	return g_object_new (DO_TYPE_REST_VIEW,
			     "units", units,
			     "date1", date1,
			     "date2", date2,
			     "object-root-path", OBJECT_ROOT_PATH,
			     (gpointer) NULL);
}
static gboolean do_rest_view_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void	do_rest_view_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_rest_view_view_get_title(DoView *view)
{
    DoRestViewPrivate *priv = DO_REST_VIEW_GET_PRIVATE(view);
    gchar *retval;
    gchar buf[256],buf1[256];
    gchar *v1,*v2;
    g_date_strftime(buf, 255, "%d/%m/%Y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%Y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL , NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL , NULL);
    retval = g_strdup_printf("Остаток за период с %s по %s", v1, v2);
    g_free(v1);g_free(v2);
    return retval;
}
static GdkPixbuf  *do_rest_view_view_get_icon(DoView *view)
{
    DoRestViewPrivate *priv = DO_REST_VIEW_GET_PRIVATE(view);
    return priv->icon;
}
typedef struct {
    gchar *desc;
    gdouble in_rest;
    gdouble out_rest;
} rest_day_t;
typedef struct {
    sklad_key0_t sklad_key0;
    gchar  *name;
    GSList *days;
    gdouble in_rest;
    gdouble out_rest;
} rest_t;

static gboolean do_rest_view_get_rest(do_alias_period_t *alias_period, GSList **rest, const gchar *unit);

static gboolean do_rest_view_fill_model(DoRestView *view)
{
    DoRestViewPrivate *priv = DO_REST_VIEW_GET_PRIVATE(view);
    gint i;
    do_string_list_t *units;
    gboolean retval = TRUE;
    GSList *rests = NULL;
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
        if ( aliases->count > 1 ) {
            do_log(LOG_ERR, "Невозможно выполнить отчет для разных периодов");
            return FALSE;
        }
        for ( j = 0 ; retval && j < aliases->count; j++ ) {
            do_alias_period_t *p;
            p = aliases->list[j];
            if ( (retval = do_alias_open(p->alias, TRUE)) != TRUE )
                break;
            retval = do_rest_view_get_rest(p, &rests, units->list[i]);
        }

    }
    do_string_list_free(units);
    domino_meta_alias_free(meta);

    g_date_strftime(buf, 255, "%d/%m/%y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL, NULL);


    header_mark = g_strdup_printf("Выручка за период с <b>%s</b> по <b>%s</b>, Аптеки: <b>%s</b>",
                                  v1, v2, do_get_unit_desc(NULL, priv->units) );
    g_free(v1);g_free(v2);
    gtk_label_set_markup(GTK_LABEL(priv->header), header_mark);

    do_end_long_operation(GTK_WIDGET(view));
    if ( retval ) {
        GSList *l;
        rest_t root;

        memset(&root, 0, sizeof(root));

        GtkTreeIter iter_root;
        gtk_tree_store_clear(priv->model);

        for ( l = rests; l; l=l->next ) {
            GtkTreeIter iter_unit;
            GSList *day;
            rest_t *rest;


            rest = l->data;
            gtk_tree_store_append(priv->model, &iter_unit, NULL);

            gtk_tree_store_set(priv->model, &iter_unit,
                               COLUMN_DESC, rest->name,
                               COLUMN_IN, rest->in_rest,
                               COLUMN_OUT, rest->out_rest,
                               -1);

            for ( day = rest->days; day; day=day->next ) {
                GtkTreeIter iter;
                rest_day_t *rec;
                rec = day->data;

                gtk_tree_store_append(priv->model,&iter, &iter_unit);
                gtk_tree_store_set(priv->model, &iter,
                                   COLUMN_DESC, rec->desc,
                                   COLUMN_IN, rec->in_rest,
                                   COLUMN_OUT, rec->out_rest,
                                   -1
                                   );
            }
            root.in_rest+=rest->in_rest;
            root.out_rest+=rest->out_rest;
        }

        gtk_tree_store_append(priv->model, &iter_root, NULL);
        gtk_tree_store_set(priv->model, &iter_root,
                               COLUMN_DESC, "<b>По всем аптекам</b>",
                               COLUMN_IN, root.in_rest,
                               COLUMN_OUT, root.out_rest,
                           -1
                           );

    }
    if ( retval ) {
        /*GtkTreePath *path;
        path = gtk_tree_path_new_first();
        gtk_tree_view_expand_row(priv->tree_view, path, FALSE);*/
    }

    return retval;
}

static gboolean do_rest_view_get_rest(do_alias_period_t *alias_period, GSList **rests, const gchar *unit)
{

    do_alias_t *alias = alias_period->alias;

    document_key3_t document_key3, key, key1;
    document_rec_t document;
    int status;
    int indx;

    for ( indx = 1; indx >= 0; indx-- ) {

        if ( !indx )
            do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
        else
            do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));

        do_text_set(alias, document_key3.sklad, unit);
        document_key3.date = 0;
        do_date_set(&key1.date, alias_period->date_b);
        do_date_set(&key.date, alias_period->date_e);
        document_key3.time = 0;

        do_cpy(key.dtype, document_key3.dtype);
        do_cpy(key.sklad, document_key3.sklad);

        status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);

        while ( status == DO_OK ) {
            if (
                 do_cmp(key.dtype, document_key3.dtype) ||
                 do_cmp(key.sklad, document_key3.sklad) ||
                 document_key3.date > key.date
                ) break;


            if ( document.data.accepted ) {
                rest_t *item;
                GSList *l;
                item = NULL;

                for ( l = *rests; l; l=l->next )
                    if ( !do_cmp(document.data.sklad, ((rest_t*)l->data)->sklad_key0.code) ) {
                        item = l->data;
                        break;
                    }
                if ( !item ) {
                    sklad_rec_t sklad;
                    item = g_new0(rest_t, 1);
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
                    *rests = g_slist_append(*rests, item);

                }
                if ( !indx )
                {
                    item->out_rest += do_document_sum(alias, &document, 11);
                    if ( document.data.date < key1.date )
                        item->in_rest += do_document_sum(alias, &document, 11);
                }
                else {
                    item->out_rest -= do_document_sum(alias, &document, 11);
                    if ( document.data.date < key1.date )
                        item->in_rest -= do_document_sum(alias, &document, 11);
                }

            }
            status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
        }

        if ( status == DO_ERROR )
            return FALSE;
    }
    return TRUE;
}
static void do_print_item(do_template_t *templ, do_template_cursor_t *line, GtkTreeModel *model, GtkTreeView *view, GtkTreePath *path);
static gchar *do_print_get_html(DoRestView *view)
{
    DoRestViewPrivate *priv = DO_REST_VIEW_GET_PRIVATE(view);
    do_template_t *templ;
    gchar *title;
    gchar *value;

    templ = do_template_new("rest", TRUE);
    if ( !templ )
        return NULL;
    do_template_cursor_t *cursor;
    do_template_cursor_t *line;
    char buf[256], buf1[256], *v1,*v2;
    g_date_strftime(buf, 255, "%d/%m/%y", &priv->date1);
    g_date_strftime(buf1, 255, "%d/%m/%y", &priv->date2);
    v1 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
    v2 = g_locale_to_utf8(buf1, -1, NULL, NULL, NULL);
    title = g_strdup_printf("Остаток за период с <b>%s</b> по <b>%s</b>",
                                  v1, v2);
    g_free(v1);g_free(v2);
    cursor = do_template_cursor(templ);
    line   = do_template_section(templ, "Line");
    if ( !line ) {
        do_log(LOG_ERR, "Неправильный шаблон \"%s\"", "rest");
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
static void  do_rest_view_do_print(DoView *view)
{
    //DoRestViewPrivate *priv = DO_REST_VIEW_GET_PRIVATE(view);
    gchar *html_string = do_print_get_html(DO_REST_VIEW(view));
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
static gboolean    do_rest_view_can_do_print(DoView *view)
{
    return TRUE;
}
static void  do_rest_view_do_print_now(DoView *view)
{
    //DoRestViewPrivate *priv = DO_REST_VIEW_GET_PRIVATE(view);
    gchar *html_string = do_print_get_html(DO_REST_VIEW(view));
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
        GValue desc = {0,}, in = {0,}, out = {0,};
        GtkTreeIter iter;
        gtk_tree_model_get_iter(model, &iter, path);
        gtk_tree_model_get_value(model, &iter, COLUMN_DESC, &desc);
        gtk_tree_model_get_value(model, &iter, COLUMN_IN, &in);
        gtk_tree_model_get_value(model, &iter, COLUMN_OUT, &out);

        gchar *desc_str, *in_str, *out_str;

        desc_str = g_value_dup_string(&desc);
        in_str = do_money_format(g_value_get_double(&in), FALSE);
        out_str = do_money_format(g_value_get_double(&out), FALSE);

        do_template_set(templ, "Desc", desc_str,
                               "In", in_str,
                               "Out", out_str,
                               NULL);
        g_free(desc_str);g_free(in_str);g_free(out_str);

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
