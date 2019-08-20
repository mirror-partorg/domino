

#include <string.h>
#include <gtk/gtk.h>
#include <domino_action.h>

#include "do_product_edit.h"
#include "do_icons.h"
#include "do_utilx.h"
#include "do_entry.h"
#include "do_filter.h"
#include "do_message.h"

#ifndef MIN_SCREEN
#include "do_rz_view.h"
#include "do_rz_edit_view.h"
#endif
#include "do_validate.h"
#include "do_view.h"
#include "do_check_dialog.h"
#include "do_barcode_dialog.h"
#include "do_ev_view.h"
#include "domino.h"
#include "hig.h"
#include <dolib.h>
#include <domino.h>
#if GTK_MAJOR_VERSION == 2
#include <gdk/gdkkeysyms.h>
#endif


enum
{
	PROP_0,
	PROP_FLAGS,
	PROP_ACTION,
	PROP_CONTEXT,
	PROP_ALIAS,
	PROP_VIEW,
	PROP_TAB,
	PROP_UNIT,
	PROP_VIEW_OBJECT,
};

#define DO_PRODUCT_EDIT_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_PRODUCT_EDIT, DoProductEditPrivate))
#define DO_TYPE_PRODUCT_EDIT_FLAGS do_product_edit_flags_get_type()


#define OBJECT_ROOT_PATH "ProductEdit"
#define DEFAULT_WINDOW_POSITION "center-on-parent"
#define DEFAULT_WINDOW_SIZE ""

#define OBJECT_ROOT_PATH_STOCK  "ProductEditStock"
#define OBJECT_ROOT_PATH_PARCEL "ProductEditParcel"
#define OBJECT_ROOT_PATH_PRIHOD "ProductEditPrihod"
#define OBJECT_ROOT_PATH_CHECK  "ProductEditCheck"
#define OBJECT_ROOT_PATH_BARCODE "ProductEditBarcode"
#define OBJECT_ROOT_PATH_RZ "ProductEditRz"
#define OBJECT_ROOT_PATH_FALSE "ProductEditFalse"
#define OBJECT_ROOT_PATH_ORDER "ProductEditOrder"
#define COLUMN_DEFAULT_SIZE -1
#define BACK_TIMEOUT 200

#define PRIHOD_MAX_COUNT 4
#define DEFAULT_PAD 10

#ifndef GDK_KEY_1
#define GDK_KEY_1 GDK_1
#endif
#ifndef GDK_KEY_2
#define GDK_KEY_2 GDK_2
#endif
#ifndef GDK_KEY_3
#define GDK_KEY_3 GDK_3
#endif
#ifndef GDK_KEY_4
#define GDK_KEY_4 GDK_4
#endif
#ifndef GDK_KEY_5
#define GDK_KEY_5 GDK_5
#endif
#ifndef GDK_KEY_6
#define GDK_KEY_6 GDK_6
#endif
#ifndef GDK_KEY_7
#define GDK_KEY_7 GDK_7
#endif
#ifndef GDK_KEY_8
#define GDK_KEY_8 GDK_8
#endif
#ifndef GDK_KEY_9
#define GDK_KEY_9 GDK_9
#endif
#ifndef GDK_KEY_Left
#define GDK_KEY_Left GDK_Left
#endif
#ifndef GDK_KEY_Right
#define GDK_KEY_Right GDK_Right
#endif


// View interface

static void do_product_edit_view_init(DoViewIface *iface);

//static gboolean	   do_product_edit_do_grab_focus(DoView *view);
static void	       do_product_edit_do_close(DoView *view);
static const char *do_product_edit_get_title(DoView *view);
static GdkPixbuf  *do_product_edit_get_icon(DoView *view);
static void		   do_product_edit_do_save(DoView *view);
static gboolean    do_product_edit_can_do_save(DoView *view);
static gboolean    do_product_edit_can_do_close_for_esc(DoView *view);

static gboolean	view_can_do_send(DoView *view);
static void		view_do_send(DoView *view);

GType do_product_edit_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_product_edit_flags_values[] = {
         { DO_PRODUCT_EDIT_FLAGS_SHOW_ALL_DATA, "DO_PRODUCT_EDIT_FLAGS_SHOW_ALL_DATA", "show-all-data" },
         { DO_PRODUCT_EDIT_FLAGS_READ_ONLY, "DO_PRODUCT_EDIT_FLAGS_READ_ONLY", "read-only" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoProductEditFlags", _do_product_edit_flags_values);
    }
    return type;
}

enum
{
/*    DESC_ENTRY,
	CODE_ENTRY,
	NAME_ENTRY,
	UNIT_ENTRY,
	GROUP_ENTRY,
	MFR_ENTRY,
	CATEGORY_ENTRY,
	LISTA_ENTRY,
	NDS_ENTRY,*/
	N_ENTRIES
};

enum
{
    STOCK_VIEW = 0,
    PARCEL_VIEW,
    PRIHOD_VIEW,
    ORDER_VIEW,
    CHECK_VIEW,
#ifndef MIN_SCREEN
    BARCODE_VIEW,
    RZ1_VIEW,
    FALSE_VIEW,
#endif
    N_VIEW
};
const gchar *view_names[N_VIEW] = {"stock","parcel","order","check"
#ifndef MIN_SCREEN
    ,"barcode","rzi","false"
#endif
};
#define MAX_SIZE_RADIO 10
struct _DoProductEditPrivate
{
	DoProductEditFlags flags;

	GtkWidget *widget[N_ENTRIES];
	GtkWidget *entry[N_ENTRIES];
	GtkWidget *title_label;

	GtkWidget *filter_check;
	GtkWidget *check_filter_entry;
	time_t     check_filter_date1;
	time_t     check_filter_date2;
	GtkWidget *check_totally;

	GtkWidget *stock_totally;

	GtkWidget *stock_mode;

	GtkWidget *filter_order;
	GtkWidget *order_filter_entry;
	time_t     order_filter_date1;
	time_t     order_filter_date2;
	GtkWidget *order_totally;

	GtkWidget *notebook;

	GtkWidget *views[N_VIEW];

	GtkTreeStore *stores[N_VIEW];

	DoView     *view;
	DoDialogAction action;
	DoContext  *context;

	gint       retail_price_n;
	gchar      *alias_name;
	gchar      *unit;

    DoView       *parent_view;
    GtkIconTheme *icon_theme;
    product_rec_t product;

};

G_DEFINE_TYPE_WITH_CODE (DoProductEdit, do_product_edit, GTK_TYPE_BOX,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_product_edit_view_init)
                                    );

static void do_product_edit_view_init(DoViewIface *iface)
{
 //   iface->do_grab_focus = do_product_edit_do_grab_focus;
    iface->do_close      = do_product_edit_do_close;
    iface->get_title     = do_product_edit_get_title;
    iface->get_icon      = do_product_edit_get_icon;
    iface->do_save       = do_product_edit_do_save;
    iface->can_do_save   = do_product_edit_can_do_save;
    iface->can_do_close_for_esc  = do_product_edit_can_do_close_for_esc;
    iface->do_mail_send     = view_do_send;
    iface->can_do_mail_send = view_can_do_send;
}

//static GtkWidget *add_field(DoProductEdit *dialog, gint id, GtkWidget *widget);

//static void entry_activate(GtkWidget *entry, DoProductEdit *dialog);
static gboolean fill_entry(DoProductEdit *dialog);

/*static gboolean product_document_save(DoProductEdit *dialog);
static gboolean product_document_get(DoProductEdit *dialog, do_alias_t *alias, document_rec_t *document);
static gboolean product_document_update(DoProductEdit *dialog, do_alias_t *alias, document_rec_t *document);

*/
//!!static void send_update_to_updated_view(DoProductEdit *dialog, do_alias_t *alias, product_rec_t *product);

static gboolean do_product_edit_create_store_parcel(DoProductEdit *dialog);
static gboolean do_product_edit_get_parcel(DoProductEdit *dialog);
static gboolean do_product_edit_create_store_stock(DoProductEdit *dialog);
static gboolean do_product_edit_get_stock(DoProductEdit *dialog);
static gboolean do_product_edit_create_store_prihod(DoProductEdit *dialog);
static gboolean do_product_edit_get_prihod(DoProductEdit *dialog);
static gboolean do_product_edit_create_store_check(DoProductEdit *dialog);
static gboolean do_product_edit_get_check(DoProductEdit *dialog);
static gboolean do_product_edit_create_store_order(DoProductEdit *dialog);
static gboolean do_product_edit_get_order(DoProductEdit *dialog);
#ifndef CASH
static GtkWidget *do_product_edit_create_stock_mode(DoProductEdit *dialog);
#endif
#ifndef MIN_SCREEN
static gboolean do_product_edit_create_store_barcode(DoProductEdit *dialog);
static gboolean do_product_edit_get_barcode(DoProductEdit *dialog);
static gboolean do_product_edit_create_store_rz(DoProductEdit *dialog);
static gboolean do_product_edit_get_rz(DoProductEdit *dialog);
static gboolean do_product_edit_create_store_false(DoProductEdit *dialog);
static gboolean do_product_edit_get_false(DoProductEdit *dialog);
#endif
static void do_product_edit_order_filter_entry_activate(GtkEntry *entry, DoProductEdit *dialog);

static void do_product_edit_init(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    priv->icon_theme = gtk_icon_theme_get_default();
}

static void do_product_edit_switch_tab (GtkNotebook     *notebook,
                                                        GtkWidget *page,
                                                        guint            page_num,
                                                      DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    do_view_actions_refresh();
    switch (page_num) {
        case STOCK_VIEW:
            if (!priv->stores[STOCK_VIEW])
                do_product_edit_create_store_stock(dialog);
            break;
        case PARCEL_VIEW:
            if (!priv->stores[PARCEL_VIEW])
                do_product_edit_create_store_parcel(dialog);
            break;
        case PRIHOD_VIEW:
            if (!priv->stores[PRIHOD_VIEW])
                do_product_edit_create_store_prihod(dialog);
            break;
        case ORDER_VIEW:
            if (!priv->stores[ORDER_VIEW])
                do_product_edit_create_store_order(dialog);
            break;
        case CHECK_VIEW:
            if (!priv->stores[CHECK_VIEW])
                do_product_edit_create_store_check(dialog);
            break;
#ifndef MIN_SCREEN
        case BARCODE_VIEW:
            if (!priv->stores[BARCODE_VIEW])
                do_product_edit_create_store_barcode(dialog);
            break;
        case RZ1_VIEW:
            if (!priv->stores[RZ1_VIEW])
                do_product_edit_create_store_rz(dialog);
            break;
        case FALSE_VIEW:
            if (!priv->stores[FALSE_VIEW])
                do_product_edit_create_store_false(dialog);
            break;
#endif
    };
    return;

}
#if GTK_MAJOR_VERSION == 2
static gint do_product_edit_get_widget_text_width(GtkWidget *widget, gint len)
{
    PangoContext *context;
	PangoFontMetrics *metrics;
	GtkStyle *style;
    gint char_width, width;
	context = gtk_widget_get_pango_context (widget);
	style = gtk_widget_get_style(widget);
	metrics = pango_context_get_metrics (context,
					     style->font_desc,
					     pango_context_get_language (context));

	char_width = pango_font_metrics_get_approximate_digit_width (metrics);
	width =  (PANGO_PIXELS (char_width) * len);
	pango_font_metrics_unref (metrics);

    return width;

}
#endif
#ifdef MIN_SCREEN
/*static gint do_product_edit_get_widget_text_height(GtkWidget *widget, gint lines)
{
	PangoLayout  *layout;
	PangoRectangle logical_rect;
	layout=gtk_widget_create_pango_layout (widget, "0\n");
	pango_layout_get_pixel_extents (layout, NULL, &logical_rect);
	return logical_rect.height;//)*lines;
}
*/
#endif
static gboolean do_product_edit_key_press(GtkWidget *widget, GdkEventKey *event, DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    gint indx = 0;
    //guint mask;

    if ( event->keyval == GDK_KEY_Right ) {
        indx = gtk_notebook_get_current_page(GTK_NOTEBOOK(priv->notebook)) + 2;
        if ( indx == N_VIEW + 1 )
            indx = 1;
    }
    if ( event->keyval == GDK_KEY_Left ) {
        indx = gtk_notebook_get_current_page(GTK_NOTEBOOK(priv->notebook));
        if ( !indx )
            indx = N_VIEW;
    }
    /*mask = gtk_accelerator_get_default_mod_mask ();
    dg
	if ((event->state & mask) == GDK_CONTROL_MASK)
	{
		switch (event->keyval)
		{
		    case GDK_KEY_1:
                indx = 1;
                break;
		    case GDK_KEY_2:
                indx = 2;
                break;
		    case GDK_KEY_3:
                indx = 3;
                break;
		    case GDK_KEY_4:
                indx = 4;
                break;
		    case GDK_KEY_5:
                indx = 5;
                break;
		    case GDK_KEY_6:
                indx = 6;
                break;
		    case GDK_KEY_7:
                indx = 7;
                break;
		    case GDK_KEY_8:
                indx = 8;
                break;
		    case GDK_KEY_9:
                indx = 9;
                break;
		}
	}*/
	if (indx && indx <= N_VIEW) {
	    if (indx - 1 != gtk_notebook_get_current_page(GTK_NOTEBOOK(priv->notebook))) {
            gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), indx - 1);
            do_tree_view_grab_focus(DO_TREE_VIEW(priv->views[indx - 1]));
	    }
	    return TRUE;
	}
	return FALSE;
}
static void do_product_edit_check_filter_entry_activate(GtkEntry *entry, DoProductEdit *dialog)
{
    DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE(dialog);
    gchar *text;
    do_scanner_t scan;
    struct tm tm;

    if (!do_validate_valid(DO_VALIDATE(entry)))
        return;

    g_object_get(entry, "text", &text, NULL);
    do_scanner_init(&scan, text, (iconv_t)-1);

    if (do_scanner_step(&scan) != do_lx_cdate)
        return;
    if (!do_atodate(scan.lexem, &tm))
        return;
    if ((priv->check_filter_date1 = mktime(&tm)) == -1)
        return;
    if (do_scanner_step(&scan) != do_lx_cdate)
        return;
    if (!do_atodate(scan.lexem, &tm))
        return;
    if ((priv->check_filter_date2 = mktime(&tm)) == -1)
        return;
    if (do_product_edit_get_check(dialog))
        do_tree_view_grab_focus(DO_TREE_VIEW(priv->views[CHECK_VIEW]));
}
static void do_product_edit_check_activate(GtkTreeView       *tree_view,
                                                        GtkTreePath       *path,
                                                        GtkTreeViewColumn *column,
                                                        DoProductEdit *dialog)
{
    DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE(dialog);
    DoContext *context;
    GValue cash = {0,};
    GValue shift = {0,};
    GValue check = {0,};
    GValue crnt = {0,};
    GtkTreeIter iter;
    DoCheckDialogFlags flags = 0;
    GtkWidget *check_dialog;

    if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->stores[CHECK_VIEW]), &iter, path))
        return;

    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[CHECK_VIEW]), &iter, 0, &cash);
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[CHECK_VIEW]), &iter, 1, &shift);
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[CHECK_VIEW]), &iter, 2, &check);
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[CHECK_VIEW]), &iter, 9, &crnt);

    context = do_context_new_with_attributes(DO_CONTEXT_KIND_CHECK,
               "cash", g_value_get_int(&cash),
               "shift", g_value_get_int(&shift),
               "check", g_value_get_int(&check),
               NULL);
    if (g_value_get_boolean(&crnt))
        flags = DO_CHECK_DIALOG_FLAGS_FROM_CURRENT;

    check_dialog = do_check_dialog_new(GTK_WIDGET(dialog), priv->alias_name, flags, context);
    do_check_dialog_run(DO_CHECK_DIALOG(check_dialog));
    g_object_unref(context);
    gtk_widget_destroy(check_dialog);

}

