

#include <string.h>
#include <gtk/gtk.h>

#include "do_pko_dialog.h"
#include "do_icons.h"
#include "do_entry_date_box.h"
#include "do_entry.h"
#include "do_entry_calc.h"
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
	PROP_MAIN_WINDOW,
	PROP_NO,
};

#define DO_PKO_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_PKO_DIALOG, DoPkoDialogPrivate))
#define DO_TYPE_PKO_DIALOG_FLAGS do_pko_dialog_flags_get_type()


#define OBJECT_ROOT_PATH "PkoDialog"
#define DEFAULT_WINDOW_POSITION "center-on-parent"
#define DEFAULT_WINDOW_SIZE ""
#define PKO_TEMPLATE "pko"

enum
{
	DATE_ENTRY,
	FIO_ENTRY,
	SUM_ENTRY,
	N_ENTRIES
};

struct _DoPkoDialogPrivate
{

	GtkWidget *widget[N_ENTRIES];
	GtkWidget *entry[N_ENTRIES];
	GtkWidget *main_window;

	GtkWidget *view;
    GtkTreeStore *stores;

	DoContext  *context;
	gchar      *alias_name;
};


G_DEFINE_TYPE (DoPkoDialog, do_pko_dialog, GTK_TYPE_DIALOG)

static GtkWidget *add_field(DoPkoDialog *dialog, gint id, GtkWidget *widget);

static void entry_activate(GtkWidget *entry, DoPkoDialog *dialog);
static gboolean do_pko_dialog_fill_entry(DoPkoDialog *dialog);

/*static gboolean pko_document_save(DoPkoDialog *dialog);
static gboolean pko_document_get(DoPkoDialog *dialog, do_alias_t *alias, document_rec_t *document);
static gboolean pko_document_update(DoPkoDialog *dialog, do_alias_t *alias, document_rec_t *document);

*/
//!!static void send_update_to_updated_view(DoPkoDialog *dialog, do_alias_t *alias, pko_rec_t *pko);

static void row_activated(GtkTreeView *tree_view, GtkTreePath *path,
                                                        GtkTreeViewColumn *column,
                                                        DoPkoDialog *dialog);

static void do_pko_dialog_init(DoPkoDialog *dialog)
{

}
static void do_pko_dialog_preview(GtkButton *button, DoPkoDialog *dialog)
{
	DoPkoDialogPrivate *priv = DO_PKO_DIALOG_GET_PRIVATE (dialog);

    GtkNotebook *nb;
    DoView *view;
    gchar *html;
    html = do_pko_dialog_html(DO_PKO_DIALOG(dialog));
    if ( !html )
        return;

    nb = GTK_NOTEBOOK (do_window_get_notebook (DO_WINDOW(priv->main_window)));
    g_return_if_fail (nb != NULL);

    view = DO_VIEW(do_html_view_new());
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    g_object_set(view, "html-string", html, "title", "ПКО", NULL);
    do_view_do_grab_focus(DO_VIEW(view));
    gtk_widget_destroy(GTK_WIDGET(dialog));
}
static void do_pko_dialog_ok(GtkButton *button, DoPkoDialog *dialog)
{
    gchar *html;
    html = do_pko_dialog_html(DO_PKO_DIALOG(dialog));
    if ( !html )
        return;
    if ( do_print_html(GTK_WIDGET(dialog), html) )
        gtk_widget_destroy(GTK_WIDGET(dialog));
}
static void do_pko_dialog_cancel(GtkButton *button, DoPkoDialog *dialog)
{
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static GObject *do_pko_dialog_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	GtkWindow *window;
	GtkDialog *dialog;
	DoPkoDialog *pko_dialog;
	DoPkoDialogPrivate *priv;
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;
	GtkWidget *vbox;
    GtkWidget *l;
    GtkWidget *t;
    GtkWidget *tb;
    GtkWidget *entry;
    GtkAccelGroup *accel_group;

	int row = 0;
	object = G_OBJECT_CLASS (do_pko_dialog_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_PKO_DIALOG_GET_PRIVATE(object);
	window = GTK_WINDOW (object);
	dialog = GTK_DIALOG (object);
	vbox   = gtk_dialog_get_content_area(dialog);
	pko_dialog = DO_PKO_DIALOG(object);

    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);


	gtk_box_set_spacing (GTK_BOX (vbox), 2); /* Message has 24, we want 12 = 2 + 2 * 5 */

	gtk_window_set_icon_name (window, DO_STOCK_DOMINO_APPS);
	gtk_window_set_title (window, "Печать ПКО");

    g_signal_connect (window, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), OBJECT_ROOT_PATH);

    //gtk_dialog_set_has_separator(dialog, FALSE);

    t = hig_workarea_create();
    //gtk_table_set_homogeneous(GTK_TABLE(t), TRUE);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, FALSE, 0);

    hig_workarea_add_section_title(t, &row, "Смены ");

    priv->stores = gtk_tree_store_new(7, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_DOUBLE);
    tb = priv->view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(priv->stores));
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tb), TRUE);

    g_signal_connect (priv->view, "row-activated",
                      G_CALLBACK (row_activated), object);

    hig_workarea_add_wide_control(t, &row, tb);

    r = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);
    gtk_tree_view_column_set_title (col, "Касса");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tb),col);

    r = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    gtk_tree_view_column_set_title (col, "Смена");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tb),col);

    r = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 2);
    gtk_tree_view_column_set_title (col, "Дата");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tb),col);

    r = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 3);
    gtk_tree_view_column_set_title (col, "Кассир");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tb),col);

    r = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 4);
    gtk_tree_view_column_set_title (col, "Наличные");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tb),col);

    hig_workarea_add_section_title(t, &row, "Данные ");

	l = add_field(pko_dialog, DATE_ENTRY, do_entry_date_box_new(DO_ENTRY_DATE_FLAGS_DEFAULT));
    hig_workarea_add_row(t, &row, "Дата:", l, NULL);

    entry = do_entry_new();
