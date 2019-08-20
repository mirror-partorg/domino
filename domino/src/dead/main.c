/**
*      @file viewer.c
*
*      @brief Too simple PDF Viewer
*
*      Copyright 2009 Yasser Nour <yassernour.wordpress.com>
*
*      This program is free software; you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation; either version 2 of the License, or
*      (at your option) any later version.
*
*      This program is distributed in the hope that it will be useful,
*      but WITHOUT ANY WARRANTY; without even the implied warranty of
*      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*      GNU General Public License for more details.
*
*      You should have received a copy of the GNU General Public License
*      along with this program; if not, write to the Free Software
*      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
*      MA 02110-1301, USA.
*
*/

/*
* Install this development packages:
* sudo apt-get install libgtk2.0-dev libpoppler-dev libpoppler-glib-dev
*
* Compile the viewer.c:
* gcc -o viewer viewer.c `pkg-config --libs --cflags gtk+-2.0 poppler-glib`
*
* Open a pdf file using it by:
* ./viewer file:////path/to/a/pdf/file.pdf
*/

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <poppler.h>
#include <math.h>

GtkWidget  *window1 ;
GtkWidget  *vbox1 ;
GtkWidget  *menubar ;
GtkWidget  *filemenu ;
GtkWidget  *filemenu_menu ;
GtkWidget  *fileopen ;
GtkWidget  *separator_1 ;
GtkWidget  *filequit ;
GtkWidget  *helpmenu ;
GtkWidget  *helpmenu_menu ;
GtkWidget  *helphelp ;
GtkWidget  *helpabout ;
GtkWidget  *toolbar1 ;
GtkToolItem  *toolback ;
GtkToolItem  *toolforward ;
GtkToolItem  *toolhome ;
GtkToolItem  *separator_2 ;
GtkToolItem  *toolquit ;
GtkWidget  *drawarea1 ;
GtkWidget  *label1 ;
GtkWidget  *hbox1 ;
GtkWidget  *button1 ;
GtkWidget  *button2 ;
GtkAccelGroup *menuGroup;
GtkWidget *statusbar;

PopplerPage *ppage;
PopplerDocument *document;
cairo_surface_t *surface;
int current_page = 0;
int total_pages = 0;

#define gtk_widget_set_tooltip_text(a, b)

void show_about(GtkWidget *widget, gpointer data)
{

GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("Noon.jpg", NULL);

GtkWidget *dialog = gtk_about_dialog_new();
gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "PView");
gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "0.0");
gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
"(c) Yasser Nour");
gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
"NourView is a too simple pdf viewer");
gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
"http://www.yassernour.wordpress.net");
gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
g_object_unref(pixbuf), pixbuf = NULL;
gtk_dialog_run(GTK_DIALOG (dialog));
gtk_widget_destroy(dialog);
}

void update_statusbar()
{
gchar *str;
str = g_strdup_printf("Page %d / %d",
current_page + 1, total_pages);

gtk_statusbar_push(GTK_STATUSBAR(statusbar),
gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), str), str);
g_free(str);
}

void  MessageBox (GtkWindow *parentWindow, char *messageValue)
{
GtkWidget *dialog;
dialog = gtk_message_dialog_new (parentWindow, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, messageValue);
gtk_window_set_title (GTK_WINDOW (dialog), "Information");
gtk_dialog_run (GTK_DIALOG (dialog));
gtk_widget_destroy (dialog);
}

/* Stop the GTK+ main loop function. */
static void destroy_window1 (GtkWidget *window, gpointer data)
{
gtk_main_quit ();
}

/* Return FALSE to destroy the widget. By returning TRUE, you can cancel a delete-event. This can be used to confirm quitting the application. */
static gboolean delete_event_window1 (GtkWidget *window, GdkEvent *event,  gpointer data)
{
return (FALSE);
}

static void drawarea1_expose_event( GtkWidget *widget, GdkEventExpose *event )
{
cairo_t *cr;

gdk_window_clear (drawarea1->window);
cr = gdk_cairo_create (drawarea1->window);

cairo_set_source_surface (cr, surface, 0, 0);
cairo_paint (cr);
cairo_destroy (cr);
}

static void redraw_callback (void *data)
{
gtk_widget_queue_draw (drawarea1);
}

static void view_set_page (int page)
{
int err;
int w, h;
double width, height;
cairo_t *cr;

ppage = poppler_document_get_page (document, page);
poppler_page_get_size (ppage, &width, &height);
w = (int) ceil(width);
h = (int) ceil(height);
cairo_surface_destroy (surface);
surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
cr = cairo_create (surface);
poppler_page_render (ppage, cr);
cairo_destroy (cr);
gtk_widget_set_size_request (drawarea1, w, h);
gtk_widget_queue_draw (drawarea1);
update_statusbar();
}

static void fileopen_activated (GtkMenuItem *fileopen, GtkWindow *parentWindow)
{
MessageBox(parentWindow, "Menu fileopen was clicked.");
}

static void filequit_activated (GtkMenuItem *filequit, GtkWindow *parentWindow)
{
gtk_main_quit ();
}

static void helphelp_activated (GtkMenuItem *helphelp, GtkWindow *parentWindow)
{
MessageBox(parentWindow, "Menu helphelp was clicked.");
}

static void helpabout_activated (GtkMenuItem *helpabout, GtkWindow *parentWindow)
{
MessageBox(parentWindow, "Menu helpabout was clicked.");
}

static void toolback_clicked (GtkToolItem *tooleditcut, GtkWindow *parentWindow)
{
if( current_page != 0)
current_page--;

view_set_page(current_page);
}

static void toolforward_clicked (GtkToolItem *tooleditcopy, GtkWindow *parentWindow)
{
if( current_page != (total_pages - 1))
current_page++;

view_set_page(current_page);
}

