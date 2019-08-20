
#ifndef DO_SELLER_DIALOG_H
#define DO_SELLER_DIALOG_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_SELLER_DIALOG         (do_seller_dialog_get_type ())
#define DO_SELLER_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_SELLER_DIALOG, DoSellerDialog))
#define DO_SELLER_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_SELLER_DIALOG, DoSellerDialogClass))
#define DO_IS_SELLER_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_SELLER_DIALOG))
#define DO_IS_SELLER_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_SELLER_DIALOG))
#define DO_SELLER_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_SELLER_DIALOG, DoSellerDialogClass))

typedef struct _DoSellerDialog		DoSellerDialog;
typedef struct _DoSellerDialogPrivate	DoSellerDialogPrivate;
typedef struct _DoSellerDialogClass	DoSellerDialogClass;

struct _DoSellerDialog
{
    GtkVBox parent_instance;
};

struct _DoSellerDialogClass
{
	GtkVBoxClass parent_class;
};

#define DO_SELLER_DIALOG_FLAGS_DEFAULT 0

GType		do_seller_dialog_get_type(void);

GtkWidget  *do_seller_dialog_new(gint report);

G_END_DECLS

#endif /* !DO_SELLER_DIALOG_H */
