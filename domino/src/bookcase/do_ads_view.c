
#include "do_ads_view.h"
#include "do_tree_view.h"

#define DO_ADS_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_ADS_VIEW, DoAdsViewPrivate))
#define DO_TYPE_ADS_VIEW_FLAGS do_ads_view_flags_get_type()

#define ROOT_OBJECT "ads-view"


static void do_ads_view_view_init(DoViewIface *iface);

//static	gboolean	do_ads_view_close_request	(DoView *view);
//static 	void		do_ads_view_do_delete		(DoView *view);
//static 	gboolean	do_ads_view_can_do_delete	(DoView *view);
//static 	void		do_ads_view_do_insert		(DoView *view);
//static 	gboolean	do_ads_view_can_do_insert	(DoView *view);
//static 	void		do_ads_view_do_copy 		(DoView *view);
//static 	gboolean	do_ads_view_can_do_copy	(DoView *view);
//static 	void		do_ads_view_do_edit		(DoView *view, const gchar *tab);
//static 	gboolean	do_ads_view_can_do_edit	(DoView *view, const gchar *tab);
//static 	void		do_ads_view_do_apply		(DoView *view);
//static 	gboolean	do_ads_view_can_do_apply	(DoView *view);
//static 	void		do_ads_view_do_unapply		(DoView *view);
//static 	gboolean	do_ads_view_can_do_unapply	(DoView *view);
//static 	void		do_ads_view_do_mail_send	(DoView *view);
//static 	gboolean	do_ads_view_can_do_mail_send (DoView *view);
//static 	void		do_ads_view_do_popup		(DoView *view, GdkEventButton* event);
//static 	gboolean	do_ads_view_do_grab_focus	(DoView *view);
//static 	void		do_ads_view_do_print		(DoView *view);
//static 	void		do_ads_view_do_print_now	(DoView *view);
//static 	gboolean	do_ads_view_can_do_print	(DoView *view);
//static 	void		do_ads_view_do_open 	 	(DoView *view);
//static 	gboolean	do_ads_view_can_do_open 	(DoView *view);
//static 	void		do_ads_view_do_save 	 	(DoView *view);
//static 	void		do_ads_view_do_save_as	 	(DoView *view);
//static 	gboolean	do_ads_view_can_do_save 	(DoView *view);
//static 	gboolean	do_ads_view_can_do_save_as (DoView *view);
static 	void	 	do_ads_view_do_close	 	(DoView *view);
static 	const char *do_ads_view_get_title 		(DoView *view);
//static 	GdkPixbuf  *do_ads_view_get_icon	 	(DoView *view);
//static 	gboolean	do_ads_view_get_load_status(DoView *view);
//static 	gint 		do_ads_view_get_load_progress(DoView *view);
//static 	DoMarked	do_ads_view_get_marked 	(DoView *view, DoContext *context);
//static 	gboolean 	do_ads_view_set_marked 	(DoView *view, DoContext *context, DoMarked marked);
//static 	gboolean 	do_ads_view_toggle_marked 	(DoView *view, DoContext *context);
//static 	gboolean 	do_ads_view_sender_do_close(DoView *view);
//static 	void	 	do_ads_view_update_context (DoView *view, DoContext *context);
//static 	void	 	do_ads_view_set_cursor_to_context(DoView *view, DoContext *context);
//static 	gboolean 	do_ads_view_can_set_cursor_to_context (DoView *view, DoContext *context);
//static 	void		do_ads_view_do_find_barcode 	 (DoView *view);
//static 	gboolean	do_ads_view_can_do_find_barcode	 (DoView *view);
//static 	gboolean 	do_ads_view_can_do_close_for_esc (DoView *view);
//static 	void 		do_ads_view_do_refresh (DoView *view);
//static 	gboolean 	do_ads_view_can_do_refresh (DoView *view);
//static 	DoView 	   *do_ads_view_get_active_child (DoView *view);

enum
{
    PROP_0,
    PROP_FLAGS,
    // to do
};

GType do_ads_view_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_ads_view_flags_values[] = {
         { DO_ADS_VIEW_FLAGS_NONE, "DO_ADS_VIEW_FLAGS_NONE", "none" },
         // to do
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoAdsViewFlags", _do_ads_view_flags_values);
    }
    return type;
}

struct _DoAdsViewPrivate
{
    DoAdsViewFlags flags;
    GtkWidget *tree_view;
    // to do
};

G_DEFINE_TYPE_WITH_CODE (DoAdsView, do_ads_view, GTK_TYPE_SCROLLED_WINDOW, // to do
    		 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_ads_view_view_init)
                                    );


