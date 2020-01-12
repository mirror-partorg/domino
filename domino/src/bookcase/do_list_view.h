
#ifndef DO_LIST_VIEW_H
#define DO_LIST_VIEW_H

#include <gtk/gtk.h>
#include "do_view.h"
#include "do_client.h"

G_BEGIN_DECLS

#define DO_TYPE_LIST_VIEW         (do_list_view_get_type ())
#define DO_LIST_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_LIST_VIEW, DoListView))
#define DO_LIST_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_LIST_VIEW, DoListViewClass))
#define DO_IS_LIST_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_LIST_VIEW))
#define DO_IS_LIST_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_LIST_VIEW))
#define DO_LIST_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_LIST_VIEW, DoListViewClass))

typedef struct _DoListView		DoListView;
typedef struct _DoListViewPrivate	DoListViewPrivate;
typedef struct _DoListViewClass		DoListViewClass;

typedef enum
{
	DO_LIST_VIEW_FLAGS_NONE = 1 << 0,
} DoListViewFlags;

struct _DoListView
{
	GtkScrolledWindow parent_instance;
};

struct _DoListViewClass
{
	GtkScrolledWindowClass parent_class;
};

#define DO_LIST_VIEW_FLAGS_DEFAULT 0

GType		do_list_view_get_type(void);

GtkWidget  *do_list_view_new(const gchar *name, DoClient *client);

G_END_DECLS

#endif /* !DO_LIST_VIEW_H */
