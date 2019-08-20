
#include "do_product_view.h"
#include "do_product_model.h"
#include "do_product_edit.h"
#include "do_view_actions.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_dnd.h"
#include "domino.h"
#include "do_icons.h"
#include <gdk/gdk.h>
#include <domino_action.h>
#include "do_common_actions.h"

#if GTK_MAJOR_VERSION == 2
#include <gdk/gdkkeysyms.h>
#endif

#define DO_PRODUCT_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_PRODUCT_VIEW, DoProductViewPrivate))
#define DO_TYPE_PRODUCT_VIEW_FLAGS do_product_view_flags_get_type()

#define DO_PRODUCT_VIEW_FILTER_TIMEOUT 300
#define DO_PRODUCT_VIEW_FILTER_SHOW_TIMEOUT 30000
#define RE_WORD "[A-ZА-Я]+"
#define RE_DIGIT "\\d+"
#define RE_NUMBER "[#|№|x|х|*|n]\\s*(?P<num>\\d+)"
#define DO_PRODUCT_VIEW_SEARCH_DIALOG_TIMEOUT 5000
#define MAX_KEYVALUE_BUFFER 13*6

#define ALL_COLS_SHOW_WIDTH 1100
#define OBJECT_ROOT_PATH "ProductView"
#define DEFAULT_COL_CODE_WIDTH 60
#define DEFAULT_COL_RECEIVED_WIDTH 30
#define DEFAULT_COL_NAME_WIDTH 400
#define DEFAULT_COL_MANU_WIDTH 100
#define DEFAULT_COL_DEFAULT_WIDTH 60

#ifdef ADS_RECOMMEND
#define DO_PRODUCT_VIEW_RECOMMEND_TIMEOUT 30000
#define DO_PRODUCT_VIEW_RECOMMEND_REFILL_TIMEOUT 300
#endif

#ifndef GDK_KEY_BackSpace
#define GDK_KEY_BackSpace GDK_BackSpace
#endif
#ifndef GDK_KEY_Escape
#define GDK_KEY_Escape GDK_Escape
#endif
#ifndef GDK_KEY_Up
#define GDK_KEY_Up GDK_Up
#endif
#ifndef GDK_KEY_Down
#define GDK_KEY_Down GDK_Down
#endif
#ifndef GDK_KEY_Page_Up
#define GDK_KEY_Page_Up GDK_Page_Up
#endif
#ifndef GDK_KEY_Page_Down
#define GDK_KEY_Page_Down GDK_Page_Down
#endif
#ifndef GDK_KEY_Return
#define GDK_KEY_Return GDK_Return
#endif
#ifndef GDK_KEY_KP_Enter
#define GDK_KEY_KP_Enter GDK_KP_Enter
#endif
#ifndef GDK_KEY_ISO_Enter
#define GDK_KEY_ISO_Enter GDK_ISO_Enter
#endif
#ifndef GDK_KEY_Right
#define GDK_KEY_Right GDK_Right
#endif
#ifndef GDK_KEY_Left
#define GDK_KEY_Left GDK_Left
#endif
#ifndef GDK_KEY_F12
#define GDK_KEY_F12 GDK_F12
#endif
#ifndef GDK_KEY_F1
#define GDK_KEY_F1 GDK_F1
#endif
#ifndef GDK_KEY_F4
#define GDK_KEY_F4 GDK_F4
#endif

const gchar *info_labels[] = {"Поиск по совпадению,МНН (нажмите →)",
                               "Поиск в интернете (нажмите →)"};

static void cell_name(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, DoProductView *do_view);
//static void cell_list(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data);
static void cell_code(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, DoProductView *do_view);
static void cell_received(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, DoProductView *do_view);

// Interactive search functions
static gboolean do_view_key_press(GtkWidget *widget, GdkEventKey *event, DoProductView *do_view);
static gboolean do_view_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean search_add(DoProductView *do_view, gchar *string);
static void search_clear_to_begin_word(DoProductView *do_view);
static void search_back(DoProductView *do_view);
static gboolean search_clear(DoProductView *do_view);
static gboolean search_find(DoProductView *do_view, const char *text);
static gboolean search_find_by_bcode(DoProductView *do_view, const char *text);
static gboolean start_find(GtkTreeView *treeview, gpointer user_data);
static gboolean search_flush_timeout(DoProductView *do_view);
static void search_refresh_timeout(DoProductView *do_view);
#ifdef ADS_RECOMMEND
static void do_product_view_set_toolbar_markup(DoProductView *view, const gchar *markup);
static void recommend_refresh_timeout(DoProductView *do_view);
static gboolean recommend_timeout(DoProductView *do_view);
#endif
#ifdef LOAD_MARLED_MANUAL
static gboolean do_product_view_load_marked(DoProductView *view);
#endif
static gchar *get_code_from_cursor(DoProductView *do_view);
static gchar *get_name_from_cursor(DoProductView *view);
static gchar *get_code_from_path(DoProductView *view, GtkTreePath *path);
static gboolean get_marked_from_path(DoProductView *view, GtkTreePath *path);

static void row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoProductView *view);

static gboolean change_marked(DoProductView *do_view, GtkTreePath *path);

static void do_product_view_sync_load_status (DoProductModel *model, GParamSpec *pspec, GtkWidget *proxy);
static void do_product_view_sync_load_progress (DoProductModel *model, GParamSpec *pspec, GtkWidget *proxy);
static void do_product_view_show_model_error (DoProductModel *model, GParamSpec *pspec, GtkWidget *proxy);
#ifdef ADS_RECOMMEND
static GHashTable *get_info_messages(const gchar *alias_name);
#endif
// Drag and Drop
static void drag_begin(GtkWidget *widget, GdkDragContext *drag_context, DoProductView *do_view);
static void drag_data_get(GtkWidget *widget, GdkDragContext *drag_context, GtkSelectionData *data, guint info, guint time, DoProductView *do_view);
//!!static GdkPixmap *view_create_drag_pixmap(GtkWidget *widget, DoProductView *do_view);

static void update_flags(DoProductView *view);
// View interface
static void do_product_view_view_init(DoViewIface *iface);

static gboolean    view_do_grab_focus(DoView *view);
static void        view_do_close(DoView *view);

static void        view_do_delete(DoView *view);
static gboolean    view_can_do_delete(DoView *view);
static void        view_do_insert(DoView *view);
static gboolean    view_can_do_insert(DoView *view);
static void    	view_do_copy(DoView *view);
static gboolean    view_can_do_copy(DoView *view);
static void    	view_do_edit(DoView *view, const gchar *tab);
static gboolean    view_can_do_edit(DoView *view, const gchar *tab);
static void    	view_do_apply(DoView *view);
static gboolean    view_can_do_apply(DoView *view);
static void    	view_do_unapply(DoView *view);
static gboolean    view_can_do_unapply(DoView *view);
static void    	view_do_popup(DoView *view, GdkEventButton* event);
static const char *view_get_title(DoView *view);
static GdkPixbuf  *view_get_icon(DoView *view);
static gboolean       view_get_load_status(DoView *view);
static gint           view_get_load_progress(DoView *view);
static void        view_update_context(DoView *view, DoContext *context);

static gboolean    view_can_do_send(DoView *view);
static void    	view_do_send(DoView *view);

static void    	view_do_save_as(DoView *view);
static gboolean    view_can_do_save_as(DoView *view);

//static gboolean do_product_view_receive(DoView *view, DoContext *context);
static gboolean do_product_view_set_marked(DoView *view, DoContext *context, DoMarked marked);
static gboolean do_product_view_toggle_marked(DoView *view, DoContext *context);
//static DoMarked do_product_view_get_marked(DoView *view, DoContext *context);
static gboolean     do_product_view_can_do_find_barcode(DoView *view);
static void      do_product_view_do_find_barcode(DoView *view);
static void  do_product_view_refresh_marked(DoProductModel *model, GtkTreeIter *iter, DoProductView *view);
#ifdef ADS_RECOMMEND
static GtkWidget *do_product_view_make_recommend(DoProductView *view);
#endif
static void do_product_view_filter_entry_changed(GtkEditable *entry, DoView *view);
static void do_product_view_filter_entry_activate(GtkEntry *entry, DoView *view);
static void do_product_view_filter_entry_action_activate(GtkEntryCompletion *completion, DoView *view);
//static void do_product_view_filter_entry_action_activate(GtkEntryCompletion *completion, gint, DoView *view);
static gboolean do_product_view_filter_entry_key_press(GtkWidget *widget, GdkEventKey *event, DoProductView *do_view);
static gboolean do_product_view_filter_apply(DoProductView *do_view);
//static gboolean do_product_view_filter_get_completion(DoProductView *do_view, const gchar *text);
static gboolean filter_show_timeout(DoProductView *do_view);
static void do_product_view_filter_clear(DoProductView *do_view);
static gboolean do_product_view_filter_entry_focus_in(GtkWidget *widget, GdkEvent *event, DoProductView *do_view);
static gboolean do_product_view_filter_entry_focus_out(GtkWidget *widget, GdkEvent *event, DoProductView *do_view);
#if GTK_MAJOR_VERSION > 2
static void do_product_view_filter_icon_press(GtkEntry *entry, GtkEntryIconPosition icon_pos, GdkEvent *event, DoProductView *do_view);
#endif
static gboolean do_product_view_set_clock(DoProductView *window);

enum
{
    PROP_0,
    PROP_RECEIVER_OBJECT,
    PROP_ALIAS,
    PROP_LOAD_STATUS,
    PROP_LOAD_PROGRESS,
    PROP_FLAGS,
    PROP_UNITS,
    PROP_TAB_VIEW,
};

GType do_product_view_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_product_view_flags_values[] = {
         { DO_PRODUCT_VIEW_FLAGS_SHOW_MULTI_PRICE, "DO_PRODUCT_VIEW_FLAGS_SHOW_MULTI_PRICE", "show-multi-price" },
         { DO_PRODUCT_VIEW_FLAGS_SHOW_RECEIVED, "DO_PRODUCT_VIEW_FLAGS_SHOW_RECEIVED", "show-received" },
         //{ DO_PRODUCT_VIEW_FLAGS_SELECTION_MULTIPLE, "DO_PRODUCT_VIEW_FLAGS_SELECTION_MULTIPLE", "selection-multiple" },
         { DO_PRODUCT_VIEW_FLAGS_ROW_ACTIVATED_TO_SEND, "DO_PRODUCT_VIEW_FLAGS_ROW_ACTIVATED_TO_SEND", "row-activated-to-send"},
         { DO_PRODUCT_VIEW_FLAGS_LOAD_IN_THREAD, "DO_PRODUCT_VIEW_FLAGS_LOAD_IN_THREAD", "load-in-thread" },
         { DO_PRODUCT_VIEW_FLAGS_REALTIME, "DO_PRODUCT_VIEW_FLAGS_REALTIME", "realtime" },
         { DO_PRODUCT_VIEW_FLAGS_SHOW_UNPACK_PRICE, "DO_PRODUCT_VIEW_FLAGS_SHOW_UNPACK_PRICE", "show-unpack-price" },
         { DO_PRODUCT_VIEW_FLAGS_SHOW_RECOMMEND, "DO_PRODUCT_VIEW_FLAGS_SHOW_RECOMMEND", "show-recomend" },
         { DO_PRODUCT_VIEW_FLAGS_SHOW_CLOCK, "DO_PRODUCT_VIEW_FLAGS_SHOW_CLOCK", "show-clock" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoProductViewFlags", _do_product_view_flags_values);
    }
    return type;
}
enum {
    DO_PRODUCT_VIEW_SEARCH_ITEM_NAME = 0,
    DO_PRODUCT_VIEW_SEARCH_ITEM_CODE,
    DO_PRODUCT_VIEW_SEARCH_ITEM_BCODE
};

enum
{
    COL_RECEIVED = 0,
    COL_CODE,
    COL_NAME1,
    COL_MANUFACTOR,
    COL_PACK,
    COL_PRICE,
    COL_STOCK,
    COL_PRICE_SMALL,
    COL_STOCK_SMALL,
    COL_LIFETIME,
    COL_AVAILABILITY,
    COL_LIST,
    N_COLUMNS,
};

struct _DoProductViewPrivate
{
    GtkTreeModel        *model;
    GtkTreeView         *tree_view;
    DoProductViewFlags   flags;
    DoView              *receiver;
    //GtkTreeSelection    *sel;
    GdkPixbuf           *received_icon;
    GtkCellRenderer     *rendererName;
    do_string_list_t    *units;
    gchar               *unit_str;
    gchar               *title;
    gchar               *tab_show;

    GtkTreeViewColumn   *cols[N_COLUMNS];

    gchar        search_text[1024];
    gint         search_char_count;
    gint         search_item;
    guint        search_flush_timeout;

    DoContext    *context_received;
    GdkPixbuf    *icon;
    gchar        *alias_name;
    do_alias_t   *alias;
#ifdef ADS_RECOMMEND
    gboolean  recommend_empty;
    do_alias_t   *alias_recommend;

    GtkWidget    *recommend;
    GtkWidget    *recommend_widget;
    GtkListStore *recommend_list;
    do_string_list_t *recommend_master;
    GtkTreeViewColumn *recommend_last_col,*recommend_first_col;

    guint        recommend_flush_timeout;
    //guint        recommend_refill_timeout;
    gboolean     refill;

#endif
    GtkWidget    *filter_entry;
    guint        filter_flush_timeout;
    guint        filter_show_timeout;
    GtkWidget    *filter_info;
    GRegex       *re_word;
    GRegex       *re_digit;
    GRegex       *re_number;
    do_string_list_t *lexs;
    gchar        *filter_text;
    //do_string_list_t *filter_completion;
    //gboolean      filter_key_press_sad;
    GtkWidget    *clock;
    guint         clock_event_source;
    GtkWidget    *notebook;
#ifdef ADS_RECOMMEND
    GHashTable   *messages;
    GRand        *rand;
#endif

    GtkWidget    *statusbar;
    GtkWidget    *statusbox;
    gboolean      set_label;
    gint          label_width;

};

G_DEFINE_TYPE_WITH_CODE (DoProductView, do_product_view, DO_TYPE_TREE_VIEW,
    		 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_product_view_view_init)
                                    );


static void do_product_view_view_init(DoViewIface *iface)
{
    iface->do_close      = view_do_close;
    iface->do_grab_focus = view_do_grab_focus;

    iface->do_mail_send     = view_do_send;
    iface->can_do_mail_send = view_can_do_send;

    iface->do_delete     = view_do_delete;
    iface->can_do_delete = view_can_do_delete;
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
    iface->do_popup      = view_do_popup;
    iface->get_title     = view_get_title;
    iface->get_icon      = view_get_icon;
    iface->get_load_status = view_get_load_status;
    iface->get_load_progress = view_get_load_progress;
    iface->set_marked   = do_product_view_set_marked;
    iface->toggle_marked = do_product_view_toggle_marked;
    //iface->get_marked   = do_product_view_get_marked;
    iface->update_context  = view_update_context;
    iface->do_save_as  =     view_do_save_as;
    iface->can_do_save_as = view_can_do_save_as;
    iface->can_do_find_barcode = do_product_view_can_do_find_barcode;
    iface->do_find_barcode = do_product_view_do_find_barcode;
}

static const GtkTargetEntry drag_types [] =
{
    { DO_DND_TEXT_TYPE, 0, 0},
    { DO_DND_PRODUCT_TYPE, GTK_TARGET_SAME_APP, 0},
};

static void wrapped_label_size_allocate_callback(GtkWidget *label,
    	      GtkAllocation *allocation,
    	    		      gpointer data) {

    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (data);
    priv->label_width=allocation->width;
    //label_height=do_product_edit_get_widget_text_height(label,5);
        gtk_widget_set_size_request(label, priv->label_width, -1);//label_height);
}

static void do_product_view_init(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (do_view);

    priv->context_received = do_context_new_with_attributes(DO_CONTEXT_KIND_PRODUCT, NULL);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, DO_STOCK_DOMINO_MED, DO_VIEW_ICON_SIZE, 0, NULL);
}
#define DO_PRODUCT_VIEW_POPUP "product-view-popup"

