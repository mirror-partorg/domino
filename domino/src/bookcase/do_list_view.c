
#include "do_list_view.h"
#include "do_list_model.h"
#include "do_tree_view.h"
#include "do_application.h"
#include "do_utilx.h"
#include "do_common_actions.h"
#include "do_view_actions.h"
#include "domino.h"


#define DO_LIST_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_LIST_VIEW, DoListViewPrivate))
#define DO_TYPE_LIST_VIEW_FLAGS do_list_view_flags_get_type()

#define ROOT_OBJECT_ "list-%s-view"

#define DO_LIST_VIEW_SEARCH_DIALOG_TIMEOUT 5000

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
    guint type;
};
enum {
    DO_LIST_VIEW_SEARCH_ITEM_SORT = 0,
    DO_LIST_VIEW_SEARCH_ITEM_CODE,
    DO_LIST_VIEW_SEARCH_ITEM_BCODE
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
static 	void		do_list_view_do_apply		(DoView *view);
static 	gboolean	do_list_view_can_do_apply	(DoView *view);
static 	void		do_list_view_do_unapply		(DoView *view);
static 	gboolean	do_list_view_can_do_unapply	(DoView *view);
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
//static void do_list_view_make_key_column(DoListView *view);
#endif

static gboolean do_list_view_key_press(GtkWidget *widget, GdkEventKey *event, DoListView *view);
static void do_list_view_cell_sort_data_func(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, DoListView *view);
static void do_list_view_cell_data_func(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, DoListView *view);

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
    GtkWidget     *entry;
    //gboolean       fields_filled;

    gchar          search_text[1024];
    gint           search_char_count;
    gint           search_item;
    guint          search_flush_timeout;
    gint           search_sort_col;
    gint           search_code_col;

    DoView        *receiver;
    guint          source_load;
    gchar         *set_cursor_for_key;

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
	iface->do_apply = do_list_view_do_apply;
	iface->can_do_apply = do_list_view_can_do_apply;
	iface->do_unapply = do_list_view_do_unapply;
	iface->can_do_unapply = do_list_view_can_do_unapply;
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

    priv->entry = gtk_label_new("");
    do_tree_view_pack_header(priv->do_view, priv->entry);


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
//    do_list_view_make_key_column(DO_LIST_VIEW(object));
#endif



