
#include "do_template_view.h"

#define DO_TEMPLATE_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_TEMPLATE_VIEW, DoTemplateViewPrivate))
#define DO_TYPE_TEMPLATE_VIEW_FLAGS do_template_view_flags_get_type()


static void do_template_view_view_init(DoViewIface *iface);

//static	gboolean	do_template_view_close_request	(DoView *view);
//static 	void		do_template_view_do_delete		(DoView *view);
//static 	gboolean	do_template_view_can_do_delete	(DoView *view);
//static 	void		do_template_view_do_insert		(DoView *view);
//static 	gboolean	do_template_view_can_do_insert	(DoView *view);
//static 	void		do_template_view_do_copy 		(DoView *view);
//static 	gboolean	do_template_view_can_do_copy	(DoView *view);
//static 	void		do_template_view_do_edit		(DoView *view, const gchar *tab);
//static 	gboolean	do_template_view_can_do_edit	(DoView *view, const gchar *tab);
//static 	void		do_template_view_do_apply		(DoView *view);
//static 	gboolean	do_template_view_can_do_apply	(DoView *view);
//static 	void		do_template_view_do_unapply		(DoView *view);
//static 	gboolean	do_template_view_can_do_unapply	(DoView *view);
//static 	void		do_template_view_do_mail_send	(DoView *view);
//static 	gboolean	do_template_view_can_do_mail_send (DoView *view);
//static 	void		do_template_view_do_popup		(DoView *view, GdkEventButton* event);
//static 	gboolean	do_template_view_do_grab_focus	(DoView *view);
//static 	void		do_template_view_do_print		(DoView *view);
//static 	void		do_template_view_do_print_now	(DoView *view);
//static 	gboolean	do_template_view_can_do_print	(DoView *view);
//static 	void		do_template_view_do_open 	 	(DoView *view);
//static 	gboolean	do_template_view_can_do_open 	(DoView *view);
//static 	void		do_template_view_do_save 	 	(DoView *view);
//static 	void		do_template_view_do_save_as	 	(DoView *view);
//static 	gboolean	do_template_view_can_do_save 	(DoView *view);
//static 	gboolean	do_template_view_can_do_save_as (DoView *view);
//static 	void	 	do_template_view_do_close	 	(DoView *view);
//static 	const char *do_template_view_get_title 		(DoView *view);
//static 	GdkPixbuf  *do_template_view_get_icon	 	(DoView *view);
//static 	gboolean	do_template_view_get_load_status(DoView *view);
//static 	gint 		do_template_view_get_load_progress(DoView *view);
//static 	DoMarked	do_template_view_get_marked 	(DoView *view, DoContext *context);
//static 	gboolean 	do_template_view_set_marked 	(DoView *view, DoContext *context, DoMarked marked);
//static 	gboolean 	do_template_view_toggle_marked 	(DoView *view, DoContext *context);
//static 	gboolean 	do_template_view_sender_do_close(DoView *view);
//static 	void	 	do_template_view_update_context (DoView *view, DoContext *context);
//static 	void	 	do_template_view_set_cursor_to_context(DoView *view, DoContext *context);
//static 	gboolean 	do_template_view_can_set_cursor_to_context (DoView *view, DoContext *context);
//static 	void		do_template_view_do_find_barcode 	 (DoView *view);
//static 	gboolean	do_template_view_can_do_find_barcode	 (DoView *view);
//static 	gboolean 	do_template_view_can_do_close_for_esc (DoView *view);
//static 	void 		do_template_view_do_refresh (DoView *view);
//static 	gboolean 	do_template_view_can_do_refresh (DoView *view);
//static 	DoView 	   *do_template_view_get_active_child (DoView *view);

enum
{
    PROP_0,
    PROP_FLAGS,
    // to do
};

GType do_template_view_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_template_view_flags_values[] = {
         { DO_TEMPLATE_VIEW_FLAGS_NONE, "DO_TEMPLATE_VIEW_FLAGS_NONE", "none" },
         // to do
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoTemplateViewFlags", _do_template_view_flags_values);
    }
    return type;
}

struct _DoTemplateViewPrivate
{
    DoTemplateViewFlags flags;
    // to do
};

G_DEFINE_TYPE_WITH_CODE (DoTemplateView, do_template_view, GTK_TYPE_WIDGET, // to do
    		 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_template_view_view_init)
                                    );