static void do_ads_view_view_init(DoViewIface *iface)
{

	//iface->close_request = do_ads_view_close_request;
	//iface->do_delete = do_ads_view_do_delete;
	//iface->can_do_delete = do_ads_view_can_do_delete;
	//iface->do_insert = do_ads_view_do_insert;
	//iface->can_do_insert = do_ads_view_can_do_insert;
	//iface->do_copy = do_ads_view_do_copy;
	//iface->can_do_copy = do_ads_view_can_do_copy;
	//iface->do_edit = do_ads_view_do_edit;
	//iface->can_do_edit = do_ads_view_can_do_edit;
	//iface->do_apply = do_ads_view_do_apply;
	//iface->can_do_apply = do_ads_view_can_do_apply;
	//iface->do_unapply = do_ads_view_do_unapply;
	//iface->can_do_unapply = do_ads_view_can_do_unapply;
	//iface->do_mail_send = do_ads_view_do_mail_send;
	//iface->can_do_mail_send = do_ads_view_can_do_mail_send;
	//iface->do_popup = do_ads_view_do_popup;
	//iface->do_grab_focus = do_ads_view_do_grab_focus;
	//iface->do_print = do_ads_view_do_print;
	//iface->do_print_now = do_ads_view_do_print_now;
	//iface->can_do_print = do_ads_view_can_do_print;
	//iface->do_open = do_ads_view_do_open;
	//iface->can_do_open = do_ads_view_can_do_open;
	//iface->do_save = do_ads_view_do_save;
	//iface->do_save_as = do_ads_view_do_save_as;
	//iface->can_do_save = do_ads_view_can_do_save;
	//iface->can_do_save_as = do_ads_view_can_do_save_as;
	iface->do_close = do_ads_view_do_close;
	iface->get_title = do_ads_view_get_title;
	//iface->get_icon = do_ads_view_get_icon;
	//iface->get_load_status = do_ads_view_get_load_status;
	//iface->get_load_progress = do_ads_view_get_load_progress;
	//iface->get_marked = do_ads_view_get_marked;
	//iface->set_marked = do_ads_view_set_marked;
	//iface->toggle_marked = do_ads_view_toggle_marked;
	//iface->sender_do_close = do_ads_view_sender_do_close;
	//iface->update_context = do_ads_view_update_context;
	//iface->set_cursor_to_context = do_ads_view_set_cursor_to_context;
	//iface->can_set_cursor_to_context = do_ads_view_can_set_cursor_to_context;
	//iface->do_find_barcode = do_ads_view_do_find_barcode;
	//iface->can_do_find_barcode = do_ads_view_can_do_find_barcode;
	//iface->can_do_close_for_esc = do_ads_view_can_do_close_for_esc;
	//iface->do_refresh = do_ads_view_do_refresh;
	//iface->can_do_refresh = do_ads_view_can_do_refresh;
	//iface->get_active_child = do_ads_view_get_active_child;
}

static void do_ads_view_init(DoAdsView *do_view)
{
    //DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE (do_view);
    // to do
}
static GObject *do_ads_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
    GObject        *object;
    //DoAdsView      *do_view;
    DoAdsViewPrivate *priv;
    GtkWidget      *view;
    GtkTreeModel  *model;
    GtkTreeView   *tree_view;
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    object = G_OBJECT_CLASS (do_ads_view_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_ADS_VIEW_GET_PRIVATE(object);
    priv->tree_view = view = do_tree_view_new(ROOT_OBJECT);

    model = GTK_TREE_MODEL(gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING));
	g_object_get(view, "tree-view", &tree_view, NULL);
    //gtk_tree_view_set_model(tree_view, model);
    if (GTK_IS_TREE_VIEW(tree_view))
        gtk_tree_view_set_model(tree_view, model);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(view), "Description", "Описание", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "markup", 1);

    gtk_container_add (GTK_CONTAINER (object), view);

    gtk_widget_show_all(GTK_WIDGET(object));
    return object;
}
static void do_ads_view_finalize (GObject *object)
{
    //DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE (object);
    // to do
    G_OBJECT_CLASS (do_ads_view_parent_class)->finalize (object);
}
static void do_ads_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE (object);

    switch (prop_id)
    {
    	case PROP_FLAGS:
    		g_value_set_flags (value, priv->flags);
    		break;
        // to do
    }

    g_assert_not_reached ();
}

static void do_ads_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE (object);

    switch (prop_id)
    {
    	case PROP_FLAGS:
    		priv->flags = g_value_get_flags (value);
    		//update_flags(DO_ADS_VIEW(object));
    		break;
        // to do
    }
}

static void do_ads_view_class_init (DoAdsViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

    o_class->constructor  = do_ads_view_constructor;
    o_class->finalize     = do_ads_view_finalize;
    o_class->get_property = do_ads_view_get_property;
    o_class->set_property = do_ads_view_set_property;

    g_type_class_add_private (o_class, sizeof (DoAdsViewPrivate));

    g_object_class_install_property
    	(o_class,
    	 PROP_FLAGS,
    	 g_param_spec_flags ("flags",
    			     "flags",
    			     "flags",
    			     DO_TYPE_ADS_VIEW_FLAGS,
    			     DO_ADS_VIEW_FLAGS_DEFAULT,
    			     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
    			     G_PARAM_CONSTRUCT_ONLY));
    // to do

}

GtkWidget *do_ads_view_new ()
{
    return g_object_new (DO_TYPE_ADS_VIEW,
    		     //"flags", flags,
    		     // to do
    		     (gpointer) NULL);
}
static 	const char *do_ads_view_get_title(DoView *view)
{
    return "Акции и рекомендации";
}
static void do_ads_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
