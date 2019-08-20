
#ifndef DO_CHECK_DIALOG_H
#define DO_CHECK_DIALOG_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"
#include "hig.h"

G_BEGIN_DECLS

#define DO_TYPE_CHECK_DIALOG         (do_check_dialog_get_type ())
#define DO_CHECK_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_CHECK_DIALOG, DoCheckDialog))
#define DO_CHECK_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_CHECK_DIALOG, DoCheckDialogClass))
#define DO_IS_CHECK_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_CHECK_DIALOG))
#define DO_IS_CHECK_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_CHECK_DIALOG))
#define DO_CHECK_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_CHECK_DIALOG, DoCheckDialogClass))

typedef struct _DoCheckDialog		DoCheckDialog;
typedef struct _DoCheckDialogPrivate	DoCheckDialogPrivate;
typedef struct _DoCheckDialogClass	DoCheckDialogClass;

typedef enum
{
	DO_CHECK_DIALOG_FLAGS_FROM_CURRENT	= 1 << 0,

} DoCheckDialogFlags;

struct _DoCheckDialog
{
	GtkDialog parent_instance;
};

struct _DoCheckDialogClass
{
	GtkDialogClass parent_class;
};

#define DO_CHECK_DIALOG_FLAGS_DEFAULT 0

GType		do_check_dialog_get_type(void);

GtkWidget  *do_check_dialog_new(GtkWidget *parent, const gchar *alias_name, DoCheckDialogFlags flags, DoContext *context);

gint        do_check_dialog_run(DoCheckDialog *dialog);

G_END_DECLS

#endif /* !DO_CHECK_DIALOG_H */
