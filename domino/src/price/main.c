#include <stdlib.h>
#include <gtk/gtk.h>
#include "dosettings.h"
#include "dowindow.h"
#include "dopricewidget.h"
#include "config.h"


int main (int argc, char *argv[])
{
  GtkWidget *win = NULL;
  GtkWidget *vbox = NULL;
  GtkWidget *h;

  /* Initialize GTK+ */
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
  gtk_init (&argc, &argv);
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);
  do_settings_new (NULL);

  /* Create the main window */
  win = do_window_new (GTK_WINDOW_TOPLEVEL, "test-win");
  gtk_container_set_border_width (GTK_CONTAINER (win), 8);
  gtk_window_set_title (GTK_WINDOW (win), "Hello World");
  gtk_window_set_icon_name (GTK_WINDOW (win), "bookcase");

  h = gtk_header_bar_new ();
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (h), TRUE);
  gtk_header_bar_set_title (GTK_HEADER_BAR (h), "Прайс");

  gtk_window_set_titlebar (GTK_WINDOW (win), h);

  /* Create a vertical box with buttons */
  vbox = do_price_widget_new(h);
  g_object_set(vbox, "connection_str", "host=database dbname=domino user=domino", "firm-nick", "tta", NULL);
  gtk_container_add (GTK_CONTAINER (win), vbox);

  gtk_widget_realize (win);
  g_signal_connect (win, "destroy", gtk_main_quit, NULL);

  /* Enter the main loop */
  gtk_widget_show_all (win);
  gtk_main ();
  return 0;
}
