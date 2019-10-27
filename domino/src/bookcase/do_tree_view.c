
#include <gtk/gtk.h>

#include "do_tree_view.h"
#include "domino.h"
#include "do_view.h"
#include "do_view_actions.h"
#include "do_filter.h"

enum
{
	PROP_0 = 0,
	PROP_OBJECT_ROOT_PATH,
	PROP_MODEL,
	PROP_TREE_VIEW,
	PROP_FIXED_HEIGHT_MODE,
	PROP_TITLE,
	//!!PROP_ICON,
	//!!PROP_LOAD_STATUS,
	//!!PROP_LOAD_PROGRESS,
};

#define DO_TREE_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_TREE_VIEW, DoTreeViewPrivate))

struct _DoTreeViewPrivate
{
    GtkTreeView         *tree_view;
    GtkTreeSelection    *tree_selection;
    gint                 n_columns;
    GtkTreeViewColumn  **columns;
    gchar              **columns_path;
    GtkWidget           *header;
    guint                setting_save_timeout;
    gchar               *object_root_path;
    gboolean             popup_menu;
    GtkWidget           *paned;
    GtkWidget           *scroll;


    gchar               *title;
    //!!GdkPixbuf           *icon;
    //!!gboolean             load_status;
    //!!gint                 load_progress;
};

static void        do_tree_view_view_init(DoViewIface *iface);

static void        do_tree_view_close(DoView *view);
static const char *do_tree_view_get_title(DoView *view);
static GdkPixbuf  *do_tree_view_get_icon(DoView *view);
static gboolean	   do_tree_view_get_load_status(DoView *view);
static gint	       do_tree_view_get_load_progress(DoView *view);
static gboolean    do_tree_view_do_grab_focus(DoView *view);

G_DEFINE_TYPE_WITH_CODE (DoTreeView, do_tree_view, GTK_TYPE_BOX,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_tree_view_view_init)
             G_ADD_PRIVATE(DoTreeView)
                            )

static void do_tree_view_view_init(DoViewIface *iface)
{
    iface->do_close          = do_tree_view_close;
    iface->get_title         = do_tree_view_get_title;
    iface->get_icon          = do_tree_view_get_icon;
    iface->get_load_status   = do_tree_view_get_load_status;
    iface->get_load_progress = do_tree_view_get_load_progress;
    iface->do_grab_focus     = do_tree_view_do_grab_focus;
}

static gboolean do_tree_view_cols_sync_width(DoTreeView *view);

static void do_tree_view_init(DoTreeView *dialog)
{
    //DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(dialog);
	//memset(priv, 0, sizeof(*priv));
}
static void do_tree_view_focus_in(GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
    //if (DO_IS_VIEW(user_data))
    //    do_view_actions_set_view(user_data);
}
static void do_tree_view_focus_out(GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(user_data);
    //if ( !priv->popup_menu )
    //    do_view_actions_set_view(NULL);
    priv->popup_menu = FALSE;
}
static void do_tree_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, DoTreeView *view)
{
	if ( do_view_can_do_edit(DO_VIEW(view), NULL) )
		do_view_do_edit(DO_VIEW(view), NULL);
}
static void do_tree_view_selection_changed(GtkTreeSelection *selection, gpointer user_data)
{
    if (DO_IS_VIEW(user_data))
        do_view_actions_refresh(GTK_WIDGET(user_data));
}

static gboolean do_tree_view_start_interactive_search(GtkTreeView *treeview,
                                                        DoTreeView *view)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(view);
    if (priv->header && DO_IS_FILTER(priv->header)) {
        GtkWidget *entry;
        g_object_get(priv->header, "entry", &entry, NULL);
        if (gtk_widget_get_sensitive(entry)) {
            gtk_widget_grab_focus(entry);
            return FALSE;
        }
    }
    return TRUE;
}
static gboolean do_tree_view_button_pressed( GtkWidget *      view,
                             GdkEventButton * event,
                             gpointer         object)
{
	DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE (object);
    GtkTreeView *tv = priv->tree_view;

    if (DO_IS_VIEW(object)) {

        if( event->type == GDK_BUTTON_PRESS  &&  event->button == 3 )
        {
            GtkTreeSelection * selection = gtk_tree_view_get_selection( tv );
            GtkTreePath *      path;
            if( gtk_tree_view_get_path_at_pos ( tv,
                                            (gint) event->x,
                                            (gint) event->y,
                                            &path, NULL, NULL, NULL ) )
            {
                if( !gtk_tree_selection_path_is_selected ( selection, path ) )
                {
                    gtk_tree_selection_unselect_all ( selection );
                    gtk_tree_selection_select_path ( selection, path );
                }
                gtk_tree_path_free( path );
            }
            do_view_do_grab_focus(DO_VIEW(object));
            priv->popup_menu = TRUE;

            do_view_do_popup(DO_VIEW(object), event);
            return TRUE;
        }
    }

    return FALSE;
}
static gboolean do_tree_view_button_released( GtkWidget *      view,
                              GdkEventButton * event,
                              gpointer         unused )
{
    GtkTreeView * tv = GTK_TREE_VIEW( view );

    if( !gtk_tree_view_get_path_at_pos ( tv,
                                         (gint) event->x,
                                         (gint) event->y,
                                         NULL, NULL, NULL, NULL ) )
    {
        GtkTreeSelection * selection = gtk_tree_view_get_selection( tv );
        gtk_tree_selection_unselect_all ( selection );
        return TRUE;
    }

    return FALSE;
}

