
#ifndef DO_PKO_DIALOG_H
#define DO_PKO_DIALOG_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_PKO_DIALOG         (do_pko_dialog_get_type ())
#define DO_PKO_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_PKO_DIALOG, DoPkoDialog))
#define DO_PKO_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_PKO_DIALOG, DoPkoDialogClass))
#define DO_IS_PKO_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_PKO_DIALOG))
#define DO_IS_PKO_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_PKO_DIALOG))
#define DO_PKO_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_PKO_DIALOG, DoPkoDialogClass))

typedef struct _DoPkoDialog		DoPkoDialog;
typedef struct _DoPkoDialogPrivate	DoPkoDialogPrivate;
typedef struct _DoPkoDialogClass	DoPkoDialogClass;

struct _DoPkoDialog
{
	GtkDialog parent_instance;
};

struct _DoPkoDialogClass
{
	GtkDialogClass parent_class;
};

#define DO_PKO_DIALOG_FLAGS_DEFAULT 0

GType		do_pko_dialog_get_type(void);

GtkWidget  *do_pko_dialog_new(GtkWidget *parent, const gchar *alias_name, DoContext *context);

gint        do_pko_dialog_run(DoPkoDialog *dialog);

gchar      *do_pko_dialog_html(DoPkoDialog *dialog);

G_END_DECLS

#endif /* !DO_PKO_DIALOG_H */
