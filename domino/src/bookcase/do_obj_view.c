
#include "do_obj_view.h"
#include "do_tree_view.h"
#include "do_application.h"
#include "do_common_actions.h"
#include "do_utilx.h"
#include <string.h>

#define DO_OBJ_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_OBJ_VIEW, DoObjViewPrivate))
#define DO_TYPE_OBJ_VIEW_FLAGS do_obj_view_flags_get_type()

#define ROOT_OBJECT "obj-view"
#define TREE_VIEW_MORE_TEXT "Показать больше данных (перейдите в конец списка и нажмите ↓)"
#define WAITING_DATA_TEXT "<b>Ожидание получения данных</b>..."
#define EMPTY_DATA_TEXT "<i>Данные не получены</i>"
static void do_obj_view_view_init(DoViewIface *iface);

static	gboolean	do_obj_view_close_request	(DoView *view);
//static 	void		do_obj_view_do_delete		(DoView *view);
//static 	gboolean	do_obj_view_can_do_delete	(DoView *view);
//static 	void		do_obj_view_do_insert		(DoView *view);
//static 	gboolean	do_obj_view_can_do_insert	(DoView *view);
//static 	void		do_obj_view_do_copy 		(DoView *view);
//static 	gboolean	do_obj_view_can_do_copy	(DoView *view);
static 	void		do_obj_view_do_edit		(DoView *view, const gchar *tab);
static 	gboolean	do_obj_view_can_do_edit	(DoView *view, const gchar *tab);
//static 	void		do_obj_view_do_apply		(DoView *view);
//static 	gboolean	do_obj_view_can_do_apply	(DoView *view);
//static 	void		do_obj_view_do_unapply		(DoView *view);
//static 	gboolean	do_obj_view_can_do_unapply	(DoView *view);
//static 	void		do_obj_view_do_mail_send	(DoView *view);
//static 	gboolean	do_obj_view_can_do_mail_send (DoView *view);
//static 	void		do_obj_view_do_popup		(DoView *view, GdkEventButton* event);
static 	gboolean	do_obj_view_do_grab_focus	(DoView *view);
//static 	void		do_obj_view_do_print		(DoView *view);
//static 	void		do_obj_view_do_print_now	(DoView *view);
//static 	gboolean	do_obj_view_can_do_print	(DoView *view);
//static 	void		do_obj_view_do_open 	 	(DoView *view);
//static 	gboolean	do_obj_view_can_do_open 	(DoView *view);
//static 	void		do_obj_view_do_save 	 	(DoView *view);
//static 	void		do_obj_view_do_save_as	 	(DoView *view);
//static 	gboolean	do_obj_view_can_do_save 	(DoView *view);
//static 	gboolean	do_obj_view_can_do_save_as (DoView *view);
static 	void	 	do_obj_view_do_close	 	(DoView *view);
static 	const char *do_obj_view_get_title 		(DoView *view);
//static 	GdkPixbuf  *do_obj_view_get_icon	 	(DoView *view);
//static 	gboolean	do_obj_view_get_load_status(DoView *view);
//static 	gint 		do_obj_view_get_load_progress(DoView *view);
//static 	DoMarked	do_obj_view_get_marked 	(DoView *view, DoContext *context);
//static 	gboolean 	do_obj_view_set_marked 	(DoView *view, DoContext *context, DoMarked marked);
//static 	gboolean 	do_obj_view_toggle_marked 	(DoView *view, DoContext *context);
//static 	gboolean 	do_obj_view_sender_do_close(DoView *view);
//static 	void	 	do_obj_view_update_context (DoView *view, DoContext *context);
//static 	void	 	do_obj_view_set_cursor_to_context(DoView *view, DoContext *context);
//static 	gboolean 	do_obj_view_can_set_cursor_to_context (DoView *view, DoContext *context);
//static 	void		do_obj_view_do_find_barcode 	 (DoView *view);
//static 	gboolean	do_obj_view_can_do_find_barcode	 (DoView *view);
static 	gboolean 	do_obj_view_can_do_close_for_esc (DoView *view);
//static 	void 		do_obj_view_do_refresh (DoView *view);
//static 	gboolean 	do_obj_view_can_do_refresh (DoView *view);
//static 	DoView 	   *do_obj_view_get_active_child (DoView *view);

