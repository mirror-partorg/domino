#include <stdlib.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#ifdef _LINUX
#include <signal.h>
//#if GTK_MAJOR_VERSION > 2 || GTK_MINOR_VERSION > 20
#include <glib-unix.h>
//#endif
#endif

//#include "version.h"

#include "../misc/send_error.h"

#include "domino.h"
#ifdef USE_EVINCE
#include <evince-view.h>
#endif
#include <gtk/gtk.h>
#include <dolib.h>
#include <locale.h>

/* command line */
static gchar *user_name = NULL;
static gchar *unit = NULL;
static gchar *alias_name = NULL;
static gchar *user_password = NULL;
static gchar **remaining_args = NULL;
static gboolean fullscreen = FALSE;
static gboolean hidecontrol = FALSE;
#ifdef _WIN32
static gint show_help;
#endif
#ifdef _LINUX
#endif


static const GOptionEntry options [] =
{
    { "user", 'u', 0, G_OPTION_ARG_STRING, &user_name,
      "Установать имя пользователя", "Пользователь"},

    { "unit", 'U', 0, G_OPTION_ARG_STRING, &unit,
      "Установать подразделение поумолчанию", "Подразделние"},

    { "alias", 'a', 0, G_OPTION_ARG_STRING, &alias_name,
      "Установать базу поумолчанию", "База"},

    { "password", 'p', 0, G_OPTION_ARG_STRING, &user_password,
      "Установать пароль пользователя", "Пароль"},

    { "fullscreen", '\0', 0, G_OPTION_ARG_NONE, &fullscreen,
      "Полноэкранный режим", "Полноэкранный"},

    { "hidecontrol", '\0', 0, G_OPTION_ARG_NONE, &hidecontrol,
      "Скрыть меню и все панели ", "Скрыть управление"},
#ifdef _WIN32
    { "help", 'h', 0, G_OPTION_ARG_NONE, &show_help,
      "Просмотр доступных опций", "Помощь"},
#endif
    { G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_STRING_ARRAY, &remaining_args,
      "Автозапуск ", "[Команды...]" },
    {NULL}
};
DoWindow *win = NULL;

static void init_main_win(DoWindow *win)
{
    if ( fullscreen )
        gtk_window_fullscreen(GTK_WINDOW(win));
    if ( hidecontrol ) {
        GtkWidget *w;
        w = do_window_get_mainmenu(DO_WINDOW(win));
        if ( w )
            gtk_widget_set_visible(w, FALSE);
        w = do_window_get_statusbar(DO_WINDOW(win));
        if ( w )
            gtk_widget_set_visible(w, FALSE);
        w = do_window_get_toolbar(DO_WINDOW(win));
        if ( w )
            gtk_widget_set_visible(w, FALSE);
        w = do_window_get_notebook(win);
        g_object_set(w, "show-tabs", FALSE, NULL);
    }
#ifdef CURSOR_OFF
    GdkCursor *cursor = gdk_cursor_new(GDK_BLANK_CURSOR);
    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(win)), cursor);
#endif
}
/*static void poppler_error(void *data, ErrorCategory category, int pos, char *msg)
{
}
*/

