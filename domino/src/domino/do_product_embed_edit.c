
#include "do_product_embed_edit.h"
#include "do_view.h"
#include "do_utilx.h"
#include "domino.h"
#include "do_product_edit.h"
#include <domino.h>
#include "do_icons.h"
#include <gdk/gdk.h>

#define DO_PRODUCT_EMBED_EDIT_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_PRODUCT_EMBED_EDIT, DoProductEmbedEditPrivate))

#define OBJECT_ROOT_PATH "ProductEmbedEdit"
#define DEFAULT_COL_CODE_WIDTH 60
#define DEFAULT_COL_NAME_WIDTH 400
#define DEFAULT_COL_DEFAULT_WIDTH 60


static void do_product_embed_edit_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoProductView *view);

// View interface
static void do_product_embed_edit_view_init(DoViewIface *iface);

static gboolean    do_product_embed_edit_view_do_grab_focus(DoView *view);
static void	       do_product_embed_edit_view_do_close(DoView *view);
static const char *do_product_embed_edit_view_get_title(DoView *view);
static GdkPixbuf  *do_product_embed_edit_view_get_icon(DoView *view);
static void		   do_product_embed_edit_view_do_edit(DoView *view, const gchar *tab);
static gboolean	   do_product_embed_edit_view_can_do_edit(DoView *view, const gchar *tab);
static void	       do_product_embed_edit_view_do_popup(DoView *view, GdkEventButton* event);

enum
{
	PROP_0,
	PROP_ALIAS,
	PROP_MAIN_ALIAS,
};

enum
{
    COLUMN_CODE = 0,
    COLUMN_NAME,
    COLUMN_NDS,
    //COLUMN_LIST,
    COLUMN_CLASS,
    COLUMN_TEMPERATURE,
    COLUMN_IMPERATIVE,
    N_COLUMNS,
};
enum
{
    STORE_NDS,
    STORE_LIST,
    STORE_CLASS,
    STORE_TEMPERATURE,
    STORE_IMPERATIVE,
    N_STORES,
};

struct _DoProductEmbedEditPrivate
{
    GtkListStore        *model_list;
    GtkTreeModelFilter  *model_filter;
    GtkTreeModelSort    *model_sort;

    GtkWidget           *filter;

    GtkTreeView         *tree_view;
    GtkTreeSelection    *sel;
    GtkTreeViewColumn   *columns[N_COLUMNS];
    GdkPixbuf           *icon;
    gchar               *alias_name;
    gchar               *main_alias_name;
    GtkListStore        *stores[N_STORES];
    do_alias_t          *main_alias;
};

G_DEFINE_TYPE_WITH_CODE (DoProductEmbedEdit, do_product_embed_edit, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_product_embed_edit_view_init)
                                    );

static void do_product_embed_edit_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_product_embed_edit_view_do_grab_focus;
    iface->do_close      = do_product_embed_edit_view_do_close;
    iface->get_title     = do_product_embed_edit_view_get_title;
    iface->get_icon      = do_product_embed_edit_view_get_icon;
    iface->do_edit       =  do_product_embed_edit_view_do_edit;
    iface->can_do_edit   =  do_product_embed_edit_view_can_do_edit;
    iface->do_popup      =  do_product_embed_edit_view_do_popup;
}

static void do_product_embed_edit_init(DoProductEmbedEdit *view)
{
	DoProductEmbedEditPrivate *priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE (view);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, DO_STOCK_DOMINO_MED, DO_VIEW_ICON_SIZE, 0, NULL);
}
typedef struct {
    gchar *code;
    gchar *name;
    gchar *params;
} DoProductEmbedEditItem;

static int do_product_embed_edit_name_cmp(void *s1, void *s2, void *user_data)
{
    return strcmp(((DoProductEmbedEditItem*)s1)->name, ((DoProductEmbedEditItem*)s2)->name);
}

