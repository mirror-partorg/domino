#ifndef PARTNER_H_INCLUDED
#define PARTNER_H_INCLUDED

#include <gtk/gtk.h>
#include "exchange.h"

#ifdef __cplusplus
extern "C" {
#endif

gboolean make_partners(const gchar *aliases, GSList *objs);

#ifdef __cplusplus
}
#endif



#endif // PARTNER_H_INCLUDED
