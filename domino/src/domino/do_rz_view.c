
#include "do_rz_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_rz_edit_view.h"
#include "domino.h"
#include "do_tree_view.h"
#include "do_product_edit.h"
#include <domino.h>
#include "hig.h"
#include "do_icons.h"
#include "do_filter.h"
#include "do_search.h"
#include <gdk/gdk.h>
#include <time.h>

#define DO_RZ_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_RZ_VIEW, DoRzViewPrivate))

#define OBJECT_ROOT_PATH "RzView"
#define DEFAULT_COL_PACK_FORM_WIDTH 400
#define DEFAULT_COL_DEFAULT_WIDTH 60
#define FILTER_TIMEOUT_SEC 500

// View interface
static void do_rz_view_view_init(DoViewIface *iface);

static gboolean    do_rz_view_view_do_grab_focus(DoView *view);
static void	       do_rz_view_view_do_close(DoView *view);
static const char *do_rz_view_view_get_title(DoView *view);
static GdkPixbuf  *do_rz_view_view_get_icon(DoView *view);
static void		   do_rz_view_view_do_edit(DoView *view, const gchar *tab);
static gboolean	   do_rz_view_view_can_do_edit(DoView *view, const gchar *tab);
static gboolean	   do_rz_view_view_can_do_find_barcode(DoView *view);
static void        do_rz_view_view_do_find_barcode(DoView *view);
static void		   do_rz_view_view_do_delete(DoView *view);
static gboolean	   do_rz_view_view_can_do_action(DoView *view);
static void		   do_rz_view_view_do_insert(DoView *view);
static void		   do_rz_view_view_do_copy(DoView *view);
static void        do_rz_view_view_update_context(DoView *view, DoContext *context);
static void		   do_rz_view_view_do_refresh(DoView *view);
static gboolean	   do_rz_view_view_can_do_refresh(DoView *view);
static void		   do_rz_view_view_set_cursor_to_context(DoView *view, DoContext *context);
static gboolean	   do_rz_view_view_can_set_cursor_to_context(DoView *view, DoContext *context);
static gboolean	   do_rz_view_view_get_load_status(DoView *view);
static gint	       do_rz_view_view_get_load_progress(DoView *view);
static void		   do_rz_view_view_do_popup(DoView *view, GdkEventButton* event);



enum
{
	PROP_0,
	PROP_ALIAS,
    PROP_LOAD_STATUS,
    PROP_LOAD_PROGRESS,
};

enum
{
    COLUMN_DTYPE,
    COLUMN_SKLAD,
    COLUMN_DOCUMENT,
    COLUMN_LINE,

    COLUMN_IN,
    COLUMN_DATE,
    COLUMN_MNN,
    COLUMN_TRADE_NAME,
    COLUMN_PACK_FORM,
    COLUMN_PACK_QUANT,
    COLUMN_UNPACK,
    COLUMN_PRICE,
    COLUMN_VALUTA,
    COLUMN_BARCODE,
    COLUMN_MANUFACTOR,
    COLUMN_LINK,
    N_COLUMNS,
};

const static GType column_type[N_COLUMNS] =
{
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_INT,

    G_TYPE_BOOLEAN,
    G_TYPE_LONG,

    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_INT,
    G_TYPE_BOOLEAN,
    G_TYPE_DOUBLE,
    G_TYPE_INT,
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_INT,
};

struct _DoRzViewPrivate
{
    GtkListStore        *model_sort;
    GtkTreeModelFilter  *model_filter;
    GtkTreeView         *tree_view;
    GtkTreeViewColumn   *columns[N_COLUMNS];
    GtkTreeSelection    *selection;
    GdkPixbuf           *icon;
    gchar               *alias_name;
    GtkWidget           *filter_combo;
    GtkWidget           *filter_entry;
    GtkWidget           *search;
    gint                 filter_indx;
    do_alias_t          *alias;
    guint                filter_timeout;
    gint                 progress;
    struct {
        GHashTable     *h;
        gint            size;
    } invalidated_rows;

};

G_DEFINE_TYPE_WITH_CODE (DoRzView, do_rz_view, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_rz_view_view_init)
                                    );

static void do_rz_view_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_rz_view_view_do_grab_focus;
    iface->do_close      = do_rz_view_view_do_close;
    iface->get_title     = do_rz_view_view_get_title;
    iface->get_icon      = do_rz_view_view_get_icon;
    iface->do_edit       = do_rz_view_view_do_edit;
    iface->do_copy       = do_rz_view_view_do_copy;
    iface->can_do_edit   = do_rz_view_view_can_do_edit;
    iface->do_find_barcode  = do_rz_view_view_do_find_barcode;
    iface->can_do_find_barcode  = do_rz_view_view_can_do_find_barcode;
    iface->do_delete       = do_rz_view_view_do_delete;
    iface->do_insert       = do_rz_view_view_do_insert;
    iface->can_do_delete   = do_rz_view_view_can_do_action;
    iface->can_do_copy     = do_rz_view_view_can_do_action;
    iface->can_do_insert   = do_rz_view_view_can_do_action;
    iface->update_context  = do_rz_view_view_update_context;
    iface->do_refresh       = do_rz_view_view_do_refresh;
    iface->can_do_refresh   = do_rz_view_view_can_do_refresh;
    iface->set_cursor_to_context  = do_rz_view_view_set_cursor_to_context;
    iface->can_set_cursor_to_context  = do_rz_view_view_can_set_cursor_to_context;
    iface->get_load_status = do_rz_view_view_get_load_status;
    iface->get_load_progress = do_rz_view_view_get_load_progress;
    iface->do_popup = do_rz_view_view_do_popup;
}

