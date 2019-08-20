
#ifndef __PRICE_SCAN_H__
#define __PRICE_SCAN_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS


typedef struct _PriceScan   PriceScan;
typedef struct _PriceScanClass PriceScanClass;

#define PRICE_TYPE_SCAN              (price_scan_get_type ())
#define PRICE_SCAN(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), PRICE_TYPE_SCAN, PriceScan))
#define PRICE_SCAN_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), PRICE_TYPE_SCAN, PriceScanClass))
#define PRICE_IS_SCAN(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), PRICE_TYPE_SCAN))
#define PRICE_IS_SCAN_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), PRICE_TYPE_SCAN))
#define PRICE_SCAN_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), PRICE_TYPE_SCAN, PriceScanClass))

typedef enum _PriceScanLexemType PriceScanLexemType;

enum _PriceScanLexemType {
    PRICE_SCAN_LEXEM_ERROR = 0,

    PRICE_SCAN_LEXEM_WORD_SIMPLE,
    PRICE_SCAN_LEXEM_WORD_SHORT,
    PRICE_SCAN_LEXEM_WORD_TITLE,
    PRICE_SCAN_LEXEM_WORD_COMBINATION,

    PRICE_SCAN_LEXEM_NUMBER,

    PRICE_SCAN_LEXEM_NN,
    PRICE_SCAN_LEXEM_MINUS,
    PRICE_SCAN_LEXEM_PLUS,
    PRICE_SCAN_LEXEM_SLASH,
    PRICE_SCAN_LEXEM_BACKSLASH,
    PRICE_SCAN_LEXEM_ASTERISK,
    PRICE_SCAN_LEXEM_LEFT_BRACKET,
    PRICE_SCAN_LEXEM_LEFT_PARENTHESIS,
    PRICE_SCAN_LEXEM_LEFT_BRACE,
    PRICE_SCAN_LEXEM_RIGHT_BRACKET,
    PRICE_SCAN_LEXEM_RIGHT_PARENTHESIS,
    PRICE_SCAN_LEXEM_RIGHT_BRACE,
    PRICE_SCAN_LEXEM_COMMA,
    PRICE_SCAN_LEXEM_SEMI_COLON,
    PRICE_SCAN_LEXEM_HASH,
    PRICE_SCAN_LEXEM_AND,
    PRICE_SCAN_LEXEM_AT,
    PRICE_SCAN_LEXEM_QUESTION,
    PRICE_SCAN_LEXEM_PERCENT,
    PRICE_SCAN_LEXEM_EXCLAMATION,
    PRICE_SCAN_LEXEM_EQUAL,
    PRICE_SCAN_LEXEM_UNDERSCORE,
    PRICE_SCAN_LEXEM_ELLIPSIS,
    PRICE_SCAN_LEXEM_CARET,
    PRICE_SCAN_LEXEM_COPYRIGHT,
    PRICE_SCAN_LEXEM_COLON,
    PRICE_SCAN_LEXEM_PIPE,
    PRICE_SCAN_LEXEM_DOLLAR,
    PRICE_SCAN_LEXEM_DOT,
    PRICE_SCAN_LEXEM_HYPEN,
    PRICE_SCAN_LEXEM_TM,


    PRICE_SCAN_LEXEM_END,
};

typedef struct _PriceScanPosition  PriceScanPosition;

struct _PriceScan {
    GObject parent_instance;
};

struct _PriceScanClass {
    GObjectClass parent_class;
};

gint price_scan_position_get_col(PriceScanPosition *position);
gint price_scan_position_get_row(PriceScanPosition *position);
gint price_scan_position_get_page(PriceScanPosition *position);
void price_scan_position_free(PriceScanPosition *position);

GType           price_scan_get_type (void) G_GNUC_CONST;
PriceScan      *price_scan_new(const gchar *text);

PriceScanLexemType price_scan_step(PriceScan *scan);
gboolean        price_scan_skip(PriceScan *scan, PriceScanLexemType lex);

PriceScanPosition *price_scan_get_position(PriceScan *scan);
void            price_scan_set_position(PriceScan *scan, PriceScanPosition *position);
void            price_scan_set_position_first(PriceScan *scan);
void            price_scan_set_text(PriceScan *scan, const gchar *text);
//void            price_scan_set_langs(PriceScan *scan, const gchar *langs);
void            price_scan_set_string(PriceScan *scan, const gchar *text, int n);
gchar          *price_scan_get_text(PriceScan *scan);
const gchar   *price_scan_get_lexem(PriceScan *scan);
gchar          *price_scan_get_text_range(PriceScan *scan, PriceScanPosition *st, PriceScanPosition *end);


G_END_DECLS

#endif /* __PRICE_SCAN_H__ */
