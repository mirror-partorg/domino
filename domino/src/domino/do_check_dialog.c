

#include <string.h>
#include <gtk/gtk.h>

#include "do_check_dialog.h"
#include "domino.h"
#include <domino.h>
#include "do_entry.h"
#include "do_tree_view.h"
//!!#include "do_product_dialog.h"

enum
{
	PROP_0,
	PROP_FLAGS,
	PROP_CONTEXT,
	PROP_ALIAS,
};

#define DO_CHECK_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_CHECK_DIALOG, DoCheckDialogPrivate))
#define DO_TYPE_CHECK_DIALOG_FLAGS do_check_dialog_flags_get_type()


#define OBJECT_ROOT_PATH "CheckDialog"
#define DEFAULT_WINDOW_POSITION "center-on-parent"
#define DEFAULT_WINDOW_SIZE ""

#define OBJECT_ROOT_PATH_STOCK  "CheckDialogPosition"
#define COLUMN_DEFAULT_SIZE 60

GType do_check_dialog_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_check_dialog_flags_values[] = {
         { DO_CHECK_DIALOG_FLAGS_FROM_CURRENT, "DO_CHECK_DIALOG_FLAGS_FROM_CURRENT", "from-current" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoCheckDialogFlags", _do_check_dialog_flags_values);
    }
    return type;
}

enum
{
	SHIFT_ENTRY,
	NUMBER_ENTRY,
	DATE_ENTRY,
	CASHIER_ENTRY,
	N_ENTRIES
};

struct _DoCheckDialogPrivate
{
	DoCheckDialogFlags flags;

	GtkWidget *widget[N_ENTRIES];
	GtkWidget *entry[N_ENTRIES];

	GtkWidget   *view;
	GtkTreeStore *store;
	GtkWidget    *title;
	GtkWidget    *totally;

	DoContext  *context;

	gchar      *alias_name;

};


G_DEFINE_TYPE (DoCheckDialog, do_check_dialog, GTK_TYPE_DIALOG)

static GtkWidget *add_field(DoCheckDialog *dialog, gint id, GtkWidget *widget);

static gboolean fill_entry(DoCheckDialog *dialog);

static void do_check_dialog_create_store(DoCheckDialog *dialog);
static gboolean do_check_dialog_get_position(DoCheckDialog *dialog);
static const gchar *oper_name[4] = {"Продажа наличные", "Возврат наличные", "Продажа б/н", "Возврат б/н"};

static void do_check_dialog_init(DoCheckDialog *dialog)
{

}
static void do_check_dialog_activate(GtkTreeView       *tree_view,
                                                        GtkTreePath       *path,
                                                        GtkTreeViewColumn *column,
                                                        DoCheckDialog *dialog)
{
    DoCheckDialogPrivate *priv = DO_CHECK_DIALOG_GET_PRIVATE(dialog);
    DoContext *context;
    GValue code = {0,};
    GtkTreeIter iter;
   // GtkWidget *product_dialog;

    if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->store), &iter, path))
        return;

    gtk_tree_model_get_value(GTK_TREE_MODEL(priv->store), &iter, 5, &code);

    context = do_context_new_with_attributes(DO_CONTEXT_KIND_PRODUCT,
               "product-code", g_value_get_string(&code),
               NULL);
//    product_dialog = do_product_dialog_new(GTK_WIDGET(dialog), priv->alias_name, 0, DO_DIALOG_ACTION_EDIT, context);
    //do_product_dialog_run(DO_PRODUCT_DIALOG(product_dialog));
    g_object_unref(context);
    //gtk_widget_destroy(product_dialog);

}