static GtkListStore *do_product_embed_edit_create_model(DoProductEmbedEdit *embed)
{
	DoProductEmbedEditPrivate *priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE(embed);
    GtkListStore  *model;
    do_alias_t    *alias;
    product_key0_t product_key0;
    do_ret_list_t *list;
    do_extended_t *extended;
    do_sort_list_t   *sort;
    do_sort_list_t   *nds_list;
    do_sort_list_t   *list_list;
    do_sort_list_t   *class_list;
    do_sort_list_t   *temperature_list;
    do_sort_list_t   *imperative_list;
    gint           nds_code;
    //gint           list_code;
    gint           class_code;
    gint           temperature_code;
    gint           imperative_code;

    DoProductEmbedEditItem *item;
    GtkTreeIter iter;
    gchar *name;
    int indx;
    struct {
        BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
        BTI_CHAR name[DOMINO_FIELD_PRODUCT_NAME_SIZE];
        BTI_CHAR params[2048];                          //422
    } *retstruct;

    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_END))
        return NULL;

    model = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, /*G_TYPE_STRING,*/ G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    sort = do_sort_list_new(1, 0, do_product_embed_edit_name_cmp, NULL);
    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_PRODUCT);
    do_extended_set_key(extended, 0, &product_key0);
    do_extended_set_filter(extended, TRUE, 1, sizeof(product_key0.base_parcel));
    do_text_set_empty(product_key0.code);
    product_key0.base_parcel = 0;
    do_extended_set_filter_term(extended, 0, DO_EXTENDED_TYPE_INTEGER,
                                offsetof(product_struct_t, base_parcel), DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO, &product_key0.base_parcel);
    do_extended_set_fields(extended, 3,
                           do_size_and_offset(product_struct_t, code),
                           do_size_and_offset(product_struct_t, name),
                           do_size_and_offset(product_struct_t, params)
                           );

    while ( ((list = do_extended_get(extended, 26)) != NULL) &&
            do_ret_list_count(list) ) {
        for (indx = 0; indx < do_ret_list_count(list); indx++) {
            retstruct = do_ret_list_item_data(list, indx);
            item = (DoProductEmbedEditItem *)g_malloc(sizeof(DoProductEmbedEditItem));
            item->code = do_text(alias, retstruct->code);
            item->name = do_text(alias, retstruct->name);
            item->params = do_alias_text_(alias, retstruct->params,
                              do_ret_list_item_len(list, indx) -
                              sizeof(retstruct->code) -
                              sizeof(retstruct->name));
            do_sort_list_add(sort, item);
        }
    }
    if (list)
        do_ret_list_free(list);
    do_extended_free(extended);

    priv->stores[STORE_NDS] = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    priv->stores[STORE_CLASS] = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    priv->stores[STORE_LIST] = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    priv->stores[STORE_TEMPERATURE] = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    priv->stores[STORE_IMPERATIVE] = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);

    nds_list = do_property_list_create_with_store("НДС", priv->stores[STORE_NDS]);
    list_list = do_property_list_create_with_store("Список", priv->stores[STORE_LIST]);
    class_list = do_property_list_create_with_store("ЖВ", priv->stores[STORE_CLASS]);
    temperature_list = do_property_list_create_with_store("Режим", priv->stores[STORE_TEMPERATURE]);
    imperative_list = do_property_list_create_with_store("Необходимый", priv->stores[STORE_IMPERATIVE]);

    for (indx = 0; indx < sort->count; indx++) {
        item = sort->list[indx];
        name = do_fromparam(item->params, do_param(DO_PARAM_PRODUCT_PARAMETR_NDS), do_alias_utf8(alias));
        nds_code = atoi(name);
        do_free(name);
        name = do_fromparam(item->params, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS), do_alias_utf8(alias));
        class_code = atoi(name);
        do_free(name);
        name = do_fromparam(item->params, do_param(DO_PARAM_PRODUCT_PARAMETR_LIST), do_alias_utf8(alias));