static gchar *popup_ui =
    "  <popup name='" DO_PRODUCT_VIEW_POPUP "'>\n"
    "    <menuitem action='InsertAction'/>\n"
    "    <menuitem action='EditAction'/>\n"
    "    <menuitem action='CopyAction'/>\n"
    "    <menuitem action='DeleteAction'/>\n"
    "    <separator/>\n"
    "    <menuitem action='ApplyAction'/>\n"
    "    <menuitem action='UndoApplyAction'/>\n"
    "  </popup>\n";

/*static gboolean true_func()
{
    return TRUE;
}*/
#ifdef ADS_RECOMMEND
static void
do_product_view_cursor_changed(GtkTreeView *tree_view,
               gpointer     user_data)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (user_data);
    gchar *code;
    do_list_t *list;
    gchar *message = NULL;

    code = get_code_from_cursor(user_data);
    if ( code ) {
        list = g_hash_table_lookup(priv->messages, code);
        if ( list ) {
            GList *l;
            gint32 indx;
            indx = g_rand_int_range (priv->rand, 0, list->count);
            l = list->list[indx];
            message = l->data;
        }
        g_free(code);
    }
    do_product_view_set_toolbar_markup(DO_PRODUCT_VIEW(user_data), message);
};
#endif
static GObject *do_product_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
    GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkTreeView         *tree_view;
    //GtkTreeSelection    *sel;
    GtkTreeModel        *model;
    gboolean            show_all_cols = TRUE;

    DoProductView       *do_view;
    DoTreeView          *do_tree_view;
    DoProductViewPrivate *priv;

    object = G_OBJECT_CLASS (do_product_view_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_PRODUCT_VIEW_GET_PRIVATE(object);

    if ( !priv->alias )
        return NULL;
#ifdef ADS_RECOMMEND
    if ( !priv->alias_recommend )
        return NULL;
#endif

    do_tree_view = DO_TREE_VIEW(object);
    do_view = DO_PRODUCT_VIEW(object);

    if ( priv->units->count > 1 ) {
        int i;
        GString *str;
        str = g_string_new("");
        for ( i = 0; i < priv->units->count; i++ )
            g_string_append_printf(str, "%s%s", i ? ",":"", g_strdup(priv->units->list[i]));
        gchar *val= g_string_free(str, FALSE);
        priv->title = g_strdup_printf("Товары (%s)", val);
        g_free(val);
    }
    else {
        do_alias_t *alias;
        gchar      *name;
        alias = domino_alias_new(priv->alias_name);
        if (alias && do_alias_open(alias, 0, DO_DB_SKLAD, DO_DB_END)) {
            sklad_rec_t sklad;
            sklad_key0_t sklad_key0;
            do_text_set(alias, sklad_key0.code, priv->units->list[0]);
            switch (do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL)) {
                case DO_ERROR:
                case DO_KEY_NOT_FOUND:
                    DOMINO_SHOW_ERROR("Ошибка чтения справочника подразделение");
                    break;
                case DO_OK:
                    name = do_text(alias, sklad.data.name);
                    priv->title = g_strdup_printf("Товары (%s)", name);
                    do_free(name);
                    break;
            }
        }
        if (alias)
            do_alias_free(alias);
    }

    g_object_get(object, "tree-view", &tree_view, NULL);

    do_view_actions_add_ui_from_string(popup_ui, NULL);
    priv->model = model = do_product_model_new(priv->alias_name, priv->units,
                                              (priv->flags & DO_PRODUCT_VIEW_FLAGS_LOAD_IN_THREAD) == DO_PRODUCT_VIEW_FLAGS_LOAD_IN_THREAD,
                                              (priv->flags & DO_PRODUCT_VIEW_FLAGS_REALTIME) == DO_PRODUCT_VIEW_FLAGS_REALTIME,
                                              (priv->flags & DO_PRODUCT_VIEW_FLAGS_SHOW_MULTI_PRICE) == DO_PRODUCT_VIEW_FLAGS_SHOW_MULTI_PRICE
                                              );

    g_object_set(object, "model", model,
                         "fixed-height-mode", TRUE,
                    //!!(priv->flags & DO_PRODUCT_VIEW_FLAGS_SHOW_MULTI_PRICE) != DO_PRODUCT_VIEW_FLAGS_SHOW_MULTI_PRICE,
                    NULL);


    g_signal_connect_object (model, "notify::load-status",
    			 G_CALLBACK (do_product_view_sync_load_status), object, 0);
    g_signal_connect_object (model, "notify::load-progress",
    			 G_CALLBACK (do_product_view_sync_load_progress), object, 0);
    g_signal_connect_object (model, "notify::error",
    			 G_CALLBACK (do_product_view_show_model_error), object, 0);

    /*if ( (priv->flags & DO_PRODUCT_VIEW_FLAGS_LOAD_IN_THREAD) != DO_PRODUCT_VIEW_FLAGS_LOAD_IN_THREAD &&
         (priv->flags & DO_PRODUCT_VIEW_FLAGS_REALTIME) != DO_PRODUCT_VIEW_FLAGS_REALTIME )
    {
     //   do_product_view_sync_load_progress(object);
       // do_product_view_sync_load_status(object);
    }*/
    priv->tree_view = tree_view;

#ifdef FULL_SCREEN
    {
    GtkWindow *window;
    gint width, height;
    window = domino_get_main_window();
    if ( window ) {
        gint width, height;
        gtk_window_get_size(window, &width, &height);
        show_all_cols = width > ALL_COLS_SHOW_WIDTH;
    }
    }
#endif


#if defined(MIN_SCREEN) || defined(CASH)
    gtk_tree_view_set_headers_visible(tree_view, FALSE);
#endif

    //priv->sel = sel = gtk_tree_view_get_selection(tree_view);

    /*if ( (priv->flags & DO_PRODUCT_VIEW_FLAGS_SELECTION_MULTIPLE) ==
          DO_PRODUCT_VIEW_FLAGS_SELECTION_MULTIPLE )
        gtk_tree_selection_set_mode( GTK_TREE_SELECTION( priv->sel ),
                                 GTK_SELECTION_MULTIPLE);*/

    gtk_tree_view_set_enable_search(tree_view, FALSE);

    g_signal_connect (tree_view, "key-press-event", G_CALLBACK (do_view_key_press), do_view);
    g_signal_connect (tree_view, "start-interactive-search", G_CALLBACK (start_find), do_view);
    g_signal_connect (tree_view, "button-press-event", G_CALLBACK (do_view_button_press), do_view);
#ifdef ADS_RECOMMEND
    g_signal_connect (tree_view, "cursor-changed",  G_CALLBACK( do_product_view_cursor_changed ), do_view);
#endif
    g_signal_connect (tree_view, "row-activated", G_CALLBACK(row_activated), object);

    gtk_drag_source_set(GTK_WIDGET(tree_view), GDK_BUTTON1_MASK, drag_types, G_N_ELEMENTS(drag_types), GDK_ACTION_MOVE | GDK_ACTION_COPY | GDK_ACTION_LINK);
    g_signal_connect(tree_view, "drag-begin", G_CALLBACK (drag_begin), object);
    g_signal_connect(tree_view, "drag-data-get", G_CALLBACK (drag_data_get), object);

    col = priv->cols[COL_RECEIVED] = do_tree_view_add_column(do_tree_view, "Recieved", "", DEFAULT_COL_RECEIVED_WIDTH);
    renderer = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_visible(col, priv->flags & DO_PRODUCT_VIEW_FLAGS_SHOW_RECEIVED);
    gtk_tree_view_column_set_cell_data_func(col, renderer, (GtkTreeCellDataFunc)cell_received, object, NULL);
    g_object_set (renderer,
                    "xalign", (gfloat)0.5,
                    "yalign", (gfloat)0.5,
                    "mode", GTK_CELL_RENDERER_MODE_INERT,
                    NULL);

    col = priv->cols[COL_CODE] = do_tree_view_add_column(do_tree_view, "Code", "Код", DEFAULT_COL_CODE_WIDTH);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_set_visible(priv->cols[COL_CODE], show_all_cols);

    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", DO_PRODUCT_MODEL_COL_CODE);
    gtk_tree_view_column_set_cell_data_func(col, renderer, (GtkTreeCellDataFunc)cell_code, do_view, NULL);

#if defined(MIN_SCREEN) || defined(CASH) || defined(HALL)
    gtk_tree_view_column_set_visible(priv->cols[COL_CODE], FALSE);
#endif
    col = priv->cols[COL_NAME1] = do_tree_view_add_column(do_tree_view, "Name", "Наименование", DEFAULT_COL_NAME_WIDTH);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    //gtk_tree_view_column_add_attribute (col, renderer, "markup", DO_PRODUCT_MODEL_COL_NAME);
    gtk_tree_view_column_set_cell_data_func(col, renderer, (GtkTreeCellDataFunc)cell_name, do_view, NULL);

    col = priv->cols[COL_MANUFACTOR] = do_tree_view_add_column(do_tree_view, "Manu", "Пр.", DEFAULT_COL_MANU_WIDTH);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_visible(priv->cols[COL_MANUFACTOR], show_all_cols);

    gtk_tree_view_column_add_attribute (col, renderer, "text", DO_PRODUCT_MODEL_COL_MANUFACTOR);

    col = priv->cols[COL_PACK] = do_tree_view_add_column(do_tree_view, "Pack", "Фас", DEFAULT_COL_DEFAULT_WIDTH);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", DO_PRODUCT_MODEL_COL_PACK);
    g_object_set (renderer, "xalign", (gfloat)0.5, NULL);

    col = priv->cols[COL_PRICE] = do_tree_view_add_column(do_tree_view, "Price", "Цена", DEFAULT_COL_DEFAULT_WIDTH);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "markup", DO_PRODUCT_MODEL_COL_PRICE);
    g_object_set (renderer, "xalign", (gfloat)0.5, NULL);

    col = priv->cols[COL_STOCK] = do_tree_view_add_column(do_tree_view, "Stock", "Остаток", DEFAULT_COL_DEFAULT_WIDTH);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", DO_PRODUCT_MODEL_COL_STOCK);
    g_object_set (renderer, "xalign", (gfloat)0.5, NULL);

    col = priv->cols[COL_PRICE_SMALL] = do_tree_view_add_column(do_tree_view, "PriceSmall", "Расф.", DEFAULT_COL_DEFAULT_WIDTH);
    gtk_tree_view_column_set_visible(priv->cols[COL_PRICE_SMALL],
#ifdef MIN_SCREEN
    			FALSE
#else
                               (priv->flags & DO_PRODUCT_VIEW_FLAGS_SHOW_UNPACK_PRICE) ==
                               DO_PRODUCT_VIEW_FLAGS_SHOW_UNPACK_PRICE
#endif
                               );
    g_object_set (renderer, "xalign", (gfloat)0.5, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", DO_PRODUCT_MODEL_COL_PRICE_MIN);

    col = priv->cols[COL_STOCK_SMALL] = do_tree_view_add_column(do_tree_view, "StockSmall", "Остаток", DEFAULT_COL_DEFAULT_WIDTH);
    gtk_tree_view_column_set_visible(priv->cols[COL_STOCK_SMALL],
#ifdef SMALLSTOCK_COL
                               (priv->flags & DO_PRODUCT_VIEW_FLAGS_SHOW_UNPACK_PRICE) ==
                               DO_PRODUCT_VIEW_FLAGS_SHOW_UNPACK_PRICE
#else
                                FALSE
#endif
                               );
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", DO_PRODUCT_MODEL_COL_STOCK_MIN);
    g_object_set (renderer, "xalign", (gfloat)0.5, NULL);

    /*col = priv->cols[COL_LIST] = do_tree_view_add_column(do_tree_view, "List", "Признак", DEFAULT_COL_DEFAULT_WIDTH);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    //gtk_tree_view_column_add_attribute (col, renderer, "text", DO_PRODUCT_MODEL_COL_LIST_CODE);
    gtk_tree_view_column_set_cell_data_func(col, renderer, cell_list, do_view, NULL);*/

    GtkWidget *box;
#if GTK_MAJOR_VERSION == 2
    priv->statusbox = box = gtk_vbox_new(FALSE, 0);
#else

    priv->statusbox = box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(box), 6);
    priv->statusbar = gtk_label_new("");
    gtk_label_set_line_wrap(GTK_LABEL(priv->statusbar), TRUE);
    if ( priv->label_width ) {
        gtk_widget_set_size_request(GTK_WIDGET(priv->statusbar), priv->label_width, -1);
    }
    else
        g_signal_connect(G_OBJECT(priv->statusbar), "size-allocate",
          G_CALLBACK(wrapped_label_size_allocate_callback), object);

    //g_object_set(priv->statusbar, "border-width", 1, NULL);
    //gtk_widget_set_size_request (priv->statusbar, 800, -1);
    gtk_box_pack_start(GTK_BOX(box), priv->statusbar, TRUE, TRUE, 0);
    do_tree_view_pack_footer(do_tree_view, priv->statusbox);



    if ( priv->flags & DO_PRODUCT_VIEW_FLAGS_SHOW_RECOMMEND ) {
        GList *children;
        children = gtk_container_get_children(GTK_CONTAINER(object));
        gint len;
        len = g_list_length(children);
        if ( len ) {
            GtkWidget *view, *notebook;
            view = children->data;
            g_object_ref(view);
            gtk_container_remove(GTK_CONTAINER(object), view);
            priv->notebook = notebook = gtk_notebook_new();
            gtk_box_pack_start(GTK_BOX(object), notebook, TRUE, TRUE, 0);
            gtk_notebook_append_page(GTK_NOTEBOOK(notebook), view, NULL);
#ifdef ADS_RECOMMEND
            priv->recommend_widget = do_product_view_make_recommend(DO_PRODUCT_VIEW(object));
            //!!g_object_unref(view);
            gtk_notebook_append_page(GTK_NOTEBOOK(notebook), priv->recommend_widget, NULL);
            gtk_widget_show_all(GTK_WIDGET(priv->recommend_widget));
#endif
            gtk_widget_show(GTK_WIDGET(notebook));
#ifdef ADS_RECOMMEND
            if ( !priv->recommend_empty )
                gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), 1);
#endif
            gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
            gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
        }
    }
#if GTK_MAJOR_VERSION == 2
    box = gtk_hbox_new(FALSE, 6);
#else

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
#endif
    GtkWidget *entry;

    gtk_container_set_border_width (GTK_CONTAINER(box), 10);
    priv->filter_entry = entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), info_labels[0]); // Поиск
    //!!gtk_widget_set_sensitive(priv->filter_entry, FALSE); //!!!

#ifdef DEPRICATED_1
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "gtk-find");
#endif
    gtk_box_pack_start(GTK_BOX(box), priv->filter_entry, TRUE, TRUE, 0);

    priv->filter_info = gtk_label_new(info_labels[1]);
    gtk_box_pack_start(GTK_BOX(box), priv->filter_info, FALSE, FALSE, 0);

    do_tree_view_pack_header(DO_TREE_VIEW(object), box);
    gtk_widget_show(box);
    gtk_widget_show(entry);

    g_signal_connect(priv->filter_entry, "changed", G_CALLBACK (do_product_view_filter_entry_changed), do_view);
    g_signal_connect(priv->filter_entry, "key-press-event", G_CALLBACK (do_product_view_filter_entry_key_press), do_view);
    g_signal_connect(priv->filter_entry, "activate", G_CALLBACK (do_product_view_filter_entry_activate), do_view);
    g_signal_connect(priv->filter_entry, "focus-in-event", G_CALLBACK (do_product_view_filter_entry_focus_in), do_view);
    g_signal_connect(priv->filter_entry, "focus-out-event", G_CALLBACK (do_product_view_filter_entry_focus_out), do_view);
#if GTK_MAJOR_VERSION > 2
    g_signal_connect(priv->filter_entry, "icon-press", G_CALLBACK (do_product_view_filter_icon_press), do_view);
