
#if GTK_MAJOR_VERSION == 2


#include "do_inventory_menu.h"
#include "do_inventory_dialog.h"
#include "do_inventory_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_view_actions.h"
#include "do_icons.h"
#include "domino.h"
#include <gdk/gdkevents.h>
#include <gdk/gdkkeysyms.h>
#include <time.h>


#include <string.h>

#define DO_INVENTORY_MENU_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_INVENTORY_MENU, DoInventoryMenuPrivate))

#define SHOW_ERROR(...) {gchar *val; val = g_strdup_printf(__VA_ARGS__); do_message_dialog_show(GTK_WIDGET(menu), GTK_MESSAGE_ERROR, GTK_BUTTONS_CANCEL, val); g_free(val); }


#define FONT_SIZE 10
#define DAY_DIFF 4
#define INV_OPER "01"
#define W_W 320
#define W_H 320


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


// Menu interface
static void do_inventory_menu_view_init(DoViewIface *iface);


static void        do_inventory_menu_close(DoView *embed);
static gboolean    do_inventory_menu_close_request(DoView *embed);
static const char *do_inventory_menu_get_title(DoView *embed);
static GdkPixbuf  *do_inventory_menu_get_icon(DoView *embed);
static gboolean    do_inventory_menu_do_grab_focus(DoView *menu);
static gboolean	   do_inventory_menu_get_load_status(DoView *embed);

static void     do_inventory_menu_button_clicked(GtkButton *button, DoInventoryMenu *menu);
static gboolean	do_inventory_menu_get_document_root(DoInventoryMenu *menu);
static gboolean	do_inventory_menu_get_document_last(DoInventoryMenu *menu);
static gboolean	do_inventory_menu_document_create(DoInventoryMenu *menu);
static gboolean	do_inventory_menu_get_user(DoInventoryMenu *menu);

enum
{
	PROP_0,
};
enum
{
    BUTTON_VIEW =0,
    BUTTON_NEW,
    BUTTON_EDIT,
    BUTTON_QUIT,
    BUTTON_NO,
};

static const gchar *button_name[BUTTON_NO] =
{"Просмотр описей",
 "Новая опись",
 "Последняя опись",
 "Выход"};

static const gboolean button_disable[BUTTON_NO] =
{FALSE,
 FALSE,
 FALSE,
 FALSE};

/*
static const gchar *button_stock[BUTTON_NO] =
{ NULL,
  NULL,
  NULL,
 GTK_STOCK_QUIT
 };
*/

struct _DoInventoryMenuPrivate
{
    GtkWidget       *menu;
    GdkPixbuf       *icon;
    GtkWidget       *buttons[BUTTON_NO];
    GtkWidget       *title;

    document_key0_t  document_root_key;
    document_rec_t   document_root;
    document_key0_t  document_crnt_key;
    document_rec_t   document_crnt;

    do_alias_t       *alias;
    partner_key0_t   user_key;
    gchar           *user_name;

};

G_DEFINE_TYPE_WITH_CODE (DoInventoryMenu, do_inventory_menu, GTK_TYPE_VBOX,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_inventory_menu_view_init)
                            );

static void do_inventory_menu_view_init(DoViewIface *iface)
{
    iface->do_close        = do_inventory_menu_close;
    iface->close_request   = do_inventory_menu_close_request;
    iface->get_title       = do_inventory_menu_get_title;
    iface->get_icon        = do_inventory_menu_get_icon;
    iface->do_grab_focus   = do_inventory_menu_do_grab_focus;
    iface->get_load_status = do_inventory_menu_get_load_status;
}


static void do_inventory_menu_init(DoInventoryMenu *temp)
{
	DoInventoryMenuPrivate *priv = DO_INVENTORY_MENU_GET_PRIVATE (temp);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_PROPERTIES, DO_VIEW_ICON_SIZE, 0, NULL);
}

#define LOAD_BUFFER_SIZE 65536

