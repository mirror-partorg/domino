

#if GTK_MAJOR_VERSION == 2


#include "do_inventory_dialog.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_view_actions.h"
#include "do_icons.h"
#include "do_entry.h"
#include "domino.h"
#include <gdk/gdkevents.h>
#include <gdk/gdkkeysyms.h>

#include <string.h>

#define DO_INVENTORY_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_INVENTORY_DIALOG, DoInventoryDialogPrivate))

#define FONT_PRODUCT "18"
#define FONT_ADDITION "16"
#define FONT_ENTRY "27"
#define BUTTON_HEIGHT 40
#define EMPTY_PRODUCT ""
#define BARCODE_LEN 13
#define DEFAULT_QUANT 1
#define QUANT_LEN 4

#ifdef _WIN32
#include <windows.h>
#endif

// Menu interface

static GtkWidget *do_message_dialog_create(GtkWidget *parent, GtkMessageType type, GtkButtonsType buttons, const gchar *msg)
{
    GtkWidget *widget;

    GtkWidget *win = NULL;
    GtkWidget *win1 = NULL;
    if (parent)
        win1 = gtk_widget_get_toplevel(parent);

    if (win1)  {
        if (!gtk_widget_get_visible(win1)) {
            if (gtk_window_get_transient_for (GTK_WINDOW(win1)))
                win = GTK_WIDGET(gtk_window_get_transient_for (GTK_WINDOW(win1)));
        }else
            win = win1;
    }
    if (!GTK_IS_WINDOW(win))
        win = NULL;
    widget = gtk_message_dialog_new (GTK_WINDOW(win), GTK_DIALOG_MODAL, type, buttons, NULL);

	gtk_window_set_icon_name (GTK_WINDOW(widget), DO_STOCK_DOMINO_APPS);
	gtk_window_set_position (GTK_WINDOW(widget), GTK_WIN_POS_CENTER);

    if ( msg )
        gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (widget), msg);
    return widget;
}
typedef struct _MessageForDialog MessageForDialog;

struct _MessageForDialog {
    GtkWidget *parent;
    GtkMessageType type;
    GtkButtonsType buttons;
    gchar *message;
    gboolean ask_exit;
};
//static GList *message_dialog_list = NULL;

static gint do_message_dialog_show(GtkWidget *parent, GtkMessageType type, GtkButtonsType buttons, const gchar *markup_format, ...)
{
    GtkWidget *widget;
    va_list args;
    gchar *msg = NULL;
    if (markup_format)
    {
        va_start (args, markup_format);
        msg = g_markup_vprintf_escaped (markup_format, args);
        va_end (args);
    }
    //message_dialog_list = g_list_append
    widget = do_message_dialog_create(parent, type, buttons, msg);
    g_free(msg);

    gint retval = gtk_dialog_run(GTK_DIALOG(widget));
    gtk_widget_destroy (widget);
    return retval;
}

static void do_inventory_dialog_view_init(DoViewIface *iface);


static void        do_inventory_dialog_close(DoView *embed);
static gboolean    do_inventory_dialog_close_request(DoView *embed);
static const char *do_inventory_dialog_get_title(DoView *embed);
static GdkPixbuf  *do_inventory_dialog_get_icon(DoView *embed);
static gboolean    do_inventory_dialog_do_grab_focus(DoView *menu);
static gboolean	   do_inventory_dialog_get_load_status(DoView *embed);

static void     do_inventory_dialog_button_clicked(GtkButton *button, DoInventoryDialog *menu);
static void     do_inventory_dialog_entry_activated(GtkEntry *entry, DoInventoryDialog *dialog);
static gboolean do_inventory_dialog_text_validate(const gchar *text, DoInventoryDialog *dialog);
static gboolean do_inventory_dialog_text_quant_validate(const gchar *text, DoInventoryDialog *dialog);

static gboolean do_inventory_dialog_follow_init(DoInventoryDialog *dialog);

static gboolean do_inventory_dialog_order_save(DoInventoryDialog *dialog);
static gboolean do_inventory_dialog_order_back(DoInventoryDialog *dialog, gboolean question);
static gboolean do_inventory_dialog_order_read_for_product(DoInventoryDialog *dialog);
static gboolean do_inventory_dialog_order_read_for_path(DoInventoryDialog *dialog);

static void     do_inventory_dialog_update_status(DoInventoryDialog *dialog);

static void     do_inventory_dialog_update_quant(DoInventoryDialog *dialog, gint quant, gboolean default_);
static gboolean do_inventory_dialog_key_pressed(GtkWidget *widget, GdkEvent *event, DoInventoryDialog *dialog);
static gboolean do_inventory_dialog_entry_key_pressed(GtkWidget *widget, GdkEvent *event, DoInventoryDialog *dialog);
static void     do_inventory_dialog_set_document(DoInventoryDialog *dialog, document_key0_t *document);
static void     do_inventory_dialog_update_document_comment(DoInventoryDialog *dialog, gboolean insert_report);

static gboolean do_inventory_dialog_get_quant_all(DoInventoryDialog *dialog);


static gboolean barcode_validate(const gchar *text);
static void show_error(DoInventoryDialog *dialog, const gchar *markup);
#define SHOW_ERROR(...) {gchar *val; val = g_strdup_printf(__VA_ARGS__); show_error(dialog, val); g_free(val); }
#ifdef _WIN32
#define SOUND_RECEIVE() PlaySound(TEXT("receive.wav"), NULL, SND_ALIAS)
#define SOUND_ERROR() PlaySound(TEXT("alert.wav"), NULL, SND_ALIAS)
#define SOUND_QUESTION() PlaySound(TEXT("question.wav"), NULL, SND_ALIAS)
#else
#define SOUND_RECEIVE()
#define SOUND_ERROR()
#define SOUND_QUESTION()
#endif

enum
{
	PROP_0,
	PROP_DOCUMENT,
};

enum
{
    BUTTON_PREV =0,
    BUTTON_NEXT,
    BUTTON_BACK,
    BUTTON_QUIT,
    BUTTON_NO,
};

static const gchar *button_name[BUTTON_NO] =
{"Назад",
 "Вперед",
 "Сторно",
 "Выход"};

/*
static const gchar *button_stock[BUTTON_NO] =
{ GTK_STOCK_MEDIA_PREVIOUS,
  NULL,
  NULL,
};
*/