    g_signal_connect (priv->tree_view, "key-press-event", G_CALLBACK (do_list_view_key_press), object);


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
static gboolean set_view_cursor(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, DoListView *view);
static gboolean do_list_view_update_title(gpointer data)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(data);
    guint percent;
    gchar *buf;
    if ( !do_list_model_full_readed(DO_LIST_MODEL(priv->model), &percent) ) {
        gtk_label_set_text(GTK_LABEL(priv->entry), "");
        gtk_widget_set_visible(GTK_WIDGET(priv->entry), FALSE);
        gchar *key;
        key = g_strdup_printf("%s.read", priv->name);
        buf = g_strdup("{}");
        JsonNode *node;
        node = json_node_new(JSON_NODE_OBJECT);
        do_client_set_cache(priv->client, key, node, buf, -1);
        g_free(key);g_free(buf);
        return FALSE;
    }
    buf = g_strdup_printf("<b>Первоначальное заполнение(%d%%)...</b>", percent);
    gtk_label_set_markup(GTK_LABEL(priv->entry), buf);
    g_free(buf);
    return TRUE;
}
static void do_list_view_read_records(DoListView *view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(view);
    do_list_model_full_read(DO_LIST_MODEL(priv->model));
    gtk_label_set_markup(GTK_LABEL(priv->entry), "<b>Первоначальное заполнение...</b>");
    priv->source_load = g_timeout_add(1000, do_list_view_update_title, view);
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
    fields = g_new0(gchar, g_slist_length(priv->fields) + 1);
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
	gtk_tree_model_foreach(GTK_TREE_MODEL(priv->model),(GtkTreeModelForeachFunc)set_view_cursor, view);
	gchar *key;
	key = g_strdup_printf("%s.read", priv->name);
    if ( !do_client_get_cache(priv->client, key, NULL) ) {
        do_list_view_read_records(view);
    }
    else {
        gtk_widget_set_visible(GTK_WIDGET(priv->entry), FALSE);
    }
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

static gboolean set_view_cursor(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, DoListView *view)
{
	DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);
	gtk_tree_view_set_cursor(priv->tree_view, path, NULL, FALSE);
	return TRUE;
}
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
	}
}
static 	gboolean do_list_view_can_do_edit(DoView *view, const gchar *tab)
{
	return TRUE;
}
static 	gboolean do_list_view_can_do_close_for_esc (DoView *view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);
    if ( priv->updated_key ) {
        return TRUE;
    }
	if ( priv->path && g_slist_length(priv->path) > 1 ) {
        return TRUE;
	}
	return FALSE;
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
    if ( json_object_has_member(obj, "type") ) {
        if ( !g_strcmp0(json_object_get_string_member(obj, "type"), "pixbuf") )
            field->type = 1;
    }
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
        n = g_slist_length(priv->fields) + DO_LIST_MODEL_N_KEYS - 1;
        if ( field->type == 1 )
            r = gtk_cell_renderer_pixbuf_new();
        else {
            r = gtk_cell_renderer_text_new();
            g_object_set(r, "xalign", field->align == PANGO_ALIGN_CENTER ? 0.5 : (field->align == PANGO_ALIGN_RIGHT ? 1. : 0.), NULL);
        }
        col = do_tree_view_add_column(DO_TREE_VIEW(priv->do_view), field->name, field->title ? field->title : "", 100);
        gtk_tree_view_column_pack_start (col, r, TRUE);
        if ( field->type == 0 ) {
            gtk_tree_view_column_add_attribute (col, r, "markup", n);
            if ( !g_strcmp0(field->name, "name") && !g_strcmp0(priv->name, "goods") ) {
                priv->search_sort_col = n;
                gtk_tree_view_column_set_cell_data_func(col, r, (GtkTreeCellDataFunc)do_list_view_cell_sort_data_func, data, NULL);
            }
            else
            if ( !g_strcmp0(field->name, "code") && !g_strcmp0(priv->name, "goods") ) {
                priv->search_code_col = n;
                gtk_tree_view_column_set_cell_data_func(col, r, (GtkTreeCellDataFunc)do_list_view_cell_sort_data_func, data, NULL);
            }
            else
                gtk_tree_view_column_set_cell_data_func(col, r, (GtkTreeCellDataFunc)do_list_view_cell_data_func, data, NULL);
        }
        else if ( field->type == 1 ) {
            gtk_tree_view_column_add_attribute (col, r, "icon-name", n);
        }
    }
}
#ifdef DEBUG
/*static void do_list_view_make_key_column(DoListView *view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(view);
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->do_view),"key", "key", 150);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);
}*/
#endif
static gboolean search_add(DoListView *do_view, gchar *string);
static void search_back(DoListView *do_view);
static void search_clear_to_begin_word(DoListView *do_view);
static void search_go_to_end_word(DoListView *do_view);
static void search_refresh_timeout(DoListView *do_view);
static gboolean search_clear(DoListView *do_view);
static gboolean search_find(DoListView *do_view, const char *text);
#ifdef CASH
static void do_list_view_send(DoListView *list);
#endif
#ifdef SEARCH_BCODE
static gboolean search_find_by_bcode(DoListView *do_view, const char *text);
#endif // SEARCH_BCODE
//static gboolean start_find(GtkTreeView *treeview, gpointer user_data);
static gboolean search_flush_timeout(DoListView *do_view);
static gchar *get_sort_from_cursor(DoListView *view);

