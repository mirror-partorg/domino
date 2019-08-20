
#ifndef DO_UNIT_BOX_H
#define DO_UNIT_BOX_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_UNIT_BOX         (do_unit_box_get_type ())
#define DO_UNIT_BOX(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_UNIT_BOX, DoUnitBox))
#define DO_UNIT_BOX_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_UNIT_BOX, DoUnitBoxClass))
#define DO_IS_UNIT_BOX(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_UNIT_BOX))
#define DO_IS_UNIT_BOX_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_UNIT_BOX))
#define DO_UNIT_BOX_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_UNIT_BOX, DoUnitBoxClass))
#define DO_ENTRY_IFACE(k)		   (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY, DoEntryIface))

typedef struct _DoUnitBox		    DoUnitBox;
typedef struct _DoUnitBoxPrivate	DoUnitBoxPrivate;
typedef struct _DoUnitBoxClass	DoUnitBoxClass;

struct _DoUnitBox
{
	GtkComboBox parent_instance;
};

struct _DoUnitBoxClass
{
	GtkComboBoxClass parent_class;
};

GType		do_unit_box_get_type(void);

GtkWidget  *do_unit_box_new();

G_END_DECLS

#endif /* !DO_UNIT_BOX_H */