static gboolean do_obj_view_fill_first(DoObjView *view);
static void do_obj_view_model_fill_cb(JsonNode *node, GArray *params);
static void do_obj_view_fill(DoObjView *view, const gchar *page);
static void do_obj_view_fill_more(DoObjView *view, const gchar *page);
static void do_obj_view_model_fill(DoObjView *view, const gchar *page_id, JsonNode *node);
static void do_obj_view_make_page(JsonArray *pages, guint index_, JsonNode *element_node, gpointer data);
static void do_obj_view_fill_page(JsonArray *pages, guint index_, JsonNode *element_node, gpointer data);
static void do_obj_view_make_column(JsonArray *pages, guint index_, JsonNode *element_node, gpointer data);
static void do_obj_view_model_append_row(JsonArray *pages, guint index_, JsonNode *element_node, gpointer data);
static gboolean do_obj_view_key_press(GtkWidget *widget, GdkEventKey *event, DoObjView *view);
static void do_obj_view_switch_page(GtkNotebook *notebook,GtkWidget *page,guint page_num, DoObjView *view);

typedef struct _DoPage DoPage;

enum
{
    PROP_0,
    PROP_KEY,
};


struct _DoObjViewPrivate
{
    GtkWidget     *label;
    GtkWidget     *notebook;
    GSList        *pages;
    gchar         *key;
    guint          source_;
    //gboolean       grab_focus;
    gboolean       filling;
    gboolean       label_init;
};
struct _DoPage
{
	gboolean   fill;
	gboolean   more;
	gboolean   visible;
	gchar     *id;
	GtkWidget *view;
	GtkWidget *header;
	GtkWidget *footer;
	GtkWidget *more_widget;
};

G_DEFINE_TYPE_WITH_CODE (DoObjView, do_obj_view, GTK_TYPE_BOX,
    		 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_obj_view_view_init)
             G_ADD_PRIVATE(DoObjView)
                                    );


static void free_page(DoPage *page)
{
	g_free(page->id);
	g_free(page);
}
static void do_obj_view_view_init(DoViewIface *iface)
{

	iface->close_request = do_obj_view_close_request;
	//iface->do_delete = do_obj_view_do_delete;
	//iface->can_do_delete = do_obj_view_can_do_delete;
	//iface->do_insert = do_obj_view_do_insert;
	//iface->can_do_insert = do_obj_view_can_do_insert;
	//iface->do_copy = do_obj_view_do_copy;
	//iface->can_do_copy = do_obj_view_can_do_copy;
	iface->do_edit = do_obj_view_do_edit;
	iface->can_do_edit = do_obj_view_can_do_edit;
	//iface->do_apply = do_obj_view_do_apply;
	//iface->can_do_apply = do_obj_view_can_do_apply;
	//iface->do_unapply = do_obj_view_do_unapply;
	//iface->can_do_unapply = do_obj_view_can_do_unapply;
	//iface->do_mail_send = do_obj_view_do_mail_send;
	//iface->can_do_mail_send = do_obj_view_can_do_mail_send;
	//iface->do_popup = do_obj_view_do_popup;
	iface->do_grab_focus = do_obj_view_do_grab_focus;
	//iface->do_print = do_obj_view_do_print;
	//iface->do_print_now = do_obj_view_do_print_now;
	//iface->can_do_print = do_obj_view_can_do_print;
	//iface->do_open = do_obj_view_do_open;
	//iface->can_do_open = do_obj_view_can_do_open;
	//iface->do_save = do_obj_view_do_save;
	//iface->do_save_as = do_obj_view_do_save_as;
	//iface->can_do_save = do_obj_view_can_do_save;
	//iface->can_do_save_as = do_obj_view_can_do_save_as;
	iface->do_close = do_obj_view_do_close;
	iface->get_title = do_obj_view_get_title;
	//iface->get_icon = do_obj_view_get_icon;
	//iface->get_load_status = do_obj_view_get_load_status;
	//iface->get_load_progress = do_obj_view_get_load_progress;
	//iface->get_marked = do_obj_view_get_marked;
	//iface->set_marked = do_obj_view_set_marked;
	//iface->toggle_marked = do_obj_view_toggle_marked;
	//iface->sender_do_close = do_obj_view_sender_do_close;
	//iface->update_context = do_obj_view_update_context;
	//iface->set_cursor_to_context = do_obj_view_set_cursor_to_context;
	//iface->can_set_cursor_to_context = do_obj_view_can_set_cursor_to_context;
	//iface->do_find_barcode = do_obj_view_do_find_barcode;
	//iface->can_do_find_barcode = do_obj_view_can_do_find_barcode;
	iface->can_do_close_for_esc = do_obj_view_can_do_close_for_esc;
	//iface->do_refresh = do_obj_view_do_refresh;
	//iface->can_do_refresh = do_obj_view_can_do_refresh;
	//iface->get_active_child = do_obj_view_get_active_child;
}

