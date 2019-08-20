#include "price-scan.h"
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "unicode-util.h"

#define MAX_LEXEM 255
#define LOG_DOMAIN "scanner"

static void price_scan_get_property  (GObject              *object,
					     guint                 param_id,
					     GValue               *value,
					     GParamSpec           *pspec);
static void price_scan_set_property  (GObject              *object,
					     guint                 param_id,
					     const GValue         *value,
					     GParamSpec           *pspec);

static void price_scan_finalize (GObject          *object);

#define PRICE_SCAN_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), PRICE_TYPE_SCAN, PriceScanPrivate))


typedef struct _PriceScanPrivate PriceScanPrivate;

typedef struct {
    gchar *name;
    PriceScanLexemType type;
    guint len;
} symbol_t;

static symbol_t const_symbol_utf8[] =
{
 {"№", PRICE_SCAN_LEXEM_NN},
 {"+", PRICE_SCAN_LEXEM_PLUS},
 {"-", PRICE_SCAN_LEXEM_MINUS},
 {"–", PRICE_SCAN_LEXEM_HYPEN},
 {"™", PRICE_SCAN_LEXEM_TM},
 {"/", PRICE_SCAN_LEXEM_SLASH},
 {"\\", PRICE_SCAN_LEXEM_BACKSLASH},
 {"(", PRICE_SCAN_LEXEM_LEFT_PARENTHESIS},
 {")", PRICE_SCAN_LEXEM_RIGHT_PARENTHESIS},
 {"[", PRICE_SCAN_LEXEM_LEFT_BRACKET},
 {"]", PRICE_SCAN_LEXEM_RIGHT_BRACKET},
 {"{", PRICE_SCAN_LEXEM_LEFT_BRACE},
 {"}", PRICE_SCAN_LEXEM_RIGHT_BRACE},
 {",", PRICE_SCAN_LEXEM_COMMA},
 {"#", PRICE_SCAN_LEXEM_HASH},
 {"&", PRICE_SCAN_LEXEM_AND},
 {"@", PRICE_SCAN_LEXEM_AT},
 {"?", PRICE_SCAN_LEXEM_QUESTION},
 {"%", PRICE_SCAN_LEXEM_PERCENT},
 {"!", PRICE_SCAN_LEXEM_EXCLAMATION},
 {"*", PRICE_SCAN_LEXEM_ASTERISK},
 {";", PRICE_SCAN_LEXEM_SEMI_COLON},
 {":", PRICE_SCAN_LEXEM_COLON},
 {"=", PRICE_SCAN_LEXEM_EQUAL},
 {"_", PRICE_SCAN_LEXEM_UNDERSCORE},
 {"|", PRICE_SCAN_LEXEM_PIPE},
 {"^", PRICE_SCAN_LEXEM_CARET},
 {"®", PRICE_SCAN_LEXEM_COPYRIGHT},
 {".", PRICE_SCAN_LEXEM_DOT},
 {"$", PRICE_SCAN_LEXEM_DOLLAR},
};

//{"...", PRICE_SCAN_LEXEM_ELLIPSIS},

static GHashTable *hash_symbols = NULL;

typedef struct {
    gchar *left;
    gchar *right;
    gunichar left_ch;
    gunichar right_ch;
    guint left_len;
    guint right_len;

} quote_symbol_t;

static quote_symbol_t quote[] =
{
  {"«", "»"},
  { "\"", "\""},
  { "'", "'" },
  { "`", "`" },
};

#define SYMBOL_HYPEN 8211

#define PRICE_TYPE_SCAN_LEXEM_TYPE price_scan_lexem_get_type()



struct _PriceScanPosition
{
    gint pos;
    gint col;
    gint row;
    gint page;

    gchar *page_;
    gchar *string_;
};

struct _PriceScanPrivate
{
    gchar *text;
    PriceScanPosition crnt;

    gchar lexem[MAX_LEXEM+1];
    gchar locale_decimal_point;
    PriceScanLexemType lexem_type;
};


