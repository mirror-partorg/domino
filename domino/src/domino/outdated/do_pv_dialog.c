

#include <string.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>

#include "do_pv_dialog.h"
#include "do_icons.h"
#include "do_entry_date_box.h"
#include "do_entry.h"
#include "do_entry_calc.h"
#include "do_tree_view.h"
#include "do_utilx.h"
#include "do_validate.h"
#include "domino.h"
#include "hig.h"
#include <dolib.h>
#include <domino.h>

enum
{
	PROP_0,
	PROP_CONTEXT,
	PROP_ALIAS,
};

#define DO_PV_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_PV_DIALOG, DoPvDialogPrivate))
#define DO_TYPE_PV_DIALOG_FLAGS do_pv_dialog_flags_get_type()

#define PAGE_BREAK "<br clear=all style='mso-special-character:line-break;page-break-before:always'>"
#define HTML_HEADER "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head><body>"
#define HTML_FOOTER "</body></html>"

#define OBJECT_ROOT_PATH "PvDialog"
#define DEFAULT_WINDOW_POSITION "center-on-parent"
#define DEFAULT_WINDOW_SIZE "600x520"
#define PV_TEMPLATE "pv_list"
#define PATH_BANKNOTES "Банкноты"
#define PATH_COINS     "Монеты"

enum
{
	BAG_ENTRY,
	DATE_ENTRY,
	SUM_ENTRY,
	N_ENTRIES
};

struct _DoPvDialogPrivate
{

	GtkWidget *widget[N_ENTRIES];
	GtkWidget *entry[N_ENTRIES];

	GtkWidget         *do_view;
	GtkTreeView       *tree_view;
    GtkTreeStore      *stores;
    GtkTreeViewColumn *total_col;
    GtkWidget         *footer;

	DoContext  *context;
	gchar      *alias_name;
	gchar      *insert_text;

};


G_DEFINE_TYPE (DoPvDialog, do_pv_dialog, GTK_TYPE_DIALOG)

static GtkWidget *add_field(DoPvDialog *dialog, gint id, GtkWidget *widget);

static void entry_activate(GtkWidget *entry, DoPvDialog *dialog);
static gboolean fill_entry(DoPvDialog *dialog);
static void do_pv_dialog_row_activated(GtkTreeView *tree_view, GtkTreePath *path,
                                                        GtkTreeViewColumn *column,
                                                        DoPvDialog *dialog);
static void do_pv_dialog_editing_started(GtkCellRenderer *renderer,
                                                        GtkCellEditable *editable,
                                                        gchar           *path,
                                                        DoPvDialog *dialog);

static gboolean do_pv_dialog_update_footer(DoPvDialog *dialog);