static void do_obj_view_init(DoObjView *do_view)
{
    //DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (do_view);
    // to do
    //DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE(do_view);
    //memset(priv, 0, sizeof(*priv));
}
static void do_obj_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, DoView *view)
{
	if ( do_view_can_do_edit(DO_VIEW(view), NULL) )
		do_view_do_edit(DO_VIEW(view), NULL);
}
static GObject *do_obj_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
    GObject        *object;
    //DoObjView      *do_view;
    DoObjViewPrivate *priv;
    GtkWidget *notebook;
    GtkWidget *l;

    object = G_OBJECT_CLASS (do_obj_view_parent_class)->constructor(type, n_construct_properties, construct_params);

    g_object_set(object, "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 6, NULL);

    priv = DO_OBJ_VIEW_GET_PRIVATE(object);


    priv->label = l = gtk_label_new("");
#if GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(l), 0.0);
#else
    g_object_set(l,"xalign", 0.0, NULL);
#endif
    gtk_label_set_selectable(GTK_LABEL(l), TRUE);
    gtk_widget_set_can_focus (l, TRUE);

    gtk_box_pack_start(GTK_BOX(object), l, FALSE, FALSE, 0);

    priv->notebook = notebook = gtk_notebook_new();

    g_signal_connect(notebook, "switch-page", G_CALLBACK(do_obj_view_switch_page), object);
    gtk_box_pack_start(GTK_BOX(object), notebook, TRUE, TRUE, 0);

    gtk_widget_show_all(GTK_WIDGET(object));
#ifdef DEBUG
    g_print("constractor DoObjView(%s)\n", priv->key);
#endif
    priv->source_ = g_idle_add((GSourceFunc)do_obj_view_fill_first, object);
    return object;
}
static void do_obj_view_finalize (GObject *object)
{
    DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (object);
	g_slist_free_full(priv->pages, (GDestroyNotify)free_page);
    G_OBJECT_CLASS (do_obj_view_parent_class)->finalize (object);
}
static gboolean do_obj_view_do_grab_focus(DoView *view)
{
    DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
    GtkWidget *page;
    gint n;
    n = gtk_notebook_get_current_page(GTK_NOTEBOOK(priv->notebook));
    if ( n == -1 )
    	gtk_widget_grab_focus(priv->label);
    else {
    	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(priv->notebook), n);
    	if ( DO_IS_VIEW(page) )
    		return do_view_do_grab_focus(DO_VIEW(page));
    	else
    		gtk_widget_grab_focus(page);
    }
    return TRUE;
}
static void do_obj_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (object);

    switch (prop_id)
    {
    	case PROP_KEY:
    		g_value_set_string(value, priv->key);
    		break;
        // to do
    }

    g_assert_not_reached ();
}

static void do_obj_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (object);

    switch (prop_id)
    {
    	case PROP_KEY:
    		priv->key = g_value_dup_string(value);
    		//update_flags(DO_OBJ_VIEW(object));
    		break;
        // to do
    }
}

static void do_obj_view_class_init (DoObjViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

    o_class->constructor  = do_obj_view_constructor;
    o_class->finalize     = do_obj_view_finalize;
    o_class->get_property = do_obj_view_get_property;
    o_class->set_property = do_obj_view_set_property;

    //g_type_class_add_private (o_class, sizeof (DoObjViewPrivate));

    g_object_class_install_property
    	(o_class,
    	 PROP_KEY,
    	 g_param_spec_string ("key",
    			     "ключ",
    			     "ключ",
    			     "",
    			     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
    			     G_PARAM_CONSTRUCT_ONLY));
}

GtkWidget *do_obj_view_new (const gchar *key)
{
#ifdef DEBUG
    g_print("create DoObjView(%s)\n", key);
#endif
    return g_object_new (DO_TYPE_OBJ_VIEW,
    		     "key", key,

    		     (gpointer) NULL);
}
static 	const char *do_obj_view_get_title(DoView *view)
{
    return "Информация";
}
static void do_obj_view_do_close(DoView *view)
{
    DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
    if ( !priv->filling )
        gtk_widget_destroy(GTK_WIDGET(view));
}

