#ifndef C_UTIL_H_INCLUDED
#define C_UTIL_H_INCLUDED

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif


#ifndef USE_UTF8
#define c_strlen(str) strlen(str)
#define c_strncpy(a,b,c) strncpy(a,b,c)
#define c_chlen(a) 1
#define c_isalnum(ch) isalnum(ch)
#define c_isalpha(ch) isalpha(ch)
#define c_isdigit(ch) isdigit(ch)
#define c_toupper(ch) toupper(ch)
#define c_isspace(ch) isspace(ch)
#define c_str_at(str, indx) &str[indx]
#define c_next_ch(str) ((char*)str + 1)

#else
int  c_strlen(const char *str);
char *c_strncpy(char *dest, const char *src, size_t n);
int  c_chlen(int ch);
int  c_isalnum(int ch);
int  c_isalpha(int ch);
int  c_isdigit(int ch);
int  c_isspace(int ch);
int  c_toupper(int ch);
char *c_str_at(const char *str, int indx);
char *c_next_ch(const char *str);
#endif
#define c_isbackspace(ch) (ch == KEY_BACKSPACE || ch == 127 || ch == 8)
#define c_isenter(ch) (ch == KEY_ENTER || ch == 13 ||  ch == 10)
int  c_upper_ru(int ch);

#ifdef __cplusplus
}
#endif



#endif // C_UTIL_H_INCLUDED