static void do_pv_dialog_init(DoPvDialog *dialog)
{

}
static void cell_edited (GtkCellRendererText *cell,
             const gchar         *path_string,
             const gchar         *new_text,
             DoPvDialog          *dialog)
{
    DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (dialog);
    GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
    GtkTreeIter iter;
    gtk_tree_model_get_iter (GTK_TREE_MODEL(priv->stores), &iter, path);
    gtk_tree_store_set (GTK_TREE_STORE (priv->stores), &iter, 1, atoi(new_text), -1);
    gtk_tree_path_free (path);
    do_pv_dialog_update_footer(dialog);
}
static gchar *do_pv_dialog_is_digital(const gchar *string, gint length)
{
    if (!length)
        return NULL;
    gchar *buf = g_locale_to_utf8 (string, -1, NULL, NULL, NULL);
    if (!buf)
        return NULL;
    gchar *result = do_malloc(sizeof (gunichar) * strlen (buf) + 1);
    gchar *p;
    gunichar out;
    gint nlen = 0;
    for (p = buf; p && *p !='\0' && (p - buf < length);
          p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        if (!g_unichar_isdigit(out)) {
            g_free(result);
            g_free(buf);
            return NULL;
        }
        nlen += g_unichar_to_utf8(out, result + nlen);
    }
    if (!nlen) {
        do_free(result);
        g_free(buf);
        return NULL;
    }
    result[nlen] = '\0';
    g_free(buf);
    return result;
}
static gboolean do_pv_dialog_key_press(GtkWidget   *widget,
                                                        GdkEventKey *event,
                                                        DoPvDialog *dialog)
{
    DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (dialog);

	gchar *text;
	guint mask = gtk_accelerator_get_default_mod_mask ();

	if ((event->state & mask) == 0)
	{
        text = (do_pv_dialog_is_digital(event->string, event->length));
        if (text) {
            GtkTreePath *path;
            gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
            if (path) {
                GtkTreeIter iter;
                if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->stores), &iter, path)) {
                    priv->insert_text = g_strdup(text);
                    gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, priv->total_col, TRUE);
                }
                gtk_tree_path_free(path);
            }
            g_free(text);
        }

	}
	return FALSE;
}
static gboolean  do_pv_dialog_calc_sum(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gdouble *sum)
{
    GValue total = {0,};
    GValue value = {0,};
    gtk_tree_model_get_value(model, iter, 4, &value);
    gtk_tree_model_get_value(model, iter, 1, &total);
    *sum += g_value_get_double(&value)*g_value_get_int(&total);
    return FALSE;
}
static gboolean do_pv_dialog_update_footer(DoPvDialog *dialog)
{
    DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (dialog);

    gdouble sum1,sum2;
    gchar *buf;
    gboolean retval;

    sum1 = 0;
    gtk_tree_model_foreach(GTK_TREE_MODEL(priv->stores), (GtkTreeModelForeachFunc)do_pv_dialog_calc_sum, &sum1);
    g_object_get(priv->entry[SUM_ENTRY], "value", &sum2, NULL);

    retval = sum1 && sum1 <= sum2 + 0.009 && sum1 >= sum2 - 0.009;
    if (retval)
        buf = g_strdup_printf("<b>Сумма:</b> <span foreground=\"green\">%.2fр.</span> <b>:</b> %.2fр.", sum1, sum2);
    else
        buf = g_strdup_printf("<b>Сумма:</b> <span foreground=\"red\">%.2fр.</span> <b>:</b> %.2fр.", sum1, sum2);
    gtk_label_set_markup(GTK_LABEL(priv->footer), buf);
    g_free(buf);
    return retval;
}
static void do_pv_dialog_sum_editable_changed(GtkEditable *editable, GParamSpec *pspec, DoPvDialog *dialog)
{
    do_pv_dialog_update_footer(dialog);
}
static GObject *do_pv_dialog_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	GtkWindow *window;
	GtkDialog *dialog;
	DoPvDialog *pv_dialog;
	DoPvDialogPrivate *priv;
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;
	GtkWidget *vbox;
    GtkWidget *l;
    GtkWidget *t;
    GtkWidget *tb;
    DoTreeView *view;
    gchar *value, *title;
    GtkAccelGroup *accel_group;

	int row = 0;
	object = G_OBJECT_CLASS (do_pv_dialog_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_PV_DIALOG_GET_PRIVATE(object);
	window = GTK_WINDOW (object);
	dialog = GTK_DIALOG (object);
	vbox   = gtk_dialog_get_content_area(dialog);
	pv_dialog = DO_PV_DIALOG(object);

    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);


	gtk_box_set_spacing (GTK_BOX (vbox), 2); /* Message has 24, we want 12 = 2 + 2 * 5 */

	gtk_window_set_icon_name (window, DO_STOCK_DOMINO_APPS);
	value = domino_get_default_location_name();
	title = g_strdup_printf("Печать перепроводительной ведомости %s", value);
	g_free(value);
	gtk_window_set_title (window, title);
	g_free(title);

    g_signal_connect (window, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), OBJECT_ROOT_PATH);

    //gtk_dialog_set_has_separator(dialog, FALSE);

    t = hig_workarea_create();

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, FALSE, 0);

    hig_workarea_add_section_title(t, &row, "Данные ");

    l = add_field(pv_dialog, BAG_ENTRY, do_entry_new());
    g_object_set(l, "regex-mask", "^([0-9]+)$", NULL);
    hig_workarea_add_row(t, &row, "Номер сумки:", l, NULL);

	l = add_field(pv_dialog, DATE_ENTRY, do_entry_date_box_new(DO_ENTRY_DATE_FLAGS_DEFAULT));
    hig_workarea_add_row(t, &row, "Дата инкассации:", l, NULL);

	l = add_field(pv_dialog, SUM_ENTRY, do_entry_calc_new());
	g_object_set(l, "precision", 2, "min-value", 0.01, NULL);

	g_signal_connect(l, "notify::text", G_CALLBACK(do_pv_dialog_sum_editable_changed), object);

    hig_workarea_add_row(t, &row, "Сумма:", l, NULL);

    hig_workarea_finish( t, &row );
    priv->stores = gtk_tree_store_new(5, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_DOUBLE);

    tb = priv->do_view = do_tree_view_new("PvTreeView");
    view = (DoTreeView*)tb;
    g_object_get(tb, "tree-view", &priv->tree_view, NULL);
    g_object_set(tb, "model", priv->stores, NULL);
    priv->footer = gtk_label_new(NULL);
    gtk_widget_show(priv->footer);
    g_object_set(priv->footer, "xalign", 0.01, NULL);
    do_tree_view_pack_footer(DO_TREE_VIEW(tb), priv->footer);
    do_pv_dialog_update_footer((DoPvDialog*)object);

    g_signal_connect (priv->tree_view, "key-press-event",
                      G_CALLBACK (do_pv_dialog_key_press), object);

    g_signal_connect (priv->tree_view, "row-activated",
                      G_CALLBACK (do_pv_dialog_row_activated), object);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (tb), TRUE, TRUE, 0);


    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(view, "N", "", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);

    r = gtk_cell_renderer_text_new();
    priv->total_col = col = do_tree_view_add_column(view, "Count", "Листов/Штук", 100);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    g_object_set (r, "xalign", (gfloat)0.5,
                    "editable", TRUE,
                    NULL);
    g_signal_connect (r, "edited",
                    G_CALLBACK (cell_edited), object);
    g_signal_connect (r, "editing-started",
                    G_CALLBACK (do_pv_dialog_editing_started), object);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(view, "N1", "Номинал", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 2);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(view, "Sum", "Сумма", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 3);

	gtk_dialog_add_button (dialog, "Отмена", GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (dialog, "Просмотр", GTK_RESPONSE_YES);
	gtk_dialog_add_button (dialog, "Ok", GTK_RESPONSE_ACCEPT);

    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(window), OBJECT_ROOT_PATH,
              "window-position", DEFAULT_WINDOW_POSITION,
              "window-size", DEFAULT_WINDOW_SIZE,
               NULL);

	gtk_widget_show_all (vbox);
	return object;
}

