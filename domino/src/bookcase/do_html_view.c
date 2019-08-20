
#include "do_html_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_view_actions.h"
#include "domino.h"
#include <webkit/webkit.h>
#include <libsoup/soup.h>
#include <string.h>
#include <unistd.h>

#define DO_HTML_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_HTML_VIEW, DoHtmlViewPrivate))
#define DEFAULT_PROXY_IGNORE "localhost,127.0.0.1,192.168.,10."

#ifndef GDK_KEY_Escape
#define GDK_KEY_Escape GDK_Escape
#endif
#ifndef GDK_KEY_L
#define GDK_KEY_L GDK_L
#endif
enum
{
	PROP_0,
	PROP_HTML_STRING1,
	PROP_URL,
	PROP_TITLE,
	PROP_LOAD_STATUS,
	PROP_LOAD_PROGRESS,
};

static void focus_in(GtkWidget *widget, GdkEventFocus *event, gpointer user_data);

// View interface
static void do_html_view_view_init(DoViewIface *iface);

static gboolean view_close_request(DoView *embed);

static void view_close(DoView *embed);
static const char *view_get_title(DoView *embed);
static GdkPixbuf    *view_get_icon			(DoView *embed);
static gboolean	  view_get_load_status	(DoView *embed);
static gint	       view_get_load_progress(DoView *view);

static void		command_do_print(DoView *view);
//static void		command_do_print_now(DoView *view);
static gboolean	command_can_do_print(DoView *view);

static void		command_do_open(DoView *view);
static gboolean	command_can_do_open(DoView *view);
static void		command_do_save_as(DoView *view);
static gboolean	command_can_do_save_as(DoView *view);
static gboolean command_do_grab_focus(DoView *view);
static void		command_do_mail_send(DoView *view);
static gboolean	command_can_do_mail_send(DoView *view);

static gboolean key_press(GtkWidget *widget, GdkEventKey *event, DoView *do_view);


#ifndef CASH
static gboolean first_start = TRUE;
static gboolean reload_page = FALSE;
#endif

struct _DoHtmlViewPrivate
{
	GtkWidget    *view_html;
    GdkPixbuf    *icon;
    gchar        *title;
    gchar        *html_string;
    gboolean      load_status;
    gint          load_progress;
    //GtkWidget    *url_entry;
    GtkWidget    *load_progress_bar;
};

G_DEFINE_TYPE_WITH_CODE (DoHtmlView, do_html_view, GTK_TYPE_BOX,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_html_view_view_init)
                            );

static void do_html_view_view_init(DoViewIface *iface)
{
    iface->do_close        = view_close;
    iface->close_request   = view_close_request;
    iface->get_title       = view_get_title;
    iface->get_icon        = view_get_icon;
    iface->get_load_status = view_get_load_status;
    iface->get_load_progress = view_get_load_progress;
    iface->do_print        = command_do_print;
    iface->do_print_now    = do_html_view_command_do_print_now;
    iface->can_do_print    = command_can_do_print;
    iface->do_save_as        = command_do_save_as;
    iface->can_do_save_as    = command_can_do_save_as;
    iface->do_open         = command_do_open;
    iface->can_do_open      = command_can_do_open;
    iface->do_save         = command_do_save_as;
    iface->can_do_save     = command_can_do_save_as;
    iface->do_grab_focus   = command_do_grab_focus;
    iface->do_mail_send    = command_do_mail_send;
    iface->can_do_mail_send    = command_can_do_mail_send;

}

static void do_html_view_init(DoHtmlView *temp)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (temp);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    //priv->icon = gtk_icon_theme_load_icon(icon_theme, DO_STOCK_DOMINO_WEB, DO_VIEW_ICON_SIZE, 0, NULL);

}

static void do_html_view_load_started(WebKitWebView  *web_view,
                                                        WebKitWebFrame *frame,
                                                        DoView *view)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);
	priv->load_status = TRUE;
	priv->load_progress = 0;
	gtk_entry_set_progress_fraction(GTK_ENTRY(priv->load_progress_bar),0);
    //gtk_widget_show(GTK_WIDGET(priv->load_progress_bar));
    //gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(priv->load_progress_bar), 0);
    g_object_notify (G_OBJECT (view), "load-status");

}
static void do_html_view_load_finished(WebKitWebView  *web_view,
                                                        WebKitWebFrame *frame,
                                                        DoView *view)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);
	priv->load_status = FALSE;
	gtk_entry_set_progress_fraction(GTK_ENTRY(priv->load_progress_bar),0);
    //gtk_widget_hide(GTK_WIDGET(priv->load_progress_bar));
   g_object_notify (G_OBJECT (view), "load-status");
