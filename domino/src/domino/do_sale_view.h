
#ifndef DO_SALE_VIEW_H
#define DO_SALE_VIEW_H

#include "do_tree_view.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_SALE_VIEW         (do_sale_view_get_type ())
#define DO_SALE_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_SALE_VIEW, DoSaleView))
#define DO_SALE_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_SALE_VIEW, DoSaleViewClass))
#define DO_IS_SALE_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_SALE_VIEW))
#define DO_IS_SALE_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_SALE_VIEW))
#define DO_SALE_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_SALE_VIEW, DoSaleViewClass))

typedef struct _DoSaleView		DoSaleView;
typedef struct _DoSaleViewPrivate	DoSaleViewPrivate;
typedef struct _DoSaleViewClass		DoSaleViewClass;


struct _DoSaleView
{
	DoTreeView parent_instance;

};

struct _DoSaleViewClass
{
	DoTreeViewClass parent_class;
};

GType		do_sale_view_get_type(void);

GtkWidget  *do_sale_view_new (const gchar *units, GDate *date1, GDate *date2);

G_END_DECLS

#endif /* !DO_SALE_VIEW_H */