static void do_pv_dialog_finalize (GObject *object)
{
	DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (object);
	if (priv->context)
        g_object_unref(priv->context);
	if (priv->alias_name)
		g_free(priv->alias_name);
	G_OBJECT_CLASS (do_pv_dialog_parent_class)->finalize (object);
}

static void do_pv_dialog_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (object);

	switch (prop_id)
	{
        case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string(value);
            break;
        case PROP_CONTEXT:
            if (!priv->context) {
                priv->context = do_context_copy(g_value_get_object(value));
                fill_entry(DO_PV_DIALOG (object));
            }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_pv_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_ALIAS:
            g_value_set_string(value, priv->alias_name);
        case PROP_CONTEXT:
            g_value_set_object (value, do_context_copy(priv->context));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;

    }
}
static void do_pv_dialog_class_init (DoPvDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_pv_dialog_constructor;
	object_class->finalize = do_pv_dialog_finalize;
	object_class->get_property = do_pv_dialog_get_property;
	object_class->set_property = do_pv_dialog_set_property;

	g_type_class_add_private (object_class, sizeof (DoPvDialogPrivate));

    g_object_class_install_property
        (object_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (object_class,
				   PROP_CONTEXT,
				   g_param_spec_object ("context",
 						      "Контекст документа",
 						      "Контекст документа описания клавиши",
 						      DO_TYPE_CONTEXT,
 						      G_PARAM_READWRITE));
}

GtkWidget *do_pv_dialog_new (GtkWidget *parent, const gchar *alias_name, DoContext *context)
{
	return g_object_new (DO_TYPE_PV_DIALOG,
			     "context", context,
			     "alias-name", alias_name,
			     (gpointer) NULL);
}

static GtkWidget *add_field(DoPvDialog *dialog, gint id, GtkWidget *widget)
{
	DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (dialog);
	g_assert (id < N_ENTRIES);
	priv->widget[id] = widget;
	if (GTK_IS_ENTRY(widget)) {
        g_signal_connect(widget, "activate", G_CALLBACK(entry_activate), dialog);
        priv->entry[id] = widget;
	}
    else
	if (DO_IS_ENTRY_DATE(widget)) {
        priv->entry[id] = widget;
//	    priv->entry[id] = do_entry_date_entry(DO_ENTRY_DATE(widget));
        g_signal_connect(priv->entry[id], "activate", G_CALLBACK(entry_activate), dialog);
	}
    else
	if (DO_IS_ENTRY_BOX(widget)) {
        priv->entry[id] = do_entry_box_entry(DO_ENTRY_BOX(widget));
//	    priv->entry[id] = do_entry_date_entry(DO_ENTRY_DATE(widget));
        g_signal_connect(priv->entry[id], "activate", G_CALLBACK(entry_activate), dialog);
	}
    else
    if (GTK_IS_BUTTON(widget))
        g_signal_connect(widget, "activate", G_CALLBACK(entry_activate), dialog);
    else
	if (GTK_IS_COMBO_BOX(widget)) {
        priv->entry[id] = gtk_bin_get_child (GTK_BIN (widget));
        if (priv->entry[id])
            g_signal_connect(priv->entry[id], "activate", G_CALLBACK(entry_activate), dialog);
	}
    return widget;
}
static gboolean do_pv_dialog_tree_grab_focus(DoPvDialog *view)
{
    DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE(view);
    do_tree_view_grab_focus(DO_TREE_VIEW(priv->do_view));
    if (priv->stores) {
        GtkTreeIter iter;
        if (gtk_tree_model_get_iter_first((GtkTreeModel*)priv->stores, &iter)) {
            GtkTreePath *path = gtk_tree_model_get_path((GtkTreeModel*)priv->stores, &iter);
            if (path) {
                gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, priv->total_col, FALSE);
                gtk_tree_path_free(path);
            }
        }
    }
    return TRUE;
}