#ifndef CASH
    if ( reload_page ) {
        webkit_web_view_reload(WEBKIT_WEB_VIEW(priv->view_html));
	reload_page = FALSE;
    }
#endif
   gtk_widget_grab_focus(priv->view_html);
}
/*static void do_html_view_load_changed(WebKitWebView  *web_view,
                                                         WebKitLoadEvent load_event,
                                                        DoView *view)
{
    switch (load_event) {
    case WEBKIT_LOAD_STARTED:
        do_html_view_load_started(web_view, view);
        break;
    case WEBKIT_LOAD_FINISHED:
        do_html_view_load_finished(web_view, view);
        break;
    };

	//fix meDoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);
	//priv->load_progress = webkit_web_view_get_estimated_load_progress(web_view)*100;
    //g_object_notify (G_OBJECT (view), "load-progress");
    //gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(priv->load_progress_bar), priv->load_progress/100.);
}*/

static void do_html_view_load_changed(WebKitWebView *web_view,
                                                        gint           progress,
                                                        DoView *view)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);
	priv->load_progress = progress;
    g_object_notify (G_OBJECT (view), "load-progress");
    //gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(priv->load_progress_bar), progress/100.);
    gtk_entry_set_progress_fraction(GTK_ENTRY(priv->load_progress_bar),progress/100.);

}

static void do_html_view_set_url(DoHtmlView *view, const gchar *url);
static void url_activated(GtkEntry *entry, DoView *view)
{
    DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);
    const gchar *text;
    text = gtk_entry_get_text(entry);
    if ( text[0] != '\0' ) {
        do_html_view_set_url(DO_HTML_VIEW(view), text);
	gtk_widget_grab_focus(priv->view_html);


    }

}
gboolean
user_function1 (WebKitWebView  *web_view,
               WebKitWebFrame *web_frame,
               gchar          *uri,
               GError         *web_error,
               gpointer        user_data) //fix me
{
    //do_data_t *data;
    //data = do_data_new();
    //do_data_add_str(data, web_error->message);
    //web_error->message = g_strdup("fuck");
    //do_data_write_to_file(data, "c:/var/1.txt");//fix me
    //webkit_web_view_set_custom_encoding(web_view, "utf-8");
    gchar *message = g_strdup_printf("Невозможно открыть адрес %s. %s", uri, web_error->message);
    DOMINO_SHOW_ERROR(message);
    g_free(message);
    DOMINO_SHOW_MESSAGE();
    return TRUE;
}
static GObject *do_html_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject            *object;
	DoHtmlViewPrivate  *priv;
	GtkBox            *frame;
	GtkWidget          *s;

	object = G_OBJECT_CLASS (do_html_view_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv = DO_HTML_VIEW_GET_PRIVATE (object);
	frame = GTK_BOX(object);

	//priv->url_entry = s = gtk_entry_new();
    //g_signal_connect(s,  "activate",  G_CALLBACK(url_activated), object);


    g_object_set(object, "orientation", GTK_ORIENTATION_VERTICAL, NULL);

    s = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(s), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    priv->view_html = webkit_web_view_new();

    g_signal_connect(priv->view_html,  "focus-in-event",  G_CALLBACK(focus_in), object);
    g_signal_connect(priv->view_html,  "key-press-event",  G_CALLBACK(key_press), object);
    //g_signal_connect(priv->view_html,  "load-changed",  G_CALLBACK(do_html_view_load_changed), object);
    g_signal_connect(priv->view_html,  "load-started",  G_CALLBACK(do_html_view_load_started), object);
    g_signal_connect(priv->view_html,  "load-finished",  G_CALLBACK(do_html_view_load_finished), object);
    g_signal_connect(priv->view_html,  "load-progress-changed",  G_CALLBACK(do_html_view_load_changed), object);
    //g_signal_connect(priv->view_html,  "load-error",  G_CALLBACK(user_function1), object);

    /*WebKitSettings *s2 = webkit_settings_new ();
    printf("%d\n", webkit_settings_get_enable_java(s2));
    WebKitWebPluginDatabase *s1;
    s1 = webkit_get_web_plugin_database();
    printf( "%d\n", s1);
    GSList *l;
    for ( l = webkit_web_plugin_database_get_plugins(s1); l; l=l->next ) {
        char *name;
        name = webkit_web_plugin_get_name(l->data);
        printf("%s\n", name);
        webkit_web_plugin_set_enabled(l->data,TRUE);
    }*/
    gchar *str;
    gdouble level = 0;

    DOMINO_LOCAL_GET("Webkit", "zoom-level", &str, NULL);
    if ( str )
        level = atof(str);

    if ( level < 0.1 )
        level = 1;

    webkit_web_view_set_zoom_level (
        WEBKIT_WEB_VIEW (priv->view_html), level);

    priv->load_progress_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(priv->load_progress_bar), "Загрузка");
    g_object_set(priv->load_progress_bar,"show-text", TRUE, NULL);
    //gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);

	//gtk_box_pack_start(GTK_BOX(frame), priv->url_entry, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER (s), priv->view_html);
    gtk_box_pack_start(GTK_BOX(frame), s, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(frame), priv->load_progress_bar, FALSE, FALSE, 0);

    gtk_widget_show(GTK_WIDGET(priv->view_html));
    //gtk_widget_show(GTK_WIDGET(priv->url_entry));
    //gtk_widget_show(GTK_WIDGET(priv->load_progress_bar));
    gtk_widget_show(GTK_WIDGET(s));
    gtk_widget_show(GTK_WIDGET(frame));

	return object;
}

