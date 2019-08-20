
#ifndef DO_TREE_VIEW_H
#define DO_TREE_VIEW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_TREE_VIEW         (do_tree_view_get_type ())
#define DO_TREE_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_TREE_VIEW, DoTreeView))
#define DO_TREE_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_TREE_VIEW, DoTreeViewClass))
#define DO_IS_TREE_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_TREE_VIEW))
#define DO_IS_TREE_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_TREE_VIEW))
#define DO_TREE_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_TREE_VIEW, DoTreeViewClass))

typedef struct _DoTreeView		DoTreeView;
typedef struct _DoTreeViewPrivate	DoTreeViewPrivate;
typedef struct _DoTreeViewClass		DoTreeViewClass;
typedef gboolean (do_tree_view_delete_func) (GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data);

struct _DoTreeView
{
	GtkVBox parent_instance;
};

struct _DoTreeViewClass
{
	GtkVBoxClass parent_class;
};

GType		       do_tree_view_get_type(void);

GtkWidget         *do_tree_view_new(const gchar *object_root_path);

GtkTreeViewColumn *do_tree_view_add_column(DoTreeView *view, const char *name, const  gchar *title, gint default_size);
gboolean           do_tree_view_grab_focus(DoTreeView *view);
gboolean           do_tree_view_is_focus(DoTreeView *view);
void               do_tree_view_pack_header(DoTreeView *view, GtkWidget *widget);
void               do_tree_view_pack_header_fill(DoTreeView *view, GtkWidget *widget);
void               do_tree_view_pack_footer(DoTreeView *view, GtkWidget *widget);
void               do_tree_view_pack_right(DoTreeView *view, GtkWidget *widget);
gboolean           do_tree_view_delete_selection_from_list_store(DoTreeView *view, do_tree_view_delete_func remove, gpointer user_data);
void               do_tree_view_set_model(DoTreeView *view, GtkTreeModel *model);
void	           do_tree_view_popup(DoTreeView *view, GdkEventButton* event, GtkWidget *menu);
void               do_tree_view_tree_view_set_visible(DoTreeView *view, gboolean visible);


G_END_DECLS

#endif /* !DO_TREE_VIEW_H */