static void do_product_edit_order_filter_entry_activate(GtkEntry *entry, DoProductEdit *dialog)
{
    DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE(dialog);
    gchar *text;
    do_scanner_t scan;
    struct tm tm;

    if (!do_validate_valid(DO_VALIDATE(entry)))
        return;

    g_object_get(entry, "text", &text, NULL);
    do_scanner_init(&scan, text, (iconv_t)-1);

    if (do_scanner_step(&scan) != do_lx_cdate)
        return;
    if (!do_atodate(scan.lexem, &tm))
        return;
    if ((priv->order_filter_date1 = mktime(&tm)) == -1)
        return;
    if (do_scanner_step(&scan) != do_lx_cdate)
        return;
    if (!do_atodate(scan.lexem, &tm))
        return;
    if ((priv->order_filter_date2 = mktime(&tm)) == -1)
        return;
    if (do_product_edit_get_order(dialog))
        do_tree_view_grab_focus(DO_TREE_VIEW(priv->views[ORDER_VIEW]));
}
#ifndef MIN_SCREEN
static void do_product_edit_barcode_activate(GtkTreeView       *tree_view,
                                                        GtkTreePath       *path,
                                                        GtkTreeViewColumn *column,
                                                        DoProductEdit *dialog)
{
    DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE(dialog);
    GValue barcode = {0,};
    GtkTreeIter iter;

    if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->stores[BARCODE_VIEW]), &iter, path))
        return;

    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[BARCODE_VIEW]), &iter, 0, &barcode);
    if ( g_value_get_string(&barcode)[0] == '\0' )
        return;

	GtkWidget *d = do_barcode_dialog_new(GTK_WIDGET(dialog), NULL, g_value_get_string(&barcode));

    do_end_long_operation(GTK_WIDGET(dialog));
    gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(GTK_WIDGET(d));
    //gtk_widget_show(GTK_WIDGET(d));

}
#endif
static void do_product_edit_run_python(const gchar *filename, ...) DO_GNUC_NULL_TERMINATED;

static void do_product_edit_run_python(const gchar *filename, ...)
{
    gchar *param = NULL;
#ifdef _WIN32
    gchar *cmdline, *ch;
    gchar *path;
    gchar *script;
    param = g_strdup_printf("/scripts/%s",filename);
    path = param;
    ch = param + strlen(param);
    while (*ch != '/' && *ch != '\\') ch--;
    *ch = '\0';
    script = ch + 1;
    va_list ap;
    va_start(ap, filename);
    gchar *arg = NULL, *str;
    int n =0;
    param=do_strdup("");
    while ( n < 32 ) {
        arg = va_arg(ap, gchar*);
        if ( !arg ) break;
        str = do_strdup_printf("%s %s", param, arg);
        do_free(param);
        param=str;
        n++;
    };
    va_end(ap);
    cmdline = g_strdup_printf("z: && cd %s && %s %s %s", path, "C:\\Python34\\pythonw.exe ", script, param);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char *argv0 = NULL;

    memset (&si, 0, sizeof (si));
    si.cb = sizeof (&si);
    memset (&pi, 0, sizeof (pi));

    //if ( !CreateProcess (argv0, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) )
    popen (cmdline, "r");
    //if ( popen (cmdline, "r") != NULL )
    //    DOMINO_SHOW_ERROR("Не удалось запустить программу: %s", g_win32_error_message (GetLastError ()));
    g_free(cmdline);
    CloseHandle (pi.hThread);
#else
    const char *prog;
    GError *error = NULL;
    DOMINO_LOCAL_GET("Extended", "python", &prog, NULL);
    if ( !prog || prog[0] == '\0' )
        prog = "python3";
    param = g_strdup_printf("/work/domino/domino/project/apteka/scripts/%s",filename);

    va_list ap;
    gchar *argv[33], *arg = NULL;
    int n=0;
    argv[n++] = (gchar*)prog;
    argv[n++] = param;
    va_start(ap, filename);
    while ( n < 32 ) {
        arg = va_arg(ap, gchar*);
        if ( !arg ) break;
        argv[n] = arg;
        n+=1;
    };
    va_end(ap);
    argv[n] = NULL;
    if ( !g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error) ) {
        DOMINO_SHOW_ERROR("Не удалось запустить программу: %s", error->message);
        g_error_free (error);
    }
#endif
    g_free(param);
}
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
#ifdef gtk_message_dialog_new
#undef gtk_message_dialog_new
#endif
    widget = gtk_message_dialog_new(GTK_WINDOW(win), GTK_DIALOG_MODAL, type, GTK_BUTTONS_NONE, NULL);
#define gtk_message_dialog_new  dead
    gtk_dialog_add_buttons(GTK_DIALOG(widget),  "Реестр", GTK_RESPONSE_YES,"Штрихкод",GTK_RESPONSE_NO, NULL);


	gtk_window_set_icon_name (GTK_WINDOW(widget), DO_STOCK_DOMINO_APPS);
	gtk_window_set_position (GTK_WINDOW(widget), GTK_WIN_POS_CENTER);

    if ( msg )
        gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (widget), msg);
    return widget;
}

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

static void do_product_edit_parcel_activate(GtkTreeView       *tree_view,
                                                        GtkTreePath       *path,
                                                        GtkTreeViewColumn *column,
                                                        DoProductEdit *dialog)
{
    DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE(dialog);
    GValue barcode = {0,};
    GtkTreeIter iter;

    if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->stores[PARCEL_VIEW]), &iter, path))
        return;

    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[PARCEL_VIEW]), &iter, 7, &barcode);
#ifdef SHOW_CERT
    gboolean show_cert = FALSE;
    if ( g_value_get_string(&barcode)[0] != '\0' ) {
        int res;

        res = do_message_dialog_show(GTK_WIDGET(dialog), GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Печать штрихкода или поиск реестра сертификатов?");
        if ( res == GTK_RESPONSE_YES )
            show_cert = TRUE;
        else {
            if ( res != GTK_RESPONSE_NO )
                return;
        }
    }
    else
        show_cert = TRUE;

    if ( show_cert ) {

        GValue code = {0,};
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[PARCEL_VIEW]), &iter, 0, &code);
        gchar  *code1,*args;
        g_object_get(priv->context, "product-code", &code1, NULL);
        args=g_strdup_printf("%s.%s",code1,g_value_get_string(&code));
        do_product_edit_run_python("user/print-cert.py", args, NULL);
        g_free(args);
        return;
    }

#endif
    if ( g_value_get_string(&barcode)[0] != '\0' ) {

        GtkWidget *d = do_barcode_dialog_new(GTK_WIDGET(dialog), NULL, g_value_get_string(&barcode));

        do_end_long_operation(GTK_WIDGET(dialog));
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(GTK_WIDGET(d));
        //gtk_widget_show(GTK_WIDGET(d));
    }

}

#ifndef MIN_SCREEN
static void do_product_edit_rz_activate(GtkTreeView       *tree_view,
                                                        GtkTreePath       *path,
                                                        GtkTreeViewColumn *column,
                                                        DoProductEdit *dialog)
{
    DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE(dialog);
    GValue dtype = {0,};
    GValue sklad = {0,};
    GValue document = {0,};
    GValue line = {0,};
    GtkTreeIter iter;

    if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->stores[RZ1_VIEW]), &iter, path))
        return;

    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[RZ1_VIEW]), &iter, 8, &dtype);
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[RZ1_VIEW]), &iter, 9, &sklad);
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[RZ1_VIEW]), &iter, 10, &document);
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[RZ1_VIEW]), &iter, 11, &line);
    GtkWidget *dialog1;
    DoContext *context;

    context = do_context_new_with_attributes(DO_CONTEXT_KIND_LINE,
     "dtype-code", g_value_get_string(&dtype),
     "sklad-code", g_value_get_string(&sklad),
     "document-number", g_value_get_string(&document),
     "line-number", g_value_get_int(&line),
     NULL);

    dialog1 = do_rz_edit_view_new(DO_VIEW(dialog), priv->alias_name, 0, DO_DIALOG_ACTION_NONE, context);

	GtkNotebook *nb;
	nb = GTK_NOTEBOOK (do_window_get_notebook (
	                    DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(dialog)))));
    do_notebook_add_tab(DO_NOTEBOOK(nb), DO_VIEW(dialog1), -1, TRUE);
    g_object_unref(context);

}
#endif
#ifndef MIN_SCREEN

static void do_product_edit_false_activate(GtkTreeView       *tree_view,
                                                        GtkTreePath       *path,
                                                        GtkTreeViewColumn *column,
                                                        DoProductEdit *dialog)
{
    DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE(dialog);
    GValue url = {0,};
    GValue doc = {0,};
    GtkTreeIter iter;

    if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->stores[FALSE_VIEW]), &iter, path))
        return;

    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[FALSE_VIEW]), &iter, 6, &url);
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[FALSE_VIEW]), &iter, 5, &doc);

    gchar *temp;
    GFile *src;
    GFile *dst;
    GError *error = NULL;
    gchar *tmp_name = "1.pdf";
    gchar *pdf_reader;
    const gchar *src_file;


    DOMINO_LOCAL_GET("Extended", "pdf", &pdf_reader, NULL);


    temp = g_strdup_printf("%s/%s", g_get_tmp_dir(), tmp_name);
    src_file = g_value_get_string(&url);

    if ( src_file[0] == '\0' ) {
        DOMINO_SHOW_INFO("Документа не найденно");
        return;
    }

    src = g_file_new_for_uri(src_file);


    dst = g_file_new_for_path(temp);
    if (!g_file_copy(src,
                dst,
                G_FILE_COPY_OVERWRITE,
                NULL,
                NULL,
                NULL,
                &error)) {
                DOMINO_SHOW_ERROR("Копирование %s->%s\n%s", src_file, temp, error->message);
                g_error_free (error);
    }
    else {
#ifdef USE_EVINCE

        GtkNotebook *nb;
        DoView *view;
        GtkWindow *window;
        window = domino_get_main_window();
        nb = GTK_NOTEBOOK (do_window_get_notebook (DO_WINDOW(window)));
        g_return_if_fail (nb != NULL);

        view = DO_VIEW(do_ev_view_new());

        do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
        do_view_do_grab_focus(DO_VIEW(view));
        gchar *uri;
        uri = g_strdup_printf("file://%s", temp);
        g_object_set(view, "uri",uri, "title", g_value_get_string(&doc) , "print-with-margin", TRUE, NULL);
        g_free(uri);

}
#else
#ifdef _WIN32
        gchar *cmdline;
        gchar *localfile;
        gsize br, bw;

        localfile = g_locale_from_utf8(temp, strlen(temp), &br, &bw, &error);
        cmdline = g_strdup_printf("%s %s", pdf_reader, localfile);
        g_free(localfile);
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        char *argv0 = NULL;
      //"p:\\windows\\system32\\calc.exe";

        memset (&si, 0, sizeof (si));
        si.cb = sizeof (&si);
        memset (&pi, 0, sizeof (pi));

        if (!CreateProcess (argv0, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            DOMINO_SHOW_ERROR("Не удалось запустить программу: %s", g_win32_error_message (GetLastError ()));

        g_free(cmdline);
        CloseHandle (pi.hThread);
#else
        gchar *argv[3] = {pdf_reader, temp, NULL};
        if (!g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
            DOMINO_SHOW_ERROR("Не удалось запустить программу: %s", error->message);
            g_error_free (error);
        }
#endif
    }
#endif

}
#endif
gboolean do_product_edit_close(DoProductEdit *edit)
{
    gtk_widget_destroy(GTK_WIDGET(edit));
    return TRUE;
}

#ifndef MIN_SCREEN
static void do_product_edit_clicked_close(GtkButton *widget, DoProductEdit *edit)
{
    do_product_edit_close(edit);
}
#endif
#ifdef MIN_SCREEN
int label_width=0,label_height=60;

static void wrapped_label_size_allocate_callback(GtkWidget *label,
		      GtkAllocation *allocation,
		    		      gpointer data) {

	label_width=allocation->width;
	//label_height=do_product_edit_get_widget_text_height(label,5);
        gtk_widget_set_size_request(label, label_width, -1);//label_height);
}
#endif
static GObject *do_product_edit_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	//!!GtkWindow *window;
	DoProductEdit *product_edit;
	DoProductEditPrivate *priv;
	GtkWidget *vbox;
	GtkWidget *entry;
	GtkWidget *button;
    GtkWidget *l;
    GtkWidget *w;
    GtkWidget *t;
    GtkWidget *tb;
    GtkWidget *filter;
    GtkTreeView *tree_view;
    gint i;

	int row = 0;
	object = G_OBJECT_CLASS (do_product_edit_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_PRODUCT_EDIT_GET_PRIVATE(object);
	vbox = GTK_WIDGET(object);
	product_edit = DO_PRODUCT_EDIT(object);

	gtk_box_set_spacing (GTK_BOX (vbox), 2); /* Message has 24, we want 12 = 2 + 2 * 5 */

    t = hig_workarea_create();
    //gtk_table_set_homogeneous(GTK_TABLE(t), TRUE);

    g_object_set(object, "orientation", GTK_ORIENTATION_VERTICAL, NULL);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, TRUE, 0);

    priv->title_label = hig_workarea_add_section_title(t, &row, "Товар ");
#ifdef MIN_SCREEN
    gtk_label_set_line_wrap(GTK_LABEL(priv->title_label), TRUE);
    if ( label_width ) {
        gtk_widget_set_size_request(GTK_WIDGET(priv->title_label), label_width, -1);
    }
    else
        g_signal_connect(G_OBJECT(priv->title_label), "size-allocate",
	      G_CALLBACK(wrapped_label_size_allocate_callback), NULL);
    //gtk_widget_set_size_request(GTK_WIDGET(priv->title_label), 700, 60);
    //gtk_widget_set_size_request(GTK_WIDGET(priv->title_label),1600,-1);
    //gtk_label_set_lines(GTK_LABEL(priv->title_label),3);
    //gtk_label_set_line_wrap_mode(GTK_LABEL(priv->title_label), );

#endif

