
#ifndef DO_PRODUCT_EMBED_EDIT_H
#define DO_PRODUCT_EMBED_EDIT_H

#include <gtk/gtk.h>
#include "do_tree_view.h"

G_BEGIN_DECLS

#define DO_TYPE_PRODUCT_EMBED_EDIT         (do_product_embed_edit_get_type ())
#define DO_PRODUCT_EMBED_EDIT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_PRODUCT_EMBED_EDIT, DoProductEmbedEdit))
#define DO_PRODUCT_EMBED_EDIT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_PRODUCT_EMBED_EDIT, DoProductEmbedEditClass))
#define DO_IS_PRODUCT_EMBED_EDIT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_PRODUCT_EMBED_EDIT))
#define DO_IS_PRODUCT_EMBED_EDIT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_PRODUCT_EMBED_EDIT))
#define DO_PRODUCT_EMBED_EDIT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_PRODUCT_EMBED_EDIT, DoProductEmbedEditClass))

typedef struct _DoProductEmbedEdit		DoProductEmbedEdit;
typedef struct _DoProductEmbedEditPrivate	DoProductEmbedEditPrivate;
typedef struct _DoProductEmbedEditClass		DoProductEmbedEditClass;

struct _DoProductEmbedEdit
{
	DoTreeView parent_instance;
};

struct _DoProductEmbedEditClass
{
	DoTreeViewClass parent_class;
};

GType		do_product_embed_edit_get_type(void);

GtkWidget *do_product_embed_edit_new (const gchar *alias_name, const gchar *main_alias_name);


G_END_DECLS

#endif /* !DO_PRODUCT_EMBED_EDIT_H */
