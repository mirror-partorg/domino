
#include "do_list_view.h"
#include "do_list_model.h"
#include "do_tree_view.h"
#include "do_application.h"
#include "do_utilx.h"
#include "do_common_actions.h"
#include "domino.h"


#define DO_LIST_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_LIST_VIEW, DoListViewPrivate))
#define DO_TYPE_LIST_VIEW_FLAGS do_list_view_flags_get_type()

#define ROOT_OBJECT_ "list-%s-view"


typedef struct _DoListViewId		DoListViewId;
typedef struct _DoListViewField		DoListViewField;

struct _DoListViewId
{
	DoListView *view;
	const gchar *id;
};
struct _DoListViewField
{
    gchar short_;
    gchar *name;
    PangoAlignment align;
    gchar *title;
    gboolean invisible;
};

static void do_list_view_field_free(DoListViewField *field)
{
    g_free(field->name);
    g_free(field->title);
    g_free(field);
}
static void do_list_view_view_init(DoViewIface *iface);

static	gboolean	do_list_view_close_request	(DoView *view);
//static 	void		do_list_view_do_delete		(DoView *view);
//static 	gboolean	do_list_view_can_do_delete	(DoView *view);
//static 	void		do_list_view_do_insert		(DoView *view);
//static 	gboolean	do_list_view_can_do_insert	(DoView *view);
//static 	void		do_list_view_do_copy 		(DoView *view);
//static 	gboolean	do_list_view_can_do_copy	(DoView *view);
static 	void		do_list_view_do_edit		(DoView *view, const gchar *tab);
static 	gboolean	do_list_view_can_do_edit	(DoView *view, const gchar *tab);
//static 	void		do_list_view_do_apply		(DoView *view);
//static 	gboolean	do_list_view_can_do_apply	(DoView *view);
//static 	void		do_list_view_do_unapply		(DoView *view);
//static 	gboolean	do_list_view_can_do_unapply	(DoView *view);
//static 	void		do_list_view_do_mail_send	(DoView *view);
//static 	gboolean	do_list_view_can_do_mail_send (DoView *view);
//static 	void		do_list_view_do_popup		(DoView *view, GdkEventButton* event);
static 	gboolean	do_list_view_do_grab_focus	(DoView *view);
//static 	void		do_list_view_do_print		(DoView *view);
//static 	void		do_list_view_do_print_now	(DoView *view);
//static 	gboolean	do_list_view_can_do_print	(DoView *view);
//static 	void		do_list_view_do_open 	 	(DoView *view);
//static 	gboolean	do_list_view_can_do_open 	(DoView *view);
//static 	void		do_list_view_do_save 	 	(DoView *view);
//static 	void		do_list_view_do_save_as	 	(DoView *view);
//static 	gboolean	do_list_view_can_do_save 	(DoView *view);
//static 	gboolean	do_list_view_can_do_save_as (DoView *view);
static 	void	 	do_list_view_do_close	 	(DoView *view);
static 	const char *do_list_view_get_title 		(DoView *view);
//static 	GdkPixbuf  *do_list_view_get_icon	 	(DoView *view);
static 	const gchar *do_list_view_get_load_status(DoView *view);
//static 	gint 		do_list_view_get_load_progress(DoView *view);
//static 	DoMarked	do_list_view_get_marked 	(DoView *view, DoContext *context);
//static 	gboolean 	do_list_view_set_marked 	(DoView *view, DoContext *context, DoMarked marked);
//static 	gboolean 	do_list_view_toggle_marked 	(DoView *view, DoContext *context);
//static 	gboolean 	do_list_view_sender_do_close(DoView *view);
//static 	void	 	do_list_view_update_context (DoView *view, DoContext *context);
//static 	void	 	do_list_view_set_cursor_to_context(DoView *view, DoContext *context);
//static 	gboolean 	do_list_view_can_set_cursor_to_context (DoView *view, DoContext *context);
//static 	void		do_list_view_do_find_barcode 	 (DoView *view);
//static 	gboolean	do_list_view_can_do_find_barcode	 (DoView *view);
static 	gboolean 	do_list_view_can_do_close_for_esc (DoView *view);
//static 	void 		do_list_view_do_refresh (DoView *view);
//static 	gboolean 	do_list_view_can_do_refresh (DoView *view);
//static 	DoView 	   *do_list_view_get_active_child (DoView *view);

