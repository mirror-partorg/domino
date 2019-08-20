
#ifndef DO_EV_VIEW_H
#define DO_EV_VIEW_H

#include <gtk/gtk.h>
#include "do_view.h"

G_BEGIN_DECLS

#define DO_TYPE_EV_VIEW         (do_ev_view_get_type ())
#define DO_EV_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_EV_VIEW, DoEvView))
#define DO_EV_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_EV_VIEW, DoEvViewClass))
#define DO_IS_EV_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_EV_VIEW))
#define DO_IS_EV_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_EV_VIEW))
#define DO_EV_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_EV_VIEW, DoEvViewClass))

typedef struct _DoEvView		DoEvView;
typedef struct _DoEvViewPrivate	DoEvViewPrivate;
typedef struct _DoEvViewClass		DoEvViewClass;

struct _DoEvView
{
	GtkVBox parent_instance;
};

struct _DoEvViewClass
{
	GtkVBoxClass parent_class;
};

#define DO_EV_VIEW_FLAGS_DEFAULT 0

GType		do_ev_view_get_type(void);

GtkWidget  *do_ev_view_new();

GtkWidget  *do_ev_view_get_view(DoEvView *view);

void    do_ev_view_open_uri(DoEvView *view, const gchar *uri);


G_END_DECLS

#endif /* !DO_EV_VIEW_H */
