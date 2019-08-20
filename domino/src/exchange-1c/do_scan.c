#include "do_scan.h"
#include <dolib.h>
#include <locale.h>

#define MAX_LEXEM 255

static void do_scan_get_property  (GObject              *object,
					     guint                 param_id,
					     GValue               *value,
					     GParamSpec           *pspec);
static void do_scan_set_property  (GObject              *object,
					     guint                 param_id,
					     const GValue         *value,
					     GParamSpec           *pspec);

static void do_scan_finalize (GObject          *object);

#define DO_SCAN_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), DO_TYPE_SCAN, DoScanPrivate))


typedef struct _DoScanPrivate DoScanPrivate;

#define DO_TYPE_SCAN_LEXEM_TYPE do_scan_lexem_get_type()

struct _DoScanPosition
{
    gint pos;
    gint col;
    gint row;
    gint page;
};

struct _DoScanPrivate
{
    gchar *text;
    gint crntpos;
    gint crntcol;
    gint crntrow;
    gint crntpage;
    gchar lexem[MAX_LEXEM+1];
    gchar locale_decimal_point;
    DoScanLexemType lexem_type;
};


enum {
    PROP_0,
    PROP_TEXT,
    PROP_LAST_LEXEM,
    PROP_LAST_LEXEM_TYPE,
    PROP_POSITION,
};

G_DEFINE_TYPE (DoScan, do_scan, G_TYPE_OBJECT)

GType do_scan_lexem_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GEnumValue _do_scan_lexem_type_values[] = {
            {DO_SCAN_LEXEM_ERROR,"DO_SCAN_LEXEM_ERROR", "error"},
            {DO_SCAN_LEXEM_LESS,"DO_SCAN_LEXEM_LESS","<"},
            {DO_SCAN_LEXEM_LESS_EQUAL,"DO_SCAN_LEXEM_LESS_EQUAL","<="},
            {DO_SCAN_LEXEM_GREATE,"DO_SCAN_LEXEM_GREATE",">"},
            {DO_SCAN_LEXEM_GREATE_EQUAL,"DO_SCAN_LEXEM_GREATE_EQUAL",">="},
            {DO_SCAN_LEXEM_EQUAL,"DO_SCAN_LEXEM_EQUAL","="},
            {DO_SCAN_LEXEM_NOT_EQUAL,"DO_SCAN_LEXEM_NOT_EQUAL","!="},
            {DO_SCAN_LEXEM_MINUS,"DO_SCAN_LEXEM_MINUS","-"},
            {DO_SCAN_LEXEM_PLUS,"DO_SCAN_LEXEM_PLUS","+"},
            {DO_SCAN_LEXEM_MULTI,"DO_SCAN_LEXEM_MULTI","*"},
            {DO_SCAN_LEXEM_AND,"DO_SCAN_LEXEM_AND","&"},
            {DO_SCAN_LEXEM_DIV,"DO_SCAN_LEXEM_DIV","/"},
            {DO_SCAN_LEXEM_COLON,"DO_SCAN_LEXEM_COLON",":"},
            {DO_SCAN_LEXEM_QUESTION,"DO_SCAN_LEXEM_QUESTION","?"},
            {DO_SCAN_LEXEM_LEFT_BRACKET,"DO_SCAN_LEXEM_LEFT_BRACKET","("},
            {DO_SCAN_LEXEM_RIGTH_BRACKET,"DO_SCAN_LEXEM_RIGTH_BRACKET",")"},
            {DO_SCAN_LEXEM_CSTRING,"DO_SCAN_LEXEM_CSTRING","const string"},
            {DO_SCAN_LEXEM_CINTEGER,"DO_SCAN_LEXEM_CINTEGER","const integer"},
            {DO_SCAN_LEXEM_CREAL,"DO_SCAN_LEXEM_CREAL","const real"},
            {DO_SCAN_LEXEM_IDENT,"DO_SCAN_LEXEM_IDENT","identifier"},
            {DO_SCAN_LEXEM_END,"DO_SCAN_LEXEM_END","end of text"},
            { 0, NULL, NULL }
        };
        type = g_enum_register_static("DoScanLexemType", _do_scan_lexem_type_values);
    }
    return type;
}


static void do_scan_init (DoScan *scan)
{
 //   DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (scan);
	//memset(priv, 0, sizeof(DoScanPrivate));
}

static GObject *do_scan_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
    GObject *object;
    DoScanPrivate *priv;
    DoScan *scan;

    object = G_OBJECT_CLASS (do_scan_parent_class)->constructor(type, n_construct_properties, construct_params);
    scan = DO_SCAN(object);
    priv = DO_SCAN_GET_PRIVATE(scan);

    priv->locale_decimal_point = '.';

	return object;
}

