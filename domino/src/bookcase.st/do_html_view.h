
#ifndef DO_HTML_VIEW_H
#define DO_HTML_VIEW_H

#include <gtk/gtk.h>
#include "do_view.h"

G_BEGIN_DECLS

#define DO_TYPE_HTML_VIEW         (do_html_view_get_type ())
#define DO_HTML_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_HTML_VIEW, DoHtmlView))
#define DO_HTML_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_HTML_VIEW, DoHtmlViewClass))
#define DO_IS_HTML_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_HTML_VIEW))
#define DO_IS_HTML_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_HTML_VIEW))
#define DO_HTML_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_HTML_VIEW, DoHtmlViewClass))

typedef struct _DoHtmlView		DoHtmlView;
typedef struct _DoHtmlViewPrivate	DoHtmlViewPrivate;
typedef struct _DoHtmlViewClass		DoHtmlViewClass;

struct _DoHtmlView
{
	GtkVBox parent_instance;
};

struct _DoHtmlViewClass
{
	GtkVBoxClass parent_class;
};

#define DO_HTML_VIEW_FLAGS_DEFAULT 0

GType		do_html_view_get_type(void);

GtkWidget  *do_html_view_new();

GtkWidget  *do_html_get_view(DoHtmlView *view);

void        do_html_set_entry(DoHtmlView *view, GtkWidget *entry);

void        do_html_view_command_do_print_now(DoView *view);


G_END_DECLS

#endif /* !DO_HTML_VIEW_H */
