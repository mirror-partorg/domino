#ifndef DO_VIEW_ACTIONS_H
#define DO_VIEW_ACTIONS_H

#include <gtk/gtk.h>

void       do_view_actions_init(GtkUIManager *ui_manager);
void       do_view_actions_action_sensitize(const char *name, gboolean b);
void       do_view_actions_action_toggle(const char *name, gboolean b);
void       do_view_actions_add_ui_from_string(const gchar *buffer, GError **error);

GtkWidget* do_view_actions_action_get_widget(const char * path);
void       do_view_actions_refresh();

void       do_view_actions_set_view(GtkWidget *view);
//DoView     *do_view_actions_get_view();

//void       do_view_doAction(const char *action_name, gpointer user_data);

#endif