static gboolean do_tree_view_menu_popup(GtkWidget *my_widget, gpointer object)
{
	DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE (object);
    if ( DO_IS_VIEW(object) ) {
        priv->popup_menu = TRUE;
        do_view_do_popup(DO_VIEW(object), NULL);
        return TRUE;
    }
    return FALSE;
}

static GObject *do_tree_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject          *object;
	GtkTreeView      *tree_view;
	GtkBox          *v_box;
    GtkTreeSelection *sel;
    DoTreeViewPrivate *priv;

	object = G_OBJECT_CLASS (do_tree_view_parent_class)->constructor(type, n_construct_properties, construct_params);
	v_box = GTK_BOX(object);
	priv = DO_TREE_VIEW_GET_PRIVATE(object);
    priv->tree_view = tree_view = (GtkTreeView*)gtk_tree_view_new();
    g_object_set(object, "orientation", GTK_ORIENTATION_VERTICAL, NULL);

/*
#if GTK_CHECK_VERSION (3, 0, 0)
    static const gchar style_fixup[] =
        "@define-color fg_normal #101010;\n"
        "* {\n"
        "-GtkTreeView-even-row-color: black;\n"
        "-GtkTreeView-odd-row-color: black;\n"
        "}\n"
        "GtkTreeView {\n"
        "}\n"
        "GtkTreeView row:nth-child(even) {\n"
        "background-color: black;\n"
        "}\n"
        "GtkTreeView row:nth-child(odd) {\n"
        "background-color: black;\n"
        "}\n";
    GtkStyleContext* context;
    GtkCssProvider* css_provider;
    context = gtk_widget_get_style_context (GTK_WIDGET(object));
    css_provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_data (css_provider, style_fixup, -1, NULL);
    gtk_style_context_add_provider (context, GTK_STYLE_PROVIDER (css_provider),
                                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);


#endif
*/
	gtk_tree_view_set_fixed_height_mode (tree_view, TRUE);
    //gtk_tree_view_set_rules_hint(tree_view, TRUE);
    gtk_tree_view_set_enable_search(tree_view, TRUE);

    g_signal_connect(tree_view, "focus-in-event", G_CALLBACK(do_tree_view_focus_in), object);
    g_signal_connect(tree_view, "focus-out-event", G_CALLBACK(do_tree_view_focus_out), object);
    g_signal_connect(tree_view, "start-interactive-search", G_CALLBACK(do_tree_view_start_interactive_search), object);
    g_signal_connect(tree_view, "popup-menu", G_CALLBACK( do_tree_view_menu_popup ), object);
    g_signal_connect(tree_view, "button-press-event", G_CALLBACK( do_tree_view_button_pressed ), object);
    g_signal_connect(tree_view, "button-release-event",  G_CALLBACK( do_tree_view_button_released ), NULL);
    g_signal_connect(tree_view, "row-activated",  G_CALLBACK( do_tree_view_row_activated ), object);

    priv->tree_selection = sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    g_signal_connect(sel, "changed", G_CALLBACK(do_tree_view_selection_changed), object);

    gchar *grid;
    DOMINO_LOCAL_GET("Style", "grid", &grid, NULL);
    if ( grid )
        gtk_tree_view_set_grid_lines(tree_view, atoi(grid));

    priv->scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(priv->scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);


#if GTK_MAJOR_VERSION < 3
    priv->paned = gtk_hpaned_new();
