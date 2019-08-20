
#ifndef DO_RZ_VIEW_H
#define DO_RZ_VIEW_H

#include <gtk/gtk.h>
#include "do_tree_view.h"

G_BEGIN_DECLS

#define DO_TYPE_RZ_VIEW         (do_rz_view_get_type ())
#define DO_RZ_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_RZ_VIEW, DoRzView))
#define DO_RZ_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_RZ_VIEW, DoRzViewClass))
#define DO_IS_RZ_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_RZ_VIEW))
#define DO_IS_RZ_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_RZ_VIEW))
#define DO_RZ_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_RZ_VIEW, DoRzViewClass))

typedef struct _DoRzView		DoRzView;
typedef struct _DoRzViewPrivate	DoRzViewPrivate;
typedef struct _DoRzViewClass		DoRzViewClass;

struct _DoRzView
{
	DoTreeView parent_instance;
};

struct _DoRzViewClass
{
	DoTreeViewClass parent_class;
};

GType		do_rz_view_get_type(void);

GtkWidget *do_rz_view_new (const gchar *alias_name);
//gboolean   do_rz_dialog_run(GtkWidget *parent, const gchar *alias_name, const gchar *document, gint line);


G_END_DECLS

#endif /* !DO_RZ_VIEW_H */