struct _DoInventoryDialogPrivate
{
    GtkWidget       *menu;
    GdkPixbuf       *icon;
    GtkWidget       *buttons[BUTTON_NO];
    GtkWidget       *entry;
    GtkWidget       *label;
    GtkWidget       *seria[2];
    GtkWidget       *price[2];
    GtkWidget       *quant[2];
    GtkWidget       *quant_all[2];
    do_alias_t      *alias;

    gboolean              insert_mode;

    gboolean              product_select;
    product_rec_t         product;

    document_order_rec_t  document_order;
    document_order_key0_t document_order_key0;

    document_key0_t       document_key0;
    document_rec_t        document;
    gint                  product_quant;
    double                product_quant_all;
    GList                *order;
    GList                *path_order;
};

G_DEFINE_TYPE_WITH_CODE (DoInventoryDialog, do_inventory_dialog, GTK_TYPE_VBOX,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_inventory_dialog_view_init)
                            );

static void do_inventory_dialog_view_init(DoViewIface *iface)
{
    iface->do_close        = do_inventory_dialog_close;
    iface->close_request   = do_inventory_dialog_close_request;
    iface->get_title       = do_inventory_dialog_get_title;
    iface->get_icon        = do_inventory_dialog_get_icon;
    iface->do_grab_focus   = do_inventory_dialog_do_grab_focus;
    iface->get_load_status = do_inventory_dialog_get_load_status;
}


static void do_inventory_dialog_init(DoInventoryDialog *temp)
{
	DoInventoryDialogPrivate *priv = DO_INVENTORY_DIALOG_GET_PRIVATE (temp);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_PROPERTIES, DO_VIEW_ICON_SIZE, 0, NULL);
}