/*	l = add_field(product_edit, DESC_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Описание:", l, NULL);
    gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);
*/
/*	l = add_field(product_edit, NAME_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Наименование:", l, NULL);
    gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);

	l = add_field(product_edit, CODE_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Код:", l, NULL);
    gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);

	l = add_field(product_edit, UNIT_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Фасовка:", l,NULL);
    gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);
    gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_PRIMARY, GTK_STOCK_FIND);

	l = add_field(product_edit, GROUP_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Группа:", l,NULL);
    gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);
    gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_PRIMARY, GTK_STOCK_FIND);

	l = add_field(product_edit, MFR_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Производитель:", l,NULL);
    gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);
    gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_PRIMARY, GTK_STOCK_FIND);

    if ((priv->flags & DO_PRODUCT_EDIT_FLAGS_SHOW_ALL_DATA) ==
         DO_PRODUCT_EDIT_FLAGS_SHOW_ALL_DATA) {
        hig_workarea_add_section_title(t, &row, "Дополнительные характеристики ");

        l = add_field(product_edit, CATEGORY_ENTRY, do_entry_new());
        hig_workarea_add_row(t, &row, "Категория:", l,NULL);

        l = add_field(product_edit, LISTA_ENTRY, do_entry_new());
        hig_workarea_add_row(t, &row, "Список:", l,NULL);

        l = add_field(product_edit, NDS_ENTRY, do_entry_new());
        hig_workarea_add_row(t, &row, "Ставка НДС:", l,NULL);

    }
    */
    w =  priv->notebook = gtk_notebook_new();
    g_signal_connect (w, "switch-page",
                      G_CALLBACK (do_product_edit_switch_tab), object);

    l =  gtk_label_new (NULL);
    gtk_label_set_markup_with_mnemonic (GTK_LABEL (l), "_Остатки");
    tb = priv->views[STOCK_VIEW] = do_tree_view_new(OBJECT_ROOT_PATH_STOCK);
    priv->stock_totally = gtk_label_new(NULL);
    do_tree_view_pack_footer(DO_TREE_VIEW(tb), priv->stock_totally);
    gtk_widget_show_all(priv->stock_totally);
    gtk_notebook_append_page(GTK_NOTEBOOK(w), tb, l);
    g_object_get(tb, "tree-view", &tree_view, NULL);
#ifndef CASH
    priv->stock_mode = l = do_product_edit_create_stock_mode(product_edit);
    do_tree_view_pack_header(DO_TREE_VIEW(tb), l);
#endif

    l = gtk_label_new(NULL);
    gtk_label_set_markup_with_mnemonic (GTK_LABEL (l), "_Партии");
    tb = priv->views[PARCEL_VIEW] = do_tree_view_new(OBJECT_ROOT_PATH_PARCEL);
    DoViewIface *iface;
    iface = DO_VIEW_GET_IFACE(tb);
    iface->do_mail_send     = view_do_send;
    iface->can_do_mail_send = view_can_do_send;

    gtk_notebook_append_page(GTK_NOTEBOOK(w), tb, l);
    g_object_get(tb, "tree-view", &tree_view, NULL);
    g_signal_connect (tree_view, "row-activated",
                        G_CALLBACK (do_product_edit_parcel_activate), object);

    l = gtk_label_new(NULL);
    gtk_label_set_markup_with_mnemonic (GTK_LABEL (l), "Пр_иходы");
    tb = priv->views[PRIHOD_VIEW] = do_tree_view_new(OBJECT_ROOT_PATH_PRIHOD);
    gtk_notebook_append_page(GTK_NOTEBOOK(w), tb, l);

    l = gtk_label_new(NULL);
    gtk_label_set_markup_with_mnemonic (GTK_LABEL (l), "_Заказы");
    tb = priv->views[ORDER_VIEW] = do_tree_view_new(OBJECT_ROOT_PATH_ORDER);
    gtk_notebook_append_page(GTK_NOTEBOOK(w), tb, l);
    g_object_get(tb, "tree-view", &tree_view, NULL);

    filter = priv->filter_order = do_filter_new(GTK_WIDGET(vbox), 0);
    //!!filter = priv->filter_order = do_filter_new(GTK_WIDGET(vboxindow));
    do_tree_view_pack_header(DO_TREE_VIEW(tb), filter);
    gtk_widget_show_all(filter);
    priv->order_totally = gtk_label_new(NULL);
    do_tree_view_pack_footer(DO_TREE_VIEW(tb), priv->order_totally);
    gtk_widget_show_all(priv->order_totally);

    g_object_get(filter, "entry", &entry, "button", &button, NULL);
    priv->order_filter_entry = entry;
    gtk_widget_set_sensitive (button, FALSE);
#if GTK_MAJOR_VERSION == 2
    gtk_widget_set_size_request(entry,
                do_product_edit_get_widget_text_width(entry, 22) + 60u, -1u);
#endif
    g_object_set(entry, "regex-mask", "^([0-3][0-9]/[0-1][0-9]/[1-2][0-9][0-9][0-9] [0-3][0-9]/[0-1][0-9]/[1-2][0-9][0-9][0-9])$", NULL);
    g_signal_connect(entry, "activate",
                      G_CALLBACK (do_product_edit_order_filter_entry_activate), object);
    l = gtk_label_new(NULL);
    gtk_label_set_markup_with_mnemonic (GTK_LABEL (l), "_Чеки");
    tb = priv->views[CHECK_VIEW] = do_tree_view_new(OBJECT_ROOT_PATH_CHECK);
    gtk_notebook_append_page(GTK_NOTEBOOK(w), tb, l);
    g_object_get(tb, "tree-view", &tree_view, NULL);
    g_signal_connect(tree_view, "row-activated",
                        G_CALLBACK (do_product_edit_check_activate), object);

//!!    filter = priv->filter_check = do_filter_new(GTK_WIDGET(window));
    filter = priv->filter_check = do_filter_new(GTK_WIDGET(vbox), 0);
    do_tree_view_pack_header(DO_TREE_VIEW(tb), filter);
    gtk_widget_show_all(filter);
    priv->check_totally = gtk_label_new(NULL);
    do_tree_view_pack_footer(DO_TREE_VIEW(tb), priv->check_totally);
    gtk_widget_show_all(priv->check_totally);

    g_object_get(filter, "entry", &entry, "button", &button, NULL);
    priv->check_filter_entry = entry;
    gtk_widget_set_sensitive (button, FALSE);
#if GTK_MAJOR_VERSION == 2
    gtk_widget_set_size_request(entry,
                do_product_edit_get_widget_text_width(entry, 22) + 60u, -1u);
#endif
    g_object_set(entry, "regex-mask", "^([0-3][0-9]/[0-1][0-9]/[1-2][0-9][0-9][0-9] [0-3][0-9]/[0-1][0-9]/[1-2][0-9][0-9][0-9])$", NULL);
    g_signal_connect (entry, "activate",
                      G_CALLBACK (do_product_edit_check_filter_entry_activate), object);
#ifndef MIN_SCREEN
    l = gtk_label_new(NULL);
    gtk_label_set_markup_with_mnemonic (GTK_LABEL (l), "Про_д.коды");
    tb = priv->views[BARCODE_VIEW] = do_tree_view_new(OBJECT_ROOT_PATH_BARCODE);
    gtk_notebook_append_page(GTK_NOTEBOOK(w), tb, l);
    g_object_get(tb, "tree-view", &tree_view, NULL);
    g_signal_connect (tree_view, "row-activated",
                        G_CALLBACK (do_product_edit_barcode_activate), object);

    l = gtk_label_new(NULL);
    gtk_label_set_markup_with_mnemonic (GTK_LABEL (l), "Росздравнадзор");

    tb = priv->views[RZ1_VIEW] = do_tree_view_new(OBJECT_ROOT_PATH_RZ);
    gtk_notebook_append_page(GTK_NOTEBOOK(w), tb, l);
    g_object_get(tb, "tree-view", &tree_view, NULL);
#ifndef MIN_SCREEN
    g_signal_connect (tree_view, "row-activated",
                        G_CALLBACK (do_product_edit_rz_activate), object);
#endif

    l = gtk_label_new(NULL);
    gtk_label_set_markup_with_mnemonic (GTK_LABEL (l), "Фальсификация");
    tb = priv->views[FALSE_VIEW] = do_tree_view_new(OBJECT_ROOT_PATH_FALSE);
    gtk_notebook_append_page(GTK_NOTEBOOK(w), tb, l);
    g_object_get(tb, "tree-view", &tree_view, NULL);
    g_signal_connect (tree_view, "row-activated",
                        G_CALLBACK (do_product_edit_false_activate), object);
#endif

    for (i = 0; i < N_VIEW; i++) {
        GtkWidget *view;
        g_object_get(priv->views[i], "tree-view", &view , NULL);
        g_signal_connect(view, "key-press-event",
                 G_CALLBACK (do_product_edit_key_press), object);
    }

    hig_workarea_finish( t, &row );

	gtk_box_pack_start (GTK_BOX (vbox), w, TRUE, TRUE, 0);

#ifndef MIN_SCREEN

#if GTK_MAJOR_VERSION == 2
    t=gtk_hbox_new(FALSE, 2);
#else
    t=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
#endif
    gtk_container_set_border_width( GTK_CONTAINER( t ), GUI_PAD_BIG );
	gtk_box_pack_end (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, TRUE, 0);

	l = gtk_button_new_with_label("Закрыть");
	g_signal_connect(l, "clicked", G_CALLBACK(do_product_edit_clicked_close), product_edit);
	gtk_box_pack_end (GTK_BOX (t), GTK_WIDGET (l), FALSE, TRUE, 0);

	l = gtk_button_new_with_label("Сохранить");
	g_signal_connect(l, "clicked", G_CALLBACK(do_product_edit_clicked_close), product_edit);
	gtk_box_pack_end (GTK_BOX (t), GTK_WIDGET (l), FALSE, TRUE, 0);
#endif
    fill_entry(product_edit);

	gtk_widget_show_all (vbox);

	if ((priv->flags & DO_PRODUCT_EDIT_FLAGS_READ_ONLY)
	       == DO_PRODUCT_EDIT_FLAGS_READ_ONLY)
        do_tree_view_grab_focus(DO_TREE_VIEW(priv->views[STOCK_VIEW]));

	return object;
}

static void do_product_edit_finalize (GObject *object)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (object);
	if (priv->context)
        g_object_unref(priv->context);
	if (priv->alias_name)
		g_free(priv->alias_name);
    if ( priv->unit )
        g_free(priv->unit);
	G_OBJECT_CLASS (do_product_edit_parent_class)->finalize (object);
}
static void do_product_edit_activate_tab(DoProductEdit *dialog, const gchar *tab)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);

	if (!tab || !strcmp(tab, "Stock")) {
         gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), STOCK_VIEW);
         do_tree_view_grab_focus(DO_TREE_VIEW(priv->views[STOCK_VIEW]));
	}
    else
	if (!strcmp(tab, "Parcel")) {
         gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), PARCEL_VIEW);
         do_tree_view_grab_focus(DO_TREE_VIEW(priv->views[PARCEL_VIEW]));
	}
#ifndef MIN_SCREEN
    else
	if (!strcmp(tab, "Prihod")) {
         gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), PRIHOD_VIEW);
         do_tree_view_grab_focus(DO_TREE_VIEW(priv->views[PRIHOD_VIEW]));
	}
    else
	if (!strcmp(tab, "Order")) {
         gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), ORDER_VIEW);
         do_tree_view_grab_focus(DO_TREE_VIEW(priv->views[ORDER_VIEW]));
	}
    else
	if (!strcmp(tab, "Check")) {
         gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), CHECK_VIEW);
         do_tree_view_grab_focus(DO_TREE_VIEW(priv->views[CHECK_VIEW]));
	}
    else
	if (!strcmp(tab, "Rz")) {
         gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), RZ1_VIEW);
         do_tree_view_grab_focus(DO_TREE_VIEW(priv->views[RZ1_VIEW]));
	}
    else
	if (!strcmp(tab, "False")) {
         gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), FALSE_VIEW);
         do_tree_view_grab_focus(DO_TREE_VIEW(priv->views[FALSE_VIEW]));
	}
#endif
}