static gboolean do_pv_dialog_valid(DoPvDialog *dialog)
{
	DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (dialog);
    gint i;
    gdouble sum, sum1;
    for (i = 0; i < N_ENTRIES; i++)
        if (DO_IS_VALIDATE(priv->entry[i]) && !do_validate_valid(DO_VALIDATE(priv->entry[i])))
            return FALSE;
    sum = 0;
    gtk_tree_model_foreach(GTK_TREE_MODEL(priv->stores), (GtkTreeModelForeachFunc)do_pv_dialog_calc_sum, &sum);
    g_object_get(priv->entry[SUM_ENTRY], "value", &sum1, NULL);
    if (sum >= sum1 + 0.009 || sum <= sum1 - 0.009) {
        if (sum == 0 && gtk_widget_has_focus(priv->widget[SUM_ENTRY]))
            do_pv_dialog_tree_grab_focus(dialog);
        else
            DOMINO_SHOW_INFO("Суммы не сходяться");
        return FALSE;
    }
    return TRUE;
}
static void entry_activate(GtkWidget *entry, DoPvDialog *dialog)
{
	DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (dialog);
    gint i;
    gboolean valid = TRUE;
    for (i = 0; i < N_ENTRIES; i++)
        if (DO_IS_VALIDATE(priv->entry[i]) && !do_validate_valid(DO_VALIDATE(priv->entry[i]))) {
            valid = FALSE;
            gtk_widget_grab_focus(priv->entry[i]);
            break;
        }
    if (valid) {
        if (do_pv_dialog_update_footer(dialog))
            gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
        else
            do_pv_dialog_tree_grab_focus(dialog);
    }
}