static gboolean do_list_view_key_press(GtkWidget *widget, GdkEventKey *event, DoListView *do_view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    gchar *str;
#ifdef ADS_RECOMMEND
    recommend_refresh_timeout(do_view);
#endif
    if ( (str = search_get_text(event->string, event->length, priv->search_char_count)) != NULL ) {
#ifdef HUMAN_SEARCH
        if ( !search_add(do_view, str) ) {
            gchar *buf;
            buf = g_strdup_printf("%s%s",priv->search_text,str);
            do_product_view_filter_set_text(do_view, buf);
            g_free(buf);
            return TRUE;
        }
#else
        search_add(do_view, str);
#endif
        g_free(str);
        return TRUE;
    }
#ifdef FILTER
    if ( priv->filter_show_timeout ) {
        g_source_remove(priv->filter_show_timeout);
        priv->filter_show_timeout = 0;
    }
    if ( do_product_model_is_filtered(DO_PRODUCT_MODEL(priv->model)) )
        priv->filter_show_timeout =
            gdk_threads_add_timeout (DO_PRODUCT_VIEW_FILTER_SHOW_TIMEOUT,
                (GSourceFunc)filter_show_timeout, do_view);
#endif
    guint mask = gtk_accelerator_get_default_mod_mask ();
    if ( (event->state & mask) == 0 )
    {
    	switch (event->keyval)
    	{

    	    //case GDK_space:
              //  mask = gtk_accelerator_get_default_mod_mask ();
                //return TRUE;
    	    case GDK_KEY_BackSpace:
                search_back(do_view);
                return TRUE;
            case GDK_KEY_Escape:
                if ( do_list_model_is_filtered(DO_LIST_MODEL(priv->model), NULL) ) {

                    GValue value = {0,};
                    //const gchar *key = NULL;
                    GtkTreePath *path;
                    gtk_tree_view_get_cursor(priv->tree_view, &path, NULL);
                    if ( path ) {
                        GtkTreeIter iter;
                        gtk_tree_model_get_iter(priv->model, &iter, path);
                        gtk_tree_model_get_value(priv->model, &iter, DO_LIST_MODEL_COL_KEY, &value);
                        priv->set_cursor_for_key = g_value_dup_string(&value);
                        gtk_tree_path_free(path);
                    }
                    /*do_list_model_set_updated(DO_LIST_MODEL(priv->model), FALSE);
                    do_list_model_set_filter(DO_LIST_MODEL(priv->model), NULL);
                    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(priv->tree_view), TRUE);//fix
                    if ( key ) {
                        GtkTreeIter iter;
                        if ( do_list_model_find_record_by_key(DO_LIST_MODEL(priv->model), &iter, key) ) {
                            path = gtk_tree_model_get_path(priv->model, &iter);
                            gtk_tree_view_set_cursor(priv->tree_view, path, NULL, FALSE);
                        }
                    }
                    do_list_model_set_updated(DO_LIST_MODEL(priv->model), TRUE);fix me*/
                    do_window_end_search(DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(do_view))));
                    return TRUE;
                }
                if ( !priv->search_char_count &&
                     priv->receiver ) {
                    GtkNotebook *nb;
                    nb = GTK_NOTEBOOK (do_window_get_notebook (
                        DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(do_view)))));
                    do_view_do_grab_focus(DO_VIEW(priv->receiver));
                    if ( gtk_notebook_page_num(nb, GTK_WIDGET(do_view)) !=
                         gtk_notebook_page_num(nb, GTK_WIDGET(priv->receiver) )
                        ) {
                        gtk_notebook_set_current_page(nb, gtk_notebook_page_num(nb, GTK_WIDGET(priv->receiver)));
                    }
                }
#ifdef ADS_RECOMMEND
                if ( !priv->recommend_empty && !priv->search_char_count && priv->flags & DO_PRODUCT_VIEW_FLAGS_SHOW_RECOMMEND ) {
                    if ( priv->refill ) {
                        do_product_view_refill_recommend(do_view);
                    }
                    gtk_widget_grab_focus(priv->recommend);
                    gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), 1);

                    //gtk_widget_set_visible(GTK_WIDGET(priv->tree_view), FALSE);
                    //!!do_tree_view_tree_view_set_visible(DO_TREE_VIEW(do_view), FALSE);
                    //gtk_widget_set_visible(GTK_WIDGET(priv->recommend), TRUE);

                }
#endif
                if ( search_clear(do_view) )
                    return TRUE;
#ifdef FILTER
                if ( do_product_model_is_filtered(DO_PRODUCT_MODEL(priv->model)) ) {
                    char *code;
                    code = get_code_from_cursor(do_view);
                    do_product_view_filter_clear(do_view);
                    if ( code ) {
                        GtkTreePath *path;
                        path = do_product_model_get_path_from_code(DO_PRODUCT_MODEL(priv->model), code);
                        g_free(code);
                        if ( path ) {
                            gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
                            gtk_tree_path_free(path);
                        }
                    }
                    return TRUE;
                }
#endif
                //return TRUE;
                return FALSE;
            case GDK_KEY_Up:
            case GDK_KEY_Down:
            case GDK_KEY_Page_Up:
            case GDK_KEY_Page_Down:
                search_clear(do_view);
                break;
            case GDK_KEY_Return:
            case GDK_KEY_KP_Enter:
            case GDK_KEY_ISO_Enter:
