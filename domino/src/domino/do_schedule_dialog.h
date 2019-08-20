
#ifndef DO_SCHEDULE_DIALOG_H
#define DO_SCHEDULE_DIALOG_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_SCHEDULE_DIALOG         (do_schedule_dialog_get_type ())
#define DO_SCHEDULE_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_SCHEDULE_DIALOG, DoScheduleDialog))
#define DO_SCHEDULE_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_SCHEDULE_DIALOG, DoScheduleDialogClass))
#define DO_IS_SCHEDULE_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_SCHEDULE_DIALOG))
#define DO_IS_SCHEDULE_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_SCHEDULE_DIALOG))
#define DO_SCHEDULE_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_SCHEDULE_DIALOG, DoScheduleDialogClass))

typedef struct _DoScheduleDialog		DoScheduleDialog;
typedef struct _DoScheduleDialogPrivate	DoScheduleDialogPrivate;
typedef struct _DoScheduleDialogClass	DoScheduleDialogClass;

struct _DoScheduleDialog
{
	GtkDialog parent_instance;
};

struct _DoScheduleDialogClass
{
	GtkDialogClass parent_class;
};

#define DO_SCHEDULE_DIALOG_FLAGS_DEFAULT 0

GType		do_schedule_dialog_get_type(void);

GtkWidget  *do_schedule_dialog_new(GtkWidget *parent);

gint        do_schedule_dialog_run(DoScheduleDialog *dialog);

gchar      *do_schedule_dialog_get_units(DoScheduleDialog *dialog);
GList      *do_schedule_dialog_get_schedule_types(DoScheduleDialog *dialog);
GDate       do_schedule_dialog_get_date1(DoScheduleDialog *dialog);
GDate       do_schedule_dialog_get_date2(DoScheduleDialog *dialog);


G_END_DECLS

#endif /* !DO_SCHEDULE_DIALOG_H */
