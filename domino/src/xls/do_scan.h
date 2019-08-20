
#ifndef __DO_SCAN_H__
#define __DO_SCAN_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS


typedef struct _DoScan   DoScan;
typedef struct _DoScanClass DoScanClass;

#define DO_TYPE_SCAN              (do_scan_get_type ())
#define DO_SCAN(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), DO_TYPE_SCAN, DoScan))
#define DO_SCAN_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), DO_TYPE_SCAN, DoScanClass))
#define DO_IS_SCAN(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), DO_TYPE_SCAN))
#define DO_IS_SCAN_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), DO_TYPE_SCAN))
#define DO_SCAN_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), DO_TYPE_SCAN, DoScanClass))

typedef enum _DoScanLexemType   DoScanLexemType;

enum _DoScanLexemType {
    DO_SCAN_LEXEM_ERROR = 0,

    DO_SCAN_LEXEM_LESS,
    DO_SCAN_LEXEM_LESS_EQUAL,
    DO_SCAN_LEXEM_GREATE,
    DO_SCAN_LEXEM_GREATE_EQUAL,
    DO_SCAN_LEXEM_EQUAL,
    DO_SCAN_LEXEM_NOT_EQUAL,
    DO_SCAN_LEXEM_MINUS,
    DO_SCAN_LEXEM_PLUS,
    DO_SCAN_LEXEM_MULTI,
    DO_SCAN_LEXEM_AND,
    DO_SCAN_LEXEM_DIV,
    DO_SCAN_LEXEM_COLON,
    DO_SCAN_LEXEM_QUESTION,
    DO_SCAN_LEXEM_LEFT_BRACKET,
    DO_SCAN_LEXEM_RIGTH_BRACKET,

    DO_SCAN_LEXEM_CSTRING,
    DO_SCAN_LEXEM_CINTEGER,
    DO_SCAN_LEXEM_CREAL,
    DO_SCAN_LEXEM_IDENT,
    DO_SCAN_LEXEM_END,
};

typedef struct _DoScanPosition  DoScanPosition;

struct _DoScan {
    GObject parent_instance;
};

struct _DoScanClass {
    GObjectClass parent_class;
};

gint do_scan_position_get_col(DoScanPosition *position);
gint do_scan_position_get_row(DoScanPosition *position);
gint do_scan_position_get_page(DoScanPosition *position);
void do_scan_position_free(DoScanPosition *position);

GType           do_scan_get_type (void) G_GNUC_CONST;
DoScan         *do_scan_new(const gchar *text);

DoScanLexemType do_scan_step(DoScan *scan);
gboolean        do_scan_skip(DoScan *scan, DoScanLexemType lex);

DoScanPosition *do_scan_get_position(DoScan *scan);
void            do_scan_set_position(DoScan *scan, DoScanPosition *position);
void            do_scan_set_position_first(DoScan *scan);
void            do_scan_set_text(DoScan *scan, const gchar *text);
gchar          *do_scan_get_text(DoScan *scan);
gchar          *do_scan_get_text_range(DoScan *scan, DoScanPosition *st, DoScanPosition *end);


G_END_DECLS

#endif /* __DO_SCAN_H__ */