//static void do_list_view_fill(DoListView *view);
static gboolean do_list_view_fill_first(DoListView *view);
//static void do_list_view_model_fill(DoListView *view, JsonNode *node);
static void do_list_view_set_load_status(DoListView *view, const gchar *load_status);
static gboolean do_list_view_make_columns(DoListView *view);
static void do_list_view_make_column(JsonArray *columns, guint index_, JsonNode *element_node, gpointer data);
#ifdef DEBUG
static void do_list_view_make_key_column(DoListView *view);
#endif
enum
{
    PROP_0,
    PROP_NAME,
	PROP_CLIENT,
    // to do
};

GType do_list_view_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_list_view_flags_values[] = {
         { DO_LIST_VIEW_FLAGS_NONE, "DO_LIST_VIEW_FLAGS_NONE", "none" },
         // to do
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoListViewFlags", _do_list_view_flags_values);
    }
    return type;
}

struct _DoListViewPrivate
{
    DoListViewFlags flags;
    DoTreeView    *do_view;
    GtkTreeView   *tree_view;
    DoClient      *client;
    guint          source_;
    GtkTreeModel  *model;
    GSList        *path;
    gchar         *updated_key;
    gchar         *load_status_;
    gchar         *name;
    GSList        *fields;
    gchar         *userfields;
    gboolean       fields_filled;
};

G_DEFINE_TYPE_WITH_CODE (DoListView, do_list_view, GTK_TYPE_SCROLLED_WINDOW, // to do
    		 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_list_view_view_init)
             G_ADD_PRIVATE(DoListView)
                                    );


static void do_list_view_view_init(DoViewIface *iface)
{

	iface->close_request = do_list_view_close_request;
	//iface->do_delete = do_list_view_do_delete;
	//iface->can_do_delete = do_list_view_can_do_delete;
	//iface->do_insert = do_list_view_do_insert;
	//iface->can_do_insert = do_list_view_can_do_insert;
	//iface->do_copy = do_list_view_do_copy;
	//iface->can_do_copy = do_list_view_can_do_copy;
	iface->do_edit = do_list_view_do_edit;
	iface->can_do_edit = do_list_view_can_do_edit;
	//iface->do_apply = do_list_view_do_apply;
	//iface->can_do_apply = do_list_view_can_do_apply;
	//iface->do_unapply = do_list_view_do_unapply;
	//iface->can_do_unapply = do_list_view_can_do_unapply;
	//iface->do_mail_send = do_list_view_do_mail_send;
	//iface->can_do_mail_send = do_list_view_can_do_mail_send;
	//iface->do_popup = do_list_view_do_popup;
	iface->do_grab_focus = do_list_view_do_grab_focus;
	//iface->do_print = do_list_view_do_print;
	//iface->do_print_now = do_list_view_do_print_now;
	//iface->can_do_print = do_list_view_can_do_print;
	//iface->do_open = do_list_view_do_open;
	//iface->can_do_open = do_list_view_can_do_open;
	//iface->do_save = do_list_view_do_save;
	//iface->do_save_as = do_list_view_do_save_as;
	//iface->can_do_save = do_list_view_can_do_save;
	//iface->can_do_save_as = do_list_view_can_do_save_as;
	iface->do_close = do_list_view_do_close;
	iface->get_title = do_list_view_get_title;
	//iface->get_icon = do_list_view_get_icon;
	iface->get_load_status = do_list_view_get_load_status;
	//iface->get_load_progress = do_list_view_get_load_progress;
	//iface->get_marked = do_list_view_get_marked;
	//iface->set_marked = do_list_view_set_marked;
	//iface->toggle_marked = do_list_view_toggle_marked;
	//iface->sender_do_close = do_list_view_sender_do_close;
	//iface->update_context = do_list_view_update_context;
	//iface->set_cursor_to_context = do_list_view_set_cursor_to_context;
	//iface->can_set_cursor_to_context = do_list_view_can_set_cursor_to_context;
	//iface->do_find_barcode = do_list_view_do_find_barcode;
	//iface->can_do_find_barcode = do_list_view_can_do_find_barcode;
	iface->can_do_close_for_esc = do_list_view_can_do_close_for_esc;
	//iface->do_refresh = do_list_view_do_refresh;
	//iface->can_do_refresh = do_list_view_can_do_refresh;
	//iface->get_active_child = do_list_view_get_active_child;
}

static void do_list_view_init(DoListView *do_view)
{
    //DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    //memset(priv, 0, sizeof(*priv));
}
static void do_list_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, DoView *view)
{
	if ( do_view_can_do_edit(DO_VIEW(view), NULL) )
		do_view_do_edit(DO_VIEW(view), NULL);
}