#ifdef SEARCH_BY_BCODE
                if (priv->search_item == DO_PRODUCT_VIEW_SEARCH_ITEM_BCODE) {
                    search_find_by_bcode(do_view, priv->search_text);
                    search_clear(do_view);
                    return TRUE;
                }
#endif
                search_clear(do_view);
                break;
            case GDK_KEY_Right: {
#ifdef CASH
                gchar *text = NULL;
                if ( priv->search_text && strlen(priv->search_text) > 0 ) {
                    text = g_strdup( priv->search_text);
                }
                do_window_start_search(DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(do_view))), text);
                if ( text )
                    g_free(text);
                return TRUE;
#else
                if ( priv->search_item == DO_LIST_VIEW_SEARCH_ITEM_SORT ) {
                    search_go_to_end_word(do_view);
                    return TRUE;
                }
                break;
#endif
            }
            case GDK_KEY_Left:
                if ( priv->search_char_count > 0 && priv->search_item == DO_LIST_VIEW_SEARCH_ITEM_SORT ) {
                    search_clear_to_begin_word(do_view);
                    return TRUE;
                }
                break;
#ifdef CASH
            case GDK_KEY_F5:
                if ( g_strcmp0(priv->name, "goods") == 0 ) {
                    do_list_view_send(do_view);
                }
                break;
