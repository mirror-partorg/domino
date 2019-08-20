
#if GTK_MAJOR_VERSION == 2

#include "do_inventory_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_rate_dialog.h"
#include "domino.h"
#include "do_tree_view.h"
#include <domino.h>
#include "hig.h"
#include "do_icons.h"
#include <gdk/gdk.h>
#include <gdk/gdkevents.h>
#include <gdk/gdkkeysyms.h>
#include <time.h>

#define DO_INVENTORY_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_INVENTORY_VIEW, DoInventoryViewPrivate))

#define OBJECT_ROOT_PATH "InventoryView"
#define DEFAULT_COL_PACK_FORM_WIDTH 400
#define DEFAULT_COL_DEFAULT_WIDTH 60
#define FONT_ "16"

#define SHOW_ERROR(...) {gchar *val; val = g_strdup_printf(__VA_ARGS__); do_message_dialog_show(GTK_WIDGET(view), GTK_MESSAGE_ERROR, GTK_BUTTONS_CANCEL, val); g_free(val); }


// View interface
static void do_inventory_view_view_init(DoViewIface *iface);

static gboolean    do_inventory_view_view_do_grab_focus(DoView *view);
static void	       do_inventory_view_view_do_close(DoView *view);
static const char *do_inventory_view_view_get_title(DoView *view);
static GdkPixbuf  *do_inventory_view_view_get_icon(DoView *view);
static void		   do_inventory_view_view_do_edit(DoView *view, const gchar *tab);
static gboolean	   do_inventory_view_view_can_do_edit(DoView *view, const gchar *tab);
static gboolean	   do_inventory_view_view_can_do_delete(DoView *view);
static void		   do_inventory_view_view_do_delete(DoView *view);
static void		   do_inventory_view_view_do_insert(DoView *view);
static gboolean    do_inventory_view_view_can_do_insert(DoView *view);
static void        do_inventory_view_set_document(DoInventoryView *view, document_key0_t *document);
static void        do_inventory_view_button_clicked(GtkButton *button, DoInventoryView *view);
static gboolean    do_inventory_view_key_pressed(GtkWidget *widget, GdkEvent *event, DoInventoryView *view);

static GtkTreeStore *do_inventory_view_create_model(DoInventoryView *view);
static gboolean      do_inventory_view_fill_model(DoInventoryView *view);


static GtkWidget *do_message_dialog_create(GtkWidget *parent, GtkMessageType type, GtkButtonsType buttons, const gchar *msg)
{
    GtkWidget *widget;

    GtkWidget *win = NULL;
    GtkWidget *win1 = NULL;
    if (parent)
        win1 = gtk_widget_get_toplevel(parent);

    if (win1)  {
        if (!gtk_widget_get_visible(win1)) {
            if (gtk_window_get_transient_for (GTK_WINDOW(win1)))
                win = GTK_WIDGET(gtk_window_get_transient_for (GTK_WINDOW(win1)));
        }else
            win = win1;
    }
    if (!GTK_IS_WINDOW(win))
        win = NULL;
    widget = gtk_message_dialog_new (GTK_WINDOW(win), GTK_DIALOG_MODAL, type, buttons, NULL);

	gtk_window_set_icon_name (GTK_WINDOW(widget), DO_STOCK_DOMINO_APPS);
	gtk_window_set_position (GTK_WINDOW(widget), GTK_WIN_POS_CENTER);

    if ( msg )
        gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (widget), msg);
    return widget;
}
typedef struct _MessageForDialog MessageForDialog;

struct _MessageForDialog {
    GtkWidget *parent;
    GtkMessageType type;
    GtkButtonsType buttons;
    gchar *message;
    gboolean ask_exit;
};
//static GList *message_dialog_list = NULL;

static gint do_message_dialog_show(GtkWidget *parent, GtkMessageType type, GtkButtonsType buttons, const gchar *markup_format, ...)
{
    GtkWidget *widget;
    va_list args;
    gchar *msg = NULL;
    if (markup_format)
    {
        va_start (args, markup_format);
        msg = g_markup_vprintf_escaped (markup_format, args);
        va_end (args);
    }
    //message_dialog_list = g_list_append
    widget = do_message_dialog_create(parent, type, buttons, msg);
    g_free(msg);

    gint retval = gtk_dialog_run(GTK_DIALOG(widget));
    gtk_widget_destroy (widget);
    return retval;
}

enum
{
	PROP_0,
	PROP_USER,
	PROP_DOCUMENT,
};

