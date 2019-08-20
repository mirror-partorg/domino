
#ifndef DO_ENTRY_DATE_BOX_H
#define DO_ENTRY_DATE_BOX_H

#include <gtk/gtk.h>
#include "do_entry_date.h"
#include "do_entry_box.h"

G_BEGIN_DECLS

#define DO_TYPE_ENTRY_DATE_BOX         (do_entry_date_box_get_type ())
#define DO_ENTRY_DATE_BOX(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_ENTRY_DATE_BOX, DoEntryDateBox))
#define DO_ENTRY_DATE_BOX_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY_DATE_BOX, DoEntryDateBoxClass))
#define DO_IS_ENTRY_DATE_BOX(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_ENTRY_DATE_BOX))
#define DO_IS_ENTRY_DATE_BOX_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_ENTRY_DATE_BOX))
#define DO_ENTRY_DATE_BOX_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_ENTRY_DATE_BOX, DoEntryDateBoxClass))
#define DO_ENTRY_IFACE(k)		   (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY, DoEntryIface))

typedef struct _DoEntryDateBox		    DoEntryDateBox;
typedef struct _DoEntryDateBoxPrivate	DoEntryDateBoxPrivate;
typedef struct _DoEntryDateBoxClass	DoEntryDateBoxClass;

struct _DoEntryDateBox
{
	DoEntryBox parent_instance;
};

struct _DoEntryDateBoxClass
{
	DoEntryBoxClass parent_class;
};

GType		do_entry_date_box_get_type(void);
GtkWidget  *do_entry_date_box_new(DoEntryDateFlags flags);

G_END_DECLS

#endif /* !DO_ENTRY_DATE_BOX_H */
