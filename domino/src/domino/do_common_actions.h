#ifndef DO_COMMON_ACTIONS_H
#define DO_COMMON_ACTIONS_H

#include <gtk/gtk.h>

void       do_common_actions_init(GtkUIManager *ui_manager, GtkWindow *win);
void       do_common_actions_action_sensitize(const char *name, gboolean b);
void       do_common_actions_action_toggle(const char *name, gboolean b);

//!!GtkWidget* do_common_actions_action_get_widget(const char * path);
GtkUIManager *do_common_actions_get_ui_manager();
void do_open_web(const gchar *uri);


//void       do_common_doAction(const char *action_name, gpointer user_data);

#endif
