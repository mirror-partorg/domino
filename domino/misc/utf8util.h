#ifndef TEMPLATE_H_INCLUDED
#define TEMPLATE_H_INCLUDED


#include <glib.h>

#ifdef __cplusplus
extern "C"
{
#endif
GIConv do_utf8_make_compatibility_cp866_init();
void   do_utf8_make_compatibility_cp866_close(GIConv conv);
gchar *do_utf8_make_compatibility_cp866(GIConv conv, const gchar* text);

#ifdef __cplusplus
}
#endif



#endif // TEMPLATE_H_INCLUDED
