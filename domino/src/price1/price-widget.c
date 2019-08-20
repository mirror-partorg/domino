#include "price-widget.h"
#include "price-view.h"
static gchar *settings_filename;

GtkWidget *price_widget_new(const gchar *conn_str, const gchar *firm_nick)
{
    GtkWidget *box;
    GtkWidget *scroll;
    GtkWidget *view;
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_IN);
    //gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(scroll), FALSE);

    view = price_view_new(conn_str, firm_nick);

    gtk_container_add(GTK_CONTAINER(scroll), view);

    gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    return box;
}