#endif
    	default:
    		break;
    	}
    }
    else if ((event->state & mask) == GDK_CONTROL_MASK) {
         if  (event->keyval == GDK_KEY_Return ||
              event->keyval == GDK_KEY_KP_Enter ||
              event->keyval == GDK_KEY_ISO_Enter ||
              event->keyval == GDK_KEY_Up ||
              event->keyval == GDK_KEY_Down ||
              event->keyval == GDK_KEY_Page_Up ||
              event->keyval == GDK_KEY_Page_Down ||
              event->keyval == GDK_KEY_Escape)
                search_clear(do_view);
         else {
            if ( event->keyval == GDK_KEY_F1 ) {
                gtk_tree_view_set_headers_visible(priv->tree_view, !gtk_tree_view_get_headers_visible(priv->tree_view));
                return TRUE;
            }
         }
    }
    else if ((event->state & mask) == GDK_SHIFT_MASK) {
#if !GTK_CHECK_VERSION(3,12,0)
         if ( event->keyval == GDK_KEY_Delete ) {
            event->keyval = GDK_KEY_Delete;
            return TRUE;
         }
#endif
    }
    return FALSE;
}
static gboolean search_add(DoListView *do_view, gchar *string)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    gboolean res = FALSE;
    int len;
    gchar *p;
    for (len = 0, p = string; p && *p != '\0'; p = (gchar*)g_utf8_next_char(p), len++);
    gchar *buf = do_malloc(strlen(priv->search_text) + strlen(string) + 1);
    strcpy(buf, priv->search_text);
    strcpy(buf + strlen(priv->search_text), string);
    if ( search_find(do_view, buf) ) {
        strcpy(priv->search_text + strlen(priv->search_text), string);
        priv->search_char_count +=len;
        search_refresh_timeout(do_view);
        res = TRUE;
    }
    else {
        if (priv->search_flush_timeout)
            g_source_remove(priv->search_flush_timeout);
        priv->search_flush_timeout = 0;
    }
    do_free(buf);
    return res;
}
static void search_back(DoListView *do_view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    if ( priv->search_char_count ) {
        priv->search_char_count--;
        int i;
        gchar *p;
        for ( i = 0, p = priv->search_text; i < priv->search_char_count; i++, p = (gchar*)g_utf8_next_char(p) );
        *p = '\0';
        if (p == priv->search_text)
            search_clear(do_view);
        else {
            search_refresh_timeout(do_view);
            gtk_tree_view_row_cursor_redraw(GTK_TREE_VIEW(priv->tree_view));
        }
    }
}
static void search_clear_to_begin_word(DoListView *do_view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    gchar *text = get_sort_from_cursor(do_view);
    if (text) {
        gunichar out;
        char *p;
        int count;
        for (count = 0, p = (gchar*)text; p && *p !='\0' && count < priv->search_char_count - 1;
                   p = (gchar*)g_utf8_next_char(p), count++);

        for (; p && p > text && count > 0; p = (gchar*)g_utf8_prev_char(p), --count) {
            out = g_utf8_get_char(p);
            if (g_unichar_isspace(out)){
                break;
            }
        }
        priv->search_char_count = count;
        if (!p)
            priv->search_text[0] = '\0';
        else
            priv->search_text[p - text] = '\0';
        g_free(text);
    }
    if (!priv->search_char_count)
        search_clear(do_view);
    else {
        search_refresh_timeout(do_view);
        gtk_tree_view_row_cursor_redraw(GTK_TREE_VIEW(priv->tree_view));
    }
}
static void search_go_to_end_word(DoListView *do_view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    gchar *text = get_sort_from_cursor(do_view);
    if ( text ) {
        gunichar out;
        char *p;
        int count;
        for (count = 0, p = (gchar*)text; p && *p !='\0' && count < priv->search_char_count - 1;
                   p = (gchar*)g_utf8_next_char(p), count++);
        for (; p && *p !='\0';
                   p = (gchar*)g_utf8_next_char(p), count++ ) {
            out = g_utf8_get_char(p);
            if (g_unichar_isspace(out) && priv->search_char_count < count - 1)
                break;
        }
        priv->search_char_count = count;
        strcpy(priv->search_text, text);
        if ( p )
            priv->search_text[p - text] = '\0';
        g_free(text);
    }
    search_refresh_timeout(do_view);
    gtk_tree_view_row_cursor_redraw(GTK_TREE_VIEW(priv->tree_view));
}
static void search_refresh_timeout(DoListView *do_view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    if (priv->search_flush_timeout)
        g_source_remove(priv->search_flush_timeout);

    priv->search_flush_timeout =
        gdk_threads_add_timeout (DO_LIST_VIEW_SEARCH_DIALOG_TIMEOUT,
    	   (GSourceFunc)search_flush_timeout, do_view);
}
static gboolean search_clear(DoListView *do_view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    gboolean re;
    re=priv->search_char_count > 0;
    priv->search_char_count = 0;
    priv->search_text[0] = '\0';
    if ( re ) {
        gtk_tree_view_redraw(GTK_TREE_VIEW(priv->tree_view));

        if (priv->search_flush_timeout) {
            g_source_remove(priv->search_flush_timeout);
            priv->search_flush_timeout = 0;
        }
        do_view_actions_refresh(gtk_widget_get_toplevel(GTK_WIDGET(do_view)));
    }
    return re;
}
static gboolean is_code(const gchar *text)
{
    gchar *p;
    gunichar out;
    int i;
    for (i = 0, p = (gchar*)text; p && *p !='\0'; p = (gchar*)g_utf8_next_char(p), i++) {
        out = g_utf8_get_char(p);
        if (i > 7 || !g_unichar_isdigit(out))
            return FALSE;
    }
    return TRUE;
}
#ifdef SEARCH_BCODE
static gboolean is_bcode(const gchar *text)
{
    gchar *p;
    gunichar out;
    int i;
    for (i = 0, p = (gchar*)text; p && *p !='\0'; p = (gchar*)g_utf8_next_char(p), i++) {
        out = g_utf8_get_char(p);
        if (i > 13 || !g_unichar_isdigit(out))
            return FALSE;
    }
    return TRUE;
}
#endif
static gboolean search_find(DoListView *do_view, const char *text)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    if (!text || !text[0])
        return TRUE;

    if (priv->search_item == DO_LIST_VIEW_SEARCH_ITEM_SORT) {
        gchar *crnt_text;
        if ((crnt_text = get_sort_from_cursor(do_view)) != NULL) {
            if ((strlen(crnt_text) >= strlen(text)) &&
                (!strncmp(crnt_text, text, strlen(text)))) {
                    g_free(crnt_text);
                    gtk_tree_view_redraw(GTK_TREE_VIEW(priv->tree_view));
                    return TRUE;
            }
            g_free(crnt_text);
        }
    }
    GtkTreeIter iter;
    gboolean result = FALSE;

    if ( do_list_model_find_record_by_sort(DO_LIST_MODEL(priv->model), &iter, text) ) {
        result = TRUE;
        priv->search_item = DO_LIST_VIEW_SEARCH_ITEM_SORT;
    }
    else if ( is_code(text) && do_list_model_find_record_by_code(DO_LIST_MODEL(priv->model), &iter, text) ) {
        result = TRUE;
        priv->search_item = DO_LIST_VIEW_SEARCH_ITEM_CODE;
    }