static gboolean do_obj_view_fill_first(DoObjView *view)
{
	//DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
#ifdef DEBUG
	DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
    g_print("FillFirst DoObjView(%s)\n", priv->key);
#endif
	GtkWidget *window;
	window = gtk_widget_get_toplevel(GTK_WIDGET(view));
	if ( !window || !GTK_IS_WINDOW(window) )
		return TRUE;

	//priv->grab_focus = TRUE;
	do_obj_view_fill(view, NULL);
	return FALSE;
}

static void do_obj_view_fill(DoObjView *view, const gchar *page)
{
    DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
    GtkApplication *app = gtk_window_get_application(
    		 GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))));
    GArray *params;
    gchar *info_key;
    gint *flags;
    if ( priv->filling )
        return;
    priv->filling = TRUE;
    if ( !priv->label_init )
        gtk_label_set_markup(GTK_LABEL(priv->label), WAITING_DATA_TEXT);
    //JsonNode *node;
    params =  g_array_new (FALSE, FALSE, sizeof (gpointer));
	g_array_append_val(params, view);
	g_array_append_val(params, page);
	flags = g_malloc(sizeof(gint));
	*flags = 1;
	g_array_append_val(params, flags);
	info_key = g_strdup_printf("%s.%s",priv->key,page ? page : "");

    /*node = */do_application_request2_async(DO_APPLICATION(app), "GET", "GetInfo", info_key, 0,
    		                    (GFunc)do_obj_view_model_fill_cb, params,
								"key", priv->key,
								"page", page ? page : "",
								NULL);
    /*if ( node ) {
    	do_obj_view_model_fill(view, node);
    }*/
    g_free(info_key);
}
static DoPage *page_by_name(DoObjView *view, const gchar *page)
{
    DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
    GSList *l;
    for ( l = priv->pages; l; l = l->next )
        if ( !g_strcmp0(((DoPage*)l->data)->id, page) )
            return l->data;
    return NULL;
}
static void do_obj_view_fill_more(DoObjView *view, const gchar *page)
{
    DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
    GtkApplication *app = gtk_window_get_application(
    		 GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))));
    DoPage *p;
    GArray *params;
    gchar *info_key;
    const gchar *id = NULL;

    if ( priv->filling )
        return;
    priv->filling = TRUE;

    p = page_by_name(view, page);
    if ( !p )
        return;

    GtkTreeView *tree_view;
    GtkTreeModel *model;
    GtkTreeIter iter = {0,};
    gint n;
    gint *flags;
    g_object_get(p->view, "tree-view", &tree_view, NULL);
    model = gtk_tree_view_get_model(tree_view);
    n = gtk_tree_model_iter_n_children(model, NULL);
    if ( n && gtk_tree_model_iter_nth_child (model, &iter, NULL, n - 1) ) {
        GValue value = {0,};
        gtk_tree_model_get_value(model, &iter, 0, &value);
        id = g_value_get_string(&value);
    }
    if ( !id )
        return;
    //JsonNode *node;
    params =  g_array_new (FALSE, FALSE, sizeof (gpointer));
    flags = g_malloc(sizeof(gint));
    *flags = 2;
	g_array_append_val(params, view);
	g_array_append_val(params, page);
	g_array_append_val(params, flags);
	info_key = g_strdup_printf("%s.%s.%s",priv->key, page, id);

    /*node = */do_application_request2_async(DO_APPLICATION(app), "GET", "GetInfo", info_key, 0,
    		                    (GFunc)do_obj_view_model_fill_cb, params,
								"key", priv->key,
								"page", page,
								"last", id,
								NULL);
    /*if ( node ) {
    	do_obj_view_model_fill(view, node);
    }*/
    g_free(info_key);
}
static void do_obj_view_model_fill_cb(JsonNode *node, GArray *params)
{
	DoObjView *view;
	const gchar *page_id;
	gint *flags;
	view = g_array_index(params, gpointer, 0);
	page_id = g_array_index(params, gpointer, 1);
	flags = g_array_index(params, gpointer, 2);
	do_obj_view_model_fill(view, page_id, node);
    if ( *flags & 1 )
    	do_obj_view_do_grab_focus(DO_VIEW(view));
    if ( *flags & 2 ) {
        DoPage *p;
        p = page_by_name(view, page_id);
        if ( p ) {
            GtkTreePath *path;
            GtkTreeView *tree_view;
            g_object_get(p->view, "tree-view", &tree_view, NULL);
            gtk_tree_view_get_cursor(tree_view, &path, NULL);
            gtk_tree_path_next(path);
            gtk_tree_view_set_cursor(tree_view, path, NULL, FALSE);
        }
    }
    g_free(flags);
    g_array_free(params, FALSE);
}
static void do_obj_view_model_fill(DoObjView *view, const gchar *page_id, JsonNode *node)
{
	DoObjViewPrivate *priv;
	JsonObject *obj;
	gchar *markup = NULL;
	priv = DO_OBJ_VIEW_GET_PRIVATE (view);
#ifdef DEBUG
    gchar *data;
    JsonGenerator *generator = NULL;
    generator = json_generator_new();
    json_generator_set_root(generator, node);
    data = json_generator_to_data(generator, NULL);
    g_print("Object json \"%s\"\n%s\n", priv->key, data);
    g_object_unref(generator);
#endif // DEBUG
	if ( node ) {
        obj = json_node_get_object(node);
        g_assert(obj != NULL);
        if ( !page_id ) {
            if ( json_object_has_member(obj, "markup") )
                markup = markup_plus(json_object_get_string_member(obj, "markup"));
            else if ( json_object_has_member(obj, "text") )
                markup = g_markup_escape_text(json_object_get_string_member(obj, "text"), -1);
            if ( markup ) {
                gtk_label_set_markup(GTK_LABEL(priv->label), markup);
                g_free(markup);
                gtk_widget_set_visible(GTK_WIDGET(priv->label), TRUE);
            }
            else
                gtk_widget_set_visible(GTK_WIDGET(priv->label), FALSE);
            priv->label_init = TRUE;
        }
        if ( json_object_has_member(obj, "pages") ) {
            if ( !priv->pages )
                json_array_foreach_element(json_object_get_array_member(obj, "pages"), do_obj_view_make_page, view);
            json_array_foreach_element(json_object_get_array_member(obj, "pages"), do_obj_view_fill_page, view);
        }
    }
    else {
        if ( !priv->label_init )
            gtk_label_set_markup(GTK_LABEL(priv->label), EMPTY_DATA_TEXT);
    }
	//gtk_widget_show_all(GTK_WIDGET(priv->notebook));
    priv->filling = FALSE;
}

