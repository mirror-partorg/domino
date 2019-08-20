#ifndef MESSAGES_H_INCLUDED
#define MESSAGES_H_INCLUDED

#include <glib.h>

#define SHOW_MESSAGE() do_message_show_all()
#define INFO(...) { gchar *v = g_strdup_printf(__VA_ARGS__); do_message_log(v, G_LOG_LEVEL_INFO, NULL); g_free(v);}
#define MY_ERROR(...) { gchar *v = g_strdup_printf(__VA_ARGS__); do_message_log(v, G_LOG_LEVEL_ERROR, NULL); g_free(v);}
#define QUESTION(r,...) { gchar *v = g_strdup_printf(__VA_ARGS__); r=do_message_question(v); g_free(v);}
#define ERROR_AND_SHOW(...) { gchar *v = g_strdup_printf(__VA_ARGS__); do_message_log(v, G_LOG_LEVEL_ERROR, NULL); g_free(v); do_message_show_all();}

#ifdef __cplusplus
extern "C" {
#endif

void       do_message_init();
void       do_message_show_all();

void       do_message_log(const gchar *message, GLogLevelFlags log_flags, void *user_data);
void       do_message_log_gtk_func(const gchar *log_domain, GLogLevelFlags log_flags, const gchar *msg, gpointer user_data);
int        do_message_log_do_func(const char *mgs, int level, void *user_data);
gboolean   do_message_question(const gchar *msg);

#ifdef __cplusplus
}
#endif


#endif // MESSAGES_H_INCLUDED