#ifdef SEARCH_BY_BCODE
    else if ( is_bcode(text) ) {
        result = TRUE;
        priv->search_item = DO_LIST_VIEW_SEARCH_ITEM_BCODE;
        do_view_actions_refresh();
    }
#endif
    if ( result ) {
        GtkTreePath *path;
        path = gtk_tree_model_get_path(priv->model, &iter);
        if ( path ) {
            gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(priv->tree_view), path, NULL,  TRUE, 0.5, 0.0);
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
            gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(priv->tree_view), path, NULL,  TRUE, 0.5, 0.0);
            gtk_tree_path_free(path);
        }
    }
    return result;
}
/*
static gboolean start_find(GtkTreeView *treeview, gpointer data)
{
    //return TRUE;
    gboolean sad;
    g_object_get(treeview, "enable-search", &sad, NULL);
    printf("sad %d\n",sad);
    return FALSE;
}*/
static gboolean search_flush_timeout(DoListView *do_view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    search_clear(do_view);
    priv->search_flush_timeout = 0;
    return FALSE;
}
static gchar *get_sort_from_cursor(DoListView *do_view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        return NULL;
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {

        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_LIST_MODEL_COL_SORT, &value);
        gtk_tree_path_free(path);
        if (!g_value_get_string(&value))
            return NULL;
        return g_strdup(g_value_get_string(&value));
    }
    gtk_tree_path_free(path);
    return NULL;
}
#ifdef CASH
static gchar *get_code_from_cursor(DoListView *do_view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(do_view);
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        return NULL;
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {

        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_LIST_MODEL_COL_CODE, &value);
        gtk_tree_path_free(path);
        if (!g_value_get_string(&value))
            return NULL;
        return g_strdup(g_value_get_string(&value));
    }
    gtk_tree_path_free(path);
    return NULL;
}
#endif
/*
static gchar *get_code_from_path(DoListView *view, GtkTreePath *path)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(view);
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {
        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_LIST_MODEL_COL_CODE, &value);
        if (!g_value_get_string(&value))
            return NULL;
        return g_strdup(g_value_get_string(&value));
    }
    return NULL;
}*/
static void do_list_view_get_cell_color(DoListView *view, gint *bg_red, gint *bg_green, gint *bg_blue, gint *fg_red, gint *fg_green, gint *fg_blue, gint *bg_red_s, gint *bg_green_s, gint *bg_blue_s);

