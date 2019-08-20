
#ifndef DO_ENTRY_BOX_H
#define DO_ENTRY_BOX_H

#include <gtk/gtk.h>
#include "do_entry_date.h"

G_BEGIN_DECLS

#define DO_TYPE_ENTRY_BOX         (do_entry_box_get_type ())
#define DO_ENTRY_BOX(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_ENTRY_BOX, DoEntryBox))
#define DO_ENTRY_BOX_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY_BOX, DoEntryBoxClass))
#define DO_IS_ENTRY_BOX(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_ENTRY_BOX))
#define DO_IS_ENTRY_BOX_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_ENTRY_BOX))
#define DO_ENTRY_BOX_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_ENTRY_BOX, DoEntryBoxClass))
#define DO_ENTRY_IFACE(k)		   (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY, DoEntryIface))

typedef struct _DoEntryBox		    DoEntryBox;
typedef struct _DoEntryBoxPrivate	DoEntryBoxPrivate;
typedef struct _DoEntryBoxClass	DoEntryBoxClass;

struct _DoEntryBox
{
	GtkHBox parent_instance;
};

struct _DoEntryBoxClass
{
	GtkHBoxClass parent_class;
};


GType		do_entry_box_get_type(void);

GtkWidget  *do_entry_box_new(DoEntryDateFlags flags);
GtkWidget  *do_entry_box_entry(DoEntryBox *box);
GtkWidget  *do_entry_box_button(DoEntryBox *box);

G_END_DECLS

#endif /* !DO_ENTRY_BOX_H */
