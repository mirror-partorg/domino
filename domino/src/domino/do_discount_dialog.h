
#ifndef DO_DISCOUNT_DIALOG_H
#define DO_DISCOUNT_DIALOG_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_DISCOUNT_DIALOG         (do_discount_dialog_get_type ())
#define DO_DISCOUNT_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_DISCOUNT_DIALOG, DoDiscountDialog))
#define DO_DISCOUNT_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_DISCOUNT_DIALOG, DoDiscountDialogClass))
#define DO_IS_DISCOUNT_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_DISCOUNT_DIALOG))
#define DO_IS_DISCOUNT_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_DISCOUNT_DIALOG))
#define DO_DISCOUNT_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_DISCOUNT_DIALOG, DoDiscountDialogClass))

typedef struct _DoDiscountDialog		DoDiscountDialog;
typedef struct _DoDiscountDialogPrivate	DoDiscountDialogPrivate;
typedef struct _DoDiscountDialogClass	DoDiscountDialogClass;

struct _DoDiscountDialog
{
	GtkDialog parent_instance;
};

struct _DoDiscountDialogClass
{
	GtkDialogClass parent_class;
};

#define DO_DISCOUNT_DIALOG_FLAGS_DEFAULT 0

GType		do_discount_dialog_get_type(void);

GtkWidget  *do_discount_dialog_new(GtkWidget *parent);

gint        do_discount_dialog_run(DoDiscountDialog *dialog);

gchar      *do_discount_dialog_get_units(DoDiscountDialog *dialog);
GList      *do_discount_dialog_get_discount_types(DoDiscountDialog *dialog);
GDate       do_discount_dialog_get_date1(DoDiscountDialog *dialog);
GDate       do_discount_dialog_get_date2(DoDiscountDialog *dialog);


G_END_DECLS

#endif /* !DO_DISCOUNT_DIALOG_H */