static GObject *do_inventory_menu_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject                 *object;
	DoInventoryMenuPrivate  *priv;
	GtkVBox                 *vbox;
	GtkWidget               *button;
	int i;

	object = G_OBJECT_CLASS (do_inventory_menu_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv = DO_INVENTORY_MENU_GET_PRIVATE (object);
	vbox = GTK_VBOX(object);

    priv->alias = domino_alias_new(NULL);
    if ( priv->alias )
        if ( !do_alias_open(priv->alias, TRUE) ) {
            do_alias_free(priv->alias);
            priv->alias = NULL;
        }

	/*gtk_rc_parse_string("gtk-button-images = 1");*/
	gtk_rc_parse_string("style \"msw-default\" "
                        "{ GtkButton::inner-border = {0,0,10,10} } "
                        "class \"*\" style \"msw-default\"");
    priv->title = button = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
	for ( i = 0; i < BUTTON_NO; i++ ) {

	    /*if ( button_stock[i] ) {
            button = gtk_button_new_from_stock(button_stock[i]);
            gtk_button_set_image_position(GTK_BUTTON(button), GTK_POS_LEFT);
	    }
	    else*/
            button = gtk_button_new_with_label(button_name[i]);
	    if ( button_disable[i] )
            gtk_widget_set_sensitive(button, FALSE);

	    priv->buttons[i] = button;
	    g_object_set_data(G_OBJECT(button), "number", GINT_TO_POINTER(i));
	    g_signal_connect(button, "clicked", G_CALLBACK(do_inventory_menu_button_clicked), object);
	    gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
	    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NORMAL);
	    gtk_button_set_use_underline(GTK_BUTTON(button), TRUE);

	    GtkWidget *l;
	    gchar *markup;
	    l = gtk_bin_get_child(GTK_BIN(button));
	    //markup=g_strdup_printf("<span font_size=\"%d\">%s</span>", FONT_SIZE, button_name[i]);
	    markup=g_strdup_printf("<b>%s</b>", button_name[i]);
	    if ( GTK_IS_LABEL(l) )
            gtk_label_set_markup(GTK_LABEL(l), markup);
        g_free(markup);

	}

    gtk_widget_show_all(GTK_WIDGET(object));
    gtk_widget_set_visible(priv->title, FALSE);
	return object;
}

static void do_inventory_menu_finalize (GObject *object)
{
	DoInventoryMenuPrivate *priv = DO_INVENTORY_MENU_GET_PRIVATE (object);
	if ( priv->alias )
        do_alias_free(priv->alias);

    g_free(priv->user_name);

	G_OBJECT_CLASS (do_inventory_menu_parent_class)->finalize (object);
}

static void do_inventory_menu_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
//	DoInventoryMenuPrivate *priv = DO_INVENTORY_MENU_GET_PRIVATE (object);
	switch (prop_id)
	{
        default :
            g_assert_not_reached ();
	}
}

static void do_inventory_menu_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
//	DoInventoryMenuPrivate *priv = DO_INVENTORY_MENU_GET_PRIVATE (object);

	switch (prop_id)
	{
        default :
            g_assert_not_reached ();
	}
}

static void do_inventory_menu_class_init (DoInventoryMenuClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_inventory_menu_constructor;
	object_class->finalize     = do_inventory_menu_finalize;
	object_class->get_property = do_inventory_menu_get_property;
	object_class->set_property = do_inventory_menu_set_property;

	g_type_class_add_private (object_class, sizeof (DoInventoryMenuPrivate));

}

GtkWidget *do_inventory_menu_new (gint kind)
{
	return g_object_new (DO_TYPE_INVENTORY_MENU,
			     (gpointer) NULL);
}