static gboolean auth (GtkWidget* win)
{
//    GDK_THREADS_ENTER();

    gboolean auth;
    auth = user_name && user_password && domino_auth(user_name, user_password) == 1;

    if (!auth) {
        GtkWidget *dialog;
        GtkWindow *top;
        int is_dialog;
        top = domino_get_top_window(&is_dialog);
        dialog = do_password_dialog_new(GTK_WIDGET(top), "Авторизация", DO_PASSWORD_DIALOG_FLAGS_DEFAULT);
	gtk_window_set_destroy_with_parent (GTK_WINDOW (dialog), TRUE);
        DoPassword *password;
        password = do_password_get(dialog, NULL, do_domino_auth, dialog);
        if (password) {
            auth = TRUE;
            g_object_unref(password);
        }
        gtk_widget_destroy(dialog);
    }
    if (auth) {
        gchar *ui;
        gsize length;
        if (domino_get_ui(&ui, &length))
            do_window_add_ui_from_string(DO_WINDOW(win), ui, length);
        else {
            DOMINO_SHOW_ERROR("Шаблон меню пользователя не найден");
            auth = FALSE;
        }
    }
    if (auth) {
        gint i;
        GtkUIManager *manager;
        manager = GTK_UI_MANAGER(do_window_get_ui_manager(DO_WINDOW(win)));
        for (i = 0; remaining_args && remaining_args[i]; i++) {
            GtkAction *action;
            action = gtk_ui_manager_get_action(manager, remaining_args[i]);
            if (action)
                gtk_action_activate(action);
        }
        auth = TRUE;
    }
    if ( !auth )
  		gtk_widget_destroy (GTK_WIDGET(win));
    else {
        init_main_win(DO_WINDOW(win));
#ifdef USE_IM
//!!    domino_im_init();
#endif
        //do_windows_set_toolbar_markup(DO_WINDOW(win), "<b><span foreground=\"red\">Чампикс</span> - УМЕНЬШАЕТ тягу к курению И СМЯГЧАЕТ СИМПТОМЫ ОТМЕНЫ </b><i>(Назначается доктором-отпускается по рецепту)</i>");
    }

//    GDK_THREADS_LEAVE();
    return FALSE;
}
#ifdef _LINUX
#if GTK_MAJOR_VERSION > 2 || GTK_MINOR_VERSION > 20
static gboolean sig_restart(gpointer signo) {
    do_log(LOG_INFO, "restart");
    restart_programm(NULL);
    return FALSE;
}
#ifdef ADS_RECOMMEND
static gboolean sig_recom(gpointer signo) {
    do_log(LOG_INFO, "recommend changed");
    GtkWidget *note;
    note=do_window_get_notebook(win);
    if ( note ) {
        gint i;
        for ( i = 0; i < gtk_notebook_get_n_pages(GTK_NOTEBOOK(note)); i++ ) {
            GtkWidget *page;
            page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(note), i);
            if ( DO_IS_PRODUCT_VIEW(page) )
                do_product_view_refill_recommend(DO_PRODUCT_VIEW(page) );
        }
    }
    return TRUE;
}
#endif
#else
static void sig_restart(int signo) {
    do_log(LOG_INFO, "restart");
    restart_programm(NULL);
}
#endif

//#if GTK_MAJOR_VERSION > 2 || GTK_MINOR_VERSION > 20
static void install_signal_handlers(void) {
//    sigset_t sig_set;

    //fprintf(stderr, "install sygnal\n");

  /*  sigemptyset(&sig_set);

    sigaddset(&sig_set, SIGCHLD);
    sigaddset(&sig_set, SIGINT);
    sigaddset(&sig_set, SIGQUIT);
    sigaddset(&sig_set, SIGILL);
    sigaddset(&sig_set, SIGABRT);
    sigaddset(&sig_set, SIGFPE);
    sigaddset(&sig_set, SIGSEGV);
    sigaddset(&sig_set, SIGALRM);
    sigaddset(&sig_set, SIGTERM);
    sigaddset(&sig_set, SIGHUP);
    sigaddset(&sig_set, SIGUSR2);
#ifdef SIGSTKFLT
    sigaddset(&sig_set, SIGSTKFLT);
#endif
#ifdef SIGIO
    sigaddset(&sig_set, SIGIO);
#endif
#ifdef SIGBUS
    sigaddset(&sig_set, SIGBUS);
#endif
*/
  //g_unix_signal_add (SIGUSR1, sig_restart, NULL);
//#if GTK_MAJOR_VERSION > 2 || GTK_MINOR_VERSION > 20
  g_unix_signal_add (SIGHUP, sig_restart, NULL);
#ifdef ADS_RECOMMEND
#if GTK_MAJOR_VERSION > 2 || GTK_MINOR_VERSION > 20
  g_unix_signal_add (SIGUSR1, sig_recom, NULL);
#endif
#endif
  do_log(LOG_WARNING, "set signal\n");
/*#else
  sigset_t sig_set;
  sigemptyset(&sig_set);
  sigaddset(&sig_set, SIGHUP);
  signal(SIGHUP, sig_restart);
#endif*/
  //signal(SIGHUP, sig_restart);
  //signal(SIGUSR1, sig_restart);

  //sigprocmask(SIG_UNBLOCK, &sig_set, NULL);
}
//#endif
#endif