static gboolean fill_entry(DoPvDialog *dialog)
{
	DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (dialog);
	GList *list, *l;
	GtkTreeIter iter;
    if (do_context_get_kind(priv->context) != DO_CONTEXT_KIND_SKLAD)
        return FALSE;
    do_entry_date_set_today(DO_ENTRY_DATE(priv->entry[DATE_ENTRY]));
    list = DOMINO_COMMON_PROPERTIES(PATH_BANKNOTES);
    for (l = list; l; l = l->next) {
        gchar *value;
        DOMINO_COMMON_GET(PATH_BANKNOTES, l->data, &value, NULL);
        if ( !value ) continue;
        gtk_tree_store_append (priv->stores, &iter, NULL);
        gtk_tree_store_set (priv->stores, &iter,
                            0, PATH_BANKNOTES,
                            2, value,
                            4, atof(value),
                            -1);

    }
    g_list_foreach(list, (GFunc)g_free, NULL);
    g_list_free(list);
    list = DOMINO_COMMON_PROPERTIES(PATH_COINS);
    for (l = list; l; l = l->next) {
        gchar *value;
        DOMINO_COMMON_GET(PATH_COINS, l->data, &value, NULL);
        if ( !value ) continue;
        gtk_tree_store_append (priv->stores, &iter, NULL);
        gtk_tree_store_set (priv->stores, &iter,
                            0, PATH_COINS,
                            2, value,
                            4, atof(value),
                            -1);

    }
    g_list_foreach(list, (GFunc)g_free, NULL);
    g_list_free(list);

    return TRUE;
}

#define LOAD_BUFFER_SIZE 65536

static gchar *month_name[12] =
{"января","февраля","марта","апреля","мая","июня","июля","августа","сентября","октября","ноября","декабря"
};

