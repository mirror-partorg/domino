
#include "do_keyboard_view.h"
#include "do_html_view.h"
#include "do_window.h"
#include "do_notebook.h"
#include "do_keyboard_model.h"
#include "do_keyboard_dialog.h"
#include "do_icons.h"
#include "do_view.h"
#include "do_context.h"
//#include "do_upload.h"
#include "do_view_actions.h"
#include "do_common_actions.h"
#include <dolib.h>
#include <domino.h>
#include "domino.h"
#ifdef ARTIX
#include <artix.h>
#endif
#include <../misc/cash_key_command.h>
#include "do_utilx.h"
#include "do_dnd.h"
#include "pango/pango-layout.h"
#include "do_keyboard_const.h"
#include <gio/gio.h>
#include <webkit/webkit.h>
#include "do_marshal.h"
#include <gdk/gdk.h>

//#include "do_icons.h"

#define DO_KEYBOARD_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_KEYBOARD_VIEW, DoKeyboardViewPrivate))
#define DO_TYPE_KEYBOARD_VIEW_FLAGS do_keyboard_view_flags_get_type()

#define DEFAULT_TEMPLATE_NAME "default.html"

// Signals
enum
{
    GO_SENDER,
    PRINT_TEMPLATE,
    LAST_SIGNAL
};
// Property
enum
{
	PROP_0,
	PROP_SENDER_OBJECT,
	PROP_TEMPLATE_OBJECT,
	PROP_SELECTED_CODE,
	PROP_FLAGS,
	PROP_ALIAS,
};

static void cell_key(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data);
static void cell_desc(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data);

// Drag and Drop
static void drag_begin(GtkWidget *widget, GdkDragContext *drag_context, DoKeyboardView *view);
static void drag_data_get(GtkWidget *widget, GdkDragContext *drag_context, GtkSelectionData *data, guint info, guint time, DoKeyboardView *view);
static void drag_data_received(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data);
//!!static GdkDrawable *view_create_drag_pixmap (GtkWidget *widget, DoKeyboardView *view);

static void selection_changed(GtkTreeSelection *selection, gpointer data);
//(GtkTreeSelection *selection, GtkTreeModel *model, GtkTreePath *path, gboolean path_currently_selected, gpointer data);

static void row_changed(GtkTreeModel *tree_model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data);
static void row_deleted(GtkTreeModel *tree_model, GtkTreePath *path, gpointer user_data);
static void row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoKeyboardView *view);

static void focus_in(GtkWidget *widget, GdkEventFocus *event, gpointer user_data);

static void update_selected_code(DoKeyboardView *view, gint selected_code, gboolean force);

//static gchar *get_desc_from_cursor(DoKeyboardView *view);
static gchar *get_desc_from_path(DoKeyboardView *view, GtkTreePath *path);
static gchar *get_code_from_path(DoKeyboardView *view, GtkTreePath *path);
//static gchar *get_code_from_cursor(DoKeyboardView *view);
static gint get_key_from_path(DoKeyboardView *view, GtkTreePath *path);
static gint get_key_from_cursor(DoKeyboardView *view);
static gboolean get_crnt_key(DoKeyboardView *view, GtkTreeIter *iter);

static void set_cursor_to_context(DoKeyboardView *view, DoContext *context);
static gboolean html_template_create(DoKeyboardView *view);

static void sender_load_status (DoView *view, GParamSpec *pspec, GtkWidget *proxy);

static void update_selected_code(DoKeyboardView *view, gint selected_code, gboolean force);

static void send_received_info(DoKeyboardView *view);

static WebKitNavigationResponse navi_response(WebKitWebView        *web_view,
                                              WebKitWebFrame       *frame,
                                              WebKitNetworkRequest *request,
                                              DoKeyboardView       *view);
// View interface
static void do_keyboard_view_view_init(DoViewIface *iface);

static void	    view_do_delete(DoView *view);
static gboolean	view_can_do_delete(DoView *view);
static void	    view_do_insert(DoView *view);
static gboolean	view_can_do_insert(DoView *view);
static void		view_do_copy(DoView *view);
static gboolean	view_can_do_copy(DoView *view);
static void		view_do_edit(DoView *view, const gchar *tab);
static gboolean	view_can_do_edit(DoView *view, const gchar *tab);
static void		view_do_apply(DoView *view);
static gboolean	view_can_do_apply(DoView *view);
//static void		view_do_mail_send(DoView *view);
//static gboolean	view_can_do_mail_send(DoView *view);
static void		view_do_unapply(DoView *view);
static gboolean	view_can_do_unapply(DoView *view);
static void		view_do_popup(DoView *view, GdkEventButton* event);
static gboolean	view_do_grab_focus(DoView *view);
static void		view_do_print(DoView *view);
static gboolean	view_can_do_print(DoView *view);
static void	    view_do_close(DoView *view);
static const char *view_get_title(DoView *view);
static GdkPixbuf  *view_get_icon(DoView *view);
static gboolean	   view_get_load_status(DoView *view);
static void        view_update_context(DoView *view, DoContext *context);
static gboolean    view_set_marked(DoView *view, DoContext *context, DoMarked marked);

static guint keyboard_view_signals [LAST_SIGNAL] = { 0 };

GType do_keyboard_view_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_keyboard_view_flags_values[] = {
         { DO_KEYBOARD_VIEW_FLAGS_PRINT_TEMPLATE, "DO_KEYBOARD_VIEW_FLAGS_PRINT_TEMPLATE", "print-template" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoKeyboardViewFlags", _do_keyboard_view_flags_values);
    }
    return type;
}
struct _DoKeyboardViewPrivate
{
	DoKeyboardViewFlags  flags;
	GtkTreeModel        *model;
	GtkTreeView         *tree_view;
	gboolean             dragging;
    GtkTreeSelection    *sel;
    DoView              *sender;
    GtkWidget           *template_object;
    GList               *html_template;
    gboolean             template_false;
    gchar               *label_width;
    gchar               *label_height;
    gchar               *alias_name;
    gint                 selected_code;
};

//G_DEFINE_TYPE (DoKeyboardView, do_keyboard_view, GTK_TYPE_VBOX)
G_DEFINE_TYPE_WITH_CODE (DoKeyboardView, do_keyboard_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_keyboard_view_view_init)
                        );


static void do_keyboard_view_view_init(DoViewIface *iface)
{
    iface->do_delete     = view_do_delete;
    iface->can_do_delete = view_can_do_delete;
    iface->do_insert     = view_do_insert;
    iface->can_do_insert = view_can_do_insert;
    iface->do_copy       = view_do_copy;
    iface->can_do_copy   = view_can_do_copy;
    iface->do_edit       = view_do_edit;
    iface->can_do_edit   = view_can_do_edit;
    iface->do_apply      = view_do_apply;
    iface->can_do_apply  = view_can_do_apply;
    iface->do_unapply     = view_do_unapply;
    iface->can_do_unapply = view_can_do_unapply;
    //iface->do_mail_send   = view_do_mail_send;
    //iface->can_do_mail_send = view_can_do_mail_send;
    iface->do_popup      = view_do_popup;
    iface->do_grab_focus = view_do_grab_focus;
    iface->do_print      = view_do_print;
    iface->do_print_now  = view_do_print;
    iface->can_do_print  = view_can_do_print;
    iface->do_close      = view_do_close;
    iface->get_title     = view_get_title;
    iface->get_icon      = view_get_icon;
    iface->get_load_status = view_get_load_status;
    iface->update_context = view_update_context;
    iface->set_marked     = view_set_marked;
}


static void do_keyboard_view_init(DoKeyboardView *temp)
{
	DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (temp);

	memset(priv, 0, sizeof(DoKeyboardViewPrivate));

}



