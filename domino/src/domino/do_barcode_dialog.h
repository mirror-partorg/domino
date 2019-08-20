
#ifndef DO_BARCODE_DIALOG_H
#define DO_BARCODE_DIALOG_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_BARCODE_DIALOG         (do_barcode_dialog_get_type ())
#define DO_BARCODE_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_BARCODE_DIALOG, DoBarcodeDialog))
#define DO_BARCODE_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_BARCODE_DIALOG, DoBarcodeDialogClass))
#define DO_IS_BARCODE_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_BARCODE_DIALOG))
#define DO_IS_BARCODE_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_BARCODE_DIALOG))
#define DO_BARCODE_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_BARCODE_DIALOG, DoBarcodeDialogClass))

typedef struct _DoBarcodeDialog		DoBarcodeDialog;
typedef struct _DoBarcodeDialogPrivate	DoBarcodeDialogPrivate;
typedef struct _DoBarcodeDialogClass	DoBarcodeDialogClass;

struct _DoBarcodeDialog
{
	GtkDialog parent_instance;
};

struct _DoBarcodeDialogClass
{
	GtkDialogClass parent_class;
};

#define DO_BARCODE_DIALOG_FLAGS_DEFAULT 0

GType		do_barcode_dialog_get_type(void);

GtkWidget  *do_barcode_dialog_new(GtkWidget *parent, const gchar *alias_name, const gchar *barcode);

gint        do_barcode_dialog_run(DoBarcodeDialog *dialog);

G_END_DECLS

#endif /* !DO_BARCODE_DIALOG_H */