static gchar *do_pv_dialog_html_list(DoPvDialog *dialog, int list_number)
{
	DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (dialog);
    gchar  *retval = NULL;

	if (!do_pv_dialog_valid(dialog))
        return NULL;

    GFileInputStream *stream;
    GString *string = g_string_new(NULL);
    gint year,month,day;
    GDate  date = {0,};
    gchar  *number;
    gdouble sum;
    gint    total_value = 0;
    gdouble value_value = 0;
    gboolean eof;

    gchar  *value;
    gchar *ch;
    GtkTreeIter iter;
    GtkTreePath *path;
    do_alias_t *alias;
    gchar *pv_firm;
    gchar *pv_INN;
    gchar *pv_KPP;
    gchar *pv_OKATO;
    //gchar *pv_bookkeeper;
    //gchar *pv_cashier;
    gchar *pv_debit_account;
    gchar *pv_credit_account;
    gchar *pv_account;
    gchar *pv_bank_in;
    gchar *pv_bank_in_osb;
    gchar *pv_bank_in_bik;
    gchar *pv_bank_out;
    gchar *pv_bank_out_osb;
    gchar *pv_bank_out_bik;
    gchar *pv_symbol;
    gchar *code_sklad;

    otdel_key0_t otdel_key0;
    otdel_rec_t otdel;
    partner_key0_t partner_key0;
    partner_rec_t  partner;
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;


    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_OTDEL, DO_DB_PARTNER, DO_DB_SKLAD, DO_DB_END)) {
        DOMINO_SHOW_ERROR("Ошибка открытия БД");
        return NULL;
    }

    do_text_set(alias, otdel_key0.code, domino_division());

    switch (do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL)) {
        case DO_ERROR:
        case DO_KEY_NOT_FOUND:
            DOMINO_SHOW_ERROR("Ошибка чтения справника отделов");
            do_alias_free(alias);
            return NULL;
        case DO_OK:
            value = do_otdel_param(alias, &otdel, do_param(DO_PARAM_OTDEL_PARAMETR_FIRM));
            ch = strchr(value, '.');
            if (!ch) {
                do_text_set(alias, partner_key0.g_code, "00");
                partner_key0.code = 1;
            }
            else {
                *ch = '\0';
                ch++;
                do_text_set(alias, partner_key0.g_code, value);
                partner_key0.code = atoi(ch);
            }
            g_free(value);
            switch (do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL)) {
                case DO_ERROR:
                case DO_KEY_NOT_FOUND:
                    DOMINO_SHOW_ERROR("Ошибка чтения партнеров");
                    do_alias_free(alias);
                    return NULL;
                case DO_OK:
                    break;
            }
            break;
    }
    do_text_set(alias, sklad_key0.code, domino_unit());
    switch (do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL)) {
        case DO_ERROR:
        case DO_KEY_NOT_FOUND:
            DOMINO_SHOW_ERROR("Ошибка чтения справника подразделений");
            do_alias_free(alias);
            return NULL;
        case DO_OK:
            break;
    }

    pv_firm = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));
    pv_INN = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_INN));
    pv_KPP = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_KPP));
    //pv_bookkeeper = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_BOOKKEEPER));
    //pv_cashier = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_CASHIER));
    pv_symbol = do_partner_param(alias, &partner, "SYMBOL");

    pv_debit_account = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PV_DEBIT_ACCOUNT));
    pv_credit_account = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PV_CREDIT_ACCOUNT));
    pv_account = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PV_ACCOUNT));
    pv_bank_in = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PV_BANK_IN));
    pv_bank_in_osb = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PV_BANK_IN_OSB));
    pv_bank_in_bik = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PV_BANK_IN_BIK));
    pv_bank_out = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PV_BANK_OUT));
    pv_bank_out_osb = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PV_BANK_OUT_OSB));
    pv_bank_out_bik = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PV_BANK_OUT_BIK));
    pv_OKATO = do_sklad_param(alias, &sklad, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_OKATO));
    code_sklad = do_sklad_param(alias, &sklad, "SB_CODE");

    do_alias_free(alias);

    g_object_get(priv->entry[DATE_ENTRY], "year", &year, "month", &month, "day", &day, NULL);
    g_date_set_dmy(&date, day, month, year);
    g_object_get(priv->entry[SUM_ENTRY], "value", &sum, NULL);
    g_object_get(priv->entry[BAG_ENTRY], "text", &number, NULL);
    path = gtk_tree_path_new_first();

    value = g_strdup_printf("%s%d", PV_TEMPLATE, list_number);
    stream = domino_get_template_stream(value);
    g_free(value);

    if (stream) {
        gssize n_read;
        int key;
        gchar buffer[LOAD_BUFFER_SIZE];
        gchar *subst;
        eof = FALSE;

        while ((n_read = g_input_stream_read (G_INPUT_STREAM(stream),
                          buffer,
                          sizeof (buffer) - 1,
                          NULL,
                          NULL)) > 0) {
            buffer[n_read] = '\0';
            gchar *head = buffer, *ch = buffer;
            while (*ch != '\0') {
                head = ch;
                while ((*ch != '\0') && (*ch != '#' || *(ch + 1) != '#')) ch++;
                if (*ch == '\0') break;
                g_string_append_len(string, head, ch - head);
                ch +=2;
                head = ch;
                while ( *ch >= '0' && *ch <= '9' ) ch++;
                key = atoi(head);
                subst = NULL;

                switch (key) {
                    case 1:
                        subst = g_strdup(number);
                        break;
                    case 2:
                        subst = g_strdup_printf("%2.2d", day);
                        break;
                    case 3:
                        subst = g_strdup(month_name[month-1]);
                        break;
                    case 4:
                        subst = g_strdup_printf("%d", year);
                        break;
                    case 5:
//                        DOMINO_COMMO_GET(PATH_PV, PATH_FIRM, &value, NULL);
                        subst = g_strdup(pv_firm);
                        break;
                    case 6:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_DEBIT_ACCOUNT, &value, NULL);
                        subst = g_strdup(pv_debit_account);
                        break;
                    case 119:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_DEBIT_ACCOUNT, &value, NULL);
                        subst = g_strdup(pv_symbol);
                        break;
                    case 7:
                        subst = do_money_buh(sum);
                        break;
                    case 8:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_RECIEVER, &value, NULL);
                        subst = g_strdup(pv_firm);
                        break;
                    case 9:
                        //OMINO_COMMO_GET(PATH_PV, PATH_CREDIT_ACCOUNT, &value, NULL);
                        subst = g_strdup(pv_credit_account);
                        break;
                    case 10:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_INN, &value, NULL);
                        subst = g_strdup(pv_INN);
                        break;
                    case 11:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_KPP, &value, NULL);
                        subst = g_strdup(pv_KPP);
                        break;
                    case 12:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_ACCOUNT, &value, NULL);
                        subst = g_strdup(pv_account);
                        break;
                    case 13:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_OKATO, &value, NULL);
                        subst = g_strdup(pv_OKATO);
                        break;
                    case 14:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_BANK_DONOR, &value, NULL);
                        subst = g_strdup(pv_bank_in);
                        break;
                    case 15:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_OCB_DONOR, &value, NULL);
                        subst = g_strdup(pv_bank_in_osb);
                        break;
                    case 16:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_BIK_DONOR, &value, NULL);
                        subst = g_strdup(pv_bank_in_bik);
                        break;
                    case 18:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_BANK_RECIEVER, &value, NULL);
                        subst = g_strdup(pv_bank_out);
                        break;
                    case 19:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_OCB_RECIEVER, &value, NULL);
                        subst = g_strdup(pv_bank_out_osb);
                        break;
                    case 20:
                        //DOMINO_COMMO_GET(PATH_PV, PATH_BIK_RECIEVER, &value, NULL);
                        subst = g_strdup(pv_bank_out_bik);
                        break;
                    case 21:
                        value = do_money_string(sum, FALSE, FALSE);
                        subst = do_first_alpha_upper(value);
                        g_free(value);
                        break;
                    case 22:
                        value = g_strdup_printf("%18.2f", sum);
                        subst = g_strdup(value + strlen(value) - 2);
                        g_free(value);
                        break;
                    case 24:
                        if (eof) {
                            subst = g_strdup("-");
                            break;
                        }
                        do {
                            GValue value = {0,};
                            GValue total = {0,};
                            if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->stores), &iter, path)) {
                                eof = TRUE;
                                subst = g_strdup("-");
                                break;
                            }
                            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores), &iter, 1, &total);
                            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores), &iter, 4, &value);
                            total_value = g_value_get_int(&total);
                            value_value = g_value_get_double(&value);
                            gtk_tree_path_next(path);
                        } while (!total_value);
                        if (!eof) {
                            if (value_value < 1)
                                subst = g_strdup_printf("%.2f", value_value);
                            else
                                subst = g_strdup_printf("%.0f", value_value);
                        }
                        break;
                    case 25:
                        if (eof || !total_value) {
                            subst = g_strdup("-");
                            break;
                        }
                        subst = g_strdup_printf("%d", total_value);
                        break;
                    case 26:
                        if (eof || !total_value) {
                            subst = g_strdup("-");
                            break;
                        }
                        if (value_value < 1)
                            subst = do_money_buh(value_value*total_value);
                        else
                            subst = g_strdup_printf("%.0f", value_value*total_value);

                        break;
                    case 27:
                      subst = g_strdup_printf("%s", code_sklad);
                      break;
                }
                if (subst)
                    g_string_append(string, subst);
            }
            if (head != ch)
                g_string_append(string, head);
        }
        g_object_unref(stream);
        retval = g_string_free(string, FALSE);
    }
    else {
        DOMINO_SHOW_INFO("Невозможно найти шаблон \"%s\"", PV_TEMPLATE);
        retval = NULL;
    }
    gtk_tree_path_free(path);
    return retval;
}

