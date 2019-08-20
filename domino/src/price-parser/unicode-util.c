
#include "unicode-util.h"

static const char*
cyrillic_latin_utf8_similar[] =
{
  "у","y",
  "к","k",
  "е","e",
  "н","h",
  "х","x",
  "в","b",
  "а","a",
  "р","p",
  "о","o",
  "с","c",
  "м","m",
  "т","t"
};

static GHashTable *hash_similar[2] = {0,};

#define CYRILLIC_INX 0
#define LATIN_INX 1

static void make_hash_symilar()
{
    int i;
    hash_similar[0] = g_hash_table_new(g_direct_hash, g_direct_equal);
    hash_similar[1] = g_hash_table_new(g_direct_hash, g_direct_equal);
    for ( i = 0; i < G_N_ELEMENTS(cyrillic_latin_utf8_similar); i+=2 ) {
        gunichar ch;
        ch = g_utf8_get_char(cyrillic_latin_utf8_similar[i]);
        g_hash_table_insert(hash_similar[CYRILLIC_INX], GUINT_TO_POINTER(ch), (gpointer)cyrillic_latin_utf8_similar[i+1]);
        ch = g_utf8_get_char(cyrillic_latin_utf8_similar[i+1]);
        g_hash_table_insert(hash_similar[LATIN_INX], GUINT_TO_POINTER(ch), (gpointer)cyrillic_latin_utf8_similar[i]);
    }
}

void util_init()
{
    if ( !hash_similar[0] )
        make_hash_symilar();
}

const gchar *util_get_similar_char(gunichar ch, GUnicodeScript script)
{
    switch ( script ) {
        case G_UNICODE_SCRIPT_CYRILLIC:
            return util_get_similar_latin_char(ch);
        case G_UNICODE_SCRIPT_LATIN:
            return util_get_similar_cyrillic_char(ch);
        default:
            return NULL;
    }
}
const gchar *util_get_similar_cyrillic_char(gunichar ch)
{
    char *res;
    res = g_hash_table_lookup(hash_similar[LATIN_INX], GINT_TO_POINTER(ch));
    return res;
}
const gchar *util_get_similar_latin_char(gunichar ch)
{
    char *res;
    res = g_hash_table_lookup(hash_similar[CYRILLIC_INX], GINT_TO_POINTER(ch));
    return res;
}
