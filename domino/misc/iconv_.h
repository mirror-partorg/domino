
#ifndef _ICONV___H
#define _ICONV___H

#include <iconv.h>

#define UTF_CHARSET "UTF-8"
#define WINDOWS_CHARSET "CP1251"

#ifdef __cplusplus
extern "C"
{
#endif

char* get_locale_lang();
char *iconv_(iconv_t conv, const char *src);
char *iconv_with_error(iconv_t conv, const char *src);
char *iconvn_(iconv_t conv, const char *src, size_t len);

#ifdef __cplusplus
}
#endif


#endif