/*        list_code = atoi(name);
        do_free(name);*/
        name = do_fromparam(item->params, do_param(DO_PARAM_PRODUCT_PARAMETR_TEMPERATURE), do_alias_utf8(alias));
        temperature_code = atoi(name);
        do_free(name);
        name = do_fromparam(item->params, do_param(DO_PARAM_PRODUCT_PARAMETR_IMPERATIVE), do_alias_utf8(alias));
        imperative_code = atoi(name);
        do_free(name);

        name = do_product_name_format(item->name);
        gtk_list_store_append (model, &iter);
        gtk_list_store_set (model, &iter,
                          COLUMN_CODE, item->code,
                          COLUMN_NAME, name,
                          COLUMN_NDS, do_property_list_get_name(nds_list, nds_code),
                          COLUMN_CLASS, do_property_list_get_name(class_list, class_code),
//                          COLUMN_LIST, do_property_list_get_name(list_list, list_code),
                          COLUMN_TEMPERATURE, do_property_list_get_name(temperature_list, temperature_code),
                          COLUMN_IMPERATIVE, do_property_list_get_name(imperative_list, imperative_code),
                          -1);
        g_free(name);
        g_free(item->name);
        g_free(item->code);
    }
    do_sort_list_free(sort);
    do_property_list_free(nds_list);
    do_property_list_free(class_list);
    do_property_list_free(list_list);
    do_property_list_free(temperature_list);
    do_property_list_free(imperative_list);
    do_alias_free(alias);
    return model;
}
static void do_product_embed_edit_cell_editing_canceled(GtkCellRenderer *renderer, DoProductEmbedEdit *embed)
{
	DoProductEmbedEditPrivate *priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE(embed);
    gtk_widget_grab_focus(GTK_WIDGET(priv->tree_view));
}
static gboolean do_product_embed_edit_find(GtkTreeModel *model,
                                           GtkTreePath *path,
                                           GtkTreeIter *iter,
                                           DoProperyItem *item)
{
    GValue value_name = { 0, };
    GValue value_code = { 0, };
    gtk_tree_model_get_value(model, iter, 1, &value_name);
    if (!strcmp(item->name, g_value_get_string(&value_name))) {
        gtk_tree_model_get_value(model, iter, 0, &value_code);
        item->code = g_value_get_int(&value_code);
        return TRUE;
    }
    return FALSE;
}
static void do_product_embed_edit_cell_edited(GtkCellRendererText *renderer, gchar *path_string, gchar *new_text, DoProductEmbedEdit *embed)
{
	DoProductEmbedEditPrivate *priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE(embed);
    GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
    GtkTreeIter iter;
    do_alias_t *alias = priv->main_alias;
    gint column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (renderer), "column"));
    product_key0_t product_key0;
    product_rec_t  product;
    GValue value = { 0, }, value_name = {0, };
    gboolean err;
    const char *name = "";
    DoProperyItem item;
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(priv->tree_view);

    gtk_tree_model_get_iter (GTK_TREE_MODEL(model), &iter, path);
    gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, COLUMN_CODE, &value);
    gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, COLUMN_NAME, &value_name);
    do_text_set(alias, product_key0.code, g_value_get_string(&value));
    product_key0.base_parcel = 0;

    switch (do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL)) {
        case DO_OK:
            err = FALSE;
            break;
        case DO_KEY_NOT_FOUND:
            err = TRUE;
            DOMINO_SHOW_ERROR("Товар с кодом \"%s\" не найден в главной базе", g_value_get_string(&value));
            break;
        case DO_ERROR:
            err = TRUE;
            DOMINO_SHOW_ERROR("Ошибка чтения главной базы");
            break;
        default:
            err = TRUE;
    }

    if ( !err ) {
        item.name = new_text;
        item.code = 0;
        gtk_tree_model_foreach(GTK_TREE_MODEL(priv->stores[column - COLUMN_NDS]), (GtkTreeModelForeachFunc)do_product_embed_edit_find, &item);

        switch (column) {
            case COLUMN_NDS:
                name = do_param(DO_PARAM_PRODUCT_PARAMETR_NDS);
                break;
            case COLUMN_CLASS:
                name = do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS);
                break;
/*            case COLUMN_LIST:
                name = do_param(DO_PARAM_PRODUCT_PARAMETR_LIST);
                break;*/
            case COLUMN_TEMPERATURE:
                name = do_param(DO_PARAM_PRODUCT_PARAMETR_TEMPERATURE);
                break;
            case COLUMN_IMPERATIVE:
                name = do_param(DO_PARAM_PRODUCT_PARAMETR_IMPERATIVE);
                break;
        }
        do_product_param_int_set(alias, &product, name, item.code);
        if ( (do_product_update(alias, &product) != DO_OK ) ||
            !do_protocol_product_add_rec(alias, &product, DO_PROTOCOL_ACTION_CHANGE)
            ) {
            err = TRUE;
            DOMINO_SHOW_ERROR("Ошибка записи главной базы");
        }
        /*FILE *fp;
        fp = fopen("log.txt", "a+");
        fprintf(fp, "%s %s\n", g_value_get_string(&value), g_value_get_string(&value_name));
        fclose(fp);*/

    }
    if (!err) {
        GtkTreeIter f_iter, s_iter;
        gtk_tree_model_sort_convert_iter_to_child_iter(priv->model_sort, &f_iter, &iter);
        gtk_tree_model_filter_convert_iter_to_child_iter(priv->model_filter, &s_iter, &f_iter);
        gtk_list_store_set (GTK_LIST_STORE (priv->model_list), &s_iter, column, new_text, -1);
    }
    gtk_widget_grab_focus(GTK_WIDGET(priv->tree_view));
}
#define DO_PRODUCT_EMBED_EDIT_POPUP "product-embed-edit-view-popup"

