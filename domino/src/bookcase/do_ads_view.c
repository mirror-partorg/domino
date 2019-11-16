
#include "do_ads_view.h"
#include "do_tree_view.h"
#include "do_application.h"
#include "do_utilx.h"
#include "do_common_actions.h"

#define DO_ADS_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_ADS_VIEW, DoAdsViewPrivate))
#define DO_TYPE_ADS_VIEW_FLAGS do_ads_view_flags_get_type()

#define ROOT_OBJECT "ads-view"

typedef struct _DoAdsViewId		DoAdsViewId;
struct _DoAdsViewId
{
	DoAdsView *view;
	const gchar *id;
};

static void do_ads_view_view_init(DoViewIface *iface);

static	gboolean	do_ads_view_close_request	(DoView *view);
//static 	void		do_ads_view_do_delete		(DoView *view);
//static 	gboolean	do_ads_view_can_do_delete	(DoView *view);
//static 	void		do_ads_view_do_insert		(DoView *view);
//static 	gboolean	do_ads_view_can_do_insert	(DoView *view);
//static 	void		do_ads_view_do_copy 		(DoView *view);
//static 	gboolean	do_ads_view_can_do_copy	(DoView *view);
static 	void		do_ads_view_do_edit		(DoView *view, const gchar *tab);
static 	gboolean	do_ads_view_can_do_edit	(DoView *view, const gchar *tab);
//static 	void		do_ads_view_do_apply		(DoView *view);
//static 	gboolean	do_ads_view_can_do_apply	(DoView *view);
//static 	void		do_ads_view_do_unapply		(DoView *view);
//static 	gboolean	do_ads_view_can_do_unapply	(DoView *view);
//static 	void		do_ads_view_do_mail_send	(DoView *view);
//static 	gboolean	do_ads_view_can_do_mail_send (DoView *view);
//static 	void		do_ads_view_do_popup		(DoView *view, GdkEventButton* event);
static 	gboolean	do_ads_view_do_grab_focus	(DoView *view);
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
static 	const gchar *do_ads_view_get_load_status(DoView *view);
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
static 	gboolean 	do_ads_view_can_do_close_for_esc (DoView *view);
//static 	void 		do_ads_view_do_refresh (DoView *view);
//static 	gboolean 	do_ads_view_can_do_refresh (DoView *view);
//static 	DoView 	   *do_ads_view_get_active_child (DoView *view);

static void do_ads_view_fill(DoAdsView *view, const gchar *id, const gchar *select_id);
static gboolean do_ads_view_fill_first(DoAdsView *view);
static void model_fill(JsonNode *node, GArray *params);
static void do_ads_view_model_fill(DoAdsView *view, const gchar *id, const gchar *select_id, JsonNode *node);
static void do_ads_view_set_load_status(DoAdsView *view, const gchar *load_status);
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
    DoTreeView    *do_view;
    GtkTreeView   *tree_view;
    guint          source_;
    GtkTreeModel  *model;
    GSList        *path;
    gchar         *updated_key;
    gchar         *load_status_;
};

G_DEFINE_TYPE_WITH_CODE (DoAdsView, do_ads_view, GTK_TYPE_SCROLLED_WINDOW, // to do
    		 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_ads_view_view_init)
             G_ADD_PRIVATE(DoAdsView)
                                    );