enum
{
    COLUMN_TIME = 0,
    COLUMN_RUB = 1,

};
enum
{
    BUTTON_PREV =0,
    BUTTON_NEXT,
    BUTTON_BACK,
    BUTTON_QUIT,
    BUTTON_NO,
};

static const gchar *button_name[BUTTON_NO] =
{"Добавить",
 "Изменить",
 "Удалить",
 "Выход"};

 static const gboolean button_disable[BUTTON_NO] =
{TRUE,
 TRUE,
 TRUE,
 FALSE};


struct _DoInventoryViewPrivate
{
    GtkTreeModel        *model;
    GtkTreeView         *tree_view;
    GtkTreeViewColumn   **columns;
    GtkTreeSelection    *selection;
    GdkPixbuf           *icon;
    do_alias_t          *alias;
    document_key0_t      document_key0;
    document_rec_t       document;
    gchar               *user_code;
    GtkWidget           *buttons[BUTTON_NO];
};

G_DEFINE_TYPE_WITH_CODE (DoInventoryView, do_inventory_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_inventory_view_view_init)
                                    );

static void do_inventory_view_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_inventory_view_view_do_grab_focus;
    iface->do_close      = do_inventory_view_view_do_close;
    iface->get_title     = do_inventory_view_view_get_title;
    iface->get_icon      = do_inventory_view_view_get_icon;
    iface->do_edit       = do_inventory_view_view_do_edit;
    iface->can_do_edit   = do_inventory_view_view_can_do_edit;
    iface->do_delete       = do_inventory_view_view_do_delete;
    iface->do_insert       = do_inventory_view_view_do_insert;
    iface->can_do_delete   = do_inventory_view_view_can_do_delete;
    iface->can_do_insert   = do_inventory_view_view_can_do_insert;
}

static void do_inventory_view_init(DoInventoryView *view)
{
	DoInventoryViewPrivate *priv = DO_INVENTORY_VIEW_GET_PRIVATE (view);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_INFO, DO_VIEW_ICON_SIZE, 0, NULL);
}

static void do_inventory_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoInventoryView *view)
{
    DoInventoryViewPrivate *priv;
	priv = DO_INVENTORY_VIEW_GET_PRIVATE (view);
    GtkTreeModel *model;
    GtkTreePath *path_child;
    GtkTreeIter iter;

    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    model = gtk_tree_view_get_model(tree_view);
    path_child = gtk_tree_path_copy(path);
    gtk_tree_path_down(path_child);

    if ( gtk_tree_model_get_iter(model, &iter, path_child) ) {
        if ( gtk_tree_view_row_expanded(tree_view, path) )
            gtk_tree_view_collapse_row(tree_view, path);
        else
            gtk_tree_view_expand_row(tree_view, path, FALSE);
    }
    else
        do_inventory_view_view_do_edit(DO_VIEW(view), NULL);
    gtk_tree_path_free(path_child);
}