static gchar *popup_ui =
    "  <popup name='" DO_PRODUCT_EMBED_EDIT_POPUP "'>\n"
    "    <menuitem action='EditAction'/>\n"
    "  </popup>\n";

static gboolean do_product_embed_edit_model_filter_visible(GtkTreeModel *model,
                                                         GtkTreeIter *iter,
                                                         DoProductEmbedEdit *view)
{
    DoProductEmbedEditPrivate *priv;
	gint indx;
	GValue value = {0,};
    DoProperyItem item;
	priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE(view);

	indx = gtk_combo_box_get_active(GTK_COMBO_BOX(priv->filter));


	switch ( indx ) {
	    case 1:
	    case 2:
	    case 3:
            gtk_tree_model_get_value(model, iter, COLUMN_CLASS, &value);
            item.name = (char*)g_value_get_string(&value);
            item.code = 0;
            gtk_tree_model_foreach(GTK_TREE_MODEL(priv->stores[STORE_CLASS]), (GtkTreeModelForeachFunc)do_product_embed_edit_find, &item);
            if ( indx == 1)
                return (item.code == 1) || (item.code == 2);
            else {
            if ( indx == 2)
                return (item.code == 2) || (item.code == 5);
            else
                return (item.code == 0);
            };
        default:
            return TRUE;
    }
    return TRUE;
}
static void do_product_embed_edit_filter_change(GtkComboBox *widget, DoProductEdit *view)
{
    DoProductEmbedEditPrivate *priv;
	priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE(view);
    gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER(priv->model_filter));

}
static GObject *do_product_embed_edit_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject             *object;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkListStore        *model;
    DoTreeView          *view;
	DoProductEmbedEdit  *embed;
	GtkWidget           *filter;
	GtkTreeView         *tree_view;
	DoProductEmbedEditPrivate *priv;

	object = G_OBJECT_CLASS (do_product_embed_edit_parent_class)->constructor(type, n_construct_properties, construct_params);

	do_view_actions_add_ui_from_string(popup_ui, NULL);

	priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE(object);
	view = DO_TREE_VIEW(object);
	embed = DO_PRODUCT_EMBED_EDIT(object);
	g_object_get(object, "tree-view", &tree_view, NULL);
    priv->sel = gtk_tree_view_get_selection(tree_view);

	priv->tree_view = tree_view;
	priv->main_alias = domino_alias_new(priv->main_alias_name);

	if (!priv->main_alias || !do_alias_open(priv->main_alias, 0, DO_DB_PRODUCT, DO_DB_PROTOCOL, DO_DB_END))
        DOMINO_SHOW_ERROR("Невозможно открыть главную базу\nРедактирование невозможно");

    g_signal_connect (tree_view, "row-activated", G_CALLBACK(do_product_embed_edit_row_activated), object);

	priv->model_list = model = do_product_embed_edit_create_model(embed);

    priv->filter = filter = gtk_combo_box_text_new();
    gtk_widget_show(filter);

    g_signal_connect(filter, "changed", G_CALLBACK(do_product_embed_edit_filter_change), object);

    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(filter), "Все");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(filter), "Лекарственные средства (в т.ч. ЖНВЛС)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(filter), "Лекарства и парафармация");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(filter), "Неопределенная");

    do_tree_view_pack_header(view, filter);
    priv->model_filter = (GtkTreeModelFilter*)gtk_tree_model_filter_new(GTK_TREE_MODEL(model), NULL);
    priv->model_sort = (GtkTreeModelSort*)gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(priv->model_filter));

    g_object_set(view, "model", priv->model_sort, NULL);

    gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(priv->model_filter),
            (GtkTreeModelFilterVisibleFunc)do_product_embed_edit_model_filter_visible,
            object, NULL);

    col = priv->columns[COLUMN_CODE]
        = do_tree_view_add_column(view, "Code", "Код", DEFAULT_COL_CODE_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_CODE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_CODE);

    col = priv->columns[COLUMN_NAME]
        = do_tree_view_add_column(view, "Name", "Наименование", DEFAULT_COL_NAME_WIDTH);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_NAME);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_NAME);

    col = priv->columns[COLUMN_NDS]
        = do_tree_view_add_column(view, "NDS", "НДС", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_combo_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_NDS);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_NDS);
    g_object_set(renderer,
                "editable", TRUE,
                "model", priv->stores[STORE_NDS],
                "text-column", 1,
                "has-entry", FALSE,
                NULL);
    g_signal_connect (renderer, "editing-canceled",
                    G_CALLBACK (do_product_embed_edit_cell_editing_canceled), object);
    g_signal_connect (renderer, "edited",
                    G_CALLBACK (do_product_embed_edit_cell_edited), object);
    g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_NDS));

    col = priv->columns[COLUMN_CLASS]
        = do_tree_view_add_column(view, "Class", "Ж/В", DEFAULT_COL_DEFAULT_WIDTH);
    renderer = gtk_cell_renderer_combo_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_CLASS);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_CLASS);
    g_object_set(renderer,
                "editable", TRUE,
                "model", priv->stores[STORE_CLASS],
                "text-column", 1,
                "has-entry", FALSE,
                NULL);
    g_signal_connect (renderer, "editing-canceled",
                    G_CALLBACK (do_product_embed_edit_cell_editing_canceled), object);
    g_signal_connect (renderer, "edited",
                    G_CALLBACK (do_product_embed_edit_cell_edited), object);
    g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_CLASS));

