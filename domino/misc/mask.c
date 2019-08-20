#include "mask.h"
#include "csv_util.h"
#include <stdio.h>
#include <dolib.h>

#define MAX_FILE_LEN 512

#ifdef _LINUX
#include <wchar.h>
#include <wctype.h>
#include <ctype.h>

static char* atowupper(char* str)
{
    if( MB_CUR_MAX > 1 )
    {
        int i,len = strlen(str);
        wchar_t wc;
        mbstate_t cur_state;
        memset(&cur_state, 0, sizeof(mbstate_t));
        for(i = 0; i <= len;)
        {
            size_t mbclen = mbrtowc(&wc, str + i, len - i, &cur_state);
            if (mbclen == (size_t) -1 || mbclen == (size_t) -2 || mbclen == 0 )
               mbclen = 1;
            else
            {
                if (iswlower((wint_t)wc))
                {
                    wc = towupper((wint_t)wc);
                    if (wcrtomb(str + i, wc, &cur_state) == -1)
                	return str;
                }
            }
            i += mbclen;
        }
    }
    else {
        char *cp;
        for(cp=str; *cp; cp++) if (islower(*cp)) *cp = toupper(*cp);
    }
    return str;
}
#elif defined(_WIN32)
#include <windows.h>
static char* atowupper(char* str)
{
    return AnsiUpper(str);
}
#endif

mask_t *mask_new(const char* mask, int uppercase)
{
    mask_t *res = (mask_t*) do_malloc(sizeof(mask_t));
    char *ch = (char*)mask, *head;
    for (; *ch != '\0' && !CSV_SPACE(ch); ch++);
    strncpy(res->format, mask, ch - mask);
    res->format[ch - mask] = '\0';
    for (; *ch != '\0' && CSV_SPACE(ch); ch++);
    res->total = 0;
    while ((*ch != '\0') && (res->total < MAX_VARIABLE))    {
        head = ch;
        for (; *ch != '\0' && !CSV_SPACE(ch); ch++);
        res->name[res->total] = (char*) do_malloc(sizeof(char)*(ch - head + 1));
        strncpy(res->name[res->total], head, ch - head);
        res->name[res->total][ch - head] = '\0';
        for (; *ch != '\0' && CSV_SPACE(ch); ch++);
        res->total++;
    }
    if ((res->uppercase = uppercase) == 1) {
        atowupper(res->format);
        int code = 0;
        char *ch;
        for (ch = res->format; *ch != '\0'; ch++ )
            if (code && *ch == 'S') {
                *ch = 's';
                code = 0;
            }
            else
            if (*ch == '%') code = 1;
    }
    return res;
}
void mask_free(mask_t *mask)
{
    int i;
    for (i = 0; i < mask->total; i++)
        do_free(mask->name[i]);
    do_free(mask);
}
int  mask_valid(mask_t *mask, const char* name)
{
    char *filename = (char*)name;
    if (mask->uppercase) {
        filename = do_strdup(name);
        atowupper(filename);
    }
    int fl;
    char *format = (char*)do_malloc(strlen(mask->format) + 1 + strlen("%s"));
    strcpy(format, mask->format);
    strcat(format,"%s");
    if (!mask->total)
        fl = !strcmp(filename, format);
    else
        fl = (mask->total == sscanf(filename, format, mask->value[0], mask->value[1], mask->value[2], mask->value[3], mask->value[4], mask->value[5], mask->value[6], mask->value[7], mask->value[8], mask->value[9], mask->value[10]));
    if (mask->uppercase)
        do_free(filename);
    do_free(format);
    return fl;
}
char *mask_out(mask_t *mask, const char *format, int total_value)
{
    if (mask->total < total_value)
        return NULL;
    char *filename = (char*)do_malloc(MAX_FILE_LEN);
    if (!total_value)
        strcpy(filename, format);
    else
        sprintf(filename, format, mask->value[0], mask->value[1], mask->value[2], mask->value[3], mask->value[4], mask->value[5], mask->value[6], mask->value[7], mask->value[8], mask->value[9], mask->value[10]);
    return filename;
}