static GObject *do_inventory_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkTreeModel        *model;
    DoTreeView          *view;
	DoInventoryView          *embed;
	GtkTreeView         *tree_view;
	DoInventoryViewPrivate   *priv;
	GtkWidget                *hbox;

	object = G_OBJECT_CLASS (do_inventory_view_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_INVENTORY_VIEW_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_INVENTORY_VIEW(object);

    priv->alias = domino_alias_new(NULL);
    if ( priv->alias )
        if ( !do_alias_open(priv->alias, TRUE) ) {
            do_alias_free(priv->alias);
            priv->alias = NULL;
        }

	g_object_get(object, "tree-view", &tree_view, NULL);
    g_object_set(tree_view, "headers-visible", FALSE, NULL);
    g_signal_connect (tree_view, "row-activated", G_CALLBACK(do_inventory_view_row_activated), object);
    g_signal_connect (tree_view, "key-press-event", G_CALLBACK (do_inventory_view_key_pressed), object);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(tree_view), FALSE);

	priv->tree_view = tree_view;
    priv->selection = gtk_tree_view_get_selection(tree_view);

	priv->model = model = GTK_TREE_MODEL(do_inventory_view_create_model(embed));
	if (model)
        g_object_set(view, "model", model,
                           "fixed-height-mode", FALSE,
                           NULL);

    col = do_tree_view_add_column(view, "", "Опись", -1);
    renderer = gtk_cell_renderer_text_new();
    //g_object_set(renderer, "font", FONT_, NULL);
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, 0);
    gtk_tree_view_column_add_attribute (col, renderer, "markup", 1);

    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(priv->model), 2, GTK_SORT_ASCENDING);

    hbox=gtk_hbox_new(TRUE, 0);
    int i;

    for ( i = 0; i < BUTTON_NO; i++ ) {
	    GtkWidget *button;
        button = gtk_button_new_with_label(button_name[i]);
	    if ( button_disable[i] )
            gtk_widget_set_sensitive(button, FALSE);
	    /*if ( button_stock[i] ) {
            button = gtk_event_box_new();
	        GtkWidget *image;
	        image = gtk_image_new_from_stock(button_stock[i], GTK_ICON_SIZE_BUTTON);
            gtk_container_add(GTK_CONTAINER(button), image);
            //gtk_button_set_image(GTK_BUTTON(button), image);
	    }*/
        //gtk_widget_set_size_request(button, -1, BUTTON_HEIGHT);

	    priv->buttons[i] = button;
	    g_object_set_data(G_OBJECT(button), "number", GINT_TO_POINTER(i));
	    g_signal_connect(button, "clicked", G_CALLBACK(do_inventory_view_button_clicked), object);

	    gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 0);
	    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_HALF);
	    //gtk_button_set_use_underline(GTK_BUTTON(button), TRUE);

	    GtkWidget *l;
	    gchar *markup;
	    l = gtk_bin_get_child(GTK_BIN(button));
	    //markup=g_strdup_printf("<span font_size=\"%d\">%s</span>", FONT_SIZE, button_name[i]);
	    markup=g_strdup_printf("<b>%s</b>", button_name[i]);
	    if ( GTK_IS_LABEL(l) )
            gtk_label_set_markup(GTK_LABEL(l), markup);
        g_free(markup);
    }
    gtk_widget_show_all(hbox);
    do_tree_view_pack_header(DO_TREE_VIEW(object), hbox);

	gtk_widget_show(GTK_WIDGET(object));

	return object;
}

static void do_inventory_view_finalize (GObject *object)
{
    DoInventoryViewPrivate *priv;
	priv = DO_INVENTORY_VIEW_GET_PRIVATE (object);
	if (priv->alias)
		do_alias_free(priv->alias);
    g_free(priv->user_code);

	G_OBJECT_CLASS (do_inventory_view_parent_class)->finalize (object);
}

static void do_inventory_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	//DoInventoryViewPrivate *priv = DO_INVENTORY_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
	/* no readable properties */
	g_assert_not_reached ();
}

static void do_inventory_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoInventoryViewPrivate *priv = DO_INVENTORY_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_DOCUMENT:
            do_inventory_view_set_document(DO_INVENTORY_VIEW(object), g_value_get_pointer(value));
			break;
		case PROP_USER:
            g_free(priv->user_code);
            priv->user_code = g_value_dup_string(value);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
}