#if GTK_MAJOR_VERSION > 2 || GTK_MINOR_VERSION >= 24
	l = gtk_combo_box_text_new_with_entry ();
    gtk_container_remove (GTK_CONTAINER (l), gtk_bin_get_child(GTK_BIN (l)));
    g_object_set(entry, "regex-mask", "^([А-Я][а-я]+ [А-Я][а-я]+ [А-Я][а-я]+)$", NULL);
    gtk_container_add (GTK_CONTAINER (l), entry);
	add_field(pko_dialog, FIO_ENTRY, l);
#else
    add_field(pko_dialog, FIO_ENTRY, entry);
#endif	
    //priv->entry[FIO_ENTRY] = entry;

    hig_workarea_add_row(t, &row, "Принято от:", l, NULL);
    //gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);


	l = add_field(pko_dialog, SUM_ENTRY, do_entry_calc_new());
	g_object_set(l, "precision", 2, "min-value", 0.01, NULL);

    hig_workarea_add_row(t, &row, "Сумма:", l, NULL);
    //gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);
    //gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_PRIMARY, GTK_STOCK_FIND);

    hig_workarea_finish( t, &row );


	l = gtk_dialog_add_button (dialog, "Отмена", GTK_RESPONSE_CANCEL);
    g_signal_connect (l, "clicked",
                      G_CALLBACK (do_pko_dialog_cancel), object);
	l = gtk_dialog_add_button (dialog, "Просмотр", GTK_RESPONSE_YES);
    g_signal_connect (l, "clicked",
                      G_CALLBACK (do_pko_dialog_preview), object);
	l = gtk_dialog_add_button (dialog, "Ok", GTK_RESPONSE_ACCEPT);
    g_signal_connect (l, "clicked",
                      G_CALLBACK (do_pko_dialog_ok), object);



	//gtk_dialog_set_default_response (dialog, GTK_RESPONSE_ACCEPT);

    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(window), OBJECT_ROOT_PATH,
              "window-position", DEFAULT_WINDOW_POSITION,
              "window-size", DEFAULT_WINDOW_SIZE,
               NULL);

	gtk_widget_show_all (vbox);

	return object;
}

static void do_pko_dialog_finalize (GObject *object)
{
	DoPkoDialogPrivate *priv = DO_PKO_DIALOG_GET_PRIVATE (object);
	if (priv->context)
        g_object_unref(priv->context);
	if (priv->alias_name)
		g_free(priv->alias_name);
	G_OBJECT_CLASS (do_pko_dialog_parent_class)->finalize (object);
}

