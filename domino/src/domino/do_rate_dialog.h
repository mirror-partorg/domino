
#ifndef DO_RATE_DIALOG_H
#define DO_RATE_DIALOG_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_RATE_DIALOG         (do_rate_dialog_get_type ())
#define DO_RATE_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_RATE_DIALOG, DoRateDialog))
#define DO_RATE_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_RATE_DIALOG, DoRateDialogClass))
#define DO_IS_RATE_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_RATE_DIALOG))
#define DO_IS_RATE_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_RATE_DIALOG))
#define DO_RATE_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_RATE_DIALOG, DoRateDialogClass))

typedef struct _DoRateDialog		DoRateDialog;
typedef struct _DoRateDialogPrivate	DoRateDialogPrivate;
typedef struct _DoRateDialogClass	DoRateDialogClass;

typedef enum
{
	DO_RATE_DIALOG_FLAGS_READ_ONLY	= 1 << 0,

} DoRateDialogFlags;

struct _DoRateDialog
{
	GtkDialog parent_instance;
};

struct _DoRateDialogClass
{
	GtkDialogClass parent_class;
};

#define DO_RATE_DIALOG_FLAGS_DEFAULT 0

GType		do_rate_dialog_get_type(void);

GtkWidget  *do_rate_dialog_new(GtkWidget *parent, const gchar *alias_name, DoRateDialogFlags flags, DoDialogAction action, DoContext *context);

gint        do_rate_dialog_run(DoRateDialog *dialog);

G_END_DECLS

#endif /* !DO_RATE_DIALOG_H */