static const GtkTargetEntry drag_types [] =
{
    { "GTK_TREE_MODEL_ROW", GTK_TARGET_SAME_WIDGET, 0},
    { DO_DND_PRODUCT_TYPE, GTK_TARGET_SAME_APP, 0},
    { DO_DND_TEXT_TYPE, 0, 0},
};
#define DO_KEYBOARD_VIEW_POPUP "keyboard-view-popup"

static gchar *popup_ui =
    "  <popup name='" DO_KEYBOARD_VIEW_POPUP "'>\n"
    "    <menuitem action='InsertAction'/>\n"
    "    <menuitem action='EditAction'/>\n"
    "    <menuitem action='CopyAction'/>\n"
    "    <menuitem action='DeleteAction'/>\n"
    "        <separator/>\n"
    "    <menuitem action='MailSendAction'/>\n"
    "  </popup>\n";

static GObject *do_keyboard_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
    GtkTreeViewColumn   *col;
    GtkTreeModel        *model;
    GtkCellRenderer     *renderer;
    GtkTreeSelection    *sel;
    GList               *sort_list = NULL, *l;
    DoTreeView          *tree_view;
    GtkTreeView         *view;

	DoKeyboardViewPrivate *priv;

	do_view_actions_add_ui_from_string(popup_ui, NULL);

	object = G_OBJECT_CLASS (do_keyboard_view_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv = DO_KEYBOARD_VIEW_GET_PRIVATE (object);
	tree_view = DO_TREE_VIEW(object);

    html_template_create(DO_KEYBOARD_VIEW(object));
    for (l = priv->html_template; l ; l = l->next) {
        l = l->next;
        gint *key;
        if (l) {
            key = l->data;
            sort_list = g_list_append(sort_list, key);
        }
        else
            break;
    }
	priv->model = model = do_keyboard_model_new(priv->alias_name, sort_list);
	g_list_free(sort_list);

    g_object_set(object, "model", priv->model, NULL);
    g_object_get(object, "tree-view", &view, NULL);

    priv->tree_view = view;

    priv->sel = sel = gtk_tree_view_get_selection( GTK_TREE_VIEW( view ) );
    gtk_tree_selection_set_mode( GTK_TREE_SELECTION( sel ),
                                 GTK_SELECTION_MULTIPLE );

/*
    gtk_tree_view_enable_model_drag_source(view, GDK_BUTTON1_MASK, drag_types, G_N_ELEMENTS (drag_types),
                         GDK_ACTION_MOVE);
    gtk_tree_view_enable_model_drag_dest(view, drag_types, G_N_ELEMENTS (drag_types),
                          GDK_ACTION_MOVE);
*/
    gtk_drag_source_set(GTK_WIDGET(view), GDK_BUTTON1_MASK, drag_types, G_N_ELEMENTS(drag_types), GDK_ACTION_MOVE | GDK_ACTION_COPY | GDK_ACTION_LINK);
    gtk_tree_view_enable_model_drag_dest(GTK_TREE_VIEW(view), drag_types, G_N_ELEMENTS(drag_types), GDK_ACTION_MOVE);

    g_signal_connect(view, "drag-begin", G_CALLBACK (drag_begin), object);
    g_signal_connect(view, "drag-data-get", G_CALLBACK (drag_data_get), object);
	g_signal_connect(view, "drag-data-received", G_CALLBACK (drag_data_received), object);
    //gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view), TRUE);

    g_signal_connect(view, "row-activated", G_CALLBACK(row_activated), object);

    g_signal_connect(view,  "focus-in-event",  G_CALLBACK(focus_in), object);

    g_signal_connect(sel,   "changed", G_CALLBACK(selection_changed), object);

    g_signal_connect(model, "row-changed", G_CALLBACK(row_changed), object);
    g_signal_connect(model, "row-deleted", G_CALLBACK(row_deleted), object);

    renderer = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(tree_view, "Desc", "Описание", -1);
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", DO_KEYBOARD_MODEL_COL_DESC);
    gtk_tree_view_column_set_cell_data_func(col, renderer, cell_desc, view, NULL);

    renderer = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(tree_view, "Key", "Клавища", -1);
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", DO_KEYBOARD_MODEL_COL_NUMBER);
    gtk_tree_view_column_set_cell_data_func(col, renderer, cell_key, view, NULL);

	gtk_widget_show(GTK_WIDGET(object));

    //gtk_widget_grab_focus(view);

//	gtk_widget_show_all (temp);

	return object;
}

static void do_keyboard_view_finalize (GObject *object)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE(object);
    if (priv->html_template) {
        g_list_foreach(priv->html_template, (GFunc)g_free, NULL);
        g_list_free(priv->html_template);
    }
	if (priv->alias_name)
		g_free(priv->alias_name);
	G_OBJECT_CLASS (do_keyboard_view_parent_class)->finalize (object);
}

static void do_keyboard_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	/* no readable properties */
	DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_FLAGS:
			g_value_set_flags (value, priv->flags);
			break;
		case PROP_ALIAS:
            g_value_set_string(value, priv->alias_name);
			break;
		case PROP_SELECTED_CODE:
            g_value_set_int(value, priv->selected_code);
			break;
		case PROP_TEMPLATE_OBJECT:
            g_value_set_pointer(value, priv->template_object);
			break;
        case PROP_SENDER_OBJECT:
            g_value_set_pointer(value, priv->sender);
            break;
	}
	g_assert_not_reached ();
}

static void do_keyboard_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_FLAGS:
			priv->flags = g_value_get_flags (value);
			break;
		case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string(value);
			break;
        case PROP_SENDER_OBJECT:
            priv->sender = g_value_get_pointer(value);
           	g_signal_connect_object (priv->sender, "notify::load-status",
				 G_CALLBACK (sender_load_status), object, 0);
            break;
        case PROP_TEMPLATE_OBJECT:
            priv->template_object = g_value_get_pointer(value);
            g_signal_connect(WEBKIT_WEB_VIEW(priv->template_object), "navigation-requested",
                              G_CALLBACK(navi_response), object);
            //update_selected_code(DO_KEYBOARD_VIEW(object), -1, TRUE);
            break;
        case PROP_SELECTED_CODE:
            update_selected_code(DO_KEYBOARD_VIEW(object), g_value_get_int(value), FALSE);
            break;
	}
}

static void do_keyboard_view_class_init (DoKeyboardViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_keyboard_view_constructor;
	object_class->finalize     = do_keyboard_view_finalize;
	object_class->get_property = do_keyboard_view_get_property;
	object_class->set_property = do_keyboard_view_set_property;

	g_type_class_add_private (object_class, sizeof (DoKeyboardViewPrivate));

    g_object_class_install_property
        (object_class,
         PROP_SENDER_OBJECT,
         g_param_spec_pointer("sender-object",
                      "sender-object",
                      "sender-object",
                       G_PARAM_READWRITE));
    g_object_class_install_property
        (object_class,
         PROP_TEMPLATE_OBJECT,
         g_param_spec_pointer("template-object",
                      "template-object",
                      "template-object",
                       G_PARAM_READWRITE));
    g_object_class_install_property
        (object_class,
         PROP_SELECTED_CODE,
         g_param_spec_int("selected-code",
                      "selected-code",
                      "selected-code",
                      0, 500, 0,
                       G_PARAM_READWRITE));
    g_object_class_install_property
        (object_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_KEYBOARD_VIEW_FLAGS,
				     DO_KEYBOARD_VIEW_FLAGS_DEFAULT,
				     G_PARAM_WRITABLE));

    keyboard_view_signals[GO_SENDER] =
        g_signal_new ("go-sender",
            G_TYPE_FROM_CLASS (object_class),
            G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET (DoKeyboardViewClass, go_sender),
            NULL, NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE, 0);

    keyboard_view_signals[PRINT_TEMPLATE] =
        g_signal_new ("print-template",
            G_TYPE_FROM_CLASS (object_class),
            G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET (DoKeyboardViewClass, print_template),
            NULL, NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE, 0);

}