static gboolean do_obj_view_close_request(DoView *view)
{
    DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
    if ( priv->filling )
        return FALSE;
    else
        return TRUE;
}
static void do_obj_view_do_edit(DoView *view, const gchar *tab)
{
	DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
    GtkWidget *page;
    gint n;
    n = gtk_notebook_get_current_page(GTK_NOTEBOOK(priv->notebook));
    if ( n == -1 ) {
        return;
    }
   	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(priv->notebook), n);
    GtkTreeView *tree_view;
    GtkTreePath *path;
    GtkTreeIter iter;
    GtkTreeModel *model;
    g_object_get(page, "tree-view", &tree_view, NULL);
    gtk_tree_view_get_cursor(tree_view, &path, NULL);
    model = gtk_tree_view_get_model(tree_view);
    if ( gtk_tree_model_get_iter(model, &iter, path) ) {
        GValue value = {0,};
        const gchar *key;
        gtk_tree_model_get_value(model, &iter, 1, &value);
        key = g_value_get_string(&value);
        if ( key && key[0] != '\0' ) {
            GVariant *parameter;
            parameter = g_variant_new_string(key);
            do_common_action_activate("ObjView", parameter);
            //to do g_variant_unref(parameter);
            return;
        }
    }
}
static 	gboolean do_obj_view_can_do_edit(DoView *view, const gchar *tab)
{
	return TRUE;
}
static 	gboolean do_obj_view_can_do_close_for_esc (DoView *view)
{
	return TRUE; // to do
}

