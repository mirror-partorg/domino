#ifndef MESSAGES_H_INCLUDED
#define MESSAGES_H_INCLUDED

#include <glib.h>
#include <gtk/gtk.h>

#define SHOW_MESSAGE() message_show_all()
#define SHOW_INFO(...) { gchar *v = g_strdup_printf(__VA_ARGS__); message_log(v, G_LOG_LEVEL_INFO, NULL); g_free(v);}
#define SHOW_ERROR(...) { gchar *v = g_strdup_printf(__VA_ARGS__); message_log(v, G_LOG_LEVEL_ERROR, NULL); g_free(v);}
#define SHOW_QUESTION(r,b,...) { gchar *v = g_strdup_printf(__VA_ARGS__); r=message_question(v,b); g_free(v);}
#define ERROR_AND_SHOW(...) { gchar *v = g_strdup_printf(__VA_ARGS__); message_log(v, G_LOG_LEVEL_ERROR, NULL); g_free(v); message_show_all();}
#define gtk_message_dialog_new dead
#define gtk_dialog_new dead

#ifdef __cplusplus
extern "C" {
#endif

void       message_init();
void       message_show_all();

void       message_log(const gchar *message, GLogLevelFlags log_flags, void *user_data);
void       message_log_gtk_func(const gchar *log_domain, GLogLevelFlags log_flags, const gchar *msg, gpointer user_data);
int        message_log_do_func(const char *mgs, int level, void *user_data);
gint       message_question(const gchar *msg, GtkButtonsType buttons);

#ifdef __cplusplus
}
#endif


#endif // MESSAGES_H_INCLUDED