#endif
#ifdef ADS_RECOMMEND
    priv->rand = g_rand_new();
    priv->messages = get_info_messages(priv->alias_name);
#endif

    priv->re_word = g_regex_new(RE_WORD,G_REGEX_CASELESS,0,NULL);
    priv->re_digit = g_regex_new(RE_DIGIT,G_REGEX_CASELESS,0,NULL);
    priv->re_number = g_regex_new(RE_NUMBER,G_REGEX_CASELESS,0,NULL);
    priv->lexs = do_string_list_new();
    priv->clock = gtk_label_new("");
    do_product_view_set_clock(DO_PRODUCT_VIEW(object));
    priv->clock_event_source = g_timeout_add(500, (GSourceFunc)do_product_view_set_clock, object);
    if ( priv->flags & DO_PRODUCT_VIEW_FLAGS_SHOW_CLOCK ) {
        gtk_box_pack_start(GTK_BOX(box), priv->clock, FALSE, FALSE, 0);
        gtk_widget_show(priv->clock);
    }
    //g_object_set(tree_view, "enable-search", FALSE, NULL);
    gtk_tree_view_set_search_entry(tree_view, GTK_ENTRY(gtk_entry_new())); // fix me
    gtk_widget_show(GTK_WIDGET(object));
    return object;
}
static gboolean do_product_view_set_clock(DoProductView *window)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(window);
    if ( !priv->clock )
        return FALSE;

    struct tm tm;
    time_t t;
    char buf[256];
    t = time(NULL);
    tm = *localtime(&t);
#ifdef _WIN32
    strftime(buf, sizeof(buf), "%d.%m.%y %H:%M", &tm);
#else
    strftime(buf, sizeof(buf), "%A, %d %b., %H:%M", &tm);
#endif // _WIN32

    gtk_label_set_label(GTK_LABEL(priv->clock), buf);

    return TRUE;
}
static void do_product_view_finalize (GObject *object)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (object);

    if (G_UNLIKELY (priv->clock_event_source))
        g_source_remove (priv->clock_event_source);
    if ( priv->search_flush_timeout ) {
        g_source_remove(priv->search_flush_timeout);
        priv->search_flush_timeout = 0;
    }
    if ( priv->filter_flush_timeout ) {
        g_source_remove(priv->filter_flush_timeout);
        priv->filter_flush_timeout = 0;
    }
#ifdef ADS_RECOMMEND
    if ( priv->recommend_flush_timeout ) {
        g_source_remove(priv->recommend_flush_timeout);
        priv->recommend_flush_timeout = 0;
    }
#endif
    if ( priv->filter_show_timeout ) {
        g_source_remove(priv->filter_show_timeout);
        priv->filter_show_timeout = 0;
    }
    g_regex_unref (priv->re_word);
    g_regex_unref (priv->re_digit);
    g_regex_unref (priv->re_number);
    do_string_list_free(priv->lexs);
    //do_string_list_free(priv->filter_completion);
    if ( priv->filter_text ) g_free(priv->filter_text);

    if ( priv->alias_name )
    	g_free(priv->alias_name);
    if ( priv->title )
    	g_free(priv->title);
    g_object_unref(priv->context_received);
    g_object_unref(priv->model);
    if ( priv->alias )
        do_alias_free(priv->alias);
#ifdef ADS_RECOMMEND
    if ( priv->recommend_master )
        do_string_list_free(priv->recommend_master);
#endif
    G_OBJECT_CLASS (do_product_view_parent_class)->finalize (object);
}
/*static void do_product_view_destroy (GtkObject *object)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (object);
    if ( priv->search_flush_timeout ) {
        gtk_timeout_remove(priv->search_flush_timeout);
        priv->search_flush_timeout = 0;
    }
}
*/
static void do_product_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (object);

    switch (prop_id)
    {
    	case PROP_FLAGS:
    		g_value_set_flags (value, priv->flags);
    		break;
    	case PROP_ALIAS:
            g_value_set_string (value, priv->alias_name);
    		break;
    	case PROP_UNITS:
            g_value_set_string (value, priv->unit_str);
    		break;
        case PROP_LOAD_STATUS:
            g_value_set_boolean(value, view_get_load_status(DO_VIEW(object)));
            break;
        case PROP_LOAD_PROGRESS: {
            g_value_set_int(value, view_get_load_progress(DO_VIEW(object)));
            break;
        }
        case PROP_RECEIVER_OBJECT:
            g_value_set_pointer(value, priv->receiver);
            break;
    }

    g_assert_not_reached ();
}

static void do_product_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (object);

    switch (prop_id)
    {
    	case PROP_FLAGS:
    		priv->flags = g_value_get_flags (value);
    		update_flags(DO_PRODUCT_VIEW(object));
    		break;
    	case PROP_ALIAS:
            if (!priv->alias_name) {
                priv->alias_name = g_value_dup_string (value);
                priv->alias = domino_alias_new(priv->alias_name);
                if ( !priv->alias ) {
                    gtk_widget_destroy(GTK_WIDGET(object));
                    break;
                }
                do_alias_open(priv->alias, DOMINO_ORDER_DBS);
#ifdef ADS_RECOMMEND
                priv->alias_recommend = domino_alias_new(priv->alias_name);
                if ( !priv->alias_recommend ) {
                    gtk_widget_destroy(GTK_WIDGET(object));
                    break;
                }
                do_alias_open(priv->alias_recommend, DOMINO_ORDER_DBS);
#endif
            }
    		break;
    	case PROP_UNITS:
            priv->unit_str = g_value_dup_string(value);
            priv->units = do_string_list_new_from_csv(priv->unit_str);
    		break;
        case PROP_RECEIVER_OBJECT:
            priv->receiver = g_value_get_pointer(value);
            if (priv->receiver == (void*)object) {
#ifdef LOAD_MARKED_MANUAL
                do_product_view_load_marked(DO_PRODUCT_VIEW(object));
#endif
                g_signal_connect (priv->model, "refresh-record", G_CALLBACK (do_product_view_refresh_marked), object);
            }
            break;
    }
}

static void do_product_view_class_init (DoProductViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

    o_class->constructor  = do_product_view_constructor;
    o_class->finalize     = do_product_view_finalize;
    o_class->get_property = do_product_view_get_property;
    o_class->set_property = do_product_view_set_property;

    g_type_class_add_private (o_class, sizeof (DoProductViewPrivate));

    g_object_class_install_property
        (o_class,
         PROP_RECEIVER_OBJECT,
         g_param_spec_pointer("receiver-object",
                      "receiver-object",
                      "receiver-object",
                       G_PARAM_READWRITE));
    g_object_class_install_property
        (o_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property
        (o_class,
         PROP_UNITS,
         g_param_spec_string("units",
                      "Подразделения",
                      "Подразделения просмотра цен и остатков",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (o_class,
    			   PROP_LOAD_STATUS,
    			   g_param_spec_boolean("load-status",
    						"Статус загрузки",
    						"Справочник загружается",
    						FALSE,
    						G_PARAM_READABLE));
    g_object_class_install_property (o_class,
    			   PROP_LOAD_PROGRESS,
    			   g_param_spec_int("load-progress",
    						"Статус загрузки",
    						"Справочник загружается",
    						0,100,0,
    						G_PARAM_READABLE));
    g_object_class_install_property
    	(o_class,
    	 PROP_FLAGS,
    	 g_param_spec_flags ("flags",
    			     "flags",
    			     "flags",
    			     DO_TYPE_PRODUCT_VIEW_FLAGS,
    			     DO_PRODUCT_VIEW_FLAGS_DEFAULT,
    			     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
    			     G_PARAM_CONSTRUCT_ONLY));

}

GtkWidget *do_product_view_new (const gchar *alias_name, const gchar *unit, DoProductViewFlags flags)
{
    return g_object_new (DO_TYPE_PRODUCT_VIEW,
    		     "flags", flags,
    		     "alias-name", alias_name,
    		     "units", unit,
    		     "object-root-path", OBJECT_ROOT_PATH,
    		     (gpointer) NULL);
}

static void cell_received(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, DoProductView *view)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, DO_PRODUCT_MODEL_COL_MARKED, &value);
#if GTK_MAJOR_VERSION == 2
    switch (g_value_get_int(&value)) {
        case DOMINO_MARKED:
            g_object_set(cell, "icon-name", GTK_STOCK_APPLY, NULL);
            break;
        case DOMINO_MARKED_AND_ACCEPT:
            g_object_set(cell, "icon-name", GTK_STOCK_OK, NULL);
            break;
        case DOMINO_MARKED_AND_REJECT:
            g_object_set(cell, "icon-name", GTK_STOCK_CANCEL, NULL);
            break;
        default:
            g_object_set(cell, "icon-name", NULL, NULL);
    }
#else
    switch (g_value_get_int(&value)) {
        case DOMINO_MARKED:
            g_object_set(cell, "icon-name", "object-select-symbolic", NULL);
            break;
        case DOMINO_MARKED_AND_ACCEPT:
            g_object_set(cell, "icon-name", "mail-send-receive-symbolic", NULL);
            break;
        case DOMINO_MARKED_AND_REJECT:
            g_object_set(cell, "icon-name", "edit-delete-symbolic", NULL);
            break;
        default:
            g_object_set(cell, "icon-name", NULL, NULL);
    }
#endif
}

static gchar *selected_text(GtkCellRenderer *cell, GtkWidget *view, gchar *text, guint selected_len)
{
#if GTK_MAJOR_VERSION > 2
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);
    GdkRGBA color_bg, color_fg;
    GtkStyleContext *context;
    context = gtk_widget_get_style_context(GTK_WIDGET(priv->tree_view));
    gtk_style_context_get_background_color(GTK_STYLE_CONTEXT(context), GTK_STATE_FLAG_NORMAL,&color_bg);
    gtk_style_context_get_color(GTK_STYLE_CONTEXT(context), GTK_STATE_FLAG_NORMAL,&color_fg);

    gint color_bg_red,color_bg_green,color_bg_blue;
    gint color_fg_red,color_fg_green,color_fg_blue;
    color_bg_red=color_bg.red*255;color_bg_green=color_bg.green*255;color_bg_blue=color_bg.blue*255;
    color_fg_red=color_fg.red*255;color_fg_green=color_fg.green*255;color_fg_blue=color_fg.blue*255;

#else
    GtkStyle *style = gtk_widget_get_style(GTK_WIDGET(view));
#endif
    int i;
    gchar *selected_text = g_strdup(text);
    gchar *tail;

    for (i = 0, tail = text; tail && *tail !='\0' && i < selected_len;
        tail = (gchar*)g_utf8_next_char(tail), i++);

    selected_text[tail - text] = '\0';
    gchar *markup;
#if GTK_MAJOR_VERSION == 2
    if ( !style )
        markup = g_markup_printf_escaped("<span background=\"white\" foreground=\"black\">%s</span>%s",
                             selected_text, tail ? tail : "");
    else
#endif
#if GTK_MAJOR_VERSION >2
        markup = g_markup_printf_escaped("<span background=\"#%2.2hX%2.2hX%2.2hX\" foreground=\"#%2.2hX%2.2hX%2.2hX\">%s</span>%s",
                             color_bg_red,color_bg_green,color_bg_blue,
                             color_fg_red,color_fg_green,color_fg_blue,
                             selected_text, tail ? tail : "");
#else
        markup = g_markup_printf_escaped("<span background=\"#%2.2hX%2.2hX%2.2hX\" foreground=\"#%2.2hX%2.2hX%2.2hX\">%s</span>%s",
                             style->base[GTK_STATE_NORMAL].red,style->base[GTK_STATE_NORMAL].green,style->base[GTK_STATE_NORMAL].blue,
                             style->fg[GTK_STATE_NORMAL].red,style->fg[GTK_STATE_NORMAL].green,style->fg[GTK_STATE_NORMAL].blue,
                             selected_text, tail ? tail : "");
#endif
    g_free(selected_text);
    return markup;

}
/*
static gboolean is_selected(GtkTreeSelection *sel, GtkTreePath *iter)
{
    gboolean retval = FALSE;
    GList *list = gtk_tree_selection_get_selected_rows(sel, NULL);
    if (list) {
        GList *l;
        for ( l = list; l; l=l->next ) {
            GtkTreePath *s;
            s = l->data;
            if ( !gtk_tree_path_compare(s, iter) ) {
                retval = TRUE;
                break;
            }
        }
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    return retval;

}*/
static void cell_name(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, DoProductView *view)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, DO_PRODUCT_MODEL_COL_NAME, &value);
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);

    //do_log(LOG_INFO)
    char *text = (char*)g_value_get_string(&value);
    if (text && text[0]) {
        GValue ads = {0,};
        gtk_tree_model_get_value(tree_model, iter, DO_PRODUCT_MODEL_COL_ADS, &ads);
        gchar *buf = do_product_name_format(text);
        gchar *markup = NULL;
        //gchar *sad = g_strdup_printf("<b>%s</b>", buf);
        g_object_set(cell, "markup", "", NULL);
        g_object_set(cell, "text", buf, NULL);
        g_object_set(cell, "background", NULL, NULL);
        //g_object_set(cell, "markup", sad, NULL);
        GtkTreePath *path, *path1;
        path = gtk_tree_model_get_path(tree_model, iter);
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path1, NULL);
        if (path1 && path && priv->search_item == DO_PRODUCT_VIEW_SEARCH_ITEM_NAME && priv->search_char_count &&
            !gtk_tree_path_compare(path, path1) ) {
            //do_log(LOG_WARNING, "%d %d %s",gtk_tree_path_get_indices(path)[0],gtk_tree_path_get_indices(path1)[0],buf);
            markup = selected_text(cell,GTK_WIDGET(view), buf, priv->search_char_count);
        }
        else {
            markup = buf;
            buf = g_markup_printf_escaped("%s",markup);
            do_free(markup);
            markup = buf;
            int i;
            for ( i = 0; i < priv->lexs->count; i++ ) {
                gchar *head;
                gchar *p = markup;
                gunichar out,in;
                gchar *ch;

                while ( p && *p != '\0' ) {
                    head = p;
                    ch = priv->lexs->list[i];
                    for (; p && *p !='\0' && ch && *ch != '\0'; p = (gchar*)g_utf8_next_char(p), ch = (gchar*)g_utf8_next_char(ch)) {
                        out = g_utf8_get_char(p);
                        out = g_unichar_tolower (out);
                        in = g_utf8_get_char(ch);
                        in = g_unichar_tolower (in);
                        if ( out != in ) break;
                    }
                    if ( !ch || *ch == '\0' ) {
                        if ( i ) {
                            p+=1;
                            p-=1;
                        }
                        buf = g_malloc0((head-markup)+strlen("<b>")+(p-head)+strlen("</b>")+strlen(markup));
                        strncpy(buf,markup,head-markup);
                        strncpy(buf+(head-markup),"<b>",strlen("<b>"));
                        strncpy(buf+(head-markup)+strlen("<b>"),head,p-head);
                        strncpy(buf+(head-markup)+strlen("<b>")+(p-head),"</b>",strlen("</b>"));
                        strcpy(buf+(head-markup)+strlen("<b>")+(p-head)+strlen("</b>"), p);
                        p = buf + (head-markup)+strlen("<b>")+(p-head)+strlen("</b>");
                        g_free(markup);

                        markup = buf;

                    }
                    else
                        p = (gchar*)g_utf8_next_char(p);
                }
            }
        }
#ifdef ADS_RECOMMEND
        if ( g_value_get_string(&ads)[0] ) {
            if (g_value_get_string(&ads)[0] != '!' ) {
            //    g_object_set(cell, "background", "green", NULL);
                char *buf;
                buf = g_strdup_printf("<b>%s</b>",markup);
                g_free(markup);
                markup = buf;
            }
            else
            {
                char *buf;
                buf = g_strdup_printf("<i>%s</i>",markup);
                g_free(markup);
                markup = buf;
                //g_object_set(cell, "background", "gray", NULL);
            }
        }