static void do_html_view_finalize (GObject *object)
{
	G_OBJECT_CLASS (do_html_view_parent_class)->finalize (object);
}
static void do_set_html_string(DoHtmlView *view, const gchar *string)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);
    if (priv->html_string)
        g_free(priv->html_string);
    priv->html_string = g_strdup(string);
    webkit_web_view_load_html_string(WEBKIT_WEB_VIEW(priv->view_html),
                  priv->html_string, NULL);
}
#ifndef CASH
#if (GTK_MAJOR_VERSION < 3 && GTK_MINOR_VERSION <= 20)
char *soup_uri_get_host(SoupURI *uri)
{
    return uri->host;
}
#endif
#endif
static void do_html_view_set_url(DoHtmlView *view, const gchar *url)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);

    gchar *httpProxy, *proxyIgnore;

    DOMINO_LOCAL_GET("Webkit", "proxy-string", &httpProxy, "proxy-ignore", &proxyIgnore, NULL);

    if ( httpProxy && httpProxy[0] != '\0' ) {
        SoupURI *proxyUri, *uri;
        gchar **ignores;
        const gchar *ignore;
        gchar *host;
        guint len, ilen;
        gboolean no_proxy;

        if ( proxyIgnore && proxyIgnore[0] != '\0' )
            ignore = proxyIgnore;
        else
            ignore = DEFAULT_PROXY_IGNORE;

        uri = soup_uri_new(url);
        if ( soup_uri_get_host(uri) != NULL ) {
            host = g_hostname_to_ascii(soup_uri_get_host(uri));
            len = strlen(host);

            ignores = g_strsplit(ignore, ",", -1);
            no_proxy = FALSE;
            if ( ignores ) {
                int i;
                for ( i = 0; i < g_strv_length(ignores) && !no_proxy; i++ ) {
                    gchar *str;
                    str = g_hostname_to_ascii(ignores[i]);
                    ilen = strlen(str);
                    if ( len >= ilen &&
                         !g_ascii_strncasecmp(host, str, ilen) )
                        no_proxy = TRUE;
                    g_free(str);
                }
                g_strfreev(ignores);
            }
            g_free(host);
        }
        soup_uri_free(uri);

        if ( !no_proxy ) {
            proxyUri = soup_uri_new(httpProxy);
            g_object_set(webkit_get_default_session(), SOUP_SESSION_PROXY_URI, proxyUri, NULL);
            soup_uri_free(proxyUri);
        }
        else
            g_object_set(webkit_get_default_session(), SOUP_SESSION_PROXY_URI, NULL, NULL);
    }

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(priv->view_html), url);
}