GtkWidget *do_keyboard_view_new (const gchar *alias_name)
{
	return g_object_new (DO_TYPE_KEYBOARD_VIEW,
	                     "alias-name", alias_name,
			     (gpointer) NULL);
}

void cell_key(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{

    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, DO_KEYBOARD_MODEL_COL_KIND, &value);
    gint kind = g_value_get_int(&value);
    if (kind != DO_KEYBOARD_MODEL_KIND_KEY)
        g_object_set(cell, "text", "", NULL);
    else {
        GValue value = { 0, };
        gtk_tree_model_get_value(tree_model, iter, DO_KEYBOARD_MODEL_COL_NUMBER, &value);
        char *text = cash_key_code_to_string(g_value_get_int(&value));
        char buf[100];
        sprintf(buf, "<b>%s</b>", text);
        g_object_set(cell, "markup", buf, NULL);
        do_free(text);
    }
}
void cell_desc(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{

    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, DO_KEYBOARD_MODEL_COL_KIND, &value);
    gint kind = g_value_get_int(&value);
    if (kind == DO_KEYBOARD_MODEL_KIND_CONTENT) {
        GValue value = { 0, };
        gtk_tree_model_get_value(tree_model, iter, DO_KEYBOARD_MODEL_COL_DESC, &value);
        if (g_value_get_string(&value)) {
        char *text = do_product_name_format(g_value_get_string(&value));
        g_object_set(cell, "text", text, NULL);
        do_free(text);
        }
    }
    else {
        GValue value = { 0, };
        gtk_tree_model_get_value(tree_model, iter, DO_KEYBOARD_MODEL_COL_CHILDLESS, &value);
        if (g_value_get_boolean(&value)) {
            GValue value = { 0, };
            gtk_tree_model_get_value(tree_model, iter, DO_KEYBOARD_MODEL_COL_DESC, &value);
            const gchar *text = g_value_get_string(&value);
            if (text) {
            gchar *markup;
            markup = g_malloc(strlen(text) + 10);
            sprintf(markup, "<b>%s</b>", text);
            g_object_set(cell, "markup", markup, NULL);
            g_free(markup);
            }
        }
    }
}

// Drag and Drop
static void drag_begin(GtkWidget *widget, GdkDragContext *drag_context, DoKeyboardView *view)
{

    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (path) {
        //gint              *indices, depth;
        //indices = gtk_tree_path_get_indices(path);
        //depth   = gtk_tree_path_get_depth(path);
        g_object_set_data_full (G_OBJECT (drag_context),
                                          "gtk-tree-view-source-row",
                                          gtk_tree_row_reference_new(priv->model, path),
                                          (GDestroyNotify) (gtk_tree_row_reference_free));

        gtk_tree_path_free(path);
        /*!!GdkPixmap *pixmap;

        pixmap = view_create_drag_pixmap (GTK_WIDGET(priv->tree_view), view);
        if (pixmap != NULL)
        {
            gtk_drag_set_icon_pixmap (drag_context,
                          gdk_drawable_get_colormap (pixmap),
                          pixmap, NULL, -2, -2);
            g_object_unref (pixmap);
        }*/
    }
}
static void drag_data_get(GtkWidget *widget, GdkDragContext *drag_context, GtkSelectionData *data, guint info, guint time, DoKeyboardView *view)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);

    GtkTreeRowReference *ref =
    g_object_get_data(G_OBJECT (drag_context), "gtk-tree-view-source-row");

    if (ref) {
        GtkTreePath *path;
        path = gtk_tree_row_reference_get_path(ref);
        if (path) {
            GtkTreeIter iter;
            if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {
                GValue value = { 0, };
                gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_KEYBOARD_MODEL_COL_CODE, &value);
                gchar *text = (char*)g_value_get_string(&value);
                if (text)
                gtk_selection_data_set_text(data,
                                  text,
                                  sizeof (text));
            }
        }

    }
}
static void drag_data_received(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (user_data);
    GtkTreePath *path = NULL;

    GdkAtom target = GDK_NONE, product_target;
    GtkTreeViewDropPosition pos;

    target = gtk_drag_dest_find_target(widget, context, NULL);
    product_target = gdk_atom_intern_static_string(DO_DND_PRODUCT_TYPE);
    if (target == product_target) {
        gtk_tree_view_get_dest_row_at_pos(GTK_TREE_VIEW(priv->tree_view), x, y, &path, &pos);
        if (path) {
            GtkTreePath *new_path = NULL;
            gchar *str = (gchar*)gtk_selection_data_get_data(data);
            if (str)
                do_keyboard_model_insert_product(DO_KEYBOARD_MODEL(priv->model), path, &new_path, str);
            if (new_path)
                gtk_tree_path_free(new_path);
            gtk_tree_path_free (path);
        }
        send_received_info(DO_KEYBOARD_VIEW(user_data));

    }

}
/*!!
static GdkDrawable *view_create_drag_pixmap(GtkWidget *widget, DoKeyboardView *view)
{
	//DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (object);
	gchar *text;
	GdkDrawable *drawable;
	PangoContext *context;
	PangoLayout  *layout;

	PangoFontMetrics *metrics;
	int pixmap_height, pixmap_width;
	int layout_width, layout_height;
	int offset_x = 0;
	int char_width;

	text = get_desc_from_cursor(view);

    context = gtk_widget_create_pango_context(widget);
	layout = pango_layout_new (context);
	context = gtk_widget_get_pango_context (widget);
	metrics = pango_context_get_metrics (context,
					     widget->style->font_desc,
					     pango_context_get_language (context));

	char_width = pango_font_metrics_get_approximate_digit_width (metrics);
	pango_font_metrics_unref (metrics);

	pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
	pango_layout_set_width (layout, char_width * DRAG_ICON_MAX_WIDTH_CHARS);
	pango_layout_set_text (layout, text, strlen(text));
	pango_layout_get_size (layout, &layout_width, &layout_height);
	pixmap_width  = layout_width  / PANGO_SCALE + DRAG_ICON_LAYOUT_BORDER * 2;
    pixmap_height = layout_height / PANGO_SCALE + DRAG_ICON_LAYOUT_BORDER * 2;

	drawable = gdk_pixmap_new (widget->window,
				   pixmap_width  + 2,
				   pixmap_height + 2,
				   -1);

	gdk_draw_rectangle (drawable,
			    widget->style->base_gc [GTK_WIDGET_STATE (widget)],
			    TRUE,
			    0, 0,
			    pixmap_width + 1,
			    pixmap_height + 1);


	gdk_draw_layout (drawable,
			 widget->style->text_gc [GTK_WIDGET_STATE (widget)],
			 1 + DRAG_ICON_LAYOUT_BORDER + offset_x,
			 1 + DRAG_ICON_LAYOUT_BORDER,
			 layout);

	gdk_draw_rectangle (drawable,
			    widget->style->black_gc,
			    FALSE,
			    0, 0,
			    pixmap_width + 1,
			    pixmap_height + 1);

	g_object_unref (layout);

	g_free (text);

	return drawable;
}
*/
/*
static gchar *get_code_from_cursor(DoKeyboardView *view)
{
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(view->priv->tree_view), &path, NULL);
    if (!path)
        return NULL;
    gchar *retval = get_code_from_path(view, path);
    gtk_tree_path_free(path);
    return retval;
}

static gchar *get_desc_from_cursor(DoKeyboardView *view)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        return NULL;
    gchar *retval = get_desc_from_path(view, path);
    gtk_tree_path_free(path);
    return retval;
}*/
static gint get_key_from_cursor(DoKeyboardView *view)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        return 0;
    gint retval = get_key_from_path(view, path);
    gtk_tree_path_free(path);
    return retval;
}
static gboolean get_crnt_key(DoKeyboardView *view, GtkTreeIter *iter)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        return 0;
    gboolean retval = gtk_tree_path_get_depth(path) >= DO_KEYBOARD_MODEL_DEPTH_KEY;
    if (retval) {
        while (gtk_tree_path_get_depth(path) > DO_KEYBOARD_MODEL_DEPTH_KEY)
            gtk_tree_path_up(path);
        retval = gtk_tree_model_get_iter(priv->model, iter, path);
    }
    gtk_tree_path_free(path);

    return retval;
}
static gchar *get_desc_from_path(DoKeyboardView *view, GtkTreePath *path)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {
        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_KEYBOARD_MODEL_COL_DESC, &value);
        if (!g_value_get_string(&value))
             return NULL;
        return g_strdup(g_value_get_string(&value));
    }
    return NULL;
}
static gint get_key_from_path(DoKeyboardView *view, GtkTreePath *path)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {
        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_KEYBOARD_MODEL_COL_NUMBER, &value);
        return g_value_get_int(&value);
    }
    return 0;
}