/*    col = priv->columns[COLUMN_LIST]
        = do_tree_view_add_column(view, "List", "Признак", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_combo_new();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_LIST);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_LIST);
    g_object_set(renderer,
                "editable", TRUE,
                "model", priv->stores[STORE_LIST],
                "text-column", 1,
                "has-entry", FALSE,
                NULL);
    g_signal_connect (renderer, "editing-canceled",
                    G_CALLBACK (do_product_embed_edit_cell_editing_canceled), object);
    g_signal_connect (renderer, "edited",
                    G_CALLBACK (do_product_embed_edit_cell_edited), object);
    g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_LIST));
*/
    col = priv->columns[COLUMN_TEMPERATURE]
        = do_tree_view_add_column(view, "Temperature", "Хранение", DEFAULT_COL_DEFAULT_WIDTH);
    renderer = gtk_cell_renderer_combo_new ();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_TEMPERATURE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_TEMPERATURE);
    g_object_set(renderer,
                "editable", TRUE,
                "model", priv->stores[STORE_TEMPERATURE],
                "text-column", 1,
                "has-entry", FALSE,
                NULL);
    g_signal_connect (renderer, "editing-canceled",
                    G_CALLBACK (do_product_embed_edit_cell_editing_canceled), object);
    g_signal_connect (renderer, "edited",
                    G_CALLBACK (do_product_embed_edit_cell_edited), object);
    g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_TEMPERATURE));


    col = priv->columns[COLUMN_IMPERATIVE]
        = do_tree_view_add_column(view, "Imperative", "Необходимый", DEFAULT_COL_DEFAULT_WIDTH);

    renderer = gtk_cell_renderer_combo_new ();
    gtk_tree_view_column_pack_start (col, renderer, TRUE);
    gtk_tree_view_column_set_sort_column_id (col, COLUMN_IMPERATIVE);
    gtk_tree_view_column_add_attribute (col, renderer, "text", COLUMN_IMPERATIVE);
    g_object_set(renderer,
                "editable", TRUE,
                "model", priv->stores[STORE_IMPERATIVE],
                "text-column", 1,
                "has-entry", FALSE,
                NULL);
    g_signal_connect (renderer, "editing-canceled",
                    G_CALLBACK (do_product_embed_edit_cell_editing_canceled), object);
    g_signal_connect (renderer, "edited",
                    G_CALLBACK (do_product_embed_edit_cell_edited), object);
    g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_IMPERATIVE));

    gtk_tree_view_set_search_column(tree_view, COLUMN_NAME);
	gtk_widget_show(GTK_WIDGET(object));

	return object;
}

static void do_product_embed_edit_finalize (GObject *object)
{
    DoProductEmbedEditPrivate *priv;
	priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE (object);
	if (priv->alias_name)
		g_free(priv->alias_name);
	if (priv->main_alias_name)
		g_free(priv->main_alias_name);

//    do_product_model_stop(DO_PRODUCT_MODEL(priv->model));
//    g_object_unref(priv->model);
	G_OBJECT_CLASS (do_product_embed_edit_parent_class)->finalize (object);
}

