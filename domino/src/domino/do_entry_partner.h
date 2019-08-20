
#ifndef DO_ENTRY_PARTNER_H
#define DO_ENTRY_PARTNER_H

#include <gtk/gtk.h>
#include "do_entry.h"

G_BEGIN_DECLS

#define DO_TYPE_ENTRY_PARTNER         (do_entry_partner_get_type ())
#define DO_ENTRY_PARTNER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_ENTRY_PARTNER, DoEntryPartner))
#define DO_ENTRY_PARTNER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY_PARTNER, DoEntryPartnerClass))
#define DO_IS_ENTRY_PARTNER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_ENTRY_PARTNER))
#define DO_IS_ENTRY_PARTNER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_ENTRY_PARTNER))
#define DO_ENTRY_PARTNER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_ENTRY_PARTNER, DoEntryPartnerClass))
#define DO_ENTRY_IFACE(k)		   (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY, DoEntryIface))
#define DO_TYPE_ENTRY_PARTNER_FLAGS do_entry_partner_flags_get_type()

typedef struct _DoEntryPartner		    DoEntryPartner;
typedef struct _DoEntryPartnerPrivate	DoEntryPartnerPrivate;
typedef struct _DoEntryPartnerClass	DoEntryPartnerClass;

typedef enum
{
	DO_ENTRY_PARTNER_FLAGS_SEARCH = 1 << 0,
	DO_ENTRY_PARTNER_FLAGS_EMPTY = 1 << 1,
} DoEntryPartnerFlags;

#define DO_ENTRY_PARTNER_FLAGS_DEFAULT DO_ENTRY_PARTNER_FLAGS_SEARCH

struct _DoEntryPartner
{
	DoEntry parent_instance;
};

struct _DoEntryPartnerClass
{
	DoEntryClass parent_class;
};


GType		do_entry_partner_get_type(void);
GType		do_entry_partner_flags_get_type(void);
GtkWidget  *do_entry_partner_new(const gchar *alias_name, DoEntryPartnerFlags flags);
void        do_entry_partner_model_fill(DoEntryPartner *partner, GtkListStore *store);
gboolean    do_entry_partner_add_partner_by_name(DoEntryPartner *partner, const gchar *name);

G_END_DECLS

#endif /* !DO_ENTRY_PARTNER_H */