#endif
        g_object_set(cell, "markup", markup, NULL);
        g_free(markup);

        if ( path ) gtk_tree_path_free(path);
    }
}
/*
static void cell_list(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
    GValue value = { 0, };
    GValue value_name = { 0, };
    gtk_tree_model_get_value(tree_model, iter, DO_PRODUCT_MODEL_COL_LIST_CODE, &value);
    gtk_tree_model_get_value(tree_model, iter, DO_PRODUCT_MODEL_COL_LIST_NAME, &value_name);
    //DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (data);

    gint list= g_value_get_int(&value);
    const gchar *text = g_value_get_string(&value_name);
    if ( list ) {
        gtk_object_set(GTK_OBJECT(cell), "text", text, "foreground", "green", NULL);
    }
    else
        gtk_object_set(GTK_OBJECT(cell), "text", "", "foreground", NULL, NULL);
}
*/
static void cell_code(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, DoProductView *view)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, DO_PRODUCT_MODEL_COL_CODE, &value);
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);

    char *text = (char*)g_value_get_string(&value);
    if (text && text[0]) {
        g_object_set(cell, "markup", "", NULL);
        g_object_set(cell, "text",text, NULL);
        GtkTreePath *path, *path1;
        path = gtk_tree_model_get_path(tree_model, iter);
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path1, NULL);

        if (path1 && path && priv->search_item == DO_PRODUCT_VIEW_SEARCH_ITEM_CODE && priv->search_char_count &&
            !gtk_tree_path_compare( path, path1)
            )
            selected_text(cell,GTK_WIDGET(view), text, priv->search_char_count);
        gtk_tree_path_free(path);
    }
}
static gboolean search_flush_timeout(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    search_clear(do_view);
    priv->search_flush_timeout = 0;
    return FALSE;
}
static gboolean filter_show_timeout(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    priv->filter_show_timeout = 0;
    if ( do_product_model_is_filtered(DO_PRODUCT_MODEL(priv->model)) ) {
        gchar *code;
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
        //do_log(LOG_INFO, "focused %d", gtk_widget_is_focus(GTK_WIDGET(priv->filter_entry)));
        if ( gtk_widget_is_focus(GTK_WIDGET(priv->filter_entry)) ) {
            do_tree_view_grab_focus(DO_TREE_VIEW(do_view));
    }
    }
    return FALSE;
}
static void do_product_view_filter_set_text(DoProductView *do_view, const gchar *text)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    search_clear(do_view);
    GtkTreePath *path;
    path = gtk_tree_path_new_first();
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
    gtk_entry_set_text(GTK_ENTRY(priv->filter_entry), text);

    gtk_widget_grab_focus(priv->filter_entry);
    gtk_editable_set_position(GTK_EDITABLE(priv->filter_entry), strlen(text));
//    search_clear(do_view);
    do_product_view_filter_apply(DO_PRODUCT_VIEW(do_view));
    gtk_tree_path_free(path);
}
static const gchar *do_product_view_filter_get_text(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    const gchar *text;
    int i;
    text = gtk_entry_get_text(GTK_ENTRY(priv->filter_entry));
    for ( i = 0; i < G_N_ELEMENTS(info_labels); i++ ) {
        if ( !strcmp(text, info_labels[i]) ) {
            gtk_entry_set_text(GTK_ENTRY(priv->filter_entry),"");
            return NULL;
        }
    }
    if ( strlen(text) )
        return text;
    return NULL;

}

static gboolean do_view_key_press(GtkWidget *widget, GdkEventKey *event, DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    gchar *str;
#ifdef ADS_RECOMMEND
    recommend_refresh_timeout(do_view);
#endif
    if ((str = search_get_text(event->string, event->length, priv->search_char_count)) != NULL) {
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
        g_object_notify (G_OBJECT (do_view), "load-status");
        g_free(str);
        return TRUE;
    }
    if ( priv->filter_show_timeout ) {
        g_source_remove(priv->filter_show_timeout);
        priv->filter_show_timeout = 0;
    }
    if ( do_product_model_is_filtered(DO_PRODUCT_MODEL(priv->model)) )
        priv->filter_show_timeout =
            gdk_threads_add_timeout (DO_PRODUCT_VIEW_FILTER_SHOW_TIMEOUT,
                (GSourceFunc)filter_show_timeout, do_view);

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
                if (!priv->search_char_count &&
                     priv->receiver) {
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
                return TRUE;
            case GDK_KEY_Up:
            case GDK_KEY_Down:
            case GDK_KEY_Page_Up:
            case GDK_KEY_Page_Down:
                search_clear(do_view);
                break;
            case GDK_KEY_Return:
            case GDK_KEY_KP_Enter:
            case GDK_KEY_ISO_Enter:
                if (priv->search_item == DO_PRODUCT_VIEW_SEARCH_ITEM_BCODE) {
                    search_find_by_bcode(do_view, priv->search_text);
                    search_clear(do_view);
                    return TRUE;
                }
                search_clear(do_view);
                break;
            case GDK_KEY_Right: {
                const gchar *text = do_product_view_filter_get_text(do_view);
                char *buf = g_strdup(text ? text : priv->search_text);
                do_product_view_filter_set_text(do_view, buf);
                g_free(buf);
                return TRUE;
                break;
            }
            case GDK_KEY_Left:
                if (priv->search_char_count >0 && priv->search_item == DO_PRODUCT_VIEW_SEARCH_ITEM_NAME) {
                    search_clear_to_begin_word(do_view);
                    return TRUE;
                }
                break;
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
    return FALSE;
}
/*
static gboolean do_view_key_release(GtkWidget *widget, GdkEventKey *event, DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    gchar *str;
    if ((str = search_get_text(event->string, event->length, priv->search_char_count)) != NULL) {
        search_add(do_view, str);
        g_object_notify (G_OBJECT (do_view), "load-status");
        g_free(str);
        return TRUE;
    }
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
                if (!priv->search_char_count &&
                     priv->receiver) {
                    GtkNotebook *nb;
                    nb = GTK_NOTEBOOK (do_window_get_notebook (
                        DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(do_view)))));
                    do_view_do_grab_focus(DO_VIEW(priv->receiver));
                    if ( gtk_notebook_page_num(nb, GTK_WIDGET(do_view)) !=
                         gtk_notebook_page_num(nb, GTK_WIDGET(priv->receiver) )
                        ) {
                        gtk_notebook_set_current_page(nb, gtk_notebook_page_num(nb, GTK_WIDGET(priv->receiver)));
    //do_log(LOG_ERR, "release");
    return FALSE;
}
*/
static gboolean do_view_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    search_clear(data);
#ifdef ADS_RECOMMEND
    recommend_refresh_timeout(data);
#endif
    return  FALSE;//do_product_view_button_pressed(widget, event, data);
}
static gboolean search_add(DoProductView *do_view, gchar *string)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    gboolean res = FALSE;
    int len;
    gchar *p;
    for (len = 0, p = string; p && *p != '\0'; p = (gchar*)g_utf8_next_char(p), len++);
    gchar *buf = do_malloc(strlen(priv->search_text) + strlen(string) + 1);
    strcpy(buf, priv->search_text);
    strcpy(buf + strlen(priv->search_text), string);
    if (search_find(do_view, buf)) {
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

static gboolean search_find(DoProductView *do_view, const char *text)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    if (!text || !text[0])
        return TRUE;
    if (priv->search_item == DO_PRODUCT_VIEW_SEARCH_ITEM_NAME) {
        gchar *crnt_text;
        if ((crnt_text = get_name_from_cursor(do_view)) != NULL) {
            if ((strlen(crnt_text) >= strlen(text)) &&
                (!strncmp(crnt_text, text, strlen(text)))) {
                    g_free(crnt_text);
                    gtk_tree_view_redraw(GTK_TREE_VIEW(priv->tree_view));
                    return TRUE;
            }
            g_free(crnt_text);
        }
    }
    gint indx;
    gboolean result = FALSE;
    if (do_product_model_find_record_by_name(DO_PRODUCT_MODEL(priv->model), text, &indx)) {
        result = TRUE;
        priv->search_item = DO_PRODUCT_VIEW_SEARCH_ITEM_NAME;
    }
    else if (is_code(text) && do_product_model_find_record_by_code(DO_PRODUCT_MODEL(priv->model), text, &indx)) {
        result = TRUE;
        priv->search_item = DO_PRODUCT_VIEW_SEARCH_ITEM_CODE;
    }
    else if (is_bcode(text)) {
        result = TRUE;
        indx = -1;
        priv->search_item = DO_PRODUCT_VIEW_SEARCH_ITEM_BCODE;
        do_view_actions_refresh();
    }

    if (result && indx != -1) {
        GtkTreePath *path;
        path = gtk_tree_path_new();
        gtk_tree_path_append_index(path, indx);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(priv->tree_view), path, NULL,  TRUE, 0.5, 0.0);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(priv->tree_view), path, NULL,  TRUE, 0.5, 0.0);
    }
    return result;
}

static gboolean start_find(GtkTreeView *treeview, gpointer data)
{
    //return TRUE;
    gboolean sad;
    g_object_get(treeview, "enable-search", &sad, NULL);
    printf("sad %d\n",sad);
    return FALSE;
}

static gboolean search_clear(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    gboolean re;
    re=priv->search_char_count > 0;
    priv->search_char_count = 0;
    priv->search_text[0] = '\0';
    priv->search_item = DO_PRODUCT_VIEW_SEARCH_ITEM_NAME;
    if ( re ) {
        gtk_tree_view_redraw(GTK_TREE_VIEW(priv->tree_view));

        if (priv->search_flush_timeout) {
            g_source_remove(priv->search_flush_timeout);
            priv->search_flush_timeout = 0;
        }
        do_view_actions_refresh();
    }
    return re;
}
static void search_back(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    if (priv->search_char_count) {
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
static void search_clear_to_begin_word(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    gchar *text = get_name_from_cursor(do_view);
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
static void search_refresh_timeout(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    if (priv->search_flush_timeout)
        g_source_remove(priv->search_flush_timeout);

    priv->search_flush_timeout =
        gdk_threads_add_timeout (DO_PRODUCT_VIEW_SEARCH_DIALOG_TIMEOUT,
    	   (GSourceFunc)search_flush_timeout, do_view);
}
#ifdef ADS_RECOMMEND
static void recommend_refresh_timeout(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    if (priv->recommend_flush_timeout)
        g_source_remove(priv->recommend_flush_timeout);

    priv->recommend_flush_timeout =
        gdk_threads_add_timeout (DO_PRODUCT_VIEW_RECOMMEND_TIMEOUT,
    	   (GSourceFunc)recommend_timeout, do_view);
}
#endif
static gchar *get_name_from_cursor(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        return NULL;
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {

        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_PRODUCT_MODEL_COL_NAME, &value);
        gtk_tree_path_free(path);
        if (!g_value_get_string(&value))
            return NULL;
        return g_strdup(g_value_get_string(&value));
    }
    gtk_tree_path_free(path);
    return NULL;
}

static gchar *get_code_from_cursor(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        return NULL;
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {

        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_PRODUCT_MODEL_COL_CODE, &value);
        gtk_tree_path_free(path);
        if (!g_value_get_string(&value))
            return NULL;
        return g_strdup(g_value_get_string(&value));
    }
    gtk_tree_path_free(path);
    return NULL;
}

static gchar *get_code_from_path(DoProductView *view, GtkTreePath *path)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {
        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_PRODUCT_MODEL_COL_CODE, &value);
        if (!g_value_get_string(&value))
            return NULL;
        return g_strdup(g_value_get_string(&value));
    }
    return NULL;
}

static gint get_marked_from_path(DoProductView *view, GtkTreePath *path)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {
        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_PRODUCT_MODEL_COL_MARKED, &value);
        return g_value_get_int(&value);
    }
    return DOMINO_MARKED_UNMARK;
}

static gboolean search_find_by_bcode(DoProductView *do_view, const char *text)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    gint indx;

    if (do_product_model_find_record_by_bcode(DO_PRODUCT_MODEL(priv->model), text, &indx)) {
        GtkTreePath *path;
        path = gtk_tree_path_new();
        gtk_tree_path_append_index(path, indx);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(priv->tree_view), path, NULL,  TRUE, 0.5, 0.0);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(priv->tree_view), path, NULL,  TRUE, 0.5, 0.0);
        return TRUE;
    }
    else {
        DOMINO_SHOW_INFO("Товар с продажный кодом '%s' не найден", text);
        return FALSE;
    }
}
// Drag and Drop
static void drag_begin(GtkWidget *widget, GdkDragContext *drag_context, DoProductView *do_view)
{

    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    GtkTreePath *path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (path) {
        g_object_set_data_full (G_OBJECT (drag_context),
                                          "gtk-tree-view-source-row",
                                          gtk_tree_row_reference_new(GTK_TREE_MODEL(priv->model), path),
                                          (GDestroyNotify) (gtk_tree_row_reference_free));

        gtk_tree_path_free(path);
        //GdkPixmap *pixmap;

        /*!!pixmap = view_create_drag_pixmap (GTK_WIDGET(priv->tree_view), do_view);
        if (pixmap != NULL)
        {
            gtk_drag_set_icon_pixmap (drag_context,
                          gdk_drawable_get_colormap (pixmap),
                          pixmap, NULL, -2, -2);
            g_object_unref (pixmap);
        }*/
    }
}