static void do_product_edit_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_FLAGS:
			priv->flags = g_value_get_flags (value);
			break;
        case PROP_ACTION:
            priv->action = g_value_get_enum (value);
            break;
        case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string(value);
            break;
        case PROP_VIEW:
            priv->view = g_value_get_pointer (value);
            break;
        case PROP_UNIT:
            priv->unit = g_value_dup_string(value);
            break;
        case PROP_TAB:
            do_product_edit_activate_tab((DoProductEdit*)object , g_value_get_string(value));
            break;
        case PROP_CONTEXT:
            if (!priv->context)
                priv->context = do_context_copy(g_value_get_object(value));
            break;
        case PROP_VIEW_OBJECT:
            priv->parent_view = g_value_get_pointer(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_product_edit_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_ACTION:
            g_value_set_enum (value, priv->action);
            break;
        case PROP_VIEW:
            g_value_set_pointer (value, priv->view);
            break;
        case PROP_UNIT:
            g_value_set_string (value, priv->unit);
            break;
        case PROP_ALIAS:
            g_value_set_string(value, priv->alias_name);
        case PROP_CONTEXT:
            g_value_set_object (value, do_context_copy(priv->context));
            break;
		case PROP_FLAGS:
			g_value_set_flags (value, priv->flags);
			break;
		case PROP_VIEW_OBJECT:
			g_value_set_object (value, priv->parent_view);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;

    }
}
static void do_product_edit_class_init (DoProductEditClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_product_edit_constructor;
	object_class->finalize = do_product_edit_finalize;
	object_class->get_property = do_product_edit_get_property;
	object_class->set_property = do_product_edit_set_property;

	g_type_class_add_private (object_class, sizeof (DoProductEditPrivate));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_PRODUCT_EDIT_FLAGS,
				     DO_PRODUCT_EDIT_FLAGS_DEFAULT,
				     G_PARAM_WRITABLE | G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (object_class,
				   PROP_ACTION,
				   g_param_spec_enum ("action",
 						      "Действие редактора",
 						      "Действие редактира с документом клавиши",
 						      DO_TYPE_DIALOG_ACTION,
 						      DO_DIALOG_ACTION_NONE,
 						      G_PARAM_READWRITE));
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
         PROP_TAB,
         g_param_spec_string("tab",
                      "Открыть вкладку",
                      "Открыть вкладку в дополнительных параметрах",
                      NULL,
                       G_PARAM_WRITABLE));

    g_object_class_install_property (object_class,
				   PROP_CONTEXT,
				   g_param_spec_object ("context",
 						      "Контекст документа",
 						      "Контекст документа описания клавиши",
 						      DO_TYPE_CONTEXT,
 						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (object_class,
				   PROP_UNIT,
				   g_param_spec_string ("unit",
 						      "Подразделение",
 						      "Подразделение",
 						      NULL,
 						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (object_class,
				   PROP_VIEW,
				   g_param_spec_pointer ("view",
							"Обьект просмотра",
							"Обьект просмотра",
							G_PARAM_READWRITE));
    g_object_class_install_property
        (object_class,
         PROP_VIEW_OBJECT,
         g_param_spec_pointer("view-object",
                      "view-object",
                      "view-object",
                      //DO_TYPE_VIEW ,
                      G_PARAM_READWRITE));
}

GtkWidget *do_product_edit_new (DoView *parent, const gchar *alias_name, DoProductEditFlags flags, DoDialogAction action, DoContext *context, const gchar *unit)
{
	return g_object_new (DO_TYPE_PRODUCT_EDIT,
			     "flags", flags,
			     //!!"view-object", parent,
			     "action", action,
			     "context", context,
			     "alias-name", alias_name,
			     "unit", unit,
			     (gpointer) NULL);

}
/*
static GtkWidget *add_field(DoProductEdit *dialog, gint id, GtkWidget *widget)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
	g_assert (id < N_ENTRIES);
	priv->widget[id] = widget;
	if (GTK_IS_ENTRY(widget)) {
        g_signal_connect(widget, "activate", G_CALLBACK(entry_activate), dialog);
        priv->entry[id] = widget;
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
	if ((priv->flags & DO_PRODUCT_EDIT_FLAGS_READ_ONLY) == DO_PRODUCT_EDIT_FLAGS_READ_ONLY &&
         priv->entry[id])
         g_object_set(priv->entry[id], "editable", FALSE, NULL);

    return widget;
}
static void entry_activate(GtkWidget *entry, DoProductEdit *dialog)
{
    gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
}
*/
static gchar *get_pack(do_alias_t *alias, product_rec_t *product)
{
    gdouble coef;
    gchar *unit, *tech_unit;
    gchar *retval;
    coef = do_product_coef(alias, product);
    unit = do_text(alias, product->data.unit);
    if (coef <= 1)
        retval = unit;
    else {
        gint coef_int;
        tech_unit = do_text(alias, product->data.tech_unit);
        retval = g_malloc(strlen(unit) + strlen(tech_unit) + 50);
        coef_int = coef;
        sprintf(retval, "1*%d %s/%s", coef_int, tech_unit, unit);
        g_free(unit);
        g_free(tech_unit);
    }
    return retval;
}
static int domino_order_get_marked_quant_m(do_alias_t *alias, const char *unit, const char *product_code,  struct tm *tm1, struct tm *tm2, domino_marked_t *marked, struct tm *time_mark, double *quant);

static gchar *get_order(DoProductEdit *dialog, const gchar *product_code)
{

    DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);

    struct tm tm1;
    struct tm tm2;
    struct tm tm_mark;
    time_t now;
    gchar *retval;
    domino_marked_t marked;
    gchar time_str[200];


    now = time(NULL);
    tm1 = *localtime(&now);
    tm2 = tm1;
    tm1.tm_mday-= DOMINO_MARKED_INTERVAL;
    mktime(&tm1);
    //if ( !domino_order_get_marked_(priv->alias_name, priv->unit, product_code, &tm1, &tm2, &marked, &tm_mark) )
        //return NULL;
    double quant;
    do_alias_t *alias;
    alias = domino_alias_new(priv->alias_name);
    if ( !alias || !do_alias_open(alias,TRUE) )
        return NULL;
    int res;

    res = domino_order_get_marked_quant_m(alias, priv->unit, product_code, &tm1, &tm2, &marked, &tm_mark, &quant);
    do_alias_free(alias);
    if ( !res ) {
        return NULL;
    }


//    if ( do_date_cmp(&tm2,&tm_mark) && domino_order_after_traffic_in_(priv->alias_name, priv->unit, product_code, &tm_mark) )
  //      marked = DOMINO_MARKED_UNMARK;

    strftime(time_str,sizeof(time_str)-1, "%d/%m %H:%M", &tm_mark);

    switch ( marked ) {
        case DOMINO_MARKED_UNMARK:
            return FALSE;
        case DOMINO_MARKED:
            retval = g_strdup_printf("Заявка %s", time_str);
            break;
        case DOMINO_MARKED_AND_ACCEPT:
            retval = g_strdup_printf("<span foreground=\"green\">Заказ</span> %s", time_str);
            break;
        case DOMINO_MARKED_AND_REJECT:
            retval = g_strdup_printf("<span foreground=\"red\">Отказ</span> %s", time_str);
            break;
    }
    return retval;
}
static gchar *get_inn(do_alias_t *alias, const gchar *product_code)
{


    product_view_key1_t product_view_key1, key;
    product_view_rec_t product_view;
    int status;
    char *retval = NULL;
    do_text_set_empty(product_view_key1.sklad);
    do_text_set(alias, product_view_key1.code, product_code);
    do_cpy(key.code, product_view_key1.code);
    status = do_product_view_get1(alias, &product_view, &product_view_key1, DO_GET_GE);
    while (status == DO_OK) {
        char *str_,*buf;
        if ( do_cmp(key.code, product_view_key1.code) ) break;
        if ( product_view.data.type == 2 ) {
            str_= do_text(alias, product_view.data.sort);
            if ( retval ) {
                buf = do_strdup_printf("%s+%s",retval,str_);
                do_free(retval);do_free(str_);
                retval = buf;
            }
            else
                retval = str_;
        }
        status = do_product_view_get1(alias, &product_view, &product_view_key1, DO_GET_NEXT);
    }
    return retval;
}
static gchar *get_last_in(do_alias_t *alias, const gchar *product_code, const char *unit)
{

	product_key0_t product_key0;
	product_rec_t product;
	prowod_key2_t prowod_key2, prowod_key;
	prowod_rec_t prowod;
	int status, i;
	gchar  *value,*retval = NULL;
	double coef;

	do_text_set(alias, product_key0.code, product_code);
	product_key0.base_parcel = 0;

	if (do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK) {
	    do_alias_free(alias);
        return FALSE;
	}
    coef = do_product_coef(alias, &product);

	do_text_set(alias, prowod_key2.division, domino_division());
    do_text_set(alias, prowod_key2.debit_account, do_param(DO_PARAM_ACCOUNT_PRODUCT_IN));
    do_text_set_empty(prowod_key2.debit_subaccount);
    do_text_set_empty(prowod_key2.debit_p_g_code);
    prowod_key2.debit_p_code = 0;
    value = g_strdup_printf("%s.%s", unit, product_code);
    do_text_set(alias, prowod_key2.debit_other, value);
    g_free(value);
    prowod_key2.date = INT_MAX;
    memcpy(&prowod_key, &prowod_key2, sizeof(prowod_key2) - sizeof(prowod_key2.date));

    status = do_prowod_get2(alias, &prowod, &prowod_key2, DO_GET_LT);

    for (i = 0; i < 1 && (status == DO_OK); i++) {
        if (memcmp(&prowod_key, &prowod_key2, sizeof(prowod_key2) - sizeof(prowod_key2.date)))
            break;

        gchar *date;
        gchar *quantity;
        gchar *price;
        gchar *supplier;
        gchar *unit;
        struct tm tm;

        do_date(prowod.data.date, &tm);
        date = do_datetoa(tm);

        quantity = do_rest_format(do_prowod_sum(alias, &prowod, DO_CONST_PROWOD_SUM_QUANT)/coef);
        price    = do_money_format(do_prowod_sum(alias, &prowod, DO_CONST_PROWOD_SUM_PRICE)*coef, TRUE);
        supplier = do_prowod_param(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_COMMENT));
        unit = do_text(alias, product.data.tech_unit);

        retval = do_strdup_printf("<b>%s %s %s</b> %s", date, quantity,unit, supplier);

        g_free(date);
        g_free(quantity);
        g_free(price);
        g_free(supplier);
        g_free(unit);

        status = do_prowod_get2(alias, &prowod, &prowod_key2, DO_GET_PREVIOUS);
	}

	return retval;
}
static gchar *get_group(do_alias_t *alias, product_rec_t *product)
{
    gchar *name[3] = {NULL, NULL, NULL};
    gchar *code[3];
    int status, i;
    code[0] = do_text(alias, product->data.class_id);
    if (code[0][0] != '\0') {
        class_key0_t class_key0;
        class_rec_t _class;
        memcpy(&class_key0.class_id, &product->data.class_id, sizeof(class_key0.class_id));
        status = do_class_get0(alias, &_class, &class_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            name[0] = do_text(alias, _class.data.name);
        else
            name[0] = g_strdup(code[0]);
    }
    g_free(code[0]);
    code[1] = do_text(alias, product->data.group_id);
    if (code[1][0] != '\0') {
        group_key0_t group_key0;
        group_rec_t  group;
        memcpy(&group_key0.class_id, &product->data.class_id, sizeof(group_key0.class_id));
        memcpy(&group_key0.group_id, &product->data.group_id, sizeof(group_key0.group_id));
        status = do_group_get0(alias, &group, &group_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            name[1] = do_text(alias, group.data.name);
        else
            name[1] = g_strdup(code[1]);
    }
    g_free(code[1]);
    code[2] = do_text(alias, product->data.subgroup_id);
    if (code[2][0] != '\0') {
        subgroup_key0_t subgroup_key0;
        subgroup_rec_t  subgroup;
        memcpy(&subgroup_key0.class_id, &product->data.class_id, sizeof(subgroup_key0.class_id));
        memcpy(&subgroup_key0.group_id, &product->data.group_id, sizeof(subgroup_key0.group_id));
        memcpy(&subgroup_key0.subgroup_id, &product->data.subgroup_id, sizeof(subgroup_key0.subgroup_id));
        status = do_subgroup_get0(alias, &subgroup, &subgroup_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            name[2] = do_text(alias, subgroup.data.name);
        else
            name[2] = g_strdup(code[2]);
    }
    g_free(code[2]);

    int len = 0;
    for (i = 0; i < 3; i++)
        if (name[i])
            len += strlen(name[i]) + 2;

    gchar *retval;
    retval = g_malloc0(len);
    for (i = 0; i < 3; i++)
        if (name[i]) {
            if (retval[0] != '\0') {
                char *buf;
                buf = g_strdup_printf("%s::%s", retval, name[i]);
                strcpy(retval, buf);
                g_free(buf);
            }
            else
                sprintf(retval, "%s", name[i]);
            g_free(name[i]);
        }
    char *ch;
    ch = g_markup_printf_escaped("%s", retval);
    g_free(retval);
    return ch;
}

typedef struct {
    gint code_int;
    gchar *code;
    gchar *name1;
    sklad_rec_t *sklad;
    gchar *phone;
}DoSkladRecord;


static int
cb_value_compare (DoSkladRecord const *a, DoSkladRecord const *b)
{
    return a->code_int - b->code_int;
}
static void
rec_free (DoSkladRecord *a)
{
    g_free(a->code);
    g_free(a->name1);
    g_free(a->phone);
    g_free(a->sklad);
    g_free(a);
}

static gchar *MonthShort[12] =
{"Янв","Фев","Мар","Апр","Май","Июн","Июл","Авг","Сен","Окт","Ноя","Дек"
};
typedef struct {
    double rest;
    double sale;
    double price;
    struct tm life_time;
    char *serial;
} do_rest_price_life_serial_t;

static int get_rest(do_alias_t *alias, const char *base_code, sklad_rec_t *sklad, do_list_t *list)
{
    stock_key0_t stock_key0, stock_key;
    stock_rec_t  stock;
    barcode_key1_t barcode_key1;
    product_key0_t product_key0;
    product_rec_t product;
    int status = DO_OK;
    double rest = 0, sale = 0;
    do_rest_price_life_serial_t rec = {0, 0, 0,};
    do_rest_price_life_serial_t *rec_p, *base_rec = NULL;
    char *sklad_code;
    int local_sklad;
    double coef;
    int parcel;
    int retail_n = do_sklad_param_int(alias, sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PRICE_RETAIL));

    sklad_code = do_text(alias, sklad->data.code);
    local_sklad = !strcmp(sklad_code, domino_unit());
    do_free(sklad_code);

    do_text_set(alias, stock_key0.code, base_code);
    memcpy(&stock_key0.sklad, &sklad->data.code, sizeof(stock_key0.sklad));
    memcpy(&stock_key, &stock_key0, sizeof(stock_key));

    memcpy(&product_key0.code, &stock_key0.code, sizeof(stock_key0.code));
    product_key0.base_parcel = 0;
    status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
    int life = do_product_param_int(alias, &product, "ЖВ") == 1;
    if (status == DO_OK) {
        coef = do_product_coef(alias, &product);
        if ( local_sklad )
            rec.price = coef*do_product_price(alias, &product, retail_n);
    }
    if (status == DO_OK) {
        if ( local_sklad )
            status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_GE);
        else {
            status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
            rec.price = do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL);
        }
    }
    if (status == DO_OK) {
        //flag = do_product_param_int(alias, &product,do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) != 1;
        //if ( flag )
        base_rec = do_list_add_alloc(list, &rec, sizeof(rec));
    }
    while (status == DO_OK) {
        if (memcmp(&stock_key0.sklad, &stock_key.sklad, sizeof(stock_key.sklad))) break;

        if (memcmp(&stock_key0.code, &stock_key.code, sizeof(stock_key.code)) &&
            !do_product_is_base_owner(stock_key.code, stock_key0.code, sizeof(stock_key.code)))
            break;

        rest = do_stock_quant(alias, &stock, DO_CONST_QUANT_REST) +
                do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE);
        sale = do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE);
        parcel = 0;
        if (do_stock_cmp(rest,0) &&
            do_product_is_parcel(stock.data.code, sizeof(stock.data.code))) {
            rec.price = 0;
            rec.rest = rest/coef;
            rec.sale = sale/coef;
            rec.life_time.tm_mday = 0;
            rec.serial = NULL;
            if (local_sklad) {
                struct tm tm;
                do_cpy(product_key0.code, stock.data.code);
                product_key0.base_parcel = 1;
                do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
                if (do_product_life_time(alias, &product, &tm))
                    rec.life_time = tm;
                rec.serial = do_product_param(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_SERIA));

                memcpy(&barcode_key1.code, &stock.data.code, sizeof(stock.data.code));
                status = do_barcode_key1(alias, &barcode_key1, DO_GET_EQUAL);
                if ( status == DO_OK ) {
                    parcel = 1;
                    rec.price = coef*do_product_price(alias, &product, retail_n);
                }
            }
            else {
                if (do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL) > 0.00000001) {
                    rec.price = do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL);
                    parcel = (rec.price > 0.0009);
                }
            }
            if (rec.price) {
                /*new_rec = 1;
                for (i = 0; i < list->count; i++) {
                    rec_p = list->list[i];
                    if (rec_p->price == rec.price) {
                        if (base_rec != rec_p || parcel) {
                            rec_p->rest += rec.rest;
                            rec_p->sale += rec.sale;
                        }
                        rec_p->life_time = rec.life_time;
                        new_rec = 0;
                        break;
                    }
                }
                if (new_rec)*/
                do_list_add_alloc(list, &rec, sizeof(rec));
            }
            else
                if (rec.life_time.tm_mday && list->count) {
                    rec_p = list->list[0];
                    rec_p->life_time = rec.life_time;
                }
        }
        if (status == DO_ERROR)
            break;
        if ( !parcel ) {
            base_rec->rest += rest/coef;
            base_rec->sale += sale/coef;
        }
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_NEXT);
    }
    if ( life && base_rec ) {
        if ((base_rec->rest == 0) && (base_rec->sale == 0)) {
            int i;
            for ( i = 0; i < list->count ; i++ ) {
                if ( list->list[i] == base_rec ) {
                    do_list_delete(list, i);
                    break;
                }
            }
        }
    }
    return status != DO_ERROR;
}


