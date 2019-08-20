
#ifndef DO_ENUM_BOX_H
#define DO_ENUM_BOX_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_ENUM_BOX         (do_enum_box_get_type ())
#define DO_ENUM_BOX(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_ENUM_BOX, DoEnumBox))
#define DO_ENUM_BOX_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENUM_BOX, DoEnumBoxClass))
#define DO_IS_ENUM_BOX(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_ENUM_BOX))
#define DO_IS_ENUM_BOX_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_ENUM_BOX))
#define DO_ENUM_BOX_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_ENUM_BOX, DoEnumBoxClass))
#define DO_ENTRY_IFACE(k)		   (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY, DoEntryIface))

typedef struct _DoEnumBox		    DoEnumBox;
typedef struct _DoEnumBoxPrivate	DoEnumBoxPrivate;
typedef struct _DoEnumBoxClass	DoEnumBoxClass;

struct _DoEnumBox
{
	GtkComboBox parent_instance;
};

struct _DoEnumBoxClass
{
	GtkComboBoxClass parent_class;
};

GType		do_enum_box_get_type(void);

GtkWidget  *do_enum_box_new(const gchar *enum_name);

G_END_DECLS

#endif /* !DO_ENUM_BOX_H */