#else
    priv->paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
#endif
    gtk_paned_add1(GTK_PANED (priv->paned), priv->scroll);
    gtk_box_pack_start(GTK_BOX (v_box), priv->paned, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(priv->scroll), GTK_WIDGET(tree_view));
	gtk_widget_show_all(GTK_WIDGET(priv->scroll));

	gtk_widget_show_all (GTK_WIDGET(object));
	return object;
}
static gboolean do_tree_view_cols_sync_fixed_width(GtkTreeViewColumn *col, GParamSpec *pspec, DoTreeView *view)
{
	DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE (view);
    if ( !G_UNLIKELY (priv->setting_save_timeout) ) {
        priv->setting_save_timeout = g_timeout_add(2000,
               (GSourceFunc)do_tree_view_cols_sync_width, view);
    }
	return TRUE;
}

static void do_tree_view_finalize (GObject *object)
{
	DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE (object);
	gint i;
	if ( priv->setting_save_timeout != 0 )
        g_source_remove (priv->setting_save_timeout);
	if (priv->object_root_path)
		g_free(priv->object_root_path);
	if (priv->columns)
		g_free(priv->columns);
    for (i = 0; i < priv->n_columns; i++)
        g_free(priv->columns_path[i]);
    g_free(priv->columns_path);
	G_OBJECT_CLASS (do_tree_view_parent_class)->finalize (object);
}

