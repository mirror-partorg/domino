
#include "do_rz_edit_view.h"
#include "do_view.h"
#include "domino.h"
#include "do_icons.h"
#include <gdk/gdk.h>

#define DO_RZ_EDIT_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_RZ_EDIT_VIEW, DoRzEditViewPrivate))

// View interface

static void do_rz_edit_view_view_init(DoViewIface *iface);

static gboolean	   do_rz_edit_view_do_grab_focus(DoView *view);
static void	       do_rz_edit_view_do_close(DoView *view);
static const char *do_rz_edit_view_get_title(DoView *view);
static GdkPixbuf  *do_rz_edit_view_get_icon(DoView *view);
static void		   do_rz_edit_view_do_save(DoView *view);
static gboolean    do_rz_edit_view_can_do_save(DoView *view);
static gboolean    do_rz_edit_view_can_do_close_for_esc(DoView *view);
static void		   do_rz_edit_view_do_edit(DoView *view, const gchar *tab);
static gboolean    do_rz_edit_view_can_do_edit(DoView *view, const gchar *tab);


enum
{
	PROP_0,
	PROP_VIEW_OBJECT,
};

struct _DoRzEditViewPrivate
{
    DoView       *parent_view;

    gboolean      modified;
//    GdkPixbuf    *icon;
    GtkIconTheme *icon_theme;
};

G_DEFINE_TYPE_WITH_CODE (DoRzEditView, do_rz_edit_view, DO_TYPE_RZ_EDIT,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_rz_edit_view_view_init)
                                    );

static void do_rz_edit_view_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_rz_edit_view_do_grab_focus;
    iface->do_close      = do_rz_edit_view_do_close;
    iface->get_title     = do_rz_edit_view_get_title;
    iface->get_icon      = do_rz_edit_view_get_icon;
    iface->do_save       = do_rz_edit_view_do_save;
    iface->can_do_save   = do_rz_edit_view_can_do_save;
    iface->can_do_close_for_esc  = do_rz_edit_view_can_do_close_for_esc;
    iface->do_edit       = do_rz_edit_view_do_edit;
    iface->can_do_edit   = do_rz_edit_view_can_do_edit;
}

static void do_rz_edit_view_init(DoRzEditView *do_view)
{
	DoRzEditViewPrivate *priv = DO_RZ_EDIT_VIEW_GET_PRIVATE (do_view);

    priv->icon_theme = gtk_icon_theme_get_default();

}
static GObject *do_rz_edit_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	object = G_OBJECT_CLASS (do_rz_edit_view_parent_class)->constructor(type, n_construct_properties, construct_params);
    return object;
}

static void do_rz_edit_view_finalize (GObject *object)
{
//    DoRzEditViewPrivate *priv = DO_RZ_EDIT_VIEW_GET_PRIVATE (object);

	G_OBJECT_CLASS (do_rz_edit_view_parent_class)->finalize (object);
}

static void do_rz_edit_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    DoRzEditViewPrivate *priv = DO_RZ_EDIT_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_VIEW_OBJECT:
			g_value_set_object (value, priv->parent_view);
			break;
	}
	/* no readable properties */
	g_assert_not_reached ();
}

static void do_rz_edit_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    DoRzEditViewPrivate *priv = DO_RZ_EDIT_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
        case PROP_VIEW_OBJECT:
            priv->parent_view = g_value_get_pointer(value);
            break;
	}
}

static void do_rz_edit_view_class_init (DoRzEditViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_rz_edit_view_constructor;
	o_class->finalize     = do_rz_edit_view_finalize;
	o_class->get_property = do_rz_edit_view_get_property;
	o_class->set_property = do_rz_edit_view_set_property;


	g_type_class_add_private (o_class, sizeof (DoRzEditViewPrivate));

    g_object_class_install_property
        (o_class,
         PROP_VIEW_OBJECT,
         g_param_spec_pointer("view-object",
                      "view-object",
                      "view-object",
                      //DO_TYPE_VIEW ,
                      G_PARAM_READWRITE));
}
GtkWidget *do_rz_edit_view_new (DoView *parent, const gchar *alias_name, DoRzEditFlags flags, DoDialogAction action, DoContext *context)
{
	return g_object_new (DO_TYPE_RZ_EDIT_VIEW,
			     "flags", flags,
			     "view-object", parent,
			     "action", action,
			     "context", context,
			     "alias-name", alias_name,
			     (gpointer) NULL);
}
static gboolean	   do_rz_edit_view_do_grab_focus(DoView *view)
{
    do_rz_edit_grab_focus(DO_RZ_EDIT(view));
    return TRUE;
}
static void	 do_rz_edit_view_do_close(DoView *view)
{
    do_rz_edit_close(DO_RZ_EDIT(view));
}
static const char *do_rz_edit_view_get_title(DoView *view)
{
    return do_rz_edit_get_title(DO_RZ_EDIT(view));
}
static GdkPixbuf  *do_rz_edit_view_get_icon(DoView *view)
{
    DoDialogAction action;
    DoRzEditFlags flags;
    g_object_get(view, "action", &action, "flags", &flags, NULL);
#if GTK_MAJOR_VERSION > 2
    return NULL;
#else
    DoRzEditViewPrivate *priv = DO_RZ_EDIT_VIEW_GET_PRIVATE (view);
    switch (action) {
        case DO_DIALOG_ACTION_EDIT:
            return  gtk_icon_theme_load_icon(priv->icon_theme, GTK_STOCK_EDIT, DO_VIEW_ICON_SIZE, 0, NULL);
        case DO_DIALOG_ACTION_INSERT:
            if (flags & DO_RZ_EDIT_FLAGS_LINK_ONLY)
                return  gtk_icon_theme_load_icon(priv->icon_theme, GTK_STOCK_ADD, DO_VIEW_ICON_SIZE, 0, NULL);
            else
                return  gtk_icon_theme_load_icon(priv->icon_theme, GTK_STOCK_NEW, DO_VIEW_ICON_SIZE, 0, NULL);
        case DO_DIALOG_ACTION_DELETE:
            return  gtk_icon_theme_load_icon(priv->icon_theme, GTK_STOCK_DELETE, DO_VIEW_ICON_SIZE, 0, NULL);
        default:
            return  gtk_icon_theme_load_icon(priv->icon_theme, GTK_STOCK_FILE, DO_VIEW_ICON_SIZE, 0, NULL);
    }
#endif
}
static void		   do_rz_edit_view_do_save(DoView *view)
{

}
static gboolean    do_rz_edit_view_can_do_save(DoView *view)
{
    return TRUE;
}
static gboolean    do_rz_edit_view_can_do_close_for_esc(DoView *view)
{
    return TRUE;
}
static gboolean    do_rz_edit_view_can_do_edit(DoView *view, const gchar *tab)
{
    if (!tab || *tab == '\0')
        return FALSE;
    return TRUE;
}
static void		   do_rz_edit_view_do_edit(DoView *view, const gchar *tab)
{
    do_rz_edit_show_product(DO_RZ_EDIT(view), tab);
}