gchar *do_pv_dialog_html(DoPvDialog *dialog, int index, int duplex)
{
    int i;
    gchar *retval = NULL;
    gchar *list, *tmp;
    if (index != -1) {
        tmp = do_pv_dialog_html_list(dialog, index);
        retval = g_strdup_printf(HTML_HEADER"%s"HTML_FOOTER, tmp);
        g_free(tmp);
        return retval;
    }
    for (i = 0; i < 3; i++) {
        list = do_pv_dialog_html_list(dialog, i + 1);
        if (!list) {
            if (retval)
                g_free(retval);
            return NULL;
        }
        if (!i)
            retval = list;
        else {
            tmp = retval;
//!!            if (duplex)
  //              retval = g_strdup_printf("%s%s%s%s", retval, PAGE_BREAK, PAGE_BREAK, list);
    //        else
            retval = g_strdup_printf("%s%s%s", retval, PAGE_BREAK, list);
            g_free(tmp);
            g_free(list);
        }
    }
    tmp = g_strdup_printf(HTML_HEADER"%s"HTML_FOOTER, retval);
    //!!save_to_file("d:/1.html", tmp, strlen(tmp));
    g_free(retval);
    return tmp;
}
static void do_pv_dialog_row_activated(GtkTreeView *tree_view, GtkTreePath *path,
                                                        GtkTreeViewColumn *column,
                                                        DoPvDialog *dialog)
{
	DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (dialog);
    gdouble sum, sum1;
    sum = 0;
    gtk_tree_model_foreach(GTK_TREE_MODEL(priv->stores), (GtkTreeModelForeachFunc)do_pv_dialog_calc_sum, &sum);
    g_object_get(priv->entry[SUM_ENTRY], "value", &sum1, NULL);
    if (sum && sum <= sum1 + 0.009 && sum >= sum1 - 0.009) {
        gint i;
        for (i = 0; i < N_ENTRIES; i++)
            if (DO_IS_VALIDATE(priv->entry[i]) && !do_validate_valid(DO_VALIDATE(priv->entry[i])))
                return;
        gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
    }
}