static void do_inventory_view_set_document(DoInventoryView *view, document_key0_t *document)
{
	DoInventoryViewPrivate  *priv;
	priv = DO_INVENTORY_VIEW_GET_PRIVATE (view);
	do_alias_t *alias;

    alias = priv->alias;
    if ( !alias )
        return;

    if ( do_document_get0(alias, &priv->document, document, DO_GET_EQUAL) == DO_OK ) {
        do_cpy(priv->document_key0.dtype, priv->document.data.dtype);
        do_cpy(priv->document_key0.sklad, priv->document.data.sklad);
        do_cpy(priv->document_key0.document, priv->document.data.document);
        do_inventory_view_fill_model(view);
    }
}
static void do_inventory_view_class_init (DoInventoryViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);
    GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);

	o_class->constructor  = do_inventory_view_constructor;
	o_class->finalize     = do_inventory_view_finalize;
	o_class->get_property = do_inventory_view_get_property;
	o_class->set_property = do_inventory_view_set_property;

	g_type_class_add_private (object_class, sizeof (DoInventoryViewPrivate));

	g_object_class_install_property
		(o_class,
		 PROP_DOCUMENT,
		 g_param_spec_pointer("document-key",
				     "Указатель на ключ документа",
				     "Указатель на ключ документа",
				     G_PARAM_WRITABLE));
	g_object_class_install_property
		(o_class,
		 PROP_USER,
		 g_param_spec_string("user-code",
				     "user-code",
				     "user-code",
				     NULL,
				     G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

GtkWidget *do_inventory_view_new (document_key0_t *key, const gchar *user_code)
{
	return g_object_new (DO_TYPE_INVENTORY_VIEW,
			     "document-key", key,
			     "user-code", user_code,
			     "object-root-path", OBJECT_ROOT_PATH,
			     (gpointer) NULL);
}
static gboolean do_inventory_view_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void	do_inventory_view_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_inventory_view_view_get_title(DoView *view)
{
    return "Курсы валют ";
}
static GdkPixbuf  *do_inventory_view_view_get_icon(DoView *view)
{
    DoInventoryViewPrivate *priv = DO_INVENTORY_VIEW_GET_PRIVATE(view);
    return priv->icon;
}

static gboolean	do_inventory_view_view_can_do_edit(DoView *view, const gchar *tab)
{
    if (tab && *tab != '\0')
        return FALSE;
    gboolean retval = FALSE;
	DoInventoryViewPrivate *priv = DO_INVENTORY_VIEW_GET_PRIVATE(view);
    GList *list = gtk_tree_selection_get_selected_rows(priv->selection, (GtkTreeModel**)&priv->model);
    if (list) {
        retval = (g_list_length(list) == 1);
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    else
        retval = FALSE;
    return retval;
}
static gboolean	do_inventory_view_view_can_do_delete(DoView *view)
{
    gboolean retval = FALSE;
	DoInventoryViewPrivate *priv = DO_INVENTORY_VIEW_GET_PRIVATE(view);
    GList *list = gtk_tree_selection_get_selected_rows(priv->selection, (GtkTreeModel**)&priv->model);
    if (list) {
        retval = (g_list_length(list) == 1);
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    else
        retval = FALSE;
    return retval;
}

static void	do_inventory_view_show_record(DoView *view, DoDialogAction action)
{
/*	DoInventoryViewPrivate *priv = DO_INVENTORY_VIEW_GET_PRIVATE(view);

    GtkTreePath *path;
    GtkTreeIter iter;
    GValue line = { 0, };
    gint response;

    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        return;

    if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {
        gtk_tree_path_free(path);
        return;
    }
    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, 0, &line);
    gtk_tree_path_free(path);
    DoContext *context, *new_context;
    GtkWidget *dialog;
    struct tm tm;
    time_t time;
    int ln;
    BTI_LONG date;
    time = g_value_get_long(&line);
    tm = *localtime(&time);
    do_date_set(&date, tm);
    ln = date;

    context = do_context_new_with_attributes(DO_CONTEXT_KIND_LINE,
     "dtype-code", do_param(DO_PARAM_DOCUMENT_TYPE_RATE),
     "sklad-code", do_param(DO_PARAM_COMMON_SKLAD),
     "document-number", DO_CONST_DEFAULT_DOCUMENT,
     "line-number", ln,
     NULL);

    dialog = do_rate_dialog_new(gtk_widget_get_toplevel(GTK_WIDGET(view)), priv->alias_name, 0, action, context);

    response = do_rate_dialog_run((DoRateDialog*)dialog);

    if (response == GTK_RESPONSE_ACCEPT) {
        switch (action) {
            case DO_DIALOG_ACTION_DELETE:
                gtk_list_store_remove(priv->model, &iter);
                break;
            case DO_DIALOG_ACTION_INSERT:
                g_object_get(dialog, "context", &new_context, NULL);
                do_inventory_view_insert_from_context(DO_INVENTORY_VIEW(view), new_context, TRUE);
                break;
            case DO_DIALOG_ACTION_EDIT:
                gtk_list_store_remove(priv->model, &iter);
                g_object_get(dialog, "context", &new_context, NULL);
                do_inventory_view_insert_from_context(DO_INVENTORY_VIEW(view), new_context, TRUE);
            default:
                break;
        }
    }
    g_object_unref(context);
    gtk_widget_destroy(dialog);
*/
}

static void	do_inventory_view_view_do_edit(DoView *view, const gchar *tab)
{
    do_inventory_view_show_record(view, DO_DIALOG_ACTION_EDIT);
}
static void	do_inventory_view_view_do_delete(DoView *view)
{
    do_inventory_view_show_record(view, DO_DIALOG_ACTION_DELETE);
}
static void	do_inventory_view_view_do_insert(DoView *view)
{
    do_inventory_view_show_record(view, DO_DIALOG_ACTION_INSERT);
}
static gboolean do_inventory_view_view_can_do_insert(DoView *view)
{
    return TRUE;
}
static void do_inventory_view_button_clicked(GtkButton *button, DoInventoryView *view)
{
	/*DoInventoryViewPrivate  *priv;
	priv = DO_INVENTORY_VIEW_GET_PRIVATE (view);*/

	gint number;

	number = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "number"));
	switch ( number ) {
	/*    case BUTTON_BACK:
            do_inventory_dialog_order_back(dialog, FALSE);
            break;
	    case BUTTON_NEXT:
            do_inventory_dialog_go_next(dialog);
            break;
	    case BUTTON_PREV:
            do_inventory_dialog_go_prev(dialog);
            break;*/
	    case BUTTON_QUIT:
            do_inventory_view_view_do_close(DO_VIEW(view));
            return;
	}
	do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static GtkTreeStore *do_inventory_view_create_model(DoInventoryView *embed)
{
//	DoInventoryViewPrivate *priv = DO_INVENTORY_VIEW_GET_PRIVATE(embed);
    GtkTreeStore    *model;
    model = gtk_tree_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);

    return model;
}
static gboolean do_inventory_view_fill_model(DoInventoryView *view)
{
	DoInventoryViewPrivate *priv = DO_INVENTORY_VIEW_GET_PRIVATE(view);
    do_alias_t      *alias;

    alias = priv->alias;
    if ( !alias )
        return FALSE;
    document_link_key0_t document_link_key0;
    document_link_rec_t  document_link;
    document_key0_t      document_key0;
    document_rec_t       document;
    int status;
    int base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);

    do_cpy(document_link_key0.m_dtype, priv->document_key0.dtype);
    do_cpy(document_link_key0.m_sklad, priv->document_key0.sklad);
    do_cpy(document_link_key0.m_document, priv->document_key0.document);
    document_link_key0.type = 106;
    document_link_key0.sort = 0;

    status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_GE);

    while ( status == DO_OK ) {
        GtkTreeIter iter;
        char *doc, *val1,*val2, *title, *user;

        if ( do_cmp(document_link_key0.m_dtype, priv->document_key0.dtype) ||
             do_cmp(document_link_key0.m_sklad, priv->document_key0.sklad) ||
             do_cmp(document_link_key0.m_document, priv->document_key0.document) ||
             document_link_key0.type != 106 ) break;


        do_cpy(document_key0.dtype, document_link.data.s_dtype);
        do_cpy(document_key0.sklad, document_link.data.s_sklad);
        do_cpy(document_key0.document, document_link.data.s_document);

        doc = do_util_document_key0_to_str(alias, &document_key0);
        if ( do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL) != DO_OK ) {
            SHOW_ERROR("Невозможно отрыть документ %s", doc);
            do_free(doc);
            return FALSE;
        }
        user = do_document_param(alias, &document, "75");
        if ( !g_strcmp0(user, priv->user_code) ) {
            int doc_number;

            val1 = do_document_param(alias, &document, "START_DESC");
            val2 = do_document_param(alias, &document, "END_DESC");
            title = g_strdup_printf("%s\n%s\n%s", doc, val1, val2);
            g_free(val1);g_free(val2);
            val1 = do_text(alias, document.data.document);
            val2 = strchr(val1, '.');
            if ( !val2 )
                doc_number=0;
            else doc_number = atoi(val2+1);
            g_free(val1);
            val1 = g_strdup_printf("%5.5d",99999-doc_number);
            gtk_tree_store_append(GTK_TREE_STORE(priv->model), &iter, NULL);
            gtk_tree_store_set(GTK_TREE_STORE(priv->model), &iter,
                                0, doc,
                                1, title,
                                2, val1,
                                3, doc_number,
                                -1);
            g_free(val1);g_free(doc);g_free(title);

            document_order_key2_t document_order_key2;
            document_order_rec_t document_order;
            product_key0_t product_key0;
            product_rec_t  product;
            double product_quant = 0;
            gchar *product_name = NULL;
            gchar *product_unit = NULL;
            int product_line = 0;
            int product_count = 0;
            double coef;

            GtkTreeIter iter_m;
            GtkTreeIter iter_p;


            do_cpy(document_order_key2.dtype, document.data.dtype);
            do_cpy(document_order_key2.sklad, document.data.sklad);
            do_cpy(document_order_key2.document, document.data.document);
            do_text_set_empty(document_order_key2.code);
            do_text_set_empty(product_key0.code);
            product_key0.base_parcel = 0;

            status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_GE);
            while ( status == DO_OK ) {
                if (
                    do_cmp(document_order_key2.dtype, document.data.dtype) ||
                    do_cmp(document_order_key2.sklad, document.data.sklad) ||
                    do_cmp(document_order_key2.document, document.data.document)
                    ) break;
                int line;
                double quant;
                gchar *code;
                code = do_text(alias, document_order.data.code);
                code[base_len] = '\0';

                if ( strncmp(product_key0.code, code, base_len) ) {
                    if ( product_name ) {
                         val2=do_rest_format(product_quant);
                         title = g_strdup_printf("%s\n <b>%s</b> %s", product_name,val2, product_unit);

                         if ( product_count  < 2 ) {
                             GValue doc = {0,};
                             gtk_tree_model_get_value(priv->model, &iter_p, 0, &doc);
                             val1 = g_value_dup_string(&doc);
                             gtk_tree_store_remove(GTK_TREE_STORE(priv->model), &iter_p);
                         }
                         else
                            val1 = g_strdup("");

                         gtk_tree_store_set(GTK_TREE_STORE(priv->model), &iter_m,
                                            0, val1,
                                            1, title,
                                            2, product_name,
                                            3, product_line,
                                            -1);
                        g_free(val1);
                        g_free(product_name);
                        g_free(product_unit);
                    }
                    product_quant = 0;
                    product_count = 0;
                    product_line = document_order.data.line;
                    do_text_set(alias, product_key0.code, code);
                    if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
                        status = DO_ERROR;
                        break;
                    }
                    product_name = do_text(alias, product.data.name);
                    product_unit = do_text(alias, product.data.tech_unit);
                    coef = do_product_coef(alias, &product);
                    gtk_tree_store_append(GTK_TREE_STORE(priv->model), &iter_m, &iter);
                }
                product_key4_t product_key4;
                do_cpy(product_key4.code, document_order.data.code);
                if ( do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) != DO_OK ) {
                    status = DO_ERROR;
                    break;
                }
                g_free(code);
                line = document_order.data.line;
                quant = do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST)/coef;
                product_quant +=quant;
                product_count++;
                val2=do_rest_format(quant);
                if ( product.data.base_parcel ) {
                    val1 = do_product_param(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_SERIA));
                    if ( val1[0] == '\0' ) {
                        g_free(val1);
                        val1 = NULL;
                    }
                }
                else
                    val1 = NULL;
                if ( val1 ) {
                    title = g_strdup_printf("%s\n %s\n <b>%s</b> %s", product_name, val1, val2, product_unit);
                    g_free(val1);
                }
                else
                    title = g_strdup_printf("%s\n <b>%s</b> %s", product_name,val2, product_unit);
                g_free(val2);
                doc = do_util_document_order_to_str(alias, &document_order);
                gtk_tree_store_append(GTK_TREE_STORE(priv->model), &iter_p, &iter_m);
                gtk_tree_store_set(GTK_TREE_STORE(priv->model), &iter_p,
                                    0, doc,
                                    1, title,
                                    2, product_name,
                                    3, line,
                                    -1);
                status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_NEXT);
                g_free(title);
            }
            if ( product_name ) {
                 val2=do_rest_format(product_quant);
                 title = g_strdup_printf("%s\n <b>%s</b> %s", product_name,val2, product_unit);

                 if ( product_count  < 2 ) {
                     GValue doc = {0,};
                     gtk_tree_model_get_value(priv->model, &iter_p, 0, &doc);
                     val1 = g_value_dup_string(&doc);
                     gtk_tree_store_remove(GTK_TREE_STORE(priv->model), &iter_p);
                 }
                 else
                    val1 = g_strdup("");

                 gtk_tree_store_set(GTK_TREE_STORE(priv->model), &iter_m,
                                    0, val1,
                                    1, title,
                                    2, product_name,
                                    3, product_line,
                                    -1);
                g_free(val1);
                g_free(product_name);
                g_free(product_unit);
            }
            if ( status == DO_ERROR )
                break;

        }
        g_free(user);
        status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;

    return TRUE;
}
static gboolean do_inventory_view_key_pressed(GtkWidget *widget, GdkEvent *event, DoInventoryView *view)
{
	guint mask = gtk_accelerator_get_default_mod_mask ();
	if ( (event->key.state & mask) == 0 )
	{
		switch (event->key.keyval)
		{
            case GDK_Escape:
                do_inventory_view_view_do_close(DO_VIEW(view));
                return TRUE;
		}
	}
	return FALSE;
}
#endif
