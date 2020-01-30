
#include "do_message.h"
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <dolib.h>
#include "domino.h"
#include "do_application.h"


typedef struct _Message Message;
struct _Message {
    gchar *message;
    GLogLevelFlags log_flags;
};
//static GThread* main_thread = NULL;
//static GMutex *mutex = NULL;
static GList *messages = NULL;
//static gboolean show_message = FALSE;
//static void do_register_icons();
//static Gtk
GtkWidget *dialog = NULL;

//void do_message_init()
//{
    //main_thread = g_thread_self();
//    mutex = g_mutex_new();
    //do_register_icons();
//}

/*
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
*/
static gboolean dialog_destroy_event(GtkWidget       *widget,
               gpointer         user_data)
{
    //do_log(LOG_INFO, "delete event");
    dialog = NULL;
    return FALSE;
}
static void button_ok_clicked(GtkWidget       *button,
               gpointer         user_data)
{
    do_log(LOG_INFO, "button ok");
    gtk_widget_destroy(dialog);
    //dialog = NULL;
}
static void button_exit_clicked (GtkButton *button,
               gpointer   user_data)
{
    exit(1);
}
#ifndef GDK_KEY_Escape
#define GDK_KEY_Escape GDK_Escape
#endif
static gboolean dialog_key_press(GtkWidget *widget, GdkEventKey *event, GtkWidget *win)
{
    guint mask = gtk_accelerator_get_default_mod_mask ();
    if ( (event->state & mask) == 0 )
    {
    	switch (event->keyval)
    	{
            case GDK_KEY_Escape:
                gtk_widget_destroy(win);
                return TRUE;
            default:
                return FALSE;
        }
    }
    return FALSE;
}
static void do_messages_show()
{
    //if ( main_thread != g_thread_self() ) {
    //    fprintf(stderr, "Not main thread!!!!!!!!!!\n");//!!
    //    return;
    //}


    //if ( show_message )
    //    return;

    //show_message = TRUE;
    GList *l;
    l = messages;
    while ( l ) {
        GtkMessageType m_type;
        GtkButtonsType m_but;
        Message *msg;
        gchar *str;

        msg = l->data;

        if ( !dialog ) {
            GtkWindow *topwin;
            int isdialog;

            topwin = do_application_get_window(do_application_get_default(), &isdialog);
            if ( !topwin || isdialog ) {
                m_type = GTK_MESSAGE_OTHER;
                m_but = GTK_BUTTONS_OK;
                if ( msg->log_flags & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL ) )
                    str = g_strdup_printf("%s.\nЗавершить работу?", msg->message);
                else
                    str = g_strdup_printf("%s", msg->message);

                if ( msg->log_flags & G_LOG_LEVEL_WARNING ) {
                    m_but = GTK_BUTTONS_OK;
                    m_type = GTK_MESSAGE_WARNING;
                };
                if ( msg->log_flags & G_LOG_LEVEL_INFO ) {
                    m_but = GTK_BUTTONS_OK;
                    m_type = GTK_MESSAGE_INFO;
                };
                if ( msg->log_flags & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL ) ) {
                    m_but = GTK_BUTTONS_YES_NO;
                    m_type = GTK_MESSAGE_ERROR;
                };
#ifdef gtk_message_dialog_new
#undef gtk_message_dialog_new
#endif // gtk_message_dialog_new
                dialog = gtk_message_dialog_new(topwin,
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  m_type,
                                  m_but,
                                  "%s", str);
#define gtk_message_dialog_new dead

                g_free(str);
                gtk_window_set_icon_name(GTK_WINDOW(dialog), "bookcase");
                gint res;
                res = gtk_dialog_run (GTK_DIALOG (dialog));
                if ( msg->log_flags & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL ) && res == GTK_RESPONSE_YES) {
                    do_log(LOG_INFO, "close domino");
                    exit(1);
                }
                gtk_widget_destroy(dialog);
                dialog = NULL;

            }
            else {
                GtkWidget *vbox;
                GtkWidget *box;
                GtkWidget *w = NULL;
                GtkWidget *button = NULL;

                dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
                g_signal_connect (dialog, "key-press-event", G_CALLBACK (dialog_key_press), dialog);
                gtk_window_set_transient_for(GTK_WINDOW(dialog), topwin);
                gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
                gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ALWAYS);
                gtk_window_set_icon_name(GTK_WINDOW(dialog), "bookcase");
                g_signal_connect (dialog, "destroy", G_CALLBACK(dialog_destroy_event), NULL);
                vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
                gtk_container_set_border_width(GTK_CONTAINER(vbox), 6);
                gtk_container_add(GTK_CONTAINER(dialog), vbox);
                box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
                gtk_box_pack_start(GTK_BOX(vbox), box, FALSE, FALSE, 0);
                if ( msg->log_flags & G_LOG_LEVEL_WARNING ) {
                    w = gtk_image_new_from_icon_name("dialog-warning-symbolic", GTK_ICON_SIZE_DIALOG);
                };
                if ( msg->log_flags & G_LOG_LEVEL_INFO ) {
                    w = gtk_image_new_from_icon_name("dialog-information-symbolic", GTK_ICON_SIZE_DIALOG);
                };
                if ( msg->log_flags & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL ) ) {
                    w = gtk_image_new_from_icon_name("dialog-error-symbolic", GTK_ICON_SIZE_DIALOG);
                    button = gtk_button_new();
                    gtk_button_set_label(GTK_BUTTON(button), "Завершить работу");
                    g_signal_connect(button, "clicked", G_CALLBACK(button_exit_clicked), dialog);
                };
                if ( w )
                    gtk_box_pack_start(GTK_BOX(box), w, FALSE, FALSE, 0);
                w = gtk_label_new("");
                gtk_label_set_markup(GTK_LABEL(w), msg->message);
                gtk_label_set_line_wrap(GTK_LABEL(w), TRUE);
                gtk_label_set_line_wrap_mode(GTK_LABEL(w), GTK_WRAP_WORD);
                gtk_label_set_selectable(GTK_LABEL(w), TRUE);
                gtk_widget_set_can_focus (w, TRUE);
                gtk_box_pack_start(GTK_BOX(box), w, TRUE, TRUE, 0);
                box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
                gtk_box_pack_start(GTK_BOX(vbox), box, FALSE, FALSE, 0);
                w = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
                gtk_box_set_homogeneous(GTK_BOX(w), TRUE);
                gtk_box_pack_end(GTK_BOX(box), w, FALSE, FALSE, 0);
                if ( button )
                    gtk_box_pack_start(GTK_BOX(w), button, FALSE, FALSE, 0);
                button = gtk_button_new();
                if ( msg->log_flags & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL ) )
                    gtk_button_set_label(GTK_BUTTON(button), "Продолжить");
                else
                    gtk_button_set_label(GTK_BUTTON(button), "OK");
                g_signal_connect(button, "clicked", G_CALLBACK(button_ok_clicked), dialog);
                gtk_box_pack_start(GTK_BOX(w), button, TRUE, TRUE, 0);
                gtk_widget_grab_focus(button);
                gtk_widget_show_all(dialog);
            }
        }

        //!!gtk_widget_destroy (dialog);
        g_free(msg->message);
        g_free(msg);
        l = l->next;
    }