static gchar *get_code_from_path(DoKeyboardView *view, GtkTreePath *path)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {
        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_KEYBOARD_MODEL_COL_CODE, &value);
        if (!g_value_get_string(&value))
	     return NULL;
        return g_strdup(g_value_get_string(&value));
    }
    return NULL;
}

static GList *clear_children(GList *list)
{
    GList *l, *p, *retlist = NULL;

    for (l = list; l; l = l->next) {
        gboolean child = FALSE;
        for (p = list; p; p = p->next)
            if (gtk_tree_path_get_depth(l->data) > gtk_tree_path_get_depth(p->data)) {
                GtkTreePath *path = gtk_tree_path_copy(l->data);
                while (gtk_tree_path_get_depth(path) > gtk_tree_path_get_depth(p->data))
                    gtk_tree_path_up(path);
                if (!gtk_tree_path_compare(path, p->data)) {
                    child = TRUE;
                    break;
                }
            }
        if (!child)
            retlist = g_list_append(retlist, gtk_tree_path_copy(l->data));
    }
    g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
    g_list_free (list);
    return retlist;
}
/*!!
static GList *up_for_all_children(DoView *view, GList *list)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (object);
    GList *l, *p, *retlist = list;
    gboolean do_ = TRUE;
    while (do_) {
        do_ = FALSE;
        for (l = retlist; l; l = l->next) {
            GtkTreePath *parent = gtk_tree_path_copy(l->data);
            if (gtk_tree_path_up(parent) && gtk_tree_path_get_depth(parent)) {
                GtkTreeIter iter;
                gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, parent);
                gint count = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(priv->model), &iter);
                gint count_sel = 0;
                for (p = retlist; p; p = p->next) {
                    GtkTreePath *path = gtk_tree_path_copy(p->data);
                    if (gtk_tree_path_up(path) && gtk_tree_path_get_depth(path) && !gtk_tree_path_compare(path, parent))
                        count_sel++;
                    gtk_tree_path_free(path);
                }
                if (count == count_sel) {
                    gtk_tree_path_free(l->data);
                    l->data = gtk_tree_path_copy(parent);
                    do_ = TRUE;
                    retlist = clear_children(retlist);
                    break;
                }
            }
            gtk_tree_path_free(parent);
        }
    }
    return retlist;
}
*/
static GtkTreeRowReference *get_prev_select(DoView *view, GList *list)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GtkTreeRowReference *retval = NULL;
    GtkTreePath *path = gtk_tree_path_copy(g_list_last(list)->data);
    GList *l;
    while (gtk_tree_path_prev(path)) {
        gboolean issel = FALSE;
        for (l = list; l; l = l->next)
            if (!gtk_tree_path_compare(l->data, path)) {
                issel = TRUE;
                break;
            }
        if (!issel) {
            retval = gtk_tree_row_reference_new(GTK_TREE_MODEL(priv->model), path);
            break;
        }
    }
    if (!retval) {
        gtk_tree_path_free(path);
        path = gtk_tree_path_copy(g_list_last(list)->data);
        if (gtk_tree_path_up(path) && gtk_tree_path_get_depth(path))
            retval = gtk_tree_row_reference_new(GTK_TREE_MODEL(priv->model), path);
        else {
            gtk_tree_path_free(path);
            path = gtk_tree_path_copy(g_list_last(list)->data);
            gtk_tree_path_next(path);
            GtkTreeIter temp_iter;
            if (gtk_tree_model_get_iter (GTK_TREE_MODEL(priv->model), &temp_iter, path))
                retval = gtk_tree_row_reference_new(GTK_TREE_MODEL(priv->model), path);
        }
    }
    gtk_tree_path_free(path);
    return retval;
}
static void	view_do_delete(DoView *view)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);

    GList *list = clear_children(gtk_tree_selection_get_selected_rows(priv->sel, &priv->model));
    //!!if (!(list = up_for_all_children(view, list)))
        //!!return;

    gboolean do_delete;
    GtkTreeRowReference *ref = get_prev_select(view, list);

    if (g_list_length(list) > 1 || gtk_tree_path_get_depth(list->data) < DO_KEYBOARD_MODEL_DEPTH_CONTENT) {
        GString *markup = g_string_new("Вы хотите удалить записи:");
        gchar *desc3 = NULL, *desc_last = NULL;
        gint count = 0;
        GList *l;
        for (l = list; l; l = l->next, count++) {
            gchar *desc = get_desc_from_path(DO_KEYBOARD_VIEW(view), (GtkTreePath*)l->data);
            gint depth = gtk_tree_path_get_depth(l->data);
            if (depth < DO_KEYBOARD_MODEL_DEPTH_CONTENT) {
                gchar *buf = g_malloc(strlen(desc) + 7 + 1);
                sprintf(buf, "<b>%s</b>", desc);
                g_free(desc);
                desc = buf;
            }
            switch (count) {
                case 0:
                case 1:
                    g_string_append_printf(markup, "\n%s", desc);
                    break;
                case 2:
                    desc3 = desc;
                    break;
                default:
                    if (!l->next)
                        desc_last = desc;
                    else
                        g_free(desc);
            }
        }
        if (count > 2) {
            if (count > 4)
                g_string_append(markup, "\n...");
            else
                g_string_append_printf(markup, "\n%s", desc3);
            if (count > 3)
                g_string_append_printf(markup, "\n%s", desc_last);
            if (count > 4)
                g_string_append_printf(markup, "\n%d - шт", count);
            g_string_append(markup, "?");
        }
        if (desc3) g_free(desc3);
        if (desc_last) g_free(desc_last);
        GtkWidget *dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_YES_NO,
                                  "%s",
                                  markup->str);
        gtk_window_set_icon_name(GTK_WINDOW (dialog), DO_STOCK_DOMINO_APPS);
        do_delete = (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES);
        gtk_widget_destroy (dialog);
        g_string_free(markup, TRUE);
    }
    else
        do_delete = list != NULL;

    if (do_delete) {
        if (do_keyboard_model_remove_from_list(DO_KEYBOARD_MODEL(priv->model), list)) {
            if (ref) {
                GtkTreePath *path = gtk_tree_row_reference_get_path(ref);
                if (path) {
                    gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
                    gtk_tree_path_free(path);
                }
            }
        }

    }
    if (ref)
        gtk_tree_row_reference_free(ref);
    g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
    g_list_free(list);
}
static gboolean	view_can_do_delete(DoView *view)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GList *list = gtk_tree_selection_get_selected_rows(priv->sel, &priv->model);
    if (list) {
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
        return TRUE;
    }
    return FALSE;
}
static void edit_document(DoKeyboardView *view, DoDialogAction action, gint key)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    const gchar *dtype = do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD);
    const gchar *sklad = domino_unit();

    DoContext *context = do_context_new_with_attributes(DO_CONTEXT_KIND_DOCUMENT,
                                      "sklad-code", sklad,
                                      "dtype-code", dtype,
                                      NULL);
    if (key) {
        gchar document[100];
        sprintf(document, "%d", key);

        g_object_set(context, "document-number", document, NULL);
    }

    GtkWidget *dialog = do_keyboard_dialog_new(gtk_widget_get_toplevel(GTK_WIDGET(view)), priv->alias_name, 0,
                                               action, context);
    g_object_set(dialog, "view", view, NULL);
    g_object_unref(context);

    if (do_keyboard_dialog_run(DO_KEYBOARD_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        g_object_get(dialog, "context", &context, NULL);
        set_cursor_to_context(DO_KEYBOARD_VIEW(view), context);
        update_selected_code(DO_KEYBOARD_VIEW(view), -1, TRUE);
        g_object_unref(context);
    }
    else {
        GtkTreeIter iter;
        if (get_crnt_key(DO_KEYBOARD_VIEW(view), &iter)) {
            GValue value = { 0, };
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_KEYBOARD_MODEL_COL_NUMBER, &value);
            update_selected_code(DO_KEYBOARD_VIEW(view), g_value_get_int(&value), TRUE);
        }
    }
    gtk_widget_destroy (dialog);
}