static void do_html_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (object);
	/* no readable properties */
	switch (prop_id)
	{
		case PROP_HTML_STRING1:
            g_value_set_string(value, priv->html_string);
			break;
        case PROP_TITLE:
            g_value_set_string(value, priv->title);
            break;
        case PROP_LOAD_STATUS:
            g_value_set_boolean(value, view_get_load_status(DO_VIEW(object)));
            break;
        case PROP_LOAD_PROGRESS: {
            g_value_set_int(value, view_get_load_progress(DO_VIEW(object)));
            break;
        }
        default :
            g_assert_not_reached ();
	}
}

static void do_html_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_HTML_STRING1:

	        if (g_value_get_string(value))
            do_set_html_string(DO_HTML_VIEW(object), g_value_get_string(value));
			break;
        case PROP_URL:
            do_html_view_set_url(DO_HTML_VIEW(object), g_value_get_string(value));
#ifndef CASH
            if ( first_start ) {
        	first_start = FALSE;
        	reload_page = TRUE;
            }
#endif
            break;
        case PROP_TITLE:
            if (priv->title)
                g_free(priv->title);
            priv->title = g_value_dup_string(value);
            break;
	}
}

static void do_html_view_class_init (DoHtmlViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_html_view_constructor;
	object_class->finalize     = do_html_view_finalize;
	object_class->get_property = do_html_view_get_property;
	object_class->set_property = do_html_view_set_property;

	g_type_class_add_private (object_class, sizeof (DoHtmlViewPrivate));

    g_object_class_install_property
        (object_class,
         PROP_HTML_STRING1,
         g_param_spec_string("html-string",
                      "",
                      "",
                      NULL,
                       G_PARAM_READWRITE));
    g_object_class_install_property
        (object_class,
         PROP_URL,
         g_param_spec_string("uri",
                      "",
                      "",
                      NULL,
                       G_PARAM_READWRITE));
    g_object_class_install_property
        (object_class,
         PROP_TITLE,
         g_param_spec_string("title",
                      "",
                      "",
                      NULL,
                       G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_LOAD_STATUS,
				   g_param_spec_boolean("load-status",
							"Статус загрузки",
							"Статус загрузки",
							FALSE,
							G_PARAM_READABLE));
    g_object_class_install_property (object_class,
				   PROP_LOAD_PROGRESS,
				   g_param_spec_int("load-progress",
							"Статус загрузки",
							"Статус загрузки",
							0,100,0,
							G_PARAM_READABLE));
}

GtkWidget *do_html_view_new ()
{
	return g_object_new (DO_TYPE_HTML_VIEW,
			     (gpointer) NULL);
}

static gboolean view_close_request(DoView *embed)
{
    return TRUE;
}
static void view_close(DoView *embed)
{
    gtk_widget_destroy(GTK_WIDGET(embed));
}
static const char *view_get_title(DoView *embed)
{
	DoHtmlViewPrivate  *priv;
	priv = DO_HTML_VIEW_GET_PRIVATE (embed);

    return priv->title;
}
static GdkPixbuf    *view_get_icon			(DoView *embed)
{
	DoHtmlViewPrivate  *priv;
	priv = DO_HTML_VIEW_GET_PRIVATE (embed);

    return priv->icon;
}
static gboolean	  view_get_load_status	(DoView *view)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);
	return priv->load_status;
}
static void command_do_print(DoView *view)
{
	DoHtmlViewPrivate  *priv;
	priv = DO_HTML_VIEW_GET_PRIVATE (view);

#ifndef USE_DEPRECATE_PRINT

    GtkPrintOperation *operation;
    GError *error = NULL;

    operation = gtk_print_operation_new ();

    gtk_print_operation_set_use_full_page (operation, FALSE);
    gtk_print_operation_set_unit (operation, GTK_UNIT_POINTS);

    webkit_web_frame_print_full(webkit_web_view_get_main_frame (
        WEBKIT_WEB_VIEW (priv->view_html)),
                                     operation,
                                     GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
                                     &error);

    g_object_unref (operation);

    if (error)  {
        DOMINO_SHOW_ERROR("Печать. %s", error->message);
        g_error_free (error);

    }
#else
    do_print_html(GTK_WIDGET(view), priv->html_string);
#endif
}
void do_html_view_command_do_print_now(DoView *view)
{
	DoHtmlViewPrivate  *priv;
	priv = DO_HTML_VIEW_GET_PRIVATE (view);

#ifndef USE_DEPRECATE_PRINT

    GtkPrintOperation *operation;
    GError *error = NULL;

    operation = gtk_print_operation_new ();

    gtk_print_operation_set_use_full_page (operation, FALSE);
    gtk_print_operation_set_unit (operation, GTK_UNIT_POINTS);

    webkit_web_frame_print_full(webkit_web_view_get_main_frame (
        WEBKIT_WEB_VIEW (priv->view_html)),
                                     operation,
                                     GTK_PRINT_OPERATION_ACTION_PRINT,
                                     &error);

    g_object_unref (operation);

    if (error)  {
        DOMINO_SHOW_ERROR("Печать. %s", error->message);
        g_error_free (error);

    }
/*
	DoHtmlViewPrivate  *priv;
	priv = DO_HTML_VIEW_GET_PRIVATE (view);

#ifndef USE_DEPRECATE_PRINT
    GtkPrintOperation *operation;
    GError *error = NULL;

    operation = gtk_print_operation_new ();

    gtk_print_operation_set_use_full_page (operation, FALSE);
    gtk_print_operation_set_unit (operation, GTK_UNIT_POINTS);

    webkit_web_frame_print_full(webkit_web_view_get_main_frame (
        WEBKIT_WEB_VIEW (priv->view_html)),
                                     operation,
                                     GTK_PRINT_OPERATION_ACTION_PRINT,
                                     &error);

    g_object_unref (operation);

    if (error)  {
        do_message_dialog_show (GTK_WIDGET(view),
				       GTK_MESSAGE_ERROR,
				       GTK_BUTTONS_CLOSE,
				       "%s", error->message);
        g_error_free (error);

    }*/
#else
    if ( !priv->html_string )
        command_do_print(view);
    else
        do_print_html(GTK_WIDGET(view), priv->html_string);
#endif
}
static gboolean	command_can_do_print(DoView *view)
{
    return TRUE;
}

