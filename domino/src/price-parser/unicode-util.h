
#ifndef __UNICODE_UTIL_H_INCLUDED
#define __UNICODE_UTIL_H_INCLUDED

#include <glib.h>

G_BEGIN_DECLS

#define WORD_CYRILLIC 1
#define WORD_LATIN 2

#define WORD_ANY WORD_CYRILLIC | WORD_LATIN

void  util_init();

const gchar *util_get_similar_char(gunichar ch, GUnicodeScript script);
const gchar *util_get_similar_cyrillic_char(gunichar ch);
const gchar *util_get_similar_latin_char(gunichar ch);


G_END_DECLS

#endif // __UNICODE_UTIL_H_INCLUDED
