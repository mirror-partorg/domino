
#ifndef DO_ENTRY_DATE_H
#define DO_ENTRY_DATE_H

#include <gtk/gtk.h>
#include "do_entry.h"

G_BEGIN_DECLS

#define DO_TYPE_ENTRY_DATE         (do_entry_date_get_type ())
#define DO_ENTRY_DATE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_ENTRY_DATE, DoEntryDate))
#define DO_ENTRY_DATE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY_DATE, DoEntryDateClass))
#define DO_IS_ENTRY_DATE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_ENTRY_DATE))
#define DO_IS_ENTRY_DATE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_ENTRY_DATE))
#define DO_ENTRY_DATE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_ENTRY_DATE, DoEntryDateClass))
#define DO_ENTRY_IFACE(k)		   (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY, DoEntryIface))
#define DO_TYPE_ENTRY_DATE_FLAGS do_entry_date_flags_get_type()

typedef struct _DoEntryDate		    DoEntryDate;
typedef struct _DoEntryDatePrivate	DoEntryDatePrivate;
typedef struct _DoEntryDateClass	DoEntryDateClass;

typedef enum
{
	DO_ENTRY_DATE_FLAGS_LIFETIME_MODE = 1 << 0,
	DO_ENTRY_DATE_FLAGS_OUR_CENTURY = 1 << 1,
	DO_ENTRY_DATE_FLAGS_EMPTY = 1 << 2,
} DoEntryDateFlags;

#define DO_ENTRY_DATE_FLAGS_DEFAULT DO_ENTRY_DATE_FLAGS_OUR_CENTURY

struct _DoEntryDate
{
	DoEntry parent_instance;
};

struct _DoEntryDateClass
{
	DoEntryClass parent_class;
};


GType		do_entry_date_get_type(void);
GType		do_entry_date_flags_get_type(void);

GtkWidget  *do_entry_date_new(DoEntryDateFlags flags);

void        do_entry_date_set_today(DoEntryDate *entry);

G_END_DECLS

#endif /* !DO_ENTRY_DATE_H */