static void command_do_save_as(DoView *view)
{
	DoHtmlViewPrivate  *priv;
	priv = DO_HTML_VIEW_GET_PRIVATE (view);
    GtkWidget* dialog;
    GtkWidget* window;
    gchar* filename;

    if ( !gtk_widget_get_visible(GTK_WIDGET(view)) )
        return;
    window = gtk_widget_get_toplevel(GTK_WIDGET(view));

    dialog = gtk_file_chooser_dialog_new (
        "Сохранить как", GTK_WINDOW( window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "Отмена", GTK_RESPONSE_CANCEL,
        "Сохранить", GTK_RESPONSE_ACCEPT,
        NULL);
    gtk_window_set_icon_name (GTK_WINDOW (dialog), "domino");
    gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        gchar *value;
        char *name;
        GError *error = NULL;
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        value = g_utf8_strdown(filename, -1);
        if ( strlen(value) < 5 ||
            ( strcmp(value + strlen(value) - 5, ".html") &&
              strcmp(value + strlen(value) - 5, ".htm"))
            ) {
            g_free(value);
            value = g_strdup_printf("%s.html", filename);
        }
        else
            value = g_strdup(filename);

        name = g_locale_from_utf8(value, -1, NULL, NULL, &error);
        if (!do_save_to_file(name, priv->html_string, strlen(priv->html_string))) {
            DOMINO_SHOW_ERROR("Файл \"%s\" не сохранен", filename);
        }
        g_free(name);
        g_free(value);
    }
    gtk_widget_destroy (dialog);
}
static gboolean	command_can_do_open(DoView *view)
{
    return TRUE;
}

static void command_do_open(DoView *view)
{
	//DoHtmlViewPrivate  *priv;
	//priv = DO_HTML_VIEW_GET_PRIVATE (view);
    GtkWidget* dialog;
    GtkWidget* window;
    gchar* filename;

    if ( !gtk_widget_get_visible(GTK_WIDGET(view)) )
        return;
    window = gtk_widget_get_toplevel(GTK_WIDGET(view));

    dialog = gtk_file_chooser_dialog_new (
        "Открыть файл", GTK_WINDOW( window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "Отмена", GTK_RESPONSE_CANCEL,
        "Открыть", GTK_RESPONSE_ACCEPT,
        NULL);
    gtk_window_set_icon_name (GTK_WINDOW (dialog), "domino");
    gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        gchar *text;
        if ((text = do_read_from_file(filename)) == NULL) {
            DOMINO_SHOW_ERROR("Файл \"%s\" не открывается", filename);
        }
        else {
            do_set_html_string(DO_HTML_VIEW(view), text);
            g_free(text);
        }
    }
    gtk_widget_destroy (dialog);
}

