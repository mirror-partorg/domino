
#ifndef DO_RZ_SELECT_H
#define DO_RZ_SELECT_H

#include <gtk/gtk.h>
#include "do_tree_view.h"
#include <domino.h>

G_BEGIN_DECLS

#define DO_TYPE_RZ_SELECT         (do_rz_select_get_type ())
#define DO_RZ_SELECT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_RZ_SELECT, DoRzSelect))
#define DO_RZ_SELECT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_RZ_SELECT, DoRzSelectClass))
#define DO_IS_RZ_SELECT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_RZ_SELECT))
#define DO_IS_RZ_SELECT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_RZ_SELECT))
#define DO_RZ_SELECT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_RZ_SELECT, DoRzSelectClass))

typedef struct _DoRzSelect		DoRzSelect;
typedef struct _DoRzSelectPrivate	DoRzSelectPrivate;
typedef struct _DoRzSelectClass		DoRzSelectClass;

struct _DoRzSelect
{
	DoTreeView parent_instance;
};

struct _DoRzSelectClass
{
	DoTreeViewClass parent_class;
};

GType		do_rz_select_get_type(void);

GtkWidget  *do_rz_select_new(const gchar *alias_name, const gchar *barcode);
rz_info_t  *do_rz_select_get_select (DoRzSelect *select);

G_END_DECLS

#endif /* !DO_RZ_SELECT_H */
