
#ifndef DO_OBJ_VIEW_H
#define DO_OBJ_VIEW_H

#include <gtk/gtk.h>
#include "do_view.h"

G_BEGIN_DECLS

#define DO_TYPE_OBJ_VIEW         (do_obj_view_get_type ())
#define DO_OBJ_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_OBJ_VIEW, DoObjView))
#define DO_OBJ_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_OBJ_VIEW, DoObjViewClass))
#define DO_IS_OBJ_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_OBJ_VIEW))
#define DO_IS_OBJ_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_OBJ_VIEW))
#define DO_OBJ_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_OBJ_VIEW, DoObjViewClass))

typedef struct _DoObjView		DoObjView;
typedef struct _DoObjViewPrivate	DoObjViewPrivate;
typedef struct _DoObjViewClass		DoObjViewClass;

typedef enum
{
	DO_OBJ_VIEW_FLAGS_NONE = 1 << 0,
} DoObjViewFlags;

struct _DoObjView
{
	GtkScrolledWindow parent_instance;
};

struct _DoObjViewClass
{
	GtkScrolledWindowClass parent_class;
};

#define DO_OBJ_VIEW_FLAGS_DEFAULT 0

GType		do_obj_view_get_type(void);

GtkWidget  *do_obj_view_new(const gchar *key); // to do

G_END_DECLS

#endif /* !DO_OBJ_VIEW_H */
