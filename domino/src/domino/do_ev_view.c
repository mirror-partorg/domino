
#ifdef USE_EVINCE

#include "do_ev_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_view_actions.h"
#include "do_icons.h"
#include "domino.h"
#include <evince-view.h>
#include <evince-document.h>
#include <string.h>
#include <unistd.h>
//#include <stdlib.h>
#if GTK_MAJOR_VERSION == 2
#include <gdk/gdkkeysyms.h>
#endif

#define DO_EV_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_EV_VIEW, DoEvViewPrivate))

enum
{
	PROP_0,
	PROP_URI,
	PROP_TITLE,
	PROP_MARGIN,
	PROP_DUPLEX,
	PROP_LOAD_STATUS,
	PROP_LOAD_PROGRESS,

};

//static void focus_in(GtkWidget *widget, GdkEventFocus *event, gpointer user_data);

// View interface
static void do_ev_view_view_init(DoViewIface *iface);

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
static void command_do_print_now(DoView *view);


static gboolean key_press(GtkWidget *widget, GdkEventKey *event, DoView *do_view);


struct _DoEvViewPrivate
{
	GtkWidget       *view;
    EvDocumentModel *model;
    GdkPixbuf    *icon;
    gchar        *title;
    gchar        *html_string;
    gboolean      load_status;
    gint          load_progress;
    gboolean      duplex;
    gboolean      margin;

    GtkPrintOperation *op;
	EvJob             *job_print;
};

G_DEFINE_TYPE_WITH_CODE (DoEvView, do_ev_view, GTK_TYPE_VBOX,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_ev_view_view_init)
                            );

static void do_ev_view_view_init(DoViewIface *iface)
{
    iface->do_close        = view_close;
    iface->close_request   = view_close_request;
    iface->get_title       = view_get_title;
    iface->get_icon        = view_get_icon;
    iface->get_load_status = view_get_load_status;
    iface->get_load_progress = view_get_load_progress;
    iface->do_print        = command_do_print;
    iface->do_print_now    = command_do_print_now;
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

static void do_ev_view_init(DoEvView *temp)
{
	DoEvViewPrivate *priv = DO_EV_VIEW_GET_PRIVATE (temp);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, DO_STOCK_DOMINO_EVINCE, DO_VIEW_ICON_SIZE, 0, NULL);

}

static GObject *do_ev_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject            *object;
	DoEvViewPrivate  *priv;
	GtkVBox            *frame;
	GtkWidget          *s;

	object = G_OBJECT_CLASS (do_ev_view_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv = DO_EV_VIEW_GET_PRIVATE (object);
	frame = GTK_VBOX(object);

    s = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(s), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    priv->view = ev_view_new();
    priv->model = ev_document_model_new();
    ev_view_set_model(EV_VIEW(priv->view), priv->model);

    //g_signal_connect(priv->view,  "focus-in-event",  G_CALLBACK(focus_in), object);
    g_signal_connect(priv->view,  "key-press-event",  G_CALLBACK(key_press), object);/*
    g_signal_connect(priv->view,  "load-started",  G_CALLBACK(do_ev_view_load_started), object);
    g_signal_connect(priv->view,  "load-finished",  G_CALLBACK(do_ev_view_load_finished), object);
    g_signal_connect(priv->view,  "load-progress-changed",  G_CALLBACK(do_ev_view_load_changed), object);
*/

    //gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER (s), priv->view);
    gtk_container_add(GTK_CONTAINER(frame), s);

    gtk_widget_show_all(GTK_WIDGET(priv->view));
    gtk_widget_show(GTK_WIDGET(s));
    gtk_widget_show(GTK_WIDGET(frame));
	return object;
}

static void do_ev_view_finalize (GObject *object)
{
	G_OBJECT_CLASS (do_ev_view_parent_class)->finalize (object);
}
static void do_set_html_string(DoEvView *view, const gchar *string)
{
/*	DoEvViewPrivate *priv = DO_EV_VIEW_GET_PRIVATE (view);
    if (priv->html_string)
        g_free(priv->html_string);
    priv->html_string = g_strdup(string);
    webkit_web_view_load_html_string(WEBKIT_WEB_VIEW(priv->view),
                  priv->html_string, NULL);*/
//    save_to_file("1.html", priv->html_string, strlen(priv->html_string));
    //webkit_web_view_load_uri(WEBKIT_WEB_VIEW(priv->view), "file:///home/mirror/tmp/IP.pdf");
}