static void do_rz_view_init(DoRzView *view)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE (view);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, DO_STOCK_DOMINO_GRLS, DO_VIEW_ICON_SIZE, 0, NULL);
    priv->alias = domino_alias_new(priv->alias_name);
    if (priv->alias)
        if (!do_alias_open(priv->alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_BARCODE, DO_DB_PRODUCT, DO_DB_END)) {
            do_alias_free(priv->alias);
            priv->alias = NULL;
        }

}
static gboolean do_rz_view_find_product(do_alias_t *alias, const char *bar_code)
{
    barcode_key0_t barcode_key0;
    char *ch = (char*)bar_code;
    do_text_set(alias, barcode_key0.barcode, ch);
    switch (do_barcode_key0(alias, &barcode_key0, DO_GET_EQUAL)) {
        case DO_OK:
            return TRUE;
        case DO_KEY_NOT_FOUND:
            break;
        default:
            return FALSE;
    }
    return FALSE;
}
int do_rz_info_no_mark(do_alias_t *alias, const char* barcode)
{
    barcode_key0_t  barcode_key0;
    barcode_rec_t   barcode_;
    product_key0_t  product_key0;
    product_rec_t   product;

    do_text_set(alias, barcode_key0.barcode, barcode);
    if (do_barcode_get0(alias, &barcode_, &barcode_key0, DO_GET_EQUAL) != DO_OK)
        return 0;
    do_cpy(product_key0.code, barcode_.data.code);
    product_key0.base_parcel = 0;

    if (do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK)
        return 0;

    return do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) !=
             DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE;
}

static gboolean do_rz_view_model_filter_visible(GtkTreeModel *model,
                                                         GtkTreeIter *iter,
                                                         DoRzView *view)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
	GValue barcode = {0,};
	if (!priv->filter_indx)
        return TRUE;
    if (!priv->alias)
        return FALSE;
    gtk_tree_model_get_value(model, iter, COLUMN_BARCODE, &barcode);
    switch (priv->filter_indx) {
        case -1: {
            const gchar *text = gtk_entry_get_text(GTK_ENTRY(priv->filter_entry));
            GValue mnn = {0, };
            GValue trade_name = {0, };
            GValue pack_form = {0, };
            GValue mfr = {0, };
            gtk_tree_model_get_value(model, iter, COLUMN_MNN, &mnn);
            if (do_text_utf8_find_str(g_value_get_string(&mnn), text))
                return TRUE;
            gtk_tree_model_get_value(model, iter, COLUMN_TRADE_NAME, &trade_name);
            if (do_text_utf8_find_str(g_value_get_string(&trade_name), text))
                return TRUE;
            gtk_tree_model_get_value(model, iter, COLUMN_PACK_FORM, &pack_form);
            if (do_text_utf8_find_str(g_value_get_string(&pack_form), text))
                return TRUE;
            gtk_tree_model_get_value(model, iter, COLUMN_MANUFACTOR, &mfr);
            if (do_text_utf8_find_str(g_value_get_string(&mfr), text))
                return TRUE;
            return FALSE;
        }
        case 1:
            return do_rz_info_multi_exist_by_barcode(priv->alias, g_value_get_string(&barcode));
        case 2:
            return do_rz_info_no_mark(priv->alias, g_value_get_string(&barcode));
    }
    return TRUE;
}

