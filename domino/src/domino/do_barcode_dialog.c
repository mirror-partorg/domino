

#include <string.h>
#include <gtk/gtk.h>

#include "do_barcode_dialog.h"
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
#ifdef CUPS
#include <cups/cups.h>
#endif

enum
{
	PROP_0,
	PROP_BARCODE,
	PROP_ALIAS,
	PROP_MAIN_WINDOW,
	PROP_NO,
};

#define DO_BARCODE_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_BARCODE_DIALOG, DoBarcodeDialogPrivate))
#define DO_TYPE_BARCODE_DIALOG_FLAGS do_barcode_dialog_flags_get_type()


#define OBJECT_ROOT_PATH "BarcodeDialog"
#define DEFAULT_WINDOW_POSITION "center-on-parent"
#define DEFAULT_WINDOW_SIZE ""
#define BARCODE_TEMPLATE "barcode"

enum
{
	BARCODE_ENTRY,
	TOTAL_ENTRY,
	N_ENTRIES
};

struct _DoBarcodeDialogPrivate
{

	GtkWidget *widget[N_ENTRIES];
	GtkWidget *entry[N_ENTRIES];
	GtkWidget *main_window;

	GtkWidget *view;
    GtkTreeStore *stores;

	gchar      *barcode;
	gchar      *alias_name;
};


G_DEFINE_TYPE (DoBarcodeDialog, do_barcode_dialog, GTK_TYPE_DIALOG)

static GtkWidget *add_field(DoBarcodeDialog *dialog, gint id, GtkWidget *widget);

static void entry_activate(GtkWidget *entry, DoBarcodeDialog *dialog);
static gboolean do_barcode_dialog_fill_entry(DoBarcodeDialog *dialog);
static gboolean do_barcode_dialog_print(DoBarcodeDialog *dialog);


static void do_barcode_dialog_init(DoBarcodeDialog *dialog)
{

}
static void do_barcode_dialog_ok(GtkButton *button, DoBarcodeDialog *dialog)
{
    if ( do_barcode_dialog_print(dialog) )
        gtk_widget_destroy(GTK_WIDGET(dialog));
}
static void do_barcode_dialog_cancel(GtkButton *button, DoBarcodeDialog *dialog)
{
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static GObject *do_barcode_dialog_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	GtkWindow *window;
	GtkDialog *dialog;
	DoBarcodeDialog *barcode_dialog;
	//DoBarcodeDialogPrivate *priv;
	GtkWidget *vbox;
    GtkWidget *l;
    GtkWidget *t;
    GtkWidget *entry;
    GtkAccelGroup *accel_group;

	int row = 0;
	object = G_OBJECT_CLASS (do_barcode_dialog_parent_class)->constructor(type, n_construct_properties, construct_params);

    //priv = DO_BARCODE_DIALOG_GET_PRIVATE(object);
	window = GTK_WINDOW (object);
	dialog = GTK_DIALOG (object);
	vbox   = gtk_dialog_get_content_area(dialog);
	barcode_dialog = DO_BARCODE_DIALOG(object);

    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);


	gtk_box_set_spacing (GTK_BOX (gtk_dialog_get_content_area(dialog)), 2); /* Message has 24, we want 12 = 2 + 2 * 5 */

	gtk_window_set_icon_name (window, DO_STOCK_DOMINO_APPS);
	gtk_window_set_title (window, "Печать продажных кодов");

    g_signal_connect (window, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), OBJECT_ROOT_PATH);

    //gtk_dialog_set_has_separator(dialog, FALSE);

    t = hig_workarea_create();
    //gtk_table_set_homogeneous(GTK_TABLE(t), TRUE);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, FALSE, 0);

    entry = do_entry_new();
    g_object_set(entry, "regex-mask", "^([0-9]+)$", NULL);
	add_field(barcode_dialog, BARCODE_ENTRY, entry);
    hig_workarea_add_row(t, &row, "Продажный код:", entry, NULL);

    entry = do_entry_new();
    g_object_set(entry, "regex-mask", "^([0-9]+)$", NULL);
	add_field(barcode_dialog, TOTAL_ENTRY, entry);
    hig_workarea_add_row(t, &row, "Количество:", entry, NULL);

    hig_workarea_finish( t, &row );

	l = gtk_dialog_add_button (dialog, "Отмена", GTK_RESPONSE_CANCEL);
    g_signal_connect (l, "clicked",
                      G_CALLBACK (do_barcode_dialog_cancel), object);
	l = gtk_dialog_add_button (dialog, "Ok", GTK_RESPONSE_ACCEPT);
    g_signal_connect (l, "clicked",
                      G_CALLBACK (do_barcode_dialog_ok), object);

    do_barcode_dialog_fill_entry(barcode_dialog);

	//gtk_dialog_set_default_response (dialog, GTK_RESPONSE_ACCEPT);

    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(window), OBJECT_ROOT_PATH,
              "window-position", DEFAULT_WINDOW_POSITION,
              "window-size", DEFAULT_WINDOW_SIZE,
               NULL);

	gtk_widget_show_all (vbox);

	return object;
}