static void do_pv_dialog_editing_started(GtkCellRenderer *renderer,
                                                        GtkCellEditable *editable,
                                                        gchar           *path,
                                                        DoPvDialog *dialog)
{
	DoPvDialogPrivate *priv = DO_PV_DIALOG_GET_PRIVATE (dialog);
    gdouble sum, sum1;

    if (priv->insert_text) {
        if (GTK_IS_ENTRY (editable)) {
            g_object_set(editable, "text", priv->insert_text, NULL);
            g_object_set(gtk_widget_get_settings (GTK_WIDGET(editable)),
                    "gtk-entry-select-on-focus",
                    FALSE,
                    NULL);
            gtk_editable_set_position (GTK_EDITABLE (editable), -1);
        }
        g_free(priv->insert_text);
        priv->insert_text = NULL;
    }
    else {
        sum = 0;
        gtk_tree_model_foreach(GTK_TREE_MODEL(priv->stores), (GtkTreeModelForeachFunc)do_pv_dialog_calc_sum, &sum);
        g_object_get(priv->entry[SUM_ENTRY], "value", &sum1, NULL);
        if (sum && sum <= sum1 + 0.009 && sum >= sum1 - 0.009) {
            gint i;
            for (i = 0; i < N_ENTRIES; i++)
                if (DO_IS_VALIDATE(priv->entry[i]) && !do_validate_valid(DO_VALIDATE(priv->entry[i])))
                    return;
            gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
        }
    }
}