#define LOAD_BUFFER_SIZE 65536
#define SPAN 0
static GObject *do_inventory_dialog_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject                  *object;
	DoInventoryDialogPrivate *priv;
	GtkVBox                  *vbox;
	GtkWidget                *hbox;
	GtkTable                 *tb;
	GtkWidget                *widget;
	PangoFontDescription     *font;
	int i;

	object = G_OBJECT_CLASS (do_inventory_dialog_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (object);
	vbox = GTK_VBOX(object);



    priv->alias = domino_alias_new(NULL);
    if ( priv->alias )
        if ( !do_alias_open(priv->alias, TRUE) ) {
            do_alias_free(priv->alias);
            priv->alias = NULL;
        }

    g_signal_connect(object, "key-press-event", G_CALLBACK(do_inventory_dialog_key_pressed), object);

    hbox=gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(hbox), FALSE, TRUE, SPAN);

	font = pango_font_description_from_string(FONT_PRODUCT);

	priv->label = widget = gtk_label_new(EMPTY_PRODUCT);
	gtk_widget_modify_font(GTK_WIDGET(widget), font);

    g_object_set(widget, "xalign", 0.01, NULL);
    g_object_set(widget, "yalign", 0.0, NULL);
    //gtk_label_set_ellipsize(GTK_LABEL(widget), PANGO_ELLIPSIZE_END);
    gtk_label_set_line_wrap(GTK_LABEL(widget), TRUE);
    gtk_label_set_line_wrap_mode(GTK_LABEL(widget), PANGO_WRAP_WORD_CHAR);



	font = pango_font_description_from_string(FONT_ADDITION);

	priv->seria[0] = widget = gtk_label_new("Серия:");
    g_object_set(widget, "xalign", 0.01, NULL);
	priv->seria[1] = widget = gtk_label_new("");
    g_object_set(widget, "xalign", 0.0, NULL);
	//gtk_widget_modify_font(GTK_WIDGET(widget), font);
	gtk_widget_modify_font(GTK_WIDGET(widget), font);
	gtk_label_set_ellipsize(GTK_LABEL(widget), PANGO_ELLIPSIZE_START);

	priv->quant_all[0] = widget = gtk_label_new("Общее:");
    g_object_set(widget, "xalign", 0.01, NULL);
	priv->quant_all[1] = widget = gtk_label_new("");
    g_object_set(widget, "xalign", 0.0, NULL);
	//gtk_widget_modify_font(GTK_WIDGET(widget), font);
	gtk_widget_modify_font(GTK_WIDGET(widget), font);
	gtk_label_set_ellipsize(GTK_LABEL(widget), PANGO_ELLIPSIZE_START);

	priv->price[0] = widget = gtk_label_new("Цена:");
    g_object_set(widget, "xalign", 0.01, NULL);
	priv->price[1] = widget = gtk_label_new("");
    g_object_set(widget, "xalign", 0.0, NULL);
	//gtk_widget_modify_font(GTK_WIDGET(widget), font);
	gtk_widget_modify_font(GTK_WIDGET(widget), font);

	priv->quant[0] = widget = gtk_label_new("Введено: ");
    g_object_set(widget, "xalign", 0.01, NULL);
	priv->quant[1] = widget = gtk_label_new("");
    g_object_set(widget, "xalign", 0.0, NULL);
	//gtk_widget_modify_font(GTK_WIDGET(widget), font);
	gtk_widget_modify_font(GTK_WIDGET(widget), font);
	gtk_label_set_ellipsize(GTK_LABEL(widget), PANGO_ELLIPSIZE_START);


	font = pango_font_description_from_string(FONT_ENTRY);

	priv->entry = widget = do_entry_new();
	g_signal_connect(widget, "activate", G_CALLBACK(do_inventory_dialog_entry_activated), object);
	gtk_widget_modify_font(GTK_WIDGET(widget), font);
    do_entry_set_validate_func(DO_ENTRY(widget), (DoValidateFunc)do_inventory_dialog_text_validate, object);
    gtk_entry_set_icon_from_stock(GTK_ENTRY(widget), GTK_ENTRY_ICON_SECONDARY, NULL);

    g_signal_connect(priv->entry, "key-press-event", G_CALLBACK(do_inventory_dialog_entry_key_pressed), object);

    tb = GTK_TABLE(gtk_table_new(2,4, FALSE));

    gtk_box_pack_start(GTK_BOX(vbox), priv->label, TRUE, TRUE, SPAN);

    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(tb), FALSE, TRUE, SPAN);


    gtk_table_attach(tb, priv->price[0], 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(tb, priv->price[1], 1, 2, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
    gtk_table_attach(tb, priv->seria[0], 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(tb, priv->seria[1], 1, 2, 1, 2, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
    gtk_table_attach(tb, priv->quant_all[0], 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(tb, priv->quant_all[1], 1, 2, 2, 3, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
    gtk_table_attach(tb, priv->quant[0], 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(tb, priv->quant[1], 1, 2, 3, 4, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

    gtk_box_pack_start(GTK_BOX(vbox), priv->entry, FALSE, TRUE, SPAN);


    for ( i = 0; i < BUTTON_NO; i++ ) {
	    GtkWidget *button;
        button = gtk_button_new_with_label(button_name[i]);
	    /*if ( button_stock[i] ) {
            button = gtk_event_box_new();
	        GtkWidget *image;
	        image = gtk_image_new_from_stock(button_stock[i], GTK_ICON_SIZE_BUTTON);
            gtk_container_add(GTK_CONTAINER(button), image);
            //gtk_button_set_image(GTK_BUTTON(button), image);
	    }*/
        //gtk_widget_set_size_request(button, -1, BUTTON_HEIGHT);

	    priv->buttons[i] = button;
	    g_object_set_data(G_OBJECT(button), "number", GINT_TO_POINTER(i));
	    g_signal_connect(button, "clicked", G_CALLBACK(do_inventory_dialog_button_clicked), object);

	    gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 0);
	    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_HALF);
	    //gtk_button_set_use_underline(GTK_BUTTON(button), TRUE);

	    GtkWidget *l;
	    gchar *markup;
	    l = gtk_bin_get_child(GTK_BIN(button));
	    //markup=g_strdup_printf("<span font_size=\"%d\">%s</span>", FONT_SIZE, button_name[i]);
	    markup=g_strdup_printf("<b>%s</b>", button_name[i]);
	    if ( GTK_IS_LABEL(l) )
            gtk_label_set_markup(GTK_LABEL(l), markup);
        g_free(markup);
    }
    do_inventory_dialog_update_status(DO_INVENTORY_DIALOG(object));
    gtk_widget_show_all(GTK_WIDGET(object));
	return object;
}

static void do_inventory_dialog_finalize (GObject *object)
{
	DoInventoryDialogPrivate *priv = DO_INVENTORY_DIALOG_GET_PRIVATE (object);
    if ( priv->alias )
        do_alias_free(priv->alias);
    g_list_free(priv->order);
	G_OBJECT_CLASS (do_inventory_dialog_parent_class)->finalize (object);
}

static void do_inventory_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
//	DoInventoryDialogPrivate *priv = DO_INVENTORY_DIALOG_GET_PRIVATE (object);
	switch (prop_id)
	{
        default :
            g_assert_not_reached ();
	}
}

static void do_inventory_dialog_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	//DoInventoryDialogPrivate *priv = DO_INVENTORY_DIALOG_GET_PRIVATE (object);

	switch (prop_id)
	{
	    case PROP_DOCUMENT:
            do_inventory_dialog_set_document(DO_INVENTORY_DIALOG(object), g_value_get_pointer(value));
            break;
        default :
            g_assert_not_reached ();
	}
}

static void do_inventory_dialog_class_init (DoInventoryDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_inventory_dialog_constructor;
	object_class->finalize     = do_inventory_dialog_finalize;
	object_class->get_property = do_inventory_dialog_get_property;
	object_class->set_property = do_inventory_dialog_set_property;

	g_object_class_install_property
		(object_class,
		 PROP_DOCUMENT,
		 g_param_spec_pointer("document-key",
				     "Указатель на ключ документа",
				     "Указатель на ключ документа",
				     G_PARAM_WRITABLE));

	g_type_class_add_private (object_class, sizeof (DoInventoryDialogPrivate));

}

GtkWidget *do_inventory_dialog_new (document_key0_t *document_key)
{
	return g_object_new (DO_TYPE_INVENTORY_DIALOG,
                      "document-key", document_key,
			     (gpointer) NULL);
}
static void  do_inventory_dialog_set_document(DoInventoryDialog *dialog, document_key0_t *document)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
	do_alias_t *alias;

    alias = priv->alias;
    if ( !alias )
        return;
    if ( do_document_get0(alias, &priv->document, document, DO_GET_EQUAL) == DO_OK ) {
        do_cpy(priv->document_key0.dtype, priv->document.data.dtype);
        do_cpy(priv->document_key0.sklad, priv->document.data.sklad);
        do_cpy(priv->document_key0.document, priv->document.data.document);
        do_inventory_dialog_follow_init(dialog);
    }
}

static gboolean do_inventory_dialog_close_request(DoView *embed)
{
/*	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (embed);
*/
    return TRUE;
}
static void do_inventory_dialog_close(DoView *embed)
{
    if ( do_inventory_dialog_close_request(embed) )
        gtk_widget_destroy(GTK_WIDGET(embed));
}
static const char *do_inventory_dialog_get_title(DoView *embed)
{
	//DoInventoryDialogPrivate  *priv;
	//priv = DO_INVENTORY_DIALOG_GET_PRIVATE (embed);

    return "Инвентаризация";
}
static GdkPixbuf  *do_inventory_dialog_get_icon(DoView *embed)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (embed);

    return priv->icon;
}
static gboolean	do_inventory_dialog_do_grab_focus(DoView *menu)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (menu);
	gtk_widget_grab_focus(GTK_WIDGET(priv->entry));
	return TRUE;
}
static gboolean	do_inventory_dialog_get_load_status(DoView *embed)
{
    return FALSE;
}
static void do_inventory_dialog_go_prev(DoInventoryDialog *dialog)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
	if ( priv->path_order && (priv->path_order->prev || !priv->product_select) ) {
	    if ( priv->product_select )
            priv->path_order = priv->path_order->prev;
        priv->order = g_list_remove(priv->order, GPOINTER_TO_INT(0));
        do_inventory_dialog_order_read_for_path(dialog);
        do_inventory_dialog_update_status(dialog);
	}
}
static void do_inventory_dialog_go_next(DoInventoryDialog *dialog)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
	if ( priv->path_order && priv->path_order->next ) {
        priv->path_order = priv->path_order->next;
        do_inventory_dialog_order_read_for_path(dialog);
        do_inventory_dialog_update_status(dialog);
	}
}
static void do_inventory_dialog_button_clicked(GtkButton *button, DoInventoryDialog *dialog)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);

	gint number;

	number = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "number"));
	switch ( number ) {
	    case BUTTON_BACK:
            do_inventory_dialog_order_back(dialog, FALSE);
            break;
	    case BUTTON_NEXT:
            do_inventory_dialog_go_next(dialog);
            break;
	    case BUTTON_PREV:
            do_inventory_dialog_go_prev(dialog);
            break;
	    case BUTTON_QUIT:
            do_inventory_dialog_close(DO_VIEW(dialog));
            return;
	}
	gtk_widget_grab_focus(priv->entry);


}
static gboolean do_inventory_dialog_check_product(DoInventoryDialog *dialog)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
	do_alias_t *alias;

	alias = priv->alias;
    if ( !alias )
        return FALSE;
	if ( !priv->product.data.base_parcel &&
       do_product_param_int(alias, &priv->product,
         do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) ==
          DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE
     ) {
         SHOW_ERROR("Товар ЖВНЛС и должен быть введен по обклееному коду");
         return FALSE;
    }
    return TRUE;
}
static gboolean do_inventory_dialog_get_product(DoInventoryDialog *dialog, const gchar *text)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
	barcode_key0_t  barcode_key0;
	barcode_rec_t   barcode;
	product_key4_t  product_key4;
	do_alias_t *alias;
	int status;

	alias = priv->alias;
	if ( !alias )
        return FALSE;

	do_text_set(alias, barcode_key0.barcode, text);
	status = do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL);

	if ( status == DO_ERROR )
        return FALSE;

    if ( status == DO_OK )
        do_cpy(product_key4.code, barcode.data.code)
    else
        do_text_set(alias, product_key4.code, text);
    status = do_product_get4(alias, &priv->product, &product_key4, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return FALSE;
    if ( status == DO_KEY_NOT_FOUND ) {
        gchar *code;
        code = do_text(alias, product_key4.code);
        if ( g_strcmp0(text, code ))
            SHOW_ERROR("Товар \"%s\" не найден", code)
        else
            SHOW_ERROR("Товар с продажным кодом \"%s\" не найден", text);
        g_free(code);
        return FALSE;
    }
    return TRUE;
}
static void do_inventory_dialog_label_fill(DoInventoryDialog *dialog)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
	GtkAllocation allocation;
	do_alias_t *alias;
	product_rec_t *product;
	double price;

	if ( !priv->product_select ) {
        gtk_label_set_label(GTK_LABEL(priv->label), "");
        gtk_label_set_label(GTK_LABEL(priv->price[1]), "");
        gtk_label_set_label(GTK_LABEL(priv->quant[1]), "");
        gtk_label_set_label(GTK_LABEL(priv->seria[1]), "");
        gtk_label_set_label(GTK_LABEL(priv->quant_all[1]), "");
        return;
	}
	alias = priv->alias;
	if ( !alias )
        return;
	product = &priv->product;

    gint width, height;
    gtk_widget_get_size_request(priv->label, &width, &height);
    gtk_widget_get_allocation(priv->label, &allocation);
    if ( width == -1 )
        width = allocation.width;
    if ( height == -1)
        height = allocation.height;
    gtk_widget_set_size_request(priv->label, width, height);

    gchar *value, *text, *ei;
    gint coef;
    coef = do_product_coef(alias, product);

    value = do_text(alias, product->data.name);
    gtk_label_set_label(GTK_LABEL(priv->label), value);
    g_free(value);

    if ( product->data.base_parcel ) {
        text=do_product_param(alias, product, do_param(DO_PARAM_PRODUCT_PARAMETR_SERIA));
        value=g_strdup_printf("<b>%s</b>", text);
        g_free(text);
    }
    else
        value = do_strdup("");
    gtk_label_set_markup(GTK_LABEL(priv->seria[1]), value);
    gtk_widget_set_visible(priv->seria[0], product->data.base_parcel);
    gtk_widget_set_visible(priv->seria[1], product->data.base_parcel);
    g_free(value);

    gtk_widget_set_visible(priv->quant_all[0], product->data.base_parcel);
    gtk_widget_set_visible(priv->quant_all[1], product->data.base_parcel);

    if ( coef > 1 ) {
        value = do_text(alias, product->data.unit);
        ei = g_strdup_printf("1*%d %s", coef, value);
        g_free(value);
    }
    else {
        ei = do_strdup("");
    }
    price = do_product_price(alias, &priv->product, DO_CONST_PRICE_RETAIL)*coef;
    if ( price > 0 )
        text = do_money_format(price, TRUE);
    else
        text = do_strdup("<i>нет</i>");

    value = g_strdup_printf("<b>%s</b> %s", text, ei);
    g_free(text);g_free(ei);
    gtk_label_set_markup(GTK_LABEL(priv->price[1]), value);
    g_free(value);

}
static void show_error(DoInventoryDialog *dialog, const gchar *markup)
{
    SOUND_ERROR();
    do_message_dialog_show(GTK_WIDGET(dialog), GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, markup);
}
static void do_inventory_dialog_clear(DoInventoryDialog *dialog)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
    priv->product_select = FALSE;
    priv->product_quant = 0;
    do_inventory_dialog_label_fill(dialog);
    gtk_entry_set_text(GTK_ENTRY(priv->entry), "");
    priv->path_order = g_list_last(priv->order);
    do_inventory_dialog_update_status(dialog);
}
static void do_inventory_dialog_clear_quant(DoInventoryDialog *dialog)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
    priv->product_quant = 0;
    gtk_entry_set_text(GTK_ENTRY(priv->entry), "");
    do_inventory_dialog_update_quant(dialog, DEFAULT_QUANT, TRUE);
}
static void do_inventory_dialog_fill_for_product(DoInventoryDialog *dialog, gboolean insert_mode)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);

    priv->product_select = TRUE;
    priv->insert_mode = insert_mode;
    do_inventory_dialog_label_fill(dialog);
    do_inventory_dialog_order_read_for_product(dialog);
    do_inventory_dialog_update_quant(dialog, DEFAULT_QUANT, TRUE);
    gtk_entry_set_text(GTK_ENTRY(priv->entry), "");
}
static void do_inventory_dialog_entry_activated(GtkEntry *entry, DoInventoryDialog *dialog)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
	gchar *text;
	gboolean quant_set;

	text = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));

	quant_set = do_inventory_dialog_text_quant_validate(text, dialog);

	if ( quant_set || barcode_validate(text) ) {
        if ( priv->product_select ) {
            if ( !do_inventory_dialog_order_save(dialog) ) {
                g_free(text);
                return;
            }
            priv->insert_mode = FALSE;
            do_inventory_dialog_clear_quant(dialog);
        }
        if ( !quant_set ) {
            if ( do_inventory_dialog_get_product(dialog, text) &&
                 do_inventory_dialog_check_product(dialog) ) {
                SOUND_RECEIVE();
                do_inventory_dialog_clear(dialog);
                do_inventory_dialog_fill_for_product(dialog, TRUE);
            }
            else {
                gtk_entry_set_text(GTK_ENTRY(entry), text);
#if GTK_MAJOR_VERSION < 3
                gtk_entry_select_region(GTK_ENTRY(entry), 0, -1);
#endif
            }
        }
	}
	else
        SOUND_ERROR();
	do_entry_valid(DO_ENTRY(priv->entry));
	g_free(text);
    do_inventory_dialog_update_status(dialog);
}
static gboolean barcode_validate(const gchar *text)
{
    gchar *p;
    gunichar ch;
    gint len = 0;

    p = (char*) text;

    ch = g_utf8_get_char (p);
    for ( ; g_unichar_isdigit(ch); p = g_utf8_next_char (p),ch = g_utf8_get_char (p) )
        len++;
    return ( len > 0 ) && ( len <= BARCODE_LEN ) && ( *p == '\0' );
}
static void do_inventory_dialog_update_status(DoInventoryDialog *dialog)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);

	gtk_widget_set_sensitive(priv->buttons[BUTTON_PREV], (priv->path_order) ? (priv->path_order->prev!=NULL) : FALSE);
	gtk_widget_set_sensitive(priv->buttons[BUTTON_NEXT], (priv->path_order) ? (priv->path_order->next!=NULL) : FALSE);

	gtk_widget_set_sensitive(priv->buttons[BUTTON_BACK], priv->product_select && priv->insert_mode);


}
static gboolean is_int(double value, double coef)
{
    gint quant, mantica;
    quant = value / coef;
    mantica = value - quant*coef;
    return !mantica;
}
static gchar *quant_text(gint value, double coef, const gchar *unit)
{
    //if ( !value )
      //  return NULL;
    if ( coef < 2 )
        return g_strdup_printf("%d", value);

    gint quant, mantica;
    quant = value / coef;
    mantica = value - quant*coef;
    if ( mantica < 0 ) {
        quant--;
        mantica += coef;
    }
    if ( mantica )
        if ( quant )
            return g_strdup_printf("%d и %d%s", quant, mantica, unit);
        else
            return g_strdup_printf("%d%s", mantica, unit);
    else
        return g_strdup_printf("%d", quant);
}
static void  do_inventory_dialog_update_quant(DoInventoryDialog *dialog, gint quant, gboolean default_)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
	double stock_quant;
	double total_quant;
	double coef;

	gchar *stock_value = NULL;
	gchar *detail_value = NULL;
	gchar *total_value = NULL;

	gchar *unit;
	gchar o_,c_,p_;
	do_alias_t *alias;

	if ( !priv->product_select ) {
        gtk_label_set_markup(GTK_LABEL(priv->quant[1]), "");
        gtk_label_set_markup(GTK_LABEL(priv->quant_all[1]), "");
        return;
	}
    alias = priv->alias;
    if ( !alias )
        return;


    coef = do_product_coef(alias, &priv->product);
    unit = do_text(alias, priv->product.data.unit);

    priv->product_quant = quant;
    if ( default_ ) {
        if ( priv->insert_mode )
            priv->product_quant = quant*coef;
        else
            priv->product_quant = do_document_order_quant(alias, &priv->document_order, DO_CONST_QUANT_REST);
    }


    stock_quant = do_document_order_quant(alias, &priv->document_order, DO_CONST_QUANT_REST)/coef;
    if ( priv->insert_mode )
        stock_quant--;

    if ( priv->insert_mode )
        total_quant = stock_quant+priv->product_quant/coef;
    else
        total_quant = priv->product_quant/coef;

    stock_value = do_rest_format(stock_quant);
    if ( priv->insert_mode ) {
        detail_value = quant_text(priv->product_quant, coef, unit);
        if ( stock_quant != total_quant )
            total_value =  do_rest_format(total_quant);
    }
    else {
        if ( stock_quant != total_quant || !is_int(stock_quant*coef,coef) )
            detail_value = quant_text(total_quant*coef, coef, unit);
    }

    if ( priv->insert_mode )
        o_='(',c_=')',p_='+';
    else
        o_=' ',c_=']',p_='[';

    GString *string;
    string = g_string_new("");
    if ( stock_value )
        g_string_append_printf(string, "<b>%s</b> ", stock_value);
    if ( detail_value )
        g_string_append_printf(string, "<i>%c%c%s%c</i> ", o_, p_, detail_value, c_);
    if ( total_value )
        g_string_append_printf(string, "=<b>%s</b>", total_value);

    gtk_label_set_markup(GTK_LABEL(priv->quant[1]), g_string_free(string, FALSE));
    g_free(stock_value);g_free(detail_value);g_free(total_value);

    if ( priv->product.data.base_parcel ) {
        gchar *value;
        if ( priv->insert_mode )
            total_quant = (priv->product_quant_all+priv->product_quant);
        else
            total_quant = (priv->product_quant_all + (priv->product_quant-stock_quant*coef));

        total_value = quant_text(total_quant, coef, unit);
        if ( total_quant == priv->product_quant_all )
            value=g_strdup_printf("<b>%s</b>", total_value);
        else
            value=g_strdup_printf("<i>[%s]</i>", total_value);
        g_free(total_value);
        gtk_label_set_markup(GTK_LABEL(priv->quant_all[1]), value);
        g_free(value);
    }
    g_free(unit);
}
static gboolean do_inventory_dialog_text_quant_validate(const gchar *text, DoInventoryDialog *dialog)
{
	DoInventoryDialogPrivate  *priv;
	priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);

    if ( !priv->product_select )
        return FALSE;

    gchar *p;
    gunichar ch;
    gint quant=0;
    double coef;
    gint len=0;
    do_alias_t *alias;

    alias = priv->alias;

    if ( !alias )
        return FALSE;

    coef = do_product_coef(alias, &priv->product);
    p = (char*) text;
    for (ch = g_utf8_get_char (p); g_unichar_isdigit(ch); p = g_utf8_next_char (p),ch = g_utf8_get_char (p) )
        len++;
    if ( len > QUANT_LEN )
        return FALSE;

    quant = atoi(text)*coef;
    if ( *p != '\0' ) {
        if ( coef < 2 || (*p != '.' && *p != ',' && *p !='-' && *p != ' ' && *p != '/') )
            return FALSE;
        if ( *p == '-' ) {
            gchar *new_text;
            new_text = g_strdup(text);
            new_text[p-text] = '/';
            gtk_entry_set_text(GTK_ENTRY(priv->entry), new_text);
            g_free(new_text);
        }
        gchar *head;
        head = p++;
        for (ch = g_utf8_get_char (p) ; g_unichar_isdigit(ch); p = g_utf8_next_char (p),ch = g_utf8_get_char (p) )
            len++;
        if ( *p != '\0' )
            return FALSE;

        if ( head[0] == '-' || head[0] == ' ' || head[0] == '/' )
            quant += atoi(head+1);
        else {
            gchar *value;
            double mantica;
            value = g_strdup_printf("0.%s", head+1);
            mantica = do_atof(value);
            quant += coef*mantica;
            g_free(value);
        }
    }
    if ( len )
        do_inventory_dialog_update_quant(dialog, quant, FALSE);
    else {
        do_inventory_dialog_update_quant(dialog, DEFAULT_QUANT, TRUE);
        return FALSE;
    }
    return TRUE;
}

