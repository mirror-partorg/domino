
#ifndef DO_UNIT_PERIOD_DIALOG_H
#define DO_UNIT_PERIOD_DIALOG_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_UNIT_PERIOD_DIALOG         (do_unit_period_dialog_get_type ())
#define DO_UNIT_PERIOD_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_UNIT_PERIOD_DIALOG, DoUnitPeriodDialog))
#define DO_UNIT_PERIOD_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_UNIT_PERIOD_DIALOG, DoUnitPeriodDialogClass))
#define DO_IS_UNIT_PERIOD_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_UNIT_PERIOD_DIALOG))
#define DO_IS_UNIT_PERIOD_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_UNIT_PERIOD_DIALOG))
#define DO_UNIT_PERIOD_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_UNIT_PERIOD_DIALOG, DoUnitPeriodDialogClass))

typedef struct _DoUnitPeriodDialog		DoUnitPeriodDialog;
typedef struct _DoUnitPeriodDialogPrivate	DoUnitPeriodDialogPrivate;
typedef struct _DoUnitPeriodDialogClass	DoUnitPeriodDialogClass;

struct _DoUnitPeriodDialog
{
	GtkDialog parent_instance;
};

struct _DoUnitPeriodDialogClass
{
	GtkDialogClass parent_class;
};

#define DO_UNIT_PERIOD_DIALOG_FLAGS_DEFAULT 0

GType		do_unit_period_dialog_get_type(void);

GtkWidget  *do_unit_period_dialog_new(GtkWidget *parent, const gchar *name, const gchar *title);

gint        do_unit_period_dialog_run(DoUnitPeriodDialog *dialog);

gchar      *do_unit_period_dialog_get_units(DoUnitPeriodDialog *dialog);
GDate       do_unit_period_dialog_get_date1(DoUnitPeriodDialog *dialog);
GDate       do_unit_period_dialog_get_date2(DoUnitPeriodDialog *dialog);


G_END_DECLS

#endif /* !DO_UNIT_PERIOD_DIALOG_H */
