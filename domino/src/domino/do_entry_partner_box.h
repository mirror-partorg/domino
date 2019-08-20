
#ifndef DO_ENTRY_PARTNER_BOX_H
#define DO_ENTRY_PARTNER_BOX_H

#include <gtk/gtk.h>
#include "do_entry_partner.h"

G_BEGIN_DECLS

#define DO_TYPE_ENTRY_PARTNER_BOX         (do_entry_partner_box_get_type ())
#define DO_ENTRY_PARTNER_BOX(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_ENTRY_PARTNER_BOX, DoEntryPartnerBox))
#define DO_ENTRY_PARTNER_BOX_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY_PARTNER_BOX, DoEntryPartnerBoxClass))
#define DO_IS_ENTRY_PARTNER_BOX(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_ENTRY_PARTNER_BOX))
#define DO_IS_ENTRY_PARTNER_BOX_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_ENTRY_PARTNER_BOX))
#define DO_ENTRY_PARTNER_BOX_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_ENTRY_PARTNER_BOX, DoEntryPartnerBoxClass))
#define DO_ENTRY_IFACE(k)		   (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY, DoEntryIface))

typedef struct _DoEntryPartnerBox		    DoEntryPartnerBox;
typedef struct _DoEntryPartnerBoxPrivate	DoEntryPartnerBoxPrivate;
typedef struct _DoEntryPartnerBoxClass	DoEntryPartnerBoxClass;

struct _DoEntryPartnerBox
{
	GtkHBox parent_instance;
};

struct _DoEntryPartnerBoxClass
{
	GtkHBoxClass parent_class;
};


GType		do_entry_partner_box_get_type(void);

GtkWidget  *do_entry_partner_box_new(const gchar *alias_name, DoEntryPartnerFlags flags);
GtkWidget  *do_entry_partner_box_entry(DoEntryPartnerBox *box);
GtkWidget  *do_entry_partner_box_button(DoEntryPartnerBox *box);

G_END_DECLS

#endif /* !DO_ENTRY_PARTNER_BOX_H */