static void do_pko_dialog_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoPkoDialogPrivate *priv = DO_PKO_DIALOG_GET_PRIVATE (object);

	switch (prop_id)
	{
        case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string(value);
            break;
        case PROP_CONTEXT:
            if (!priv->context) {
                priv->context = do_context_copy(g_value_get_object(value));
                //do_start_long_operation(GTK_WIDGET(object));
                //g_idle_add_full(G_PRIORITY_HIGH_IDLE, (GSourceFunc)do_pko_dialog_fill_entry, object, NULL);
                do_pko_dialog_fill_entry(DO_PKO_DIALOG(object));
            }
            break;
        case PROP_MAIN_WINDOW:
            priv->main_window = g_value_get_object(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_pko_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoPkoDialogPrivate *priv = DO_PKO_DIALOG_GET_PRIVATE (object);
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
static void do_pko_dialog_class_init (DoPkoDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_pko_dialog_constructor;
	object_class->finalize = do_pko_dialog_finalize;
	object_class->get_property = do_pko_dialog_get_property;
	object_class->set_property = do_pko_dialog_set_property;

	g_type_class_add_private (object_class, sizeof (DoPkoDialogPrivate));

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
    g_object_class_install_property (object_class,
				   PROP_MAIN_WINDOW,
				   g_param_spec_object ("main-window",
 						      "Главное окно",
 						      "Главное окно",
 						      GTK_TYPE_WIDGET,
 						      G_PARAM_WRITABLE));
}
GtkWidget *do_pko_dialog_new (GtkWidget *parent, const gchar *alias_name, DoContext *context)
{
    GtkWidget *object;
    object = g_object_new (DO_TYPE_PKO_DIALOG,
			     "main-window", parent,
			     "alias-name", alias_name,
			     "context", context,
			     (gpointer) NULL);
    return object;
}

static GtkWidget *add_field(DoPkoDialog *dialog, gint id, GtkWidget *widget)
{
	DoPkoDialogPrivate *priv = DO_PKO_DIALOG_GET_PRIVATE (dialog);
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
static gboolean do_pko_dialog_valid(DoPkoDialog *dialog)
{
	DoPkoDialogPrivate *priv = DO_PKO_DIALOG_GET_PRIVATE (dialog);
    gint i;
    for (i = 0; i < N_ENTRIES; i++)
        if (DO_IS_VALIDATE(priv->entry[i]) && !do_validate_valid(DO_VALIDATE(priv->entry[i])))
            return FALSE;
    return TRUE;
}
static void entry_activate(GtkWidget *entry, DoPkoDialog *dialog)
{
	DoPkoDialogPrivate *priv = DO_PKO_DIALOG_GET_PRIVATE (dialog);
    gint i;
    gboolean valid = TRUE;
    for (i = 0; i < N_ENTRIES; i++)
        if (DO_IS_VALIDATE(priv->entry[i]) && !do_validate_valid(DO_VALIDATE(priv->entry[i]))) {
            valid = FALSE;
            gtk_widget_grab_focus(priv->entry[i]);
            break;
        }
    if ( valid )
        do_pko_dialog_ok(NULL, dialog);
        //gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
}

static gboolean do_pko_dialog_fill_entry(DoPkoDialog *dialog)
{
	DoPkoDialogPrivate *priv = DO_PKO_DIALOG_GET_PRIVATE (dialog);
    if ( do_context_get_kind(priv->context) != DO_CONTEXT_KIND_SKLAD ) {
        //do_end_long_operation(GTK_WIDGET(dialog));
        return FALSE;
    }
    do_alias_t *alias = domino_alias_new(priv->alias_name);
    if (!alias) {
        //do_end_long_operation(GTK_WIDGET(dialog));
        return FALSE;
    }
    if (!do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_DOCUMENT, DO_DB_PARTNER, DO_DB_END)) {
        do_alias_free(alias);
        //do_end_long_operation(GTK_WIDGET(dialog));
        return FALSE;
    }

    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    document_key0_t document_key0, document_key;
    document_rec_t document;
    partner_key1_t partner_key1, partner_key;
    partner_rec_t  partner;
    int status;
    gchar *sklad;
    gdouble sum;
    gint cash;
    gint shift = 0;
    gchar *operation;
    gint cashier_code = 0;
    gchar *cashier = NULL;
    gchar date_s[100];
    struct tm tm;

    g_object_get(priv->context, "sklad-code", &sklad, NULL);

    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_CRNTSALE));
    do_text_set(alias, document_key0.sklad, sklad);
    do_text_set(alias, document_key0.document, "");

    memcpy(&document_key, &document_key0, sizeof(document_key0.dtype) + sizeof(document_key0.sklad));

    status = do_document_get0(alias, &document, &document_key0, DO_GET_GE);

    while (status == DO_OK) {
        if (memcmp(&document_key, &document_key0, sizeof(document_key0.dtype) + sizeof(document_key0.sklad)))
            break;
        gchar *sopr, *ch;
        memcpy(&document_order_key0, &document_key0, sizeof(document_key0));
        document_order_key0.line = 0;
        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
        sum = 0;
        cash = document.data.p_code;
        sopr = do_text(alias, document.data.p_doc);
        ch = strchr (sopr, '/');
        if (ch) {
            ch++;
            shift = atoi(ch);
        }
        g_free(sopr);

        while (status == DO_OK) {
            if (memcmp(&document_order_key0, &document_key0, sizeof(document_key0)))
                break;

            operation = do_document_order_param(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_OPERATION));

            if (!strcmp(operation, "01") || !strcmp(operation, "02")) {
                sum += do_document_order_quant(alias, &document_order, DO_CONST_QUANT_CRNTSALE) *
                       do_document_order_price(alias, &document_order, DO_CONST_PRICE_FACT);

                do_date(document_order.data.date, &tm);
                do_time(document_order.data.time, &tm);
                cashier_code = do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_CASHIER));
            }
            g_free(operation);

            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
        }
        if (status == DO_ERROR)
            break;

        sprintf(date_s, "%2.2d/%2.2d/%4.4d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

        GtkTreeIter iter;
        gchar *sum_m = do_money_format(sum, TRUE);
        gtk_tree_store_append (priv->stores, &iter, NULL);

        cashier = do_get_partner_name(alias, do_param(DO_PARAM_PARTNER_REGION_CASHIER), cashier_code);

        gtk_tree_store_set (priv->stores, &iter,
                            0, cash,
                            1, shift,
                            2, date_s,
                            3, cashier,
                            4, sum_m,
                            5, cashier_code,
                            6, sum,
                            -1);
        g_free(sum_m);
        g_free(cashier);

        status = do_document_get0(alias, &document, &document_key0, DO_GET_NEXT);
    }

    do_text_set(alias, partner_key1.g_code, do_param(DO_PARAM_PARTNER_REGION_CASHIER));
    do_text_set(alias, partner_key1.name, "");
    memcpy(&partner_key.g_code, &partner_key1.g_code, sizeof(partner_key1.g_code));

    status = do_partner_get1(alias, &partner, &partner_key1, DO_GET_GE);

    while (status == DO_OK) {
        if (memcmp(&partner_key.g_code, &partner_key1.g_code, sizeof(partner_key1.g_code)))
            break;
        gchar *name;

        name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_CASHIER_FIO));

        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT (priv->widget[FIO_ENTRY]), name);
        g_free(name);

        status = do_partner_get1(alias, &partner, &partner_key1, DO_GET_NEXT);
    }

    do_alias_free(alias);
    //do_end_long_operation(GTK_WIDGET(dialog));
    return FALSE;
}

