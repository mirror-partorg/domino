#include "c_util.h"
#include <slang.h>
#include <string.h>
#include <glib.h>

#ifdef USE_UTF8
static const char utf8_skip_data[256] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

const char * const utf8_skip = utf8_skip_data;
inline int c_chlen(int ch)
{
    return utf8_skip[(const unsigned char)ch];
}
char *c_strncpy(char *dest, const char *src, size_t n)
{
    int len = 0, i;
    SLuchar_Type *p = (SLuchar_Type *)src;
    SLuchar_Type *m = (SLuchar_Type *)(src + strlen(src));
    for (i = 0; i < n && *p; i++)
      p = SLutf8_skip_char(p, m);
    len = (char*)p - src;

    return strncpy(dest, src, len);
}
inline int  c_strlen(const char *str)
{
    return SLutf8_strlen((SLuchar_Type *)str, 0);
}
int c_isalnum(int ch)
{
    SLuchar_Type *u = (SLuchar_Type *)&ch;
    SLuchar_Type *umax = (SLuchar_Type *)((char*)&ch + c_chlen(ch));
    SLwchar_Type w;
    if (SLutf8_decode(u, umax, &w, NULL) == NULL)
        return 0;
    return SLwchar_isalnum(w);
}

int c_isalpha(int ch)
{
    SLuchar_Type *u = (SLuchar_Type *)&ch;
    SLuchar_Type *umax = (SLuchar_Type *)((char*)&ch + c_chlen(ch));
    SLwchar_Type w;
    if (SLutf8_decode(u, umax, &w, NULL) == NULL)
        return 0;
    return SLwchar_isalpha(w);
}
int c_isdigit(int ch)
{
    SLuchar_Type *u = (SLuchar_Type *)&ch;
    SLuchar_Type *umax = (SLuchar_Type *)((char*)&ch + c_chlen(ch));
    SLwchar_Type w;
    if (SLutf8_decode(u, umax, &w, NULL) == NULL)
        return 0;
    return SLwchar_isdigit(w);
}

int c_isspace(int ch)
{
    SLuchar_Type *u = (SLuchar_Type *)&ch;
    SLuchar_Type *umax = (SLuchar_Type *)((char*)&ch + c_chlen(ch));
    SLwchar_Type w;
    if (SLutf8_decode(u, umax, &w, NULL) == NULL)
        return 0;
    return SLwchar_isspace(w);
}
int c_toupper(int ch)
{
    int buf;
    gunichar  out;
    gchar *p = (gchar*)&ch;
    out = g_unichar_toupper(g_utf8_get_char(p));
    g_unichar_to_utf8(out, (gchar*)&buf);
    return buf;
}

char *c_str_at(const char *str, int indx)
{
    int i;
    SLuchar_Type *p = (SLuchar_Type *)str;
    SLuchar_Type *m = (SLuchar_Type *)(str + strlen(str));
    for (i = 0; i < indx && *p; i++)
      p = SLutf8_skip_char(p, m);
    return (char*)p;
}

char *c_next_ch(const char *str)
{
    SLuchar_Type *p = (SLuchar_Type *)str;
    SLuchar_Type *m = (SLuchar_Type *)(str + strlen(str));
    return (char*)SLutf8_skip_char(p, m);
}

#endif // USE_UTF8

static int keyboard_ru[3][34] =
{ {'q','w','e','r','t','y','u','i','o','p','[',']','a','s','d','f','g','h','j','k','l',';','\'','z','x','c','v','b','n','m',',','.','/','-'},
  {'Q','W','E','R','T','Y','U','I','O','P','{','}','A','S','D','F','G','H','J','K','L',':','"','Z','X','C','V','B','N','M','<','>','?','-'},
#ifdef USE_UTF8
  {0x99D0,0xA6D0,0xA3D0,0x9AD0,0x95D0,0x9DD0,0x93D0,0xA8D0,0xA9D0,0x97D0,0xA5D0,0xAAD0,0xA4D0,0xABD0,0x92D0,0x90D0,
   0x9FD0,0xA0D0,0x9ED0,0x9BD0,0x94D0,0x96D0,0xADD0,0xAFD0,0xA7D0,0xA1D0,0x9CD0,0x98D0,0xA2D0,0xACD0,0x91D0,0xAED0,
   '.',0x2D}
#else
  {0xC9,0xD6,0xD3,0xCA,0xC5,0xCD,0xC3,0xD8,0xD9,0xC7,0xD5,0xDA,0xD4,0xDB,0xC2,0xC0,
   0xCF,0xD0,0xCE,0xCB,0xC4,0xC6,0xDD,0xDF,0xD7,0xD1,0xCC,0xC8,0xD2,0xDC,0xC1,0xDE,
   '.',0x2D
  }
#endif
};
int c_upper_ru(int ch)
{

    int i, k;
    if (c_isspace(ch))
        return ' ';
    if (c_isdigit(ch))
        return ch;

    for (k = 0; k < 2; k++)
        for (i = 0; i < 34; i++)
            if (ch == keyboard_ru[k][i])
                return keyboard_ru[2][i];
    if ( c_isalpha(ch) )
        return c_toupper(ch);

    return 0;
}

