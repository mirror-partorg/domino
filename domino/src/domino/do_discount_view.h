
#ifndef DO_DISCOUNT_VIEW_H
#define DO_DISCOUNT_VIEW_H

#include "do_tree_view.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_DISCOUNT_VIEW         (do_discount_view_get_type ())
#define DO_DISCOUNT_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_DISCOUNT_VIEW, DoDiscountView))
#define DO_DISCOUNT_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_DISCOUNT_VIEW, DoDiscountViewClass))
#define DO_IS_DISCOUNT_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_DISCOUNT_VIEW))
#define DO_IS_DISCOUNT_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_DISCOUNT_VIEW))
#define DO_DISCOUNT_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_DISCOUNT_VIEW, DoDiscountViewClass))

typedef struct _DoDiscountView		DoDiscountView;
typedef struct _DoDiscountViewPrivate	DoDiscountViewPrivate;
typedef struct _DoDiscountViewClass		DoDiscountViewClass;


struct _DoDiscountView
{
	DoTreeView parent_instance;

};

struct _DoDiscountViewClass
{
	DoTreeViewClass parent_class;
};

GType		do_discount_view_get_type(void);

GtkWidget  *do_discount_view_new (const gchar *units, GDate *date1, GDate *date2, const gchar *range_name, gint group);

G_END_DECLS

#endif /* !DO_DISCOUNT_VIEW_H */