static void row_activated(GtkTreeView *tree_view, GtkTreePath *path,
                                                        GtkTreeViewColumn *column,
                                                        DoPkoDialog *dialog)
{
	DoPkoDialogPrivate *priv = DO_PKO_DIALOG_GET_PRIVATE (dialog);
    GtkTreeIter iter;

    do_alias_t *alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return;
    if (!do_alias_open(alias, 0, DO_DB_PARTNER, DO_DB_END)) {
        do_alias_free(alias);
        return;
    }
    gboolean ret = TRUE;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->stores), &iter, path)) {

        GValue code = { 0, };
        GValue sum = { 0, };
        GValue date = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores), &iter, 2, &date);
        if (g_value_get_string(&date))
            g_object_set(priv->entry[DATE_ENTRY], "text", g_value_get_string(&date), NULL);
        else
            ret = FALSE;
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores), &iter, 5, &code);
        if (g_value_get_int(&code)) {
            partner_key0_t partner_key0;
            partner_rec_t partner;
            do_text_set(alias, partner_key0.g_code, do_param(DO_PARAM_PARTNER_REGION_CASHIER));
            partner_key0.code = g_value_get_int(&code);

            if (do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) == DO_OK) {
                gchar *name;
                name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_CASHIER_FIO));
                gtk_entry_set_text(GTK_ENTRY(priv->entry[FIO_ENTRY]), name);
                g_free(name);
            }
        }
        else
            ret = FALSE;
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores), &iter, 6, &sum);
        if (g_value_get_double(&sum))
            g_object_set(priv->widget[SUM_ENTRY], "value", g_value_get_double(&sum), NULL);
        else
            ret = FALSE;
    }
    else
        ret = FALSE;
    if (ret)
         gtk_widget_grab_focus(priv->widget[SUM_ENTRY]);

    do_alias_free(alias);
}
#define LOAD_BUFFER_SIZE 65536