static void cache_update_columns_info(JsonArray *pages, guint index_, JsonNode *element_node, gpointer data)
{
	//DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (data);
    GtkApplication *app = gtk_window_get_application(
    		 GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(data))));
	gchar *key;
	const gchar *page_id;
	JsonObject *page;

	page = json_node_get_object(element_node);
	page_id = json_object_get_string_member(page, "id");
	key = g_strdup_printf("PAGE.%s.COLUMNS", page_id);
	if ( json_object_has_member(page, "columns") ) {
		do_application_set_cache(DO_APPLICATION(app), key, json_object_get_member(page, "columns"));
	}
	g_free(key);
}
static void more_button_clicked(GtkWidget *widget, gpointer data)
{
	DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (data);
	GtkWidget *view;
	gint n;
	gchar *id;
	n = gtk_notebook_get_current_page(GTK_NOTEBOOK(priv->notebook));
	if ( n == -1 )
        return;
	view = gtk_notebook_get_nth_page(GTK_NOTEBOOK(priv->notebook), n);
	id = g_object_get_data(G_OBJECT(view), "id");
	do_obj_view_fill_more(DO_OBJ_VIEW(data), id);
}
static void do_obj_view_make_page(JsonArray *pages, guint index_, JsonNode *element_node, gpointer data)
{
	DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (data);
    GtkApplication *app = gtk_window_get_application(
    		 GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(data))));
	gchar *key;
	const gchar *page_id;
	JsonObject *page, *obj;
	JsonArray *columns;
	JsonNode *node;
	GtkWidget *view;
	GtkWidget *tree_view;
	GtkWidget *l;
	GtkListStore *model;
	guint n_columns, i;
	GType *types;
	gchar *name;
	DoPage *p;
	//GtkTreeSelection *selection;


	page = json_node_get_object(element_node);
	page_id = json_object_get_string_member(page, "id");
	key = g_strdup_printf("PAGE.%s.COLUMNS", page_id);
	if ( json_object_has_member(page, "columns") ) {
		do_application_set_cache(DO_APPLICATION(app), key, json_object_get_member(page, "columns"));
	}
	p = g_new0(DoPage, 1);
	p->id = g_strdup(json_object_get_string_member(page, "id"));
	priv->pages = g_slist_append(priv->pages, p);

	if ( json_object_has_member(page, "visible") &&
	     !json_object_get_boolean_member(page, "visible") )
		return;

	node = do_application_get_cache(DO_APPLICATION(app), key);
	if ( !node ) {
        node = do_application_request2(DO_APPLICATION(app), "GET", "GetInfo", priv->key, DO_CLIENT_FLAGS_NOCACHE,
								"key", priv->key,
								"page", page_id,
								NULL);

        obj = json_node_get_object(node);
        g_assert(obj != NULL);
        g_assert(json_object_has_member(obj, "pages"));
        json_array_foreach_element(json_object_get_array_member(obj, "pages"), cache_update_columns_info, data);
        node = do_application_get_cache(DO_APPLICATION(app), key);
        g_assert(node == NULL);
    }

	p->visible = TRUE;

	columns = json_node_get_array(node);
	n_columns = json_array_get_length(columns);

	name = g_strdup_printf("Page-%s", json_object_get_string_member(page, "id"));
	view = do_tree_view_new(name);
	p->header = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(p->header), WAITING_DATA_TEXT);
#if GTK_CHECK_VERSION(3,16,0)
	gtk_label_set_xalign(GTK_LABEL(p->header), 0.0);
#else
	g_object_set(p->header,"xalign", 0.0, NULL);
#endif

	p->footer = gtk_label_new("");
	p->more_widget = gtk_button_new();
	gtk_widget_child_focus(p->more_widget, FALSE);
	gtk_button_set_relief(GTK_BUTTON(p->more_widget), GTK_RELIEF_NONE);
	gtk_button_set_label(GTK_BUTTON(p->more_widget), TREE_VIEW_MORE_TEXT);
	g_signal_connect(p->more_widget, "clicked", G_CALLBACK(more_button_clicked), data);
#if GTK_CHECK_VERSION(3,16,0)
	gtk_label_set_xalign(GTK_LABEL(p->footer), 0.0);
#else
	g_object_set(p->footer,"xalign", 0.0, NULL);
