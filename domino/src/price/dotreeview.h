/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * do-tree-view.h
 * Copyright (C) 2015 Ришес Юрий <mir@glekar.com>
 *
 * do-tree-view.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * do-tree-view.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DO_TREE_VIEW_H_
#define _DO_TREE_VIEW_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_TREE_VIEW             (do_tree_view_get_type ())
#define DO_TREE_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), DO_TYPE_TREE_VIEW, DoTreeView))
#define DO_TREE_VIEW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), DO_TYPE_TREE_VIEW, DoTreeViewClass))
#define DO_IS_TREE_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DO_TYPE_TREE_VIEW))
#define DO_IS_TREE_VIEW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), DO_TYPE_TREE_VIEW))
#define DO_TREE_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), DO_TYPE_TREE_VIEW, DoTreeViewClass))

typedef struct _DoTreeViewClass DoTreeViewClass;
typedef struct _DoTreeView DoTreeView;
typedef struct _DoTreeViewPrivate DoTreeViewPrivate;


struct _DoTreeViewClass
{
	GtkTreeViewClass parent_class;
};

struct _DoTreeView
{
	GtkTreeView parent_instance;

	DoTreeViewPrivate *priv;
};

typedef GtkTreePath* (*DoTreeViewSearchFunc) (GtkTreeModel            *model,
						gint                     column,
						const gchar             *key,
						gpointer                 user_data);


GtkWidget         *do_tree_view_new (const gchar *name);
GType              do_tree_view_get_type (void) G_GNUC_CONST;
void              do_tree_view_set_search_func (DoTreeView        *tree_view,
								DoTreeViewSearchFunc   search_func,
								gpointer               search_user_data,
								GDestroyNotify         search_destroy);
GtkTreeViewColumn *do_tree_view_column_new (DoTreeView *view,
                                            const gchar *name,
                                            const  gchar *title,
                                            const gchar *default_text);
gint               do_tree_view_append_column (DoTreeView *tree_view,
                                               GtkTreeViewColumn *column);
void               do_tree_view_set_search_inline_column (DoTreeView *tree_view,
                                            GtkTreeViewColumn *column,
                                            GtkCellRenderer *render);
GtkTreeViewColumn *do_tree_view_get_search_inline_column (DoTreeView *tree_view);
gboolean           do_tree_view_grab_focus (DoTreeView *view);

G_END_DECLS

#endif /* _DO_TREE_VIEW_H_ */

