
#ifndef DO_SALE_DIALOG_H
#define DO_SALE_DIALOG_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_SALE_DIALOG         (do_sale_dialog_get_type ())
#define DO_SALE_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_SALE_DIALOG, DoSaleDialog))
#define DO_SALE_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_SALE_DIALOG, DoSaleDialogClass))
#define DO_IS_SALE_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_SALE_DIALOG))
#define DO_IS_SALE_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_SALE_DIALOG))
#define DO_SALE_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_SALE_DIALOG, DoSaleDialogClass))

typedef struct _DoSaleDialog		DoSaleDialog;
typedef struct _DoSaleDialogPrivate	DoSaleDialogPrivate;
typedef struct _DoSaleDialogClass	DoSaleDialogClass;

struct _DoSaleDialog
{
	GtkDialog parent_instance;
};

struct _DoSaleDialogClass
{
	GtkDialogClass parent_class;
};

#define DO_SALE_DIALOG_FLAGS_DEFAULT 0

GType		do_sale_dialog_get_type(void);

GtkWidget  *do_sale_dialog_new(GtkWidget *parent);

gint        do_sale_dialog_run(DoSaleDialog *dialog);

gchar      *do_sale_dialog_get_units(DoSaleDialog *dialog);
GDate       do_sale_dialog_get_date1(DoSaleDialog *dialog);
GDate       do_sale_dialog_get_date2(DoSaleDialog *dialog);


G_END_DECLS

#endif /* !DO_SALE_DIALOG_H */