static GObject *do_list_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
    GObject        *object;
    //DoListView      *do_view;
    DoListViewPrivate *priv;
    //GtkTreeModel  *model;
    gchar *path;

    object = G_OBJECT_CLASS (do_list_view_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_LIST_VIEW_GET_PRIVATE(object);

    path = g_strdup_printf(ROOT_OBJECT_, priv->name);

    priv->do_view = DO_TREE_VIEW(do_tree_view_new(path));
    DOMINO_LOCAL_GET(path, "fields", &priv->userfields, NULL);
    g_free(path);


    //priv->model = model = GTK_TREE_MODEL(gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING));//to do
	g_object_get(priv->do_view, "tree-view", &priv->tree_view, NULL);
	g_signal_connect(priv->tree_view, "row-activated",  G_CALLBACK( do_list_view_row_activated ), object);

    if (GTK_IS_TREE_VIEW(priv->tree_view)) {
        //gtk_tree_view_set_headers_visible(|GTK_TREE_VIEW(priv->tree_view), FALSE);
        gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(priv->tree_view), TRUE);
    }

    gtk_container_add (GTK_CONTAINER (object), GTK_WIDGET(priv->do_view));

    do_list_view_make_columns(DO_LIST_VIEW(object));
#ifdef DEBUG
    do_list_view_make_key_column(DO_LIST_VIEW(object));
#endif

    gtk_widget_show_all(GTK_WIDGET(object));

    priv->source_ = g_idle_add((GSourceFunc)do_list_view_fill_first, object);//to do
    return object;
}
static void do_list_view_finalize (GObject *object)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (object);
    g_free(priv->load_status_);
    g_slist_foreach(priv->fields, (GFunc)do_list_view_field_free, object);
    G_OBJECT_CLASS (do_list_view_parent_class)->finalize (object);
}
static gboolean do_list_view_do_grab_focus(DoView *view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);
    return do_view_do_grab_focus(DO_VIEW(priv->do_view));
}
static void do_list_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (object);

    switch (prop_id)
    {
    	case PROP_NAME:
    		g_value_set_string(value, priv->name);
    		break;
        // to do
    }

    g_assert_not_reached ();
}

static void do_list_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (object);

    switch (prop_id)
    {
    	case PROP_NAME:
            //g_free(priv->name);
    		priv->name = g_value_dup_string(value);
    		//update_flags(DO_LIST_VIEW(object));
    		break;
		case PROP_CLIENT:
            if ( !priv->client ) {
                priv->client = g_value_get_pointer(value);
                g_object_ref(priv->client);
            }
			break;
        // to do
    }
}