static void do_ads_view_view_init(DoViewIface *iface)
{

	iface->close_request = do_ads_view_close_request;
	//iface->do_delete = do_ads_view_do_delete;
	//iface->can_do_delete = do_ads_view_can_do_delete;
	//iface->do_insert = do_ads_view_do_insert;
	//iface->can_do_insert = do_ads_view_can_do_insert;
	//iface->do_copy = do_ads_view_do_copy;
	//iface->can_do_copy = do_ads_view_can_do_copy;
	iface->do_edit = do_ads_view_do_edit;
	iface->can_do_edit = do_ads_view_can_do_edit;
	//iface->do_apply = do_ads_view_do_apply;
	//iface->can_do_apply = do_ads_view_can_do_apply;
	//iface->do_unapply = do_ads_view_do_unapply;
	//iface->can_do_unapply = do_ads_view_can_do_unapply;
	//iface->do_mail_send = do_ads_view_do_mail_send;
	//iface->can_do_mail_send = do_ads_view_can_do_mail_send;
	//iface->do_popup = do_ads_view_do_popup;
	iface->do_grab_focus = do_ads_view_do_grab_focus;
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
	iface->get_load_status = do_ads_view_get_load_status;
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
	iface->can_do_close_for_esc = do_ads_view_can_do_close_for_esc;
	//iface->do_refresh = do_ads_view_do_refresh;
	//iface->can_do_refresh = do_ads_view_can_do_refresh;
	//iface->get_active_child = do_ads_view_get_active_child;
}

static void do_ads_view_init(DoAdsView *do_view)
{
    //DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE(do_view);
    //memset(priv, 0, sizeof(*priv));
}
static void do_ads_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, DoView *view)
{
	if ( do_view_can_do_edit(DO_VIEW(view), NULL) )
		do_view_do_edit(DO_VIEW(view), NULL);
}

static GObject *do_ads_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
    GObject        *object;
    //DoAdsView      *do_view;
    DoAdsViewPrivate *priv;
    GtkTreeModel  *model;
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    object = G_OBJECT_CLASS (do_ads_view_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_ADS_VIEW_GET_PRIVATE(object);
    priv->do_view = DO_TREE_VIEW(do_tree_view_new(ROOT_OBJECT));

    priv->model = model = GTK_TREE_MODEL(gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING));
	g_object_get(priv->do_view, "tree-view", &priv->tree_view, NULL);
	g_signal_connect(priv->tree_view, "row-activated",  G_CALLBACK( do_ads_view_row_activated ), object);

    //gtk_tree_view_set_model(tree_view, model);
    if (GTK_IS_TREE_VIEW(priv->tree_view))
        gtk_tree_view_set_model(GTK_TREE_VIEW(priv->tree_view), model);
    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->do_view), "Description", "Описание", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "markup", 1);

    gtk_container_add (GTK_CONTAINER (object), GTK_WIDGET(priv->do_view));

    gtk_widget_show_all(GTK_WIDGET(object));

    priv->source_ = g_idle_add((GSourceFunc)do_ads_view_fill_first, object);
    return object;
}
static void do_ads_view_finalize (GObject *object)
{
    DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE (object);
    g_free(priv->load_status_);
    G_OBJECT_CLASS (do_ads_view_parent_class)->finalize (object);
}
static gboolean do_ads_view_do_grab_focus(DoView *view)
{
    DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE (view);
    return do_view_do_grab_focus(DO_VIEW(priv->do_view));
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

    //g_type_class_add_private (o_class, sizeof (DoAdsViewPrivate));

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
    DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE (view);
    if ( priv->updated_key ) {
        GtkApplication *app = gtk_window_get_application(
        		 GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))));
    	do_application_cancel_request(DO_APPLICATION(app), priv->updated_key);
    	priv->updated_key = NULL;
    }
	if ( priv->path && g_slist_length(priv->path) > 1 ) {
		gchar *id,*select_id;
		GSList *last;
		last = g_slist_last(priv->path);
		select_id = last->data;
		priv->path = g_slist_remove_link(priv->path, last);
		last = g_slist_last(priv->path);
		id = last->data;
		priv->path = g_slist_remove_link(priv->path, last);
		do_ads_view_fill(DO_ADS_VIEW(view), id, select_id);
		g_free(id);g_free(select_id);
	}
    //gtk_widget_destroy(GTK_WIDGET(view));
}

