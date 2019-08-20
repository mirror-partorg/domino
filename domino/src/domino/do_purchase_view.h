
#ifndef DO_PURCHASE_VIEW_H
#define DO_PURCHASE_VIEW_H

#include "do_tree_view.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_PURCHASE_VIEW         (do_purchase_view_get_type ())
#define DO_PURCHASE_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_PURCHASE_VIEW, DoPurchaseView))
#define DO_PURCHASE_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_PURCHASE_VIEW, DoPurchaseViewClass))
#define DO_IS_PURCHASE_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_PURCHASE_VIEW))
#define DO_IS_PURCHASE_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_PURCHASE_VIEW))
#define DO_PURCHASE_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_PURCHASE_VIEW, DoPurchaseViewClass))

typedef struct _DoPurchaseView		DoPurchaseView;
typedef struct _DoPurchaseViewPrivate	DoPurchaseViewPrivate;
typedef struct _DoPurchaseViewClass		DoPurchaseViewClass;


struct _DoPurchaseView
{
	DoTreeView parent_instance;

};

struct _DoPurchaseViewClass
{
	DoTreeViewClass parent_class;
};

GType		do_purchase_view_get_type(void);

GtkWidget  *do_purchase_view_new (const gchar *units, GDate *date1, GDate *date2);

G_END_DECLS

#endif /* !DO_PURCHASE_VIEW_H */