static gboolean do_inventory_menu_close_request(DoView *embed)
{
    return TRUE;
}
static void do_inventory_menu_close(DoView *embed)
{
    gtk_widget_destroy(GTK_WIDGET(embed));
}
static const char *do_inventory_menu_get_title(DoView *embed)
{
	//DoInventoryMenuPrivate  *priv;
	//priv = DO_INVENTORY_MENU_GET_PRIVATE (embed);

    return "Инвентаризация";
}
static GdkPixbuf  *do_inventory_menu_get_icon(DoView *embed)
{
	DoInventoryMenuPrivate  *priv;
	priv = DO_INVENTORY_MENU_GET_PRIVATE (embed);

    return priv->icon;
}
static gboolean	do_inventory_menu_do_grab_focus(DoView *menu)
{
	DoInventoryMenuPrivate  *priv;
	priv = DO_INVENTORY_MENU_GET_PRIVATE (menu);
	gtk_widget_grab_focus(GTK_WIDGET(priv->buttons[BUTTON_EDIT]));
	return TRUE;
}
static gboolean	do_inventory_menu_get_load_status(DoView *embed)
{
    return FALSE;
}
static void  do_inventory_menu_button_clicked(GtkButton *button, DoInventoryMenu *menu)
{
	DoInventoryMenuPrivate  *priv;
	priv = DO_INVENTORY_MENU_GET_PRIVATE (menu);
	gint number;

	number = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "number"));

	switch ( number ) {
	    case BUTTON_VIEW: {

	        if ( !do_inventory_menu_get_document_root(menu) )
                break;
	        if ( !do_inventory_menu_get_user(menu) )
                break;

            GtkNotebook *nb;
            DoView *view;

            nb = GTK_NOTEBOOK (do_window_get_notebook (DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(menu)))));
            g_return_if_fail (nb != NULL);
            gchar *user_code;
            user_code = do_util_partner_key0_to_str(priv->alias, &priv->user_key);
            view = DO_VIEW(do_inventory_view_new(&priv->document_root_key, user_code));
            g_free(user_code);
            do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
            do_view_do_grab_focus(DO_VIEW(view));

            break;
	    }
	    case BUTTON_NEW:
	        if ( !do_inventory_menu_document_create(menu) )
                break;
	    case BUTTON_EDIT: {

	        if ( !do_inventory_menu_get_document_last(menu) )
                break;

            GtkNotebook *nb;
            DoView *view;

            nb = GTK_NOTEBOOK (do_window_get_notebook (DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(menu)))));
            g_return_if_fail (nb != NULL);

            view = DO_VIEW(do_inventory_dialog_new(&priv->document_crnt_key));
            do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
            do_view_do_grab_focus(DO_VIEW(view));

            break;
	    }
	    case BUTTON_QUIT:
            gtk_main_quit();
            break;
	}
}
static gboolean	do_inventory_menu_get_document_root(DoInventoryMenu *menu)
{
	DoInventoryMenuPrivate  *priv;
	priv = DO_INVENTORY_MENU_GET_PRIVATE (menu);

    do_alias_t *alias;

    alias = priv->alias;
    if ( !alias )
        return FALSE;

    document_key3_t document_key3, key;

    struct tm tm;
    time_t now = time(NULL);
    int status;
    gchar *oper;


    tm = *localtime(&now);

    do_text_set(alias, document_key3.dtype, "ИТ");
    do_text_set(alias, document_key3.sklad, domino_unit());
    do_date_set(&document_key3.date, tm);
    document_key3.time=0;
    do_cpy(key.dtype, document_key3.dtype);
    do_cpy(key.sklad, document_key3.sklad);

    key.date = document_key3.date-DAY_DIFF;
    document_key3.date= document_key3.date+DAY_DIFF;

    status = do_document_get3(alias, &priv->document_root, &document_key3, DO_GET_LE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.dtype, document_key3.dtype) ||
             do_cmp(key.sklad, document_key3.sklad) ||
            key.date > document_key3.date ) break;

        if ( !priv->document_root.data.accepted ) {
            oper = do_text(alias, priv->document_root.data.dtype1);
            if ( !strcmp(oper, INV_OPER) ) {
                do_free(oper);
                do_cpy(priv->document_root_key.dtype, priv->document_root.data.dtype);
                do_cpy(priv->document_root_key.sklad, priv->document_root.data.sklad);
                do_cpy(priv->document_root_key.document, priv->document_root.data.document);
                return TRUE;
            }
            do_free(oper);
        }
        status = do_document_get3(alias, &priv->document_root, &document_key3, DO_GET_PREVIOUS);
    }
    /*if ( status != DO_ERROR )
        do_message_dialog_show(GTK_WIDGET(menu), GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Документа инвентаризации не найдено!");*/
    return FALSE;
}
static gboolean	do_inventory_menu_get_document_last(DoInventoryMenu *menu)
{
	DoInventoryMenuPrivate  *priv;
	priv = DO_INVENTORY_MENU_GET_PRIVATE (menu);

    do_alias_t *alias;

    alias = priv->alias;
    if ( !alias )
        return FALSE;

    if ( !do_inventory_menu_get_document_root(menu) )
        return FALSE;
    if ( !do_inventory_menu_get_user(menu) )
        return FALSE;

    document_link_key0_t document_link_key0;
    document_link_rec_t document_link;
    int status;
    char *us_code;

    us_code = do_util_partner_key0_to_str(alias, &priv->user_key);

    do_cpy(document_link_key0.m_dtype, priv->document_root_key.dtype);
    do_cpy(document_link_key0.m_sklad, priv->document_root_key.sklad);
    do_cpy(document_link_key0.m_document, priv->document_root_key.document);
    document_link_key0.type = 106;
    document_link_key0.sort = 999999999;

    status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_LE);

    while ( status == DO_OK ) {
        if ( do_cmp(document_link_key0.m_dtype, priv->document_root_key.dtype) ||
             do_cmp(document_link_key0.m_sklad, priv->document_root_key.sklad) ||
             do_cmp(document_link_key0.m_document, priv->document_root_key.document) ||
             document_link_key0.type != 106 ) break;

        char *user_code;
        document_key0_t document_key0;
        do_cpy(document_key0.dtype, document_link.data.s_dtype);
        do_cpy(document_key0.sklad, document_link.data.s_sklad);
        do_cpy(document_key0.document, document_link.data.s_document);

        if ( do_document_get0(alias, &priv->document_crnt, &document_key0, DO_GET_EQUAL) != DO_OK ) {
            char *doc;
            doc = do_util_document_key0_to_str(alias, &priv->document_root_key);
            SHOW_ERROR("Невозможно отрыть документ %s", doc);
            do_free(doc);
            return FALSE;
        }
        user_code = do_document_param(alias, &priv->document_crnt, "75");
        if ( !g_strcmp0(user_code, us_code) ) {
            do_free(user_code);do_free(us_code);
            do_cpy(priv->document_crnt_key.dtype, document_link.data.s_dtype);
            do_cpy(priv->document_crnt_key.sklad, document_link.data.s_sklad);
            do_cpy(priv->document_crnt_key.document, document_link.data.s_document);
            return TRUE;
        }
        status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_PREVIOUS);
    }
    do_free(us_code);
    if ( status == DO_ERROR )
        return FALSE;
    if ( do_message_dialog_show(GTK_WIDGET(menu), GTK_MESSAGE_INFO, GTK_BUTTONS_YES_NO, "Подописей для пользователя не найденно. Создать новую?")
        == GTK_RESPONSE_YES ) {
        return do_inventory_menu_document_create(menu);
    }

    return FALSE;
}
static gboolean	do_inventory_menu_document_create(DoInventoryMenu *menu)
{
	DoInventoryMenuPrivate  *priv;
	priv = DO_INVENTORY_MENU_GET_PRIVATE (menu);

    do_alias_t *alias;

    alias = priv->alias;
    if ( !alias )
        return FALSE;

    if ( !do_inventory_menu_get_document_root(menu) )
        return FALSE;
    if ( !do_inventory_menu_get_user(menu) )
        return FALSE;

    int status;
    gint document_number = 0;
    char *value, *str;

    document_link_key0_t document_link_key0;
    document_link_rec_t document_link;

    do_cpy(document_link_key0.m_dtype, priv->document_root_key.dtype);
    do_cpy(document_link_key0.m_sklad, priv->document_root_key.sklad);
    do_cpy(document_link_key0.m_document, priv->document_root_key.document);
    document_link_key0.type = 106;
    document_link_key0.sort = 0;

    status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_GE);

    while ( status == DO_OK ) {
        if ( do_cmp(document_link_key0.m_dtype, priv->document_root_key.dtype) ||
             do_cmp(document_link_key0.m_sklad, priv->document_root_key.sklad) ||
             do_cmp(document_link_key0.m_document, priv->document_root_key.document) ||
             document_link_key0.type != 106 ) break;

        char *s, *ch;
        s = do_text(alias, document_link.data.s_document);
        ch = strchr(s, '.');
        if ( !ch ) {
            SHOW_ERROR("Неправильная нумерация %s", s);
            return FALSE;
        }
        if ( atoi(ch+1) > document_number )
                document_number = atoi(ch+1);
        do_free(s);
        status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;

    do_text_set(alias, priv->document_crnt.data.dtype, "ИП");
    do_text_set(alias, priv->document_crnt.data.dtype1, "01");
    do_text_set_empty(priv->document_crnt.data.name);
    do_text_set(alias, priv->document_crnt.data.sklad, domino_unit());
    str = do_text(alias, priv->document_root_key.document);
    value = g_strdup_printf("%s.%d", str, document_number+1);

    do_text_set(alias, priv->document_crnt.data.document, value);
    g_free(value);g_free(str);

    priv->document_crnt.data.p_date=0;
    priv->document_crnt.data.p_time=0;
    do_text_set_empty(priv->document_crnt.data.p_doc);
    do_text_set_empty(priv->document_crnt.data.party);
    priv->document_crnt.data.accepted=0;
    priv->document_crnt.data.date = priv->document_root.data.date;
    priv->document_crnt.data.time = priv->document_root.data.time;
    priv->document_crnt.data.p_code = priv->document_root.data.p_code;
    do_cpy(priv->document_crnt.data.p_g_code,priv->document_root.data.p_g_code);
    do_document_params_clear(alias, &priv->document_crnt);
    value = do_util_partner_key0_to_str(alias, &priv->user_key);
    do_document_param_set(alias, &priv->document_crnt, "75", value);
    g_free(value);
    do_document_param_set(alias, &priv->document_crnt, "4", priv->user_name);

    if ( do_alias_tran_begin(alias) != DO_OK )
        return FALSE;
    gboolean res;
    res = TRUE;

    if ( do_document_insert(alias, &priv->document_crnt) != DO_OK )
        res = FALSE;
    if ( res ) {
        do_cpy(priv->document_crnt_key.dtype, priv->document_crnt.data.dtype);
        do_cpy(priv->document_crnt_key.sklad, priv->document_crnt.data.sklad);
        do_cpy(priv->document_crnt_key.document, priv->document_crnt.data.document);
    }
    int sort = 0;
    if ( res ) {

        document_link_key0_t document_link_key0;
        do_cpy(document_link_key0.m_dtype, priv->document_root_key.dtype);
        do_cpy(document_link_key0.m_sklad, priv->document_root_key.sklad);
        do_cpy(document_link_key0.m_document, priv->document_root_key.document);
        document_link_key0.type = 106;
        document_link_key0.sort = 999999999;

        status = do_document_link_key0(alias, &document_link_key0, DO_GET_LE);

        if ( status == DO_OK &&
            !do_cmp(document_link_key0.m_dtype, priv->document_root_key.dtype) &&
            !do_cmp(document_link_key0.m_sklad, priv->document_root_key.sklad) &&
            !do_cmp(document_link_key0.m_document, priv->document_root_key.document) &&
             document_link_key0.type == 106 ) {
            sort = document_link_key0.sort;
        }
        if ( status == DO_ERROR )
            res = FALSE;
    }
    if ( res ) {
        document_link_rec_t document_link;

        document_link.data.type = 106;
        do_cpy(document_link.data.m_dtype, priv->document_root_key.dtype);
        do_cpy(document_link.data.m_sklad, priv->document_root_key.sklad);
        do_cpy(document_link.data.m_document, priv->document_root_key.document);
        do_cpy(document_link.data.s_dtype, priv->document_crnt_key.dtype);
        do_cpy(document_link.data.s_sklad, priv->document_crnt_key.sklad);
        do_cpy(document_link.data.s_document, priv->document_crnt_key.document);
        document_link.data.sort = sort+1;
        document_link.size = sizeof(document_link_struct_t);

        res = do_document_link_insert(alias, &document_link);
    }
    if ( res )
        res = do_alias_tran_end(alias) == DO_OK;
    else
        do_alias_tran_abort(alias);

    return res;
}