static void do_template_view_view_init(DoViewIface *iface)
{

	//iface->close_request = do_template_view_close_request;
	//iface->do_delete = do_template_view_do_delete;
	//iface->can_do_delete = do_template_view_can_do_delete;
	//iface->do_insert = do_template_view_do_insert;
	//iface->can_do_insert = do_template_view_can_do_insert;
	//iface->do_copy = do_template_view_do_copy;
	//iface->can_do_copy = do_template_view_can_do_copy;
	//iface->do_edit = do_template_view_do_edit;
	//iface->can_do_edit = do_template_view_can_do_edit;
	//iface->do_apply = do_template_view_do_apply;
	//iface->can_do_apply = do_template_view_can_do_apply;
	//iface->do_unapply = do_template_view_do_unapply;
	//iface->can_do_unapply = do_template_view_can_do_unapply;
	//iface->do_mail_send = do_template_view_do_mail_send;
	//iface->can_do_mail_send = do_template_view_can_do_mail_send;
	//iface->do_popup = do_template_view_do_popup;
	//iface->do_grab_focus = do_template_view_do_grab_focus;
	//iface->do_print = do_template_view_do_print;
	//iface->do_print_now = do_template_view_do_print_now;
	//iface->can_do_print = do_template_view_can_do_print;
	//iface->do_open = do_template_view_do_open;
	//iface->can_do_open = do_template_view_can_do_open;
	//iface->do_save = do_template_view_do_save;
	//iface->do_save_as = do_template_view_do_save_as;
	//iface->can_do_save = do_template_view_can_do_save;
	//iface->can_do_save_as = do_template_view_can_do_save_as;
	//iface->do_close = do_template_view_do_close;
	//iface->get_title = do_template_view_get_title;
	//iface->get_icon = do_template_view_get_icon;
	//iface->get_load_status = do_template_view_get_load_status;
	//iface->get_load_progress = do_template_view_get_load_progress;
	//iface->get_marked = do_template_view_get_marked;
	//iface->set_marked = do_template_view_set_marked;
	//iface->toggle_marked = do_template_view_toggle_marked;
	//iface->sender_do_close = do_template_view_sender_do_close;
	//iface->update_context = do_template_view_update_context;
	//iface->set_cursor_to_context = do_template_view_set_cursor_to_context;
	//iface->can_set_cursor_to_context = do_template_view_can_set_cursor_to_context;
	//iface->do_find_barcode = do_template_view_do_find_barcode;
	//iface->can_do_find_barcode = do_template_view_can_do_find_barcode;
	//iface->can_do_close_for_esc = do_template_view_can_do_close_for_esc;
	//iface->do_refresh = do_template_view_do_refresh;
	//iface->can_do_refresh = do_template_view_can_do_refresh;
	//iface->get_active_child = do_template_view_get_active_child;
}


static void do_template_view_init(DoTemplateView *do_view)
{
    DoTemplateViewPrivate *priv = DO_TEMPLATE_VIEW_GET_PRIVATE (do_view);
    // to do
}
static GObject *do_template_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
    GObject             *object;
    DoTemplateView      *do_view;
    DoTemplateViewPrivate *priv;

    object = G_OBJECT_CLASS (do_template_view_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_TEMPLATE_VIEW_GET_PRIVATE(object);
    // to do
    //gtk_widget_show(GTK_WIDGET(object));
    return object;
}
static void do_template_view_finalize (GObject *object)
{
    DoTemplateViewPrivate *priv = DO_TEMPLATE_VIEW_GET_PRIVATE (object);
    // to do
    G_OBJECT_CLASS (do_template_view_parent_class)->finalize (object);
}
static void do_template_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    DoTemplateViewPrivate *priv = DO_TEMPLATE_VIEW_GET_PRIVATE (object);

    switch (prop_id)
    {
    	case PROP_FLAGS:
    		g_value_set_flags (value, priv->flags);
    		break;
        // to do
    }

    g_assert_not_reached ();
}

static void do_template_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    DoTemplateViewPrivate *priv = DO_TEMPLATE_VIEW_GET_PRIVATE (object);

    switch (prop_id)
    {
    	case PROP_FLAGS:
    		priv->flags = g_value_get_flags (value);
    		//update_flags(DO_TEMPLATE_VIEW(object));
    		break;
        // to do
    }
}

static void do_template_view_class_init (DoTemplateViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

    o_class->constructor  = do_template_view_constructor;
    o_class->finalize     = do_template_view_finalize;
    o_class->get_property = do_template_view_get_property;
    o_class->set_property = do_template_view_set_property;

    g_type_class_add_private (o_class, sizeof (DoTemplateViewPrivate));

    g_object_class_install_property
    	(o_class,
    	 PROP_FLAGS,
    	 g_param_spec_flags ("flags",
    			     "flags",
    			     "flags",
    			     DO_TYPE_TEMPLATE_VIEW_FLAGS,
    			     DO_TEMPLATE_VIEW_FLAGS_DEFAULT,
    			     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
    			     G_PARAM_CONSTRUCT_ONLY));
    // to do

}

GtkWidget *do_template_view_new (DoTemplateViewFlags flags)
{
    return g_object_new (DO_TYPE_TEMPLATE_VIEW,
    		     "flags", flags,
    		     // to do
    		     (gpointer) NULL);
}
