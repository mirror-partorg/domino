#ifndef DO_VALIDATE_H
#define DO_VALIDATE_H

#include <glib-object.h>
#include <glib.h>
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_VALIDATE		(do_validate_get_type ())
#define DO_VALIDATE(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_VALIDATE, DoValidate))
#define DO_VALIDATE_IFACE(k)	(G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_VALIDATE, DoValidateIface))
#define DO_IS_VALIDATE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_VALIDATE))
#define DO_IS_VALIDATE_IFACE(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_VALIDATE))
#define DO_VALIDATE_GET_IFACE(inst)	(G_TYPE_INSTANCE_GET_INTERFACE ((inst), DO_TYPE_VALIDATE, DoValidateIface))


typedef struct _DoValidate	DoValidate;
typedef struct _DoValidateIface	DoValidateIface;

struct _DoValidateIface
{
	GTypeInterface base_iface;
    gboolean (* valid)(DoValidate *iface);
    void     (* completion)(DoValidate *iface);
    gboolean (* substitution)(DoValidate *iface);
};

typedef gboolean (*DoValidateFunc) (const gchar *text, gpointer);

GType		do_validate_get_type		(void);

gboolean  do_validate_valid(DoValidate *iface);
gboolean  do_validate_substitution(DoValidate *iface);
void      do_validate_completion(DoValidate *validate);

G_END_DECLS

#endif
