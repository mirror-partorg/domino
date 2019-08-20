
#include "do_message.h"
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <dolib.h>
#include "do_stock.h"
#include "misc.h"


typedef struct _Message Message;
struct _Message {
    gchar *message;
    GLogLevelFlags log_flags;
};
static GThread* main_thread = NULL;
static GMutex *mutex = NULL;
static GList *messages = NULL;
static gboolean show_message = FALSE;
static void do_register_icons();

void do_message_init()
{
    main_thread = g_thread_self();
    mutex = g_mutex_new();
    do_register_icons();
}


void do_register_icons()
{
    GtkIconFactory * factory = gtk_icon_factory_new();
    GtkIconTheme *   theme = gtk_icon_theme_get_default( );

    gtk_icon_factory_add_default( factory );

    const char * name = "domino";

    if( !gtk_icon_theme_has_icon( theme, name ) )
    {
        int          width;
        GdkPixbuf *  p;

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_apps_48x48, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_apps_32x32, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_apps_16x16, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

    }
    g_object_unref ( G_OBJECT ( factory ) );
}

static void do_messages_show()
{
    if ( main_thread != g_thread_self() ) {
        fprintf(stderr, "Not main thread!!!!!!!!!!\n");//!!
        return;
    }


    if ( show_message )
        return;

    show_message = TRUE;
    GList *l;
    l = messages;
    while ( l ) {
        GtkMessageType m_type;
        GtkButtonsType m_but;
        Message *msg;
        gchar *str;

        msg = l->data;
        m_type = GTK_MESSAGE_OTHER;
        m_but = GTK_BUTTONS_OK;

        if ( msg->log_flags & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL ) ) {
            m_but = GTK_BUTTONS_OK;
            m_type = GTK_MESSAGE_ERROR;
        };
        if ( msg->log_flags & G_LOG_LEVEL_WARNING ) {
            m_but = GTK_BUTTONS_YES_NO;
            m_type = GTK_MESSAGE_WARNING;
        };
        if ( msg->log_flags & G_LOG_LEVEL_INFO ) {
            m_but = GTK_BUTTONS_OK;
            m_type = GTK_MESSAGE_INFO;
        };
        if ( m_but == GTK_BUTTONS_YES_NO )
            str = g_strdup_printf("%s.\nЗавершить работу?", msg->message);
        else
            str = g_strdup_printf("%s", msg->message);

        GtkWidget *dialog = gtk_message_dialog_new (NULL,
                          GTK_DIALOG_DESTROY_WITH_PARENT,
                          m_type,
                          m_but,
                          "%s", str);
        g_free(str);
        gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ALWAYS);
        gtk_window_set_icon_name(GTK_WINDOW(dialog), "domino");
        gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
        //gtk_window_set_title(GTK_WINDOW(dialog), "Ошибка");
        //if ( gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES && m_but == GTK_BUTTONS_YES_NO )
        gtk_dialog_run (GTK_DIALOG (dialog));
        if ( msg->log_flags & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL ) )
            exit(1);

        gtk_widget_destroy (dialog);
        g_free(msg->message);
        g_free(msg);
        l = l->next;
    }
    g_mutex_lock(mutex);
    g_list_free(messages);
    messages = NULL;
    g_mutex_unlock(mutex);
    show_message = FALSE;
}

void do_message_log(const gchar *message, GLogLevelFlags log_flags, void *user_data)
{

#ifndef G_OS_WIN32
    if ( log_flags & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING ) )
        fprintf(stderr, "%s\n", message);
    else
        fprintf(stdout, "%s\n", message);
#endif
    if ( !(
           log_flags &
           (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO )
          )
        )
        return;
    g_mutex_lock(mutex);
    Message *msg;
    msg = g_new0(Message, 1);
    msg->message = g_strdup(message);
    msg->log_flags = log_flags;
    messages = g_list_append(messages, msg);
    g_mutex_unlock(mutex);
}
void do_message_log_gtk_func(const gchar *log_domain, GLogLevelFlags log_flags, const gchar *msg, gpointer user_data)
{
    do_message_log(msg, log_flags, NULL);
}
int do_message_log_do_func(const char *mgs, int level, void *user_data)
{
    GLogLevelFlags log_flags = 0;
    if ( level == LOG_ERR )
        log_flags = G_LOG_LEVEL_ERROR;
    else
        log_flags = G_LOG_LEVEL_MASK;

    do_message_log(mgs, log_flags, user_data);
    return level;
}
static gboolean idle(gboolean *event)
{
    do_messages_show();
    *event = FALSE;
    return FALSE;
}
void do_message_show_all()
{
    if ( main_thread != g_thread_self() ) {
        gboolean event = TRUE;
        gdk_threads_add_idle((GSourceFunc)idle, &event);
        wait_for(&event);
    }
    else
        do_messages_show();
}
static gboolean show_question(const gchar *msg)
{
    GtkWidget *w;
    gint res;

    w = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", msg);
    gtk_window_set_icon_name(GTK_WINDOW(w), "domino");
    gtk_window_set_position(GTK_WINDOW(w), GTK_WIN_POS_CENTER);
    res = gtk_dialog_run(GTK_DIALOG(w));
    gtk_widget_destroy(w);
    return res == GTK_RESPONSE_YES;
}
typedef struct {
    const gchar *msg;
    gboolean event;
    gboolean res;
} idle_msg_t;

static gboolean idle_question(idle_msg_t *m)
{
    m->res = show_question(m->msg);
    m->event = FALSE;
    return FALSE;
}
gboolean do_message_question(const gchar *msg)
{
    if ( main_thread != g_thread_self() ) {
        idle_msg_t m;
        m.event = TRUE;
        m.msg = msg;
        gdk_threads_add_idle((GSourceFunc)idle_question, &m);
	wait_for(&m.event);
        return m.res;

    }
    else
        return show_question(msg);
}