static gboolean do_ads_view_fill_first(DoAdsView *view)
{
	GtkWidget *window;
	window = gtk_widget_get_toplevel(GTK_WIDGET(view));
	if ( !window || !GTK_IS_WINDOW(window) )
		return TRUE;
	do_ads_view_fill(view, NULL, NULL);
	return FALSE;
}

static void do_ads_view_fill(DoAdsView *view, const gchar *id, const gchar *select_id)
{
    DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE (view);
    GtkApplication *app = gtk_window_get_application(
    		 GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))));
    if ( priv->updated_key ) {
        GtkApplication *app = gtk_window_get_application(
        		 GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))));

    	g_print("updated!!!\n");
    	do_application_cancel_request(DO_APPLICATION(app), priv->updated_key);
    	priv->updated_key = NULL;
    }
    gchar *key;
    gchar *id_,*select_id_;
    GArray *params;
    JsonNode *node;
    id_= g_strdup(id ? id : "");
    select_id_= g_strdup(select_id ? select_id : "");
    params =  g_array_new (FALSE, FALSE, sizeof (gpointer));
	key = g_strdup_printf("Ads.%s",id ? id : "");
    priv->updated_key = key;
	g_array_append_val(params, view);
	g_array_append_val(params, id_);
	g_array_append_val(params, select_id_);
	g_array_append_val(params, key);
    node = do_application_request_async(DO_APPLICATION(app), "GET", "GetAds", key, FALSE, FALSE,
    		                    (GFunc)model_fill, params,
								"id",id ? id : "", NULL);
    if ( node ) {
        do_ads_view_set_load_status(view, "Обновление данных");
    	do_ads_view_model_fill(view, id, select_id, node);
    }
    else {
        do_ads_view_set_load_status(view, "Запрос данных");
    }
	//g_free(key);
}
static void model_append(JsonArray  *array,
                                   guint       index_,
                                   JsonNode   *node,
                                   GtkListStore *model)
{
	JsonObject *obj;
	GtkTreeIter iter;
	gchar *text = NULL, *id = NULL, *key = NULL, *markup = NULL;
	obj = json_node_get_object(node);
	g_assert(obj != NULL);

	if ( json_object_has_member(obj,"text") )
		text = (gchar*)json_object_get_string_member(obj, "text");
	if ( json_object_has_member(obj,"markup") )
		markup = markup_plus(json_object_get_string_member(obj, "markup"));
	else
		markup = g_markup_escape_text(text, -1);
	if ( json_object_has_member(obj,"id") )
		id = (gchar*)json_object_get_string_member(obj, "id");
	if ( json_object_has_member(obj,"key") )
		key = (gchar*)json_object_get_string_member(obj, "key");
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
			           0,text ? text : "",
					   1,markup,
					   2,id,3,key,
					   -1);
	g_free(markup);
}

static void model_fill(JsonNode *node, GArray *params)
{
	DoAdsView *view;
	DoAdsViewPrivate *priv;
	gchar *id;
	gchar *select_id;
	gchar *key;
	view = g_array_index(params, gpointer, 0);
	priv = DO_ADS_VIEW_GET_PRIVATE (view);
	id = g_array_index(params, gpointer, 1);
	select_id = g_array_index(params, gpointer, 2);
	key = g_array_index(params, gpointer, 3);
	do_ads_view_model_fill(view,id,select_id,node);
	g_array_free(params,FALSE);
	g_free(id);g_free(select_id);g_free(key);
	priv->updated_key = NULL;
	do_ads_view_set_load_status(view, NULL);
}

static gboolean set_view_cursor(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, DoAdsViewId *AdsId)
{
	DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE (AdsId->view);
	if ( (!AdsId->id) || AdsId->id[0] == '\0' ) {
		gtk_tree_view_set_cursor(priv->tree_view, path, NULL, FALSE);
		return TRUE;
	}
	GValue value = {0,};
	gtk_tree_model_get_value(model,iter,2,&value);
	const gchar *sad;
	sad = g_value_get_string(&value);
	if ( !g_strcmp0(AdsId->id, sad) ) {
		gtk_tree_view_set_cursor(priv->tree_view, path, NULL, FALSE);
		return TRUE;
	}
	return FALSE;
}