static GtkTreeModel *do_rz_view_create_model(DoRzView *embed)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(embed);
    GtkListStore    *model;
    do_alias_t      *alias;
    do_ret_list_t   *list;
    do_extended_t   *extended;
    GtkTreeIter      iter;
    GtkTreeModelFilter  *model_filter;
    int j, indx;
    struct tm tm;
    time_t date;
    document_order_key0_t document_order_key0;
    struct PACK_STRUCT {
        BTI_CHAR document[10];
        BTI_LONG line;
        BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
        BTI_LONG date;
        BTI_BYTE quant[4][7];
        BTI_SINT nquant[4];
        BTI_REAL price[8];
        BTI_SINT nprice[8];
        BTI_BYTE accepted;
        BTI_CHAR params[1024];
    } *ret_;
    gchar *mnn;
    gchar *trade_name;
    gchar *pack_form;
    double price, quant;
    int valuta;
    gchar *barcode;
    gchar *manufactor;
    gchar *params;
    gchar *value;
    gint  pack_quant;
    gint  unpack;
    gboolean link;

    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_PRODUCT, DO_DB_BARCODE, DO_DB_END))
        return NULL;


    model = gtk_list_store_new(N_COLUMNS,
                                column_type[COLUMN_DTYPE],
                                column_type[COLUMN_SKLAD],
                                column_type[COLUMN_DOCUMENT],
                                column_type[COLUMN_LINE],

                                column_type[COLUMN_IN],
                                column_type[COLUMN_DATE],
                                column_type[COLUMN_MNN],
                                column_type[COLUMN_TRADE_NAME],
                                column_type[COLUMN_PACK_FORM],
                                column_type[COLUMN_PACK_QUANT],
                                column_type[COLUMN_UNPACK],
                                column_type[COLUMN_PRICE],
                                column_type[COLUMN_VALUTA],
                                column_type[COLUMN_BARCODE],
                                column_type[COLUMN_MANUFACTOR],
                                column_type[COLUMN_LINK]
                               );

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);
    do_extended_set_key(extended, 0, &document_order_key0);
    do_extended_set_filter(extended, TRUE, 2, sizeof(document_order_key0.dtype),
                                              sizeof(document_order_key0.sklad));

    do_text_set(alias, document_order_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_RZN));
    do_text_set(alias, document_order_key0.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    do_text_set_empty(document_order_key0.document);
    document_order_key0.line = 0;

    do_extended_set_filter_term(extended, 0, DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, dtype), DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND, &document_order_key0.dtype);
    do_extended_set_filter_term(extended, 1, DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, sklad), DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO, &document_order_key0.sklad);

    do_extended_set_fields(extended, 10,
                           do_size_and_offset(document_order_struct_t, document),
                           do_size_and_offset(document_order_struct_t, line),
                           do_size_and_offset(document_order_struct_t, code),
                           do_size_and_offset(document_order_struct_t, date),
                           do_size_and_offset(document_order_struct_t, quant),
                           do_size_and_offset(document_order_struct_t, nquant),
                           do_size_and_offset(document_order_struct_t, price),
                           do_size_and_offset(document_order_struct_t, nprice),
                           do_size_and_offset(document_order_struct_t, accepted),
                           do_size_and_offset(document_order_struct_t, params)
                           );
    priv->invalidated_rows.size = 0;
    priv->progress = 0;

    while ( ((list = do_extended_get(extended, 52)) != NULL) &&
            do_ret_list_count(list) ) {
        for (indx = 0; indx < do_ret_list_count(list); indx++) {
            ret_ = do_ret_list_item_data(list, indx);
            params = do_alias_text_(alias, ret_->params,
                              do_ret_list_item_len(list, indx) -
                              (sizeof(*ret_) -
                               sizeof(ret_->params)));
            mnn = do_fromparam(params, do_param(DO_PARAM_DORDER_PARAMETR_MNN), do_alias_utf8(alias));
            trade_name = do_fromparam(params, do_param(DO_PARAM_DORDER_PARAMETR_TRADE_NAME), do_alias_utf8(alias));
            pack_form = do_fromparam(params, do_param(DO_PARAM_DORDER_PARAMETR_PACK_FORM), do_alias_utf8(alias));
            valuta = atoi(do_fromparam(params, do_param(DO_PARAM_DORDER_PARAMETR_VALUTA_CODE), do_alias_utf8(alias)));
            barcode = do_fromparam(params, do_param(DO_PARAM_DORDER_PARAMETR_BARCODE), do_alias_utf8(alias));
            manufactor = do_fromparam(params, do_param(DO_PARAM_DORDER_PARAMETR_MANUFACTOR), do_alias_utf8(alias));
            unpack = atoi(do_fromparam(params, do_param(DO_PARAM_DORDER_PARAMETR_UNPACK), do_alias_utf8(alias)));
            link = ret_->accepted;
            price = 0;
            date = do_date(ret_->date, &tm);

            for (j = 0; j < 8; j++)
                if (ret_->nprice[j] == DO_CONST_PRICE_REGISTER) {
                    price = ret_->price[j];
                    break;
                }
            pack_quant = 0;
            //printf ("\"%s\",\"%s\",\"%s\",\"%s\"\n", mnn, trade_name, pack_form, barcode);
            for (j = 0; j < 4; j++)
                if (ret_->nquant[j] == DO_CONST_QUANT_REST) {
                    quant = do_pdecimal(ret_->quant[j], 7, 3);
                    pack_quant = quant;
                    break;
                }
            value = do_text(alias, ret_->document);
            gtk_list_store_append (model, &iter);
            gtk_list_store_set (model, &iter,
                                COLUMN_DTYPE, do_param(DO_PARAM_DOCUMENT_TYPE_RZN),
                                COLUMN_SKLAD, do_param(DO_PARAM_COMMON_SKLAD),
                                COLUMN_DOCUMENT, value,
                                COLUMN_LINE, ret_->line,

                                COLUMN_IN, do_rz_view_find_product(alias, barcode),
                                COLUMN_DATE, date,
                                COLUMN_MNN, mnn,
                                COLUMN_TRADE_NAME, trade_name,
                                COLUMN_PACK_FORM, pack_form,
                                COLUMN_PACK_QUANT, pack_quant,
                                COLUMN_UNPACK, unpack,
                                COLUMN_PRICE, price,
                                COLUMN_VALUTA, valuta,
                                COLUMN_BARCODE, barcode,
                                COLUMN_MANUFACTOR, manufactor,
                                COLUMN_LINK, link,
                                -1);
            g_hash_table_insert(priv->invalidated_rows.h, iter.user_data, GUINT_TO_POINTER(1));
            priv->invalidated_rows.size++;
            g_free(mnn);
            g_free(trade_name);
            g_free(pack_form);
            g_free(barcode);
            g_free(manufactor);
        }
        //!!
        //break;
        do_ret_list_free(list);
    }
    if (list)
        do_ret_list_free(list);
    do_extended_free(extended);

    do_alias_free(alias);

    g_object_notify (G_OBJECT (embed), "load-progress");
    g_object_notify (G_OBJECT (embed), "load-status");

    model_filter = priv->model_filter = (GtkTreeModelFilter*)gtk_tree_model_filter_new(GTK_TREE_MODEL(model), NULL);

    gtk_tree_model_filter_set_visible_func(model_filter,
            (GtkTreeModelFilterVisibleFunc)do_rz_view_model_filter_visible,
            embed, NULL);

    return GTK_TREE_MODEL(model);
}

