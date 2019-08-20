#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolib.h>
#include <domino.h>
#include "../misc/forms.h"

#include <time.h>
#include <locale.h>
#include <stddef.h>

#ifdef _LINUX
#include <wchar.h>
#include <wctype.h>
static  char* atowlower(char* str)
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
                if (iswupper((wint_t)wc))
                {
                    wc = towlower((wint_t)wc);
                    if (wcrtomb(str + i, wc, &cur_state) == -1) {
                	return str;
                    }
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
static char* atowlower(char* str)
{
    return AnsiLower(str);
}
#endif

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();
    do_alias_t *alias;
    char *string;
    char *substring;

    if (argc != 4) {
        fprintf(stderr, "Usage %s <path for ddf> <string> <substring>", argv[0]);
        exit(1);
    }

    if (!domino_init(NULL, FALSE, DO_PARAM_END))
        exit(1);

    if ((alias = domino_alias_new(NULL)) == NULL)
        exit(1);
    do_alias_set_struct_location(alias, argv[1]);

    if (!do_alias_open(alias, DO_DB_STRUCT_FILE, DO_DB_END))
        exit(1);
    struct_file_rec_t struct_file;
    struct_file_key0_t struct_file_key0;
    int status, flag;

    string = atowlower(argv[2]);
    substring = atowlower(argv[3]);

    struct_file_key0.id = 0;
    status = do_struct_file_get0(alias, &struct_file, &struct_file_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        char *location;
        char *ch;
        location = atowlower(do_text(alias, struct_file.data.location));
        flag = 0;
        ch = location;
        while ( *ch != '\0' ) {
            if ( *ch == '\\' ) {
                *ch = '/';
                flag = 1;
            }
            ch++;
        }
        if ( (ch = strstr(location, string)) != NULL ) {
            char *buf;
            flag = 1;
            buf = do_malloc(500);
            strncpy(buf, location, ch - location);
            buf[ch - location] = '\0';
            strcat(buf, substring);
            strcat(buf, ch + strlen(string));
            location = buf;
        }
        if ( flag ) {
            do_text_set(alias, struct_file.data.location, location);
            if ( do_struct_file_update(alias, &struct_file) != DO_OK ) {
                status = DO_ERROR;
                break;
            }
        }
        status = do_struct_file_get0(alias, &struct_file, &struct_file_key0, DO_GET_NEXT);
    }
    do_alias_free(alias);
    return status == DO_ERROR;
}