#endif
	do_tree_view_pack_header(DO_TREE_VIEW(view), p->header);
	do_tree_view_pack_footer(DO_TREE_VIEW(view), p->more_widget);
	do_tree_view_pack_footer(DO_TREE_VIEW(view), p->footer);
	p->view = view;
	g_object_set_data(G_OBJECT(view), "id", p->id);
	g_object_get(view, "tree-view", &tree_view, NULL);
	//selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	types = g_new0(GType, n_columns + 2);
	for ( i = 0; i < n_columns + 2; i++ )
		types[i] = G_TYPE_STRING;
	model = gtk_list_store_newv(n_columns + 2, types);
	g_free(types);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(model));
	g_signal_connect(tree_view, "row-activated",  G_CALLBACK(do_obj_view_row_activated), data);
	g_signal_connect(tree_view, "key-press-event",  G_CALLBACK(do_obj_view_key_press), data);
	l = gtk_label_new("");
	if ( json_object_has_member(page, "markup") )
		gtk_label_set_markup(GTK_LABEL(l), json_object_get_string_member(page, "markup"));
	else if ( json_object_has_member(page, "text") )
			gtk_label_set_text(GTK_LABEL(l), json_object_get_string_member(page, "text"));

	gtk_notebook_append_page(GTK_NOTEBOOK(priv->notebook), view, l);
	g_free(name);
	json_array_foreach_element(columns, do_obj_view_make_column, view);
#ifdef DEBUG
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
	renderer = gtk_cell_renderer_text_new();
	column = do_tree_view_add_column(DO_TREE_VIEW(view), "id", "id", -1);
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	gtk_tree_view_column_add_attribute(column, renderer, "text", 0);
	renderer = gtk_cell_renderer_text_new();
	column = do_tree_view_add_column(DO_TREE_VIEW(view), "key", "key", -1);
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	gtk_tree_view_column_add_attribute(column, renderer, "text", 1);
#endif
	gtk_widget_show_all(GTK_WIDGET(view));
}
static void do_obj_view_fill_page(JsonArray *pages, guint index_, JsonNode *element_node, gpointer data)
{
	DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (data);
	JsonObject *page;
	GtkWidget *view;
	GtkWidget *tree_view;
	GtkTreeModel *model;
	GSList *l;
	DoPage *p = NULL;
	const gchar *page_id;

	page = json_node_get_object(element_node);
	page_id = json_object_get_string_member(page, "id");

    if ( !json_object_has_member(page, "rows") ) {
        return;
    }

	for ( l = priv->pages; l; l = l->next ) {
        if ( !g_strcmp0(((DoPage*)l->data)->id, page_id) ) {
            p = l->data;
            break;
        }
    }
    if ( !p || !p->visible )
		return;

    view = p->view;
    g_object_get(view, "tree-view", &tree_view, NULL);
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
    if ( json_object_has_member(page, "rows") )
        json_array_foreach_element(json_object_get_array_member(page, "rows"), do_obj_view_model_append_row, model);
    if ( json_object_has_member(page, "header") )
        gtk_label_set_markup(GTK_LABEL(p->header), json_object_get_string_member(page, "header"));
    else
        gtk_label_set_text(GTK_LABEL(p->header), "");
    if ( json_object_has_member(page, "footer") )
        gtk_label_set_markup(GTK_LABEL(p->footer), json_object_get_string_member(page, "footer"));
    else
        gtk_label_set_text(GTK_LABEL(p->footer), "");
    p->fill = TRUE;
    p->more = json_object_has_member(page, "more") && json_object_get_int_member(page, "more") == 1;

    gtk_widget_set_visible(p->header, strlen(gtk_label_get_label(GTK_LABEL(p->header))) > 0);
    gtk_widget_set_visible(p->footer, strlen(gtk_label_get_label(GTK_LABEL(p->footer))) > 0);
    gtk_widget_set_visible(p->more_widget, p->more);
}