static void view_do_insert(DoView *view)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    gboolean go_sender = FALSE;
    GtkTreeIter iter;

    if (priv->sender && get_crnt_key(DO_KEYBOARD_VIEW(view), &iter)) {
        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_KEYBOARD_MODEL_COL_CHILDLESS, &value);
        go_sender = !g_value_get_boolean(&value);
    }
    if (go_sender) {
        gint response;
        GValue value = { 0, };
        gchar buffer[1024];
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_KEYBOARD_MODEL_COL_DESC, &value);
        if (g_value_get_string(&value)) {

        sprintf(buffer, "Добавить новую клавишу? Или добавлять товар в клавишу <b>%s</b>?", g_value_get_string(&value));

        GtkWidget *dialog = gtk_message_dialog_new_with_markup(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_NONE,
                                  "%s",
                                  buffer);
        gtk_window_set_icon_name(GTK_WINDOW (dialog), DO_STOCK_DOMINO_APPS);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Клавишу", GTK_RESPONSE_YES);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Товар", GTK_RESPONSE_ACCEPT);

        response = gtk_dialog_run (GTK_DIALOG (dialog));
        go_sender = response == GTK_RESPONSE_ACCEPT;
        gtk_widget_destroy (dialog);
        if (response != GTK_RESPONSE_YES && response != GTK_RESPONSE_ACCEPT)
            return;
        }
    }
    if (go_sender) {
        g_signal_emit (view, keyboard_view_signals[GO_SENDER], 0);
        do_view_do_grab_focus(DO_VIEW(priv->sender));
    }
    else
        edit_document(DO_KEYBOARD_VIEW(view), DO_DIALOG_ACTION_INSERT, 0);
}
static gboolean	view_can_do_insert(DoView *view)
{
    return TRUE;
}
static void	 view_do_copy(DoView *view)
{
    gint key = get_key_from_cursor(DO_KEYBOARD_VIEW(view));
    if (!key)
        return;
    edit_document(DO_KEYBOARD_VIEW(view), DO_DIALOG_ACTION_INSERT, key);
}
static gboolean	view_can_do_copy(DoView *view)
{
    gboolean retval = FALSE;
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GList *list = gtk_tree_selection_get_selected_rows(priv->sel, &priv->model);
    if (list) {
        if (g_list_length(list) == 1)
            retval = gtk_tree_path_get_depth(list->data) == DO_KEYBOARD_MODEL_DEPTH_KEY;
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    return retval;
}


static void	view_do_popup(DoView *view, GdkEventButton* event)
{
    GtkWidget *menu = do_view_actions_action_get_widget("/" DO_KEYBOARD_VIEW_POPUP);
    do_tree_view_popup(DO_TREE_VIEW(view), event, menu);
}
static void	view_do_edit(DoView *view, const gchar *tab)
{
    gint key = get_key_from_cursor(DO_KEYBOARD_VIEW(view));
    if (!key)
        return;
    edit_document(DO_KEYBOARD_VIEW(view), DO_DIALOG_ACTION_EDIT, key);
}
static gboolean	view_can_do_edit(DoView *view, const gchar *tab)
{
    if (tab && *tab != '\0')
        return FALSE;
    gboolean retval = FALSE;
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GList *list = gtk_tree_selection_get_selected_rows(priv->sel, &priv->model);
    if (list) {
        if (g_list_length(list) == 1)
            retval = gtk_tree_path_get_depth(list->data) == DO_KEYBOARD_MODEL_DEPTH_KEY;
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    return retval;
}
static void	view_do_apply(DoView *view)
{
}
static gboolean	view_can_do_apply(DoView *view)
{
    return FALSE;
}
static void	view_do_unapply(DoView *view)
{
}
static gboolean	view_can_do_unapply(DoView *view)
{
    return FALSE;
}

static const gchar *key_to_ru[2][33] =
{
    {"`",
     "q","w","e","r","t","y","u","i","o","p","[","]",
     "a","s","d","f","g","h","j","k","l",";","'",
     "z","x","c","v","b","n","m",",","."},
    {"ё",
     "й","ц","у","к","е","н","г","ш","щ","з","х","ъ",
     "ф","ы","в","а","п","р","о","л","д","ж","э",
     "я","ч","с","м","и","т","ь","б","ю",
    }
};
const gchar* do_keyboard_view_str_get_key_name(int key)
{
    if (key <=32 || key > 127)
        return NULL;
    int i;
    for (i = 0; i < 33; i++)
        if (key == key_to_ru[0][i][0])
            return key_to_ru[1][i];
    return NULL;
}

static void group_changed(GtkRadioButton *style, GtkWidget **widget)
{
    gtk_widget_set_sensitive(widget[0], TRUE);
    gtk_widget_set_sensitive(widget[1], TRUE);
}
static void group_changed1(GtkRadioButton *style, GtkWidget **widget)
{
    gtk_widget_set_sensitive(widget[0], FALSE);
    gtk_widget_set_sensitive(widget[1], FALSE);
}
static gchar *get_html_print(DoKeyboardView *view)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE(view);
    do_alias_t *alias;

    alias = domino_alias_new(priv->alias_name);

    if (!alias)
        return NULL;
    if (!do_alias_open(alias, TRUE)) {
        do_alias_free(alias);
        return NULL;
    }
    GString *buffer;

    gchar *cell_width;
    gchar *cell_height;
    gchar *default_font_family;
	gchar *default_font_style;
	gint   default_font_size;
	gint   count_cash = 0;
	gint   count_col = 0;
	gint   flag = 0;
    gchar *font_family;
	gchar *font_style;
	gint   font_size;



	int status;
	sklad_rec_t sklad;
	sklad_key0_t sklad_key0;
	do_text_set(alias, sklad_key0.code, domino_unit());
	status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL);
    if (status != DO_OK) {
        DOMINO_SHOW_ERROR("Склад %s не найден", domino_unit());
        do_alias_free(alias);
        return NULL;
    };

    gchar *value;
    value = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_KEYBOARD_WIDTH));
    if (*value != '\0') {
        cell_width = g_strdup(value);
    }
    else
        cell_width = g_strdup(label_size_width_default);
    do_free(value);
    value = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_KEYBOARD_HEIGHT));
    if (*value != '\0') {
        cell_height = g_strdup(value);
    }
    else
        cell_height = g_strdup(label_size_width_default);
    count_col = do_sklad_param_int(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_KEYBOARD_COL));

    if (!count_col) {
        count_col = 10;
    }

	gtk_widget_get_font_desc(GTK_WIDGET(view), label_font_default,
	                      &default_font_family, &default_font_style, &default_font_size);

    partner_rec_t partner;
    partner_key0_t partner_key0, partner_key;

    do_text_set(alias, partner_key0.g_code, do_param(DO_PARAM_PARTNER_REGION_CASH));
    memcpy(&partner_key.g_code, partner_key0.g_code, sizeof(partner_key.g_code));
    partner_key0.code = 0;

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_GE);
    while (status == DO_OK) {
        if (memcmp(&partner_key.g_code, partner_key0.g_code, sizeof(partner_key.g_code))) break;

        value = do_text(alias, partner.data.sklad);
        if (!strcmp(value, domino_unit())) count_cash++;
        do_free(value);
        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_NEXT);
    }

    gint i;
    gchar buf[1024];


    GtkBuilder *builder;



    builder = do_builder_new("keyboard_print");
    if ( !builder )
        return NULL;


    GtkWidget *dialog;
    GList *l, *sort_list = NULL;

    dialog = do_builder_get_widget(builder, "dialog_keyboard_print");

	gtk_window_set_icon_name (GTK_WINDOW(dialog), DO_STOCK_DOMINO_APPS);

    GtkWidget *entry[2], *r1, *r2, *r3;