static void row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, GtkDialog *dialog)
{
    gtk_dialog_accept(dialog);
}
static gboolean	do_inventory_menu_get_user(DoInventoryMenu *menu)
{
	DoInventoryMenuPrivate  *priv;
	priv = DO_INVENTORY_MENU_GET_PRIVATE (menu);

    do_alias_t *alias;

    alias = priv->alias;
    if ( !alias )
        return FALSE;
    if ( priv->user_key.code )
        return TRUE;

    partner_rec_t partner;
    partner_key1_t partner_key1, key;
    GtkListStore *store;
    int status;
    char *crnt_user;
    gchar *user_param;

    document_rec_t document;

    if ( do_document_get0(alias, &document, &priv->document_root_key, DO_GET_EQUAL) != DO_OK ) {
        char *doc;
        doc = do_util_document_key0_to_str(alias, &priv->document_root_key);
        SHOW_ERROR("Невозможно отрыть документ %s", doc);
        do_free(doc);
        return FALSE;
    }
    user_param = g_strdup_printf("U-%s", g_get_user_name());
    crnt_user = do_document_param(alias, &document, user_param);

    store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);

    do_text_set(alias, partner_key1.g_code, "CAS");
    do_cpy(key.g_code, partner_key1.g_code);
    do_text_set_empty(partner_key1.name);

    status = do_partner_get1(alias, &partner, &partner_key1, DO_GET_GE);
    GtkTreePath *path = NULL;

    while ( status == DO_OK ) {
        if ( do_cmp(key.g_code, partner_key1.g_code) ) break;
        gchar *unit;
        unit = do_text(alias, partner.data.sklad);
        if ( !g_strcmp0(unit, domino_unit()) ) {

            gchar *name;
            GtkTreeIter iter;
            name = do_text(alias, partner.data.name);
            gtk_list_store_append(store, &iter);
            if ( !g_strcmp0(name, crnt_user) )
                path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
            gtk_list_store_set(store, &iter, 0, partner.data.code, 1, name, -1);
            g_free(name);
        }
        g_free(unit);

        status = do_partner_get1(alias, &partner, &partner_key1, DO_GET_NEXT);
    }
    do_free(crnt_user);
    if ( status == DO_ERROR ) {
        if ( path )
            gtk_tree_path_free(path);
        g_free(user_param);
        return FALSE;
    }

    GtkWidget *dialog, *content, *view;
    GtkTreeViewColumn *column;
    GtkCellRenderer *cell;
    GtkWidget *s;

    dialog = gtk_dialog_new();
    gtk_window_set_default_size (GTK_WINDOW(dialog), W_W, W_H);

    s = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(s), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gtk_dialog_add_buttons(GTK_DIALOG(dialog), "Ok", GTK_RESPONSE_ACCEPT,"Отм_ена", GTK_RESPONSE_CANCEL, NULL);

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    if ( path ) {
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(view), path, NULL, FALSE);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(view), path, NULL,  TRUE, 0.5, 0.0);
        gtk_tree_path_free(path);
    }

    g_object_set(view, "headers-visible", FALSE, NULL);
    g_signal_connect(view, "row-activated", G_CALLBACK(row_activated), dialog);

    cell = gtk_cell_renderer_text_new();
    g_object_set(cell, "font", "16", NULL);
    column = gtk_tree_view_column_new_with_attributes ("Имя", cell, "text", 1, NULL);

    gtk_tree_view_append_column (GTK_TREE_VIEW(view), column);

    gtk_container_add(GTK_CONTAINER(s), view);
    gtk_box_pack_start(GTK_BOX(content), s , TRUE, TRUE, 0);
    gtk_widget_show_all(content);


    gtk_window_fullscreen(GTK_WINDOW(dialog));

    gboolean res;

    res = FALSE;

    if ( gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT ) {
        GtkTreePath *path = NULL;
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(view), &path, NULL);
        if ( path ) {
            GtkTreeIter iter;
            GValue code = {0,}, name = {0,};
            gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path);
            gtk_tree_model_get_value(GTK_TREE_MODEL(store), &iter, 0, &code);
            gtk_tree_model_get_value(GTK_TREE_MODEL(store), &iter, 1, &name);

            do_text_set(alias, priv->user_key.g_code, "CAS");
            priv->user_key.code = g_value_get_int(&code);
            g_free(priv->user_name);
            priv->user_name=g_strdup(g_value_get_string(&name));
            gtk_tree_path_free(path);
            res = TRUE;
            gtk_label_set_text(GTK_LABEL(priv->title), priv->user_name);
            gtk_widget_set_visible(priv->title, TRUE);

            if ( do_document_get0(alias, &document, &priv->document_root_key, DO_GET_EQUAL) != DO_OK ) {
                char *doc;
                doc = do_util_document_key0_to_str(alias, &priv->document_root_key);
                SHOW_ERROR("Невозможно отрыть документ %s", doc);
                do_free(doc);
                res = FALSE;
            }
            if ( res ) {
                do_document_param_set(alias, &document, user_param, priv->user_name);
                res = do_document_update(alias, &document) == DO_OK;
            }
        }

    }

    g_free(user_param);
    gtk_widget_destroy(dialog);

    return res;

}
#endif