static void do_barcode_dialog_finalize (GObject *object)
{
	DoBarcodeDialogPrivate *priv = DO_BARCODE_DIALOG_GET_PRIVATE (object);
	if (priv->alias_name)
		g_free(priv->alias_name);
	if (priv->barcode)
		g_free(priv->barcode);
	G_OBJECT_CLASS (do_barcode_dialog_parent_class)->finalize (object);
}

static void do_barcode_dialog_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoBarcodeDialogPrivate *priv = DO_BARCODE_DIALOG_GET_PRIVATE (object);

	switch (prop_id)
	{
        case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string(value);
            break;
        case PROP_BARCODE:
            if ( priv->barcode )
                g_free(priv->barcode);
            if ( g_value_get_string(value) )
                priv->barcode = g_value_dup_string(value);
            else
                priv->barcode = NULL;
            break;
        case PROP_MAIN_WINDOW:
            priv->main_window = g_value_get_object(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_barcode_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoBarcodeDialogPrivate *priv = DO_BARCODE_DIALOG_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_ALIAS:
            g_value_set_string(value, priv->alias_name);
        case PROP_BARCODE:
            g_value_set_string(value, priv->barcode);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;

    }
}
static void do_barcode_dialog_class_init (DoBarcodeDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_barcode_dialog_constructor;
	object_class->finalize = do_barcode_dialog_finalize;
	object_class->get_property = do_barcode_dialog_get_property;
	object_class->set_property = do_barcode_dialog_set_property;

	g_type_class_add_private (object_class, sizeof (DoBarcodeDialogPrivate));

    g_object_class_install_property
        (object_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property
        (object_class,
         PROP_BARCODE,
         g_param_spec_string("barcode",
                      "Продкод",
                      "Продажный код",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (object_class,
				   PROP_MAIN_WINDOW,
				   g_param_spec_object ("main-window",
 						      "Главное окно",
 						      "Главное окно",
 						      GTK_TYPE_WIDGET,
 						      G_PARAM_WRITABLE));
}
GtkWidget *do_barcode_dialog_new (GtkWidget *parent, const gchar *alias_name, const gchar *barcode)
{
    GtkWidget *object;
    object = g_object_new (DO_TYPE_BARCODE_DIALOG,
			     "main-window", parent,
			     "alias-name", alias_name,
			     "barcode", barcode,
			     (gpointer) NULL);
    return object;
}

static GtkWidget *add_field(DoBarcodeDialog *dialog, gint id, GtkWidget *widget)
{
	DoBarcodeDialogPrivate *priv = DO_BARCODE_DIALOG_GET_PRIVATE (dialog);
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
static gboolean do_barcode_dialog_valid(DoBarcodeDialog *dialog)
{
	DoBarcodeDialogPrivate *priv = DO_BARCODE_DIALOG_GET_PRIVATE (dialog);
    gint i;
    for (i = 0; i < N_ENTRIES; i++)
        if (DO_IS_VALIDATE(priv->entry[i]) && !do_validate_valid(DO_VALIDATE(priv->entry[i])))
            return FALSE;
    return TRUE;
}
static void entry_activate(GtkWidget *entry, DoBarcodeDialog *dialog)
{
	DoBarcodeDialogPrivate *priv = DO_BARCODE_DIALOG_GET_PRIVATE (dialog);
    gint i;
    gboolean valid = TRUE;
    for (i = 0; i < N_ENTRIES; i++)
        if (DO_IS_VALIDATE(priv->entry[i]) && !do_validate_valid(DO_VALIDATE(priv->entry[i]))) {
            valid = FALSE;
            gtk_widget_grab_focus(priv->entry[i]);
            break;
        }
    if ( valid )
        do_barcode_dialog_ok(NULL, dialog);
        //gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
}

static gboolean do_barcode_dialog_fill_entry(DoBarcodeDialog *dialog)
{
	DoBarcodeDialogPrivate *priv = DO_BARCODE_DIALOG_GET_PRIVATE (dialog);
    if ( priv->barcode ) {
        g_object_set(priv->widget[BARCODE_ENTRY], "text", priv->barcode, NULL);
        if ( do_validate_valid(DO_VALIDATE(priv->widget[BARCODE_ENTRY])) )
            gtk_widget_grab_focus(priv->entry[TOTAL_ENTRY]);
    }
    return FALSE;
}


static gboolean do_barcode_dialog_print(DoBarcodeDialog *dialog)
{
	if (!do_barcode_dialog_valid(dialog))
        return FALSE;

	DoBarcodeDialogPrivate *priv = DO_BARCODE_DIALOG_GET_PRIVATE (dialog);
    do_alias_t *alias;
    barcode_key0_t barcode_key0;
    barcode_rec_t barcode;
#ifndef CUPS
    sklad_rec_t sklad;
    sklad_key0_t sklad_key0;
#endif
    product_rec_t product;
    product_key4_t product_key4;
    gboolean retval = TRUE;
    gchar *code;
    gint   total;
    gchar *printer;
    //int nprice = 4;

    g_object_get(priv->widget[TOTAL_ENTRY], "text", &code, NULL);
    total = atoi(code);
    g_object_get(priv->widget[BARCODE_ENTRY], "text", &code, NULL);

    alias = domino_alias_new(priv->alias_name);
    if ( !alias )
        return FALSE;
    if ( !do_alias_open(alias, TRUE) ) {
        do_alias_free(alias);
        return FALSE;
    }
    do_text_set(alias, barcode_key0.barcode, code);
    switch ( do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            DOMINO_SHOW_ERROR("Товар с продажным кодом \"%s\" не найден", code);
            retval = FALSE;
            break;
        default:
            retval = FALSE;
    }
    if ( retval ) {
        do_cpy(product_key4.code, barcode.data.code);
        switch ( do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) ) {
            case DO_OK:
                break;
            case DO_KEY_NOT_FOUND:
                code = do_text(alias, product_key4.code);
                DOMINO_SHOW_ERROR("Товар с кодом \"%s\" не найден", code);
                do_free(code);
                retval = FALSE;
                break;
            default:
                retval = FALSE;
        }
    }
    if ( retval ) {
#ifndef CUPS
        do_text_set(alias, sklad_key0.code, domino_unit());
        switch ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) ) {
            case DO_OK:
                printer = do_sklad_param(alias, &sklad, "15");
                break;
            case DO_KEY_NOT_FOUND:
                DOMINO_SHOW_ERROR("Подразделение с кодом \"%s\" не найдено", domino_unit());
                retval = FALSE;
                break;
            default:
                retval = FALSE;
        }
#else
        gchar *zebra;
        DOMINO_LOCAL_GET("Print", "zebra", &zebra, NULL);
        printer = g_strdup(( zebra ) ? zebra : "zebra");
#endif
    }
    if ( retval ) {
        gchar *value;
        gchar *text;
        GList  *names;
        GString *command;
        value = do_text(alias, product.data.name);
        names = util_string_to_colon(value, 18);
        if ( g_list_length(names) < 2 )
            names = g_list_append(names, g_strdup("") );
        g_free(value);
        command = g_string_new(
                        "Y96,N,8,1\r\n"
                        "N\r\n"
                        "D7\r\n"
                        "S4\r\n"
                        "ZB\r\n"
                        "Q160,24\r\n"
                        "q232\r\n");
        g_string_append_printf(command,
                        "A23,10,0,a,1,1,N,\"%s\"\r\n"
                        "A23,30,0,a,1,1,N,\"%s\"\r\n",
                        (gchar*)g_list_nth(names, 0)->data, (gchar*)g_list_nth(names, 1)->data);

        if ( product.data.base_parcel ) {
            g_string_append_printf(command,
                        "B15,60,0,E30,2,3,50,N,\"%s\"\r\n",
                        code);
            value = do_product_param(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_SERIA));
            if ( do_product_param_int(alias, &product, "ЖВ") == 1 )
	            g_string_append_printf(command,
                        "A25,110,0,b,1,1,N,\"ЖВНЛП %s\"\r\n",
                        value);
            else
	            g_string_append_printf(command,
                        "A25,110,0,b,1,1,N,\"%s\"\r\n",
                        value);
            do_free(value);
        }
        else {
            g_string_append_printf(command,
                        "B15,60,0,E30,2,3,70,N,\"%s\"\r\n",
                        code);
            value = do_text(alias, product.data.code);
            g_string_append_printf(command,
                        "A25,130,0,b,1,1,N,\"Код:%s\"\r\n",
                        value);
            do_free(value);
        }
        g_string_append_printf(command,
                        "P%d\r\n",
                        total);
        g_list_foreach(names, (GFunc)g_free, NULL);
        g_list_free(names);
        value = g_string_free(command, FALSE);
        text = g_convert(value, -1, DOMINO_CHARSET, "UTF-8", NULL, NULL, NULL);
        if ( !text ) {
            DOMINO_SHOW_INFO("Ошибка ковертации \"%s\"", value);
            retval = FALSE;
        }
        g_free(value);
        value = util_create_uniq_filename(g_get_tmp_dir(), "barcode", "txt");
        retval = util_save_to_file(value, text, -1) ;
        g_free(text);
        if ( retval ) {
            GFile *f;
            f = g_file_new_for_path(value);
#ifdef CUPS
            cups_option_t *options;
            options     = NULL;
            cupsAddOption("raw", "true", 0, &options);
            retval = cupsPrintFile(printer, value, "", 1, options);
            cupsFreeOptions(1, options);
            if ( !retval )
                DOMINO_SHOW_ERROR("Ошибка печати на \"%s\":%s", printer, cupsLastErrorString());
#else
            GFile *pf;
            GError *error = NULL;
            pf = g_file_new_for_path(printer);
            retval = g_file_copy(f, pf, 0, NULL, NULL, NULL, &error);
            if ( !retval ) {
                DOMINO_SHOW_ERROR("Ошибка печати на \"%s\":%s", printer, error->message);
                g_error_free(error);
            }
#endif
	    if ( retval ) {
		gchar *value;
		value = g_strdup_printf("ПЕЧАТЬ %s(%d)", code, total);
		do_protocol_add(alias, DO_PROTOCOL_OBJECT_OTHER, "ПРОДКОД", DO_PROTOCOL_ACTION_NONE, value);
		g_free(value);
	    }
            g_file_delete(f, NULL, NULL);
        }
        g_free(value);
    }
    g_free(printer);
    do_alias_free(alias);
    return retval;
}