static void do_list_view_cell_sort_data_func(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, DoListView *view)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);
    //GValue value = { 0, };
    char *text;

    //gtk_tree_model_get_value(tree_model, iter, priv->search_sort_col, &value);
    g_object_get(cell, "text", &text, NULL);
    //text = (char*)g_value_get_string(&value);

    //do_log(LOG_INFO)
    //char *text = (char*)g_value_get_string(&value);
    if ( text && text[0] ) {
        int i, j;
        GtkTreePath *path, *path1;
        path = gtk_tree_model_get_path(tree_model, iter);
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path1, NULL);
        gboolean select;
        select = path1 && path && !gtk_tree_path_compare(path, path1);
        if ( path ) gtk_tree_path_free(path);
        if ( path1 ) gtk_tree_path_free(path1);
        gint bg_red, bg_green, bg_blue, fg_red, fg_green, fg_blue, bg_red_s, bg_green_s, bg_blue_s;
        do_list_view_get_cell_color(view, &bg_red, &bg_green, &bg_blue, &fg_red, &fg_green, &fg_blue, &bg_red_s, &bg_green_s, &bg_blue_s);
        gchar **lexems;
        gchar *markup = NULL;
        if ( do_list_model_is_filtered(DO_LIST_MODEL(priv->model), &lexems) ) {
            gchar *buf, *buf1;
            gint crnt;
            buf = NULL;
            markup = g_strdup(text);
            for ( i = 0; i < g_strv_length(lexems); i++ ) {
                gchar **bufv;
                buf1 = g_strdup(markup);
                buf = to_upper_text(markup);
                gchar *lexem, *item, *itemlex;
                lexem = to_upper_text(lexems[i]);
                bufv = g_strsplit(buf, lexem, -1);
                crnt = 0;
                if ( g_strv_length(bufv) > 1 ) {
                    g_free(markup);
                    for ( j = 0; j < g_strv_length(bufv); j++ ) {
                        if ( j ) {
                            g_free(buf);
                            buf = g_strdup(markup);
                            g_free(markup);
                        }
                        if ( j == g_strv_length(bufv) - 1 ) {
                            //markup = g_strdup_printf("%s%s", buf, bufv[j]);
                            markup = g_strdup_printf("%s%s", buf, buf1 + crnt);
                            g_free(buf);
                            buf = g_strdup(markup);
                        }
                        else {
                            item = g_strndup(buf1+crnt, strlen(bufv[j]));
                            itemlex = g_strndup(buf1+crnt+strlen(bufv[j]), strlen(lexems[i]));
                            crnt += strlen(bufv[j]) + strlen(lexems[i]);
                            if ( select )
                                markup = g_strdup_printf("%s%s<b><span background=\"#%2.2hX%2.2hX%2.2hX\" foreground=\"#%2.2hX%2.2hX%2.2hX\">%s</span></b>", j ? buf : "", item,
                                         bg_red, bg_green, bg_blue, fg_red, fg_green, fg_blue, itemlex);
                            else {
                                //markup = g_strdup_printf("%s%s<b><span foreground=\"#%2.2hX%2.2hX%2.2hX\">%s</span></b>", j ? buf : "", bufv[j], bg_red_s, bg_green_s, bg_blue_s, lexem);
                                markup = g_strdup_printf("%s%s<b><span foreground=\"#%2.2hX%2.2hX%2.2hX\">%s</span></b>", j ? buf : "", item, bg_red_s, bg_green_s, bg_blue_s, itemlex);

                            }
                            g_free(item);
                            g_free(itemlex);
                        }
                    }
                }
                g_free(lexem);
                g_strfreev(bufv);
                buf1 = buf;
            }
            if (buf) g_free(buf);
        }
        else {
            g_object_set(cell, "background", NULL, NULL);
            if ( select && priv->search_char_count ) {
                gchar *buf;
                buf = to_ru_upper_text(text);
                if ( !strncmp(buf, priv->search_text, priv->search_char_count) ) {
                    gchar *selected_text = g_strdup(text);
                    gchar *tail;
                    for (i = 0, tail = text; tail && *tail !='\0' && i < priv->search_char_count;
                        tail = (gchar*)g_utf8_next_char(tail), i++);

                    selected_text[tail - text] = '\0';
                    markup = g_markup_printf_escaped("<span background=\"#%2.2hX%2.2hX%2.2hX\" foreground=\"#%2.2hX%2.2hX%2.2hX\">%s</span>%s",
                                         bg_red,bg_green,bg_blue,fg_red,fg_green,fg_blue,
                                         selected_text, tail ? tail : "");
                    g_free(selected_text);
                }
                g_free(buf);
            }
        }
        if ( markup ) {
            g_object_set(cell, "markup", markup, NULL);
            g_free(markup);
        }
    }
}
static void do_list_view_get_cell_color(DoListView *view, gint *bg_red, gint *bg_green, gint *bg_blue, gint *fg_red, gint *fg_green, gint *fg_blue, gint *bg_red_s, gint *bg_green_s, gint *bg_blue_s)
{
#if GTK_MAJOR_VERSION > 2
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);
    GdkRGBA color_bg, color_fg, color_bg_s;
    GtkStyleContext *context;

    context = gtk_widget_get_style_context(GTK_WIDGET(priv->tree_view));
    gtk_style_context_get_background_color(GTK_STYLE_CONTEXT(context), GTK_STATE_FLAG_NORMAL, &color_bg);
    gtk_style_context_get_background_color(GTK_STYLE_CONTEXT(context), GTK_STATE_FLAG_SELECTED, &color_bg_s);
    gtk_style_context_get_color(GTK_STYLE_CONTEXT(context), GTK_STATE_FLAG_NORMAL, &color_fg);

    *bg_red = color_bg.red*255;*bg_green=color_bg.green*255;*bg_blue=color_bg.blue*255;
    *fg_red = color_fg.red*255;*fg_green=color_fg.green*255;*fg_blue=color_fg.blue*255;
    *bg_red_s = color_bg_s.red*255;*bg_green_s=color_bg_s.green*255;*bg_blue_s=color_bg_s.blue*255;

