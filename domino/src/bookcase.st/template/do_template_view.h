
#ifndef DO_TEMPLATE_VIEW_H
#define DO_TEMPLATE_VIEW_H

#include <gtk/gtk.h>
#include "do_view.h"

G_BEGIN_DECLS

#define DO_TYPE_TEMPLATE_VIEW         (do_template_view_get_type ())
#define DO_TEMPLATE_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_TEMPLATE_VIEW, DoTemplateView))
#define DO_TEMPLATE_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_TEMPLATE_VIEW, DoTemplateViewClass))
#define DO_IS_TEMPLATE_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_TEMPLATE_VIEW))
#define DO_IS_TEMPLATE_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_TEMPLATE_VIEW))
#define DO_TEMPLATE_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_TEMPLATE_VIEW, DoTemplateViewClass))

typedef struct _DoTemplateView		DoTemplateView;
typedef struct _DoTemplateViewPrivate	DoTemplateViewPrivate;
typedef struct _DoTemplateViewClass		DoTemplateViewClass;

typedef enum
{
	DO_TEMPLATE_VIEW_FLAGS_NONE = 1 << 0,
} DoTemplateViewFlags;

struct _DoTemplateView
{
	//to do DoTreeView parent_instance;
};

struct _DoTemplateViewClass
{
	//to do DoTreeViewClass parent_class;
};

#define DO_TEMPLATE_VIEW_FLAGS_DEFAULT 0

GType		do_template_view_get_type(void);

GtkWidget  *do_template_view_new(DoTemplateViewFlags flags); // to do

G_END_DECLS

#endif /* !DO_TEMPLATE_VIEW_H */
