
#ifndef DO_INVENTORY_VIEW_H
#define DO_INVENTORY_VIEW_H

#include <gtk/gtk.h>
#include <domino.h>
#include "do_tree_view.h"

G_BEGIN_DECLS

#define DO_TYPE_INVENTORY_VIEW         (do_inventory_view_get_type ())
#define DO_INVENTORY_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_INVENTORY_VIEW, DoInventoryView))
#define DO_INVENTORY_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_INVENTORY_VIEW, DoInventoryViewClass))
#define DO_IS_INVENTORY_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_INVENTORY_VIEW))
#define DO_IS_INVENTORY_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_INVENTORY_VIEW))
#define DO_INVENTORY_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_INVENTORY_VIEW, DoInventoryViewClass))

typedef struct _DoInventoryView		DoInventoryView;
typedef struct _DoInventoryViewPrivate	DoInventoryViewPrivate;
typedef struct _DoInventoryViewClass		DoInventoryViewClass;

struct _DoInventoryView
{
	DoTreeView parent_instance;
};

struct _DoInventoryViewClass
{
	DoTreeViewClass parent_class;
};

GType	   do_inventory_view_get_type(void);

GtkWidget *do_inventory_view_new (document_key0_t *key, const gchar *user_code);

G_END_DECLS

#endif /* !DO_INVENTORY_VIEW_H */
