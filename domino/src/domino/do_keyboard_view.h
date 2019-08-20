
#ifndef DO_KEYBOARD_VIEW_H
#define DO_KEYBOARD_VIEW_H

#include "do_tree_view.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_KEYBOARD_VIEW         (do_keyboard_view_get_type ())
#define DO_KEYBOARD_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_KEYBOARD_VIEW, DoKeyboardView))
#define DO_KEYBOARD_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_KEYBOARD_VIEW, DoKeyboardViewClass))
#define DO_IS_KEYBOARD_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_KEYBOARD_VIEW))
#define DO_IS_KEYBOARD_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_KEYBOARD_VIEW))
#define DO_KEYBOARD_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_KEYBOARD_VIEW, DoKeyboardViewClass))

typedef struct _DoKeyboardView		DoKeyboardView;
typedef struct _DoKeyboardViewPrivate	DoKeyboardViewPrivate;
typedef struct _DoKeyboardViewClass		DoKeyboardViewClass;

#define do_keyboard_str_head_html \
"<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">" \
"<style type=\"text/css\">" \
"td {border-top:solid windowtext 1.0pt; border-left:solid windowtext 1.0pt;" \
"    border-right:solid windowtext 1.0pt; border-bottom:solid windowtext 1.0pt;}" \
"</style>" \
"</head><body>"

#define do_keyboard_str_tail_html "</body></html>"
#define do_keyboard_str_table_head_html "<table border=0 cellpandding=0 cellspacing=1>"
#define do_keyboard_str_table_tail_html "</table>"
#define do_keyboard_str_tr_head_html "<tr align=center>"
#define do_keyboard_str_tr_tail_html "</tr>"
#define do_keyboard_str_td_html \
        "<td><div style='overflow:hidden; width:%s; height:%s; font-size:%d; font-family: \"%s\";'>%s</div></td>"
#define do_keyboard_str_td_html_aplha1 \
        "<td><div style='overflow:hidden; width:%s; height:%s;'>" \
        "<div style='overflow:hidden; height:%dmm;font-size:%d; font-family:\"%s\";'><b>%s</b></div>" \
        "<div style='overflow:hidden; font-size:%d; font-family:\"%s\"'>%s</div></div></td>"
#define do_keyboard_str_td_html_empty \
        "<td><div style=' width:%s; height:%s; font-size:%d; font-family:\"%s\";'><b>%s</b></div></td>"


typedef enum
{
	DO_KEYBOARD_VIEW_FLAGS_PRINT_TEMPLATE	= 1 << 0,

} DoKeyboardViewFlags;

struct _DoKeyboardView
{
	DoTreeView parent_instance;

};

struct _DoKeyboardViewClass
{
	DoTreeViewClass parent_class;
    void     (* document_changed)  (DoKeyboardView *view);
    void     (* go_sender)         (DoKeyboardView *view);
    void     (* print_template)    (DoKeyboardView *view);
};

#define DO_KEYBOARD_VIEW_FLAGS_DEFAULT 0

GType		do_keyboard_view_get_type(void);

GtkWidget  *do_keyboard_view_new(const gchar *alias_name);

void        do_keyboard_view_show_template(DoKeyboardView *view);
const gchar* do_keyboard_view_str_get_key_name(int key);

G_END_DECLS

#endif /* !DO_KEYBOARD_VIEW_H */