//    g_mutex_lock(mutex);
    g_list_free(messages);
    messages = NULL;
//    g_mutex_unlock(mutex);
    //show_message = FALSE;
}

void do_message_log(const gchar *message, GLogLevelFlags log_flags, void *user_data)
{

    do_log(LOG_INFO, "message log '%s'", message);
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
//    g_mutex_lock(mutex);
    Message *msg;
    msg = g_new0(Message, 1);
    msg->message = g_strdup(message);
    msg->log_flags = log_flags;
    messages = g_list_append(messages, msg);
//    g_mutex_unlock(mutex);
    //if ( main_thread == g_thread_self() ) {
        do_message_show_all();
    //}
}
void do_message_log_gtk_func(const gchar *log_domain, GLogLevelFlags log_flags, const gchar *msg, gpointer user_data)
{
    do_message_log(msg, log_flags, NULL);
}
int do_message_log_do_func(const char *mgs, int level, void *user_data)
{
    if ( level != LOG_ERR )
        return 0;
    do_message_log(mgs, G_LOG_LEVEL_ERROR, user_data);
    return level;
}
/*
static gboolean idle(gboolean *event)
{
    do_messages_show();
    *event = FALSE;
    return FALSE;
}*/
void wait_for(gboolean *event)
{
    while ( *event ) g_usleep(5000);
}
void do_message_show_all()
{
    /*if ( main_thread != g_thread_self() ) {
        gboolean event = TRUE;
        gdk_threads_add_idle((GSourceFunc)idle, &event);
        wait_for(&event);
    }
    else*/
        do_messages_show();
}
static gboolean show_question(const gchar *msg)
{
    GtkWidget *w;
    gint res;

#ifdef gtk_message_dialog_new
#undef gtk_message_dialog_new
#endif // gtk_message_dialog_new
    GtkWindow *win;
    int i;
    win = do_application_get_window(do_application_get_default(), &i);
    w = gtk_message_dialog_new(win, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", msg);
#define gtk_message_dialog_new dead
    gtk_window_set_icon_name(GTK_WINDOW(w), "domino");
    gtk_window_set_position(GTK_WINDOW(w), GTK_WIN_POS_CENTER);
    res = gtk_dialog_run(GTK_DIALOG(w));
    gtk_widget_destroy(w);
    return res;
}
typedef struct {
    const gchar *msg;
    gboolean event;
    gboolean res;
} idle_msg_t;

/*static gboolean idle_question(idle_msg_t *m)
{
    m->res = show_question(m->msg);
    m->event = FALSE;
    return FALSE;
}*/
gint do_message_question(const gchar *msg, GtkButtonsType button)
{
    /*if ( main_thread != g_thread_self() ) {
        idle_msg_t m;
        m.event = TRUE;
        m.msg = msg;
        gdk_threads_add_idle((GSourceFunc)idle_question, &m);
        wait_for(&m.event);
        return m.res;

    }
    else*/
        return show_question(msg);
}
