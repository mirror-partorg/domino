
#ifndef DO_ENTRY_CALC_H
#define DO_ENTRY_CALC_H

#include <gtk/gtk.h>
#include "do_entry.h"

G_BEGIN_DECLS

#define DO_TYPE_ENTRY_CALC         (do_entry_calc_get_type ())
#define DO_ENTRY_CALC(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_ENTRY_CALC, DoEntryCalc))
#define DO_ENTRY_CALC_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY_CALC, DoEntryCalcClass))
#define DO_IS_ENTRY_CALC(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_ENTRY_CALC))
#define DO_IS_ENTRY_CALC_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_ENTRY_CALC))
#define DO_ENTRY_CALC_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_ENTRY_CALC, DoEntryCalcClass))
#define DO_ENTRY_IFACE(k)		   (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ENTRY, DoEntryIface))
#define DO_TYPE_ENTRY_CALC_FLAGS do_entry_calc_flags_get_type()

typedef struct _DoEntryCalc		    DoEntryCalc;
typedef struct _DoEntryCalcPrivate	DoEntryCalcPrivate;
typedef struct _DoEntryCalcClass	DoEntryCalcClass;

#define DO_ENTRY_CALC_FLAGS_DEFAULT DO_ENTRY_CALC_FLAGS_OUR_CENTURY

struct _DoEntryCalc
{
	DoEntry parent_instance;
};

struct _DoEntryCalcClass
{
	DoEntryClass parent_class;
};


GType		do_entry_calc_get_type(void);
GtkWidget  *do_entry_calc_new(void);

G_END_DECLS

#endif /* !DO_ENTRY_CALC_H */
