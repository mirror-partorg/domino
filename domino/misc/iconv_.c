#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include "iconv_.h"
#include <dolib.h>

char* get_locale_lang()
{
    char *str = getenv("LANG") ;
    if (str && ((str = strstr(str, ".")) != NULL))
        return ++str;
    else
        return WINDOWS_CHARSET;
}

#ifdef PSEUDO_NUMBER_SIGN
#define NUMBER_SIGN (char)252
#define SHARP_SIGN '#'
static int number_sign_exist = 0;
static char *number2sharp(char *buf, size_t len)
{
    int i;
    for (i = 0; i < len; i++)
    if (buf[i] == NUMBER_SIGN)
        buf[i] = SHARP_SIGN;
    return buf;
}

static char *sharp2number(char *buf, size_t len)
{
    int i;
    for (i =0; i < len; i++)
        if (buf[i] == SHARP_SIGN)
            buf[i] = NUMBER_SIGN;
        return buf;
}
#endif
char* iconv_(iconv_t conv, const char *str)
{
    if (conv == (iconv_t)-1)
        return do_strdup(str);
    size_t result;
    size_t len = strlen(str);
    char ansb[len];
    char* ansa = (char*) do_malloc(len * 2 + 1);
    char *ansbptr = ansb;
    char *ansaptr = ansa;
    size_t len1 = 2 * len;
    size_t length = len1;

    strncpy(ansb, str, len);
    result = iconv(conv, (char**)&ansbptr, &len, &ansaptr, &len1);
    if (result == (size_t) - 1)
    {
        do_free(ansa);
        return do_strdup(str);
    }
    else
	  ansa[length-len1] = '\0';
#ifdef PSEUDO_NUMBER_SIGN
    if (!number_sign_exist)
        sharp2number(ansa, result);
#endif
    return ansa;
}
char* iconvn_(iconv_t conv, const char *str, size_t len)
{
    if (conv == (iconv_t)-1){
	char *retval;
        retval = do_malloc(len+1);
        memcpy(retval,str,len);
        retval[len]='\0';
        return retval;
    }
    size_t result;
    char ansb[len];
    char* ansa = (char*) do_malloc(len * 2 + 1);
    char *ansbptr = ansb;
    char *ansaptr = ansa;
    size_t len1 = 2 * len;
    size_t length = len1;
    size_t len2 = len;

    memcpy(ansb, str, len);
    result = iconv(conv, (char**)&ansbptr, &len2, &ansaptr, &len1);
    if (result == (size_t) - 1)
    {
        do_free(ansa);
	char *retval;
	retval = do_malloc(len+1);
	memcpy(retval,str,len+1);
        return retval;
    }
    else
	  ansa[length-len1] = '\0';
#ifdef PSEUDO_NUMBER_SIGN
    if (!number_sign_exist)
        sharp2number(ansa, result);
#endif
    return ansa;
}
char* iconv_with_error(iconv_t conv, const char *str)
{
    if (conv == (iconv_t)-1)
        return do_strdup(str);
    size_t result;
    size_t len = strlen(str);
    char ansb[len];
    char* ansa = (char*) do_malloc(len * 2 + 1);
    char *ansbptr = ansb;
    char *ansaptr = ansa;
    size_t len1 = 2 * len;
    size_t length = len1;

    strncpy(ansb, str, len);
    result = iconv(conv, (char**)&ansbptr, &len, &ansaptr, &len1);
    if (result == (size_t) - 1)
    {
        do_free(ansa);
        return NULL;
    }
    else
	  ansa[length-len1] = '\0';
#ifdef PSEUDO_NUMBER_SIGN
    if (!number_sign_exist)
        sharp2number(ansa, result);
#endif
    return ansa;
}
