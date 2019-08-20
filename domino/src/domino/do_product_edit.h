
#ifndef DO_PRODUCT_EDIT_H
#define DO_PRODUCT_EDIT_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"
#include "do_view.h"

G_BEGIN_DECLS

#define DO_TYPE_PRODUCT_EDIT         (do_product_edit_get_type ())
#define DO_PRODUCT_EDIT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_PRODUCT_EDIT, DoProductEdit))
#define DO_PRODUCT_EDIT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_PRODUCT_EDIT, DoProductEditClass))
#define DO_IS_PRODUCT_EDIT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_PRODUCT_EDIT))
#define DO_IS_PRODUCT_EDIT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_PRODUCT_EDIT))
#define DO_PRODUCT_EDIT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_PRODUCT_EDIT, DoProductEditClass))

typedef struct _DoProductEdit		DoProductEdit;
typedef struct _DoProductEditPrivate	DoProductEditPrivate;
typedef struct _DoProductEditClass	DoProductEditClass;

typedef enum
{
	DO_PRODUCT_EDIT_FLAGS_SHOW_ALL_DATA	= 1 << 0,
	DO_PRODUCT_EDIT_FLAGS_READ_ONLY	= 1 << 1,

} DoProductEditFlags;

struct _DoProductEdit
{
    GtkVBox parent_instance;
//	GtkDialog parent_instance;
};

struct _DoProductEditClass
{
	//GtkDialogClass parent_class;
	GtkVBoxClass parent_class;
};

#define DO_PRODUCT_EDIT_FLAGS_DEFAULT 0

GType		do_product_edit_get_type(void);

GtkWidget  *do_product_edit_new(DoView *parent, const gchar *alias_name, DoProductEditFlags flags, DoDialogAction action, DoContext *context, const gchar *unit);

G_END_DECLS

#endif /* !DO_PRODUCT_EDIT_H */