static void do_tree_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE (object);
	gboolean valbool;

	switch (prop_id)
	{
        case PROP_OBJECT_ROOT_PATH:
            if (!priv->object_root_path)
                priv->object_root_path = g_value_dup_string(value);
            break;
        case PROP_MODEL:
            gtk_tree_view_set_model(priv->tree_view, g_value_get_object (value));
            break;
        case PROP_FIXED_HEIGHT_MODE:
            valbool = g_value_get_boolean(value);
            gtk_tree_view_set_fixed_height_mode(priv->tree_view, valbool);
            break;
        case PROP_TITLE:
            if ( priv->title )
                g_free(priv->title);
            if ( g_value_get_string(value) )
                priv->title = g_value_dup_string(value);
            else
                priv->title = NULL;
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_tree_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE (object);
	gboolean valbool;

    switch (prop_id)
    {
        case PROP_OBJECT_ROOT_PATH:
            g_value_set_string(value, priv->object_root_path);
            break;
        case PROP_MODEL:
            g_value_set_object(value, gtk_tree_view_get_model(priv->tree_view));
            break;
        case PROP_TREE_VIEW:
            g_value_set_object(value, priv->tree_view);
            break;
        case PROP_FIXED_HEIGHT_MODE:
            g_object_get(priv->tree_view, "fixed-height-mode", &valbool, NULL);
            g_value_set_boolean(value, valbool);
            break;
        case PROP_TITLE:
            g_value_set_string(value, priv->title);
            break;
/*!!        case PROP_ICON:
            g_value_set_object(value, priv->icon);
            break;
        case PROP_LOAD_STATUS:
            g_value_set_boolean(value, priv->load_status);
            break;
        case PROP_LOAD_PROGRESS:
            g_value_set_int(value, priv->load_status);
            break;*/
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}
static void do_tree_view_class_init (DoTreeViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_tree_view_constructor;
	object_class->finalize = do_tree_view_finalize;
	object_class->get_property = do_tree_view_get_property;
	object_class->set_property = do_tree_view_set_property;

	//g_type_class_add_private (object_class, sizeof (DoTreeViewPrivate));

    g_object_class_install_property
        (object_class,
         PROP_OBJECT_ROOT_PATH,
         g_param_spec_string("object-root-path",
                      "Имя настроек",
                      "Имя настроек",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property
        (object_class,
         PROP_TITLE,
         g_param_spec_string("title",
                      "Заголовок",
                      "Заголовок",
                      NULL,
                       G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
                                   PROP_MODEL,
                                   g_param_spec_object ("model",
							"Модель TreeView",
							"Модель для просмотра в виде дерева",
							GTK_TYPE_TREE_MODEL,
							G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
                                   PROP_TREE_VIEW,
                                   g_param_spec_object ("tree-view",
							"TreeView",
							"Просмотр в виде дерева",
							GTK_TYPE_TREE_VIEW,
							G_PARAM_READABLE));

    g_object_class_install_property (object_class,
                                     PROP_FIXED_HEIGHT_MODE,
                                     g_param_spec_boolean ("fixed-height-mode",
                                                          "Режим фиксированной высота",
                                                           "Ускоряет виджет GtkTreeView, так как предполагается, что все строки имеют "
                                                             "одинаковую высоту",
                                                           TRUE,
                                                           G_PARAM_READWRITE));


}

GtkWidget  *do_tree_view_new(const gchar *object_root_path)
{
	return g_object_new (DO_TYPE_TREE_VIEW,
			     "object-root-path", object_root_path,
			     (gpointer) NULL);
}
static gboolean do_tree_view_cols_sync_width(DoTreeView *view)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(view);
    gchar *value, *path;
    gint i, width, last = -1;
    for (i = 0; i < priv->n_columns; i++)
        if (gtk_tree_view_column_get_visible(priv->columns[i]))
            last = i;
    for (i = 0; i < priv->n_columns; i++) {
        if (i != last) {
            width = gtk_tree_view_column_get_width(priv->columns[i]);
            if (width) {
                value = g_strdup_printf("%d", width);
                path = g_strdup_printf("%s/%s", priv->object_root_path, priv->columns_path[i]);
                DOMINO_PROFILE_SET(path, "GtkTreeViewColumn", "fixed-width", value, NULL);
                g_free(value);
                g_free(path);
            }
        }
    }
    priv->setting_save_timeout = 0;
	return FALSE;
}
GtkTreeViewColumn *do_tree_view_add_column(DoTreeView *view, const char *name, const  gchar *title, gint default_size)
{
	DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE (view);
	GtkTreeViewColumn *col;
	priv->n_columns++;
	priv->columns = g_realloc(priv->columns, sizeof(GtkTreeViewColumn*)*priv->n_columns);
	priv->columns[priv->n_columns-1] = col = gtk_tree_view_column_new();
	priv->columns_path = g_realloc(priv->columns_path, sizeof(gchar*)*priv->n_columns);
	priv->columns_path[priv->n_columns-1] = g_strdup(name);

    if (default_size != -1) {
        gchar *str, *path;
        str = g_strdup_printf("%d", default_size);
        path = g_strdup_printf("%s/%s", priv->object_root_path, name);
        DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(col), path,
                                "fixed-width" , str, NULL);
        g_free(str);
        g_free(path);
        gtk_tree_view_column_set_resizable(col, TRUE);
        gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
        gtk_tree_view_column_set_reorderable(col, TRUE);
    }
    else {
        g_object_set(priv->tree_view, "fixed-height-mode", FALSE, NULL);
    }
    gtk_tree_view_append_column(priv->tree_view, col);
    if ( title )
        gtk_tree_view_column_set_title(col, title);
	g_signal_connect(col, "notify::width",
		  G_CALLBACK (do_tree_view_cols_sync_fixed_width), view);
	return col;
}
gboolean do_tree_view_is_focus(DoTreeView *view)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(view);
    return gtk_widget_is_focus(GTK_WIDGET(priv->tree_view));
}

gboolean do_tree_view_grab_focus(DoTreeView *view)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(view);
    GtkTreePath *path = NULL;

    gtk_widget_grab_focus(GTK_WIDGET(priv->tree_view));
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);

    if ( !path )
        path = gtk_tree_path_new_first();
    //do_log(LOG_WARNING, "set cursor");
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
    gtk_tree_path_free(path);
    return TRUE;
}
void do_tree_view_pack_header(DoTreeView *view, GtkWidget *widget)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(view);
    GtkBox *box;
    box = GTK_BOX(view);
    gtk_box_pack_start(box, widget, FALSE , FALSE, 0);
    gtk_box_reorder_child(box, widget, 0);
    priv->header = widget;
}
void do_tree_view_pack_header_fill(DoTreeView *view, GtkWidget *widget)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(view);
    GtkBox *box;
    box = GTK_BOX(view);
    gtk_box_pack_start(box, widget, TRUE, TRUE, 0);
    gtk_box_reorder_child(box, widget, 0);
    priv->header = widget;
}

void do_tree_view_pack_right(DoTreeView *view, GtkWidget *widget)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(view);
    gint width;
#if GTK_MAJOR_VERSION < 3
    GtkRequisition requisition;
    gtk_widget_size_request(GTK_WIDGET(view), &requisition);
    width = requisition.width;
#else
    GtkRequisition m, n;
    gtk_widget_get_preferred_size(GTK_WIDGET(view), &m, &n);
    width = n.width;