static gboolean do_product_edit_get_stock(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
	//int status;
	sklad_rec_t sklad;
	sklad_key0_t sklad_key0;
	GList *list = NULL, *l;
	DoSkladRecord *rec;
	//gchar *type;
	gchar *rest;
	gchar *sale;
	gchar *total_buf;
	gchar *price;
	gchar *life_time;
	gchar *serial;
	//gint kind;
	do_alias_t *alias;
	gchar  *code;
	gdouble total_rest = 0;
	gchar *mode;
	int stock_mode = 0;
	int firm;


    gtk_tree_store_clear( priv->stores[STOCK_VIEW] );

    DOMINO_LOCAL_GET("ProductEditStock", "stock-mode", &mode, NULL);
    if ( mode )
        stock_mode = atoi(mode);


    alias = domino_alias_new(priv->alias_name);
    if ( !alias )
        return FALSE;
    if ( !do_alias_open(alias, TRUE) ) {
        do_alias_free(alias);
        return FALSE;
    }

    g_object_get(priv->context, "product-code", &code, NULL);
    g_return_val_if_fail ( code != NULL, FALSE );

	do_text_set(alias, sklad_key0.code, priv->unit);
	if ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) != DO_OK )
	    return FALSE;
	do_string_list_t *units;
	int i;

	switch ( stock_mode ) {
        case 0:
            units = do_city_get_units(alias, do_city_get_city_by_unit(alias, priv->unit));
            break;
        case 1:
            firm = do_firm_get_firm_by_unit(alias, domino_unit());
            units = do_firm_get_units(alias, firm);
            break;
        default:
            units = do_get_units(alias);
    }
    for ( i = 0; i < units->count; i++ ) {

        do_text_set(alias, sklad_key0.code, units->list[i]);
        switch ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) ) {
            case DO_KEY_NOT_FOUND:
                do_log(LOG_ERR, "unit \"%s\" not found", units->list[i]);
                return FALSE;
            case DO_OK:
                break;
            default:
                return FALSE;
        }
        rec = g_malloc(sizeof(DoSkladRecord));

        rec->phone = g_strdup("");
        rec->code = do_text(alias, sklad.data.code);
        rec->name1 = do_text(alias, sklad.data.name);
        rec->sklad = g_malloc(sizeof(sklad_rec_t));
        memcpy(rec->sklad, &sklad, sizeof(sklad_rec_t));
        if (!strcmp(rec->code, priv->unit)) {
            rec->code_int = 0;
            priv->retail_price_n = do_sklad_param_int(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PRICE_RETAIL));
        }
        else
            rec->code_int = atoi(sklad.data.code);

        list = g_list_insert_sorted(list, rec,  (GCompareFunc)cb_value_compare);
	}
    partner_rec_t partner;
    partner_key0_t partner_key0, key;
    int status;
    do_text_set(alias, partner_key0.g_code, "01");
    partner_key0.code = 0;
    do_cpy(key.g_code, partner_key0.g_code);
    product_rec_t product;
    product_key4_t product_key4;
    do_text_set(alias, product_key4.code, code);
    do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL);
    double coef=do_product_coef(alias, &product);
    char *ei = do_text(alias, product.data.unit);

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.g_code, partner_key0.g_code) ) break;
        char *code;
        code = do_text(alias, partner.data.sklad);
        for (l = list; l; l = l->next) {
            rec = l->data;
            if ( !strcmp(code, rec->code) ) {
                char *name;
                do_free(rec->phone);
                rec->phone = do_partner_param(alias, &partner, "PHONE");
                name = do_partner_param(alias, &partner, "SNAME");
                if ( name[0] != '\0' ) {
                    do_free(rec->name1);
                    rec->name1 = name;
                }
                else
                    do_free(name);
                break;
            }
        }
        do_free(code);

        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_NEXT);
    }


	do_string_list_free(units);
#ifdef MIN_SCREEN
	gboolean flag = TRUE;
#endif
	for (l = list; l; l = l->next) {
	    rec = l->data;
        GtkTreeIter iter;
        do_list_t *list;

        list = do_list_new(1);

        if (get_rest(alias, code, rec->sklad, list)) {
            int i;
            char *store;
            store = do_text(alias,rec->sklad->data.code);
    	    int local_=!strcmp(store,(char*)domino_unit());
    	    do_free(store);
            GString *mprice = g_string_new(NULL);
            GString *mrest = g_string_new(NULL);
            GString *msale = g_string_new(NULL);
            GString *mlife_time = g_string_new(NULL);
            GString *mserial = g_string_new(NULL);
            for (i = 0; i < list->count; i++) {
                do_rest_price_life_serial_t *rec = list->list[i];
                char *sale_;
                if (i > 0) {
#ifdef MIN_SCREEN
                    if (coef > 1.01 && flag)
                        g_string_append_printf(mprice, "\n%s / %s", do_money_format(rec->price, TRUE),do_money_format(rec->price/coef, TRUE));
                    else
#endif
                        g_string_append_printf(mprice, "\n%s", do_money_format(rec->price, TRUE));
                    if (local_)
                    sale_ =  do_rest_format(rec->sale);
                    else
                    sale_=do_strdup("");

                    g_string_append_printf(mrest, "\n%s%s%s", do_rest_format_ei(rec->rest, coef, ei), sale_[0] != '\0' ? " / " : "", sale_[0] != '\0' ? sale_ : "");
                    do_free(sale_);

                    g_string_append_printf(msale, "\n%s", do_rest_format(-rec->sale));
                    if (rec->life_time.tm_mday)
                        g_string_append_printf(mlife_time, "\n%s %d", MonthShort[rec->life_time.tm_mon], rec->life_time.tm_year - 100);
                    else
                        g_string_append_printf(mlife_time, "\n");
                    if ( rec->serial ) {
                        g_string_append_printf(mserial, "\n%s", rec->serial);
                        do_free(rec->serial);
                    }
                    else
                        g_string_append_printf(mserial, "\n");


                }
                else {
#ifdef MIN_SCREEN
                    if (coef > 1.01 && flag)
                        g_string_append_printf(mprice, "%s / %s", do_money_format(rec->price, TRUE),do_money_format(rec->price/coef, TRUE));
                    else
#endif
                        g_string_append_printf(mprice, "%s", do_money_format(rec->price, TRUE));
                    //g_string_append_printf(mrest, "%s", do_rest_format(rec->rest));
                    if (local_)
                    sale_ =  do_rest_format(rec->sale);
                    else
                    sale_=do_strdup("");

                    g_string_append_printf(mrest, "%s%s%s", do_rest_format_ei(rec->rest,coef,ei), sale_[0] != '\0' ? " / " : "", sale_[0] != '\0' ? sale_ : "");
                    do_free(sale_);
                    g_string_append_printf(msale, "%s", do_rest_format(-rec->sale));
                    if (rec->life_time.tm_mday)
                        g_string_append_printf(mlife_time, "%s %d", MonthShort[rec->life_time.tm_mon], rec->life_time.tm_year - 100);

                    if ( rec->serial ) {
                        g_string_append_printf(mserial, "%s", rec->serial);
                        do_free(rec->serial);
                    }
                }
                total_rest += rec->rest;
            }
            price = g_string_free(mprice, FALSE);
            rest = g_string_free(mrest, FALSE);
            sale = g_string_free(msale, FALSE);
            life_time = g_string_free(mlife_time, FALSE);
            serial = g_string_free(mserial, FALSE);


        }
        else {
            rest = g_malloc0(1);
            price = g_malloc0(1);
            sale = g_malloc0(1);
            life_time = g_malloc0(1);
        }
#ifdef MIN_SCREEN
        flag = FALSE;
#endif
        if ( rest[0] ) {
            gtk_tree_store_append (priv->stores[STOCK_VIEW], &iter, NULL);
            gtk_tree_store_set (priv->stores[STOCK_VIEW], &iter,
                            0, rec->name1,
                            1, price,
                            2, rest,
                            3, sale,
                            4, life_time,
                            5, rec->phone,
                            6, serial,
                            -1);
        };
        g_free(price);
        g_free(rest);
        g_free(sale);
        g_free(serial);
        do_list_free(list);

	}
	g_list_foreach(list, (GFunc)rec_free, NULL );
	g_list_free(list);
	do_alias_free(alias);
	do_free(ei);

	rest = do_rest_format(total_rest);

	total_buf = g_strdup_printf("Общий остаток: <b>%s</b>", rest);
    gtk_label_set_markup(GTK_LABEL(priv->stock_totally), total_buf);
    gtk_label_set_justify(GTK_LABEL(priv->stock_totally), GTK_JUSTIFY_LEFT);
    g_object_set(priv->stock_totally, "xalign", 0.01, NULL);
	g_free(total_buf);
	g_free(rest);


	return TRUE;


}
#ifdef ADS_RECOMMEND
static gchar* get_ads(do_alias_t *alias, const char *code)
{
    product_rec_t product;
    product_key0_t product_key0;
    /*document_rec_t document;
    document_key0_t document_key0;*/
    //do_string_list_t *keys;
    char *value, *res;
    //int i;

    product_key0.base_parcel = 0;
    do_text_set(alias, product_key0.code, code);

    if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK )
        return NULL;
    value = do_product_param(alias, &product, "MR");
    if ( !value[0] ) {
        do_free(value);
        return NULL;
    }
    return value;
    /*keys = do_string_list_new_from_csv(value);
    do_free(value);
    res = NULL;
    for ( i = 0; i < keys->count; i++ ) {
        char *buf,*ch;
        value = keys->list[i];
        if ( value[0] == '!' )
            value++;
        ch = strchr(value, '.');
        if ( !ch ) continue;
        ch[0] = '\0';
        do_text_set(alias, document_key0.dtype, value);
        value = ch+1;
        ch = strchr(value, '.');
        if ( !ch ) continue;
        ch[0] = '\0';
        do_text_set(alias, document_key0.sklad, value);
        value = ch+1;
        do_text_set(alias, document_key0.document, value);
        if ( do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL) == DO_OK ) {
            if ( res ) {
                buf = res;
                value = do_document_param(alias, &document, "37");
                res = g_strdup_printf("%s %s", buf, value);
                g_free(buf);
                g_free(value);
            }
            else
                res = do_document_param(alias, &document, "37");

        }
    }
    do_string_list_free(keys);*/
    return res;
}
#endif
static gboolean fill_entry(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    if (do_context_get_kind(priv->context) != DO_CONTEXT_KIND_PRODUCT)
        return FALSE;

    do_alias_t *alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, TRUE)) {
        do_alias_free(alias);
        return FALSE;
    }

    gchar *code;
    g_object_get(priv->context, "product-code", &code, NULL);
    g_return_val_if_fail ( code != NULL, FALSE );

    gboolean retval = TRUE;
    product_key4_t product_key4;
    //product_rec_t product;
    int status;

    do_text_set(alias, product_key4.code, code);
    status = do_product_get4(alias, &priv->product, &product_key4, DO_GET_EQUAL);
    if (retval && status == DO_OK) {
        GString *val_string;
        val_string = g_string_new("<b>");
        switch (do_product_param_int(alias, &priv->product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS))) {
            case 1:
                if (do_rz_info_exist_by_code(alias, code))
                    g_string_append(val_string, "(<span foreground=\"green\">ЖНВЛС</span>) ");
                else
                    g_string_append(val_string, "(<span foreground=\"red\">ЖНВЛС</span>) ");
                break;
            case 3:
                g_string_append(val_string, "(СМН) ");
                break;
        };
        char *product_name, *str_;
        product_name=do_text(alias, priv->product.data.name);
        g_string_append(val_string, g_markup_printf_escaped("%s ", code));
        g_string_append(val_string, g_markup_printf_escaped("%s", product_name));
        g_free(product_name);
        g_string_append(val_string, "</b>");
        /*g_string_append(val_string, "\n");
        g_string_append(val_string, do_text(alias, product.data.name));*/
        g_string_append(val_string, " ");
        g_string_append(val_string, get_pack(alias, &priv->product));
        g_string_append(val_string, "\n<b>");
        g_string_append(val_string, get_group(alias, &priv->product));
        g_string_append(val_string, "</b>\n");
        product_name=do_get_partner_name_no_conv(alias, priv->product.data.mfr_region, priv->product.data.mfr_code);
        g_string_append(val_string, g_markup_printf_escaped("%s", product_name));
        str_ = get_inn(alias, code);
        if ( str_ ) {
            g_string_append(val_string, "\nМНН: ");
            g_string_append(val_string, "<b>");
            g_string_append(val_string, g_markup_printf_escaped("%s", str_));
            g_string_append(val_string, "</b>");
            g_free(str_);
        }
        str_ = get_last_in(alias, code, priv->unit);
        if ( str_ ) {
            g_string_append(val_string, "\nПриход: ");
            g_string_append(val_string, str_);
            g_free(str_);
        }
        str_ = get_order(dialog, code);
        if ( str_ ) {
            g_string_append(val_string, "\n");
            g_string_append(val_string, str_);
            g_free(str_);
        }
#ifdef ADS_RECOMMEND
        str_ = get_ads(alias, code);
        if ( str_ ) {
            g_string_append(val_string, "\n<b>");
            g_string_append(val_string, str_);
            g_string_append(val_string, "</b>");
            g_free(str_);
        }
#endif
        document_key0_t document_key0;
        document_order_key2_t document_order_key2, key;
        document_rec_t document;
        partner_key0_t partner_key0;
        partner_rec_t partner;
        int status;

        do_text_set(alias, document_key0.dtype, "LT");
        do_text_set(alias, document_key0.sklad, "ТБ");
        do_text_set_empty(document_key0.document);
        do_cpy(key.dtype, document_key0.dtype);
        do_cpy(key.sklad, document_key0.sklad);

        status = do_document_key0(alias, &document_key0, DO_GET_GE);
        while ( status == DO_OK ) {
            if ( do_cmp(key.dtype, document_key0.dtype) ||
                 do_cmp(key.sklad, document_key0.sklad) ) break;
            do_cpy(document_order_key2.dtype, document_key0.dtype);
            do_cpy(document_order_key2.sklad, document_key0.sklad);
            do_cpy(document_order_key2.document, document_key0.document);
            do_text_set(alias, document_order_key2.code, code);
            if ( do_document_order_key2(alias, &document_order_key2, DO_GET_EQUAL) == DO_OK ) {
                if ( do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL) == DO_OK ) {
                    char *val1, *val2;
                    val1 = do_document_param(alias, &document, "452");
                    if ( strcmp(val1, "1") ) {
                        g_free(val1);

                        do_cpy(partner_key0.g_code, document.data.p_g_code);
                        partner_key0.code = document.data.p_code;
                        do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
                        val1 = do_text(alias, partner.data.name);
                        val2 = do_document_param(alias, &document, "37");

                        g_string_append_printf(val_string, "\n<b>%s</b> %s", val1, val2);
                        g_free(val1);
                        g_free(val2);
                    }
                    else
                        g_free(val1);
                }
            }
            status = do_document_key0(alias, &document_key0, DO_GET_NEXT);
        }
        gtk_label_set_markup(GTK_LABEL(priv->title_label), g_string_free(val_string, FALSE));
        /*
        val_string = do_text(alias, product.data.name);
        gtk_entry_set_text(GTK_ENTRY(priv->widget[DESC_ENTRY]), val_string);
        do_free(val_string);

        val_string = do_text(alias, product.data.name);
        gtk_entry_set_text(GTK_ENTRY(priv->widget[NAME_ENTRY]), val_string);
        do_free(val_string);
        val_string = do_text(alias, product.data.code);
        gtk_entry_set_text(GTK_ENTRY(priv->widget[CODE_ENTRY]), val_string);
        do_free(val_string);
        val_string = get_pack(alias, &product);
        gtk_entry_set_text(GTK_ENTRY(priv->widget[UNIT_ENTRY]), val_string);
        do_free(val_string);
        val_string = get_group(alias, &product);
        gtk_entry_set_text(GTK_ENTRY(priv->widget[GROUP_ENTRY]), val_string);
        do_free(val_string);
        val_string = do_get_partner_name_no_conv(alias, product.data.mfr_region, product.data.mfr_code);
        gtk_entry_set_text(GTK_ENTRY(priv->widget[MFR_ENTRY]), val_string);
        do_free(val_string);
*/
    }
    do_alias_free(alias);
    return retval;
}

