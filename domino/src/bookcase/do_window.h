
#ifndef DO_WINDOW_H
#define DO_WINDOW_H

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "do_view.h"

G_BEGIN_DECLS

#define DO_TYPE_WINDOW	(do_window_get_type ())
#define DO_WINDOW(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_WINDOW, DoWindow))
#define DO_WINDOW_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_WINDOW, DoWindowClass))
#define DO_IS_WINDOW(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_WINDOW))
#define DO_IS_WINDOW_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_WINDOW))
#define DO_WINDOW_GET_CLASS(o)(G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_WINDOW, DoWindowClass))

typedef struct _DoWindowClass	DoWindowClass;
typedef struct _DoWindow		DoWindow;
typedef struct _DoWindowPrivate	DoWindowPrivate;

struct _DoWindow
{
	GtkWindow parent;

};

struct _DoWindowClass
{
	GtkWindowClass parent_class;
};

GType		  do_window_get_type (void);

DoWindow	 *do_window_new	();

GObject		 *do_window_get_ui_manager(DoWindow *window);

GtkWidget	 *do_window_get_mainmenu(DoWindow *window);

GtkWidget	 *do_window_get_toolbar(DoWindow *window);

GtkWidget	 *do_window_get_notebook(DoWindow *window);

GtkWidget	 *do_window_get_statusbar(DoWindow *window);

GtkWidget 	 *do_window_get_active_child(DoWindow *window);
void          do_window_set_active_child(DoWindow *window, DoView *view);
GList        *do_window_get_children (DoWindow *window);

void          do_window_add_ui_from_string(DoWindow *window, const char *ui, gsize length);

void          do_window_set_tree_view_lines(DoWindow *window, GtkTreeViewGridLines grid_lines);

void          do_window_set_toolbar_progress(DoWindow *window, int proc);
int           do_window_get_toolbar_progress(DoWindow *window);
void          do_window_set_toolbar_text(DoWindow *window, const gchar *text);
const gchar  *do_window_get_toolbar_text(DoWindow *window);

void          do_window_update_toolbar(DoWindow *window);
void          do_window_set_goods(DoWindow *window, DoView *view);



G_END_DECLS

#endif