static void do_rz_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoRzView *view)
{
    do_rz_view_view_do_edit(DO_VIEW(view), NULL);
}
static void do_rz_view_cell_link_data_func(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{

	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(data);
    GValue link  = { 0, };
    gtk_tree_model_get_value(tree_model, iter, COLUMN_LINK, &link);

    if (g_hash_table_size(priv->invalidated_rows.h)) {
        gint pr;
        if (g_hash_table_lookup(priv->invalidated_rows.h, iter->user_data) != NULL)
            g_hash_table_remove(priv->invalidated_rows.h, iter->user_data);

        if (priv->invalidated_rows.size) {
            pr = ((priv->invalidated_rows.size - g_hash_table_size(priv->invalidated_rows.h))
                / (priv->invalidated_rows.size*1.))*100;
            if (priv->progress < pr) {
                priv->progress = pr;
                g_object_notify (G_OBJECT (data), "load-progress");
            }
        }
        if (g_hash_table_size(priv->invalidated_rows.h) == 0) {
            g_hash_table_remove_all(priv->invalidated_rows.h);
            priv->progress = 100;
            g_object_notify (G_OBJECT (data), "load-progress");
            g_object_notify (G_OBJECT (data), "load-status");
        }
    }


    switch (g_value_get_int(&link)) {
        case RZ_INFO_LINK_NO:
            g_object_set(cell, "cell-background-set", FALSE, NULL);
            g_object_set(cell, "text", "", NULL);
            break;
        case RZ_INFO_LINK_OK:
            g_object_set(cell, "cell-background-set", FALSE, NULL);
            g_object_set(cell, "text", "@", NULL);
            break;
        case RZ_INFO_LINK_FAIL:
            g_object_set(cell, "cell-background-set", TRUE, NULL);
            g_object_set(cell, "text", "!@", NULL);
            break;
    };
}
static gboolean do_rz_view_filter_timeout(DoRzView *view)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
	GtkTreeIter iter;
	GValue      value = {0,};
    GtkTreeModel *model;
    priv->filter_timeout = 0;

    gint i;

    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(priv->filter_combo), &iter)) {
        GdkCursor* cursor;
		model = gtk_combo_box_get_model (GTK_COMBO_BOX(priv->filter_combo));
		gtk_tree_model_get_value (model, &iter, 0, &value);
		priv->filter_indx = g_value_get_int(&value);
        cursor = gdk_cursor_new(GDK_WATCH);
        gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(view)), cursor);
        g_object_unref(cursor);
    }
    else {
        priv->filter_indx = -1;
    }
    if (!priv->filter_indx) {
        gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER(priv->model_filter));
        model = (GtkTreeModel*)priv->model_sort;
        gtk_tree_view_set_model(priv->tree_view, model);
        for (i = 0; i < N_COLUMNS; i++)
            if (priv->columns[i])
                gtk_tree_view_column_set_sort_column_id (priv->columns[i], i);
        gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(priv->model_sort), COLUMN_TRADE_NAME, GTK_SORT_ASCENDING);
        gtk_tree_view_set_search_column(priv->tree_view, COLUMN_TRADE_NAME);
    }
    else {
        gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(priv->model_sort), COLUMN_BARCODE, GTK_SORT_ASCENDING);
        model = (GtkTreeModel*)priv->model_filter;
        gtk_tree_view_set_model(priv->tree_view, model);
        gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER(priv->model_filter));
        for (i = 0; i < N_COLUMNS; i++)
            if (priv->columns[i])
                gtk_tree_view_column_set_sort_column_id (priv->columns[i], -1);
        gtk_tree_view_set_search_column(priv->tree_view, COLUMN_TRADE_NAME);
    }
    if (priv->filter_indx <= 0)
        gtk_tree_view_move_column_after(priv->tree_view, priv->columns[COLUMN_BARCODE], priv->columns[COLUMN_MANUFACTOR]);
    else
        gtk_tree_view_move_column_after(priv->tree_view, priv->columns[COLUMN_BARCODE], NULL);

    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(view)), NULL);
    return FALSE;

}
static void do_rz_view_filter_change(GtkComboBox *widget, DoRzView *view)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);

    if (priv->filter_timeout)
        g_source_remove (priv->filter_timeout);

    priv->filter_timeout =
      gdk_threads_add_timeout(FILTER_TIMEOUT_SEC,
         (GSourceFunc)do_rz_view_filter_timeout, view);
}
typedef struct {
    const gchar *barcode;
    GtkTreePath *path;
}DoRzViewBarcodePath;

static gboolean  do_rz_view_find_barcode(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, DoRzViewBarcodePath *barcode)
{
    GValue value = {0,};
    gtk_tree_model_get_value(model, iter, COLUMN_BARCODE, &value);
    if (!strcmp(g_value_get_string(&value), barcode->barcode)) {
        barcode->path = gtk_tree_path_copy(path);
        return TRUE;
    }
    return FALSE;
}
static void do_rz_view_search_find(DoSearch *search, gboolean direct, gint creterion, const gchar *text, DoRzView *view)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
    DoRzViewBarcodePath bpath;
    GtkTreeModel *model;
    GtkTreePath *path;

    model = gtk_tree_view_get_model(priv->tree_view);

    if (!text || *text == '\0')
        return;
    bpath.barcode = text;
    bpath.path = NULL;

    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        gtk_tree_model_foreach(model, (GtkTreeModelForeachFunc)do_rz_view_find_barcode, &bpath);
    else {
        GtkTreeIter iter;
        if (direct)
            gtk_tree_path_next(path);
        else
            gtk_tree_path_prev(path);
        while (gtk_tree_model_get_iter(model, &iter, path)) {
            if (do_rz_view_find_barcode(model, path, &iter, &bpath))
                break;
            if (direct)
                gtk_tree_path_next(path);
            else
                if (!gtk_tree_path_prev(path))
                    break;
        }
        gtk_tree_path_free(path);
        if (!bpath.path) {
            if (direct)
                path = gtk_tree_path_new_first();
            else
                path = gtk_tree_path_new_last(model);
            while (gtk_tree_model_get_iter(model, &iter, path)) {
                if (do_rz_view_find_barcode(model, path, &iter, &bpath))
                    break;
                if (direct)
                    gtk_tree_path_next(path);
                else
                    if (!gtk_tree_path_prev(path))
                        break;
            }
            gtk_tree_path_free(path);
        }
    }
    if (bpath.path) {
        gtk_widget_grab_focus(GTK_WIDGET(priv->tree_view));
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(priv->tree_view), bpath.path, NULL,  TRUE, 0.5, 0.0);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(priv->tree_view), bpath.path, NULL, FALSE);
    }
    else {
        DOMINO_SHOW_ERROR("Запись с продажный кодом <b>%s</b> не найдена", text);
    }
}
#define DO_RZ_VIEW_POPUP "rz-view-popup"

static gchar *popup_ui =
    "  <popup name='" DO_RZ_VIEW_POPUP "'>\n"
    "    <menuitem action='InsertAction'/>\n"
    "    <menuitem action='EditAction'/>\n"
    "    <menuitem action='CopyAction'/>\n"
    "    <menuitem action='DeleteAction'/>\n"
    "  </popup>\n";

