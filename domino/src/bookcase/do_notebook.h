
#ifndef DO_NOTEBOOK_H
#define DO_NOTEBOOK_H

#include <glib.h>
#include <gtk/gtk.h>
#include "do_view.h"

G_BEGIN_DECLS

#define DO_TYPE_NOTEBOOK		(do_notebook_get_type ())
#define DO_NOTEBOOK(o)	     	(G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_NOTEBOOK, DoNotebook))
#define DO_NOTEBOOK_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_NOTEBOOK, DoNotebookClass))
#define DO_IS_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_NOTEBOOK))
#define DO_IS_NOTEBOOK_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_NOTEBOOK))
#define DO_NOTEBOOK_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_NOTEBOOK, DoNotebookClass))


typedef struct _DoNotebookClass	DoNotebookClass;
typedef struct _DoNotebook		DoNotebook;
typedef struct _DoNotebookPrivate	DoNotebookPrivate;

struct _DoNotebook
{
	GtkNotebook parent;
	DoNotebookPrivate *priv;
};

struct _DoNotebookClass
{
        GtkNotebookClass parent_class;

	/* Signals */
	void	 (* tab_close_req)  (DoNotebook *notebook, DoView *embed);
};

GType		do_notebook_get_type		(void);

int		    do_notebook_add_tab	(DoNotebook *nb,
						 DoView *embed,
						 int position,
						 gboolean jump_to);

void		do_notebook_set_show_tabs	(DoNotebook *nb,
						 gboolean show_tabs);

void		do_notebook_set_dnd_enabled	(DoNotebook *nb,
						 gboolean enabled);

GList *         do_notebook_get_focused_pages (DoNotebook *nb);

G_END_DECLS

#endif /* DO_NOTEBOOK_H */
