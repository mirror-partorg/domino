
#ifndef DO_LIMIT_VIEW_H
#define DO_LIMIT_VIEW_H

#include "do_tree_view.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_LIMIT_VIEW         (do_limit_view_get_type ())
#define DO_LIMIT_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_LIMIT_VIEW, DoLimitView))
#define DO_LIMIT_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_LIMIT_VIEW, DoLimitViewClass))
#define DO_IS_LIMIT_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_LIMIT_VIEW))
#define DO_IS_LIMIT_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_LIMIT_VIEW))
#define DO_LIMIT_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_LIMIT_VIEW, DoLimitViewClass))

typedef struct _DoLimitView		DoLimitView;
typedef struct _DoLimitViewPrivate	DoLimitViewPrivate;
typedef struct _DoLimitViewClass		DoLimitViewClass;


struct _DoLimitView
{
	DoTreeView parent_instance;

};

struct _DoLimitViewClass
{
	DoTreeViewClass parent_class;
};

GType		do_limit_view_get_type(void);

GtkWidget  *do_limit_view_new ();

G_END_DECLS

#endif /* !DO_LIMIT_VIEW_H */
