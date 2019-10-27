#ifndef DO_COMMON_ACTIONS_H
#define DO_COMMON_ACTIONS_H

#include <gtk/gtk.h>
#include "do_window.h"

void       do_common_actions_init(DoWindow *window);
void       do_common_action_activate(const gchar *action_name, GVariant *parameter);

#endif
