#ifndef DO_PRINT_H
#define DO_PRINT_H

#include <glib-object.h>
#include <glib.h>
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_PRINT		(do_print_get_type ())
#define DO_PRINT(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_PRINT, DoPrint))
#define DO_PRINT_IFACE(k)		(G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_PRINT, DoPrintIface))
#define DO_IS_PRINT(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_PRINT))
#define DO_IS_PRINT_IFACE(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_PRINT))
#define DO_PRINT_GET_IFACE(inst)	(G_TYPE_INSTANCE_GET_INTERFACE ((inst), DO_TYPE_PRINT, DoPrintIface))

typedef struct _DoPrint	DoPrint;
typedef struct _DoPrintIface DoPrintIface;

struct _DoPrintIface
{
	GTypeInterface base_iface;
	gchar*  (* get_html)  (DoPrint *print);

	/* Signals */

};

GType	   do_print_get_type		(void);

gchar 	  *do_print_get_html(DoPrint *print);

G_END_DECLS

#endif