static void drag_data_get(GtkWidget *widget, GdkDragContext *drag_context, GtkSelectionData *data, guint info, guint time, DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);

    GtkTreeRowReference *ref =
    g_object_get_data(G_OBJECT (drag_context), "gtk-tree-view-source-row");

    if (ref) {
        GtkTreePath *path;
        path = gtk_tree_row_reference_get_path(ref);
        if (path) {
            GtkTreeIter iter;
            if (gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->model), &iter, path)) {
                GValue value = { 0, };
                gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_PRODUCT_MODEL_COL_CODE, &value);
                if (g_value_get_string(&value)) {
                    gchar *text = do_strdup((char*)g_value_get_string(&value));
                    gtk_selection_data_set(data, gdk_atom_intern_static_string(DO_DND_PRODUCT_TYPE), 8, (const guchar*)text, strlen(text));
                    do_free(text);
                }
            }
        }

    }
}
/*!!
static GdkDrawable *view_create_drag_pixmap(GtkWidget *widget, DoProductView *do_view)
{
    //DoKeyboardViewPrivate *priv = view->priv;
    gchar *text;
    GdkDrawable *drawable;
    PangoContext *context;
    PangoLayout  *layout;

    PangoFontMetrics *metrics;
    int pixmap_height, pixmap_width;
    int layout_width, layout_height;
    int offset_x = 0;
    int char_width;

    text = get_name_from_cursor(do_view);

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
static void    view_do_delete(DoView *view)
{

}
static gboolean    view_can_do_delete(DoView *view)
{
    return FALSE;
}
static void        view_do_insert(DoView *view)
{
}
static gboolean    view_can_do_insert(DoView *view)
{
    return TRUE;
}
static void    	view_do_copy(DoView *view)
{
}
static gboolean    view_can_do_copy(DoView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    gboolean retval = FALSE;
    GtkTreePath *path = NULL;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if ( path ) {
        gtk_tree_path_free(path);
    }
    return retval;
}

static void    view_do_popup(DoView *view, GdkEventButton* event)
{
    GtkWidget * menu = do_view_actions_action_get_widget("/" DO_PRODUCT_VIEW_POPUP);
    do_tree_view_popup(DO_TREE_VIEW(view), event, menu);
}

static void edit_product(DoProductView *view, DoDialogAction action, const gchar *code, const gchar *tab)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);

    DoContext *context = do_context_new();
    do_context_set_kind(context, DO_CONTEXT_KIND_PRODUCT);
    if (code)
        g_object_set(context, "product-code", code, NULL);

    DoView *edit = (DoView*)do_product_edit_new(DO_VIEW(view), priv->alias_name,
                                            DO_PRODUCT_EDIT_FLAGS_READ_ONLY,
                                               action, context, priv->units->list[0]);

    do_notebook_add_tab(DO_NOTEBOOK(do_window_get_notebook(
                                     DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view))))),
                        edit, -1, TRUE);
    g_object_set(edit, "view", view, "tab", tab, NULL);
//    do_view_do_grab_focus(edit);
}
static void    view_do_edit(DoView *view, const gchar *tab)
{
    gchar *code;
    code = get_code_from_cursor(DO_PRODUCT_VIEW(view));
    edit_product(DO_PRODUCT_VIEW(view), DO_DIALOG_ACTION_EDIT, code, tab);
    g_free(code);
}

static gboolean    view_can_do_edit(DoView *view, const gchar *tab)
{
    gboolean retval = FALSE;
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    if (priv->search_item != DO_PRODUCT_VIEW_SEARCH_ITEM_BCODE) {
        GtkTreePath *path;
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
        if ( path ) {
            retval = TRUE;
            gtk_tree_path_free(path);
        }
    }
    return retval;
}
static void     view_do_apply(DoView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    if (priv->receiver) {
        GtkTreePath *path;
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
        if ( path ) {
            DoContext *context = do_context_new_with_attributes(DO_CONTEXT_KIND_PRODUCT, NULL);
            gchar *code = get_code_from_path(DO_PRODUCT_VIEW(view), path);
            if (code) {
                   g_object_set(context, "product-code", code, NULL);
                    if ( !do_view_toggle_marked(priv->receiver, context) )
                        do_view_set_marked(priv->receiver, context, DO_MARKED);
                    g_free(code);
            }
            gtk_tree_view_row_redraw(GTK_TREE_VIEW(priv->tree_view), path);
            gtk_tree_path_free(path);
        }
    }
}
static gboolean    view_can_do_apply(DoView *view)
{
    if ( !domino_check_role(DOMINO_ROLE_HANDBOOK | DOMINO_ROLE_OPERATOR | DOMINO_ROLE_MANAGER ) )
        return FALSE;

    gboolean retval = FALSE;
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    if ( priv->receiver ) {
        GtkTreePath *path;
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
        if ( path ) {
            retval = TRUE;
            gtk_tree_path_free(path);
        }
    }
    return retval;
}
static void     view_do_unapply(DoView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    if (priv->receiver) {
        GtkTreePath *path;
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
        if ( path ) {
            DoContext *context = do_context_new_with_attributes(DO_CONTEXT_KIND_PRODUCT, NULL);
            gpointer code = get_code_from_path(DO_PRODUCT_VIEW(view), path);
            if (code) {
                g_object_set(context, "product-code", code, NULL);
                do_view_set_marked(priv->receiver, context, DO_MARKED_UNMARK);
                g_free(code);
            }
            g_object_unref(context);
            //gtk_tree_selection_unselect_all(priv->sel);
            //l = g_list_last(list);
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), path, NULL, FALSE);
            gtk_tree_view_redraw(GTK_TREE_VIEW(priv->tree_view));
            gtk_tree_path_free( path );
        }
    }
}
static gboolean    view_can_do_unapply(DoView *view)
{
    if ( !domino_check_role(DOMINO_ROLE_HANDBOOK | DOMINO_ROLE_OPERATOR |
                               DOMINO_ROLE_MANAGER) )
        return FALSE;

    gboolean retval = FALSE;
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    if (priv->receiver) {
        GtkTreePath *path;
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
        if ( path ) {
            retval = TRUE;
            gtk_tree_path_free(path);
        }
    }
    return retval;
}

gboolean view_do_grab_focus(DoView *view)
{
#ifdef ADS_RECOMMEND
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    if ( !priv->notebook || gtk_notebook_get_current_page(GTK_NOTEBOOK(priv->notebook)) != 1 )
#endif
       return do_tree_view_grab_focus(DO_TREE_VIEW(view));
#ifdef ADS_RECOMMEND
    else {
    GtkTreePath *path = NULL;
    gtk_widget_grab_focus(GTK_WIDGET(priv->recommend));
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->recommend), &path, NULL);

    if ( !path ) {
        path = gtk_tree_path_new_first();
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->recommend), path, NULL, FALSE);
    }
    gtk_tree_path_free(path);

    }
#endif
    return TRUE;
}

static void row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoProductView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    search_clear(DO_PRODUCT_VIEW(view));
    if (priv->flags & DO_PRODUCT_VIEW_FLAGS_ROW_ACTIVATED_TO_SEND) {
        if ( change_marked(view, path) ) {
            //gtk_tree_selection_unselect_all(priv->sel);
            GtkTreePath *next = gtk_tree_path_copy(path);
            gtk_tree_path_next(next);
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), next, NULL, FALSE);
            gtk_tree_path_free(next);
        }
    }
    else
        if (view_can_do_edit(DO_VIEW(view), NULL))
            view_do_edit(DO_VIEW(view), NULL);
}

static gboolean change_marked(DoProductView *do_view, GtkTreePath *path)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    gboolean retval = FALSE;
      if (priv->receiver) {
        gpointer code = get_code_from_path(do_view, path);
        gboolean received = get_marked_from_path(do_view, path);
        if (code) {
            DoContext *context = do_context_new_with_attributes(DO_CONTEXT_KIND_PRODUCT,
                                                                 "product-code", code, NULL);
            retval = do_view_set_marked(priv->receiver, context, received ? DO_MARKED_UNMARK : DO_MARKED);
            g_free(code);
            g_object_unref(context);
        }
      }
      return retval;
}
static void update_flags(DoProductView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    if (priv->cols[COL_RECEIVED])
        gtk_tree_view_column_set_visible(priv->cols[COL_RECEIVED],
                               (priv->flags & DO_PRODUCT_VIEW_FLAGS_SHOW_RECEIVED) ==
                               DO_PRODUCT_VIEW_FLAGS_SHOW_RECEIVED);
}

static void view_do_close(DoView *embed)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(embed);
    if ( priv->receiver )
        if ( !do_view_sender_do_close(priv->receiver) )
            return;
    gtk_widget_destroy(GTK_WIDGET(embed));
}

static const char *view_get_title(DoView *embed)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(embed);
    return priv->title;
}
static GdkPixbuf    *view_get_icon    		(DoView *embed)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(embed);
    return priv->icon;
}
static gboolean      view_get_load_status	(DoView *embed)
{
    gboolean retval;
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(embed);
    g_object_get(priv->model, "load-status", &retval, NULL);
    return retval;
}
static gint      view_get_load_progress	(DoView *embed)
{
    gint retval;
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(embed);
    g_object_get(priv->model, "load-progress", &retval, NULL);
    return retval;
}
static void do_product_view_sync_load_status (DoProductModel *model, GParamSpec *pspec, GtkWidget *proxy)
{
    g_object_notify (G_OBJECT (proxy), "load-status");
}
static void do_product_view_sync_load_progress (DoProductModel *model, GParamSpec *pspec, GtkWidget *proxy)
{
    g_object_notify (G_OBJECT (proxy), "load-progress");
}
static gchar *get_as_csv(DoProductView *view)
{
    DoProductViewPrivate  *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);
    GString *csv = g_string_new(NULL);
    GtkTreePath *path = gtk_tree_path_new();
    GtkTreeIter iter;
    for (gtk_tree_path_append_index(path, 0);
         gtk_tree_model_get_iter(priv->model, &iter, path);
         gtk_tree_path_next(path)) {
        GValue code_v = { 0, };
        GValue name_v = { 0, };
        GValue price_v = { 0, };
        GValue rest_v = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_PRODUCT_MODEL_COL_CODE, &code_v);
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_PRODUCT_MODEL_COL_NAME, &name_v);
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_PRODUCT_MODEL_COL_PRICE, &price_v);
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->model), &iter, DO_PRODUCT_MODEL_COL_STOCK, &rest_v);

        g_string_append_printf(csv,
            "\"%s\",\"%s\",%s,%s\n",
                g_value_get_string(&code_v),
                g_value_get_string(&name_v),
                g_value_get_string(&price_v),
                g_value_get_string(&rest_v));

    }
    gtk_tree_path_free(path);

    return g_string_free(csv, FALSE);

}
static void view_do_save_as(DoView *view)
{
//    DoProductViewPrivate  *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);
    GtkWidget* dialog;
    GtkWidget* window;
    gchar* filename;

    if (!gtk_widget_get_visible(GTK_WIDGET(view)))
        return;
    window = gtk_widget_get_toplevel(GTK_WIDGET(view));

    dialog = gtk_file_chooser_dialog_new (
        "Сохранить как", GTK_WINDOW( window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "Отмена", GTK_RESPONSE_CANCEL,
        "Сохранить", GTK_RESPONSE_ACCEPT,
        NULL);
    gtk_window_set_icon_name (GTK_WINDOW (dialog), "domino");
    gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        gchar *csv = get_as_csv(DO_PRODUCT_VIEW(view));
        if (csv && !do_save_to_file(filename, csv, strlen(csv))) {
            DOMINO_SHOW_ERROR("Файл \"%s\" не сохранен", filename);
        }
        if (csv)
            g_free(csv);
    }
    gtk_widget_destroy (dialog);
}
static gboolean    view_can_do_save_as(DoView *view)
{
    return TRUE;
}

static void view_update_context(DoView *view, DoContext *context)
{
    //!!DoProductViewPrivate  *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);
    if (do_context_get_kind(context) != DO_CONTEXT_KIND_PRODUCT)
        return;
    gchar *product_code;
    g_object_get(context, "product-code", &product_code, NULL);
    //!!if (product_code)
        //!!do_product_model_update_product(DO_PRODUCT_MODEL(priv->model), product_code);
}
static gboolean show_error(DoProductModel *model)
{
    DoProductViewPrivate  *priv = DO_PRODUCT_VIEW_GET_PRIVATE (model);
    gchar *error;
    g_object_get(priv->model, "error", &error, NULL);
    if (error)
        DOMINO_SHOW_ERROR("%s", error);
    return FALSE;
}
static void do_product_view_show_model_error (DoProductModel *model, GParamSpec *pspec, GtkWidget *proxy)
{
     gdk_threads_add_timeout(10, (GSourceFunc)show_error, proxy);
}
/*
static gboolean do_product_view_receive(DoView *view, DoContext *context)
{
    DoProductViewPrivate  *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);
    gchar *product_code;
    const char *unit;
    gboolean retval;

    g_object_get(context, "product-code", &product_code, NULL);

    if ( priv->units->count )
        unit = priv->units->list[0];
    else
        unit = domino_unit();

    retval = domino_action_order_get_marked_list_(priv->alias_name, unit, product_code);

    if ( retval ) {
        GList *list = NULL;

        list = g_list_append(list, g_strdup(product_code));
        do_view_add_received(view, list);
        if (list) {
            g_list_foreach(list, (GFunc)g_free, NULL);
            g_list_free(list);
        }
    }
    return retval;
}
*/
/*
static gboolean do_product_view_receive(DoView *view, DoContext *context)
{
    DoProductViewPrivate  *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);
    GList *list = NULL;
    gchar *product_code;
    do_alias_t *alias;
    gchar *document_number;
    document_order_key2_t document_order_key2;
    document_order_key0_t document_order_key0;
    document_key0_t document_key0;
    document_order_rec_t document_order;
    struct tm tm;
    time_t now;
    gboolean insert;


    g_object_get(context, "product-code", &product_code, NULL);

    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_END)) {
        if (alias)
            do_alias_free(alias);
        return FALSE;
    }
    now = time(NULL);
    tm = *localtime(&now);
    do_text_set(alias, document_order_key2.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_ORDER));
    do_text_set(alias, document_order_key2.sklad, domino_unit());
    document_number = g_strdup_printf("%4.4d%2.2d%2.2d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    do_text_set(alias, document_order_key2.document, document_number);
    g_free(document_number);
    do_text_set(alias, document_order_key2.code, product_code);
    insert = FALSE;
    switch (do_document_order_key2(alias, &document_order_key2, DO_GET_EQUAL)) {
        case DO_ERROR:
            do_alias_free(alias);
            return FALSE;
        case DO_KEY_NOT_FOUND:received_from_request
            insert = TRUE;
            break;
        case DO_OK:
            insert = FALSE;
            break;
    }
    if (insert) {
        do_cpy(document_key0.dtype, document_order_key2.dtype);
        do_cpy(document_key0.sklad, document_order_key2.sklad);
        do_cpy(document_key0.document, document_order_key2.document);

        memcpy(&document_order_key0, &document_key0, sizeof(document_key0));
        document_order_key0.line = 2147483646;

        if (do_document_order_key0(alias, &document_order_key0, DO_GET_LT) == DO_ERROR) {
            do_alias_free(alias);
            return FALSE;
        }
        if (memcmp(&document_order_key0, &document_key0, sizeof(document_key0))) {
            memcpy(&document_order_key0, &document_key0, sizeof(document_key0));
            document_order_key0.line = 0;
        }

        do_cpy(document_order.data.dtype, document_order_key0.dtype);
        do_text_set(alias, document_order.data.dtype1, "1");
        do_cpy(document_order.data.sklad, document_order_key0.sklad);
        do_cpy(document_order.data.document, document_order_key0.document);
        document_order.data.line = document_order_key0.line + 1;
        document_order.data.sort = 0;
        do_text_set(alias, document_order.data.code, product_code);
        do_date_set(&document_order.data.date, tm);
        do_time_set(&document_order.data.time, tm);
        document_order.data.accepted = 0;
        do_set_empty(document_order.data.nquant);
        do_set_empty(document_order.data.nprice);
        document_order.size = sizeof(document_order.data) - sizeof(document_order.data.params);
        if (do_document_order_insert(alias, &document_order) != DO_OK) {
            do_alias_free(alias);
            return FALSE;
        }
    }
    do_alias_free(alias);

    list = g_list_append(list, g_strdup(product_code));

    do_view_add_received(view, list);

    if (list) {
        g_list_foreach(list, (GFunc)g_free, NULL);
        g_list_free(list);
    }
    return TRUE;
}
*/
/*
static void do_product_view_get_diff_date(struct tm *now, struct tm *first)
{
    gchar *day_off;
    int day_dif;
    DOMINO_PROFILE_GET(ORDER_ROOT_PATH, "Дней ожидания", &day_off, NULL);
    if (!day_off || (day_dif = atoi(day_off)) == 0)
        day_dif = DAY_OFF;
    *first = *now;
    do {
        first->tm_mday--;
        mktime(first);
        if (first->tm_wday != 0 && first->tm_wday != 6)
            day_dif--;
    } while (day_dif);
}
*/
#ifdef LOAD_MARKED_MANUAL
static gboolean do_product_view_load_marked(DoProductView *view)
{

    DoProductViewPrivate  *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);
    do_list_t *products;

    if (priv->receiver != (void*)view)
        return FALSE;

    struct tm tm1;
    struct tm tm2;
    time_t now;
    const char *unit;
    int i;


    now = time(NULL);
    tm1 = *localtime(&now);
    tm2 = tm1;
    tm1.tm_mday-= DOMINO_MARKED_INTERVAL;
    mktime(&tm1);

    if ( priv->units->count )
        unit = priv->units->list[0];
    else
        unit = domino_unit();

    do_alias_t *alias;
    alias = domino_alias_new(priv->alias_name);
    if ( !alias )
        return FALSE;
    if ( !do_alias_open(alias, DOMINO_ORDER_DBS) ) {
        do_alias_free(alias);
        return FALSE;
    }

    products = domino_order_get_marked_list(alias, unit, &tm1, &tm2, TRUE);


    if ( !products ) {
        do_alias_free(alias);
        return FALSE;
    }

    for ( i = 0; i < products->count ; i++ ) {
        domino_order_marked_list_item_t *item;
        item = products->list[i];

        //if ( !do_date_cmp(&tm2,&item->tm) ||
        if ( !domino_order_after_traffic_in(alias, unit, item->code, &item->tm) )
           do_product_model_set_marked(DO_PRODUCT_MODEL(priv->model), item->code, item->marked);
    }
    domino_order_marked_list_free(products);
    do_alias_free(alias);
    return TRUE;
}
#endif
/*
static gboolean do_product_view_load_received(DoProductView *view)
{

    DoProductViewPrivate  *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);
    GList *list = NULL;

    do_alias_t *alias;
    gchar *document_number;
    document_order_key2_t document_order_key2;
    document_key0_t document_key0;
    struct tm tm;
    struct tm first;
    time_t now;
    int status;

    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_END)) {
        if (alias)
            do_alias_free(alias);
        return FALSE;
    }
    now = time(NULL);
    tm = *localtime(&now);
    do_product_view_get_diff_date(&tm, &first);

    do_text_set(alias, document_order_key2.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_ORDER));
    do_text_set(alias, document_order_key2.sklad, domino_unit());
    document_number = g_strdup_printf("%4.4d%2.2d%2.2d", first.tm_year + 1900, first.tm_mon + 1, first.tm_mday);
    do_text_set(alias, document_order_key2.document, document_number);
    g_free(document_number);
    do_text_set_empty(document_order_key2.code);
    do_cpy(document_key0.dtype, document_order_key2.dtype);
    do_cpy(document_key0.sklad, document_order_key2.sklad);
    document_number = g_strdup_printf("%4.4d%2.2d%2.2d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    do_text_set(alias, document_key0.document, document_number);
    g_free(document_number);

    status = do_document_order_key2(alias, &document_order_key2, DO_GET_GE);

    while (status == DO_OK) {
        if (memcmp(&document_key0, &document_order_key2, sizeof(document_key0)) < 0)
            break;
        list = g_list_append(list, do_text(alias, document_order_key2.code));

        status = do_document_order_key2(alias, &document_order_key2, DO_GET_NEXT);
    }
    do_alias_free(alias);
    if (status == DO_ERROR)
        return FALSE;
    if (list) {
        do_view_mark_received(DO_VIEW(view), list);

        if (list) {
            g_list_foreach(list, (GFunc)g_free, NULL);
            g_list_free(list);
        }
    }
    return TRUE;
}
*/
static gboolean do_product_view_set_marked(DoView *view, DoContext *context, DoMarked marked)
{
    DoProductViewPrivate  *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);
    gchar *product_code;
    g_object_get(context, "product-code", &product_code, NULL);

    if (priv->receiver == (void*)view) {
        const char *unit;

        struct tm tm1;
        struct tm tm2;
        time_t now;

        now = time(NULL);
        tm1 = *localtime(&now);
        tm2 = tm1;
        tm1.tm_mday-= DOMINO_MARKED_INTERVAL;
        mktime(&tm1);

        if ( priv->units->count )
            unit = priv->units->list[0];
        else
            unit = domino_unit();

        if ( !domino_order_set_marked_(priv->alias_name, unit, product_code, &tm1, &tm2, marked) )
            return FALSE;
    }
    do_product_model_set_marked(DO_PRODUCT_MODEL(priv->model), product_code, marked);
    return TRUE;

}
/*
static gboolean do_product_view_undo_receive(DoView *view, DoContext *context)
{
    DoProductViewPrivate  *priv = DO_PRODUCT_VIEW_GET_PRIVATE (view);
    GList *list = NULL;
    gchar *product_code;
    do_alias_t *alias;
    gchar *document_number;
    document_order_key2_t document_order_key2;
    document_order_rec_t document_order;
    struct tm tm;
    time_t now;
    g_object_get(context, "product-code", &product_code, NULL);

    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_END)) {
        if (alias)
            do_alias_free(alias);
        return FALSE;
    }
    now = time(NULL);
    tm = *localtime(&now);
    do_text_set(alias, document_order_key2.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_ORDER));
    do_text_set(alias, document_order_key2.sklad, domino_unit());
    document_number = g_strdup_printf("%4.4d%2.2d%2.2d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    do_text_set(alias, document_order_key2.document, document_number);
    g_free(document_number);
    do_text_set(alias, document_order_key2.code, product_code);
    switch (do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL)) {
        case DO_ERROR:
            do_alias_free(alias);
            return FALSE;
        case DO_KEY_NOT_FOUND:
            break;
        case DO_OK:
            if (do_document_order_delete(alias) != DO_OK) {
                do_alias_free(alias);
                return FALSE;
            }
            break;
    }
    do_alias_free(alias);
    list = g_list_append(list, g_strdup(product_code));

    do_view_del_received(view, list);

    if (list) {
        g_list_foreach(list, (GFunc)g_free, NULL);
        g_list_free(list);
    }
    return FALSE;
}
*/
/*
static void  do_product_view_add_received(DoView *view, GList *list)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    do_product_model_add_received(DO_PRODUCT_MODEL(priv->model), list);
}
static void  do_product_view_del_received(DoView *view, GList *list)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    do_product_model_del_received(DO_PRODUCT_MODEL(priv->model), list);
}
*/
static gboolean    do_product_view_can_do_find_barcode(DoView *view)
{
    return TRUE;
}
static void     do_product_view_do_find_barcode(DoView *view)
{
    gchar *barcode;
    barcode = do_find_barcode_dialog(GTK_WIDGET(view));
    if (!barcode)
        return;

    search_find_by_bcode(DO_PRODUCT_VIEW(view), barcode);

    g_free(barcode);
}
/*
static void  do_product_view_refresh_recieved(DoProductModel *model, GtkTreeIter *iter, DoProductView *view)
{
    do_product_model_refresh_received_from_request(model, iter);
}

*/
static void  do_product_view_refresh_marked(DoProductModel *model, GtkTreeIter *iter, DoProductView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);


    if (priv->receiver != (void*)view)
        return;

    GValue value = { 0, };
    GValue markedv = { 0, };
    gtk_tree_model_get_value(GTK_TREE_MODEL(model), iter, DO_PRODUCT_MODEL_COL_CODE, &value);
    gtk_tree_model_get_value(GTK_TREE_MODEL(model), iter, DO_PRODUCT_MODEL_COL_MARKED, &markedv);
    struct tm tm1;
    struct tm tm2;
    struct tm tm_marked;
    time_t now;
    const gchar *product_code;
    domino_marked_t marked;
    const char *unit;

    now = time(NULL);
    tm1 = *localtime(&now);
    tm2 = tm1;
    tm1.tm_mday-= DOMINO_MARKED_INTERVAL;
    mktime(&tm1);

    product_code = g_value_get_string(&value);


    if ( priv->units->count )
        unit = priv->units->list[0];
    else
        unit = domino_unit();

    if ( !domino_order_get_marked(priv->alias, unit, product_code, &tm1, &tm2, &marked, &tm_marked) )
        return;

    if ( marked != DOMINO_MARKED_UNMARK && domino_order_after_traffic_in(priv->alias, unit, product_code, &tm_marked) )
        marked = DOMINO_MARKED_UNMARK;
        /*if ( do_date_cmp(&tm2,&tm_marked) && domino_order_after_traffic_in(priv->alias, unit, product_code, &tm_marked) )
           marked = DOMINO_MARKED_UNMARK;*/

    if ( marked != g_value_get_int(&markedv) ) {
        do_product_model_set_marked(DO_PRODUCT_MODEL(priv->model), product_code, marked);
    }
}
static gboolean do_product_view_toggle_marked(DoView *view, DoContext *context)
{
    if ( domino_check_role(DOMINO_ROLE_MANAGER) ) {

        DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);

        struct tm tm1;
        struct tm tm2;
        struct tm tm_marked;
        time_t now;
        const gchar *product_code;
        const char *unit;
        domino_marked_t marked;

        now = time(NULL);
        tm1 = *localtime(&now);
        tm2 = tm1;
        tm1.tm_mday-= DOMINO_MARKED_INTERVAL;
        mktime(&tm1);

        g_object_get(context, "product-code", &product_code, NULL);

        if ( priv->units->count )
            unit = priv->units->list[0];
        else
            unit = domino_unit();

        if ( !domino_order_get_marked(priv->alias, unit, product_code, &tm1, &tm2, &marked, &tm_marked) )
            return FALSE;
        if ( marked != DOMINO_MARKED_UNMARK &&
             domino_order_after_traffic_in(priv->alias, unit, product_code, &tm_marked) )
                marked = DOMINO_MARKED_UNMARK;
/*        if ( marked != DOMINO_MARKED_UNMARK )
            if ( do_date_cmp(&tm2,&tm_marked) && domino_order_after_traffic_in(priv->alias, unit, product_code, &tm_marked) )
                marked = DOMINO_MARKED_UNMARK;*/

        switch ( marked ) {
            case DOMINO_MARKED_UNMARK:
                marked = DOMINO_MARKED;
                break;
            case DOMINO_MARKED:
                marked = DOMINO_MARKED_AND_ACCEPT;
                break;
            case DOMINO_MARKED_AND_ACCEPT:
                marked = DOMINO_MARKED_AND_REJECT;
                break;
            case DOMINO_MARKED_AND_REJECT:
                marked = DOMINO_MARKED_UNMARK;
                break;
        }
        return do_product_view_set_marked(view, context, marked);
    }
    if ( domino_check_role(DOMINO_ROLE_OPERATOR | DOMINO_ROLE_HANDBOOK) )
        return do_product_view_set_marked(view, context, DO_MARKED);

    return FALSE;
}
#ifdef ADS_RECOMMEND
static int cmp_func(char *code1, char *code2, DoProductModel *model)
{
    const gchar *name1, *name2;
    name1 = do_product_model_find_name_by_code(model, code1);
    name2 = do_product_model_find_name_by_code(model, code2);
    if ( !name1 || !name2 ) return 0;
    return strcmp(name1,name2);
}
static int do_document_valid_date(do_alias_t *alias, document_rec_t *document, struct tm *tm)
{
    struct tm tm1;
    if ( !tm ) {
        time_t now;
        now = time(NULL);
        tm1 = *localtime(&now);
    }
    else
        tm1 = *tm;

    struct tm d1;
    struct tm d2;
    char *val;
    val = do_document_param(alias, document, "D1");
    if ( !val[0] || !do_atodate(val, &d1) )
        d1.tm_mday = 0;
    do_free(val);
    val = do_document_param(alias, document, "D2");
    if ( !val[0] || !do_atodate(val, &d2) )
        d2.tm_mday = 0;
    do_free(val);

    return ((!d1.tm_mday || do_date_cmp(&tm1, &d1) >= 0) &&
           ((!d2.tm_mday || do_date_cmp(&tm1, &d2) <= 0) ) );
}
static gboolean do_product_view_fill_recommend(DoProductView *view, const char *master, const char *select, const char *list_src)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    do_alias_t *alias = priv->alias_recommend;
    document_key0_t document_key0, key1;
    document_rec_t document;
    gboolean res = TRUE;
    char  *desc;
    int status;
    GtkTreeIter iter, *iter_select = NULL;
    //GtkTreePath *path_select = NULL;
    char *list = NULL;

    gtk_list_store_clear(priv->recommend_list);

    if ( !priv->recommend_master )
        priv->recommend_master = do_string_list_new();
    do_string_list_add_alloc(priv->recommend_master, master);
    if ( master && (!list_src || !list_src[0]) ) {
        do_text_set(alias, document_key0.dtype, "MR");
        do_text_set(alias, document_key0.sklad, do_param(DO_PARAM_COMMON_SKLAD));
        do_text_set(alias, document_key0.document, master);
        if ( do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL) == DO_OK ) {
            list = do_document_param(alias ,&document, "LIST");
            if ( !list[0] ) {
                do_free(list);
                list = NULL;
            }
        }
    }
    if ( !list )
        list = (char*)list_src;
    do_text_set(alias, document_key0.dtype, "MR");
    do_text_set(alias, document_key0.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    do_text_set_empty(document_key0.document);

    do_cpy(key1.dtype, document_key0.dtype);
    do_cpy(key1.sklad, document_key0.sklad);

    if ( !list || !list[0] ) {
        status = do_document_get0(alias, &document, &document_key0, DO_GET_GE);
        while ( status == DO_OK ) {
            if ( do_cmp(key1.dtype, document_key0.dtype) ||
                 do_cmp(key1.sklad, document_key0.sklad)
                 )

                  break;
            char *value;
            value = do_document_param(alias, &document, "MASTER");
            if ( !strcmp(value, master) && do_document_valid_date(alias, &document, NULL) ) {
                if ( do_document_param_int(alias, &document, "END") == 1 ) {
                    document_order_key0_t document_order_key0;
                    document_order_rec_t document_order;
                    int indx;
                    do_string_list_t *codes=do_string_list_new();
                    char *code;
                    do_cpy(document_order_key0.dtype, document_key0.dtype);
                    do_cpy(document_order_key0.sklad, document_key0.sklad);
                    do_cpy(document_order_key0.document, document_key0.document);
                    document_order_key0.line = 0;
                    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GT);
                    while ( status == DO_OK ) {
                        if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                             do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                             do_cmp(document_order_key0.document, document_key0.document)
                           )
                           break;
                        code= do_text(alias, document_order.data.code);
                        do_string_list_add(codes, code);
                        if ( do_product_model_find_record_by_code(DO_PRODUCT_MODEL(priv->model), code, &indx) ) {
                            GtkTreePath *path;
                            GtkTreeIter miter;
                            GValue desc={0,};
                            GValue mfr={0,};
                            GValue price={0,};
                            GValue stock={0,};
                            path = gtk_tree_path_new();
                            gtk_tree_path_append_index(path, indx);
                            gtk_tree_model_get_iter(priv->model, &miter, path);
                            gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_NAME, &desc);
                            gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_MANUFACTOR, &mfr);
                            gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_PRICE, &price);
                            gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_STOCK, &stock);

                            gtk_list_store_append(priv->recommend_list, &iter);

                            gtk_list_store_set(priv->recommend_list, &iter, 0, g_value_get_string(&desc),
                            1, g_value_get_string(&mfr),
                            2, g_value_get_string(&price),
                            3, g_value_get_string(&stock),
                            4, code,
                            5, 1, 6, master, 7, " ",
                             -1);
                        }
                        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
                    }
                    for ( indx = 0; indx < codes->count; indx++ )
                        domino_set_recommend(alias, codes->list[indx]);
                    do_string_list_free(codes);
                    break;
                }
                else {
                    char *sad = do_text(alias,document.data.document);
                    char *list = do_document_param(alias ,&document, "LIST");
                    char *ch;
                    char *plist = do_document_param(alias ,&document, "PLIST");
                    if ( plist[0] && list[0]) {
                       char *val;
                       val = g_strdup_printf("%s,%s",plist,list)                    ;
                       do_free(list);
                       list = val;
                    }
                    else
                        do_free(plist);
                    desc = do_document_param(alias ,&document, "MARKUP37");
                    if ( !desc[0] ) {
                        do_free(desc);
                        desc = do_document_param(alias ,&document, "37");
                    }
                    for ( ch = desc; *ch; ch++ )
                        if ( *ch == '|' ) *ch = '\n';
                    gtk_list_store_append(priv->recommend_list, &iter);
                    gtk_list_store_set(priv->recommend_list, &iter, 0, desc, 4, sad, 5, 0, 6, master, 8, list, -1);
                    if ( select && !strcmp(select,sad) ) {
                        iter_select = do_malloc(sizeof(iter));
                        memcpy(iter_select, &iter, sizeof(iter));
                        //path_select = gtk_tree_model_get_path(GTK_TREE_MODEL(priv->recommend_list), &iter);
                    }
                    do_free(desc);do_free(sad);do_free(list);
                }
            }
            do_free(value);
            status = do_document_get0(alias, &document, &document_key0, DO_GET_NEXT);
        }

    }
    else {
        do_string_list_t *source;
        source = do_string_list_new_from_csv(list);
        int i,j;
        for ( j = 0; j < source->count; j++ ) {

            gchar **key;
            key = g_strsplit(source->list[j], ".", -1);
            document_order_key0_t document_order_key0;
            document_order_rec_t document_order;
            int indx;
            do_sort_list_t *codes = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)cmp_func, priv->model);
            int dclub, add, lst;

            char *code;
            GHashTable *sk;
            sk =  g_hash_table_new(g_str_hash, g_str_equal);

            if ( key && g_strv_length(key) > 1 ) {
                if ( g_strv_length(key) > 2) {
                    dclub = !strcmp(key[0], "SK");
                    lst = !strcmp(key[0], "LT");
                    do_text_set(alias, document_key0.dtype, key[0]);
                    do_text_set(alias, document_key0.sklad, key[1]);
                    do_text_set(alias, document_key0.document, key[2]);
                    indx = (g_strv_length(key) > 3) ? atoi(key[3]) : 0 ;
                    do_cpy(document_order_key0.dtype, document_key0.dtype);
                    do_cpy(document_order_key0.sklad, document_key0.sklad);
                    do_cpy(document_order_key0.document, document_key0.document);
                    document_order_key0.line = 0;
                    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GT);
                    while ( status == DO_OK ) {
                        if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                             do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                             do_cmp(document_order_key0.document, document_key0.document)
                           )
                           break;
                        add = TRUE;
                        code= do_text(alias, document_order.data.code);
                        if ( dclub ) {
                            char *str;
                            time_t d1,d2,now;
                            struct tm tm1;
                            struct tm tm;
                            time ( &now );
                            tm1 = *localtime(&now);
                            tm1.tm_hour = 0;
                            tm1.tm_min = 0;
                            tm1.tm_sec = 0;
                            now = mktime(&tm1);

                            d1=0;d2=0;
                            str = do_document_order_param(alias, &document_order, "DATE1");
                            if ( do_atodate(str, &tm) )
                                d1 = mktime(&tm);
                            do_free(str);
                            str = do_document_order_param(alias, &document_order, "DATE2");
                            if ( do_atodate(str, &tm) )
                                d2 = mktime(&tm);
                            do_free(str);
                            add = ((d1 == 0) || (d1 <= now)) && ((d2 == 0) || (d2 >= now));
                            str = NULL;
                            if ( do_document_order_param_int(alias, &document_order, "PEREOPRED") == 1 ) {
                                if ( !do_document_order_param_int(alias, &document_order, "PROC_SUM") ) {
                                    char *buf;
                                    buf = do_rest_format(do_document_order_price(alias, &document_order, 2));
                                    str = g_strdup_printf("%s%s",buf,"%");
                                    do_free(buf);
                                }
                                else
                                    str = g_strdup_printf("%.2f руб.",do_document_order_price(alias, &document_order, 4));
                            }
                            if ( str )
                               g_hash_table_insert(sk, code, str);
                        }
                        if ( lst ) {
                            char *str;
                            str = do_document_order_param(alias, &document_order, "SK_DESC");
                            if ( str[0] )
                               g_hash_table_insert(sk, code, str);
                            else
                                do_free(str);
                        }
                        if ( add && indx )
                            add = do_document_order_param_int(alias, &document_order, "INDX") == indx;
                        if ( add )
                            do_sort_list_add(codes, code);

                        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
                    }
                }
                else {
                    do_text_set(alias, document_key0.dtype, key[0]);
                    do_text_set(alias, document_key0.sklad, key[1]);
                    do_text_set_empty(document_key0.document);

                    do_cpy(key1.dtype, document_key0.dtype);
                    do_cpy(key1.sklad, document_key0.sklad);

                    status = do_document_get0(alias, &document, &document_key0, DO_GET_GE);
                    while ( status == DO_OK ) {
                        if ( do_cmp(key1.dtype, document_key0.dtype) ||
                             do_cmp(key1.sklad, document_key0.sklad)
                             )

                              break;
                        if ( do_document_valid_date(alias, &document, NULL) ) {
                            if ( do_document_param_int(alias, &document, "END") == 1 ) {
                                document_order_key0_t document_order_key0;
                                document_order_rec_t document_order;
                                int indx;
                                do_string_list_t *codes=do_string_list_new();
                                char *code;
                                do_cpy(document_order_key0.dtype, document_key0.dtype);
                                do_cpy(document_order_key0.sklad, document_key0.sklad);
                                do_cpy(document_order_key0.document, document_key0.document);
                                document_order_key0.line = 0;
                                status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GT);
                                while ( status == DO_OK ) {
                                    if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                                         do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                                         do_cmp(document_order_key0.document, document_key0.document)
                                       )
                                       break;
                                    code= do_text(alias, document_order.data.code);
                                    do_string_list_add(codes, code);
                                    if ( do_product_model_find_record_by_code(DO_PRODUCT_MODEL(priv->model), code, &indx) ) {
                                        GtkTreePath *path;
                                        GtkTreeIter miter;
                                        GValue desc={0,};
                                        GValue mfr={0,};
                                        GValue price={0,};
                                        GValue stock={0,};
                                        path = gtk_tree_path_new();
                                        gtk_tree_path_append_index(path, indx);
                                        gtk_tree_model_get_iter(priv->model, &miter, path);
                                        gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_NAME, &desc);
                                        gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_MANUFACTOR, &mfr);
                                        gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_PRICE, &price);
                                        gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_STOCK, &stock);

                                        gtk_list_store_append(priv->recommend_list, &iter);

                                        gtk_list_store_set(priv->recommend_list, &iter, 0, g_value_get_string(&desc),
                                        1, g_value_get_string(&mfr),
                                        2, g_value_get_string(&price),
                                        3, g_value_get_string(&stock),
                                        4, code,
                                        5, 1, 6, master, 7, " ",
                                         -1);
                                    }
                                    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
                                }
                                for ( indx = 0; indx < codes->count; indx++ )
                                    domino_set_recommend(alias, codes->list[indx]);
                                do_string_list_free(codes);
                                break;
                            }
                            else {
                                char *sad = do_text(alias,document.data.document);
                                char *list = do_document_param(alias ,&document, "LIST");
                                char *ch;
                                char *plist = do_document_param(alias ,&document, "PLIST");
                                if ( plist[0] && list[0]) {
                                   char *val;
                                   val = g_strdup_printf("%s,%s",plist,list)                    ;
                                   do_free(list);
                                   list = val;
                                }
                                else
                                    do_free(plist);
                                desc = do_document_param(alias ,&document, "MARKUP37");
                                if ( !desc[0] ) {
                                    do_free(desc);
                                    desc = do_document_param(alias ,&document, "37");
                                }
                                for ( ch = desc; *ch; ch++ )
                                    if ( *ch == '|' ) *ch = '\n';
                                gtk_list_store_append(priv->recommend_list, &iter);
                                gtk_list_store_set(priv->recommend_list, &iter, 0, desc, 4, sad, 5, 0, 6, master, 8, list, -1);
                                if ( select && !strcmp(select,sad) ) {
                                    iter_select = do_malloc(sizeof(iter));
                                    memcpy(iter_select, &iter, sizeof(iter));
                                    //path_select = gtk_tree_model_get_path(GTK_TREE_MODEL(priv->recommend_list), &iter);
                                }
                                do_free(desc);do_free(sad);do_free(list);
                            }
                        }
                        status = do_document_get0(alias, &document, &document_key0, DO_GET_NEXT);
                    }
                }
            }
            g_strfreev(key);
            for ( i = 0; i < codes->count; i++ ) {
                int indx;
                code = codes->list[i];
                if ( do_product_model_find_record_by_code(DO_PRODUCT_MODEL(priv->model), code, &indx) ) {
                    GtkTreePath *path;
                    GtkTreeIter miter;
                    gchar *str;
                    GValue desc={0,};
                    GValue mfr={0,};
                    GValue price={0,};
                    GValue stock={0,};
                    path = gtk_tree_path_new();
                    gtk_tree_path_append_index(path, indx);
                    str = g_hash_table_lookup(sk, code);
                    gtk_tree_model_get_iter(priv->model, &miter, path);
                    gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_NAME, &desc);
                    gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_MANUFACTOR, &mfr);
                    gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_PRICE, &price);
                    gtk_tree_model_get_value(priv->model, &miter, DO_PRODUCT_MODEL_COL_STOCK, &stock);

                    gtk_list_store_append(priv->recommend_list, &iter);

                    gtk_list_store_set(priv->recommend_list, &iter, 0, g_value_get_string(&desc),
                    1, g_value_get_string(&mfr),
                    2, g_value_get_string(&price),
                    3, g_value_get_string(&stock),
                    4, code,
                    5, 1, 6, master, 7, " ", 9, str,
                     -1);
                }
                domino_set_recommend(alias, code);
            }
            //!!g_hash_table_foreach(sk, (GHFunc)g_free, NULL);
            g_hash_table_destroy(sk);
            do_sort_list_free(codes);

        }
        do_string_list_free(source);
    }
    GtkTreePath *path_select = NULL;
    if ( !iter_select )
        path_select = gtk_tree_path_new_first();
    else {
        path_select = gtk_tree_model_get_path(GTK_TREE_MODEL(priv->recommend_list), iter_select);
        do_free(iter_select);
    }
    if ( priv->recommend )
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->recommend), path_select, 0, FALSE);
    gtk_tree_path_free(path_select);
    if ( list != list_src )
        do_free(list);

    {
        GtkTreeIter iter;
        GtkTreePath *path;
        path = gtk_tree_path_new_first();
        res = gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->recommend_list), &iter, path);
        gtk_tree_path_free(path);
    }
    return res;
}
static void do_product_view_recommend_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoProductView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    GtkTreeIter iter;

    if ( gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->recommend_list), &iter, path) ) {

        GValue value = { 0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->recommend_list), &iter, 5, &value);
        if ( g_value_get_int(&value) == 0 ) {
            GValue master = {0, };
            GValue list = {0, };
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->recommend_list), &iter, 4, &master);
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->recommend_list), &iter, 8, &list);
            do_product_view_fill_recommend(view, g_value_get_string(&master), NULL, g_value_get_string(&list));
        }
        else {
            GValue master = {0, };
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->recommend_list), &iter, 4, &master);
            edit_product(DO_PRODUCT_VIEW(view), DO_DIALOG_ACTION_EDIT, g_value_get_string(&master), NULL);
        }
    }
}
static gboolean clear_flag(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    priv->refill = FALSE;
    return FALSE;
}

