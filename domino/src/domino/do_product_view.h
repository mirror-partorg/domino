
#ifndef DO_PRODUCT_VIEW_H
#define DO_PRODUCT_VIEW_H

#include <gtk/gtk.h>
#include "do_tree_view.h"

G_BEGIN_DECLS

#define DO_TYPE_PRODUCT_VIEW         (do_product_view_get_type ())
#define DO_PRODUCT_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_PRODUCT_VIEW, DoProductView))
#define DO_PRODUCT_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_PRODUCT_VIEW, DoProductViewClass))
#define DO_IS_PRODUCT_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_PRODUCT_VIEW))
#define DO_IS_PRODUCT_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_PRODUCT_VIEW))
#define DO_PRODUCT_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_PRODUCT_VIEW, DoProductViewClass))

typedef struct _DoProductView		DoProductView;
typedef struct _DoProductViewPrivate	DoProductViewPrivate;
typedef struct _DoProductViewClass		DoProductViewClass;

typedef enum
{
	DO_PRODUCT_VIEW_FLAGS_SHOW_MULTI_PRICE = 1 << 0,
	DO_PRODUCT_VIEW_FLAGS_SHOW_RECEIVED    = 1 << 1,
	DO_PRODUCT_VIEW_FLAGS_SHOW_UNPACK_PRICE = 1 << 2,
	DO_PRODUCT_VIEW_FLAGS_SELECTION_MULTIPLE = 1 << 3,
	DO_PRODUCT_VIEW_FLAGS_ROW_ACTIVATED_TO_SEND = 1 << 4,
	DO_PRODUCT_VIEW_FLAGS_LOAD_IN_THREAD = 1 << 5,
	DO_PRODUCT_VIEW_FLAGS_REALTIME = 1 << 6,
	DO_PRODUCT_VIEW_FLAGS_SHOW_RECOMMEND = 1 << 7,
	DO_PRODUCT_VIEW_FLAGS_SHOW_CLOCK = 1 << 8,
} DoProductViewFlags;

struct _DoProductView
{
	DoTreeView parent_instance;
};

struct _DoProductViewClass
{
	DoTreeViewClass parent_class;
};

#define DO_PRODUCT_VIEW_FLAGS_DEFAULT 0

GType		do_product_view_get_type(void);

GtkWidget  *do_product_view_new (const gchar *alias_name, const gchar *unit, DoProductViewFlags flags);
void do_product_view_refill_recommend(DoProductView *view);

G_END_DECLS

#endif /* !DO_PRODUCT_VIEW_H */
