
#ifndef DO_INVENTORY_MENU_H
#define DO_INVENTORY_MENU_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_INVENTORY_MENU         (do_inventory_menu_get_type ())
#define DO_INVENTORY_MENU(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_INVENTORY_MENU, DoInventoryMenu))
#define DO_INVENTORY_MENU_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_INVENTORY_MENU, DoInventoryMenuClass))
#define DO_IS_INVENTORY_MENU(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_INVENTORY_MENU))
#define DO_IS_INVENTORY_MENU_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_INVENTORY_MENU))
#define DO_INVENTORY_MENU_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_INVENTORY_MENU, DoInventoryMenuClass))

typedef struct _DoInventoryMenu		DoInventoryMenu;
typedef struct _DoInventoryMenuPrivate	DoInventoryMenuPrivate;
typedef struct _DoInventoryMenuClass		DoInventoryMenuClass;

struct _DoInventoryMenu
{
	GtkVBox parent_instance;
};

struct _DoInventoryMenuClass
{
	GtkVBoxClass parent_class;
};

#define DO_INVENTORY_MENU_FLAGS_DEFAULT 0

GType		do_inventory_menu_get_type(void);

GtkWidget  *do_inventory_menu_new();

G_END_DECLS

#endif /* !DO_INVENTORY_MENU_H */