gint do_product_edit_run(DoProductEdit *dialog)
{
    gint response = 0;
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    while (!response) {
        response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_ACCEPT &&
            priv->action != DO_DIALOG_ACTION_NONE) {
            //if (!product_document_save(dialog))
                //response = 0;
        }
    }
    return response;
}
static gboolean do_product_edit_create_store_stock(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    priv->stores[STOCK_VIEW] = gtk_tree_store_new(7,
                                                    G_TYPE_STRING,
                                                    G_TYPE_STRING,
                                                    G_TYPE_STRING,
                                                    G_TYPE_STRING,
                                                    G_TYPE_STRING,
                                                    G_TYPE_STRING,
                                                    G_TYPE_STRING
                                                    );

    g_object_set(priv->views[STOCK_VIEW], "model", priv->stores[STOCK_VIEW], NULL);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[STOCK_VIEW]), "Unit", "Аптека", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[STOCK_VIEW]), "Price", "Цена", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[STOCK_VIEW]), "Stock", "Ост/Пр", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 2);
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);


    /*r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[STOCK_VIEW]), "Sale", "Продажи", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 3);*/

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[STOCK_VIEW]), "LifeTime", "Срок", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 4);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[STOCK_VIEW]), "Serial", "Серия", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 6);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[STOCK_VIEW]), "phone", "Телефон", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 5);

    return do_product_edit_get_stock(dialog);
}

static gboolean do_product_edit_create_store_parcel(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    priv->stores[PARCEL_VIEW] = gtk_tree_store_new(10, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                                       G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                                       G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                                       G_TYPE_STRING
                                                          );

    g_object_set(priv->views[PARCEL_VIEW], "model", priv->stores[PARCEL_VIEW], NULL);

    g_object_set_data(G_OBJECT(priv->views[PARCEL_VIEW]), "view-name", (gpointer)view_names[PARCEL_VIEW]);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PARCEL_VIEW]), "Code", "Код", -1);
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);
    gtk_tree_view_column_set_sort_column_id (col, 0);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PARCEL_VIEW]), "RetailPrice", "Розница", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 4);
    g_object_set(r, "xpad" , 5, NULL);


    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PARCEL_VIEW]), "Serial", "Серия", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 5);
    gtk_tree_view_column_set_sort_column_id (col, 5);
    g_object_set(r, "xpad" , 5, NULL);
    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PARCEL_VIEW]), "LifeTime", "Срок", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 6);
    g_object_set(r, "xpad" , 5, NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PARCEL_VIEW]), "Stocks", "Остаток", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 8);
    g_object_set(r, "xpad" , 5, NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PARCEL_VIEW]), "SupplierPrice", "Закуп", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 3);
    g_object_set(r, "xpad" , 5, NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PARCEL_VIEW]), "Supplier", "Поставщик", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 2);
    g_object_set(r, "xpad" , 5, NULL);


    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PARCEL_VIEW]), "Barcode", "Прод.код", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 7);
    gtk_tree_view_column_set_sort_column_id (col, 7);
    //g_object_set(r, "xalign", 0.5, NULL);
    g_object_set(r, "xpad" , 5, NULL);



    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PARCEL_VIEW]), "Date", "Дата", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    g_object_set(r, "xpad" , 5, NULL);

    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(    priv->stores[PARCEL_VIEW]), 0, GTK_SORT_DESCENDING);

    return do_product_edit_get_parcel(dialog);
}

static gboolean do_product_edit_get_parcel(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
	product_key0_t product_key0, product_key;
	product_rec_t product;
	barcode_key1_t barcode_key1;
	barcode_rec_t barcode;
	stock_key0_t stock_key0;
	stock_rec_t  stock;
	int status;
	do_alias_t *alias;
	gchar  *code;

    alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, TRUE)) {
        do_alias_free(alias);
        return FALSE;
    }

    g_object_get(priv->context, "product-code", &code, NULL);
    g_return_val_if_fail ( code != NULL, FALSE );

	do_text_set(alias, product_key0.code, code);

	product_key0.base_parcel = 1;

	memcpy(&product_key, &product_key0, sizeof(product_key));

	status = do_product_get0(alias, &product, &product_key0, DO_GET_GE);
	while (status == DO_OK) {
	    if (!product_key0.base_parcel) break;
	    if (!do_product_is_base_owner(product_key.code, product_key0.code, sizeof(product_key0.code)))
            break;

        gchar *code_par;
        gchar date_s[100];
        gchar date_l[100] = {0, };
        struct tm tm;
        gchar *partner;
        gchar *purchase_price;
        gchar *register_price;
        gchar *retail_price;
        gchar *seria;
        gchar *b_code;
        gchar *rest;
        gdouble coef = do_product_coef(alias, &product);

        GtkTreeIter iter;
        code_par = do_alias_text_(alias, (char*)product.data.code + do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH) + 1,
                                  sizeof(product.data.code) - do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH) - 1);
        do_date(product.data.date, &tm);
        sprintf(date_s, "%2.2d/%2.2d/%4.4d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
        partner = do_get_partner_name_no_conv(alias, product.data.supplier_region,
                                                     product.data.supplier_code);
        purchase_price = do_money_format(do_product_price(alias, &product, DO_CONST_PRICE_PURCHASE)*coef, TRUE);

        register_price = do_money_format(do_product_price(alias, &product, DO_CONST_PRICE_REGISTER)*coef, TRUE);

        retail_price = do_money_format(do_product_price(alias, &product, priv->retail_price_n)*coef, TRUE);
        seria = do_product_param(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_SERIA));
        if (do_product_life_time(alias, &product, &tm))
            sprintf(date_l, "%s %2.2d", MonthShort[tm.tm_mon], tm.tm_year - 100);

        memcpy(&barcode_key1.code, &product.data.code, sizeof(barcode_key1.code));
        if (do_barcode_get1(alias, &barcode, &barcode_key1, DO_GET_EQUAL) == DO_OK)
            b_code = do_text(alias, barcode.data.barcode);
        else
            b_code = g_malloc0(1);

        memcpy(&stock_key0.code, &product.data.code, sizeof(stock_key0.code));
        do_text_set(alias, stock_key0.sklad, priv->unit);
        if (do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL) == DO_OK )
            rest = do_rest_format((do_stock_quant(alias, &stock , DO_CONST_QUANT_REST) +
                                  do_stock_quant(alias, &stock , DO_CONST_QUANT_CRNTSALE)) / coef);
        else
            rest = g_malloc0(1);

        gtk_tree_store_append (priv->stores[PARCEL_VIEW], &iter, NULL);
        gtk_tree_store_set (priv->stores[PARCEL_VIEW], &iter,
                            0, code_par,
                            1, date_s,
                            2, partner,
                            3, purchase_price,
                            4, retail_price,
                            5, seria,
                            6, date_l,
                            7, b_code,
                            8, rest,
                            -1);
        g_free(code_par);
        g_free(partner);
        g_free(purchase_price);
        g_free(register_price);
        g_free(retail_price);
        g_free(seria);
        g_free(b_code);
        g_free(rest);

        status = do_product_get0(alias, &product, &product_key0, DO_GET_NEXT);
	}

	do_alias_free(alias);
	return status != DO_ERROR;
}


static gboolean do_product_edit_create_store_prihod(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    priv->stores[PRIHOD_VIEW] = gtk_tree_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                                       G_TYPE_STRING
                                                          );

    g_object_set(priv->views[PRIHOD_VIEW], "model", priv->stores[PRIHOD_VIEW], NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PRIHOD_VIEW]), "Supplier", "Дата", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PRIHOD_VIEW]), "Quantity", "Количество", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PRIHOD_VIEW]), "Price", "Цена", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 2);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[PRIHOD_VIEW]), "Supplier", "Поставщик", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 3);


    return do_product_edit_get_prihod(dialog);
}

static gboolean do_product_edit_get_prihod(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
	product_key0_t product_key0;
	product_rec_t product;
	prowod_key2_t prowod_key2, prowod_key;
	prowod_rec_t prowod;
	int status, i;
	do_alias_t *alias;
	gchar  *code;
	gchar  *value;
	double coef;

    alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, TRUE)) {
        do_alias_free(alias);
        return FALSE;
    }

    g_object_get(priv->context, "product-code", &code, NULL);
    g_return_val_if_fail ( code != NULL, FALSE );

	do_text_set(alias, product_key0.code, code);

	product_key0.base_parcel = 0;

	if (do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK) {
	    do_alias_free(alias);
        return FALSE;
	}
    coef = do_product_coef(alias, &product);

	do_text_set(alias, prowod_key2.division, domino_division());
    do_text_set(alias, prowod_key2.debit_account, do_param(DO_PARAM_ACCOUNT_PRODUCT_IN));
    do_text_set_empty(prowod_key2.debit_subaccount);
    do_text_set_empty(prowod_key2.debit_p_g_code);
    prowod_key2.debit_p_code = 0;
    value = g_strdup_printf("%s.%s", priv->unit, code);
    do_text_set(alias, prowod_key2.debit_other, value);
    g_free(value);
    prowod_key2.date = INT_MAX;
    memcpy(&prowod_key, &prowod_key2, sizeof(prowod_key2) - sizeof(prowod_key2.date));

    status = do_prowod_get2(alias, &prowod, &prowod_key2, DO_GET_LT);

    for (i = 0; i < PRIHOD_MAX_COUNT && (status == DO_OK); i++) {
        if (memcmp(&prowod_key, &prowod_key2, sizeof(prowod_key2) - sizeof(prowod_key2.date)))
            break;

        gchar *date;
        gchar *quantity;
        gchar *price;
        gchar *supplier;
        GtkTreeIter iter;
        struct tm tm;

        do_date(prowod.data.date, &tm);
        date = do_datetoa(tm);

        quantity = do_rest_format(do_prowod_sum(alias, &prowod, DO_CONST_PROWOD_SUM_QUANT)/coef);
        price    = do_money_format(do_prowod_sum(alias, &prowod, DO_CONST_PROWOD_SUM_PRICE)*coef, TRUE);
        supplier = do_prowod_param(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_COMMENT));

        gtk_tree_store_append (priv->stores[PRIHOD_VIEW], &iter, NULL);
        gtk_tree_store_set (priv->stores[PRIHOD_VIEW], &iter,
                            0, date,
                            1, quantity,
                            2, price,
                            3, supplier,
                            -1);
        g_free(date);
        g_free(quantity);
        g_free(price);
        g_free(supplier);

        status = do_prowod_get2(alias, &prowod, &prowod_key2, DO_GET_PREVIOUS);
	}

	do_alias_free(alias);
	return status != DO_ERROR;
}
//#endif
static gboolean do_product_edit_create_store_check(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    if (!priv->check_filter_date1) {
    	time_t now;
    	struct tm tm;
    	char *val1, *val2, *buf;
    	now = time(NULL);
    	tm = *localtime(&now);
    	tm.tm_hour = 0;tm.tm_min = 0;tm.tm_sec = 0;
    	priv->check_filter_date1 = mktime(&tm);
    	val1 = do_datetoa(tm);
    	tm.tm_hour = 23;tm.tm_min = 59;tm.tm_sec = 59;
    	priv->check_filter_date2 = mktime(&tm);
    	val2 = do_datetoa(tm);
    	buf = g_strdup_printf("%s %s", val1, val2);
    	g_object_set(priv->check_filter_entry, "text", buf, NULL);
    	g_free(val1);g_free(val2);g_free(buf);
    }

    priv->stores[CHECK_VIEW] = gtk_tree_store_new(10,
                                                    G_TYPE_INT, // cash
                                                    G_TYPE_INT, // shift
                                                    G_TYPE_INT, // check
                                                    G_TYPE_LONG, // time
                                                    G_TYPE_STRING, // operation
                                                    G_TYPE_DOUBLE, // quantity
                                                    G_TYPE_DOUBLE, // sum
                                                    G_TYPE_STRING, // cashier
                                                    G_TYPE_DOUBLE, // price
                                                    G_TYPE_BOOLEAN
                                                  );
    g_object_set(priv->views[CHECK_VIEW], "model", priv->stores[CHECK_VIEW], NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[CHECK_VIEW]), "Time", "Время", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 3);
    gtk_tree_view_column_set_sort_column_id (col, 3);
    gtk_tree_view_column_set_cell_data_func(col ,r, do_tree_cell_data_func_date_time, GINT_TO_POINTER(3), NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[CHECK_VIEW]), "Cash", "Касса", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[CHECK_VIEW]), "Operation", "Операция", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 4);
    gtk_tree_view_column_set_sort_column_id (col, 4);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[CHECK_VIEW]), "Quantity", "Количество", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 5);
    gtk_tree_view_column_set_sort_column_id (col, 5);
    gtk_tree_view_column_set_cell_data_func(col ,r, do_tree_cell_data_func_rest, GINT_TO_POINTER(5), NULL);


    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[CHECK_VIEW]), "Price", "Цена", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 8);
    gtk_tree_view_column_set_sort_column_id (col, 8);
    gtk_tree_view_column_set_cell_data_func(col ,r, do_tree_cell_data_func_money, GINT_TO_POINTER(8), NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[CHECK_VIEW]), "Sum", "Сумма", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 6);
    gtk_tree_view_column_set_sort_column_id (col, 6);
    gtk_tree_view_column_set_cell_data_func(col ,r, do_tree_cell_data_func_money, GINT_TO_POINTER(6), NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[CHECK_VIEW]), "Cashier", "Кассир", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 7);
    gtk_tree_view_column_set_sort_column_id (col, 7);

    return do_product_edit_get_check(dialog);
}
//#endif
typedef struct {
    time_t time;
    gint cash;
    gint shift;
    gint check;
    gint operation;
    gdouble quant;
    gdouble price;
    gint cashier;
    gboolean crnt;
} DoCheckRecord;

static gboolean do_product_edit_get_crntsale(DoProductEdit *dialog, do_alias_t *alias, do_sort_list_t *list, const gchar *code, time_t date1, time_t date2)
{
	int status;
	struct tm tm;
    DoCheckRecord record;
	document_key0_t document_key0, document_key;
	document_order_key2_t document_order_key2, document_order_key;
	document_order_rec_t document_order;
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);

    record.crnt = TRUE;

    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_CRNTSALE));
    do_text_set(alias, document_key0.sklad, priv->unit);
    do_text_set_empty(document_key0.document);
    memcpy(&document_key, &document_key0, sizeof(document_key0.dtype) + sizeof(document_key0.sklad));

    status = do_document_key0(alias, &document_key0, DO_GET_GE);
    while (status == DO_OK) {
        if (memcmp(&document_key, &document_key0, sizeof(document_key0.dtype) + sizeof(document_key0.sklad)))
            break;
        memcpy(&document_order_key2, &document_key0, sizeof(document_key0));
        do_text_set(alias, document_order_key2.code, code);
        do_cpy(document_order_key, document_order_key2);

        status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_GE);
        while (status == DO_OK) {
            if (memcmp(&document_order_key, &document_order_key2, sizeof(document_order_key.dtype)+sizeof(document_order_key.document)+sizeof(document_order_key.sklad)) ||
                strncmp(document_order_key2.code,code,do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)) )
                break;
            do_date(document_order.data.date, &tm);
            record.time = do_time(document_order.data.time, &tm);
            if (record.time >= date1 && record.time <= date2) {
                record.shift = atoi(document_order.data.document + 2);
                document_order.data.document[2] = '\0';
                record.cash = atoi(document_order.data.document);
                record.check = do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_CHECK));
                record.operation = do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_OPERATION));
                record.cashier = do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_CASHIER));
                record.quant = do_document_order_quant(alias, &document_order, DO_CONST_QUANT_CRNTSALE);
                record.price = do_document_order_price(alias, &document_order, DO_CONST_PRICE_FACT);
                do_sort_list_add_alloc(list, &record, sizeof(record));
            }
            status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_NEXT);
        }
        if (status == DO_ERROR)
            break;

        status = do_document_key0(alias, &document_key0, DO_GET_NEXT);
    }
    if (status == DO_ERROR) {
        do_alias_free(alias);
        return FALSE;
    }
    return TRUE;
}

