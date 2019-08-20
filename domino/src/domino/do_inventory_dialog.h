
#ifndef DO_INVENTORY_DIALOG_H
#define DO_INVENTORY_DIALOG_H

#include <gtk/gtk.h>
#include <domino.h>

G_BEGIN_DECLS

#define DO_TYPE_INVENTORY_DIALOG         (do_inventory_dialog_get_type ())
#define DO_INVENTORY_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_INVENTORY_DIALOG, DoInventoryDialog))
#define DO_INVENTORY_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_INVENTORY_DIALOG, DoInventoryDialogClass))
#define DO_IS_INVENTORY_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_INVENTORY_DIALOG))
#define DO_IS_INVENTORY_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_INVENTORY_DIALOG))
#define DO_INVENTORY_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_INVENTORY_DIALOG, DoInventoryDialogClass))

typedef struct _DoInventoryDialog		DoInventoryDialog;
typedef struct _DoInventoryDialogPrivate	DoInventoryDialogPrivate;
typedef struct _DoInventoryDialogClass		DoInventoryDialogClass;

struct _DoInventoryDialog
{
	GtkVBox parent_instance;
};

struct _DoInventoryDialogClass
{
	GtkVBoxClass parent_class;
};

#define DO_INVENTORY_DIALOG_FLAGS_DEFAULT 0

GType		do_inventory_dialog_get_type(void);

GtkWidget  *do_inventory_dialog_new(document_key0_t *document_key);

G_END_DECLS

#endif /* !DO_INVENTORY_DIALOG_H */