static void do_list_view_class_init (DoListViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

    o_class->constructor  = do_list_view_constructor;
    o_class->finalize     = do_list_view_finalize;
    o_class->get_property = do_list_view_get_property;
    o_class->set_property = do_list_view_set_property;

    //g_type_class_add_private (o_class, sizeof (DoListViewPrivate));

    g_object_class_install_property
    	(o_class,
    	 PROP_NAME,
    	 g_param_spec_string("name",
    			     "name",
    			     "name",
    			     "goods",
    			     G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (o_class,
				   PROP_CLIENT,
				   g_param_spec_pointer("client",
							"Клиент запросов",
							"Клиент запросов",
							G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
    // to do

}

GtkWidget *do_list_view_new (const gchar *name, DoClient *client)
{
    return g_object_new (DO_TYPE_LIST_VIEW,
    		     "name", name,
    		     "client", client,
    		     // to do
    		     (gpointer) NULL);
}
static 	const char *do_list_view_get_title(DoView *view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);
    return g_strcmp0(priv->name, "goods") == 0 ? "Товары" : priv->name;
}
static void do_list_view_do_close(DoView *view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);
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
	//	do_list_view_fill(DO_LIST_VIEW(view), id, select_id);
		g_free(id);g_free(select_id);
	}
    //gtk_widget_destroy(GTK_WIDGET(view));
}

static gboolean do_list_view_fill_first(DoListView *view)
{
	GtkWidget *window;
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);

	window = gtk_widget_get_toplevel(GTK_WIDGET(view));
	if ( !window || !GTK_IS_WINDOW(window) )
		return TRUE;

/*fix me    if ( !priv->fields ) {
        if ( !priv->fields_filled ) {
            priv->fields_filled = TRUE;
            do_list_view_make_columns(view);
        }
        return TRUE;
    }*/

    GtkTreeModel *model;
    gchar *fields;
    GSList *l;
    gint i;
    gint code_to_col = -1, sort_to_col = -1, key_to_col = -1;
    g_return_val_if_fail (priv->fields != NULL, FALSE);
    fields = g_new0(gchar, g_slist_length(priv->fields));
    for ( i = 0, l = priv->fields; l; l=l->next,i++ ) {
        DoListViewField *field;
        field = l->data;
        fields[i] = field->short_;
        if ( !g_strcmp0(priv->name, "goods") ) { // fix me
            if ( !g_strcmp0(field->name, "code") )
                code_to_col = i;
            else if ( !g_strcmp0(field->name, "name") )
                sort_to_col = i;
        }
    }
    priv->model = model = do_list_model_new(priv->name, fields, priv->client);
    do_list_model_empty_col_change_depricated(DO_LIST_MODEL(model), code_to_col, sort_to_col, key_to_col);
    g_free(fields);
    gtk_tree_view_set_model(priv->tree_view, model);
	return FALSE;
}
/*
static void do_list_view_fill(DoListView *view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);
    GtkApplication *app = gtk_window_get_application(
    		 GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))));
    JsonNode *node;
    gboolean archived = TRUE;
    priv->updated_key = g_strdup(priv->name);
    node = do_application_request_async(DO_APPLICATION(app), "GET", "GetList", priv->name, archived, FALSE,
    		                    (GFunc)model_fill, view,
								"name", priv->name,
								// ""
								 NULL);
    if ( node ) {
        do_list_view_set_load_status(view, "Обновление данных");
    	do_list_view_model_fill(view, node);
    }
    else {
        do_list_view_set_load_status(view, "Запрос данных");
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
	gchar *key = NULL, *sort = NULL, *code = NULL;
	obj = json_node_get_object(node);
	g_assert(obj != NULL);

	if ( json_object_has_member(obj,"key") )
		key = (gchar*)json_object_get_string_member(obj, "key");
	if ( json_object_has_member(obj,"sort") )
		sort = (gchar*)json_object_get_string_member(obj, "sort");
	if ( json_object_has_member(obj,"code") )
		code = (gchar*)json_object_get_string_member(obj, "code");

	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
			           0,key,
					   1,sort,
					   2,code,
					   -1);
}
static void model_fill(JsonNode *node, gpointer view)
{
	DoListViewPrivate *priv;
	priv = DO_LIST_VIEW_GET_PRIVATE (view);
	do_list_view_model_fill(view,node);
	priv->updated_key = NULL;
	do_list_view_set_load_status(view, NULL);
}
*/
/*todo
static gboolean set_view_cursor(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, DoListViewId *ListId)
{
	DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (ListId->view);
	if ((!ListId->id) || ListId->id[0] == '\0' ) {
		gtk_tree_view_set_cursor(priv->tree_view, path, NULL, FALSE);
		return TRUE;
	}
	GValue value = {0,};
	gtk_tree_model_get_value(model,iter,2,&value);
	const gchar *sad;
	sad = g_value_get_string(&value);
	if ( !g_strcmp0(ListId->id, sad) ) {
		gtk_tree_view_set_cursor(priv->tree_view, path, NULL, FALSE);
		return TRUE;
	}
	return FALSE;
}*/
/*
static void do_list_view_model_fill(DoListView *view, JsonNode *node)
{
	DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);
	JsonObject *obj;
	JsonArray *array;
	if ( !node )
		return;
	obj = json_node_get_object(node);
	if ( obj ) {
		gtk_list_store_clear(GTK_LIST_STORE(priv->model));
		array = json_object_get_array_member(obj, "items");
		json_array_foreach_element(array, (JsonArrayForeach)model_append, priv->model);
		DoListViewId ListId;
		ListId.view = view;
		ListId.id = NULL;
		gtk_tree_model_foreach(GTK_TREE_MODEL(priv->model),(GtkTreeModelForeachFunc)set_view_cursor, &ListId);
	}
	//json_node_unref(node);
}
*/
static gboolean do_list_view_close_request(DoView *view)
{
	return TRUE;
}
static void do_list_view_do_edit(DoView *view, const gchar *tab)
{
	DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);

	GtkTreePath *path = NULL;
	GtkTreeIter iter;
	GValue value = {0,};
	const gchar *key;
	gtk_tree_view_get_cursor(priv->tree_view, &path, NULL);

	if ( !path ) return;
	gtk_tree_model_get_iter(priv->model, &iter, path);
	gtk_tree_model_get_value(priv->model, &iter, 0, &value);// key
	key = g_value_get_string(&value);
	if ( key && key[0] != '\0' ) {
		GVariant *parameter;
		parameter = g_variant_new_string(key);
		do_common_action_activate("ObjView", parameter);
		//to do g_variant_unref(parameter);
		return;
	}
}
static 	gboolean do_list_view_can_do_edit(DoView *view, const gchar *tab)
{
	return TRUE;
}
static 	gboolean do_list_view_can_do_close_for_esc (DoView *view)
{
	return TRUE; // to do
}
static 	const gchar *do_list_view_get_load_status(DoView *view)
{
	DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(view);
    return priv->load_status_;
}
static void do_list_view_set_load_status(DoListView *view, const gchar *load_status)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(view);
    g_free(priv->load_status_);
    if ( load_status && load_status[0] != '\0')
        priv->load_status_ = g_strdup(load_status);
    else
        priv->load_status_ = NULL;

}
/*
static gboolean do_list_view_update_columns(JsonNode *node,DoListView *view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(view);
	JsonObject *obj;
    //to do gboolean fill_model = priv->fields == NULL;
    if ( !node )
        return FALSE;
    obj = json_node_get_object(node);
    g_assert(obj != NULL);
    if ( json_object_has_member(obj, "fields") ) {
        if ( !priv->fields )
            json_array_foreach_element(json_object_get_array_member(obj, "fields"), do_list_view_make_column, view);
    }
    //to do if ( fill_model )
    //    do_list_view_fill(view);
    return TRUE;
}*/
static gboolean do_list_view_make_columns(DoListView *view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(view);
    JsonNode *node;
	JsonObject *obj;
    gchar *key;
    key = g_strdup_printf("DESC-%s", priv->name);
    node = do_client_request2(priv->client, "GET", "GetRecord", key, 0,
								"name", priv->name,
								// ""
								 NULL);
    if ( !node ) {
        do_list_view_set_load_status(view, "Запрос данных");
        return FALSE;
    }
    obj = json_node_get_object(node);
    g_assert(obj != NULL);
    if ( json_object_has_member(obj, "fields") ) {
        if ( !priv->fields )
            json_array_foreach_element(json_object_get_array_member(obj, "fields"), do_list_view_make_column, view);
    }
    return TRUE;
}
static void do_list_view_make_column(JsonArray *columns, guint index_, JsonNode *element_node, gpointer data)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(data);
    DoListViewField *field;
	JsonObject *obj;
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;
    gint n;
    const gchar *align;

    obj = json_node_get_object(element_node);
    g_assert(obj != NULL);
    field = g_new0(DoListViewField,1);
    g_assert(json_object_has_member(obj, "short"));
    g_assert(json_object_has_member(obj, "name"));
    field->short_ = json_object_get_string_member(obj, "short")[0];
    field->name = g_strdup(json_object_get_string_member(obj, "name"));
    if ( json_object_has_member(obj, "title") )
        field->title = g_strdup(json_object_get_string_member(obj, "title"));
    field->align = PANGO_ALIGN_LEFT;
    if ( json_object_has_member(obj, "align") ) {
        align = json_object_get_string_member(obj, "align");
        if ( !g_strcmp0(align, "left") )
            field->align = PANGO_ALIGN_LEFT;
        else if ( !g_strcmp0(align, "center") )
            field->align = PANGO_ALIGN_CENTER;
        else if ( !g_strcmp0(align, "right") )
            field->align = PANGO_ALIGN_RIGHT;
    }
    if ( !priv->userfields || strchr(priv->userfields, field->short_) ) {
        priv->fields = g_slist_append(priv->fields, field);

        r = gtk_cell_renderer_text_new();
        g_object_set(r, "xalign", field->align == PANGO_ALIGN_CENTER ? 0.5 : (field->align == PANGO_ALIGN_RIGHT ? 1. : 0.), NULL);

        col = do_tree_view_add_column(DO_TREE_VIEW(priv->do_view), field->name, field->title ? field->title : "", 100);
        gtk_tree_view_column_pack_start (col, r, TRUE);
        n = g_slist_length(priv->fields) + DO_LIST_MODEL_N_KEYS - 1;
        gtk_tree_view_column_add_attribute (col, r, "markup", n);
    }
}
#ifdef DEBUG
static void do_list_view_make_key_column(DoListView *view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(view);
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->do_view),"key", "key", 150);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);
}
#endif
