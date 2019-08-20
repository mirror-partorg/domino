
#ifndef DO_ORDER_VIEW_H
#define DO_ORDER_VIEW_H

#include "do_tree_view.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_ORDER_VIEW         (do_order_view_get_type ())
#define DO_ORDER_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_ORDER_VIEW, DoOrderView))
#define DO_ORDER_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ORDER_VIEW, DoOrderViewClass))
#define DO_IS_ORDER_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_ORDER_VIEW))
#define DO_IS_ORDER_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_ORDER_VIEW))
#define DO_ORDER_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_ORDER_VIEW, DoOrderViewClass))

typedef struct _DoOrderView		DoOrderView;
typedef struct _DoOrderViewPrivate	DoOrderViewPrivate;
typedef struct _DoOrderViewClass		DoOrderViewClass;


struct _DoOrderView
{
	DoTreeView parent_instance;

};

struct _DoOrderViewClass
{
	DoTreeViewClass parent_class;
};

GType		do_order_view_get_type(void);

GtkWidget  *do_order_view_new ();

G_END_DECLS

#endif /* !DO_ORDER_VIEW_H */
