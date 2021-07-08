#ifndef DO_VIEW_ACTIONS_H
#define DO_VIEW_ACTIONS_H

#include <gtk/gtk.h>
#include "do_window.h"

void       do_view_actions_init(DoWindow *window);
void       do_view_actions_action_sensitize(DoWindow *window, const char *name, gboolean b);
void       do_view_actions_action_toggle(DoWindow *window, const char *name, gboolean b);
void       do_view_actions_add_ui_from_string(DoWindow *window, const gchar *buffer, GError **error);

//GtkWidget* do_view_actions_action_get_widget(DoWindow *window, const char *path);
void       do_view_actions_refresh(GtkWidget *window);
//void       do_view_actions_set_view(GtkWidget *view);
//DoView     *do_view_actions_get_view();

//void       do_view_doAction(const char *action_name, gpointer user_data);
void       do_view_action_activate(const gchar *action_name, GVariant *parameter);


#endif
