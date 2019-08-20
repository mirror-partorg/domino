
#include "message.h"

typedef _Message Message;
struct _Message {
    gchar *message;
    MessageType type;
};

void       do_message_init();
void       do_message_show(const gchar *message, MessageType type);
void       do_message_error_log(const gchar *message, int level, void *user_data);
gboolean   do_message_show_complect();