#if GTK_MAJOR_VERSION > 2
    GAction *a;
#else
    GtkAction *a;
#endif

    a = do_builder_get_action(builder, "action_print_goods");
    g_signal_connect (a, "activate" , G_CALLBACK(group_changed), entry);
    a = do_builder_get_action(builder, "action_print_function");
    g_signal_connect (a, "activate" , G_CALLBACK(group_changed), entry);
    a = do_builder_get_action(builder, "action_print_all");
    g_signal_connect (a, "activate" , G_CALLBACK(group_changed), entry);
    a = do_builder_get_action(builder, "action_print_template");
    g_signal_connect (a, "activate" , G_CALLBACK(group_changed1), entry);

    r1 = do_builder_get_widget (builder, "print_goods");
    r2 = do_builder_get_widget (builder, "print_function");
    r3 = do_builder_get_widget (builder, "print_all");

    entry[0] = do_builder_get_widget (builder, "edit_copy");
    entry[1] = do_builder_get_widget (builder, "edit_rows");
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(entry[0]), 1, 10);
    gtk_spin_button_set_increments(GTK_SPIN_BUTTON(entry[0]), 1, 10);
    gtk_spin_button_set_increments(GTK_SPIN_BUTTON(entry[1]), 1, 10);
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(entry[1]), 1, 40);


	gtk_spin_button_set_value(GTK_SPIN_BUTTON(entry[0]), count_cash);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(entry[1]), count_col);

    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    count_cash = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entry[0]));
    count_col = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entry[1]));
    flag = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(r1))) ? 0 :
           (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(r2))) ? 1 :
           (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(r3))) ? 2 : 3;

    gtk_widget_destroy(dialog);
    if (result != -3)
        return NULL;
    if (flag == 3) {
        g_signal_emit (view, keyboard_view_signals[PRINT_TEMPLATE], 0);
        return NULL;
    }
    buffer = g_string_new(do_keyboard_str_head_html);
    g_string_append(buffer, do_keyboard_str_table_head_html);

    for (l = priv->html_template; l ; l = l->next) {
        l = l->next;
        gint *key;
        if (l) {
            key = l->data;
            sort_list = g_list_append(sort_list, key);
        }
        else
            break;
    }

    g_string_append(buffer, do_keyboard_str_tr_head_html);
    for (i = 0; i < count_cash; i++) {

        document_rec_t document;
        document_key0_t document_key0;
        do_text_set(alias, document_key0.sklad, domino_unit());
        do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD));

        gint count = 0;
        for (l = sort_list; l ; l = l->next) {
            gchar doc[100];
            gint *key = l->data;
            const gchar *key_label;
            if (!flag || flag == 2)
                key_label = do_keyboard_view_str_get_key_name(*key);
            else
                key_label = NULL;

            sprintf(doc, "%d", *key);
            do_text_set(alias, document_key0.document, doc);

            status = do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL);
            if (status == DO_OK) {
                gchar *label = do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_COMMENT));
                if ( !strncmp(label,"<b>", 3) ) {
                    gchar *buf = g_strdup(label+3);
                    g_free(label);
                    label=buf;
                }
                gint kind = do_document_param_int(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND));
                if (*label != '\0' &&
                    (flag == 2 ||
                    (flag == 1 && kind == DO_CONST_KEY_KIND_COMMAND) ||
                    (flag == 0 && kind != DO_CONST_KEY_KIND_COMMAND)
                    )) {
                    if (!count)
                        g_string_append(buffer, do_keyboard_str_tr_head_html);
                    count++;
                    value = do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_FONT));
                    if (*value != '\0') {
                        gtk_widget_get_font_desc(GTK_WIDGET(view), value,
                                        &font_family, &font_style, &font_size);

                        if (!key_label)
                            sprintf(buf, do_keyboard_str_td_html, cell_width, cell_height, font_size, font_family, label);
                        else
                            sprintf(buf, do_keyboard_str_td_html_aplha1,
                                cell_width, cell_height, 4 ,//atoi(cell_height) - 3,
                                font_size, font_family, key_label,
                                default_font_size, default_font_family,
                                label
                                );
                        g_free(font_family);
                        g_free(font_style);
                    }
                    else {
                        if (!key_label)
                            sprintf(buf, do_keyboard_str_td_html, cell_width, cell_height, default_font_size, default_font_family, label);
                        else
                            sprintf(buf, do_keyboard_str_td_html_aplha1,
                                cell_width, cell_height, 4, //atoi(cell_height) - 3,
                                default_font_size, default_font_family, key_label,
                                default_font_size, default_font_family,
                                label
                                );
                    }
                    g_string_append(buffer, buf);
                    if (count == count_col) {
                        count = 0;
                        g_string_append(buffer, do_keyboard_str_tr_tail_html);
                    }
                    do_free(value);
                }
                if (*label == '\0' && key_label) {
                    if (!count)
                        g_string_append(buffer, do_keyboard_str_tr_head_html);
                    count++;
                    sprintf(buf, do_keyboard_str_td_html_empty, cell_width, cell_height, default_font_size, default_font_family, key_label);
                    g_string_append(buffer, buf);
                    if (count == count_col) {
                        count = 0;
                        g_string_append(buffer, do_keyboard_str_tr_tail_html);
                    }
                }
                do_free(label);
            }
            else {
                if (key_label) {
                    if (!count)
                        g_string_append(buffer, do_keyboard_str_tr_head_html);
                    count++;
                    sprintf(buf, do_keyboard_str_td_html_empty, cell_width, cell_height, default_font_size, default_font_family, key_label);
                    g_string_append(buffer, buf);
                    if (count == count_col) {
                        count = 0;
                        g_string_append(buffer, do_keyboard_str_tr_tail_html);
                    }
                }
            }
        }
        g_string_append(buffer, do_keyboard_str_tr_tail_html);
    }
    g_string_append(buffer, do_keyboard_str_table_tail_html);
    g_string_append(buffer, do_keyboard_str_tail_html);

    do_alias_free(alias);
    g_object_unref(builder);

    return g_string_free(buffer, result != GTK_RESPONSE_ACCEPT);
}
static void	view_do_print(DoView *view)
{
    GtkWidget *html;
    GtkWidget *notebook;
    GtkWidget  *window;
    gchar *html_string;
    html_string = get_html_print(DO_KEYBOARD_VIEW(view));
    if (html_string) {
        window = gtk_widget_get_toplevel(GTK_WIDGET(view));
        html = do_html_view_new();
        notebook = do_window_get_notebook(DO_WINDOW(window));
        do_notebook_add_tab(DO_NOTEBOOK(notebook), DO_VIEW(html), -1, TRUE);
        g_object_set(html, "html-string", html_string, "title", "Клавиши", NULL);
        g_free(html_string);
    }
}
static gboolean	view_can_do_print(DoView *view)
{
    return TRUE;
}
static void selection_changed(GtkTreeSelection *selection, gpointer data)
{
    do_view_actions_refresh();
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE(data);
    send_received_info(DO_KEYBOARD_VIEW(data));
    GtkTreeIter iter;
    if (get_crnt_key(DO_KEYBOARD_VIEW(data), &iter)) {
        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_KEYBOARD_MODEL_COL_NUMBER, &value);
        update_selected_code(DO_KEYBOARD_VIEW(data), g_value_get_int(&value), FALSE);
    }
}
static void row_changed(GtkTreeModel *tree_model, GtkTreePath  *path, GtkTreeIter *iter, gpointer user_data)
{
    do_view_actions_refresh();
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (user_data);
    send_received_info(DO_KEYBOARD_VIEW(user_data));
    GValue value = { 0, };
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), iter, DO_KEYBOARD_MODEL_COL_NUMBER, &value);
    update_selected_code(DO_KEYBOARD_VIEW(user_data), g_value_get_int(&value), FALSE);
}
static void row_deleted(GtkTreeModel *tree_model, GtkTreePath  *path, gpointer user_data)
{
    do_view_actions_refresh();
    send_received_info(DO_KEYBOARD_VIEW(user_data));
}