static GObject *do_rz_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkListStore        *model;
    DoTreeView          *view;
	DoRzView            *embed;
	GtkTreeView         *tree_view;
	DoRzViewPrivate     *priv;
    GtkWidget           *filter;
    GtkWidget           *search;
    GtkListStore        *filter_model;
    GtkTreeIter          iter;
    GtkWidget           *w;
    GtkWidget           *l;

	object = G_OBJECT_CLASS (do_rz_view_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_RZ_VIEW_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_RZ_VIEW(object);
	g_object_get(object, "tree-view", &tree_view, NULL);

	do_view_actions_add_ui_from_string(popup_ui, NULL);

    g_signal_connect (tree_view, "row-activated", G_CALLBACK(do_rz_view_row_activated), object);

	priv->tree_view = tree_view;
    priv->selection = gtk_tree_view_get_selection(tree_view);

    priv->invalidated_rows.h = g_hash_table_new(NULL, NULL);

	priv->model_sort = model = GTK_LIST_STORE(do_rz_view_create_model(embed));
	if (model)
        g_object_set(view, "model", model,
                           "fixed-height-mode", FALSE,
                           NULL);

    col = priv->columns[COLUMN_LINK]
        = do_tree_view_add_column(view, "Link", "", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_LINK);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_LINK);
    g_object_set(renderer, "cell-background", "red", "cell-background-set", FALSE, NULL);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
            do_rz_view_cell_link_data_func, object, NULL);

    col = priv->columns[COLUMN_IN]
        = do_tree_view_add_column(view, "In", "", DEFAULT_COL_DEFAULT_WIDTH);
    renderer = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_IN);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_pixbuf_bool, GINT_TO_POINTER(COLUMN_IN), NULL);

    col = priv->columns[COLUMN_DATE]
        = do_tree_view_add_column(view, "Date", "Дата", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_DATE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_DATE);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_date_time_date_only, GINT_TO_POINTER(COLUMN_DATE), NULL);

    col = priv->columns[COLUMN_MNN]
        = do_tree_view_add_column(view, "MNN", "МНН", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_MNN);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_MNN);
    g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);
//    gtk_tree_view_column_set_cell_data_func(col ,renderer,
  //           do_rz_view_cell_data_func, GINT_TO_POINTER(COLUMN_MNN), NULL);

    col = priv->columns[COLUMN_TRADE_NAME]
        = do_tree_view_add_column(view, "TradeName", "Торговое наименование", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_TRADE_NAME);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_TRADE_NAME);
    g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);

    col = priv->columns[COLUMN_PACK_FORM]
        = do_tree_view_add_column(view, "PackForm", "Форма выпуска", DEFAULT_COL_PACK_FORM_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_PACK_FORM);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_PACK_FORM);
    g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);

    col = priv->columns[COLUMN_PACK_QUANT]
        = do_tree_view_add_column(view, "PackQuant", "Количество", DEFAULT_COL_PACK_FORM_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_PACK_QUANT);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_PACK_QUANT);

    col = priv->columns[COLUMN_UNPACK]
        = do_tree_view_add_column(view, "Unpack", "Цена перв.уп.", DEFAULT_COL_PACK_FORM_WIDTH);

    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
//    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_UNPACK);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_UNPACK);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_pixbuf_warning, GINT_TO_POINTER(COLUMN_UNPACK), NULL);

    col = priv->columns[COLUMN_PRICE]
        = do_tree_view_add_column(view, "Price", "Цена", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_PRICE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_PRICE);
    gtk_tree_view_column_set_cell_data_func(col ,renderer,
             do_tree_cell_data_func_money_with_sign, GINT_TO_POINTER(COLUMN_PRICE), NULL);

    col = priv->columns[COLUMN_MANUFACTOR]
        = do_tree_view_add_column(view, "Manufactor", "Производитель", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_MANUFACTOR);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_MANUFACTOR);
    g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, "wrap-width", gtk_tree_view_column_get_fixed_width(col), NULL);

    col = priv->columns[COLUMN_BARCODE]
        = do_tree_view_add_column(view, "Barcode", "Штрихкод", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_BARCODE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_BARCODE);

    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(model), COLUMN_TRADE_NAME, GTK_SORT_ASCENDING);

    gtk_tree_view_set_search_column(tree_view, COLUMN_TRADE_NAME);

    filter =  do_filter_new(GTK_WIDGET(object), 0);
    do_tree_view_pack_header(DO_TREE_VIEW(object), filter);
	gtk_widget_show(filter);
	g_object_get(filter, "combo", &w, "entry", &l, NULL);
	filter_model = GTK_LIST_STORE(gtk_combo_box_get_model (GTK_COMBO_BOX(w)));
	priv->filter_entry = l;
	priv->filter_combo = w;
    gtk_widget_set_size_request(w,
                300u, -1u);
    //g_object_set(l, "editable", FALSE, NULL);
    gtk_list_store_append (filter_model, &iter);
    gtk_list_store_set (filter_model, &iter,
                             0, 0,
                             1, "Все",
                             -1);
    gtk_list_store_append (filter_model, &iter);
    gtk_list_store_set (filter_model, &iter,
                             0, 1,
                             1, "Только дублирующие продажные кода",
                             -1);
    gtk_list_store_append (filter_model, &iter);
    gtk_list_store_set (filter_model, &iter,
                             0, 2,
                             1, "Непомеченные как ЖВНЛС",
                             -1);
    g_signal_connect(w, "changed", G_CALLBACK(do_rz_view_filter_change), object);

    search = priv->search = do_search_new(GTK_WIDGET(object), DO_SEARCH_FLAGS_SHOW_LABEL | DO_SEARCH_FLAGS_SHOW_BUTTONS);
    do_tree_view_pack_footer(DO_TREE_VIEW(object), search);
    g_signal_connect(search, "find", G_CALLBACK(do_rz_view_search_find), object);
    do_search_add_criterion(DO_SEARCH(search), "Штрихкод");

	gtk_widget_show(GTK_WIDGET(object));
	return object;
}