static gboolean do_inventory_dialog_text_validate(const gchar *text, DoInventoryDialog *dialog)
{
//    DoInventoryDialogPrivate  *priv;
//    priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);

    if ( do_inventory_dialog_text_quant_validate(text, dialog) )
        return TRUE;
    else {
        do_inventory_dialog_update_quant(dialog, DEFAULT_QUANT, TRUE);
        return barcode_validate(text);
    }

}
static gboolean do_inventory_dialog_order_save(DoInventoryDialog *dialog)
{
    DoInventoryDialogPrivate  *priv;
    priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
    do_alias_t *alias;
    alias = priv->alias;
    if ( !alias )
        return FALSE;

    double coef, quant;

    coef = do_product_coef(alias, &priv->product);
    quant = do_document_order_quant(alias, &priv->document_order, DO_CONST_QUANT_REST);

    if ( priv->insert_mode && priv->product_quant == coef )
        return TRUE;

    if ( !priv->insert_mode && quant == priv->product_quant )
        return TRUE;

    if ( priv->insert_mode)
        do_document_order_quant_set(alias, &priv->document_order, DO_CONST_QUANT_REST, quant - coef + priv->product_quant);
    else
        do_document_order_quant_set(alias, &priv->document_order, DO_CONST_QUANT_REST, priv->product_quant);

    int line, status;
    line = priv->document_order.data.line;
    priv->order = g_list_remove(priv->order, GINT_TO_POINTER(line));
    if ( do_document_order_quant(alias, &priv->document_order, DO_CONST_QUANT_REST) == 0 ) {
        status = do_document_order_delete(alias);
        do_inventory_dialog_clear(dialog);
    }
    else {
        status = do_document_order_update(alias, &priv->document_order);
        if ( status == DO_OK )
            priv->order = g_list_append(priv->order, GINT_TO_POINTER(line));
        if ( priv->product.data.base_parcel ) {
            if ( !do_inventory_dialog_get_quant_all(dialog) )
                return FALSE;
            status = do_document_order_get0(alias, &priv->document_order, &priv->document_order_key0, DO_GET_EQUAL);
            if ( status != DO_OK ) {
                SHOW_ERROR("Строка %s не найдена", do_util_document_order_key0_to_str(alias, &priv->document_order_key0));
                return FALSE;
            }
        }
    }
    if ( status == DO_OK ) {
        priv->path_order = g_list_last(priv->order);
        do_inventory_dialog_update_status(dialog);
    }
    return status == DO_OK;
}
static gboolean do_inventory_dialog_order_back(DoInventoryDialog *dialog, gboolean question)
{
    DoInventoryDialogPrivate  *priv;
    priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
    do_alias_t *alias;
    alias = priv->alias;
    int status;
    if ( !alias )
        return FALSE;

    if ( !priv->product_select || !priv->insert_mode )
        return TRUE;

    if ( question &&
         do_message_dialog_show(GTK_WIDGET(dialog), GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Сторнировать только введеную запись?") != GTK_RESPONSE_YES )
        return FALSE;

    double coef, quant;

    coef = do_product_coef(alias, &priv->product);
    quant = do_document_order_quant(alias, &priv->document_order, DO_CONST_QUANT_REST);

    do_document_order_quant_set(alias, &priv->document_order, DO_CONST_QUANT_REST, quant - coef);

    if ( do_document_order_quant(alias, &priv->document_order, DO_CONST_QUANT_REST) == 0 ) {
        status = do_document_order_delete(alias);
        do_inventory_dialog_update_document_comment(dialog, FALSE);
        int line;
        line = priv->document_order.data.line;
        priv->order = g_list_remove(priv->order, GINT_TO_POINTER(line));
        priv->path_order = g_list_last(priv->order);
        do_inventory_dialog_update_status(dialog);
    }
    else
        status = do_document_order_update(alias, &priv->document_order);

    if ( status == DO_OK ) {
        priv->product_select = FALSE;
        do_inventory_dialog_clear(dialog);
        gtk_entry_set_text(GTK_ENTRY(priv->entry), "");
        do_inventory_dialog_update_status(dialog);
        do_entry_valid(DO_ENTRY(priv->entry));
        return TRUE;
    }

    return FALSE;
}
int do_document_get_first_line(do_alias_t *alias, document_key0_t *document_key0)
{
    document_order_key0_t document_order_key0;
    memcpy(&document_order_key0, document_key0, sizeof(document_key0_t));
    document_order_key0.line = 0;
    if ( (do_document_order_key0(alias, &document_order_key0, DO_GET_GE) != DO_OK) ||
        memcmp(&document_order_key0, document_key0, sizeof(document_key0_t)) )
        return 0;
    return document_order_key0.line;
}
static gboolean do_inventory_dialog_get_quant_all(DoInventoryDialog *dialog)
{
    DoInventoryDialogPrivate  *priv;
    priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
    do_alias_t *alias;
    int status;

    alias = priv->alias;
    if ( !alias )
        return FALSE;

    document_order_key2_t document_order_key2;
    document_order_rec_t document_order;
    product_key4_t product_key4;
    int len;
    len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);

    do_cpy(product_key4.code, priv->product.data.code);
    product_key4.code[len] = '\0';


    do_cpy(document_order_key2.dtype, priv->document_key0.dtype);
    do_cpy(document_order_key2.sklad, priv->document_key0.sklad);
    do_cpy(document_order_key2.document, priv->document_key0.document);

    do_text_set(alias, document_order_key2.code, product_key4.code);

    priv->product_quant_all = 0;
    status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_GE);
    while ( status == DO_OK ) {
        if (
            do_cmp(document_order_key2.dtype, priv->document_key0.dtype) ||
            do_cmp(document_order_key2.sklad, priv->document_key0.sklad) ||
            do_cmp(document_order_key2.document, priv->document_key0.document) ||
            strncmp(document_order_key2.code, product_key4.code, len)
            ) break;
        priv->product_quant_all+=do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST);

        status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;

    return TRUE;
}
static gboolean do_inventory_dialog_order_read_for_product(DoInventoryDialog *dialog)
{
    DoInventoryDialogPrivate  *priv;
    priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
    do_alias_t *alias;
    int status;

    alias = priv->alias;
    if ( !alias )
        return FALSE;

    if ( priv->product.data.base_parcel )
        if ( !do_inventory_dialog_get_quant_all(dialog) )
            return FALSE;

    document_order_key2_t document_order_key2;
    do_cpy(document_order_key2.dtype, priv->document_key0.dtype);
    do_cpy(document_order_key2.sklad, priv->document_key0.sklad);
    do_cpy(document_order_key2.document, priv->document_key0.document);
    do_cpy(document_order_key2.code, priv->product.data.code);

    status = do_document_order_get2(alias, &priv->document_order, &document_order_key2, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return FALSE;

    if ( status == DO_KEY_NOT_FOUND ) {
        int line;
        line = do_document_get_last_line(alias, &priv->document_key0);

        do_cpy(priv->document_order.data.dtype, priv->document_key0.dtype);
        do_cpy(priv->document_order.data.dtype1, priv->document.data.dtype1);
        do_cpy(priv->document_order.data.sklad, priv->document_key0.sklad);
        do_cpy(priv->document_order.data.document, priv->document_key0.document);
        priv->document_order.data.line = line + 1;
        priv->document_order.data.sort = 0;
        do_cpy(priv->document_order.data.code, priv->product.data.code);
        priv->document_order.data.date = priv->document.data.date;
        priv->document_order.data.time = priv->document.data.time;
        priv->document_order.data.accepted = 0;
        do_document_order_params_clear(alias, &priv->document_order);
        do_document_order_price_clear(alias, &priv->document_order);
        do_document_order_quant_clear(alias, &priv->document_order);
        do_document_order_price_set(alias, &priv->document_order, 2, do_product_price(alias, &priv->product, 2));
        do_document_order_price_set(alias, &priv->document_order, 4, do_product_price(alias, &priv->product, 4));
        do_document_order_price_set(alias, &priv->document_order, 11, do_product_price(alias, &priv->product, 11));
        do_document_order_param_int_set(alias, &priv->document_order, "КОЛ_УПАК", do_product_coef(alias, &priv->product));
        do_document_order_quant_set(alias, &priv->document_order, DO_CONST_QUANT_REST, do_product_coef(alias, &priv->product));

        if ( do_document_order_insert(alias, &priv->document_order) != DO_OK )
            return FALSE;

        do_cpy(priv->document_order_key0.dtype, priv->document_order.data.dtype);
        do_cpy(priv->document_order_key0.document, priv->document_order.data.document);
        do_cpy(priv->document_order_key0.sklad, priv->document_order.data.sklad);
        priv->document_order_key0.line = priv->document_order.data.line;

        do_inventory_dialog_update_document_comment(dialog, TRUE);
        line = priv->document_order.data.line;
        priv->order = g_list_remove(priv->order, GINT_TO_POINTER(line));
        priv->order = g_list_append(priv->order, GINT_TO_POINTER(line));
        priv->path_order = g_list_last(priv->order);
    }
    else {
        do_document_order_quant_set(alias, &priv->document_order, DO_CONST_QUANT_REST,
                                    do_document_order_quant(alias, &priv->document_order, DO_CONST_QUANT_REST) +
                                    do_product_coef(alias, &priv->product));

        if ( do_document_order_update(alias, &priv->document_order) != DO_OK )
            return FALSE;
        int line;
        line = priv->document_order.data.line;
        priv->order = g_list_remove(priv->order, GINT_TO_POINTER(line));
        priv->order = g_list_append(priv->order, GINT_TO_POINTER(line));
        priv->path_order = g_list_last(priv->order);
        do_inventory_dialog_update_status(dialog);
    }
    return TRUE;
}
static gboolean do_inventory_dialog_order_read_for_path(DoInventoryDialog *dialog)
{
    DoInventoryDialogPrivate  *priv;
    priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
    do_alias_t *alias;
    int status;

    alias = priv->alias;
    if ( !alias )
        return FALSE;

    if ( !priv->path_order )
        return FALSE;

    int line;
    priv->insert_mode = FALSE;
    priv->product_select = FALSE;
    line = GPOINTER_TO_INT(priv->path_order->data);

    if ( line ) {

        do_cpy(priv->document_order_key0.dtype, priv->document_key0.dtype);
        do_cpy(priv->document_order_key0.sklad, priv->document_key0.sklad);
        do_cpy(priv->document_order_key0.document, priv->document_key0.document);
        priv->document_order_key0.line = line ;

        status = do_document_order_get0(alias, &priv->document_order, &priv->document_order_key0, DO_GET_EQUAL);
        if ( status != DO_OK ) {
            SHOW_ERROR("Строка %s не найдена", do_util_document_order_key0_to_str(alias, &priv->document_order_key0));
            return FALSE;
        }
        product_key4_t product_key4;
        do_cpy(product_key4.code, priv->document_order.data.code);
        status = do_product_get4(alias, &priv->product, &product_key4, DO_GET_EQUAL);
        if ( status != DO_OK ) {
            SHOW_ERROR("Товар %s не найден", do_text(alias, product_key4.code));
            return FALSE;
        }
        if ( priv->product.data.base_parcel ) {
            if ( !do_inventory_dialog_get_quant_all(dialog) )
                return FALSE;
            status = do_document_order_get0(alias, &priv->document_order, &priv->document_order_key0, DO_GET_EQUAL);
            if ( status != DO_OK ) {
                SHOW_ERROR("Строка %s не найдена", do_util_document_order_key0_to_str(alias, &priv->document_order_key0));
                return FALSE;
            }
        }
        priv->product_select = TRUE;
    }
    do_inventory_dialog_label_fill(dialog);
    priv->product_quant = 0;
    gtk_entry_set_text(GTK_ENTRY(priv->entry), "");
    do_inventory_dialog_update_quant(dialog, 0, TRUE);
    return TRUE;
}
static gboolean do_inventory_dialog_key_pressed(GtkWidget *widget, GdkEvent *event, DoInventoryDialog *dialog)
{
    DoInventoryDialogPrivate  *priv;
    priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);

	guint mask = gtk_accelerator_get_default_mod_mask ();
	if ( (event->key.state & mask) == 0 )
	{
		switch (event->key.keyval)
		{
            case GDK_Escape: {
                if ( ((char*)gtk_entry_get_text(GTK_ENTRY(priv->entry)))[0] != '\0' ) {
                    gtk_entry_set_text(GTK_ENTRY(priv->entry), "");
                }
                else {
                    if ( priv->product_select && priv->insert_mode ) {

                        if ( !do_inventory_dialog_order_back(dialog, TRUE) )
                            return TRUE;
                    }
                    else
                        do_inventory_dialog_close(DO_VIEW(dialog));
                }
                return TRUE;
            }
		}
	}
	return FALSE;
}