static GObject *do_check_dialog_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	GtkWindow *window;
	GtkDialog *dialog;
	DoCheckDialog *check_dialog;
	DoCheckDialogPrivate *priv;
	GtkWidget *vbox;
    GtkWidget *l;
    GtkWidget *w;
    GtkWidget *tree_view;
    GtkWidget *t;
    GtkAccelGroup *accel_group;

	int row = 0;
	object = G_OBJECT_CLASS (do_check_dialog_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_CHECK_DIALOG_GET_PRIVATE(object);
	window = GTK_WINDOW (object);
	dialog = GTK_DIALOG (object);
	vbox   = gtk_dialog_get_content_area(dialog);
	check_dialog = DO_CHECK_DIALOG(object);


    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);


	gtk_box_set_spacing (GTK_BOX (vbox), 2); /* Message has 24, we want 12 = 2 + 2 * 5 */

	gtk_window_set_icon_name (window, DO_STOCK_DOMINO_APPS);
	gtk_window_set_title (window, "Просмотр чека");

    g_signal_connect(window, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), OBJECT_ROOT_PATH);

    //gtk_dialog_set_has_separator(dialog, FALSE);

    t = hig_workarea_create();
    //gtk_table_set_homogeneous(GTK_TABLE(t), TRUE);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, TRUE, 0);

    priv->title = hig_workarea_add_section_title(t, &row, "Данные чека ");


	l = add_field(check_dialog, SHIFT_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Смена:", l, NULL);
    //gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);

	l = add_field(check_dialog, NUMBER_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Номер:", l,NULL);
    //gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);

	l = add_field(check_dialog, DATE_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Время:", l,NULL);
    //gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);

	l = add_field(check_dialog, CASHIER_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Кассир:", l,NULL);
    //gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);

    w = priv->view = do_tree_view_new(OBJECT_ROOT_PATH_STOCK);
    g_object_get(w, "tree-view", &tree_view, NULL);
    g_signal_connect(tree_view, "row-activated",
                        G_CALLBACK (do_check_dialog_activate), object);


    hig_workarea_finish( t, &row );

	gtk_box_pack_start (GTK_BOX (vbox), w, TRUE, TRUE, 0);

	gtk_dialog_add_button (dialog, "Отмена", GTK_RESPONSE_CANCEL);
	//gtk_dialog_add_button (dialog, GTK_STOCK_PRINT, GTK_RESPONSE_YES);
	gtk_dialog_add_button (dialog, "Ok", GTK_RESPONSE_ACCEPT);
	//gtk_dialog_set_default_response (dialog, GTK_RESPONSE_ACCEPT);

    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(window), OBJECT_ROOT_PATH,
              "window-position", DEFAULT_WINDOW_POSITION,
              "window-size", DEFAULT_WINDOW_SIZE,
               NULL);
    priv->totally = gtk_label_new(NULL);
    do_tree_view_pack_footer(DO_TREE_VIEW(priv->view), priv->totally);

    do_check_dialog_create_store(DO_CHECK_DIALOG (object));
    fill_entry(DO_CHECK_DIALOG (object));

	gtk_widget_show_all (vbox);

    do_tree_view_grab_focus(DO_TREE_VIEW(priv->view));

	return object;
}

static void do_check_dialog_finalize (GObject *object)
{
	DoCheckDialogPrivate *priv = DO_CHECK_DIALOG_GET_PRIVATE (object);
	if (priv->context)
        g_object_unref(priv->context);
	if (priv->alias_name)
		g_free(priv->alias_name);
	G_OBJECT_CLASS (do_check_dialog_parent_class)->finalize (object);
}
static void do_check_dialog_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoCheckDialogPrivate *priv = DO_CHECK_DIALOG_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_FLAGS:
			priv->flags = g_value_get_flags (value);
			break;
        case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string(value);
            break;
        case PROP_CONTEXT:
            if (!priv->context)
                priv->context = do_context_copy(g_value_get_object(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_check_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoCheckDialogPrivate *priv = DO_CHECK_DIALOG_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_ALIAS:
            g_value_set_string(value, priv->alias_name);
        case PROP_CONTEXT:
            g_value_set_object (value, do_context_copy(priv->context));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;

    }
}
static void do_check_dialog_class_init (DoCheckDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_check_dialog_constructor;
	object_class->finalize = do_check_dialog_finalize;
	object_class->get_property = do_check_dialog_get_property;
	object_class->set_property = do_check_dialog_set_property;

	g_type_class_add_private (object_class, sizeof (DoCheckDialogPrivate));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_CHECK_DIALOG_FLAGS,
				     DO_CHECK_DIALOG_FLAGS_DEFAULT,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property
        (object_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (object_class,
				   PROP_CONTEXT,
				   g_param_spec_object ("context",
 						      "Контекст документа",
 						      "Контекст документа описания клавиши",
 						      DO_TYPE_CONTEXT,
 						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

GtkWidget *do_check_dialog_new (GtkWidget *parent, const gchar *alias_name, DoCheckDialogFlags flags, DoContext *context)
{
	return g_object_new (DO_TYPE_CHECK_DIALOG,
			     "flags", flags,
			     "context", context,
			     "alias-name", alias_name,
			     (gpointer) NULL);
}
static GtkWidget *add_field(DoCheckDialog *dialog, gint id, GtkWidget *widget)
{
	DoCheckDialogPrivate *priv = DO_CHECK_DIALOG_GET_PRIVATE (dialog);
	g_assert (id < N_ENTRIES);
	priv->widget[id] = widget;
	priv->entry[id] = widget;
    g_object_set(priv->entry[id], "editable", FALSE, NULL);

    return widget;
}

static gboolean do_check_dialog_get_crnt_position(DoCheckDialog *dialog)
{
	DoCheckDialogPrivate *priv = DO_CHECK_DIALOG_GET_PRIVATE (dialog);
	int status;
	document_order_key0_t document_key0, document_key;
	document_order_rec_t  document_order;
	product_key4_t product_key4;
	product_rec_t product;
	do_alias_t *alias;
	gint cash;
	gint shift;
	gint check;
	gchar *buf;
	gboolean init = FALSE;
	gint total_position = 0;
	gdouble total_sum = 0;

    alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_PRODUCT, DO_DB_PARTNER, DO_DB_END)) {
        do_alias_free(alias);
        return FALSE;
    }

    g_object_get(priv->context, "cash", &cash, "shift", &shift, "check", &check, NULL);
    g_return_val_if_fail ( cash && shift && check, FALSE );

    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_CRNTSALE));
    do_text_set(alias, document_key0.sklad, domino_unit());
    buf = g_strdup_printf("%2.2d%4.4d", cash, shift);
    do_text_set(alias, document_key0.document, buf);
    g_free(buf);
    document_key0.line = check*1000;
    do_cpy(document_key, document_key0);
    document_key.line += 1000;
    buf = g_strdup_printf("%d/%d", cash, shift);
    g_object_set(priv->entry[SHIFT_ENTRY], "text", buf, NULL);
    g_free(buf);
    buf = g_strdup_printf("%d", check);
    g_object_set(priv->entry[NUMBER_ENTRY], "text", buf, NULL);
    g_free(buf);
	status = do_document_order_get0(alias, &document_order, &document_key0, DO_GET_GE);

    while (status == DO_OK) {
	    gchar *barcode;
	    gint   operation;
	    gchar *cashier;
	    gchar *code;
	    gdouble coef;
	    gchar *name;
	    gdouble price;
	    gdouble quant;
	    struct tm tm;
	    GtkTreeIter iter;

	    if (memcmp(&document_key0, &document_key, sizeof(document_key) - sizeof(document_key.line)) ||
	        document_key0.line >= document_key.line
	        ) break;

        if (!init) {
            operation = do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_OPERATION));
            cashier = do_get_partner_name(alias, do_param(DO_PARAM_PARTNER_REGION_CASHIER),
                do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_CASHIER)));
            g_object_set(priv->entry[CASHIER_ENTRY], "text", cashier, NULL);
            do_date(document_order.data.date, &tm);
            buf = do_date_time_format(do_time(document_order.data.time, &tm));
            g_object_set(priv->entry[DATE_ENTRY], "text", buf, NULL);
            g_free(buf);
            buf = g_strdup_printf("<b>Данные чека (%s)</b>", oper_name[operation - 1]);
            g_object_set(priv->title, "label", buf, NULL);
            g_free(buf);
        }
        quant = do_document_order_quant(alias, &document_order, DO_CONST_QUANT_CRNTSALE);
        price = do_document_order_price(alias, &document_order, DO_CONST_PRICE_FACT);
        barcode = do_document_order_param(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_BARCODE));
        do_cpy(product_key4.code, document_order.data.code);
        document_order.data.code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)] = '\0';
        code = do_text(alias, document_order.data.code);
        status = do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL);
        total_position++;
        total_sum += price*quant;
        if (status == DO_ERROR)
            break;
        if (status != DO_OK) {
            coef = 1;
            buf = do_text(alias, product_key4.code);
            name = g_strdup_printf("Товар %s", buf);
            g_free(buf);
        }
        else {
            coef = do_product_coef(alias, &product);
            name = do_text(alias, product.data.name);
        }
        gtk_tree_store_append (priv->store, &iter, NULL);
        gtk_tree_store_set (priv->store, &iter,
                            0, barcode,
                            1, name,
                            2, quant/coef,
                            3, price*coef,
                            4, price*quant,
                            5, code,
                            -1);
        g_free(barcode);
        g_free(name);

        status = do_document_order_get0(alias, &document_order, &document_key0, DO_GET_NEXT);
	}
	buf = g_strdup_printf("Позиций : <b>%d</b> Сумма : <b>%s</b>",
	            total_position, do_money_format(total_sum, TRUE));
    gtk_label_set_markup(GTK_LABEL(priv->totally), buf);
    g_object_set(priv->totally, "xalign", 0.01, NULL);
    g_free(buf);
    do_alias_free(alias);
	return status != DO_ERROR;
}
static gboolean fill_entry(DoCheckDialog *dialog)
{
	DoCheckDialogPrivate *priv = DO_CHECK_DIALOG_GET_PRIVATE (dialog);
    if (do_context_get_kind(priv->context) != DO_CONTEXT_KIND_CHECK)
        return FALSE;

    if ((priv->flags & DO_CHECK_DIALOG_FLAGS_FROM_CURRENT) == DO_CHECK_DIALOG_FLAGS_FROM_CURRENT)
        return do_check_dialog_get_crnt_position(dialog);
    else
        return do_check_dialog_get_position(dialog);
}