static void do_product_embed_edit_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoProductEmbedEditPrivate *priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            g_value_set_string (value, priv->alias_name);
			break;
		case PROP_MAIN_ALIAS:
            g_value_set_string (value, priv->main_alias_name);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
	/* no readable properties */
	g_assert_not_reached ();
}

static void do_product_embed_edit_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoProductEmbedEditPrivate *priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string (value);
			break;
		case PROP_MAIN_ALIAS:
            if (!priv->main_alias_name)
                priv->main_alias_name = g_value_dup_string (value);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
}
static void do_product_embed_edit_class_init (DoProductEmbedEditClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_product_embed_edit_constructor;
	o_class->finalize     = do_product_embed_edit_finalize;
	o_class->get_property = do_product_embed_edit_get_property;
	o_class->set_property = do_product_embed_edit_set_property;

	g_type_class_add_private (o_class, sizeof (DoProductEmbedEditPrivate));

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
         PROP_MAIN_ALIAS,
         g_param_spec_string("main-alias-name",
                      "Алиас главной БД",
                      "Алиас главной БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

}
GtkWidget *do_product_embed_edit_new (const gchar *alias_name, const gchar *main_alias_name)
{
	return g_object_new (DO_TYPE_PRODUCT_EMBED_EDIT,
			     "alias-name", alias_name,
			     "main-alias-name", main_alias_name,
			     "object-root-path", OBJECT_ROOT_PATH,
			     (gpointer) NULL);
}
static gboolean do_product_embed_edit_view_do_grab_focus(DoView *view)
{
    return do_tree_view_grab_focus(DO_TREE_VIEW(view));
}
static void	do_product_embed_edit_view_do_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *do_product_embed_edit_view_get_title(DoView *view)
{
    return "Справочник товаров групповое изменение параметров";
}
static GdkPixbuf  *do_product_embed_edit_view_get_icon(DoView *view)
{
    DoProductEmbedEditPrivate *priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE(view);
    return priv->icon;
}
static gboolean	do_product_embed_edit_view_can_do_edit(DoView *view, const gchar *tab)
{
    return TRUE;
    gboolean retval = FALSE;
    DoProductEmbedEditPrivate *priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE(view);
    GtkTreeModel *model = NULL;

    GList *list = gtk_tree_selection_get_selected_rows(priv->sel, &model);
    if (list) {
        retval = (g_list_length(list) == 1);
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
    }
    return retval;
}
static void	do_product_embed_edit_view_do_edit(DoView *view, const gchar *tab)
{
    DoProductEmbedEditPrivate *priv = DO_PRODUCT_EMBED_EDIT_GET_PRIVATE(view);
    GtkTreePath *path;
    GtkTreeModel *model;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(priv->tree_view), &path, NULL);
    if (!path)
        return;
    GtkTreeIter iter;
    model = gtk_tree_view_get_model(priv->tree_view);
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path)) {
        GValue value = { 0, }, value_name = {0, };
        gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, COLUMN_CODE, &value);
        gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, COLUMN_NAME, &value_name);
        DoContext *context;

        context = do_context_new_with_attributes(DO_CONTEXT_KIND_PRODUCT,
                      "product-code", g_value_get_string(&value), NULL);

        GtkWidget *dialog = do_product_edit_new(view, priv->alias_name,
                                                DO_PRODUCT_EDIT_FLAGS_READ_ONLY,
                                                   DO_DIALOG_ACTION_NONE, context, "");


        g_object_set(dialog, "view", view, "tab", tab, NULL);
        GtkNotebook *nb;
        nb = GTK_NOTEBOOK (do_window_get_notebook (
                            DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view)))));
        do_notebook_add_tab(DO_NOTEBOOK(nb), DO_VIEW(dialog), -1, TRUE);
        g_object_unref(context);
    }
    gtk_tree_path_free(path);
}
static void	do_product_embed_edit_view_do_popup(DoView *view, GdkEventButton* event)
{
    GtkWidget *menu = do_view_actions_action_get_widget("/" DO_PRODUCT_EMBED_EDIT_POPUP);
    do_tree_view_popup(DO_TREE_VIEW(view), event, menu);
    printf("sad product embed\n");
}
static void do_product_embed_edit_row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoProductView *view)
{
    if ( do_product_embed_edit_view_can_do_edit(DO_VIEW(view), NULL) )
        do_product_embed_edit_view_do_edit(DO_VIEW(view), NULL);
}

