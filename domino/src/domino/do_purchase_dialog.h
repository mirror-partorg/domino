
#ifndef DO_PURCHASE_DIALOG_H
#define DO_PURCHASE_DIALOG_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_PURCHASE_DIALOG         (do_purchase_dialog_get_type ())
#define DO_PURCHASE_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_PURCHASE_DIALOG, DoPurchaseDialog))
#define DO_PURCHASE_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_PURCHASE_DIALOG, DoPurchaseDialogClass))
#define DO_IS_PURCHASE_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_PURCHASE_DIALOG))
#define DO_IS_PURCHASE_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_PURCHASE_DIALOG))
#define DO_PURCHASE_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_PURCHASE_DIALOG, DoPurchaseDialogClass))

typedef struct _DoPurchaseDialog		DoPurchaseDialog;
typedef struct _DoPurchaseDialogPrivate	DoPurchaseDialogPrivate;
typedef struct _DoPurchaseDialogClass	DoPurchaseDialogClass;

struct _DoPurchaseDialog
{
	GtkDialog parent_instance;
};

struct _DoPurchaseDialogClass
{
	GtkDialogClass parent_class;
};

#define DO_PURCHASE_DIALOG_FLAGS_DEFAULT 0

GType		do_purchase_dialog_get_type(void);

GtkWidget  *do_purchase_dialog_new(GtkWidget *parent);

gint        do_purchase_dialog_run(DoPurchaseDialog *dialog);

gchar      *do_purchase_dialog_get_units(DoPurchaseDialog *dialog);
GDate       do_purchase_dialog_get_date1(DoPurchaseDialog *dialog);
GDate       do_purchase_dialog_get_date2(DoPurchaseDialog *dialog);


G_END_DECLS

#endif /* !DO_PURCHASE_DIALOG_H */