#endif
    gtk_paned_add2(GTK_PANED(priv->paned), widget);
    gtk_paned_set_position(GTK_PANED(priv->paned), width);
}
void do_tree_view_pack_footer(DoTreeView *view, GtkWidget *widget)
{
    GtkBox *box;
    box = GTK_BOX(view);
    gtk_box_pack_end(box, widget, FALSE, FALSE, 0);
}
gboolean do_tree_view_delete_selection_from_list_store(DoTreeView *view, do_tree_view_delete_func remove, gpointer user_data)
{
	GList *llist, *rlist = NULL, *l, *r;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreePath *path;
	GtkTreeIter iter, iter2;
	GtkTreeRowReference *row_ref = NULL;
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(view);
    gboolean retval = TRUE;

	selection = gtk_tree_view_get_selection(priv->tree_view);
	llist = gtk_tree_selection_get_selected_rows (selection, &model);

	if ( llist == NULL )
		return retval;

	for ( l = llist; l != NULL; l = l->next )
		rlist = g_list_prepend (rlist, gtk_tree_row_reference_new(model, (GtkTreePath *)l->data));

	path = gtk_tree_row_reference_get_path((GtkTreeRowReference *)g_list_first (rlist)->data);

	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_path_free(path);
	iter2 = iter;

	if ( gtk_tree_model_iter_next(GTK_TREE_MODEL (model), &iter) )	{
		path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), &iter);
		row_ref = gtk_tree_row_reference_new (model, path);
	}
	else {
		path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), &iter2);
		if ( gtk_tree_path_prev(path) )
			row_ref = gtk_tree_row_reference_new (model, path);
	}
	gtk_tree_path_free (path);

	/* Removal */

	for ( r = rlist; r != NULL && retval; r = r->next ) {
        path = gtk_tree_row_reference_get_path
        ((GtkTreeRowReference *)r->data);
        gtk_tree_model_get_iter (model, &iter, path);
        retval = remove (model, &iter, user_data);
        if ( retval )
            gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
        gtk_tree_path_free (path);
	}

    g_list_foreach(rlist, (GFunc)gtk_tree_row_reference_free, NULL);
	g_list_foreach(llist, (GFunc)gtk_tree_path_free, NULL);
	g_list_free (llist);
	g_list_free (rlist);

	/* Selection */

	if (row_ref != NULL)
	{
		path = gtk_tree_row_reference_get_path (row_ref);

		if (path != NULL)
		{
			gtk_tree_view_set_cursor (GTK_TREE_VIEW (priv->tree_view), path, NULL, FALSE);
			gtk_tree_path_free (path);
		}

		gtk_tree_row_reference_free (row_ref);
	}
	return retval;
}
void do_tree_view_set_model(DoTreeView *view, GtkTreeModel *model)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(view);
    gtk_tree_view_set_model(priv->tree_view, model);
}

static void do_tree_view_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_tree_view_get_title(DoView *view)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(view);
    return priv->title;
}
static GdkPixbuf *do_tree_view_get_icon(DoView *view)
{
    return NULL;
}
static gboolean	do_tree_view_get_load_status(DoView *view)
{
    return FALSE;
}
static gint	do_tree_view_get_load_progress(DoView *view)
{
    return 100;
}
static gboolean do_tree_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
void do_tree_view_popup(DoTreeView *view, GdkEventButton* event, GtkWidget *menu)
{
#if GTK_CHECK_VERSION(3,22,0)
    if ( event )
        gtk_menu_popup_at_pointer(GTK_MENU( menu ), (GdkEvent*)(event));
    else {
        gtk_menu_popup_at_pointer(GTK_MENU( menu ), NULL);
        gtk_menu_shell_select_first(GTK_MENU_SHELL(menu), FALSE);
    }
#else
    if ( event )
        gtk_menu_popup ( GTK_MENU( menu ), NULL, NULL, NULL, NULL,
                            event->button,
                            event->time );
    else {
        gtk_menu_popup ( GTK_MENU( menu ), NULL, NULL,
                        NULL, view,
                        0, gtk_get_current_event_time() );
        gtk_menu_shell_select_first(GTK_MENU_SHELL(menu), FALSE);
    }
#endif // GTK_CHECK_VERSION
}
void do_tree_view_tree_view_set_visible(DoTreeView *view, gboolean visible)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW_GET_PRIVATE(view);
    gtk_widget_set_visible(GTK_WIDGET(priv->scroll), visible);

}