static gboolean do_product_view_recommend_key_press(GtkWidget *widget, GdkEventKey *event, DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    guint mask = gtk_accelerator_get_default_mod_mask ();
    if ( (event->state & mask) == 0 )
    {
    	switch (event->keyval)
    	{
            case GDK_KEY_Escape:
                if ( priv->recommend_master && priv->recommend_master->count > 1 ) {
                    gchar *master,*select;
                    master = do_strdup(priv->recommend_master->list[priv->recommend_master->count-2]);
                    select = do_strdup(priv->recommend_master->list[priv->recommend_master->count-1]);
                    do_string_list_delete(priv->recommend_master,priv->recommend_master->count-1);
                    do_string_list_delete(priv->recommend_master,priv->recommend_master->count-1);
                    do_product_view_fill_recommend(do_view,master,select,NULL);
                }
                else {
                    //gtk_widget_set_visible(GTK_WIDGET(priv->tree_view), TRUE);
                    //!!do_tree_view_tree_view_set_visible(DO_TREE_VIEW(do_view), TRUE);
                    //gtk_widget_set_visible(GTK_WIDGET(priv->recommend), FALSE);
                    gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), 0);
                    do_tree_view_grab_focus(DO_TREE_VIEW(do_view));
                    priv->refill = TRUE;
                    gdk_threads_add_timeout (DO_PRODUCT_VIEW_RECOMMEND_REFILL_TIMEOUT,
                            (GSourceFunc)clear_flag, do_view);
                }
                return TRUE;
#ifdef ADS_RECOMMEND
            case GDK_KEY_Right:
#ifdef MIN_SCREEN
                if ( priv->recommend_master && priv->recommend_master->count > 1 ) {
#endif
                    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(priv->recommend), NULL, priv->recommend_last_col, TRUE, 0.5, 0.0);
                    //!!gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(priv->recommend_scroll),  GTK_CORNER_TOP_LEFT);
                    return TRUE;
#ifdef MIN_SCREEN
                }
                else {
                    gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), 0);
                    do_product_view_filter_set_text(do_view, "");
                    return TRUE;
                }