gint do_check_dialog_run(DoCheckDialog *dialog)
{
    gint response = 0;
	//DoCheckDialogPrivate *priv = DO_CHECK_DIALOG_GET_PRIVATE (dialog);
    while (!response) {
        response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_ACCEPT) {
        }
    }
    return response;
}
static void do_check_dialog_create_store(DoCheckDialog *dialog)
{
	DoCheckDialogPrivate *priv = DO_CHECK_DIALOG_GET_PRIVATE (dialog);
    GtkCellRenderer   *r;
    GtkTreeViewColumn *col;

    priv->store = gtk_tree_store_new(6,
                                                    G_TYPE_STRING,//barcode
                                                    G_TYPE_STRING,//name
                                                    G_TYPE_DOUBLE,//quantity
                                                    G_TYPE_DOUBLE,//price
                                                    G_TYPE_DOUBLE, // sum
                                                    G_TYPE_STRING // code
                                                    );

    g_object_set(priv->view, "model", priv->store, NULL);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->view), "Barcode", "Прод.код", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);
    gtk_tree_view_column_set_sort_column_id (col, 0);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->view), "Name", "Наименование", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    gtk_tree_view_column_set_sort_column_id (col, 1);
    gtk_tree_view_column_set_cell_data_func(col ,r, do_tree_cell_data_func_product_name, GINT_TO_POINTER(1), NULL);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->view), "Quantity", "Количество", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 2);
    gtk_tree_view_column_set_sort_column_id (col, 2);
    gtk_tree_view_column_set_cell_data_func(col ,r, do_tree_cell_data_func_rest, GINT_TO_POINTER(2), NULL);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->view), "Price", "Цена", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 3);
    gtk_tree_view_column_set_sort_column_id (col, 3);
    gtk_tree_view_column_set_cell_data_func(col ,r, do_tree_cell_data_func_money, GINT_TO_POINTER(3), NULL);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(DO_TREE_VIEW(priv->view), "Sum", "Сумма", COLUMN_DEFAULT_SIZE);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 4);
    gtk_tree_view_column_set_sort_column_id (col, 4);
    gtk_tree_view_column_set_cell_data_func(col ,r, do_tree_cell_data_func_money, GINT_TO_POINTER(4), NULL);


}
static gboolean do_check_dialog_get_position(DoCheckDialog *dialog)
{
	DoCheckDialogPrivate *priv = DO_CHECK_DIALOG_GET_PRIVATE (dialog);
	int status;
	check_key0_t check_key0, check_key;
	check_rec_t check;
	product_key4_t product_key4;
	product_rec_t product;
	do_alias_t *alias;
	gint cash;
	gint shift;
	gint check_;
	gchar *buf;
	gboolean init = FALSE;
	gint total_position = 0;
	gdouble total_sum = 0;

    alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, 0, DO_DB_CHECK, DO_DB_PRODUCT, DO_DB_PARTNER, DO_DB_END)) {
        do_alias_free(alias);
        return FALSE;
    }

    g_object_get(priv->context, "cash", &cash, "shift", &shift, "check", &check_, NULL);
    g_return_val_if_fail ( cash && shift && check_, FALSE );

    do_set_empty(check_key0);
    check_key0.cash = cash;
    check_key0.shift = shift;
    check_key0.check =check_;
    do_cpy(check_key, check_key0);

    check_key0.check = check_;
    check_key0.shift = shift;
    check_key0.cash = cash;
    do_cpy(check_key, check_key0);

    buf = g_strdup_printf("%d/%d", cash, shift);
    g_object_set(priv->entry[SHIFT_ENTRY], "text", buf, NULL);
    g_free(buf);
    buf = g_strdup_printf("%d", check_);
    g_object_set(priv->entry[NUMBER_ENTRY], "text", buf, NULL);
    g_free(buf);

	status = do_check_get0(alias, &check, &check_key0, DO_GET_GE);

    while (status == DO_OK) {
	    gchar *barcode;
	    gint   operation;
	    gchar *cashier;
	    gchar *code;
	    gdouble coef;
	    gchar *name;
	    gdouble price;
	    gdouble quant;
	    struct tm tm;
	    GtkTreeIter iter;

	    if (check_key0.cash != check_key.cash ||
            check_key0.shift != check_key.shift ||
            check_key0.check != check_key.check
	        ) break;

        if (!init) {
            operation = atoi(check.data.operation);
            cashier =
                do_get_partner_name(alias, do_param(DO_PARAM_PARTNER_REGION_CASHIER), check.data.cashier);
            g_object_set(priv->entry[CASHIER_ENTRY], "text", cashier, NULL);
            do_sqltodate(check.data.date, &tm);
            buf = do_date_time_format(do_sqltotime(check.data.time, &tm));
            g_object_set(priv->entry[DATE_ENTRY], "text", buf, NULL);
            g_free(buf);
            buf = g_strdup_printf("<b>Данные чека (%s)</b>", oper_name[operation - 1]);
            g_object_set(priv->title, "label", buf, NULL);
            g_free(buf);
        }
        quant = check.data.quantity;
        price = check.data.sale_price;
        barcode = do_text(alias, check.data.bar_code);
        do_cpy(product_key4.code, check.data.product_code);
        check.data.product_code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)] = '\0';
        code = do_text(alias, check.data.product_code);


        status = do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL);
        total_position++;
        total_sum += price*quant;
        if (status == DO_ERROR)
            break;
        if (status != DO_OK) {
            coef = 1;
            buf = do_text(alias, product_key4.code);
            name = g_strdup_printf("Товар %s", buf);
            g_free(buf);
        }
        else {
            coef = do_product_coef(alias, &product);
            name = do_text(alias, product.data.name);
        }
        gtk_tree_store_append (priv->store, &iter, NULL);
        gtk_tree_store_set (priv->store, &iter,
                            0, barcode,
                            1, name,
                            2, quant/coef,
                            3, price*coef,
                            4, price*quant,
                            5, code,
                            -1);
        g_free(barcode);
        g_free(name);

        status = do_check_get0(alias, &check, &check_key0, DO_GET_NEXT);
	}
	buf = g_strdup_printf("Позиций : <b>%d</b> Сумма : <b>%s</b>",
	            total_position, do_money_format(total_sum, TRUE));
    gtk_label_set_markup(GTK_LABEL(priv->totally), buf);
    g_object_set(priv->totally, "xalign", 0.01, NULL);
    g_free(buf);
    do_alias_free(alias);
	return status != DO_ERROR;
}