static void do_ads_view_model_fill(DoAdsView *view, const gchar *id, const gchar *select_id, JsonNode *node)
{
	DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE (view);
	JsonObject *obj;
	JsonArray *array;
	if ( !node )
		return;
	obj = json_node_get_object(node);
	if ( obj ) {
		DoAdsViewId AdsId;
		gboolean refill;
		AdsId.view = view;
		AdsId.id = select_id;
		refill = g_slist_length(priv->path) > 0 && !g_strcmp0(g_slist_last(priv->path)->data, id);
		if ( refill ) {
			GtkTreePath *path = NULL;
			GtkTreeIter iter;
			GValue value = {0,};
			gtk_tree_view_get_cursor(priv->tree_view, &path, NULL);
			if ( path ) {
				gtk_tree_model_get_iter(priv->model, &iter, path);
				gtk_tree_model_get_value(priv->model, &iter, 2, &value);
				AdsId.id = g_value_get_string(&value);
			}
		}
		gtk_list_store_clear(GTK_LIST_STORE(priv->model));
		array = json_object_get_array_member(obj, "items");
		json_array_foreach_element(array, (JsonArrayForeach)model_append,priv->model);
		if ( !refill ) {
			priv->path = g_slist_append(priv->path, g_strdup(id ? id : ""));
        }
		gtk_tree_model_foreach(GTK_TREE_MODEL(priv->model),(GtkTreeModelForeachFunc)set_view_cursor, &AdsId);
	}
	//json_node_unref(node);
}

static gboolean do_ads_view_close_request(DoView *view)
{
	return TRUE;
}
static void do_ads_view_do_edit(DoView *view, const gchar *tab)
{
	DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE (view);
	if ( priv->updated_key ) {
        GtkApplication *app = gtk_window_get_application(
        		 GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))));

    	g_print("updated!!!\n");
    	do_application_cancel_request(DO_APPLICATION(app), priv->updated_key);
    	priv->updated_key = NULL;
	}

	GtkTreePath *path = NULL;
	GtkTreeIter iter;
	GValue value = {0,};
	const gchar *key, *id;
	gtk_tree_view_get_cursor(priv->tree_view, &path, NULL);

	if ( !path ) return;
	gtk_tree_model_get_iter(priv->model, &iter, path);
	gtk_tree_model_get_value(priv->model, &iter, 3, &value);// key
	key = g_value_get_string(&value);
	if ( key && key[0] != '\0' ) {
		GVariant *parameter;
		parameter = g_variant_new_string(key);
		do_common_action_activate("ObjView", parameter);
		//to do g_variant_unref(parameter);
		return;
	}
	g_value_unset(&value);
	gtk_tree_model_get_value(priv->model, &iter, 2, &value);// id
	id = g_value_get_string(&value);
	if ( id && id[0] != '\0' ) {
		do_ads_view_fill(DO_ADS_VIEW(view), id, NULL);
	}
}
static 	gboolean do_ads_view_can_do_edit(DoView *view, const gchar *tab)
{
	return TRUE;
}
static 	gboolean do_ads_view_can_do_close_for_esc (DoView *view)
{
	return TRUE; // to do
}
static 	const gchar *do_ads_view_get_load_status(DoView *view)
{
	DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE(view);
    return priv->load_status_;
}
static void do_ads_view_set_load_status(DoAdsView *view, const gchar *load_status)
{
    DoAdsViewPrivate *priv = DO_ADS_VIEW_GET_PRIVATE(view);
    g_free(priv->load_status_);
    if ( load_status && load_status[0] != '\0')
        priv->load_status_ = g_strdup(load_status);
    else
        priv->load_status_ = NULL;

}