#endif
#endif

            default: {
                gchar *buf = search_get_text (event->string, event->length, 0 );
                if ( buf && buf[0] != '\0' ) {
                    g_free(buf);
                    gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), 0);
                        //gtk_widget_grab_focus(GTK_WIDGET(priv->tree_view));
                    do_tree_view_grab_focus(DO_TREE_VIEW(do_view));
                    return do_view_key_press(widget, event, do_view);
                }
                if ( buf )
                    g_free(buf);
            }
        }
    }
    return FALSE;
}
void do_product_view_refill_recommend(DoProductView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    gtk_list_store_clear(priv->recommend_list);

    priv->recommend_empty = !do_product_view_fill_recommend(view, "", "", NULL);
    GtkTreePath *path = NULL;
    gtk_widget_grab_focus(GTK_WIDGET(priv->recommend));
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->recommend), &path, NULL);

    if ( !path ) {
        path = gtk_tree_path_new_first();
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->recommend), path, NULL, FALSE);
    }
    gtk_tree_path_free(path);
    gtk_widget_set_visible(priv->recommend_widget, !priv->recommend_empty);
}

static GtkWidget *do_product_view_make_recommend(DoProductView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    GtkWidget *w;
    GtkListStore *store;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *col;

    priv->recommend_list = store = gtk_list_store_new(10,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_INT,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);

    priv->recommend_empty = !do_product_view_fill_recommend(view, "", "", NULL);

    priv->recommend = w = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW(w), FALSE);
#if defined(MIN_SCREEN) || defined(CASH)
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(w), FALSE);
#endif

    g_signal_connect (w, "row-activated", G_CALLBACK(do_product_view_recommend_row_activated), view);
    g_signal_connect (w, "key-press-event", G_CALLBACK (do_product_view_recommend_key_press), view);


    renderer = gtk_cell_renderer_text_new();
    priv->recommend_first_col = col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, "  ");
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    //gtk_tree_view_column_add_attribute (col, renderer, "text", 7);
    gtk_tree_view_append_column(GTK_TREE_VIEW(w), col);

    renderer = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "markup", 0);
    //gtk_tree_view_column_set_sort_column_id (col, 0);
    //gtk_tree_view_column_set_sort_order(col, GTK_SORT_ASCENDING);
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(store), 0, GTK_SORT_ASCENDING);

    //gtk_tree_view_column_set_cell_data_func(col, renderer, cell_desc, view, NULL);
    gtk_tree_view_column_set_title(col, "Наименование");
    gtk_tree_view_append_column(GTK_TREE_VIEW(w), col);

    /*renderer = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "markup", 0);
    gtk_tree_view_column_set_sort_column_id (col, 0);
    gtk_tree_view_column_set_sort_order(col, GTK_SORT_ASCENDING);
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(store), 0, GTK_SORT_ASCENDING);

    //gtk_tree_view_column_set_cell_data_func(col, renderer, cell_desc, view, NULL);
    gtk_tree_view_column_set_title(col, "Наименование");
    gtk_tree_view_append_column(GTK_TREE_VIEW(w), col);*/

    renderer = gtk_cell_renderer_text_new();

    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", 1);
    //gtk_tree_view_column_set_cell_data_func(col, renderer, cell_desc, view, NULL);
    gtk_tree_view_column_set_title(col, "Пр.");
    gtk_tree_view_column_set_resizable(col, TRUE);
    gtk_tree_view_column_set_sizing(col, 150);
    gtk_tree_view_column_set_reorderable(col, TRUE);


//#ifdef MIN_SCREEN
    gtk_tree_view_column_set_visible(col,FALSE);