static void set_style()
{
    char *text = NULL;
    domino_config_property_get(DOMINO_CONFIG_LOCAL,"Style", "default", &text, NULL);
    if ( text && !strcmp(text,"1" ) ) {
        domino_config_property_get(DOMINO_CONFIG_LOCAL,"Style", "font", &text, NULL);
        if ( text )
            domino_font_style_apply(text);
    }
}

void log_gtk_func(const gchar *log_domain, GLogLevelFlags log_flags, const gchar *msg, gpointer user_data)
{
    do_log(LOG_ALERT, msg);
}
int main (int argc, char *argv[])
{

    GError *error = NULL;
    do_log_set_stdout();

#ifdef DEBUG
    do_log_set_stdout();
#endif
    do_log_set_type("Domino", DO_LOG_TYPE_SYSLOG, "");
    //do_log_set_type("Domino", DO_LOG_TYPE_FILE, "/usr/local/domino/log/domino.log");

#ifdef _WIN32
//    install_event_message("domino");
#endif

#ifdef _LINUX
//#if GTK_MAJOR_VERSION > 2 || GTK_MINOR_VERSION > 20
    install_signal_handlers();
//#endif
#endif

    do_log_set_stdout();//!!

  /* Initialize GTK+ */

    setlocale(LC_ALL, "");

    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    //g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, log_gtk_func, NULL);
    //do_set_main_thread();
    do_log_set_log_func(do_message_log_do_func, NULL);
    do_message_init();

    if ( !DOMINO_INIT ) {
        g_timeout_add_seconds(5, (GSourceFunc)gtk_main_quit, NULL);
        gtk_main ();
        return 1;
    }
    //PYTHON_INIT();

    GOptionContext *context;
    context = g_option_context_new(" - Домино");
    g_option_context_add_main_entries (context, options, NULL);
    g_option_context_add_group (context, gtk_get_option_group (FALSE));
#ifdef _WIN32
    g_option_context_set_help_enabled(context, FALSE);
#endif
    update_action_set_arg(argv, argc);
    if (!g_option_context_parse (context, &argc, &argv, &error)) {
       DOMINO_SHOW_INFO("%s\nЗапустите '%s --help' для просмотра доступных опций.\n",
                        error->message, argv[0]);
        g_error_free (error);
        return 1;
    }
    if ( unit )
        domino_set_unit(unit);
    if ( alias_name )
        domino_set_alias_name(alias_name);
#ifdef _WIN32
    if (show_help) {
        gchar *help;
        help = g_option_context_get_help (context, TRUE, NULL);
        DOMINO_SHOW_ERROR("%s", help);
        g_print ("%s", help);
        g_free (help);
        return 1;
    }
#endif
    //if ( g_thread_supported() )
      // g_thread_init(NULL);
    //gdk_threads_init ();

//    GDK_THREADS_ENTER();
    set_style();

    g_option_context_free (context);
    win = do_window_new ();
    domino_set_main_window(GTK_WINDOW(win));
#if defined(BINARY_UPDATE)
#ifndef DEBUG
    update_action_run(GTK_WIDGET(win), TRUE);
#endif
#endif
#ifdef USE_EVINCE
    if ( !ev_init() ) {
        DOMINO_SHOW_ERROR("Невозможно инициализировать evince");
        return 1;
    }
    ev_stock_icons_init ();
#endif

    g_signal_connect (win, "destroy", gtk_main_quit, NULL);
    gtk_widget_show(GTK_WIDGET(win));
    g_timeout_add(100,  (GSourceFunc)auth, win);
    init_main_win(win);


    gtk_main ();
//    GDK_THREADS_LEAVE();

    DOMINO_FINALIZE();
#ifdef USE_EVINCE
    ev_shutdown ();
	ev_stock_icons_shutdown ();
#endif
   // PYTHON_FINALIZE();

    return 0;
}