static gboolean do_product_edit_get_check_list(do_alias_t *alias, do_sort_list_t *list, const gchar *code, time_t date1, time_t date2)
{
	int status;
	struct tm tm;
    DoCheckRecord record;
    check_key2_t check_key2, check_key;
    check_rec_t  check;
    product_key0_t product_key0;

    record.crnt = FALSE;

    do_set_empty(check_key2);
    do_text_set(alias, check_key2.product_code, code);
    tm = *localtime(&date1);
    check_key2.date = do_datetosql(tm);
    do_cpy(check_key.product_code, check_key2.product_code);
    tm = *localtime(&date2);
    check_key.date = do_datetosql(tm);

    status = do_check_get2(alias, &check, &check_key2, DO_GET_GE);
    while (status == DO_OK) {
        if (do_cmp(check_key.product_code, check_key2.product_code) ||
            check_key2.date > check_key.date
            )
            break;
        do_sqltodate(check.data.date, &tm);
        do_sqltotime(check.data.date, &tm);
        record.time = mktime(&tm);

        record.cash = check.data.cash;
        record.shift = check.data.shift;
        record.check = check.data.check;
        record.operation = atoi(check.data.operation);
        record.cashier = check.data.cashier;
        record.quant = check.data.quantity;
        record.price = check.data.sale_price;
        do_sort_list_add_alloc(list, &record, sizeof(record));

        status = do_check_get2(alias, &check, &check_key2, DO_GET_NEXT);
    }
    if (status == DO_ERROR) {
        do_alias_free(alias);
        return FALSE;
    }
    do_text_set(alias, product_key0.code, code);
    product_key0.base_parcel = 1;
    status = do_product_key0(alias, &product_key0, DO_GET_GE);

    while (status == DO_OK ) {

        if ( !strncmp(product_key0.code, code, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH))) break;

        record.crnt = FALSE;

        do_set_empty(check_key2);
        do_text_set(alias, check_key2.product_code, product_key0.code);
        tm = *localtime(&date1);
        check_key2.date = do_datetosql(tm);
        do_cpy(check_key.product_code, check_key2.product_code);
        tm = *localtime(&date2);
        check_key.date = do_datetosql(tm);

        status = do_check_get2(alias, &check, &check_key2, DO_GET_GE);
        while (status == DO_OK) {
            if (do_cmp(check_key.product_code, check_key2.product_code) ||
                check_key2.date > check_key.date
                )
                break;
            do_sqltodate(check.data.date, &tm);
            do_sqltotime(check.data.date, &tm);
            record.time = mktime(&tm);

            record.cash = check.data.cash;
            record.shift = check.data.shift;
            record.check = check.data.check;
            record.operation = atoi(check.data.operation);
            record.cashier = check.data.cashier;
            record.quant = check.data.quantity;
            record.price = check.data.sale_price;
            do_sort_list_add_alloc(list, &record, sizeof(record));

            status = do_check_get2(alias, &check, &check_key2, DO_GET_NEXT);
        }
        if (status == DO_ERROR) {
            do_alias_free(alias);
            return FALSE;
        }
        status = do_product_key0(alias, &product_key0, DO_GET_NEXT);
    }
    if (status == DO_ERROR) {
        do_alias_free(alias);
        return FALSE;
    }
    return TRUE;
}

static int do_product_edit_check_cmp(DoCheckRecord *r1, DoCheckRecord *r2)
{
    return r1->time - r2->time;
}

static gboolean do_product_edit_get_check(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
	do_alias_t *alias;
	gchar  *code;
	double coef;
	product_rec_t product;
	product_key0_t product_key0;
	do_sort_list_t *list;
	int i;
	gint total_pos = 0;
	gdouble total_quant = 0;
	gdouble total_sum = 0;
	gchar *total_buf;

	gtk_tree_store_clear(priv->stores[CHECK_VIEW]);

    alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, TRUE)) {
        do_alias_free(alias);
        return FALSE;
    }
    g_object_get(priv->context, "product-code", &code, NULL);
    g_return_val_if_fail ( code != NULL, FALSE );

	do_text_set(alias, product_key0.code, code);

	product_key0.base_parcel = 0;

	if (do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK) {
	    do_alias_free(alias);
        return FALSE;
	}
    coef = do_product_coef(alias, &product);

    list = do_sort_list_new(1, 0, (do_list_cmp_func)do_product_edit_check_cmp, NULL);
    if (!do_product_edit_get_crntsale(dialog, alias, list, code, priv->check_filter_date1, priv->check_filter_date2)) {
	    do_alias_free(alias);
        return FALSE;
    }
    if (!do_product_edit_get_check_list(alias, list, code, priv->check_filter_date1, priv->check_filter_date2)) {
	    do_alias_free(alias);
        return FALSE;
    }

    for (i = 0; i < list->count; i++) {
        DoCheckRecord *rec;
        GtkTreeIter iter;
        glong time;
        gchar *cashier;
        static const gchar *oper_name[4] = {"Нал", "Возв", "Б/н", "Возв"};

        rec = list->list[i];
        time = rec->time;
        cashier = do_get_partner_name(alias, do_param(DO_PARAM_PARTNER_REGION_CASHIER), rec->cashier);

        gtk_tree_store_append (priv->stores[CHECK_VIEW], &iter, NULL);
        gtk_tree_store_set (priv->stores[CHECK_VIEW], &iter,
                            0, rec->cash,
                            1, rec->shift,
                            2, rec->check,
                            3, time,
                            4, oper_name[(rec->operation > 0 && rec->operation < 5) ? rec->operation - 1 : 0],
                            5, rec->quant/coef,
                            6, rec->quant*rec->price,
                            7, cashier,
                            8, rec->price*coef,
                            9, rec->crnt,
                            -1);
        g_free(cashier);
        total_pos++;
        total_quant += rec->quant/coef;
        total_sum += rec->quant*rec->price;

	}
	do_sort_list_free(list);
	total_buf = g_strdup_printf("Позиций : <b>%d</b> Количество : <b>%s</b> Сумма : <b>%s</b>",
	            total_pos, do_rest_format(total_quant), do_money_format(total_sum, TRUE));
    gtk_label_set_markup(GTK_LABEL(priv->check_totally), total_buf);
    gtk_label_set_justify(GTK_LABEL(priv->check_totally), GTK_JUSTIFY_LEFT);
    g_object_set(priv->check_totally, "xalign", 0.01, NULL);
	g_free(total_buf);

	do_alias_free(alias);
	return TRUE;
}
#ifndef MIN_SCREEN
static gboolean do_product_edit_create_store_barcode(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    priv->stores[BARCODE_VIEW] = gtk_tree_store_new(1, G_TYPE_STRING);

    g_object_set(priv->views[BARCODE_VIEW], "model", priv->stores[BARCODE_VIEW], NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[BARCODE_VIEW]), "Barcode", "Продажный код", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);
    //g_object_set (r, "editable", TRUE, NULL);

    return do_product_edit_get_barcode(dialog);
}

static gboolean do_product_edit_get_barcode(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
	barcode_key1_t barcode_key1, barcode_key;
	barcode_rec_t barcode;

	int status;
	do_alias_t *alias;
	gchar *code;

    alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, TRUE)) {
        do_alias_free(alias);
        return FALSE;
    }

    g_object_get(priv->context, "product-code", &code, NULL);
    g_return_val_if_fail ( code != NULL, FALSE );

	do_text_set(alias, barcode_key1.code, code);
	do_cpy(barcode_key, barcode_key1);

    status = do_barcode_get1(alias, &barcode, &barcode_key1, DO_GET_GE);

    while (status == DO_OK) {
        if (do_cmp(barcode_key, barcode_key1))
            break;
        gchar *barcode_;
        GtkTreeIter iter;
        barcode_ = do_text(alias, barcode.data.barcode);
        gtk_tree_store_append (priv->stores[BARCODE_VIEW], &iter, NULL);
        gtk_tree_store_set (priv->stores[BARCODE_VIEW], &iter,
                            0, barcode_,
                            -1);
        g_free(barcode_);

        status = do_barcode_get1(alias, &barcode, &barcode_key1, DO_GET_NEXT);
	}

	do_alias_free(alias);
	return status != DO_ERROR;
}
static gboolean do_product_edit_create_store_rz(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    priv->stores[RZ1_VIEW] = gtk_tree_store_new(12, G_TYPE_STRING,
                                                  G_TYPE_STRING,
                                                  G_TYPE_STRING,
                                                  G_TYPE_INT,
                                                  G_TYPE_STRING,
                                                  G_TYPE_DOUBLE,
                                                  G_TYPE_INT,
                                                  G_TYPE_STRING,
                                                  G_TYPE_STRING,
                                                  G_TYPE_STRING,
                                                  G_TYPE_STRING,
                                                  G_TYPE_INT
                                                          );

    g_object_set(priv->views[RZ1_VIEW], "model", priv->stores[RZ1_VIEW], NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[RZ1_VIEW]), "Mnn", "МНН", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);
    g_object_set(r, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[RZ1_VIEW]), "TradeName", "Тор. наименование", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    g_object_set(r, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);


    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[RZ1_VIEW]), "PackForm", "Форма выпуска", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 2);
    g_object_set(r, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[RZ1_VIEW]), "PackQuant", "Количество в потребителькой упаковке", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 3);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[RZ1_VIEW]), "Manufactor", "Производитель", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 4);
    g_object_set(r, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[RZ1_VIEW]), "Price", "Цена", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 5);
    gtk_tree_view_column_set_cell_data_func(col ,r,
             do_tree_cell_data_func_money_with_sign, GINT_TO_POINTER(5), NULL);

/*    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[RZ1_VIEW]), "Valuta", "Валюта", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 6);
*/
    return do_product_edit_get_rz(dialog);
}

static gboolean do_product_edit_create_store_false(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    priv->stores[FALSE_VIEW] = gtk_tree_store_new(7, G_TYPE_INT,
                                                  G_TYPE_STRING,
                                                  G_TYPE_STRING,
                                                  G_TYPE_STRING,
                                                  G_TYPE_STRING,
                                                  G_TYPE_STRING,
                                                  G_TYPE_STRING
                                                          );

    g_object_set(priv->views[FALSE_VIEW], "model", priv->stores[FALSE_VIEW], NULL);

    r = gtk_cell_renderer_text_new();
    //g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[FALSE_VIEW]), "TradeName", "Тор. наименование", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    g_object_set(r, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);


    r = gtk_cell_renderer_text_new();
    //g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[FALSE_VIEW]), "Seria", "Серия", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 2);
    //g_object_set(r, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);

    r = gtk_cell_renderer_text_new();
    //g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[FALSE_VIEW]), "Manufactor", "Производитель", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 3);
    g_object_set(r, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);

    r = gtk_cell_renderer_text_new();
    //g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[FALSE_VIEW]), "Defect", "Описание", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 4);
    g_object_set(r, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);

    r = gtk_cell_renderer_text_new();
    //g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[FALSE_VIEW]), "Document", "Документ", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 5);
    //g_object_set(r, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);


/*    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[FALSE_VIEW]), "Valuta", "Валюта", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 6);
*/
    return do_product_edit_get_false(dialog);
}

static gboolean do_product_edit_get_rz(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);

	do_alias_t *alias;
	gchar *code;
    do_list_t *list;
    int i;

    alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, TRUE)) {
        do_alias_free(alias);
        return FALSE;
    }

    g_object_get(priv->context, "product-code", &code, NULL);
    g_return_val_if_fail ( code != NULL, FALSE );

    list = do_rz_info_list_from_code(alias, code);
    if (!list) {
        DOMINO_SHOW_ERROR("Ошибка чтения БД");
        do_alias_free(alias);
        return FALSE;
    }
    for (i = 0; i < list->count; i++) {
        GtkTreeIter iter;
        rz_info_t *rz_info;

        rz_info = list->list[i];
        gtk_tree_store_append (priv->stores[RZ1_VIEW], &iter, NULL);
        gtk_tree_store_set (priv->stores[RZ1_VIEW], &iter,
                        0, rz_info->mnn,
                        1, rz_info->trade_name,
                        2, rz_info->pack_form,
                        3, rz_info->pack_quant,
                        4, rz_info->manufactor,
                        5, rz_info->price,
                        6, rz_info->valuta,
                        7, rz_info->barcode,
                        8, rz_info->key.dtype,
                        9, rz_info->key.sklad,
                        10, rz_info->key.document,
                        11, rz_info->key.line,
                        -1);
	}
	do_rz_info_list_free(list);
	do_alias_free(alias);
	return TRUE;
}
static gboolean do_product_edit_get_false(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);

	do_alias_t *alias;
	gchar *code;

    alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, TRUE)) {
        do_alias_free(alias);
        return FALSE;
    }

    g_object_get(priv->context, "product-code", &code, NULL);
    g_return_val_if_fail ( code != NULL, FALSE );

    product_data_key0_t product_data_key0, key;
    product_data_rec_t product_data;
    int status;

    do_text_set(alias, product_data_key0.code, code);
    product_data_key0.type = 3;
    product_data_key0.number = 0;
    do_cpy(key.code, product_data_key0.code);
    key.type = product_data_key0.type;

    status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.code, product_data_key0.code) ||
             key.type != product_data_key0.type ) break;

        GtkTreeIter iter;
        char *name;
        char *seria;
        char *manufactor;
        char *defect;
        char *document;
        char *document_date;
        char *doc;
        char *url;

        name = do_product_data_param(alias, &product_data, "2");
        seria = do_product_data_param(alias, &product_data, "3");
        manufactor = do_product_data_param(alias, &product_data, "4");
        defect = do_product_data_param(alias, &product_data, "5");
        document = do_product_data_param(alias, &product_data, "6");
        document_date = do_product_data_param(alias, &product_data, "7");
        url = do_product_data_param(alias, &product_data, "8");
        doc = g_strdup_printf("%s от %s", document, document_date);

        gtk_tree_store_append (priv->stores[FALSE_VIEW], &iter, NULL);
        gtk_tree_store_set (priv->stores[FALSE_VIEW], &iter,
                        0, product_data.data.number,
                        1, name,
                        2, seria,
                        3, manufactor,
                        4, defect,
                        5, doc,
                        6, url,
                        -1);
        do_free(name);do_free(seria);do_free(manufactor);do_free(defect);
        do_free(document);do_free(document_date);g_free(doc);do_free(url);


        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_NEXT);
	}
	do_alias_free(alias);
	return TRUE;
}
#endif