static void do_ev_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoEvViewPrivate *priv = DO_EV_VIEW_GET_PRIVATE (object);
	/* no readable properties */
	switch (prop_id)
	{
        case PROP_TITLE:
            g_value_set_string(value, priv->title);
            break;
        case PROP_MARGIN:
            g_value_set_boolean(value, priv->margin);
            break;
        case PROP_DUPLEX:
            g_value_set_boolean(value, priv->duplex);
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

static void do_ev_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoEvViewPrivate *priv = DO_EV_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
        case PROP_URI:
            do_ev_view_open_uri(DO_EV_VIEW(object), g_value_get_string(value));
            break;
        case PROP_TITLE:
            if (priv->title)
                g_free(priv->title);
            priv->title = g_value_dup_string(value);
            break;
        case PROP_MARGIN:
            priv->margin = g_value_get_boolean(value);
            break;
        case PROP_DUPLEX:
            priv->duplex = g_value_get_boolean(value);
            break;
	}
}

static void do_ev_view_class_init (DoEvViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_ev_view_constructor;
	object_class->finalize     = do_ev_view_finalize;
	object_class->get_property = do_ev_view_get_property;
	object_class->set_property = do_ev_view_set_property;

	g_type_class_add_private (object_class, sizeof (DoEvViewPrivate));

    g_object_class_install_property
        (object_class,
         PROP_URI,
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
    g_object_class_install_property (object_class,
				   PROP_MARGIN,
				   g_param_spec_boolean("print-with-margin",
							"Печать с отступами",
							"Печать с отступами",
							FALSE,
							G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_MARGIN,
				   g_param_spec_boolean("print-duplex",
							"Печать с обеих сторон",
							"Печать с обеих сторон",
							FALSE,
							G_PARAM_READWRITE));
}

GtkWidget *do_ev_view_new ()
{
	return g_object_new (DO_TYPE_EV_VIEW,
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
	DoEvViewPrivate  *priv;
	priv = DO_EV_VIEW_GET_PRIVATE (embed);

    return priv->title;
}
static GdkPixbuf    *view_get_icon			(DoView *embed)
{
	DoEvViewPrivate  *priv;
	priv = DO_EV_VIEW_GET_PRIVATE (embed);

    return priv->icon;
}
static gboolean	  view_get_load_status	(DoView *view)
{
	DoEvViewPrivate *priv = DO_EV_VIEW_GET_PRIVATE (view);
	return priv->load_status;
}
static void command_do_print_now(DoView *view)
{
	DoEvViewPrivate  *priv;
	priv = DO_EV_VIEW_GET_PRIVATE (view);

    EvDocument *doc;
    const gchar *uri;


    doc = ev_document_model_get_document(priv->model);
    uri = ev_document_get_uri(doc);

    do_utilx_print_pdf_file(domino_get_main_window(), uri, FALSE, priv->duplex, priv->margin);

    //command_do_print(view);

}
#ifdef USE_EV_PRINT

static void do_ev_view_evince_print(DoView *view)
{
	DoEvViewPrivate  *priv;
	priv = DO_EV_VIEW_GET_PRIVATE (view);

	GtkWindow *window;
	EvPrintOperation *oper;
	EvDocument       *doc;
    //GKeyFile         *print_settings_file;
	GtkPrintSettings *print_settings;
	GtkPageSetup     *print_page_setup;
	gint              current_page;
	//gint              document_last_page;

    doc = ev_document_model_get_document(priv->model);
    if ( !doc )
        return;
    oper = ev_print_operation_new(doc);
	if ( !oper ) {
        DOMINO_SHOW_INFO("Печать не поддерживается");
        return;
	}

	window = domino_get_main_window();

	print_settings = gtk_print_settings_new ();
	gtk_print_settings_set_int(print_settings, "evince-print-setting-page-scale", 0);

	print_page_setup = do_utils_get_default_page_setup();

    current_page = ev_document_model_get_page (priv->model);
	//document_last_page = ev_document_get_n_pages (doc);
	ev_print_operation_set_job_name (oper, gtk_window_get_title (window));
	ev_print_operation_set_current_page (oper, current_page);
	ev_print_operation_set_print_settings(oper, print_settings);
	ev_print_operation_set_default_page_setup(oper, print_page_setup);
	ev_print_operation_set_embed_page_setup (oper, TRUE);
	ev_print_operation_run (oper, window);
}
#endif
static void command_do_print(DoView *view)
{
#ifdef USE_EV_PRINT
    do_ev_view_evince_print(view);
#else
	DoEvViewPrivate  *priv;
	priv = DO_EV_VIEW_GET_PRIVATE (view);

    EvDocument *doc;
    const gchar *uri;


    doc = ev_document_model_get_document(priv->model);
    uri = ev_document_get_uri(doc);

    do_utilx_print_pdf_file(domino_get_main_window(), uri, TRUE, priv->duplex, priv->margin);
#endif

}
static gboolean	command_can_do_print(DoView *view)
{
    return TRUE;
}

static void command_do_save_as(DoView *view)
{
	DoEvViewPrivate  *priv;
	priv = DO_EV_VIEW_GET_PRIVATE (view);
    GtkWidget* dialog;
    GtkWidget* window;
    gchar* filename;

    if ( !gtk_widget_get_visible(GTK_WIDGET(view)) )
        return;
    window = gtk_widget_get_toplevel(GTK_WIDGET(view));

    dialog = gtk_file_chooser_dialog_new (
        "Сохранить как", GTK_WINDOW( window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
        NULL);
    gtk_window_set_icon_name (GTK_WINDOW (dialog), GTK_STOCK_SAVE);
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
	//DoEvViewPrivate  *priv;
	//priv = DO_EV_VIEW_GET_PRIVATE (view);
    GtkWidget* dialog;
    GtkWidget* window;
    gchar* filename;

    if ( !gtk_widget_get_visible(GTK_WIDGET(view)) )
        return;
    window = gtk_widget_get_toplevel(GTK_WIDGET(view));

    dialog = gtk_file_chooser_dialog_new (
        "Открыть файл", GTK_WINDOW( window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
        NULL);
    gtk_window_set_icon_name (GTK_WINDOW (dialog), GTK_STOCK_OPEN);
    gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        gchar *text;
        if ((text = do_read_from_file(filename)) == NULL) {
            DOMINO_SHOW_ERROR("Файл \"%s\" не открывается", filename);
        }
        else {
            do_set_html_string(DO_EV_VIEW(view), text);
            g_free(text);
        }
    }
    gtk_widget_destroy (dialog);
}

static gboolean	command_can_do_save_as(DoView *view)
{
    return TRUE;
}

/*
static void focus_in(GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
    do_view_actions_set_view(user_data);
}
*/
static gboolean key_press(GtkWidget *widget, GdkEventKey *event, DoView *do_view)
{
	guint mask = gtk_accelerator_get_default_mod_mask ();
	if ((event->state & mask) == 0)
	{
		switch (event->keyval)
		{
            case GDK_KEY_Escape:
                view_close(do_view);
            	return TRUE;
		default:
			break;
		}
	}
    return FALSE;
}

static gboolean	command_do_grab_focus(DoView *view)
{
	DoEvViewPrivate  *priv;
	priv = DO_EV_VIEW_GET_PRIVATE (view);
	gtk_widget_grab_focus(priv->view);
	return TRUE;
}
static void	command_do_mail_send(DoView *view)
{
	DoEvViewPrivate *priv = DO_EV_VIEW_GET_PRIVATE (view);
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
        DOMINO_SHOW_INFO("Неопределено приложение электронной почты");
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
GtkWidget *do_ev_view_get_view(DoEvView *view)
{
	DoEvViewPrivate *priv = DO_EV_VIEW_GET_PRIVATE (view);
	return priv->view;
}
static gint view_get_load_progress(DoView *view)
{
	DoEvViewPrivate *priv = DO_EV_VIEW_GET_PRIVATE (view);
	return priv->load_progress;
}
static void do_ev_viewer_load_job_finished (EvJob           *job,
				EvDocumentModel *model)
{
	if (ev_job_is_failed (job)) {
		g_object_unref (job);
		return;
	}
	ev_document_model_set_document (model, job->document);
	g_object_unref (job);
}

void do_ev_view_open_uri(DoEvView *view, const gchar *uri)
{
	DoEvViewPrivate *priv = DO_EV_VIEW_GET_PRIVATE (view);

	EvJob *job;

	job = ev_job_load_new (uri);
	g_signal_connect (job, "finished",
			  G_CALLBACK (do_ev_viewer_load_job_finished),
			  priv->model);
	ev_job_scheduler_push_job (job, EV_JOB_PRIORITY_NONE);

}
#endif