enum {
    PROP_0,
    PROP_TEXT,
    PROP_LAST_LEXEM,
    PROP_LAST_LEXEM_TYPE,
    PROP_POSITION,

};

G_DEFINE_TYPE (PriceScan, price_scan, G_TYPE_OBJECT)

GType price_scan_lexem_get_type(void)
{
    static GType type = 0;
    if ( G_UNLIKELY (type == 0) )
    {
        static const GEnumValue _price_scan_lexem_type_values[] = {
            {PRICE_SCAN_LEXEM_ERROR,"PRICE_SCAN_LEXEM_ERROR", "error"},
            {PRICE_SCAN_LEXEM_MINUS,"PRICE_SCAN_LEXEM_MINUS","-"},
            //{PRICE_SCAN_LEXEM_DIV,"PRICE_SCAN_LEXEM_DIV","/"},
            {PRICE_SCAN_LEXEM_LEFT_BRACKET,"PRICE_SCAN_LEXEM_LEFT_BRACKET","("},
            {PRICE_SCAN_LEXEM_RIGHT_BRACKET,"PRICE_SCAN_LEXEM_RIGTH_BRACKET",")"},

            {PRICE_SCAN_LEXEM_END,"PRICE_SCAN_LEXEM_END","end of text"},
            { 0, NULL, NULL },
        };
        type = g_enum_register_static("PriceScanLexemType", _price_scan_lexem_type_values);
    }
    return type;
}


static void price_scan_init (PriceScan *scan)
{
 //   PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (scan);
	//memset(priv, 0, sizeof(PriceScanPrivate));
	if ( !hash_symbols ) {
        int i;
        hash_symbols = g_hash_table_new(g_direct_hash, g_direct_equal);
        for ( i = 0; i < G_N_ELEMENTS(const_symbol_utf8); i++ ) {
            gunichar ch;
            gchar *p;
            ch = g_utf8_get_char(const_symbol_utf8[i].name);
            const_symbol_utf8[i].len = strlen(const_symbol_utf8[i].name);
            p = g_utf8_next_char(const_symbol_utf8[i].name);
            if ( *p != '\0' )
                g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Строка \"%s\" больше одного символа", const_symbol_utf8[i].name);
            g_hash_table_insert(hash_symbols, GUINT_TO_POINTER(ch), (gpointer)&const_symbol_utf8[i]);

        }
        for ( i = 0; i < G_N_ELEMENTS(quote); i++ ) {
            quote[i].left_ch = g_utf8_get_char(quote[i].left);
            quote[i].left_len = strlen(quote[i].left);
            quote[i].right_ch = g_utf8_get_char(quote[i].right);
            quote[i].right_len = strlen(quote[i].right);
        }
	}
	util_init();
}

static GObject *price_scan_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
    GObject *object;
    PriceScanPrivate *priv;
    PriceScan *scan;

    object = G_OBJECT_CLASS (price_scan_parent_class)->constructor(type, n_construct_properties, construct_params);
    scan = PRICE_SCAN(object);
    priv = PRICE_SCAN_GET_PRIVATE(scan);

    priv->locale_decimal_point = '.';

	return object;
}

