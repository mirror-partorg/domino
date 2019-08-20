
#ifndef DO_PV_DIALOG_H
#define DO_PV_DIALOG_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_PV_DIALOG         (do_pv_dialog_get_type ())
#define DO_PV_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_PV_DIALOG, DoPvDialog))
#define DO_PV_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_PV_DIALOG, DoPvDialogClass))
#define DO_IS_PV_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_PV_DIALOG))
#define DO_IS_PV_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_PV_DIALOG))
#define DO_PV_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_PV_DIALOG, DoPvDialogClass))

typedef struct _DoPvDialog		DoPvDialog;
typedef struct _DoPvDialogPrivate	DoPvDialogPrivate;
typedef struct _DoPvDialogClass	DoPvDialogClass;

struct _DoPvDialog
{
	GtkDialog parent_instance;
};

struct _DoPvDialogClass
{
	GtkDialogClass parent_class;
};

#define DO_PV_DIALOG_FLAGS_DEFAULT 0

GType		do_pv_dialog_get_type(void);

GtkWidget  *do_pv_dialog_new(GtkWidget *parent, const gchar *alias_name, DoContext *context);

gint        do_pv_dialog_run(DoPvDialog *dialog);

gchar      *do_pv_dialog_html(DoPvDialog *dialog, int index, int duplex);

G_END_DECLS

#endif /* !DO_PV_DIALOG_H */
