#ifndef MESSAGES_H_INCLUDED
#define MESSAGES_H_INCLUDED

#include <glib/glib.h>

typedef _MessageType MessageType;

enum _MessageType
{
    MESSAGE_TYPE_ERROR = 0,
    MESSAGE_TYPE_INFO,
}

#define SHOW_ERROR_(...) { gchar *v = g_strdup_printf(__VA_ARGS__); message_error(v); }
#define ERROR(...) { gchar *v = g_strdup_printf(__VA_ARGS__); message_error_log(v, LOG_ERR, NULL); g_free(v);}

#ifdef __cplusplus
extern "C" {
#endif

void       message_init();
void       message_show(const gchar *message, MessageType type);
void       message_error_log(const gchar *message, int level, void *user_data);
gboolean   message_show_complect();


#ifdef __cplusplus
}
#endif


#endif // MESSAGES_H_INCLUDED