#else
    GtkStyle *style = gtk_widget_get_style(GTK_WIDGET(view));
    *bg_red = style->base[GTK_STATE_NORMAL].red;*bg_green=style->base[GTK_STATE_NORMAL].green;*bg_blue=style->base[GTK_STATE_NORMAL].blue;
    *fg_red = style->fg[GTK_STATE_NORMAL].red;*fg_green=style->fg[GTK_STATE_NORMAL].green;*fg_blue=style->fg[GTK_STATE_NORMAL].blue;
    *bg_red_s = style->base[GTK_STATE_SELECTED].red;*bg_green_s=style->base[GTK_STATE_SELECTED].green;*bg_blue_s=style->base[GTK_STATE_SELECTED].blue;
#endif
}
static void do_list_view_cell_data_func(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, DoListView *view)
{
    //DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);

    if ( !do_list_model_record_is_relevant(DO_LIST_MODEL(tree_model), iter) ) {
        gchar *markup, *text;
        g_object_get(cell, "text", &text, NULL);
        markup = g_strdup_printf("<i>%s</i>", text);
        g_object_set(cell, "markup", markup, NULL);
        g_free(markup);
    }
}
void do_list_view_external_search(DoListView *view, JsonNode *node)
{
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE (view);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(priv->tree_view), node == NULL);
    if ( !do_list_model_full_readed(DO_LIST_MODEL(priv->model), NULL) ) {
        do_list_model_set_filter(DO_LIST_MODEL(priv->model), node);
        if ( priv->set_cursor_for_key ) {
            GtkTreePath *path;
            GtkTreeIter iter;
            if ( do_list_model_find_record_by_key(DO_LIST_MODEL(priv->model), &iter, priv->set_cursor_for_key) ) {
                path = gtk_tree_model_get_path(priv->model, &iter);
                gtk_tree_view_set_cursor(priv->tree_view, path, NULL, FALSE);
            }
            g_free(priv->set_cursor_for_key);
            priv->set_cursor_for_key = NULL;
        }

    }
}
static void do_list_view_mark_(DoListView *view, const gchar *mark)
{

    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(view);
    if ( !g_strcmp0(priv->name, "goods") ) {
        GtkTreePath *path;
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
        if ( path ) {
            GtkTreeIter iter = {0,};
            GValue value = {0,};
            gtk_tree_model_get_iter(priv->model, &iter, path);
            gtk_tree_model_get_value(priv->model, &iter, DO_LIST_MODEL_COL_CODE, &value);
            do_client_request2(priv->client, "POST", "SetOrder", NULL, 0,
                                        "name", priv->name,
                                        "code", g_value_get_string(&value),
                                        "mark", mark,
                                        // ""
                                         NULL);
            do_list_model_record_update(DO_LIST_MODEL(priv->model), &iter);

            gtk_tree_path_free(path);
        }
    }
}
static void do_list_view_do_apply(DoView *view)
{
    do_list_view_mark_(DO_LIST_VIEW(view), "1");
}
static gboolean	do_list_view_can_do_apply(DoView *view)
{
    gboolean retval = FALSE;
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(view);
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if ( path ) {
        retval = TRUE;
        gtk_tree_path_free(path);
    }
    return retval;
}
static void	do_list_view_do_unapply(DoView *view)
{
    do_list_view_mark_(DO_LIST_VIEW(view), "0");
}
static gboolean	do_list_view_can_do_unapply(DoView *view)
{
    gboolean retval = FALSE;
    DoListViewPrivate *priv = DO_LIST_VIEW_GET_PRIVATE(view);
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if ( path ) {
        retval = TRUE;
        gtk_tree_path_free(path);
    }
    return retval;
}
#ifdef CASH
static gboolean sad(gpointer data)
{
    do_log(LOG_INFO, "code %s 0 ", (char*)data);
    domino_send_barcode_eventkey_to_crntwin((char *)data);
    do_log(LOG_INFO, "code %s 1 ", (char*)data);
    do_free(data);
    return FALSE;
}
static void do_list_view_send(DoListView *view)
{
    gchar *code;
    gboolean parcel = FALSE;
    code = get_code_from_cursor(DO_LIST_VIEW(view));
    if ( !code )
        return;
    g_print("code %s\n", code);
    domino_back_window_focus();
    g_timeout_add(300, sad,code);
}
#endif