static void do_rz_view_finalize (GObject *object)
{
    DoRzViewPrivate *priv;
	priv = DO_RZ_VIEW_GET_PRIVATE (object);
    g_hash_table_destroy(priv->invalidated_rows.h);
    if (priv->filter_timeout)
        g_source_remove (priv->filter_timeout);
	if (priv->alias_name)
		g_free(priv->alias_name);
    if (priv->alias)
        do_alias_free(priv->alias);


	G_OBJECT_CLASS (do_rz_view_parent_class)->finalize (object);
}

static void do_rz_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            g_value_set_string (value, priv->alias_name);
			break;
        case PROP_LOAD_STATUS:
            g_value_set_boolean(value, g_hash_table_size(priv->invalidated_rows.h) > 0);
            break;
        case PROP_LOAD_PROGRESS:
            g_value_set_int(value, priv->progress);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
	/* no readable properties */
	g_assert_not_reached ();
}

static void do_rz_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string (value);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
}

static void do_rz_view_class_init (DoRzViewClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_rz_view_constructor;
	o_class->finalize     = do_rz_view_finalize;
	o_class->get_property = do_rz_view_get_property;
	o_class->set_property = do_rz_view_set_property;

	g_type_class_add_private (o_class, sizeof (DoRzViewPrivate));

    g_object_class_install_property
        (o_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (o_class,
				   PROP_LOAD_STATUS,
				   g_param_spec_boolean("load-status",
							"Статус загрузки",
							"Справочник загружается",
							TRUE,
							G_PARAM_READABLE));

    g_object_class_install_property (o_class,
				   PROP_LOAD_PROGRESS,
				   g_param_spec_int("load-progress",
							"Статус загрузки",
							"Справочник загружается",
							0,100,0,
							G_PARAM_READABLE));
}

GtkWidget *do_rz_view_new (const gchar *alias_name)
{
	return g_object_new (DO_TYPE_RZ_VIEW,
			     "alias-name", alias_name,
			     "object-root-path", OBJECT_ROOT_PATH,
			     (gpointer) NULL);
}
static gboolean do_rz_view_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void	do_rz_view_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_rz_view_view_get_title(DoView *view)
{
    return "Зарегистрированные ";
}
static GdkPixbuf  *do_rz_view_view_get_icon(DoView *view)
{
    DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
    return priv->icon;
}
static gboolean	do_rz_view_view_can_do_edit(DoView *view, const gchar *tab)
{
    if (tab && *tab != '\0')
        return FALSE;
    gboolean retval = FALSE;
    GtkTreeModel *model;
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
    GList *list = gtk_tree_selection_get_selected_rows(priv->selection, &model);
    if (list) {
        retval = (g_list_length(list) == 1);
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    else
        retval = FALSE;
    return retval;
}
static gboolean	do_rz_view_view_can_do_action(DoView *view)
{
    gboolean retval = FALSE;
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
	GtkTreeModel *model;
    GList *list = gtk_tree_selection_get_selected_rows(priv->selection, &model);
    if (list) {
        retval = (g_list_length(list) == 1);
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    else
        retval = FALSE;
    return retval;
}

static void do_rz_view_show_product_stock(GtkWidget *parent, const gchar *alias_name, const gchar *tab, const gchar *product_code, const gchar *barcode)
{
    do_alias_t *alias;
    barcode_rec_t barcode_;
    barcode_key0_t barcode_key0;
    gchar *code;

    if (!product_code) {

        alias = domino_alias_new(alias_name);
        if (!alias || !do_alias_open(alias, 0, DO_DB_BARCODE, DO_DB_END))
            return;

        do_text_set(alias, barcode_key0.barcode, barcode);
        if (do_barcode_get0(alias, &barcode_, &barcode_key0, DO_GET_EQUAL) != DO_OK) {
            DOMINO_SHOW_ERROR("Товар с продажный кодом <b>%s</b> не найдена", barcode);
            return;
        }
        code = do_text(alias, barcode_.data.code);
        do_alias_free(alias);
    }
    else
        code = g_strdup(product_code);

    DoContext *context = do_context_new();
    do_context_set_kind(context, DO_CONTEXT_KIND_PRODUCT);
    g_object_set(context, "product-code", code, NULL);
    g_free(code);

    GtkWidget *dialog = do_product_edit_new(NULL, alias_name,
                                            DO_PRODUCT_EDIT_FLAGS_READ_ONLY,
                                              DO_DIALOG_ACTION_NONE, context, domino_unit());
    g_object_set(dialog, "view", parent, "tab", tab, NULL);
	GtkNotebook *nb;
	nb = GTK_NOTEBOOK (do_window_get_notebook (
	                    DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(parent)))));
    do_notebook_add_tab(DO_NOTEBOOK(nb), DO_VIEW(dialog), -1, TRUE);
    g_object_unref(context);
}

static void	do_rz_view_show_record(DoView *view, DoDialogAction action, gboolean make_link)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
    GtkTreePath *path;
    GtkTreeIter iter;
    GValue dtype = { 0, };
    GValue sklad = { 0, };
    GValue document = { 0, };
    GValue line = { 0, };
    gint flags = 0;
    GtkTreeModel *model;

    if (make_link)
        flags = DO_RZ_EDIT_FLAGS_LINK_ONLY;


    model = gtk_tree_view_get_model(GTK_TREE_VIEW(priv->tree_view));
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        return;

    if (!gtk_tree_model_get_iter(model, &iter, path)) {
        gtk_tree_path_free(path);
        return;
    }
    gtk_tree_model_get_value(model, &iter, COLUMN_DTYPE, &dtype);
    gtk_tree_model_get_value(model, &iter, COLUMN_SKLAD, &sklad);
    gtk_tree_model_get_value(model, &iter, COLUMN_DOCUMENT, &document);
    gtk_tree_model_get_value(model, &iter, COLUMN_LINE, &line);
    gtk_tree_path_free(path);
    DoContext *context;
    GtkWidget *dialog;
  	GtkNotebook *nb;

    context = do_context_new_with_attributes(DO_CONTEXT_KIND_LINE,
     "dtype-code", g_value_get_string(&dtype),
     "sklad-code", g_value_get_string(&sklad),
     "document-number", g_value_get_string(&document),
     "line-number", g_value_get_int(&line),
     NULL);


    dialog = do_rz_edit_view_new(view, priv->alias_name, flags, action, context);
    g_object_set(dialog, "view", view, NULL);
    if (action != DO_DIALOG_ACTION_DELETE) {
        nb = GTK_NOTEBOOK (do_window_get_notebook (
	                    DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view)))));
        do_notebook_add_tab(DO_NOTEBOOK(nb), DO_VIEW(dialog), -1, TRUE);
        if (action == DO_DIALOG_ACTION_EDIT)
            do_rz_edit_ck_link((DoRzEdit*)dialog);

    }
    else {
        if (!do_rz_edit_accept(DO_RZ_EDIT(dialog)))
            gtk_widget_destroy(dialog);
    }
    g_object_unref(context);
}