static void do_scan_class_init (DoScanClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->constructor  = do_scan_constructor;
    object_class->get_property = do_scan_get_property;
    object_class->set_property = do_scan_set_property;
    object_class->finalize     = do_scan_finalize;

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
							DO_TYPE_SCAN_LEXEM_TYPE,
							DO_SCAN_LEXEM_ERROR,
							G_PARAM_READABLE));

    g_object_class_install_property (object_class,
				   PROP_POSITION,
				   g_param_spec_pointer("position",
							"position",
							"position",
							G_PARAM_READABLE));

    g_type_class_add_private (object_class, sizeof (DoScanPrivate));
}

static void do_scan_get_property (GObject     *object,
				guint        param_id,
				GValue      *value,
				GParamSpec  *pspec)
{
    DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (object);

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
            g_value_set_pointer(value, do_scan_get_position(DO_SCAN(object)));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
            break;
    }
}

static void
do_scan_set_property (GObject      *object,
				guint         param_id,
				const GValue *value,
				GParamSpec   *pspec)
{
 //   DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (object);

    switch ( param_id ) {
        case PROP_TEXT:
            do_scan_set_text(DO_SCAN(object), g_value_get_string(value));
            break;
        case PROP_POSITION:
            do_scan_set_position(DO_SCAN(object), g_value_get_pointer(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
            break;
    }
}
static void do_scan_finalize (GObject          *object)
{
    DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (object);
    g_free(priv->text);


    G_OBJECT_CLASS (do_scan_parent_class)->finalize (object);
}
DoScan *do_scan_new(const gchar *text)
{
    return g_object_new (DO_TYPE_SCAN, "text", text, NULL);
}
gint do_scan_position_get_col(DoScanPosition *position)
{
    return position->col;
}
gint do_scan_position_get_row(DoScanPosition *position)
{
    return position->row;
}
gint do_scan_position_get_page(DoScanPosition *position)
{
    return position->page;
}
void do_scan_position_free(DoScanPosition *position)
{
    g_free(position);
}
DoScanPosition *do_scan_get_position(DoScan *scan)
{
    DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (scan);
    DoScanPosition *res;
    res = g_malloc0(sizeof(DoScanPosition));
    res->col=priv->crntcol+1;
    res->row=priv->crntcol+1;
    res->pos=priv->crntpos;
    res->page=priv->crntpage+1;
    return res;
}
void do_scan_set_position(DoScan *scan, DoScanPosition *position)
{
    DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (scan);
    priv->crntpos=position->pos;
    priv->crntrow=position->row-1;
    priv->crntcol=position->col-1;
    priv->crntpage=position->page-1;
}
void do_scan_set_position_first(DoScan *scan)
{
    DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (scan);
    priv->crntpos=0;
    priv->crntrow=0;
    priv->crntcol=0;
    priv->crntpage=0;
}
void do_scan_set_text(DoScan *scan, const gchar *text)
{
    DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (scan);
    g_free(priv->text);
    priv->text = g_strdup(text);
    do_scan_set_position_first(scan);
    priv->lexem_type=DO_SCAN_LEXEM_ERROR;
}
gchar *do_scan_get_text(DoScan *scan)
{
    DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (scan);
    return priv->text;
}
gchar *do_scan_get_text_range(DoScan *scan, DoScanPosition *st, DoScanPosition *end)
{
    DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (scan);
    gint pos1,pos2;
    pos1 = ( !st ) ? 0 : st->pos;
    pos2 = ( !end ) ? priv->crntpos : end->pos;
    return g_strndup(priv->text+pos1, pos2-pos1);
}


#define RETURN_NULL(a) { priv->lexem[0] = '\0'; return priv->lexem_type = a; }
#define RETURN(a) { priv->crntcol+= (p - priv->text-priv->crntpos);priv->crntpos = p - priv->text; priv->lexem[0] = '\0'; return priv->lexem_type = a; }
#define IS_CH(a) ( *p == a )
#define RETURN_STR(a) { priv->crntcol+= (p - priv->text-priv->crntpos);priv->crntpos = p - priv->text; strncpy(priv->lexem, head, p - head); priv->lexem[p - head] = '\0'; return priv->lexem_type = a; }

typedef struct {
    gchar *name;
    DoScanLexemType type;
} const_key_t;

#define CONST_KEY_COUNT 16
const const_key_t const_keys[CONST_KEY_COUNT] =
{
 {"<=", DO_SCAN_LEXEM_LESS_EQUAL},
 {">=", DO_SCAN_LEXEM_GREATE_EQUAL},
 {"!=", DO_SCAN_LEXEM_NOT_EQUAL},
 {"<>", DO_SCAN_LEXEM_NOT_EQUAL},
 {"<", DO_SCAN_LEXEM_LESS},
 {">", DO_SCAN_LEXEM_GREATE},
 {"=", DO_SCAN_LEXEM_EQUAL},
 {"-", DO_SCAN_LEXEM_MINUS},
 {"+", DO_SCAN_LEXEM_PLUS},
 {"*", DO_SCAN_LEXEM_MULTI},
 {"&", DO_SCAN_LEXEM_AND},
 {"/", DO_SCAN_LEXEM_DIV},
 {":", DO_SCAN_LEXEM_COLON},
 {"?", DO_SCAN_LEXEM_QUESTION},
 {"(", DO_SCAN_LEXEM_LEFT_BRACKET},
 {")", DO_SCAN_LEXEM_RIGTH_BRACKET},
};

DoScanLexemType do_scan_step(DoScan *scan)
{

    DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (scan);
    gchar *head;
    gchar *p;
    gunichar ch;
    int i;

    if ( !priv->text )
        RETURN_NULL(DO_SCAN_LEXEM_END);

    p = priv->text + priv->crntpos;


    for ( ;*p != '\0'; p = g_utf8_next_char (p) ) { // trim space
        ch = g_utf8_get_char (p);
        if ( !g_unichar_isspace (ch) )
            break;
        if ( *p == '\n' ) {
            priv->crntrow++;
            priv->crntcol=0;
        }
    }

    if ( IS_CH('\0') ) // end text
        RETURN(DO_SCAN_LEXEM_END);

    head = p;
    ch = g_utf8_get_char (p);

    for ( i = 0; i < CONST_KEY_COUNT; i++ ) { // const keys
        if ( !strncmp(p, const_keys[i].name, strlen(const_keys[i].name) ) ) {
            p += strlen(const_keys[i].name);
            RETURN_STR(const_keys[i].type);
        }
    }

    if ( g_unichar_isdigit(ch) ) {
        for( ; g_unichar_isdigit(ch) ; p = g_utf8_next_char (p), ch = g_utf8_get_char (p));

        if ( IS_CH(priv->locale_decimal_point) ) {
            for( ; g_unichar_isdigit(ch) ; p = g_utf8_next_char (p), ch = g_utf8_get_char (p) );
            RETURN_STR(DO_SCAN_LEXEM_CREAL);
        }
        else
        if ( g_unichar_isalpha(ch) ) {
            for( ; g_unichar_isdigit(ch) ||
                   g_unichar_isalpha(ch) ||
                   *p == '_'
                ; p = g_utf8_next_char (p), ch = g_utf8_get_char (p) );
            RETURN_STR(DO_SCAN_LEXEM_IDENT);
        }

        RETURN_STR(DO_SCAN_LEXEM_CINTEGER);
    }

    if ( g_unichar_isalpha(ch) ) {
        for( ; g_unichar_isdigit(ch) ||
               g_unichar_isalpha(ch) ||
               *p == '_'
            ; p = g_utf8_next_char (p), ch = g_utf8_get_char (p) );
        RETURN_STR(DO_SCAN_LEXEM_IDENT);
    }
    if ( IS_CH('"') ) {
        p++;
        head = p;
        for( ; *p != '"' && *p != '\0'; p = g_utf8_next_char (p) );

        if ( *p != '\0') {
            priv->crntcol+= (p - priv->text-priv->crntpos)+1;
            priv->crntpos = p - priv->text+1;
            strncpy(priv->lexem, head, p - head);
            priv->lexem[p - head] = '\0';
            return priv->lexem_type = DO_SCAN_LEXEM_CSTRING;
        }
        else
            RETURN_STR(DO_SCAN_LEXEM_CSTRING);
    }
    p = g_utf8_next_char (p);
    RETURN_STR(DO_SCAN_LEXEM_ERROR);
}
gboolean do_scan_skip(DoScan *scan, DoScanLexemType lex)
{
    DoScanPrivate *priv = DO_SCAN_GET_PRIVATE (scan);
    DoScanLexemType type = DO_SCAN_LEXEM_ERROR;
    while ( type != DO_SCAN_LEXEM_END && type != lex ) {
        type = do_scan_step(scan);
    }
    if ( type == lex ) {
        priv->crntpos -= strlen(priv->lexem);
    }
    return type == lex;
}