static void toolhome_clicked (GtkToolItem *tooleditpaste, GtkWindow *parentWindow)
{
current_page = 0;
view_set_page(0);
}

static void toolquit_clicked (GtkToolItem *tooleditselectall, GtkWindow *parentWindow)
{
gtk_main_quit ();
}

void  InitializeUI ()
{
menuGroup = gtk_accel_group_new();

/* initialize main window */
window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
gtk_window_set_title (GTK_WINDOW (window1), "pView");
gtk_window_set_default_size (GTK_WINDOW (window1), 400, 200);
gtk_window_maximize (GTK_WINDOW (window1));

/* Connect the main window to the destroy and delete-event signals. */
g_signal_connect (G_OBJECT (window1), "destroy", G_CALLBACK (destroy_window1), NULL);
g_signal_connect (G_OBJECT (window1), "delete_event", G_CALLBACK (delete_event_window1), NULL);

vbox1 = gtk_vbox_new (FALSE, 0);
gtk_container_add (GTK_CONTAINER (window1), vbox1);

menubar = gtk_menu_bar_new ();
gtk_box_pack_start (GTK_BOX (vbox1), menubar, FALSE, FALSE, 2);

filemenu = gtk_menu_item_new_with_label ("File");
filemenu_menu = gtk_menu_new ();
gtk_menu_item_set_submenu (GTK_MENU_ITEM (filemenu), filemenu_menu);
gtk_menu_shell_append (GTK_MENU_SHELL (menubar), filemenu);

fileopen = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN, menuGroup);
gtk_menu_shell_append (GTK_MENU_SHELL (filemenu_menu), fileopen);
g_signal_connect (G_OBJECT (fileopen), "activate", G_CALLBACK (fileopen_activated), (gpointer) window1);

separator_1 = gtk_separator_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (filemenu_menu), separator_1);

filequit = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, menuGroup);
gtk_menu_shell_append (GTK_MENU_SHELL (filemenu_menu), filequit);
g_signal_connect (G_OBJECT (filequit), "activate", G_CALLBACK (filequit_activated), (gpointer) window1);

helpmenu = gtk_menu_item_new_with_label ("Help");
helpmenu_menu = gtk_menu_new ();
gtk_menu_item_set_submenu (GTK_MENU_ITEM (helpmenu), helpmenu_menu);
gtk_menu_shell_append (GTK_MENU_SHELL (menubar), helpmenu);

helphelp = gtk_image_menu_item_new_from_stock (GTK_STOCK_HELP, menuGroup);
gtk_menu_shell_append (GTK_MENU_SHELL (helpmenu_menu), helphelp);
g_signal_connect (G_OBJECT (helphelp), "activate", G_CALLBACK (helphelp_activated), (gpointer) window1);

helpabout = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, menuGroup);
gtk_menu_shell_append (GTK_MENU_SHELL (helpmenu_menu), helpabout);
g_signal_connect (G_OBJECT (helpabout), "activate", G_CALLBACK (show_about), (gpointer) window1);

toolbar1 = gtk_toolbar_new ();
gtk_toolbar_set_show_arrow (GTK_TOOLBAR (toolbar1), TRUE);
gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH);
gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 2);

toolback = gtk_tool_button_new_from_stock (GTK_STOCK_GO_BACK);
gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolback, 0);
gtk_widget_set_tooltip_text (GTK_WIDGET(toolback), "Previous Page");
g_signal_connect (G_OBJECT (toolback), "clicked", G_CALLBACK (toolback_clicked), (gpointer) window1);

toolforward = gtk_tool_button_new_from_stock (GTK_STOCK_GO_FORWARD);
gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolforward, 1);
gtk_widget_set_tooltip_text (GTK_WIDGET(toolforward), "Next page");
g_signal_connect (G_OBJECT (toolforward), "clicked", G_CALLBACK (toolforward_clicked), (gpointer) window1);

toolhome = gtk_tool_button_new_from_stock (GTK_STOCK_HOME);
gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolhome, 2);
gtk_widget_set_tooltip_text (GTK_WIDGET(toolhome), "Paste the Clipboard");
g_signal_connect (G_OBJECT (toolhome), "clicked", G_CALLBACK (toolhome_clicked), (gpointer) window1);

separator_2 = gtk_separator_tool_item_new ();
gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), separator_2, 3);

toolquit = gtk_tool_button_new_from_stock (GTK_STOCK_QUIT);
gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), toolquit, 4);
gtk_widget_set_tooltip_text (GTK_WIDGET(toolquit), "exit");
g_signal_connect (G_OBJECT (toolquit), "clicked", G_CALLBACK (toolquit_clicked), (gpointer) window1);

drawarea1 = gtk_drawing_area_new();
gtk_box_pack_start (GTK_BOX (vbox1), drawarea1, TRUE, TRUE, 2);
g_signal_connect (G_OBJECT (drawarea1), "expose_event", G_CALLBACK (drawarea1_expose_event), (gpointer) window1);

statusbar = gtk_statusbar_new();
gtk_box_pack_start(GTK_BOX(vbox1), statusbar, FALSE, TRUE, 1);

gtk_window_add_accel_group( GTK_WINDOW(window1), menuGroup);
}

int main (int argc, char *argv[])
{
gtk_init (&argc, &argv);

if (argc != 2) {
printf("Usage: viewer <uri>\n");
return 1;
}

document = poppler_document_new_from_file (argv[1], NULL, NULL);

if (document == NULL)
return -1;

InitializeUI();

total_pages = poppler_document_get_n_pages (document);
view_set_page(0);

gtk_widget_show_all (window1);
gtk_main ();

return (0);
}