static void price_scan_class_init (PriceScanClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->constructor  = price_scan_constructor;
    object_class->get_property = price_scan_get_property;
    object_class->set_property = price_scan_set_property;
    object_class->finalize     = price_scan_finalize;

// signal
// property

    g_object_class_install_property (object_class,
				   PROP_TEXT,
				   g_param_spec_string ("text",
							"Text",
							"Text",
							NULL,
							G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

    g_object_class_install_property (object_class,
				   PROP_LAST_LEXEM,
				   g_param_spec_string ("last-lexem",
							"Last lexem",
							"Last lexem",
							NULL,
							G_PARAM_READABLE));
    g_object_class_install_property (object_class,
				   PROP_LAST_LEXEM_TYPE,
				   g_param_spec_enum("last-lexem-type",
							"Last lexem type",
							"Last lexem type",
							PRICE_TYPE_SCAN_LEXEM_TYPE,
							PRICE_SCAN_LEXEM_ERROR,
							G_PARAM_READABLE));

    g_object_class_install_property (object_class,
				   PROP_POSITION,
				   g_param_spec_pointer("position",
							"position",
							"position",
							G_PARAM_READABLE));

    g_type_class_add_private (object_class, sizeof (PriceScanPrivate));
}

static void price_scan_get_property (GObject     *object,
				guint        param_id,
				GValue      *value,
				GParamSpec  *pspec)
{
    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (object);

    switch (param_id) {
        case PROP_TEXT:
            g_value_set_string (value, priv->text);
            break;
        case PROP_LAST_LEXEM:
            g_value_set_string (value, priv->lexem);
            break;
        case PROP_LAST_LEXEM_TYPE:
            g_value_set_enum(value, priv->lexem_type);
            break;
        case PROP_POSITION:
            g_value_set_pointer(value, price_scan_get_position(PRICE_SCAN(object)));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
            break;
    }
}

static void
price_scan_set_property (GObject      *object,
				guint         param_id,
				const GValue *value,
				GParamSpec   *pspec)
{
 //   PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (object);

    switch ( param_id ) {
        case PROP_TEXT:
            price_scan_set_text(PRICE_SCAN(object), g_value_get_string(value));
            break;
        case PROP_POSITION:
            price_scan_set_position(PRICE_SCAN(object), g_value_get_pointer(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
            break;
    }
}
static void price_scan_finalize (GObject          *object)
{
    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (object);
    g_free(priv->text);

    G_OBJECT_CLASS (price_scan_parent_class)->finalize (object);
}
PriceScan *price_scan_new(const gchar *text)
{
    return g_object_new (PRICE_TYPE_SCAN, "text", text, NULL);
}
gint price_scan_position_get_col(PriceScanPosition *position)
{
    return position->col;
}
gint price_scan_position_get_row(PriceScanPosition *position)
{
    return position->row;
}
gint price_scan_position_get_page(PriceScanPosition *position)
{
    return position->page;
}
void price_scan_position_free(PriceScanPosition *position)
{
    g_free(position);
}
const gchar *price_scan_get_lexem(PriceScan *scan)
{
    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (scan);
    return priv->lexem;
}
PriceScanPosition *price_scan_get_position(PriceScan *scan)
{
    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (scan);
    PriceScanPosition *res;
    res = g_malloc0(sizeof(PriceScanPosition));

    *res = priv->crnt;

    return res;
}
void price_scan_set_position(PriceScan *scan, PriceScanPosition *position)
{
    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (scan);
    priv->crnt = *position;
}
void price_scan_set_position_first(PriceScan *scan)
{
    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (scan);
    priv->crnt.pos=0;
    priv->crnt.row=0;
    priv->crnt.col=0;
    priv->crnt.page=0;
    priv->crnt.page_ = priv->text;
    priv->crnt.string_ = priv->text;
}
void price_scan_set_text(PriceScan *scan, const gchar *text)
{
    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (scan);
    g_free(priv->text);
    priv->text = g_strdup(text);
    price_scan_set_position_first(scan);
    priv->lexem_type=PRICE_SCAN_LEXEM_ERROR;
}
void price_scan_set_string(PriceScan *scan, const gchar *text, int n)
{
    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (scan);
    price_scan_set_text(scan, text);
    priv->crnt.row=n;
}

gchar *price_scan_get_text(PriceScan *scan)
{
    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (scan);
    return priv->text;
}
gchar *price_scan_get_text_range(PriceScan *scan, PriceScanPosition *st, PriceScanPosition *end)
{
    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (scan);
    gint pos1,pos2;
    pos1 = ( !st ) ? 0 : st->pos;
    pos2 = ( !end ) ? priv->crnt.pos : end->pos;
    return g_strndup(priv->text+pos1, pos2-pos1);
}

#define RETURN_NULL(a) { priv->lexem[0] = '\0'; return priv->lexem_type = a; }
#define RETURN(a) { priv->crnt.col+= (p - priv->text-priv->crnt.pos);priv->crnt.pos = p - priv->text; priv->lexem[0] = '\0'; return priv->lexem_type = a; }
//#define IS_CH(p,a) (g_utf8_skip[*(guchar*)p] == 1 &&  *p == a )
#define RETURN_STR(a) { priv->crnt.col+= (p - priv->text-priv->crnt.pos);priv->crnt.pos = p - priv->text; strncpy(priv->lexem, head, p - head); priv->lexem[p - head] = '\0'; return priv->lexem_type = a; }
#define RETURN_STR_(a,b) { priv->crnt.col+= (p - priv->text-priv->crnt.pos);priv->crnt.pos = p - priv->text; strncpy(priv->lexem, head, p - head); priv->lexem[p - head] = '\0'; b; return priv->lexem_type = a; }
#define POS_ADD_CHAR {priv->crnt.col++;priv->crnt.pos++;}

static void skip_alpha(char **p, int *flags)
{
    gunichar ch;

    for (ch = g_utf8_get_char (*p); g_unichar_isalpha(ch) ; *p = g_utf8_next_char (*p), ch = g_utf8_get_char (*p) ) {
        GUnicodeScript script = g_unichar_get_script(ch);
        switch (script) {
            case G_UNICODE_SCRIPT_LATIN:
                if ( util_get_similar_cyrillic_char(ch) )
                    break;
                *flags &= ~WORD_CYRILLIC;
                break;
            case G_UNICODE_SCRIPT_CYRILLIC:
                if ( util_get_similar_latin_char(ch) )
                    break;
                *flags &= ~WORD_LATIN;
                break;
            default:
                *flags &= ~(WORD_LATIN | WORD_CYRILLIC);
        }
    }
}

static int count_err = 0;

PriceScanLexemType price_scan_step(PriceScan *scan)
{

    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (scan);
    gchar *head;
    gchar *p;
    gchar *save_pos;
    gchar *buf;
    gunichar ch;
    int col;
    int i;

    if ( !priv->text )
        RETURN_NULL(PRICE_SCAN_LEXEM_END);

    p = priv->text + priv->crnt.pos;

start:
    for ( ;*p != '\0'; p = g_utf8_next_char (p) ) { // trim space
        ch = g_utf8_get_char (p);
        if ( !g_unichar_isspace (ch) )
            break;
        if ( *p == '\n' ) {
            priv->crnt.row++;
            priv->crnt.col=0;
            priv->crnt.string_ = p+1;
        }
    }

    if ( *p == '\0' ) // end text
        RETURN(PRICE_SCAN_LEXEM_END);

    head = p;
    ch = g_utf8_get_char (p);


    if ( g_unichar_isdigit(ch) ) {
        for ( ; g_unichar_isdigit(ch) ; p = g_utf8_next_char (p), ch = g_utf8_get_char (p));

        if ( *p == '.' || *p == ',' ) {
            save_pos = p++;
            for (ch = g_utf8_get_char (p); g_unichar_isdigit(ch) ; p = g_utf8_next_char (p), ch = g_utf8_get_char (p) );
            if ( save_pos == p - 1 )
                p = save_pos;

        }
        RETURN_STR(PRICE_SCAN_LEXEM_NUMBER);
    }

    if ( g_unichar_isalpha(ch) ) {
        int word = WORD_ANY;

        skip_alpha(&p, &word);

        /*if ( !word )
            RETURN_STR_(PRICE_SCAN_LEXEM_WORD_SIMPLE, fprintf(stderr, "%s\n", priv->lexem));*/

        ch = g_utf8_get_char (p);

        if ( *p == '-' || ch == SYMBOL_HYPEN ) {
            save_pos = p;
            p = g_utf8_next_char(p);

            skip_alpha(&p, &word);

            if ( save_pos < p - 1 )
                RETURN_STR_(PRICE_SCAN_LEXEM_WORD_COMBINATION, {if (*p == '.') POS_ADD_CHAR; if (*(p+1) == '.') POS_ADD_CHAR; });
            p = save_pos;
        }

        if ( (*p == '\'' || *p == '`' ) && (word == WORD_LATIN) ) {
            save_pos = p++;

            skip_alpha(&p, &word);

            if ( (word == WORD_LATIN) && (save_pos < p - 1) )
                RETURN_STR(PRICE_SCAN_LEXEM_WORD_SIMPLE);
            p = save_pos;
        }
        if ( (*p == '/') && (word == WORD_CYRILLIC) ) {
            save_pos = p++;

            skip_alpha(&p, &word);

            if ( (word == WORD_CYRILLIC) && (save_pos < p - 1) )
                RETURN_STR_(PRICE_SCAN_LEXEM_WORD_COMBINATION, {if (*p == '.') POS_ADD_CHAR; if (*(p+1) == '.') POS_ADD_CHAR; });
            p = save_pos;
        }
        /*if ( g_unichar_isdigit(ch) ) {
            for ( ; g_unichar_isdigit(ch) ; p = g_utf8_next_char (p), ch = g_utf8_get_char (p));

            RETURN_STR_(PRICE_SCAN_LEXEM_WORD_SIMPLE, fprintf(stderr, "%s\n", priv->lexem));
            //RETURN_STR(PRICE_SCAN_LEXEM_WORD_SIMPLE);
        }*/
        if ( *p == '.' )
            RETURN_STR_(PRICE_SCAN_LEXEM_WORD_SHORT, { POS_ADD_CHAR; if (*(p+1) == '.') POS_ADD_CHAR; });

        RETURN_STR(PRICE_SCAN_LEXEM_WORD_SIMPLE);
    }
    for ( i = 0; i < G_N_ELEMENTS(quote); i++ ) {
        if ( ch == quote[i].left_ch ) {
            p+=quote[i].left_len;
            head = p;

            for (ch = g_utf8_get_char (p); ch != quote[i].right_ch && *p != '\0'; p = g_utf8_next_char(p), ch = g_utf8_get_char (p));

            if ( *p != '\0') {
                p+=quote[i].right_len;
                RETURN_STR(PRICE_SCAN_LEXEM_WORD_TITLE);//fprintf(stderr, "%s\n", priv->lexem);} )
            }
            else {
                for ( p = priv->crnt.string_, col=0; p < head; p = g_utf8_next_char(p), col++ );
                g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, " %d [%d:%d] Не закрытый символ (%s) : %s", ++count_err, priv->crnt.row+1, col, quote[i].left, priv->crnt.string_);
                p = head;
                goto start;
            }
            break;
        }
    }
    if ( *p == '.' && *(p+1) == '.' && *(p+2) == '.' ) {
        p+=3;
        RETURN_STR(PRICE_SCAN_LEXEM_ELLIPSIS);
    }

    symbol_t *symbol;
    symbol = g_hash_table_lookup(hash_symbols, GINT_TO_POINTER(ch));
    if ( symbol ) {
        p += symbol->len;
        RETURN_STR(symbol->type);
    }
    /*POS_ADD_CHAR;
    return PRICE_SCAN_LEXEM_NN;
    /*/
    head = p;
    p = g_utf8_next_char(p);
    buf = g_strndup(head, p - head + 1);
    buf[p - head] = '\0';
    for ( p = priv->crnt.string_, col=0; p < head; p = g_utf8_next_char(p), col++ );

    g_log(LOG_DOMAIN, G_LOG_LEVEL_ERROR, "[%d:%d] Неизвестный символ (%s)", priv->crnt.row+1, col, buf);
    g_free(buf);

    RETURN_STR(PRICE_SCAN_LEXEM_ERROR);
}
gboolean price_scan_skip(PriceScan *scan, PriceScanLexemType lex)
{
    PriceScanPrivate *priv = PRICE_SCAN_GET_PRIVATE (scan);
    PriceScanLexemType type = PRICE_SCAN_LEXEM_ERROR;
    while ( type != PRICE_SCAN_LEXEM_END && type != lex ) {
        type = price_scan_step(scan);
    }
    if ( type == lex ) {
        priv->crnt.pos -= strlen(priv->lexem);
    }
    return type == lex;
}


