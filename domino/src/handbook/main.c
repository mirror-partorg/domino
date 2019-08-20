#include <stdlib.h>
#include <gtk/gtk.h>
#include "utils.h"
#include <gtkdomino.h>

void util_font_style_apply(const gchar *fontname)
{
    if ( !fontname )
        return;
#if GTK_MAJOR_VERSION == 2
    gchar *style;
    style = g_strdup_printf(
    "style \"myfont\""
    "{"
    "font_name = \"%s\""
    "}"
    "widget_class \"*\" style \"myfont\"",//gtk-font-name = \"%s\"\ngtk-icon-sizes = \"gtk-menu=48,48\"\n",
        fontname
        //fontname
    );
    gtk_rc_parse_string("gtk-icon-sizes = \"panel-menu=48,48:panel=48,48:gtk-button=48,48:gtk-large-toolbar=48,48:gtk-menu=48,48\"");
    gtk_rc_parse_string(style);
    g_free(style);
    GtkSettings *setting = NULL;

    setting = gtk_settings_get_default();
    gtk_rc_reset_styles(setting);
#else


#endif // GTK_MAJOR_VERSION

}
/*
static void  font_changed(GtkFontButton *widget, gpointer user_data)
{
    util_font_style_apply(gtk_font_button_get_font_name(widget));
}
*/
void invalid(DoEdit *edit, GtkWindow *win)
{
    SHOW_INFO("Activate");
}

int main (int argc, char *argv[])
{
  //GtkWidget *button = NULL;
  GtkWidget *win = NULL;
  //GtkWidget *vbox = NULL;

  if ( !MODULES_INIT(argc, argv) )
      return -1;


  //util_font_style_apply("Droid 12");

  /* Create the main window */
  GtkBuilder *builder;
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "one2.glade", NULL);
  GtkWidget *entry;

  win = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
  BUILDER_GET_WIDGET(entry , builder, "DoEntryProduct1");
  do_entry_set_multiple(DO_ENTRY(entry), TRUE);
  gtk_entry_set_text(GTK_ENTRY(entry), "ПРАДА");
  //do_entry_set_required(DO_ENTRY(entry), TRUE);
  //do_entry_set_regex_mask(DO_ENTRY(entry), "^(Да|Нет|Да пошел ты)$");
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
  g_signal_connect (win, "destroy", gtk_main_quit, NULL);
  g_signal_connect (GTK_ENTRY(entry), "activate", G_CALLBACK(invalid), win);
  gtk_widget_show_all (win);
  gtk_main ();
  return 0;
}
