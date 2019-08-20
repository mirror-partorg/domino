
#ifndef DO_REST_VIEW_H
#define DO_REST_VIEW_H

#include "do_tree_view.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_REST_VIEW         (do_rest_view_get_type ())
#define DO_REST_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_REST_VIEW, DoRestView))
#define DO_REST_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_REST_VIEW, DoRestViewClass))
#define DO_IS_REST_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_REST_VIEW))
#define DO_IS_REST_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_REST_VIEW))
#define DO_REST_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_REST_VIEW, DoRestViewClass))

typedef struct _DoRestView		DoRestView;
typedef struct _DoRestViewPrivate	DoRestViewPrivate;
typedef struct _DoRestViewClass		DoRestViewClass;


struct _DoRestView
{
	DoTreeView parent_instance;

};

struct _DoRestViewClass
{
	DoTreeViewClass parent_class;
};

GType		do_rest_view_get_type(void);

GtkWidget  *do_rest_view_new (const gchar *units, GDate *date1, GDate *date2);

G_END_DECLS

#endif /* !DO_REST_VIEW_H */