static gchar *do_inventory_dialog_get_line_product(DoInventoryDialog *dialog, int line)
{
    DoInventoryDialogPrivate  *priv;
    priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
    do_alias_t *alias;

    alias = priv->alias;
    if ( !alias )
        return g_strdup("");

    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    product_key4_t product_key4;
    product_rec_t product;

    do_cpy(document_order_key0.dtype, priv->document_key0.dtype);
    do_cpy(document_order_key0.sklad, priv->document_key0.sklad);
    do_cpy(document_order_key0.document, priv->document_key0.document);
    document_order_key0.line = line;

    if ( do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_EQUAL) != DO_OK)
        return g_strdup("");

    do_cpy(product_key4.code, document_order.data.code);

    if ( do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) != DO_OK)
        return g_strdup("");

    return do_text(alias, product.data.name);
}
static void  do_inventory_dialog_update_document_comment(DoInventoryDialog *dialog, gboolean insert_record)
{
    DoInventoryDialogPrivate  *priv;
    priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
    do_alias_t *alias;
    gchar *start=NULL, *end=NULL, *comment;

    alias = priv->alias;
    if ( !alias )
        return;
    start = do_document_param(alias, &priv->document, "START_DESC");
    end = do_document_param(alias, &priv->document, "END_DESC");

    if ( insert_record ) {
        if ( priv->document_order.data.line == 1 ) {
            g_free(start);
            start = do_text(alias, priv->product.data.name);
        } else {
            g_free(end);
            end = do_text(alias, priv->product.data.name);
        }
    }
    else {
        int line_f, line_l;
        line_l=do_document_get_last_line(alias, &priv->document_key0);
        line_f=do_document_get_first_line(alias, &priv->document_key0);

        if ( line_l < priv->document_order.data.line ) {
            g_free(end);
            end = do_inventory_dialog_get_line_product(dialog, line_l);
        }

        if ( line_f > priv->document_order.data.line ) {
            g_free(start);
            start = do_inventory_dialog_get_line_product(dialog, line_f);
        }

    }
    comment = g_strdup_printf("%s - %s", start, end);
    do_document_param_set(alias, &priv->document, "START_DESC", start);
    do_document_param_set(alias, &priv->document, "END_DESC", end);
    do_document_param_set(alias, &priv->document, "37", comment);
    g_free(start);g_free(end);g_free(comment);

    do_document_update(alias, &priv->document);

}
static gboolean fill_for_path(DoInventoryDialog *dialog)
{
    do_inventory_dialog_order_read_for_path(dialog);
    do_inventory_dialog_update_status(dialog);
    return FALSE;
}
static gboolean do_inventory_dialog_follow_init(DoInventoryDialog *dialog)
{
    DoInventoryDialogPrivate  *priv;
    priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
    do_alias_t *alias;

    alias = priv->alias;
    if ( !alias )
        return FALSE;

    g_list_free(priv->order);

    document_order_key0_t document_order_key0;
    int status;
    do_cpy(document_order_key0.dtype, priv->document_key0.dtype);
    do_cpy(document_order_key0.sklad, priv->document_key0.sklad);
    do_cpy(document_order_key0.document, priv->document_key0.document);
    document_order_key0.line = 0;

    status = do_document_order_key0(alias, &document_order_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if (
            do_cmp(document_order_key0.dtype, priv->document_key0.dtype) ||
            do_cmp(document_order_key0.sklad, priv->document_key0.sklad) ||
            do_cmp(document_order_key0.document, priv->document_key0.document)
            ) break;
        gint line;
        line = document_order_key0.line;
        priv->order = g_list_append(priv->order, GINT_TO_POINTER(line));

        status = do_document_order_key0(alias, &document_order_key0, DO_GET_NEXT);
    }
    priv->path_order = g_list_last(priv->order);
    g_idle_add((GSourceFunc)fill_for_path, dialog);
    return (status != DO_ERROR);
}

static gboolean do_inventory_dialog_entry_key_pressed(GtkWidget *widget, GdkEvent *event, DoInventoryDialog *dialog)
{
    //DoInventoryDialogPrivate  *priv;
    //priv = DO_INVENTORY_DIALOG_GET_PRIVATE (dialog);
    const gchar *text;

    text = gtk_entry_get_text(GTK_ENTRY(widget));
    if ( text[0] != '\0' )
        return FALSE;

	guint mask = gtk_accelerator_get_default_mod_mask ();
	if ( (event->key.state & mask) == 0 )
	{
		switch (event->key.keyval)
		{
            case GDK_Left:
                do_inventory_dialog_go_prev(dialog);
                return TRUE;
            case GDK_Right:
                do_inventory_dialog_go_next(dialog);
                return TRUE;
		}
	}
	return FALSE;
}
#endif