static gboolean do_product_edit_create_store_order(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    if (!priv->order_filter_date1) {
    	time_t now;
    	struct tm tm;
    	char *val1, *val2, *buf;
    	now = time(NULL);
    	tm = *localtime(&now);
    	tm.tm_mday -=30;
    	tm.tm_hour = 0;tm.tm_min = 0;tm.tm_sec = 0;
    	priv->order_filter_date1 = mktime(&tm);
    	val1 = do_datetoa(tm);
    	tm = *localtime(&now);
    	tm.tm_hour = 23;tm.tm_min = 59;tm.tm_sec = 59;
    	priv->order_filter_date2 = mktime(&tm);
    	val2 = do_datetoa(tm);
    	buf = g_strdup_printf("%s %s", val1, val2);
    	g_object_set(priv->order_filter_entry, "text", buf, NULL);
    	g_free(val1);g_free(val2);g_free(buf);
    }

    priv->stores[ORDER_VIEW] = gtk_tree_store_new(3,
                                                    G_TYPE_LONG, // time
                                                    G_TYPE_DOUBLE, // quantity
                                                    G_TYPE_STRING // status
                                                  );
    g_object_set(priv->views[ORDER_VIEW], "model", priv->stores[ORDER_VIEW], NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[ORDER_VIEW]), "Time", "Дата", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);
    gtk_tree_view_column_set_sort_column_id (col, 0);
    gtk_tree_view_column_set_cell_data_func(col ,r, do_tree_cell_data_func_date_time, GINT_TO_POINTER(0), NULL);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[ORDER_VIEW]), "Status", "Статус", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "markup", 2);
    gtk_tree_view_column_set_sort_column_id (col, 2);

    r = gtk_cell_renderer_text_new();
    g_object_set(r, "xpad", DEFAULT_PAD, NULL);
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->views[ORDER_VIEW]), "Quantity", "Количество", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    gtk_tree_view_column_set_sort_column_id (col, 1);
    gtk_tree_view_column_set_cell_data_func(col ,r, do_tree_cell_data_func_rest, GINT_TO_POINTER(1), NULL);

    return do_product_edit_get_order(dialog);
}
static int domino_order_get_marked_quant_m(do_alias_t *alias, const char *unit, const char *product_code,  struct tm *tm1, struct tm *tm2, domino_marked_t *marked, struct tm *time_mark, double *quant)
{
	*marked = DOMINO_MARKED_UNMARK;
    document_order_key2_t document_order_key2;
    document_order_rec_t document_order;
    char document_number[40];
    struct tm tm;



    do_text_set(alias, document_order_key2.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_ORDER1));
    do_text_set(alias, document_order_key2.sklad, unit);
    do_text_set(alias, document_order_key2.code, product_code);

    tm = *tm2;
    while ( do_date_cmp(&tm, tm1) >= 0 ) {

        sprintf(document_number, "%4.4d%2.2d%2.2d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
        do_text_set(alias, document_order_key2.document, document_number);

        switch ( do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL) ) {
            case DO_ERROR:
                return FALSE;
            case DO_OK: {
                *marked = document_order.data.accepted;
                do_date(document_order.data.date, time_mark);
                do_time(document_order.data.time, time_mark);
                if ( quant )
                    *quant = do_document_order_quant(alias, &document_order, 1);
                switch ( document_order.data.accepted )  {
                    case DOMINO_MARKED:
                        do_date(document_order.data.date, time_mark);
                        do_time(document_order.data.time, time_mark);
                        break;
                    case DOMINO_MARKED_AND_ACCEPT:
                    case DOMINO_MARKED_AND_REJECT:
                        do_date(do_document_order_param_int(alias, &document_order, "DATE_ORDER"), time_mark);
                        do_time(do_document_order_param_int(alias, &document_order, "TIME_ORDER"), time_mark);
                        break;
                    default:
                        break;
                }
                return TRUE;
            }
            case DO_KEY_NOT_FOUND:
                break;
        }
        tm.tm_mday-=1;
        mktime(&tm);
    }

    return TRUE;
}

static gboolean do_product_edit_get_order(DoProductEdit *dialog)
{
	DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (dialog);
	do_alias_t *alias;
	gchar  *code;
	double  coef;
	product_rec_t product;
	product_key0_t product_key0;
	gdouble total_quant = 0;
	gchar *total_buf;
	struct tm tm, tmd;
	int status;

	gtk_tree_store_clear(priv->stores[ORDER_VIEW]);

    alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, TRUE)) {
        do_alias_free(alias);
        return FALSE;
    }
    g_object_get(priv->context, "product-code", &code, NULL);
    g_return_val_if_fail ( code != NULL, FALSE );

	do_text_set(alias, product_key0.code, code);

	product_key0.base_parcel = 0;

	if (do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK) {
	    do_alias_free(alias);
        return FALSE;
	}
    document_order_key2_t document_order_key2, document_order_key;
    document_order_rec_t document_order;

    do_text_set(alias, document_order_key2.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_ORDER1));
    do_text_set(alias, document_order_key2.sklad, priv->unit);
    memcpy(&document_order_key, &document_order_key2, sizeof(document_order_key2.dtype)+ sizeof(document_order_key2.sklad));

    coef = do_product_coef(alias, &product);

    tm = *localtime(&priv->order_filter_date1);

    while (mktime(&tm) <= priv->order_filter_date2) {
        gchar *value;
        memcpy(&document_order_key2, &document_order_key, sizeof(document_order_key2.dtype)+ sizeof(document_order_key2.sklad));
        value = g_strdup_printf("%4.4d%2.2d%2.2d",  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
        tm.tm_mday += 1;
        do_text_set(alias, document_order_key2.document, value);
        g_free(value);
        do_text_set(alias, document_order_key2.code, code);
        do_cpy(document_order_key, document_order_key2);

        status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL);
        while (status == DO_OK) {
            GtkTreeIter iter;
            domino_marked_t marked;
            if (do_cmp(document_order_key, document_order_key2))
                break;
            struct tm tm_mark;

            marked = document_order.data.accepted;
            /*switch ( marked ) {
                case DOMINO_MARKED_AND_ACCEPT:
                case DOMINO_MARKED_AND_REJECT:
                    do_date(do_document_order_param_int(alias, &document_order, "DATE_ORDER"), &tm_mark);
                    do_time(do_document_order_param_int(alias, &document_order, "TIME_ORDER"), &tm_mark);
                    break;
                default:
                    do_date(document_order.data.date, &tm_mark);
                    do_time(document_order.data.time, &tm_mark);
            }*/
            do_date(do_document_order_param_int(alias, &document_order, "DATE_ORDER"), &tm_mark);
            do_time(do_document_order_param_int(alias, &document_order, "TIME_ORDER"), &tm_mark);
            char *stat, time_str[30];
            strftime(time_str,sizeof(time_str)-1, "%d/%m %H:%M", &tm_mark);

            switch ( marked ) {
                case DOMINO_MARKED_UNMARK:
                    stat = g_strdup("");
                    break;
                case DOMINO_MARKED:
                    stat = g_strdup_printf("Заявка %s", time_str);
                    break;
                case DOMINO_MARKED_AND_ACCEPT:
                    stat = g_strdup_printf("<span foreground=\"green\">Заказ</span> %s", time_str);
                    break;
                case DOMINO_MARKED_AND_REJECT:
                    stat = g_strdup_printf("<span foreground=\"red\">Отказ</span> %s", time_str);
                    break;
            }
            time_t time;
            do_date(document_order.data.date, &tmd);
            time = do_time(document_order.data.time, &tmd);

            gtk_tree_store_append (priv->stores[ORDER_VIEW], &iter, NULL);
            gtk_tree_store_set (priv->stores[ORDER_VIEW], &iter,
                            0, time,
                            1, do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST)/coef,
                            2, stat,
                            -1);
            g_free(stat);
            total_quant = total_quant + do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST)/coef;
            status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_NEXT);
        }
        if (status == DO_ERROR) {
            DOMINO_SHOW_ERROR("Ошибка чтения БД");
            do_alias_free(alias);
            return FALSE;
        }
	}
	total_buf = g_strdup_printf("Количество : <b>%s</b>",
	            do_rest_format(total_quant));
    gtk_label_set_markup(GTK_LABEL(priv->order_totally), total_buf);
    gtk_label_set_justify(GTK_LABEL(priv->order_totally), GTK_JUSTIFY_LEFT);
    g_object_set(priv->order_totally, "xalign", 0.01, NULL);
	g_free(total_buf);

	do_alias_free(alias);
	return TRUE;
}
//#endif
static void	do_product_edit_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_product_edit_get_title(DoView *view)
{
    return "Товар";
}
static GdkPixbuf  *do_product_edit_get_icon(DoView *view)
{
    DoDialogAction action;
    DoProductEditFlags flags;
    g_object_get(view, "action", &action, "flags", &flags, NULL);
#if GTK_MAJOR_VERSION == 2
    DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE (view);
    switch (action) {
        case DO_DIALOG_ACTION_EDIT:
            return  gtk_icon_theme_load_icon(priv->icon_theme, GTK_STOCK_EDIT, DO_VIEW_ICON_SIZE, 0, NULL);
        case DO_DIALOG_ACTION_INSERT:
            return  gtk_icon_theme_load_icon(priv->icon_theme, GTK_STOCK_NEW, DO_VIEW_ICON_SIZE, 0, NULL);
        case DO_DIALOG_ACTION_DELETE:
            return  gtk_icon_theme_load_icon(priv->icon_theme, GTK_STOCK_DELETE, DO_VIEW_ICON_SIZE, 0, NULL);
        default:
            return  gtk_icon_theme_load_icon(priv->icon_theme, GTK_STOCK_FILE, DO_VIEW_ICON_SIZE, 0, NULL);
    }
#else
    return NULL;
#endif
}
static void		   do_product_edit_do_save(DoView *view)
{

}
static gboolean    do_product_edit_can_do_save(DoView *view)
{
    return TRUE;
}
static gboolean    do_product_edit_can_do_close_for_esc(DoView *view)
{
    return TRUE;
}
static gboolean	view_can_do_send(DoView *view)
{
	return TRUE;
	/*DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE(view);

    gboolean retval = FALSE;
    if ( gtk_notebook_current_page(GTK_NOTEBOOK(priv->notebook)) == PARCEL_VIEW ) {
        GtkTreePath *path = NULL;
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->views[PARCEL_VIEW]), &path, NULL);
        if ( path ) {
            gtk_tree_path_free(path);
            retval = TRUE;
        }
    }
    return retval;*/
}
#ifdef CASH
static gboolean sad(gpointer data)
{
    domino_send_barcode_eventkey_to_crntwin((char *)data);
    do_free(data);
    return FALSE;
}
#endif
static void view_do_send(DoView *view)
{
#ifdef CASH
    gchar *code;
    if ( DO_IS_PRODUCT_EDIT(view) ) {
        gint page_number;

        DoProductEditPrivate *priv = DO_PRODUCT_EDIT_GET_PRIVATE(view);
        page_number = gtk_notebook_get_current_page(GTK_NOTEBOOK(priv->notebook));

        if ( page_number == PARCEL_VIEW ) {
            GtkTreePath *path = NULL;
            gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->views[PARCEL_VIEW]), &path, NULL);
            if ( path ) {
                GValue value = {0,};
                GtkTreeIter iter;
                gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->stores[PARCEL_VIEW]), &iter, path);
                gtk_tree_model_get_value(GTK_TREE_MODEL(priv->stores[PARCEL_VIEW]), &iter, 7, &value);
                if ( strlen(g_value_get_string(&value))  ) {
                    code = g_value_dup_string(&value);

                    domino_back_window_focus();
                    g_timeout_add(BACK_TIMEOUT, sad,code);
                }
            }
        }
        else {
            domino_back_window_focus();
            g_object_get(priv->context, "product-code", &code, NULL);
            g_timeout_add(BACK_TIMEOUT, sad, code);
        }
    }
    else {
        if ( DO_TREE_VIEW(view )) {
            gchar *name = NULL;
            name = g_object_get_data(G_OBJECT(view), "view-name");
            if ( name && !g_strcmp0(name, view_names[PARCEL_VIEW]) ) {
                GtkTreePath *path = NULL;
                GtkTreeView *tree_view;
                GtkTreeModel *tree_model;
                g_object_get(view, "tree-view", &tree_view, NULL);
                tree_model = gtk_tree_view_get_model(tree_view);
                gtk_tree_view_get_cursor(tree_view, &path, NULL);
                if ( path ) {
                    GValue value = {0,};
                    GtkTreeIter iter;
                    gtk_tree_model_get_iter(tree_model, &iter, path);
                    gtk_tree_model_get_value(tree_model, &iter, 7, &value);
                    if ( strlen(g_value_get_string(&value))  ) {
                        code = g_value_dup_string(&value);

                        domino_back_window_focus();
                        g_timeout_add(500, sad,code);
                    }
                }
            }
            /*else {
                    if ( )
                GtkWidget *widget;

                domino_back_window_focus();
                g_object_get(priv->context, "product-code", &code, NULL);
                g_timeout_add(BACK_TIMEOUT, sad, code);
            }*/
        }
    }
#endif
}
#ifndef CASH
static void do_product_edit_stock_mode_toggled(GtkRadioButton *button, DoProductEdit *dialog)
{
    int stock_mode,old=0;
    gchar *mode;
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)) ) {
        stock_mode = GPOINTER_TO_INT(g_object_get_data( G_OBJECT(button), "stock-mode"));
        DOMINO_LOCAL_GET("ProductEditStock", "stock-mode", &mode, NULL);
        if ( mode )
            old = atoi(mode);
        if ( old != stock_mode ) {
            mode = g_strdup_printf("%d", stock_mode);
            domino_config_property_set(DOMINO_CONFIG_LOCAL,
                       "ProductEditStock", "stock-mode", mode, NULL);
            domino_config_save(DOMINO_CONFIG_LOCAL,TRUE);
            do_product_edit_get_stock(dialog);
        }
    }
}
static const char *stock_mode_name[] = {"Город","Фирма","Все"};
GtkWidget *do_product_edit_create_stock_mode(DoProductEdit *dialog)
{
    GtkWidget *box;
    GtkWidget *r[3];
    int stock_mode = 0, i;
    gchar *mode;
    GSList *group;

#if GTK_MAJOR_VERSION == 2
    box = gtk_hbox_new(FALSE, 2);
#else
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(box), GUI_PAD_BIG);
    for ( i=0; i < 3; i++ ) {
        r[i] = gtk_radio_button_new_with_label(i ? group : NULL, stock_mode_name[i]);
        g_object_set_data(G_OBJECT(r[i]), "stock-mode", GINT_TO_POINTER(i));
        gtk_box_pack_start (GTK_BOX (box), r[i], FALSE, TRUE, 0);
        g_signal_connect(G_OBJECT(r[i]), "toggled",
              G_CALLBACK(do_product_edit_stock_mode_toggled), dialog);
        //if ( !i )
        group = gtk_radio_button_get_group( GTK_RADIO_BUTTON( r[i] ) );
    }
    DOMINO_LOCAL_GET("ProductEditStock", "stock-mode", &mode, NULL);
    if ( mode )
        stock_mode = atoi(mode);
    if ( stock_mode < 0 || stock_mode > 2 ) stock_mode = 0;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(r[stock_mode]), TRUE);
    return box;
}
#endif

















