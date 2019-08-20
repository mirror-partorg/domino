

#include "utf8util.h"
#include <dolib.h>
#include <glib.h>

#define CHANGE_TOTAL 6
static const gunichar subst[CHANGE_TOTAL*2] =
{8211, 45,
 8482,  32,
 171, 32,
 '\n', 32,
 187, '"',
 174, '"',
};
//{"–", "-"
//};
GIConv do_utf8_make_compatibility_cp866_init()
{
    GIConv result;
    result = g_iconv_open("CP866", "UTF-8");
    if ( result == (GIConv)-1 )
        do_log(LOG_ERR, "Error open iconv from CP866 to UTF-8");
    return result;
}
void do_utf8_make_compatibility_cp866_close(GIConv conv)
{
    if ( conv != (GIConv)-1 )
        g_iconv_close(conv);
}
gchar *do_utf8_make_compatibility_cp866(GIConv conv, const gchar* text)
{
    int j;
    gchar *result = do_malloc(sizeof (gunichar) * strlen(text) + 1);
    gchar *p;
    gchar buffer[7];
    gint len;
    gunichar out;
    gint nlen = 0;
    gchar *ch;
    gboolean sub;

    for (p = (char*)text; p && *p !='\0'; p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        sub = FALSE;
        for (j = 0; j < CHANGE_TOTAL; j++)
            if ( subst[j*2] == out ) {
                out = subst[j*2 + 1];
                sub = TRUE;
                break;
            }
        if ( sub || conv == (GIConv)-1 )
            nlen += g_unichar_to_utf8(out, result + nlen);
        else {
            len  = g_unichar_to_utf8(out, buffer);
            ch = g_convert_with_iconv(buffer, len, conv, NULL, NULL, NULL);
            if ( !ch )
                result[nlen++] = ' ';
            else {
                memcpy(result + nlen, buffer, len);
                nlen += len;
            }
        }
    }
    result[nlen] = '\0';
    return result;
}