static void	do_rz_view_view_do_edit(DoView *view, const gchar *tab)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
    if (!tab || strcmp(tab, "Stock"))
        do_rz_view_show_record(view, DO_DIALOG_ACTION_EDIT, FALSE);
    else {
        GtkTreeIter iter;
        GtkTreePath *path;
        GtkTreeModel *model;
        GValue value = {0,};
        model = gtk_tree_view_get_model(priv->tree_view);
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
        if (!path)
            return;
        if (!gtk_tree_model_get_iter(model, &iter, path)) {
            gtk_tree_path_free(path);
            return;
        }
        gtk_tree_model_get_value(model, &iter, COLUMN_BARCODE, &value);
        do_rz_view_show_product_stock(GTK_WIDGET(view), priv->alias_name, tab, NULL, g_value_get_string(&value));
    }
}
static void	do_rz_view_view_do_delete(DoView *view)
{
    do_rz_view_show_record(view, DO_DIALOG_ACTION_DELETE, FALSE);
}
static void	do_rz_view_view_do_insert(DoView *view)
{
    do_rz_view_show_record(view, DO_DIALOG_ACTION_INSERT, TRUE);
}
static void	do_rz_view_view_do_copy(DoView *view)
{
    do_rz_view_show_record(view, DO_DIALOG_ACTION_INSERT, FALSE);
}
static gboolean	do_rz_view_view_can_do_find_barcode(DoView *view)
{
    return TRUE;
}

static void do_rz_view_view_do_find_barcode(DoView *view)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
	do_search_show(DO_SEARCH(priv->search), 0);
}

typedef struct {
    GtkTreeModel *model;
    gchar        *dtype;
    gchar        *sklad;
    gchar        *document;
    gint          line;
    GtkTreePath  *path;
} DoRzViewFindKeyInfo;

typedef struct
{
  gint depth;
  gint *indices;
} MGtkTreePath;

static gboolean  do_rz_view_find_by_key(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter,
                                                         DoRzViewFindKeyInfo *info)
{
    GValue dtype = {0,};
    GValue sklad = {0,};
    GValue document = {0,};
    GValue line = {0,};
    gtk_tree_model_get_value(model, iter, COLUMN_DTYPE, &dtype);
    gtk_tree_model_get_value(model, iter, COLUMN_SKLAD, &sklad);
    gtk_tree_model_get_value(model, iter, COLUMN_DOCUMENT, &document);
    gtk_tree_model_get_value(model, iter, COLUMN_LINE, &line);
    if (
         !strcmp(info->dtype, g_value_get_string(&dtype)) &&
         !strcmp(info->sklad, g_value_get_string(&sklad)) &&
         !strcmp(info->document, g_value_get_string(&document)) &&
         info->line == g_value_get_int(&line)
       ) {
        info->path = gtk_tree_path_copy(path);
        return TRUE;
    }
    return FALSE;
}

static GtkTreePath *do_rz_view_get_path(DoRzView *view, const gchar *dtype, const gchar *sklad, const gchar *document, gint line)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
    DoRzViewFindKeyInfo info;

    info.model = gtk_tree_view_get_model(priv->tree_view);
    info.dtype = (gchar*)dtype;
    info.sklad = (gchar*)sklad;
    info.document = (gchar*)document;
    info.line = line;
    info.path = NULL;

    gtk_tree_model_foreach(info.model, (GtkTreeModelForeachFunc)do_rz_view_find_by_key, &info);

    return info.path;
}

static gboolean do_rz_view_remove_record(DoRzView *view, GtkTreePath *path)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
	GtkTreeIter iter;
	GtkTreeModel *model;

    model = gtk_tree_view_get_model(priv->tree_view);
	gtk_tree_model_get_iter(model, &iter, path);

	return gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
}
static gboolean do_rz_view_append_record(DoRzView *view, do_alias_t *alias, rz_info_t *rz)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(priv->tree_view);

    gtk_list_store_append (GTK_LIST_STORE(model), &iter);
    gtk_list_store_set (GTK_LIST_STORE(model), &iter,
                                COLUMN_DTYPE, rz->key.dtype,
                                COLUMN_SKLAD, rz->key.sklad,
                                COLUMN_DOCUMENT, rz->key.document,
                                COLUMN_LINE, rz->key.line,

                                COLUMN_IN, do_rz_view_find_product(alias, rz->barcode),
                                COLUMN_DATE, rz->order.time,
                                COLUMN_MNN, rz->mnn,
                                COLUMN_TRADE_NAME, rz->trade_name,
                                COLUMN_PACK_FORM, rz->pack_form,
                                COLUMN_PACK_QUANT, rz->pack_quant,
                                COLUMN_UNPACK, rz->unpack,
                                COLUMN_PRICE, rz->price,
                                COLUMN_VALUTA, rz->valuta,
                                COLUMN_BARCODE, rz->barcode,
                                COLUMN_MANUFACTOR, rz->manufactor,
                                COLUMN_LINK, rz->link.type,
                                -1);
	return TRUE;
}

