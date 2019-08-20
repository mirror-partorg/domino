#ifndef DOCUMENT_H_INCLUDED
#define DOCUMENT_H_INCLUDED


#include <gtk/gtk.h>
#include "exchange.h"

#ifdef __cplusplus
extern "C" {
#endif

gboolean make_goods_documents(const gchar *aliases, GSList *objs, struct tm *tm1, struct tm *tm2, GSList *units, gboolean protocol, const gchar *filename, ExchangeObject *user);
gboolean make_bank_documents(const gchar *aliases, struct tm *tm1, struct tm *tm2, const gchar *filename, const gchar *user_name);



#ifdef __cplusplus
}
#endif


#endif // DOCUMENT_H_INCLUDED