static gboolean	command_can_do_save_as(DoView *view)
{
    return TRUE;
}

static void focus_in(GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
    do_view_actions_set_view(user_data);
}

static gboolean key_press(GtkWidget *widget, GdkEventKey *event, DoView *do_view)
{
	DoHtmlViewPrivate  *priv;
	priv = DO_HTML_VIEW_GET_PRIVATE (do_view);
	guint mask = gtk_accelerator_get_default_mod_mask ();
	if ((event->state & mask) == 0)
	{
#ifdef CASH
        gchar *str;
        if ( (str = search_get_text(event->string, event->length, 0)) != NULL) {
            g_free(event->string);
            event->string = str;
            event->length = strlen(str);
            event->keyval = gdk_unicode_to_keyval( g_utf8_get_char(str));
            return FALSE;

        }
#endif
		switch (event->keyval)
		{
            case GDK_KEY_Escape:
                view_close(do_view);
            	return TRUE;

		default:
			break;
		}
	}

	if ((event->state & mask) == GDK_CONTROL_MASK)
	{
        if ( event->keyval == GDK_KEY_L ) {
            //to do gtk_widget_show(GTK_WIDGET(priv->url_entry));
            //gtk_widget_grab_focus(GTK_WIDGET(priv->url_entry));
            return TRUE;
        }
    }
    return FALSE;
}
static gboolean	command_do_grab_focus(DoView *view)
{
	DoHtmlViewPrivate  *priv;
	priv = DO_HTML_VIEW_GET_PRIVATE (view);
	gtk_widget_grab_focus(priv->view_html);
	return TRUE;
}
static void	command_do_mail_send(DoView *view)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);
    gchar *email;
    GError *error = NULL;
    char *templ;
    char *filename;
    char *name;
    int fd;

    DOMINO_LOCAL_GET("Extended", "email", &email, NULL);
    if ( !priv->html_string )
        return;

    if ( !email ) {
        DOMINO_SHOW_ERROR("Неопределено приложение электронной почты");
        return;
    }

    templ = g_build_filename (g_get_tmp_dir (), "reportXXXXXX", NULL);
    fd = g_mkstemp (templ);
    close (fd);

    filename = g_strconcat (templ, ".html", NULL);
    g_free (templ);
    name = g_locale_from_utf8(filename, -1, NULL, NULL, &error);
    if (!do_save_to_file(name, priv->html_string, strlen(priv->html_string))) {
        DOMINO_SHOW_ERROR("Файл \"%s\" не сохранен", filename);
        return;
    }
#ifdef _WIN32
    gchar *command;
    gchar *cmdline;
    command = g_strconcat (email," ", "/MAIL:",
               "SUBJECT=\"", "Отчет",
               "\";ATTACH=\"", filename, "\"", NULL);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char *argv0 = NULL;

    memset (&si, 0, sizeof (si));
    si.cb = sizeof (&si);
    memset (&pi, 0, sizeof (pi));
    cmdline = g_locale_from_utf8(command, -1, NULL, NULL, &error);

    if (!CreateProcess (argv0, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        DOMINO_SHOW_ERROR("Не удалось запустить программу: %s", g_win32_error_message (GetLastError ()));

    g_free(command);
    g_free(cmdline);
    CloseHandle (pi.hThread);
#else
        /*gchar *argv[3] = {pdf_reader, temp, NULL};
        if (!g_spawn_sync(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL, NULL, &error)) {
            do_message_dialog_show (GTK_WIDGET(edit), GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Не удалось запустить программу: %s", error->message);
            g_error_free (error);
        }*/
#endif
}
static gboolean	command_can_do_mail_send(DoView *view)
{
    gchar *email;
    DOMINO_LOCAL_GET("Extended", "email", &email, NULL);

    return email != NULL;
}
GtkWidget *do_html_get_view(DoHtmlView *view)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);
	return priv->view_html;
}
static gint view_get_load_progress(DoView *view)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);
	return priv->load_progress;
}
void do_html_set_entry(DoHtmlView *view, GtkWidget *entry)
{
	DoHtmlViewPrivate *priv = DO_HTML_VIEW_GET_PRIVATE (view);
	return priv->load_progress_bar = entry;

}