//#endif

    gtk_tree_view_append_column(GTK_TREE_VIEW(w), col);
    renderer = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", 2);
    //gtk_tree_view_column_set_cell_data_func(col, renderer, cell_desc, view, NULL);
    gtk_tree_view_column_set_title(col, "Цена");
    gtk_tree_view_append_column(GTK_TREE_VIEW(w), col);

    renderer = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", 3);
    gtk_tree_view_column_set_title(col, "Остаток");

    gtk_tree_view_append_column(GTK_TREE_VIEW(w), col);
    //!!gtk_widget_set_size_request(w, -1, 100);
    //!!ree_view_tree_view_set_visible(DO_TREE_VIEW(view), FALSE);
    //gtk_widget_set_visible(GTK_WIDGET(priv->tree_view), FALSE);
    //gtk_widget_set_visible(GTK_WIDGET(priv->recommend), TRUE);
    renderer = gtk_cell_renderer_text_new();
    priv->recommend_last_col = col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", 9);
    gtk_tree_view_column_set_title(col, "Скидка");
    gtk_tree_view_append_column(GTK_TREE_VIEW(w), col);


    GtkWidget *scroll;
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll), GTK_WIDGET(w));

    GtkTreePath *path = NULL;

    gtk_widget_grab_focus(GTK_WIDGET(w));
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(w), &path, NULL);

    if ( !path ) {
        path = gtk_tree_path_new_first();
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->recommend), path, NULL, FALSE);
    }
    gtk_tree_path_free(path);

    return scroll;
}
#endif
static gboolean text_compare(const char *s1, const char *s2)
{
    if ( (!s1 && s2) || (s1 && !s2) )
        return TRUE;
    if ( !s1 && !s2 )
        return FALSE;
    char *d1,*d2;
    d1 = strdup(s1);
    d2 = strdup(s2);
    int i;
    for ( i = strlen(d1)-1; i >= 0; i-- )
    if (d1[i] != ' ') break;
    d1[i+1]='\0';
    for ( i = strlen(d2)-1; i >= 0; i-- )
    if (d2[i] != ' ') break;
    d2[i+1]='\0';
    gboolean res;
    res = strcmp(d1,d2);
    do_free(d1);do_free(d2);
    return res != 0;
}
static void do_product_view_filter_entry_changed(GtkEditable *entry, DoView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    gchar *filter_text;
    const gchar *text;
    int i;


    text = gtk_entry_get_text(GTK_ENTRY(priv->filter_entry));
    for ( i = 0; i < G_N_ELEMENTS(info_labels); i++ ) {
        if ( !strcmp(text, info_labels[i]) ) {
            return;
        }
    }
    if ( !strlen(text) && !gtk_widget_is_focus(GTK_WIDGET(entry)) )  {
        gtk_entry_set_text(GTK_ENTRY(entry), info_labels[0]);
        return;
    }
    if ( strlen(text) && gtk_widget_is_focus(GTK_WIDGET(entry)) )
        gtk_widget_show(priv->filter_info);
    else
        gtk_widget_hide(priv->filter_info);

    filter_text = filter_get_text(text);
    if ( g_strcmp0(text, filter_text) && filter_text )
        gtk_entry_set_text(GTK_ENTRY(priv->filter_entry), filter_text);

    if ( text_compare(filter_text, priv->filter_text) ) {
        if ( priv->filter_flush_timeout ) {
        g_source_remove(priv->filter_flush_timeout);
            priv->filter_flush_timeout = 0;
        }
        if ( priv->filter_text ) g_free(priv->filter_text);
        priv->filter_text = filter_text;
        priv->filter_flush_timeout = gdk_threads_add_timeout (DO_PRODUCT_VIEW_FILTER_TIMEOUT,
    	   (GSourceFunc)do_product_view_filter_apply, view);
        //do_log(LOG_WARNING, "%s",priv->filter_text);
    }
    else {
        if ( filter_text ) g_free(filter_text);
    }
}
static gboolean do_product_view_grab_focus_tree(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    int n;
    n = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(priv->model), NULL);
    if ( n )
    	do_tree_view_grab_focus(DO_TREE_VIEW(do_view));
    return n > 0;
}
static void do_product_view_filter_entry_activate(GtkEntry *entry, DoView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    if ( priv->filter_flush_timeout ) {
        g_source_remove(priv->filter_flush_timeout);
        do_product_view_filter_apply(DO_PRODUCT_VIEW(view));
    }
    if ( do_product_model_is_filtered_and_empty(DO_PRODUCT_MODEL(priv->model)) )
        do_product_view_filter_entry_action_activate(NULL, DO_VIEW(view));
    else
        do_product_view_grab_focus_tree(DO_PRODUCT_VIEW(view));
}

static void do_product_view_filter_clear(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    do_product_model_set_filter(DO_PRODUCT_MODEL(priv->model), NULL, NULL, NULL);
    g_object_set(do_view, "fixed-height-mode", TRUE, NULL);

    do_string_list_clear(priv->lexs);
    gtk_widget_queue_draw(GTK_WIDGET(do_view));
    gtk_entry_set_text(GTK_ENTRY(priv->filter_entry), "");
}
static gboolean do_product_view_filter_entry_key_press(GtkWidget *widget, GdkEventKey *event, DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    guint mask = gtk_accelerator_get_default_mod_mask ();
    if ( (event->state & mask) == 0 )
    {
    	switch (event->keyval)
    	{
            case GDK_KEY_Escape: {
                const gchar *text;
                text = gtk_entry_get_text(GTK_ENTRY(priv->filter_entry));
            if ( strlen(text) > 0 )
                    do_product_view_filter_clear(do_view);
                do_tree_view_grab_focus(DO_TREE_VIEW(do_view));
                return TRUE;
            }
            case GDK_KEY_Down: {
                do_product_view_grab_focus_tree(DO_PRODUCT_VIEW(do_view));;
                return TRUE;
            }
            case GDK_KEY_Right: {
                do_product_view_filter_entry_action_activate(NULL, DO_VIEW(do_view));
                return TRUE;
            }
            default:
                break;
        }
    }

    return FALSE;
}
#if GTK_MAJOR_VERSION > 2
static void do_product_view_filter_icon_press(GtkEntry *entry, GtkEntryIconPosition icon_pos, GdkEvent *event, DoProductView *do_view)
{
    //DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    if ( icon_pos == GTK_ENTRY_ICON_SECONDARY )
        do_product_view_filter_clear(do_view);
}
#endif
static gboolean do_product_view_filter_apply(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    priv->filter_flush_timeout = 0;
    const gchar *text;
    text = gtk_entry_get_text(GTK_ENTRY(priv->filter_entry));
    GMatchInfo *info = NULL;
    gboolean res;
    do_string_list_t *lexs;
    int indx,i;
    gchar *lex;

    for ( i = 0; i < G_N_ELEMENTS(info_labels); i++ ) {
        if ( !strcmp(text, info_labels[i]) ) {
            text = "";
            break;
        }
    }

    lexs = do_string_list_new();

    res = g_regex_match(priv->re_word, text, 0, &info);
    if ( res && info ) {
        while ( g_match_info_matches( info ) ) {
            lex = g_match_info_fetch( info, 0 );
            if ( !do_string_list_find(lexs, &indx, lex) )
                do_string_list_add(lexs,lex);
            else
                g_free(lex);
            g_match_info_next( info, NULL );
        }
        g_match_info_free( info );
    }
    res = g_regex_match(priv->re_digit, text, 0, &info);
    if ( res && info ) {
        while ( g_match_info_matches( info ) ) {
            lex = g_match_info_fetch( info, 0 );
            if ( !do_string_list_find(lexs, &indx, lex) )
                do_string_list_add(lexs,lex);
            else
                g_free(lex);
            g_match_info_next( info, NULL );
        }
        g_match_info_free( info );
    }
    res = priv->lexs->count == lexs->count;
    if ( res )
        for ( i = 0; i < lexs->count; i++ )
            if ( !do_string_list_find(priv->lexs, &indx, lexs->list[i]) ) {
                res = FALSE;
                break;
            }

    if ( !res ) {
        do_string_list_free(priv->lexs);
        priv->lexs = lexs;
    }
    //do_log(LOG_WARNING, "apply %s",text);
    if ( strlen(text) > 1 ) {
        g_object_set(do_view, "fixed-height-mode", FALSE, NULL);
        do_product_model_set_filter(DO_PRODUCT_MODEL(priv->model), priv->lexs, NULL, text);
        if ( priv->filter_show_timeout ) {
            g_source_remove(priv->filter_show_timeout);
            priv->filter_show_timeout = 0;
         }
        if ( do_product_model_is_filtered(DO_PRODUCT_MODEL(priv->model)) )
          priv->filter_show_timeout =
              gdk_threads_add_timeout (DO_PRODUCT_VIEW_FILTER_SHOW_TIMEOUT,
                 (GSourceFunc)filter_show_timeout, do_view);
        //do_log(LOG_WARNING, "filtered");

    }
    else {
        if ( !strlen(text) )
            do_product_view_filter_clear(do_view);
    }
    return FALSE;
}
//static void do_product_view_filter_entry_action_activate(GtkEntryCompletion *completion, gint indx, DoView *view)
static void do_product_view_filter_entry_action_activate(GtkEntryCompletion *completion, DoView *view)
{
#ifndef MINIMAL_WIN
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    const char *text;
    text = gtk_entry_get_text(GTK_ENTRY(priv->filter_entry));
    //if ( indx == 1 ) {
    if ( strlen(text) > 0 ) {
        char *uri, *text_e;
        text_e = g_uri_escape_string(text, NULL, TRUE);
        gchar *url;
        DOMINO_LOCAL_GET("Webkit", "start-page", &url, NULL);
        if ( url && url[0] != '\0' )
           uri = g_strdup_printf("%s/search?q=%s&ie=UTF-8&oe=UTF-8",url,text_e);
        else
           uri = g_strdup_printf("https://www.google.ru/search?q=%s&ie=UTF-8&oe=UTF-8",text_e);
        do_open_web(uri);
        g_free(uri);g_free(text_e);
    }
    //}
#endif
}
static gboolean do_product_view_filter_entry_focus_in(GtkWidget *widget, GdkEvent *event, DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    const gchar *text;
    int i;
    text = gtk_entry_get_text(GTK_ENTRY(priv->filter_entry));
    for ( i = 0; i < G_N_ELEMENTS(info_labels); i++ ) {
        if ( !strcmp(text, info_labels[i]) ) {
            gtk_entry_set_text(GTK_ENTRY(priv->filter_entry),"");
            return TRUE;
        }
    }
    if ( strlen(text) )
        gtk_widget_show(priv->filter_info);
    return TRUE;

}
static gboolean do_product_view_filter_entry_focus_out(GtkWidget *widget, GdkEvent *event, DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    const gchar *text;
    text = gtk_entry_get_text(GTK_ENTRY(priv->filter_entry));
    if ( !strlen(text) )
        gtk_entry_set_text(GTK_ENTRY(priv->filter_entry),info_labels[0]);
    gtk_widget_hide(priv->filter_info);
    return TRUE;
}
#ifdef ADS_RECOMMEND
static gboolean recommend_timeout(DoProductView *do_view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(do_view);
    if ( !priv->recommend_empty && gtk_widget_is_focus(GTK_WIDGET(priv->tree_view)) ) {
        gtk_widget_grab_focus(priv->recommend);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), 1);
        do_product_view_set_toolbar_markup(do_view, NULL);
    }
    priv->recommend_flush_timeout = 0;
    return FALSE;
}
#endif
static gboolean    view_can_do_send(DoView *view)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);
    gboolean retval = FALSE;
    GtkTreePath *path = NULL;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if ( path ) {
        gtk_tree_path_free(path);
        retval = TRUE;
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
#endif
static void    	view_do_send(DoView *view)
{
#ifdef CASH
    gchar *code;
    gboolean parcel = FALSE;
    code = get_code_from_cursor(DO_PRODUCT_VIEW(view));
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);

    do_alias_t *alias;
    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_END)) {
        if (alias)
            do_alias_free(alias);
        return;
    }
    product_key0_t product_key0;
    product_rec_t product;

    do_text_set(alias, product_key0.code, code);
    product_key0.base_parcel = 0;

    if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) == DO_OK )
        parcel = do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) ==
             DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE;
    do_alias_free(alias);
    if ( parcel ) {
        edit_product(DO_PRODUCT_VIEW(view), DO_DIALOG_ACTION_EDIT, code, "Parcel");
        g_free(code);
    }
    else {
        do_log(LOG_INFO, "code %s", code);
        if ( code ) { //}&& domino_get_back_window() ) {

            domino_back_window_focus();
            g_timeout_add(300, sad,code);
        }
    }
#endif
}
#ifdef ADS_RECOMMEND
static GHashTable*
get_info_messages(const gchar *alias_name)
{
    GHashTable *res;
    GList *l, *messages = NULL;
    do_list_t *h;
    res = g_hash_table_new(g_str_hash, g_str_equal);
    do_alias_t *alias;
    alias = domino_alias_new(alias_name);
    if ( !alias )
        return res;
    if ( !do_alias_open(alias, TRUE) ) {
        do_alias_free(alias);
        return res;
    }
    document_key0_t document_key0, key;
    document_rec_t document;
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    char *code;
    char *message;
    int status;

    do_text_set(alias, document_key0.dtype, "LT");
    do_text_set(alias, document_key0.sklad, "LST");
    do_text_set_empty(document_key0.document);

    do_cpy(key.dtype, document_key0.dtype);
    do_cpy(key.sklad, document_key0.sklad);

    status = do_document_get0(alias, &document, &document_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.dtype, document_key0.dtype) ||
             do_cmp(key.sklad, document_key0.sklad) ) break;

        if ( document.data.p_code == 14 ) {
            char *value;
            gint indx;
            do_string_list_t *units;
            value = do_document_param(alias, &document, "UNITS");
            units = do_string_list_new_from_csv(value);
            char *str;
            gboolean flag = TRUE;
            time_t d1,d2,now;
            struct tm tm1;
            struct tm tm;
            time ( &now );
            tm1 = *localtime(&now);
            tm1.tm_hour = 0;
            tm1.tm_min = 0;
            tm1.tm_sec = 0;
            now = mktime(&tm1);

            flag = flag && (!units->count || do_string_list_find(units, &indx, domino_unit()));

            d1=0;d2=0;
            str = do_document_param(alias, &document, "DATE1");
            if ( do_atodate(str, &tm) )
                d1 = mktime(&tm);
            do_free(str);
            str = do_document_param(alias, &document, "DATE2");
            if ( do_atodate(str, &tm) )
                d2 = mktime(&tm);
            do_free(str);
            flag = flag && ( ((d1 == 0) || (d1 <= now)) && ((d2 == 0) || (d2 >= now)));


            if ( flag ) {

                do_cpy(document_order_key0.dtype, document_key0.dtype);
                do_cpy(document_order_key0.sklad, document_key0.sklad);
                do_cpy(document_order_key0.document, document_key0.document);
                document_order_key0.line = 0;

                status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
                while ( status == DO_OK ) {
                    if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                         do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                         do_cmp(document_order_key0.document, document_key0.document) ) break;

                    code = do_text(alias, document_order.data.code);
                    message = do_document_order_param(alias, &document_order, "MARKUP");
                    for ( l = messages; l; l=l->next )
                        if ( !g_strcmp0(l->data, message) )
                            break;
                    if ( !l )
                        l = messages = g_list_append(messages, message);
                    else
                        g_free(message);

                    h = g_hash_table_lookup(res, code);
                    if ( !h ) {
                        h = do_list_new(FALSE);
                        g_hash_table_insert(res, code, h);
                    }
                    else
                        g_free(code);
                    do_list_add(h, l);
                    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
                }
            }
            do_string_list_free(units);
            g_free(value);
        }
        status = do_document_get0(alias, &document, &document_key0, DO_GET_NEXT);
    }
    do_alias_free(alias);
    return res;
}
#endif
#ifdef ADS_RECOMMEND
static void
do_product_view_set_toolbar_markup(DoProductView *view, const gchar *markup)
{
    DoProductViewPrivate *priv = DO_PRODUCT_VIEW_GET_PRIVATE(view);

    if ( markup ) {
        gtk_label_set_markup(GTK_LABEL(priv->statusbar), markup);
        gtk_widget_show_all(priv->statusbox);
        priv->set_label = TRUE;
    }
    else {
        if ( priv->set_label ) {
            priv->set_label = FALSE;
            gtk_label_set_text(GTK_LABEL(priv->statusbar), "");
            gtk_widget_hide(priv->statusbox);
        }
    }

}
#endif