static gboolean do_rz_view_update_record(DoRzView *view, do_alias_t *alias, GtkTreePath *path, rz_info_t *rz)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
	GtkTreeIter iter;
    GtkTreeModel *model;

    model = gtk_tree_view_get_model(priv->tree_view);
    if (!gtk_tree_model_get_iter(model, &iter, path))
        return FALSE;
    gtk_list_store_set (GTK_LIST_STORE(model), &iter,
                                COLUMN_DTYPE, rz->key.dtype,
                                COLUMN_SKLAD, rz->key.sklad,
                                COLUMN_DOCUMENT, rz->key.document,
                                COLUMN_LINE, rz->key.line,

                                COLUMN_MNN, rz->mnn,
                                COLUMN_TRADE_NAME, rz->trade_name,
                                COLUMN_PACK_FORM, rz->pack_form,
                                COLUMN_PACK_QUANT, rz->pack_quant,
                                COLUMN_UNPACK, rz->unpack,
                                COLUMN_PRICE, rz->price,
                                COLUMN_VALUTA, rz->valuta,
                                COLUMN_BARCODE, rz->barcode,
                                COLUMN_MANUFACTOR, rz->manufactor,
                                COLUMN_LINK, rz->link.type,
                                -1);
	return TRUE;
}

static void do_rz_view_view_update_context(DoView *view, DoContext *context)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
    gchar *dtype;
    gchar *sklad;
    gchar *document;
    gint   line;
    rz_info_t *rz;
    do_alias_t *alias;
    GtkTreePath *path;

    if (do_context_get_kind(context) != DO_CONTEXT_KIND_LINE)
        return;

    g_object_get(context, "dtype-code", &dtype, "sklad-code", &sklad, "document-number", &document, "line-number", &line, NULL);

    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_PRODUCT, DO_DB_BARCODE, DO_DB_END)) {
        if (alias)
            do_alias_free(alias);
            return;
    }


    rz   = do_rz_info_new(alias, dtype, sklad, document, line);
    path = do_rz_view_get_path(DO_RZ_VIEW(view), dtype, sklad, document, line);

    if (!rz) {
        if (path)
            do_rz_view_remove_record(DO_RZ_VIEW(view), path);
    }
    else {
        if (path)
            do_rz_view_update_record(DO_RZ_VIEW(view), alias, path, rz);
        else
            do_rz_view_append_record(DO_RZ_VIEW(view), alias, rz);
    }
    if (rz)
        do_rz_info_free(rz);
    if (path)
        gtk_tree_path_free(path);
    do_alias_free(alias);
}
static void	do_rz_view_view_do_refresh(DoView *view)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);

    GdkCursor* cursor;

    int i;
    cursor = gdk_cursor_new(GDK_WATCH);
    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(view)), cursor);
    g_object_unref(cursor);

	gtk_list_store_clear(priv->model_sort);
	g_object_unref(priv->model_sort);
	priv->model_sort = GTK_LIST_STORE(do_rz_view_create_model(DO_RZ_VIEW(view)));
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(priv->model_sort), COLUMN_TRADE_NAME, GTK_SORT_ASCENDING);
	priv->filter_indx = 0;
	gtk_entry_set_text(GTK_ENTRY(priv->filter_entry), "");
	gtk_tree_view_set_model(priv->tree_view, (GtkTreeModel*)priv->model_sort);
    for (i = 0; i < N_COLUMNS; i++)
        if (priv->columns[i])
            gtk_tree_view_column_set_sort_column_id (priv->columns[i], i);

    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(priv->model_sort), COLUMN_TRADE_NAME, GTK_SORT_ASCENDING);
    gtk_tree_view_move_column_after(priv->tree_view, priv->columns[COLUMN_BARCODE], priv->columns[COLUMN_MANUFACTOR]);
    gtk_tree_view_set_search_column(priv->tree_view, COLUMN_TRADE_NAME);


    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(view)), NULL);
}
static gboolean	do_rz_view_view_can_do_refresh(DoView *view)
{
    return TRUE;
}
static void do_rz_view_view_set_cursor_to_context(DoView *view, DoContext *context)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(view);
    gchar *dtype;
    gchar *sklad;
    gchar *document;
    gint   line;
    GtkTreePath *path;

    if (do_context_get_kind(context) != DO_CONTEXT_KIND_LINE)
        return;

    g_object_get(context, "dtype-code", &dtype, "sklad-code", &sklad, "document-number", &document, "line-number", &line, NULL);
    path = do_rz_view_get_path(DO_RZ_VIEW(view), dtype, sklad, document, line);

    if (path) {
        gtk_tree_view_set_cursor(priv->tree_view, path, NULL, FALSE);
        gtk_tree_path_free(path);
    }
}
static gboolean	do_rz_view_view_can_set_cursor_to_context(DoView *view, DoContext *context)
{
    return TRUE;
}
static gboolean	 do_rz_view_view_get_load_status	(DoView *embed)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(embed);
    return g_hash_table_size(priv->invalidated_rows.h) > 0;
}
static gint	do_rz_view_view_get_load_progress	(DoView *embed)
{
	DoRzViewPrivate *priv = DO_RZ_VIEW_GET_PRIVATE(embed);
    return priv->progress;
}
static void	do_rz_view_view_do_popup(DoView *view, GdkEventButton* event)
{
    GtkWidget * menu = do_view_actions_action_get_widget("/" DO_RZ_VIEW_POPUP);
    do_tree_view_popup(DO_TREE_VIEW(view), event, menu);
}
