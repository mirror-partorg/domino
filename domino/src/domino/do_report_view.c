

#include "do_report_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_view_actions.h"
#include "do_icons.h"
#include "do_html_view.h"
#include "do_window.h"
#include "do_print.h"
#include "do_notebook.h"
#include "domino.h"
#include "../misc/template.h"
#include <string.h>
//#include <unistd.h>

#define DO_REPORT_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_REPORT_VIEW, DoReportViewPrivate))

enum
{
	PROP_0,
	PROP_TITLE,
};


// View interface
static void do_report_view_view_init(DoViewIface *iface);
static const char  *do_report_view_get_title(DoView *embed);
static GdkPixbuf   *do_report_view_get_icon(DoView *embed);
static gboolean     do_report_view_do_grab_focus(DoView *view);
static void         do_report_view_do_close(DoView *view);
static void         do_report_view_do_print(DoView *view);
static gboolean     do_report_view_can_do_print(DoView *view);
static gboolean do_report_view_can_do_save_as(DoView *view);
static void do_report_view_do_save_as(DoView *view);
static void	do_report_view_do_mail_send(DoView *view);
static gboolean     do_report_view_can_do_mail_send(DoView *view);


struct _DoReportViewPrivate
{
    GdkPixbuf    *icon;
    gchar        *title;
    GtkWidget    *header;
};

G_DEFINE_TYPE_WITH_CODE (DoReportView, do_report_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_report_view_view_init)
                            );

static void do_report_view_view_init(DoViewIface *iface)
{
    iface->do_close        = do_report_view_do_close;
    iface->get_title       = do_report_view_get_title;
    iface->get_icon        = do_report_view_get_icon;
    iface->do_grab_focus   = do_report_view_do_grab_focus;
    iface->do_print        = do_report_view_do_print;
    iface->do_print_now    = do_report_view_do_print;
    iface->can_do_print    = do_report_view_can_do_print;
    iface->can_do_save_as  = do_report_view_can_do_save_as;
    iface->do_save_as      = do_report_view_do_save_as;
    iface->do_mail_send    = do_report_view_do_mail_send;
    iface->can_do_mail_send  = do_report_view_can_do_mail_send;
}
static void do_report_view_init(DoReportView *temp)
{
#if GTK_MAJOR_VERSION == 2
	DoReportViewPrivate *priv = DO_REPORT_VIEW_GET_PRIVATE (temp);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_FILE, DO_VIEW_ICON_SIZE, 0, NULL);
#endif
}

static GObject *do_report_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
	DoReportViewPrivate *priv;


	object = G_OBJECT_CLASS (do_report_view_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv = DO_REPORT_VIEW_GET_PRIVATE (object);

	priv->header = gtk_label_new("");
	g_object_set(priv->header, "xalign", 0.01, NULL);
	gtk_label_set_line_wrap(GTK_LABEL(priv->header), TRUE);


	do_tree_view_pack_header(DO_TREE_VIEW(object), priv->header);
	gtk_widget_show(priv->header);

	return object;
}

static void do_report_view_finalize (GObject *object)
{
	DoReportViewPrivate *priv = DO_REPORT_VIEW_GET_PRIVATE (object);
	if ( priv->title )
        g_free(priv->title);
	G_OBJECT_CLASS (do_report_view_parent_class)->finalize (object);
}

static void do_report_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoReportViewPrivate *priv = DO_REPORT_VIEW_GET_PRIVATE (object);
	/* no readable properties */
	switch (prop_id)
	{
        case PROP_TITLE:
            g_value_set_string(value, priv->title);
            break;
        default :
            g_assert_not_reached ();
	}
}

static void do_report_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoReportViewPrivate *priv = DO_REPORT_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
        case PROP_TITLE:
            if (priv->title)
                g_free(priv->title);
            priv->title = g_value_dup_string(value);
            gtk_label_set_markup(GTK_LABEL(priv->header), priv->title);
            break;
	}
}

static void do_report_view_class_init (DoReportViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_report_view_constructor;
	object_class->finalize     = do_report_view_finalize;
	object_class->get_property = do_report_view_get_property;
	object_class->set_property = do_report_view_set_property;

	g_type_class_add_private (object_class, sizeof (DoReportViewPrivate));

    g_object_class_install_property
        (object_class,
         PROP_TITLE,
         g_param_spec_string("title",
                      "",
                      "",
                      NULL,
                       G_PARAM_READWRITE));
}