gchar *do_pko_dialog_html(DoPkoDialog *dialog)
{
	DoPkoDialogPrivate *priv = DO_PKO_DIALOG_GET_PRIVATE (dialog);

	if (!do_pko_dialog_valid(dialog))
        return NULL;

    GFileInputStream *stream;
    GString *string;
    gint year,month,day;
    GDate  date = {0,};
    gchar  *fio;
    gdouble sum;
    gchar  *retval;
    gchar  *bookkeeper;
    gchar  *cashier;
    gchar  *firm;
    otdel_rec_t otdel;
    otdel_key0_t otdel_key0;
    partner_rec_t partner;
    partner_key0_t partner_key0;
    do_alias_t *alias;
    gchar *value, *ch;

    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_PARTNER, DO_DB_OTDEL, DO_DB_END)) {
        DOMINO_SHOW_ERROR("Невозможно открыть БД");
        if (alias)
            do_alias_free(alias);
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
                    bookkeeper = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_BOOKKEEPER));
                    cashier = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_CASHIER));
                    firm = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));
                    break;
                default:
                    return NULL;
            }
            break;
        default:
            return NULL;
    }

    string = g_string_new(NULL);

    g_object_get(priv->entry[DATE_ENTRY], "year", &year, "month", &month, "day", &day, NULL);
    g_date_set_dmy(&date, day, month, year);
    g_object_get(priv->entry[SUM_ENTRY], "value", &sum, NULL);
    g_object_get(priv->entry[FIO_ENTRY], "text", &fio, NULL);

    stream = domino_get_template_stream(PKO_TEMPLATE);

    if (stream) {
        gssize n_read;
        int key;
        gchar buffer[LOAD_BUFFER_SIZE];
        gchar buf[100];
        gchar *subst;

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
                        g_date_strftime(buf, sizeof(buf), "%d %B %Yг.", &date);
                        subst = g_strdup(buf);
                        break;
                    case 2:
                    case 8:
                        subst = g_strdup(fio);
                        break;
                    case 3:
                        g_date_strftime(buf, sizeof(buf), "%d.%m.%Y", &date);
                        subst = g_strdup(buf);
                        break;
                    case 4:
                        subst = do_money_format(sum, TRUE);
                        break;
                    case 5:
                    case 9:
                        value = do_money_string(sum, TRUE, TRUE);
                        subst = do_first_alpha_upper(value);
                        g_free(value);
                        break;
                    case 6:
                        subst = g_strdup(domino_unit());
                        break;
                    case 7:
                        subst = do_money_buh(sum);
                        break;
                    case 10:
                        g_date_strftime(buf, sizeof(buf), "%d %B %Yг.", &date);
                        subst = g_strdup(buf);
                        break;
                    case 11:
                    case 12:
                        subst = g_strdup(bookkeeper);
                        break;
                    case 13:
                    case 14:
                        subst = g_strdup(cashier);
                        break;
                    case 15:
                        subst = g_strdup(firm);
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
        DOMINO_SHOW_INFO("Невозможно найти шаблон \"%s\"", PKO_TEMPLATE);
        retval = NULL;
    }
    do_alias_free(alias);
    g_free(bookkeeper);
    g_free(cashier);
    g_free(firm);
    return retval;
}