static void do_obj_view_make_column(JsonArray *pages, guint index_, JsonNode *element_node, gpointer data)
{
	DoTreeView *view = DO_TREE_VIEW(data);
	//DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	JsonObject *obj;
	gchar *name;
	const gchar* title = "";

	obj = json_node_get_object(element_node);
	renderer = gtk_cell_renderer_text_new();
	name = g_strdup_printf("column%d", index_);
	if ( json_object_has_member(obj, "title") )
		title = json_object_get_string_member(obj, "title");
	column = do_tree_view_add_column(view, name, title, -1);
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	gtk_tree_view_column_add_attribute(column, renderer, "markup", index_ + 2);
	if ( json_object_has_member(obj, "align") ) {
		if ( !g_strcmp0(json_object_get_string_member(obj, "align"), "center") )
			g_object_set(renderer, "xalign", 0.5, NULL);
		else if ( !g_strcmp0(json_object_get_string_member(obj, "align"), "right") )
			g_object_set(renderer, "xalign", 1., NULL);
	}
}
static gboolean do_obj_view_key_press(GtkWidget *widget, GdkEventKey *event, DoObjView *view)
{
    guint mask = gtk_accelerator_get_default_mod_mask ();
    gint page_num;
	DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
	DoPage *p;
    if ((event->state & mask) == 0)
    {
        switch (event->keyval)
        {
            case GDK_KEY_Right:
            case GDK_KEY_Left:
                if ( gtk_notebook_get_n_pages(GTK_NOTEBOOK(priv->notebook)) < 2 )
                    break;
                page_num = gtk_notebook_get_current_page(GTK_NOTEBOOK(priv->notebook));
                if ( event->keyval == GDK_KEY_Left )
                    page_num = ( page_num > 0 ? page_num : gtk_notebook_get_n_pages(GTK_NOTEBOOK(priv->notebook)) ) - 1;
                else
                    page_num = ( page_num >= gtk_notebook_get_n_pages(GTK_NOTEBOOK(priv->notebook)) - 1 ? -1 : page_num ) + 1;
                gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), page_num);
                return TRUE;
            case GDK_KEY_Down:
            case GDK_KEY_Page_Down:
                page_num = gtk_notebook_get_current_page(GTK_NOTEBOOK(priv->notebook));
                widget = gtk_notebook_get_nth_page(GTK_NOTEBOOK(priv->notebook), page_num);
                p = page_by_name(view, g_object_get_data(G_OBJECT(widget), "id"));
                if ( p && p->more ) {
                    GtkTreeView *tree_view;
                    GtkTreeModel *model;
                    GtkTreePath *path;
                    GtkTreeIter iter;
                    g_object_get(widget, "tree-view", &tree_view, NULL);
                    gtk_tree_view_get_cursor(tree_view, &path, NULL);
                    model = gtk_tree_view_get_model(tree_view);
                    if ( gtk_tree_model_get_iter(model, &iter, path) &&
                        !gtk_tree_model_iter_next(model, &iter) ) {
                        do_obj_view_fill_more(view, p->id);
                        return TRUE;
                    }
                }
                break;
            default:
                break;
        };
    }
    else {
        if ( (event->state & mask) == GDK_CONTROL_MASK ) {
            if ( event->keyval == GDK_KEY_Left ||
                 event->keyval == GDK_KEY_Right ) {
                GtkWidget *scroll;
                scroll = gtk_widget_get_parent(GTK_WIDGET(widget));
                if ( GTK_IS_SCROLLED_WINDOW(scroll) ) {
                    g_signal_emit_by_name(scroll, "scroll-child", event->keyval == GDK_KEY_Right ? GTK_SCROLL_STEP_RIGHT : GTK_SCROLL_STEP_LEFT, TRUE);
                }
            }
        }
    }
	return FALSE;
}
static void do_obj_view_model_append_row(JsonArray *pages, guint index_, JsonNode *element_node, gpointer data)
{
	GtkListStore *model = data;
	const gchar *id = NULL, *key = NULL;
	JsonObject *obj;
	JsonArray *values = NULL;
	GtkTreeIter iter;
	GValue value = {0,};
	gint i, length;

	obj = json_node_get_object(element_node);

	if ( json_object_has_member(obj, "id") )
		id = json_object_get_string_member(obj, "id");
	if ( json_object_has_member(obj, "key") )
		key = json_object_get_string_member(obj, "key");
	if ( json_object_has_member(obj, "values") )
		values = json_object_get_array_member(obj, "values");

	g_value_init(&value, G_TYPE_STRING);
	gtk_list_store_append(model, &iter);
	g_value_set_string(&value, id);
	gtk_list_store_set_value(model, &iter, 0, &value);
	g_value_set_string(&value, key);
	gtk_list_store_set_value(model, &iter, 1, &value);
	if ( values ) {
		length = json_array_get_length(values);
		for ( i = 0; i < length; i++ ) {
			g_value_set_string(&value, json_array_get_string_element(values, i));
			gtk_list_store_set_value(model, &iter, i+2, &value);
		}
	};
}
static void do_obj_view_switch_page(GtkNotebook *notebook,GtkWidget *page,guint page_num, DoObjView *view)
{
	DoObjViewPrivate *priv = DO_OBJ_VIEW_GET_PRIVATE (view);
	DoPage *p;
	p = g_slist_nth_data(priv->pages, page_num);
	if ( !p->fill )
		do_obj_view_fill(view, p->id);
}
