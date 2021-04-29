
#ifndef DO_ENTRY_H
#define DO_ENTRY_H

#include <gtk/gtk.h>
#include "do_validate.h"

G_BEGIN_DECLS

#define DO_TYPE_ENTRY         (do_entry_get_type ())
#define DO_ENTRY(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_ENTRY, DoEntry))
#define DO_ENTRY_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY, DoEntryClass))
#define DO_IS_ENTRY(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_ENTRY))
#define DO_IS_ENTRY_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_ENTRY))

typedef struct _DoEntry		DoEntry;
typedef struct _DoEntryPrivate	DoEntryPrivate;
typedef struct _DoEntryClass	DoEntryClass;
typedef struct _DoEntryIface	DoEntryIface;

struct _DoEntry
{
	GtkEntry parent_instance;
};

struct _DoEntryClass
{
	GtkEntryClass parent_class;

    void (* substitution) (DoEntry  *entry);
};



GType		do_entry_get_type(void);

GtkWidget  *do_entry_new();

void        do_entry_set_regex_mask(DoEntry  *entry, const char* mask);

gboolean    do_entry_valid(DoEntry  *entry);

void        do_entry_set_validate_func(DoEntry *entry, DoValidateFunc func, gpointer data);

G_END_DECLS

#endif /* !DO_ENTRY_H */