GtkWidget *do_report_view_new ()
{
	return g_object_new (DO_TYPE_REPORT_VIEW,
			     (gpointer) NULL);
}

static void do_report_view_do_close(DoView *embed)
{
    gtk_widget_destroy(GTK_WIDGET(embed));
}
static const char *do_report_view_get_title(DoView *embed)
{
	DoReportViewPrivate  *priv;
	priv = DO_REPORT_VIEW_GET_PRIVATE (embed);

    return priv->title;
}
static GdkPixbuf *do_report_view_get_icon(DoView *embed)
{
	DoReportViewPrivate  *priv;
	priv = DO_REPORT_VIEW_GET_PRIVATE (embed);

    return priv->icon;
}
static gboolean	do_report_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}

static gboolean do_report_view_can_do_print(DoView *view)
{
    GtkTreeModel *model;
    g_object_get(view, "model", &model, NULL);
    return DO_IS_PRINT(model);
}
static void  do_report_view_do_print(DoView *view)
{
    GtkTreeModel *model;
    g_object_get(view, "model", &model, NULL);
    if ( DO_IS_PRINT(model) ) {
        gchar *html_string;
        GtkWidget *html;
        GtkWidget *notebook;
        GtkWidget  *window;
        html_string = do_print_get_html(DO_PRINT(model));
        if (html_string) {
#ifndef MINIMAL_WIN
            window = gtk_widget_get_toplevel(GTK_WIDGET(view));
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
}
static gboolean do_report_view_can_do_save_as(DoView *view)
{
    GtkTreeModel *model;
    g_object_get(view, "model", &model, NULL);
    return DO_IS_PRINT(model);

}
static gboolean do_report_view_can_do_mail_send(DoView *view)
{
    GtkTreeModel *model;
    g_object_get(view, "model", &model, NULL);
    return DO_IS_PRINT(model);

}
static void do_report_view_do_save_as(DoView *view)
{
    GtkTreeModel *model;
    g_object_get(view, "model", &model, NULL);
    if ( !DO_IS_PRINT(model) )
        return;
    if (!gtk_widget_get_visible(GTK_WIDGET(view)))
        return;

    gchar *html_string;
    GtkWidget  *window;
    GtkWidget *dialog;
    html_string = do_print_get_html(DO_PRINT(model));
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
        gchar *filename;
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
        if (!do_save_to_file(name, html_string, strlen(html_string))) {
            DOMINO_SHOW_ERROR("Файл \"%s\" не сохранен", filename);
        }
        g_free(name);
        g_free(value);
    }
    gtk_widget_destroy (dialog);
}
static void	 do_report_view_do_mail_send(DoView *view)
{
	//DoReportViewPrivate  *priv;
	//priv = DO_REPORT_VIEW_GET_PRIVATE (view);
    gchar *email;
    GError *error = NULL;
    char *templ;
    char *filename;
    char *name;
    int fd;

    DOMINO_LOCAL_GET("Extended", "email", &email, NULL);
    if ( !email ) {
        DOMINO_SHOW_ERROR("Неопределено приложение электронной почты");
        return;
    }
    GtkTreeModel *model;
    g_object_get(view, "model", &model, NULL);
    if ( !DO_IS_PRINT(model) )
        return;
    if (!gtk_widget_get_visible (GTK_WIDGET(view)))
        return;

    gchar *html_string;
    html_string = do_print_get_html(DO_PRINT(model));

    templ = g_build_filename (g_get_tmp_dir (), "reportXXXXXX", NULL);
    fd = g_mkstemp (templ);
    close (fd);

    filename = g_strconcat (templ, ".html", NULL);
    g_free (templ);
    name = g_locale_from_utf8(filename, -1, NULL, NULL, &error);
    if (!do_save_to_file(name, html_string, strlen(html_string))) {
        DOMINO_SHOW_ERROR("Файл \"%s\" не сохранен", filename);
        return;
    }
#ifdef _WIN32
    gchar *command;
    gchar *cmdline;
    command = g_strconcat (email," ", "mailto:",
			       "?Subject=\"", "Отчет",
			       "\"&Body=\"", "Замените текст письма и добавте файл ",
			       "<link>", filename ,"</link>", "\"", NULL);

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