static void focus_in(GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
    do_view_actions_set_view(user_data);
}
static gboolean	view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoKeyboardView *view)
{
    if (view_can_do_edit(DO_VIEW(view), NULL))
        view_do_edit(DO_VIEW(view), NULL);
}

static gboolean	view_receive(DoView *view, DoContext *context)
{
    gboolean retval = FALSE;
    if (do_context_get_kind(context) == DO_CONTEXT_KIND_PRODUCT) {
        gchar *product_code;
        DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
        GtkTreePath *path;
        g_object_get(context, "product-code", &product_code, NULL);
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
        GtkTreePath *new_path = NULL;
        if (path)
            retval = do_keyboard_model_insert_product(DO_KEYBOARD_MODEL(priv->model), path, &new_path, product_code);
        if (!retval) {
            GtkWidget *dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))),
                                      GTK_DIALOG_DESTROY_WITH_PARENT,
                                      GTK_MESSAGE_ERROR,
                                      GTK_BUTTONS_OK,
                                      "Невозможно добавить товары");
            gtk_window_set_icon_name(GTK_WINDOW (dialog), DO_STOCK_DOMINO_APPS);
            gtk_dialog_run (GTK_DIALOG (dialog));
            gtk_widget_destroy (dialog);
        }
        else {
            if (gtk_tree_path_get_depth(path) == DO_KEYBOARD_MODEL_DEPTH_KEY) {
                gtk_tree_view_expand_row(GTK_TREE_VIEW(priv->tree_view), path, FALSE);
            }
            if (new_path) {
                gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), new_path, NULL, FALSE);
                gtk_tree_path_free(new_path);
            }
        }
        gtk_tree_path_free(path);
    }
    if (retval)
        send_received_info(DO_KEYBOARD_VIEW(view));
    return retval;
}
static gboolean	view_undo_receive(DoView *view, DoContext *context)
{
    GtkTreeIter key_iter;
    if (!get_crnt_key(DO_KEYBOARD_VIEW(view), &key_iter))
        return FALSE;
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);

    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    GtkTreeRowReference *cursor_ref = gtk_tree_row_reference_new(priv->model, path);
    gtk_tree_path_free(path);

    gboolean retval = FALSE;
    DoContextKind kind = do_context_get_kind(context);
    switch (kind) {
        case DO_CONTEXT_KIND_PRODUCT:
        {
            gchar *product_code;
            g_object_get(context, "product-code", &product_code, NULL);
            GtkTreeIter content_iter;
            if (!gtk_tree_model_iter_children(priv->model, &content_iter, &key_iter)) // no child
                break;
            GList *l, *list = NULL;
            do {
                path = gtk_tree_model_get_path(priv->model, &content_iter);
                list = g_list_append(list, gtk_tree_row_reference_new(priv->model, path));
                gtk_tree_path_next(path);
            } while (gtk_tree_model_iter_next(priv->model, &content_iter));

            for (l = list; l; l=l->next) {
                path = gtk_tree_row_reference_get_path(l->data);
                gchar *code = get_code_from_path(DO_KEYBOARD_VIEW(view), path);//!!!
                if (!strcmp(product_code, code)) {
                    retval = do_keyboard_model_remove(DO_KEYBOARD_MODEL(priv->model), path);
                    if (!retval) break;
                }
                gtk_tree_path_free(path);
                gtk_tree_row_reference_free(l->data);
                g_free(code);
            }
            g_list_free(list);
        }
        default:
            break;
    }
    path = gtk_tree_row_reference_get_path(cursor_ref);
    if (!path) {
         path = gtk_tree_model_get_path(priv->model, &key_iter);
         if (path)
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
    }
    if (path) {
        gtk_tree_path_free(path);
    }
    if (retval)
        send_received_info(DO_KEYBOARD_VIEW(view));
    return retval;
}
static gboolean view_set_marked(DoView *view, DoContext *context, DoMarked marked)
{
    if ( marked == DO_MARKED_UNMARK )
        return view_undo_receive(view, context);
    else
        return view_receive(view, context);
}

static void view_update_context(DoView *view, DoContext *context)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE(view);
    GtkTreePath *path;
    GtkTreeRowReference *ref, *pref = NULL;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    ref = gtk_tree_row_reference_new(priv->model, path);
    if (gtk_tree_path_prev(path))
        pref = gtk_tree_row_reference_new(priv->model, path);
    gtk_tree_path_free(path);

    do_keyboard_model_update_context(DO_KEYBOARD_MODEL(priv->model), context);

    path = gtk_tree_row_reference_get_path(ref);
    if (!path) {
         if (pref)
            path = gtk_tree_row_reference_get_path(pref);
         if (path)
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
    }
}

static void set_cursor_to_context(DoKeyboardView *view, DoContext *context)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE(view);
    GtkTreePath *path;

    path = do_keyboard_model_get_path_from_context(DO_KEYBOARD_MODEL(priv->model), context);
    if (path)
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
}
static void view_do_close(DoView *embed)
{
    gtk_widget_destroy(GTK_WIDGET(embed));
}
static const char *view_get_title(DoView *embed)
{
    return "Справочник горячих клавиш";
}
static GdkPixbuf    *view_get_icon			(DoView *embed)
{
    return NULL;
}
static gboolean	  view_get_load_status	(DoView *embed)
{
    return FALSE;
}
#define LOAD_BUFFER_SIZE 65536
static void update_selected_code(DoKeyboardView *view, gint selected_code, gboolean force)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE(view);
    if (priv->selected_code == selected_code && !force)
        return;
    if (selected_code != -1)
        priv->selected_code = selected_code;
    if (!priv->template_object && !WEBKIT_IS_WEB_VIEW(priv->template_object))
        return;

    if (!priv->html_template)
        if (!html_template_create(view))
            return;


    GString *html_string = g_string_new(NULL);

    GList *l = priv->html_template;
    char *value, *buf;
    gint *key;

#if GTK_MAJOR_VERSION > 2
    GdkRGBA color_bg, color_fg;
    GtkStyleContext *context;
    context = gtk_widget_get_style_context(GTK_WIDGET(priv->tree_view));
    gtk_style_context_get_background_color(GTK_STYLE_CONTEXT(context), GTK_STATE_FLAG_NORMAL,&color_bg);
    gtk_style_context_get_color(GTK_STYLE_CONTEXT(context), GTK_STATE_FLAG_NORMAL,&color_fg);

#else
    GdkColor color_bg, color_fg;
    GtkStyle *style;
    style = gtk_widget_get_style (GTK_WIDGET(priv->tree_view));
    color_bg = style->bg[GTK_STATE_SELECTED];
    color_fg = style->fg[GTK_STATE_SELECTED];
#endif


    while (l) {
        g_string_append(html_string, (gchar*)l->data);
        l = l->next;
        if (l) {
            key = l->data;
            value = do_keyboard_model_get_label_from_number(DO_KEYBOARD_MODEL(priv->model), *key);
            if (!value)
                value = cash_key_code_to_string(*key);
            if (value) {
                if (selected_code == *key) {
                    buf = (char*) g_malloc(1024);
                    sprintf(buf,
#if GTK_MAJOR_VERSION > 2
                    "<div style='overflow:hidden; width:%s; height:%s;background-color:rgb(%.0f,%.0f,%.0f);'>"
                      "<font color=rgb(%.0f,%.0f,%.0f)>%s</font></div>",
#else
                    "<div style='overflow:hidden; width:%s; height:%s;background-color:rgb(%d,%d,%d);'>"
                      "<font color=rgb(%d,%d,%d)>%s</font></div>",
#endif
                       priv->label_width, priv->label_height,
#if GTK_MAJOR_VERSION > 2
                       color_bg.red*255, color_bg.green*255, color_bg.blue*255,
                       color_fg.red*255, color_fg.green*255, color_fg.blue*255,
#else
                       color_bg.red / 256, color_bg.green / 256, color_bg.blue / 256 ,
                       color_fg.red / 256, color_fg.green / 256, color_fg.blue / 256 ,

#endif
                       value);
                    g_free(value);
                    value = buf;
                }
                gchar buf_link[1024];
                sprintf(buf_link, "<a href=\"%d\" title=\"Клавиша %d\">", *key, *key);
                g_string_append(html_string, buf_link);
                g_string_append(html_string, value);
                g_string_append(html_string, "</a>");
                g_free(value);
            }
            l = l->next;
        }
    }

    gchar *html =  g_string_free(html_string, FALSE);
    webkit_web_view_load_html_string(WEBKIT_WEB_VIEW(priv->template_object), html, "");
    g_free(html);

}
static gboolean html_template_create(DoKeyboardView *view)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE(view);
    if (priv->template_false)
        return FALSE;

    priv->html_template = NULL;
    do_alias_t *alias;
    alias = domino_alias_new(priv->alias_name);
    sklad_rec_t sklad;
    sklad_key0_t sklad_key0;
    if (!alias || !do_alias_open(alias, 0, DO_DB_SKLAD, DO_DB_END)) {
        DOMINO_SHOW_ERROR("Невозможно открыть БД");
        if (alias)
            do_alias_free(alias);
        return FALSE;
    }
    do_text_set(alias, sklad_key0.code, domino_unit());

    if (do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) != DO_OK) {
        DOMINO_SHOW_ERROR("Невозможно прочитать подразделение %s", domino_unit());
        do_alias_free(alias);
        return FALSE;
    }
    priv->label_width = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_KEYBOARD_WIDTH));
    priv->label_height = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_KEYBOARD_HEIGHT));
    gchar *f_model = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_KEYBOARD_TEMPLATE));
    if (*f_model == '\0') {
        g_free(f_model);
        f_model = g_strdup(DEFAULT_TEMPLATE_NAME);
    }
    do_alias_free(alias);
    GFileInputStream *stream;
    stream = domino_get_template_stream(f_model);
    if (stream) {
        gssize n_read;
        gchar buffer[LOAD_BUFFER_SIZE];
        n_read = g_input_stream_read (G_INPUT_STREAM(stream),
                          buffer,
                          sizeof (buffer) - 1,
                          NULL,
                          NULL);
        if (n_read > 0) {
            buffer[n_read] = '\0';
            gchar *head = buffer, *ch = buffer;
            while (*ch != '\0') {
                head = ch;
                while ((*ch != '\0') && (*ch != '#' || *(ch + 1) != '#')) ch++;
                if (*ch == '\0') break;

                gchar *value = g_strndup(head, ch - head + 1);
                value[ch - head] = '\0';
                priv->html_template = g_list_append(priv->html_template, value);
                ch +=2;
                head = ch;
                while ( *ch >= '0' && *ch <= '9' ) ch++;
                int *key = g_malloc(sizeof(int));
                *key = atoi(head);
                priv->html_template = g_list_append(priv->html_template, key);
            }
            if (head != ch)
                priv->html_template = g_list_append(priv->html_template, g_strdup(head));
        }
        g_object_unref(stream);
    }
    else {
        DOMINO_SHOW_ERROR("Невозможно найти шаблон на клавиатуру \"%s\"", f_model);
        priv->template_false = TRUE;
    }
    do_free(f_model);
    return TRUE;
}
/*
static void	view_do_mail_send(DoView *view)
{
	DoContext *sklad = do_context_new_with_attributes(DO_CONTEXT_KIND_SKLAD,
	                       "sklad-code", domino_unit(), NULL);

	GtkWidget *win = do_upload_new(gtk_widget_get_toplevel(GTK_WIDGET(view)), NULL,
	                               DO_UPLOAD_FLAGS_SEND_KEYBOARD |
	                               DO_UPLOAD_FLAGS_SEND_AND_WAIT_FOR_APPLY |
	                               DO_UPLOAD_FLAGS_SEND_NOW
	                               , sklad);

    g_object_unref(sklad);
    gtk_widget_show(win);
}
static gboolean	view_can_do_mail_send(DoView *view)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    GtkTreePath *path = gtk_tree_path_new_first();
    GtkTreeIter iter;
    gboolean retval;
    retval = gtk_tree_model_get_iter(priv->model, &iter, path);
    gtk_tree_path_free(path);
    return retval;
}*/
static WebKitNavigationResponse navi_response(WebKitWebView        *web_view,
                                              WebKitWebFrame       *frame,
                                              WebKitNetworkRequest *request,
                                              DoKeyboardView       *view)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    gint  key;
    key = atoi(webkit_network_request_get_uri(request));
    if (key > 0) {
        update_selected_code(view, key, TRUE);
        if (do_keyboard_model_exist_number(DO_KEYBOARD_MODEL(priv->model), key))
            edit_document(view, DO_DIALOG_ACTION_EDIT, key);
        else
            edit_document(view, DO_DIALOG_ACTION_INSERT, key);
    }
    return WEBKIT_NAVIGATION_RESPONSE_IGNORE;
}
static void send_received_info(DoKeyboardView *view)
{
    DoKeyboardViewPrivate *priv = DO_KEYBOARD_VIEW_GET_PRIVATE (view);
    if (!priv->sender)
        return;
    GList *list = NULL, *l;
    GtkTreeIter iter;
    if (get_crnt_key(view, &iter))
        list = do_keyboard_model_get_product_codes(DO_KEYBOARD_MODEL(priv->model), &iter);

    DoContext *context;

    context = do_context_new();
    do_context_set_kind(context, DO_CONTEXT_KIND_PRODUCT);

    for ( l = list; l; l=l->next ) {
        g_object_set(context, "product-code", l->data, NULL);
        do_view_set_marked(priv->sender, context, DOMINO_MARKED);
    }
    g_object_unref(context);

    if (list) {
        g_list_foreach(list, (GFunc)g_free, NULL);
        g_list_free(list);
    }
}
static void sender_load_status(DoView *view, GParamSpec *pspec, GtkWidget *proxy)
{
    send_received_info(DO_KEYBOARD_VIEW(proxy));
}
void do_keyboard_view_show_template(DoKeyboardView *view)
{
    update_selected_code(view, -1, TRUE);
}
