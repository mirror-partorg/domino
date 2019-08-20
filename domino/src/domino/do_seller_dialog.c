

#include <string.h>
#include <gtk/gtk.h>

#include "do_seller_dialog.h"
#include "do_utilx.h"
#include "do_enum_box.h"
#include "do_enum_box.h"
#include "do_icons.h"
#include "do_entry.h"
#include "do_filter.h"
#include "do_rz_view.h"
#include "do_validate.h"
#include "do_entry_date_box.h"
#include "do_enum_box.h"
#include "do_unit_box.h"
#include "do_enum_value.h"
#include "do_entry_partner_box.h"
#include "do_view.h"
#include "do_report_view.h"
#include "do_report_unit_model.h"
#include "do_check_dialog.h"
#include "do_rz_edit_view.h"
#include "domino.h"
#include "hig.h"
#include <dolib.h>
#include <domino.h>


#define DO_SELLER_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_SELLER_DIALOG, DoSellerDialogPrivate))

enum
{
	SKLADS_ENTRY,
	DATE_B_ENTRY,
	DATE_E_ENTRY,
	KIND_ENTRY,
	KIND_SUM_ENTRY,
	KIND_SHOW_UNIT,
	KIND_PRICE_ENTRY,
	SUPPLIER_ENTRY,
	SELLER_ENTRY,
	N_ENTRIES
};
enum
{
	PROP_0,
	PROP_REPORT,
};
static const gchar *report_title[]=
{
    "Приход-расход по представителю",
    "Детальный приход  по представителю",
};

#define MAX_SIZE_RADIO 10
struct _DoSellerDialogPrivate
{
	GtkWidget  *widget[N_ENTRIES];
	GtkWidget  *entry[N_ENTRIES];
    GdkPixbuf  *icon;

	GtkListStore *pstore;
	GtkWidget    *pfooter;
	do_alias_t   *alias;
	partner_key0_t crnt_partner_key0;
	GtkWidget    *product_view;

	gint          report;


	DoView     *view;

	GtkTreeView  *mfr_view;
	GtkListStore *mfr_store;
	GtkTreeViewColumn *mfr_col;
	gboolean      mfr_report;

};
#define OBJECT_ROOT_PATH "SellerDialog"
#define DEFAULT_PANED_POSITION "600"

static GtkWidget *add_field(DoSellerDialog *dialog, gint id, GtkWidget *widget);

static void entry_activate(GtkWidget *entry, DoSellerDialog *dialog);
static gboolean fill_entry(DoSellerDialog *dialog);

static gboolean do_seller_dialog_pview_fill(DoSellerDialog *dialog);
static void     do_seller_dialog_pview_focus_change(GtkWidget *widget, GdkEventFocus *event, gpointer user_data);
static void     do_seller_dialog_pview_selection_changed(GtkTreeSelection *selection, gpointer user_data);

static gboolean    do_seller_dialog_view_do_grab_focus(DoView *view);
static void	       do_seller_dialog_view_do_close(DoView *view);
static const char *do_seller_dialog_view_get_title(DoView *view);
static GdkPixbuf  *do_seller_dialog_view_get_icon(DoView *view);
static void	       do_seller_dialog_view_do_insert(DoView *view);
static gboolean	   do_seller_dialog_view_can_do_insert(DoView *view);
static void	       do_seller_dialog_view_do_delete(DoView *view);
static gboolean	   do_seller_dialog_view_can_do_delete(DoView *view);
static gboolean    do_seller_dialog_view_set_marked(DoView *view, DoContext *context, DoMarked marked);
static gboolean    do_seller_dialog_view_sender_do_close(DoView *view);


static void do_seller_dialog_send_received_info(DoSellerDialog *view);
static void do_seller_dialog_report1(DoSellerDialog *dialog);
static void do_seller_dialog_report2(DoSellerDialog *dialog);
static void do_seller_dialog_read_last_value(DoSellerDialog *dialog);

static void do_seller_dialog_view_init(DoViewIface *iface);
static gboolean paned_sync_position(GtkWidget *paned, GParamSpec *pspec, DoSellerDialog *view);

G_DEFINE_TYPE_WITH_CODE (DoSellerDialog, do_seller_dialog, DO_TYPE_TREE_VIEW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_seller_dialog_view_init)
                                    );

static void do_seller_dialog_view_init(DoViewIface *iface)
{
    iface->do_grab_focus = do_seller_dialog_view_do_grab_focus;
    iface->do_close      = do_seller_dialog_view_do_close;
    iface->get_title     = do_seller_dialog_view_get_title;
    iface->get_icon      = do_seller_dialog_view_get_icon;
    iface->do_insert     = do_seller_dialog_view_do_insert;
    iface->can_do_insert = do_seller_dialog_view_can_do_insert;
    iface->do_delete     = do_seller_dialog_view_do_delete;
    iface->can_do_delete = do_seller_dialog_view_can_do_delete;
    iface->set_marked    = do_seller_dialog_view_set_marked;
    iface->sender_do_close  = do_seller_dialog_view_sender_do_close;
}
static void do_seller_dialog_init(DoSellerDialog *dialog)
{
#if GTK_MAJOR_VERSION == 2
	DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (dialog);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_FILE, DO_VIEW_ICON_SIZE, 0, NULL);
#endif
}
static void entry_activate(GtkWidget *entry, DoSellerDialog *dialog)
{
	DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (dialog);
    gint i, indx;
    //gboolean valid = TRUE;
    indx = -1;
    for (i = 0; i < N_ENTRIES; i++) {
        if (DO_IS_VALIDATE(priv->entry[i]) &&
            !do_validate_valid(DO_VALIDATE(priv->entry[i])) &&
            gtk_widget_get_sensitive(priv->widget[i])) {
            //valid = FALSE;
            gtk_widget_grab_focus(priv->entry[i]);
            break;
        }
        if ( entry == priv->entry[i] ) {
            indx = i;
            break;
        }
    }
    if ( indx != -1 ) {
        for ( ; indx < N_ENTRIES - 1 ; indx++ )
            if (  priv->entry[indx+1] ) {
                gtk_widget_grab_focus(priv->entry[indx+1]);
                return;
            }
    }
    if ( indx == SELLER_ENTRY )
        do_tree_view_grab_focus(DO_TREE_VIEW(dialog));
}
typedef struct {
    gchar *code;
    gchar *name;
    gchar *bar;
    double coef;
}parcel_t;

static void do_seller_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_REPORT:
			g_value_set_int (value, priv->report);
			break;
	}

	g_assert_not_reached ();
}

static void do_seller_dialog_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_REPORT:
			priv->report = g_value_get_int (value);
			break;
	}
}
static GtkWidget *add_field(DoSellerDialog *dialog, gint id, GtkWidget *widget)
{
	DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (dialog);
	g_assert (id < N_ENTRIES);
	priv->widget[id] = widget;

	if (GTK_IS_ENTRY(widget)) {
        g_signal_connect(widget, "activate", G_CALLBACK(entry_activate), dialog);
        priv->entry[id] = widget;
	}
    else
	if (DO_IS_ENTRY_DATE(widget)) {
        priv->entry[id] = widget;
//	    priv->entry[id] = do_entry_date_entry(DO_ENTRY_DATE(widget));
        g_signal_connect(priv->entry[id], "activate", G_CALLBACK(entry_activate), dialog);
	}
    else
	if (DO_IS_ENTRY_BOX(widget)) {
        priv->entry[id] = do_entry_box_entry(DO_ENTRY_BOX(widget));
//	    priv->entry[id] = do_entry_date_entry(DO_ENTRY_DATE(widget));
        g_signal_connect(priv->entry[id], "activate", G_CALLBACK(entry_activate), dialog);
	}
    else
	if (DO_IS_ENTRY_PARTNER_BOX(widget)) {
        priv->entry[id] = do_entry_partner_box_entry(DO_ENTRY_PARTNER_BOX(widget));
//	    priv->entry[id] = do_entry_date_entry(DO_ENTRY_DATE(widget));
        g_signal_connect(priv->entry[id], "activate", G_CALLBACK(entry_activate), dialog);
	}
    else
    if (GTK_IS_BUTTON(widget))
        g_signal_connect(widget, "activate", G_CALLBACK(entry_activate), dialog);
    else
	if (GTK_IS_COMBO_BOX(widget)) {
        priv->entry[id] = gtk_bin_get_child (GTK_BIN (widget));
        if (priv->entry[id])
            g_signal_connect(priv->entry[id], "activate", G_CALLBACK(entry_activate), dialog);
	}

    return widget;
}
static gboolean do_seller_dialog_partner_change(GtkWidget *paned, GParamSpec *pspec, DoSellerDialog *dialog)
{
	do_seller_dialog_pview_fill(dialog);
	return FALSE;
}
static void do_seller_dialog_clicked_close(GtkButton *widget, DoSellerDialog *edit)
{
    do_seller_dialog_view_do_close(DO_VIEW(edit));
}
static gboolean count_func (GtkTreeModel *model,
                                                         GtkTreePath *path,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    gint *count = data;
    (*count)++;
    return FALSE;
}
static void do_seller_dialog_clicked_manufactor(GtkButton *widget, DoSellerDialog *edit);
static void do_seller_dialog_clicked_print(GtkButton *widget, DoSellerDialog *edit)
{
	DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (edit);
    gint i;//, indx;
   // gboolean valid = TRUE;
    //indx = -1;
    for (i = 0; i < N_ENTRIES; i++) {
        if (DO_IS_VALIDATE(priv->entry[i]) &&
            !do_validate_valid(DO_VALIDATE(priv->entry[i])) &&
            gtk_widget_get_sensitive(priv->widget[i])) {
            //valid = FALSE;
            gtk_widget_grab_focus(priv->entry[i]);
            return;
        }
    }
    GtkTreeView *tree_view;
    gint count = 0;
    g_object_get(edit, "tree-view", &tree_view, NULL);
    gtk_tree_model_foreach(gtk_tree_view_get_model(tree_view), count_func, &count);
    if ( !count ) {
        do_tree_view_grab_focus(DO_TREE_VIEW(edit));
        return;
    }
    switch ( priv->report ) {
        case 1:
            do_seller_dialog_report1(edit);
            break;
        case 2:
            do_seller_dialog_report2(edit);
            break;
    }
}
static void  kind_changed(GtkComboBox *widget,  DoSellerDialog *dialog)
{
	DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (dialog);
	DoEnumValue *value;
	gint kind;
    g_object_get(priv->widget[KIND_ENTRY], "value", &value, NULL);
    kind = do_enum_value_get_int(value);
    g_object_unref(value);
    gtk_widget_set_sensitive(priv->widget[DATE_B_ENTRY], kind != 4);
    gtk_widget_set_sensitive(priv->widget[DATE_E_ENTRY], kind != 4);
    gtk_widget_set_sensitive(priv->widget[SUPPLIER_ENTRY], kind == 2);
    //if ( )


}
static void cell_received1(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, 4, &value);
    if ( !g_value_get_boolean(&value) )
#if GTK_MAJOR_VERSION > 2
        g_object_set(cell, "icon-name", "object-select-symbolic", NULL);
#else
        g_object_set(cell, "icon-name", GTK_STOCK_APPLY, NULL);
#endif
    else
        g_object_set(cell, "icon-name", NULL, NULL);

}
static gpointer product_thread_func(gpointer data)
{
    GObject *object = data;

    //g_object_ref(object);
    GtkWidget    *view;
    view = do_product_view_new(NULL, domino_unit(),
	                                         DO_PRODUCT_VIEW_FLAGS_ROW_ACTIVATED_TO_SEND |
	                                         DO_PRODUCT_VIEW_FLAGS_SHOW_RECEIVED
	                                         );
    if ( GTK_IS_WIDGET(object) ) {
        DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(object);
        priv->product_view = view;
        g_object_set(view, "receiver-object", object, NULL);
        do_seller_dialog_send_received_info(DO_SELLER_DIALOG(object));
    }

    return NULL;
}

static GObject *do_seller_dialog_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	DoSellerDialog *seller_dialog;
	DoSellerDialogPrivate *priv;
	GtkWidget *vbox;
    GtkWidget *l;
    GtkWidget *t;
    GtkWidget *tb;
    GtkWidget *hb;
    GtkTreeView *tree_view;
    GtkTreeViewColumn *col;
    GtkCellRenderer * r;
    DoTreeView *do_view;

	int row = 0;
	object = G_OBJECT_CLASS (do_seller_dialog_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_SELLER_DIALOG_GET_PRIVATE(object);
	vbox = GTK_WIDGET(object);
	seller_dialog = DO_SELLER_DIALOG(object);

	gtk_box_set_spacing (GTK_BOX (vbox), 2); /* Message has 24, we want 12 = 2 + 2 * 5 */

#if GTK_MAJOR_VERSION == 2
    hb = gtk_hpaned_new();
    tb=gtk_hbox_new(FALSE, 0);
#else
    hb = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    tb=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#endif
	g_signal_connect(hb, "notify::position",
			  G_CALLBACK (paned_sync_position), object);
    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(hb), OBJECT_ROOT_PATH,
                               "paned-position", DEFAULT_PANED_POSITION, NULL);

    t = hig_workarea_create();
#if GTK_MAJOR_VERSION == 2
    gtk_table_set_homogeneous(GTK_TABLE(t), FALSE);
#endif
    gtk_paned_add1(GTK_PANED (hb), t);
    gtk_paned_add2(GTK_PANED (hb), tb);


    hig_workarea_add_section_title(t, &row, report_title[priv->report-1]);


	l = add_field(seller_dialog, SKLADS_ENTRY,
               do_unit_box_new());
    hig_workarea_add_row(t, &row, "Аптеки:", l, NULL);

	l = add_field(seller_dialog, DATE_B_ENTRY,
               do_entry_date_box_new(0));
    hig_workarea_add_row(t, &row, "Начало:", l, NULL);
	l = add_field(seller_dialog, DATE_E_ENTRY,
               do_entry_date_box_new(0));
    hig_workarea_add_row(t, &row, "Конец:", l, NULL);

    if ( priv->report == 1 ) {
        hig_workarea_add_section_title(t, &row, "Вид отчета ");
        l = add_field(seller_dialog, KIND_ENTRY,
                   do_enum_box_new("SellerKind"));
        g_signal_connect(l, "changed", G_CALLBACK(kind_changed), object);

        hig_workarea_add_row(t, &row, "Движение:", l, NULL);

        l = add_field(seller_dialog, KIND_SUM_ENTRY,
                   do_enum_box_new("SellerKindSum"));
        hig_workarea_add_row(t, &row, "Показатели:", l, NULL);

        l = add_field(seller_dialog, KIND_SHOW_UNIT,
                    gtk_check_button_new());

        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l), TRUE);

        hig_workarea_add_row(t, &row, "По аптекам:", l, NULL);

        l = add_field(seller_dialog, KIND_PRICE_ENTRY,
                   do_enum_box_new("SellerKindPrice"));
        hig_workarea_add_row(t, &row, "Цены:", l, NULL);

        l = add_field(seller_dialog, SUPPLIER_ENTRY,
                      do_entry_partner_box_new(NULL, DO_ENTRY_PARTNER_FLAGS_SEARCH | DO_ENTRY_PARTNER_FLAGS_EMPTY));

        gtk_widget_set_sensitive(do_entry_partner_box_button(DO_ENTRY_PARTNER_BOX(l)), FALSE);

        g_object_set(l, "region-code", "03", NULL);
        hig_workarea_add_row(t, &row, "Поставщик:", l, NULL);
    }
    else {
        hig_workarea_add_section_title(t, &row, "Вид отчета ");

        l = add_field(seller_dialog, KIND_SHOW_UNIT,
                    gtk_check_button_new());
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l), TRUE);

        hig_workarea_add_row(t, &row, "По аптекам:", l, NULL);
   }
    hig_workarea_add_section_title(t, &row, "Представитель/товары ");
	l = add_field(seller_dialog, SELLER_ENTRY,
               do_entry_partner_box_new(NULL, DO_ENTRY_PARTNER_FLAGS_SEARCH | DO_ENTRY_PARTNER_FLAGS_EMPTY));
    g_object_set(l, "region-code", "SLR", NULL);
	g_signal_connect(do_entry_partner_box_entry(DO_ENTRY_PARTNER_BOX(l)), "notify::context",
			  G_CALLBACK (do_seller_dialog_partner_change), object);

    hig_workarea_add_row(t, &row, "Представитель:", l, NULL);


    hig_workarea_finish( t, &row );
    priv->alias = domino_alias_new(NULL);
    if ( priv->alias )
        do_alias_open(priv->alias, TRUE);

    priv->pstore = gtk_list_store_new(5, G_TYPE_STRING,
                                         G_TYPE_STRING,
                                         G_TYPE_DOUBLE,
                                         G_TYPE_DOUBLE,
                                         G_TYPE_BOOLEAN);

    tb = GTK_WIDGET(object);
    do_tree_view_pack_header(DO_TREE_VIEW(tb), hb);

    do_view = (DoTreeView*)tb;
    g_object_get(tb, "tree-view", &tree_view, NULL);
    g_object_set(tb, "model", priv->pstore, NULL);
    g_signal_connect(tree_view, "focus-in-event", G_CALLBACK(do_seller_dialog_pview_focus_change), object);
    g_signal_connect(tree_view, "focus-out-event", G_CALLBACK(do_seller_dialog_pview_focus_change), object);
    GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    g_signal_connect(sel, "changed", G_CALLBACK(do_seller_dialog_pview_selection_changed), object);
    gtk_tree_selection_set_mode(sel, GTK_SELECTION_MULTIPLE);

    priv->pfooter = gtk_label_new(NULL);
    gtk_widget_show(priv->pfooter);
    g_object_set(priv->pfooter, "xalign", 0.01, NULL);
	//!!gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (tb), TRUE, TRUE, 0);


    priv->mfr_col = col = do_tree_view_add_column(do_view, "R", "", -1);
    r = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, r, cell_received1, object, NULL);
    g_object_set (r,
                    "xalign", (gfloat)0.5,
                    "yalign", (gfloat)0.5,
                    "mode", GTK_CELL_RENDERER_MODE_INERT,
                    NULL);
    gtk_tree_view_column_set_visible(col, FALSE);
    gtk_tree_view_column_set_sort_column_id(col, 5);


    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(do_view, "Code", "Код", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 0);
    gtk_tree_view_column_set_sort_column_id (col, 0);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(do_view, "Name", "Наименование", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    gtk_tree_view_column_set_cell_data_func(col, r, do_tree_cell_data_func_product_name, GINT_TO_POINTER(1), NULL);
    gtk_tree_view_column_set_sort_column_id (col, 1);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(do_view, "Price", "Цена", -1);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 2);
    gtk_tree_view_column_set_cell_data_func(col, r, do_tree_cell_data_func_money, GINT_TO_POINTER(2), NULL);
    gtk_tree_view_column_set_sort_column_id (col, 2);

#if GTK_MAJOR_VERSION == 2
    t=gtk_hbox_new(FALSE, 2);
#else
    t=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
#endif

    gtk_container_set_border_width( GTK_CONTAINER( t ), GUI_PAD_BIG );
	do_tree_view_pack_footer(DO_TREE_VIEW(tb), GTK_WIDGET(t));
    do_tree_view_pack_footer(DO_TREE_VIEW(tb), priv->pfooter);
	//gtk_box_pack_end (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, TRUE, 0);

	l = gtk_button_new_with_label("Закрыть");
	g_signal_connect(l, "clicked", G_CALLBACK(do_seller_dialog_clicked_close), seller_dialog);
	gtk_box_pack_end (GTK_BOX (t), GTK_WIDGET (l), FALSE, TRUE, 0);

	l = gtk_button_new_with_label("Печать");
	g_signal_connect(l, "clicked", G_CALLBACK(do_seller_dialog_clicked_print), seller_dialog);
	gtk_box_pack_end (GTK_BOX (t), GTK_WIDGET (l), FALSE, TRUE, 0);

	l = gtk_button_new_with_label("Отобрать по производителям");
	g_signal_connect(l, "clicked", G_CALLBACK(do_seller_dialog_clicked_manufactor), seller_dialog);
	gtk_box_pack_end (GTK_BOX (t), GTK_WIDGET (l), FALSE, TRUE, 0);

    fill_entry(seller_dialog);

	gtk_widget_show_all (vbox);
#if GLIB_MINOR_VERSION < 32
	product_thread_func(object);
#else
	g_thread_unref (g_thread_new ("product-thread", product_thread_func, object));
#endif
    do_seller_dialog_read_last_value(DO_SELLER_DIALOG(object));

	return object;
}
static gboolean fill_entry(DoSellerDialog *dialog)
{
    return TRUE;

}

static void do_seller_dialog_finalize (GObject *object)
{
	DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (object);
	if ( priv->alias )
        do_alias_free(priv->alias);
	G_OBJECT_CLASS (do_seller_dialog_parent_class)->finalize (object);
}
static void do_seller_dialog_class_init (DoSellerDialogClass *klass)
{
    GObjectClass *o_class = G_OBJECT_CLASS (klass);

	o_class->constructor  = do_seller_dialog_constructor;
	o_class->finalize     = do_seller_dialog_finalize;
	o_class->get_property = do_seller_dialog_get_property;
	o_class->set_property = do_seller_dialog_set_property;

	g_type_class_add_private (o_class, sizeof (DoSellerDialogPrivate));

    g_object_class_install_property (o_class,
				   PROP_REPORT,
				   g_param_spec_int("report",
							"тип отчета",
							"Номер отчета",
							1,100,1,
							G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

}
GtkWidget *do_seller_dialog_new (gint report)
{
	return g_object_new (DO_TYPE_SELLER_DIALOG,
                      "object-root-path", "SellerTreeView",
                      "report", report,
			     (gpointer) NULL);
}
static const char *do_seller_dialog_view_get_title(DoView *view)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(view);

    return report_title[priv->report-1];
}
static gboolean do_seller_dialog_view_do_grab_focus(DoView *view)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(view);

    //do_tree_view_grab_focus(DO_TREE_VIEW(view))
    gtk_widget_grab_focus(priv->entry[0]);
    return TRUE;
}
static void	do_seller_dialog_view_do_close(DoView *view)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(view);
    GtkNotebook *nb;
    nb = GTK_NOTEBOOK (do_window_get_notebook (
                    DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view)))));
    if ( gtk_notebook_page_num(nb, GTK_WIDGET(priv->product_view)) == -1 )
        gtk_widget_destroy(GTK_WIDGET(priv->product_view));
    else
        gtk_notebook_remove_page(nb, gtk_notebook_page_num(nb, GTK_WIDGET(priv->product_view)));
    gtk_widget_destroy(GTK_WIDGET(view));
}
static GdkPixbuf  *do_seller_dialog_view_get_icon(DoView *view)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(view);
    return priv->icon;
}
static void do_seller_dialog_pfooter_update(DoSellerDialog *dialog)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(dialog);
    GtkTreeView *tree_view;
    gint count = 0;
    g_object_get(dialog, "tree-view", &tree_view, NULL);
    gtk_tree_model_foreach(gtk_tree_view_get_model(tree_view), count_func, &count);
    gchar *name = g_strdup_printf("Позиций: <b>%d</b>", count);
    gtk_label_set_markup(GTK_LABEL(priv->pfooter), name);
    g_free(name);

}
static do_string_list_t *product_list_get(do_alias_t *alias, partner_key0_t *partner_key0)
{

    document_order_key0_t document_order_key0, key;
    document_order_rec_t document_order;
    char buf[50];
    int value;
    int status;

    do_string_list_t *list;
    list=do_string_list_new();

    do_text_set(alias, document_order_key0.dtype, "AG");
    do_text_set(alias, document_order_key0.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    value = partner_key0->code;

    sprintf(buf, "%d", value);
    do_text_set(alias, document_order_key0.document, buf);
    do_cpy(key,document_order_key0);
    document_order_key0.line = 0;

    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( memcmp(&key, &document_order_key0, sizeof(document_order_key0.dtype)+
                                               sizeof(document_order_key0.sklad)+
                                               sizeof(document_order_key0.document)) ) break;
        do_string_list_add(list, do_text(alias, document_order.data.code));

        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
    }
    return list;
}
static gboolean do_seller_dialog_pview_fill(DoSellerDialog *dialog)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(dialog);
    DoContext *context;
    gchar *region;
    gint   code;
    do_string_list_t *list;
    partner_key0_t partner_key0;
    int i;
    do_alias_t *alias = priv->alias;
    if ( !alias )
        return FALSE;

    g_object_get(priv->widget[SELLER_ENTRY], "context", &context, NULL);
    if ( do_context_get_kind(context) == DO_CONTEXT_KIND_PARTNER ) {
        g_object_get(context, "region-code", &region, "partner-code", &code, NULL);

        do_text_set(alias, partner_key0.g_code, region);
        partner_key0.code = code;
        if ( !do_cmp(partner_key0, priv->crnt_partner_key0) )
            return TRUE;

        do_cpy(priv->crnt_partner_key0, partner_key0);

        list = product_list_get(priv->alias, &partner_key0);
        if ( !list )
            return FALSE;

        gtk_list_store_clear(priv->pstore);
        for ( i = 0; i < list->count; i++ ) {
            product_key4_t product_key4;
            product_rec_t product;
            gchar *code;
            gchar *name;
            GtkTreeIter iter;
            do_text_set(alias,product_key4.code,list->list[i]);
            code = list->list[i];
            if ( do_util_product_get4(alias, &product, &product_key4) )
                name = do_text(alias, product.data.name);
            else
                name = strdup(code);
            gtk_list_store_append(priv->pstore, &iter);
            gtk_list_store_set(priv->pstore, &iter,
                               0, code,
                               1, name,
                               2, do_product_coef(alias, &product)*
                                  do_product_price(alias, &product, DO_CONST_PRICE_RETAIL),
                               3, do_product_coef(alias, &product),
                               -1
                               );
            g_free(name);

        }
        do_string_list_free(list);
        do_seller_dialog_pfooter_update(dialog);
        do_seller_dialog_send_received_info(DO_SELLER_DIALOG(dialog));
    }
    return TRUE;
}

/*
static gboolean do_seller_dialog_pview_fill(DoSellerDialog *dialog)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(dialog);
    DoContext *context;
    gchar *region;
    gint   code;
    do_ret_list_t *list;
    partner_key0_t partner_key0;
    int i;
    do_alias_t *alias = priv->alias;
    if ( !alias )
        return FALSE;

    g_object_get(priv->widget[SELLER_ENTRY], "context", &context, NULL);
    if ( do_context_get_kind(context) == DO_CONTEXT_KIND_PARTNER ) {
        g_object_get(context, "region-code", &region, "partner-code", &code, NULL);

        do_text_set(alias, partner_key0.g_code, region);
        partner_key0.code = code;
        if ( !do_cmp(partner_key0, priv->crnt_partner_key0) )
            return TRUE;

        do_cpy(priv->crnt_partner_key0, partner_key0);

        do_log(LOG_INFO,"get list");
        list = do_util_product_list_get(priv->alias, &partner_key0);
        if ( !list )
            return FALSE;

        do_log(LOG_INFO,"get product name");
        gtk_list_store_clear(priv->pstore);
        for ( i = 0; i < do_ret_list_count(list); i++ ) {
            product_key4_t *product_key4;
            product_rec_t product;
            gchar *code;
            gchar *name;
            GtkTreeIter iter;
            product_key4 = do_ret_list_item_data(list, i);

            code = do_text(alias, product_key4->code);
            if ( do_util_product_get4(alias, &product, product_key4) )
                name = do_text(alias, product.data.name);
            else
                name = do_text(alias, product_key4->code);
            gtk_list_store_append(priv->pstore, &iter);
            gtk_list_store_set(priv->pstore, &iter,
                               0, code,
                               1, name,
                               2, do_product_coef(alias, &product)*
                                  do_product_price(alias, &product, DO_CONST_PRICE_RETAIL),
                               3, do_product_coef(alias, &product),
                               -1
                               );
            g_free(code);
            g_free(name);

        }
        do_log(LOG_INFO,"end get product name");
        do_seller_dialog_pfooter_update(dialog);
        do_seller_dialog_send_received_info(DO_SELLER_DIALOG(dialog));
    }
    return TRUE;
}*/
static void do_seller_dialog_pview_focus_change(GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
    do_view_actions_set_view(user_data);
}
static void do_seller_dialog_pview_selection_changed(GtkTreeSelection *selection, gpointer user_data)
{
    do_view_actions_refresh();
}

static void do_seller_dialog_view_do_insert(DoView *view)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(view);
    GtkNotebook *nb;
    nb = GTK_NOTEBOOK (do_window_get_notebook (
                    DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view)))));
    while ( !priv->product_view )
        DO_SEC_SLEEP(1);


    if ( gtk_notebook_page_num(nb, GTK_WIDGET(priv->product_view)) == -1 )
        do_notebook_add_tab(DO_NOTEBOOK(nb), DO_VIEW(priv->product_view), -1, TRUE);
    else
        gtk_notebook_set_current_page(nb, gtk_notebook_page_num(nb, GTK_WIDGET(priv->product_view)));
}
static gboolean	do_seller_dialog_view_can_do_insert(DoView *view)
{
    return TRUE;
    //DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(view);
    //return do_tree_view_is_focus(DO_TREE_VIEW(priv->pview));
}
static gboolean do_seller_dialog_premove(GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(user_data);
    DoContext *context;
    GValue code = {0,};
    partner_key0_t partner_key0;
    product_key4_t product_key4;
    gchar *region;
    gint  pcode;
    do_alias_t *alias = priv->alias;
    gboolean retval;

    g_object_get(priv->widget[SELLER_ENTRY], "context", &context, NULL);
    if ( do_context_get_kind(context) != DO_CONTEXT_KIND_PARTNER )
        return TRUE;

    gtk_tree_model_get_value(model, iter, 0, &code);
    g_object_get(context, "region-code", &region, "partner-code", &pcode, NULL);
    do_text_set(alias, partner_key0.g_code, region);
    partner_key0.code = pcode;
    do_text_set(alias, product_key4.code, g_value_get_string(&code));
    retval = do_util_product_list_delete_product(alias, &partner_key0, &product_key4);

    return retval;
}
static gboolean do_seller_dialog_pinsert(DoSellerDialog *dialog, const gchar *product_code)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(dialog);
    DoContext *context;
    partner_key0_t partner_key0;
    product_key4_t product_key4;
    gchar *region;
    gint  pcode;
    do_alias_t *alias = priv->alias;

    do_text_set(alias, product_key4.code, product_code);
    g_object_get(priv->widget[SELLER_ENTRY], "context", &context, NULL);
    if ( do_context_get_kind(context) == DO_CONTEXT_KIND_PARTNER ) {

        g_object_get(context, "region-code", &region, "partner-code", &pcode, NULL);
        do_text_set(alias, partner_key0.g_code, region);
        partner_key0.code = pcode;
        if ( !do_util_product_list_insert_product(alias, &partner_key0, &product_key4) )
            return FALSE;
    }
    product_rec_t product;
    gchar *code;
    gchar *name;
    GtkTreeIter iter;

    code = do_text(alias, product_key4.code);
    if ( do_util_product_get4(alias, &product, &product_key4) )
        name = do_text(alias, product.data.name);
    else
        name = do_text(alias, product_key4.code);
    gtk_list_store_append(priv->pstore, &iter);
    gtk_list_store_set(priv->pstore, &iter,
                       0, code,
                       1, name,
                       2, do_product_coef(alias, &product)*
                          do_product_price(alias, &product, DO_CONST_PRICE_RETAIL),
                       3, do_product_coef(alias, &product),
                       -1
                       );
    g_free(code);
    g_free(name);

    return TRUE;
}
static gboolean remove_list (GtkTreeModel *model,
                                                         GtkTreePath *path,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    gchar *code = data;
    GValue value = {0,};
    gtk_tree_model_get_value(model, iter, 0, &value);
    if ( !strcmp(code, g_value_get_string(&value)) ) {
        gtk_list_store_remove(GTK_LIST_STORE(model), iter);
        return TRUE;
    }
    return FALSE;
}

static gboolean do_seller_dialog_pdelete(DoSellerDialog *dialog, const gchar *product_code)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(dialog);
    DoContext *context;
    partner_key0_t partner_key0;
    product_key4_t product_key4;
    gchar *region;
    gint  pcode;
    do_alias_t *alias = priv->alias;

    do_text_set(alias, product_key4.code, product_code);
    g_object_get(priv->widget[SELLER_ENTRY], "context", &context, NULL);
    if ( do_context_get_kind(context) == DO_CONTEXT_KIND_PARTNER ) {

        g_object_get(context, "region-code", &region, "partner-code", &pcode, NULL);
        do_text_set(alias, partner_key0.g_code, region);
        partner_key0.code = pcode;
        if ( !do_util_product_list_delete_product(alias, &partner_key0, &product_key4) )
            return FALSE;
    }
    gtk_tree_model_foreach(GTK_TREE_MODEL(priv->pstore), (GtkTreeModelForeachFunc)remove_list, (gpointer)product_code);
    return TRUE;
}
static gboolean do_seller_dialog_get_pcode(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
    GList **list = data;
    GValue code = {0,};
    gtk_tree_model_get_value(model, iter, 0, &code);
    *list =
    g_list_append(*list,
                    g_value_dup_string(&code));
    return FALSE;
}

static void do_seller_dialog_send_received_info(DoSellerDialog *view)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (view);
    if ( !priv || !priv->product_view )
        return;
    GList *list = NULL, *l;
    gtk_tree_model_foreach(GTK_TREE_MODEL(priv->pstore), do_seller_dialog_get_pcode, &list);
    DoContext *context;

    context = do_context_new();
    do_context_set_kind(context, DO_CONTEXT_KIND_PRODUCT);

    for ( l = list; l; l=l->next ) {
        g_object_set(context, "product-code", l->data, NULL);
        do_view_set_marked(DO_VIEW(priv->product_view), context, DOMINO_MARKED);
    }
    g_object_unref(context);

    if (list) {
        g_list_foreach(list, (GFunc)g_free, NULL);
        g_list_free(list);
    }
}
static void do_seller_dialog_view_do_delete(DoView *view)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(view);
    if ( !do_tree_view_is_focus(DO_TREE_VIEW(view)) )
        return;



    GtkTreeSelection *selection;
    GtkTreeView *tree_view;
    GtkTreeModel *model;
    GList *list, *l;
    g_object_get(view, "tree-view", &tree_view, NULL);
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    list = gtk_tree_selection_get_selected_rows (selection, &model);
    DoContext *context = do_context_new();

    do_context_set_kind(context, DO_CONTEXT_KIND_PRODUCT);

    for ( l = list; l; l=l->next ) {
        GtkTreeIter iter ={0,};
        GValue value = {0,};
        gtk_tree_model_get_iter(model, &iter, l->data);
        gtk_tree_model_get_value(model, &iter, 0,  &value);
        g_object_set(context, "product-code", g_value_get_string(&value), NULL);
        if ( priv->product_view )
            do_view_set_marked(DO_VIEW(priv->product_view), context, DOMINO_MARKED_UNMARK);
    }
    do_tree_view_delete_selection_from_list_store(DO_TREE_VIEW(view),
                                                  do_seller_dialog_premove, view) ;

    do_seller_dialog_pfooter_update(DO_SELLER_DIALOG(view));
    do_seller_dialog_send_received_info(DO_SELLER_DIALOG(view));
}
static gboolean	do_seller_dialog_view_can_do_delete(DoView *view)
{
    //DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(view);
    if ( !do_tree_view_is_focus(DO_TREE_VIEW(view)) )
        return FALSE;
    GtkTreeSelection *sel;
    GtkTreeModel *model;
    GtkTreeView *tree_view;
    g_object_get(view, "tree-view", &tree_view, NULL);
    sel = gtk_tree_view_get_selection(tree_view);
    GList *list = gtk_tree_selection_get_selected_rows(sel, &model);
    if (list) {
        g_list_foreach(list, (GFunc)gtk_tree_path_free, view);
        g_list_free(list);
        return TRUE;
    }
    return FALSE;
}
static gboolean	do_seller_dialog_view_receive(DoView *view, DoContext *context)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (view);
    gboolean retval = FALSE;
    if (do_context_get_kind(context) == DO_CONTEXT_KIND_PRODUCT) {
        gchar *product_code;
        g_object_get(context, "product-code", &product_code, NULL);
        retval = do_seller_dialog_pinsert(DO_SELLER_DIALOG(view), product_code);
        if ( retval && priv->product_view )
            do_view_set_marked(DO_VIEW(priv->product_view), context, DOMINO_MARKED);
    }
    if ( retval ) {
        do_seller_dialog_pfooter_update(DO_SELLER_DIALOG(view));
        do_seller_dialog_send_received_info(DO_SELLER_DIALOG(view));
    }
    return retval;
}
static gboolean	do_seller_dialog_view_undo_receive(DoView *view, DoContext *context)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (view);
    gboolean retval = FALSE;
    if (do_context_get_kind(context) == DO_CONTEXT_KIND_PRODUCT) {
        gchar *product_code;
        g_object_get(context, "product-code", &product_code, NULL);
        retval = do_seller_dialog_pdelete(DO_SELLER_DIALOG(view), product_code);
        if ( retval && priv->product_view )
            do_view_set_marked(DO_VIEW(priv->product_view), context, DOMINO_MARKED_UNMARK);
    }
    if ( retval ) {
        do_seller_dialog_pfooter_update(DO_SELLER_DIALOG(view));
        do_seller_dialog_send_received_info(DO_SELLER_DIALOG(view));
    }
    return retval;
}
static gboolean	do_seller_dialog_view_set_marked(DoView *view, DoContext *context, DoMarked marked)
{
    if ( marked == DO_MARKED_UNMARK )
        return do_seller_dialog_view_undo_receive(view, context);
    else
        return do_seller_dialog_view_receive(view, context);
}

static gboolean do_seller_dialog_view_sender_do_close(DoView *view)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (view);
    GtkNotebook *nb;
    gint tab;
    nb = GTK_NOTEBOOK (do_window_get_notebook (
                    DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(view)))));
    tab = gtk_notebook_page_num(nb, GTK_WIDGET(priv->product_view));
    if ( tab != -1 ) {
         g_object_ref(priv->product_view);
         gtk_container_remove (GTK_CONTAINER(nb), priv->product_view);
    }
    return FALSE;
}

static void do_seller_dialog_save_value(DoSellerDialog *dialog)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(dialog);
    gint value_date_start_year;
    gint value_date_start_month;
    gint value_date_start_day;
    gint value_date_end_year;
    gint value_date_end_month;
    gint value_date_end_day;
    gchar *v_d_s_year;
    gchar *v_d_s_month;
    gchar *v_d_s_day;
    gchar *v_d_e_year;
    gchar *v_d_e_month;
    gchar *v_d_e_day;
    gchar *units;
    DoEnumValue *kind = NULL;
    DoEnumValue *kind_sum = NULL;
    DoEnumValue *kind_price = NULL;

    g_object_get(priv->widget[SKLADS_ENTRY], "units", &units, NULL);
    if ( priv->widget[KIND_ENTRY] )
        g_object_get(priv->widget[KIND_ENTRY], "value", &kind, NULL);
    if ( priv->widget[KIND_SUM_ENTRY] )
        g_object_get(priv->widget[KIND_SUM_ENTRY], "value", &kind_sum, NULL);
    if ( priv->widget[KIND_PRICE_ENTRY] )
        g_object_get(priv->widget[KIND_PRICE_ENTRY], "value", &kind_price, NULL);

    g_object_get(priv->entry[DATE_B_ENTRY], "year", &value_date_start_year,
                                            "month", &value_date_start_month,
                                            "day", &value_date_start_day,
                                            NULL);
    g_object_get(priv->entry[DATE_E_ENTRY], "year", &value_date_end_year,
                                            "month", &value_date_end_month,
                                            "day", &value_date_end_day,
                                            NULL);
    v_d_s_year = g_strdup_printf("%d", value_date_start_year);
    v_d_s_month = g_strdup_printf("%d", value_date_start_month);
    v_d_s_day = g_strdup_printf("%d", value_date_start_day);
    v_d_e_year = g_strdup_printf("%d", value_date_end_year);
    v_d_e_month = g_strdup_printf("%d", value_date_end_month);
    v_d_e_day = g_strdup_printf("%d", value_date_end_day);
    DOMINO_PROFILE_SET(OBJECT_ROOT_PATH, "LastValue",
                                        "DateStartYear", v_d_s_year,
                                        "DateStartMonth", v_d_s_month,
                                        "DateStartDay", v_d_s_day,
                                        "DateEndYear", v_d_e_year,
                                        "DateEndMonth", v_d_e_month,
                                        "DateEndDay", v_d_e_day,
                                        "Units", units,
                                        NULL);
    if ( kind )
        DOMINO_PROFILE_SET(OBJECT_ROOT_PATH, "LastValue",
                                        "Kind", do_enum_value_get_string(kind),
                                        NULL);
    if ( kind_sum )
        DOMINO_PROFILE_SET(OBJECT_ROOT_PATH, "LastValue",
                                        "KindSum", do_enum_value_get_string(kind_sum),
                                        NULL);

    if ( kind_price )
        DOMINO_PROFILE_SET(OBJECT_ROOT_PATH, "LastValue",
                                        "KindPrice", do_enum_value_get_string(kind_price),
                                        NULL);


    g_object_unref(kind);
    g_object_unref(kind_sum);
    g_object_unref(kind_price);
    g_free(v_d_s_year);

    g_free(v_d_s_month);
    g_free(v_d_s_day);
    g_free(v_d_e_year);
    g_free(v_d_e_month);
    g_free(v_d_e_day);
}
static void do_seller_dialog_read_last_value(DoSellerDialog *dialog)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(dialog);
    gchar *v_d_s_year;
    gchar *v_d_s_month;
    gchar *v_d_s_day;
    gchar *v_d_e_year;
    gchar *v_d_e_month;
    gchar *v_d_e_day;
    gchar *v_units;
    gchar *v_kind;
    gchar *v_sum;
    gchar *v_price;

    DOMINO_PROFILE_GET(OBJECT_ROOT_PATH,
                                        "DateStartYear", &v_d_s_year,
                                        "DateStartMonth", &v_d_s_month,
                                        "DateStartDay", &v_d_s_day,
                                        "DateEndYear", &v_d_e_year,
                                        "DateEndMonth", &v_d_e_month,
                                        "DateEndDay", &v_d_e_day,
                                        "Units", &v_units,
                                        "Kind", &v_kind,
                                        "KindSum", &v_sum,
                                        "KindPrice", &v_price,
                                        NULL);
    if ( v_d_s_year && atoi(v_d_s_year) > 0 )
        g_object_set(priv->entry[DATE_B_ENTRY], "year", atoi(v_d_s_year),
                                            "month", atoi(v_d_s_month),
                                            "day", atoi(v_d_s_day),
                                            NULL);
    if ( v_d_s_year && atoi(v_d_s_year) > 0 )
        g_object_set(priv->entry[DATE_E_ENTRY], "year", atoi(v_d_e_year),
                                            "month", atoi(v_d_e_month),
                                            "day", atoi(v_d_e_day),
                                            NULL);
    if ( v_units ) {
        g_object_set(priv->widget[SKLADS_ENTRY], "units", v_units, NULL);
    }
    if ( v_kind && priv->widget[KIND_ENTRY] ) {
        DoEnumValue *value;
        value = do_enum_value_new();
        do_enum_value_set_string(value, v_kind);
        g_object_set(priv->widget[KIND_ENTRY], "value", value, NULL);
        g_object_unref(value);
    }
    if ( v_sum && priv->widget[KIND_SUM_ENTRY] ) {
        DoEnumValue *value;
        value = do_enum_value_new();
        do_enum_value_set_string(value, v_sum);
        g_object_set(priv->widget[KIND_SUM_ENTRY], "value", value, NULL);
        g_object_unref(value);
    }
    if ( v_price && priv->widget[KIND_PRICE_ENTRY] ) {
        DoEnumValue *value;
        value = do_enum_value_new();
        do_enum_value_set_string(value, v_sum);
        g_object_set(priv->widget[KIND_PRICE_ENTRY], "value", value, NULL);
        g_object_unref(value);
    }

}
static void do_seller_dialog_value_edited(GtkCellRendererText *renderer,
                                            gchar  *path_string,
                                            gchar  *new_text,
                                            DoTreeView *view)
{
    gint column;
    GtkTreeModel *model;
    gchar *text;
    gchar *tail, *tail_ansi;
    gdouble value;
    GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
    GtkTreeIter iter;

    g_object_get(view, "model", &model, NULL);
    if ( !DO_IS_REPORT_UNIT_MODEL(model) )
        return;

    column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(renderer), "report-column"));
    text = g_strdup(new_text);
    g_strtod(text, &tail);
    value = g_ascii_strtod(text, &tail_ansi);
    if ( tail > tail_ansi )
        value = g_strtod(text, &tail);
    g_free(text);
    if ( gtk_tree_model_get_iter(model, &iter, path) ) {
        do_report_unit_model_set_value(DO_REPORT_UNIT_MODEL(model), &iter,
                                       column, value);

    }

}
static GtkTreeViewColumn *do_seller_dialog_add_col_tail(DoSellerDialog *dialog, DoTreeView *report, gint *i, gint type_, gchar *title, gboolean show)
{
    //DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(dialog);
    GtkCellRenderer *r;
    GtkTreeViewColumn *col;
    gchar *col_name;

    col_name = g_strdup_printf("Col%d", *i);
    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(report, col_name, title, 100);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", *i);
    if ( type_ ) {
        gtk_tree_view_column_set_cell_data_func(col, r, do_report_unit_model_cell_data_func_rest, GINT_TO_POINTER(*i), NULL);
        g_object_set (r, "xalign", (gfloat)0.5,
                    "editable", TRUE,
                    "ellipsize", PANGO_ELLIPSIZE_MIDDLE,
                    NULL);
    }
    else {
        gtk_tree_view_column_set_cell_data_func(col, r, do_report_unit_model_cell_data_func_money, GINT_TO_POINTER(*i), NULL);\
        g_object_set (r, "xalign", (gfloat)1.,
                    "editable", TRUE,
                    "ellipsize", PANGO_ELLIPSIZE_START,
                    NULL);
    }
    g_signal_connect (r, "edited",
                    G_CALLBACK (do_seller_dialog_value_edited), report);
    g_object_set_data(G_OBJECT(r), "report-column", GINT_TO_POINTER(*i));
    gtk_tree_view_column_set_sort_column_id (col, *i);

    g_free(col_name);
    g_free(title);
    (*i)++;
    if ( !show )
        gtk_tree_view_column_set_visible(col, FALSE);
    return col;
}
static gboolean get_stock(do_meta_alias_t *meta, const gchar *code, const gchar *sklad, struct tm *date, double *quant, double *sum);

static gboolean do_seller_dialog_report1_code(const gchar *code,
                                                int all_parcel,
                                              double coef,
                                              do_string_list_t *units,
                                              DoSellerDialogPrivate *priv,
                                              int kind,
                                              int kind_sum,
                                              do_meta_alias_t *meta,
                                              struct tm date_b,
                                              struct tm date_e,
                                              int price_purchase,
                                              int price_fact,
                                              DoReportUnitModel *model,
                                              GtkTreeIter *iter_new)
{

    int indx;
    const char *dtype_in = do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN);
    const char *dtype_out = do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT);
    do_string_list_t *dtype1_in = do_string_list_new_from_csv("01");
    do_string_list_t *dtype1_out = do_string_list_new_from_csv("01,02,03,04");

    for ( indx = 0; indx < units->count; indx++ ) {
        double quant_in, sum_in;
        double quant_out, sum_out;
        if ( !all_parcel ) {
            char *sk;
            sk = g_strdup_printf("%2.2d", atoi(units->list[indx]));
            if ( strncmp(code + do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)+1, sk ,2) ) {
                g_free(sk);
                continue;
            }
            g_free(sk);
        }
        if ( kind == 1 || kind == 2 ) {
            int fill = FALSE;
            if ( kind == 2 ) {
                DoContext *context;
                gchar *region;
                gint   partner_code;
                partner_key0_t partner_key0;
                do_alias_t *alias = priv->alias;
                if ( !alias )
                    return FALSE;
                g_object_get(priv->widget[SUPPLIER_ENTRY], "context", &context, NULL);
                if ( do_context_get_kind(context) == DO_CONTEXT_KIND_PARTNER ) {
                    g_object_get(context, "region-code", &region, "partner-code", &partner_code, NULL);
                    do_text_set(alias, partner_key0.g_code, region);
                    partner_key0.code = partner_code;
                    fill = TRUE;

                    do_report_product_meta_traffic_partner(meta, code,all_parcel,
                                                   dtype_in, dtype1_in,
                                                   (char*)units->list[indx],
                                                    &date_b, &date_e,
                                                    DO_CONST_QUANT_REST,
                                                    price_purchase,
                                                    &quant_in, &sum_in,
                                                    &partner_key0);
                }
            }
            if ( !fill )
                do_report_product_meta_traffic(meta, code,all_parcel,
                                           dtype_in, dtype1_in,
                                           (char*)units->list[indx],
                                            &date_b, &date_e,
                                            DO_CONST_QUANT_REST,
                                            price_purchase,
                                            &quant_in, &sum_in);
        }
        if ( kind == 1 || kind == 3 ) {
            do_report_product_meta_traffic(meta, code,all_parcel,
                                           dtype_out, dtype1_out,
                                           (char*)units->list[indx],
                                            &date_b, &date_e,
                                            DO_CONST_QUANT_REST,
                                            price_fact,
                                            &quant_out, &sum_out);
        }
        if ( kind == 4 ) {
            if ( !get_stock(meta, code, (char*)units->list[indx], &date_b, &quant_in, &sum_in) )
                break;
        }
        quant_in /= coef;
        quant_out /= coef;

        if ( all_parcel ) {
            switch ( kind ) {
                case 1:
                    switch ( kind_sum ) {
                        case 1:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 1, sum_in);
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 2, quant_out);
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 3, sum_out);
                            break;
                        case 2:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            do_report_unit_model_set_unit_value(model, iter_new,indx, 1, quant_out);
                            break;
                        case 3:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, sum_in);
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 1, sum_out);
                            break;
                    };
                    break;
                case 2:
                    switch ( kind_sum ) {
                        case 1:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 1, sum_in);
                            break;
                        case 2:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            break;
                        case 3:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, sum_in);
                            break;
                    };
                    break;
                case 3:
                    switch ( kind_sum ) {
                        case 1:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, quant_out);
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 1, sum_out);
                            break;
                        case 2:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, quant_out);
                            break;
                        case 3:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, sum_out);
                            break;
                    };
                    break;
                case 4:
                    switch ( kind_sum ) {
                        case 1:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 1, sum_in);
                            break;
                        case 2:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            break;
                        case 3:
                            do_report_unit_model_set_unit_value(model, iter_new,
                                                indx, 0, sum_in);
                            break;
                    };
                    break;
            }
        }
        else {
            switch ( kind ) {
                case 1:
                    switch ( kind_sum ) {
                        case 1:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 1, sum_in);
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 2, quant_out);
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 3, sum_out);
                            break;
                        case 2:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            do_report_unit_model_add_unit_value(model, iter_new,indx, 1, quant_out);
                            break;
                        case 3:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, sum_in);
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 1, sum_out);
                            break;
                    };
                    break;
                case 2:
                    switch ( kind_sum ) {
                        case 1:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 1, sum_in);
                            break;
                        case 2:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            break;
                        case 3:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, sum_in);
                            break;
                    };
                    break;
                case 3:
                    switch ( kind_sum ) {
                        case 1:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, quant_out);
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 1, sum_out);
                            break;
                        case 2:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, quant_out);
                            break;
                        case 3:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, sum_out);
                            break;
                    };
                    break;
                case 4:
                    switch ( kind_sum ) {
                        case 1:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 1, sum_in);
                            break;
                        case 2:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, quant_in);
                            break;
                        case 3:
                            do_report_unit_model_add_unit_value(model, iter_new,
                                                indx, 0, sum_in);
                            break;
                    };
                    break;
            }
        }
    }
    return TRUE;
}
static void do_seller_dialog_report1(DoSellerDialog *dialog)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(dialog);
    DoTreeView *report;
    GtkTreeViewColumn *col;
    GtkCellRenderer *r;
    DoReportUnitModel *model;
    GtkNotebook  *nb;
    GtkTreeIter   iter;
    GtkTreePath  *path;
    do_string_list_t *units;
    gboolean show;
    DoEnumValue *value;
    gchar *un;
    gint        kind;
    gint        kind_sum;
    gint        kind_price;
    gint        n_values;
    gint        i, indx;
    gint date_start_year;
    gint date_start_month;
    gint date_start_day;
    gint date_end_year;
    gint date_end_month;
    gint date_end_day;
    GList *list;
    gchar *title;

    do_seller_dialog_save_value(dialog);

    do_start_long_operation(GTK_WIDGET(dialog));
    nb = GTK_NOTEBOOK (do_window_get_notebook(
                    DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(dialog)))));
    report = DO_TREE_VIEW(do_report_view_new());
    g_object_set(report, "title", "Приход-расход по представителю ",NULL);
    do_notebook_add_tab(DO_NOTEBOOK(nb), DO_VIEW(report), -1, TRUE);

    g_object_get(priv->widget[SKLADS_ENTRY], "units", &un, NULL);
    units = do_string_list_new_from_csv(un);
    g_object_get(priv->widget[KIND_ENTRY], "value", &value, NULL);
    kind = do_enum_value_get_int(value);
    g_object_unref(value);
    g_object_get(priv->widget[KIND_SUM_ENTRY], "value", &value, NULL);
    kind_sum = do_enum_value_get_int(value);
    g_object_unref(value);
    g_object_get(priv->widget[KIND_PRICE_ENTRY], "value", &value, NULL);
    kind_price = do_enum_value_get_int(value);
    g_object_unref(value);
    g_object_get(priv->entry[DATE_B_ENTRY], "year", &date_start_year,
                                            "month", &date_start_month,
                                            "day", &date_start_day,
                                            NULL);
    g_object_get(priv->entry[DATE_E_ENTRY], "year", &date_end_year,
                                            "month", &date_end_month,
                                            "day", &date_end_day,
                                            NULL);
    show = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->widget[KIND_SHOW_UNIT]));

    n_values = ( (kind == 1) ? 2 : 1 ) * ( (kind_sum == 1) ? 2 : 1 );

    model = (DoReportUnitModel*)do_report_unit_model_new(units->count, n_values,
                                     ((units->count > 1) ? DO_REPORT_UNIT_MODEL_FLAG_TOTAL_COLUMN : 0) |
                                     DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW
                                     );
    do_report_unit_model_set_print_units(model, show);
    //g_object_set(report, "model", model, NULL);
    list = NULL;
    for ( indx = 0; indx < units->count; indx++ )
        list = g_list_append(list, units->list[indx]);
    do_report_unit_model_set_unit_title(model, list);
    g_list_free(list);

    gchar *supplier_name = NULL;
    partner_key0_t supplier_key0;


    if ( kind == 2 ) {
        DoContext *context;
        gchar *region;
        gint   partner_code;
        do_alias_t *alias = priv->alias;
        if ( !alias )
            return;
        g_object_get(priv->widget[SUPPLIER_ENTRY], "context", &context, NULL);
        if ( do_context_get_kind(context) == DO_CONTEXT_KIND_PARTNER ) {
            g_object_get(context, "region-code", &region, "partner-code", &partner_code, NULL);
            do_text_set(alias, supplier_key0.g_code, region);
            supplier_key0.code = partner_code;
            partner_rec_t partner;
            if ( do_partner_get0(alias, &partner, &supplier_key0, DO_GET_EQUAL) != DO_OK )
                return;
            supplier_name = do_text(alias, partner.data.name);
        }
    }


    switch ( kind ) {
        case 1:
            title = g_strdup_printf("Приход/Расход за период с %2.2d/%2.2d/%4.4d по %2.2d/%2.2d/%4.4d",
                                    date_start_day, date_start_month, date_start_year,
                                    date_end_day, date_end_month, date_end_year
                                    );
            switch ( kind_sum ) {
                case 1:
                    do_report_unit_model_set_header_title (model, 3,
                                                           1, "Аптека",
                                                           2, "Приход", "Расход",
                                                           4, "Кол-во", "Закуп", "Кол-во", "Розница"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_REST,
                                                         DO_REPORT_VALUE_TYPE_MONEY,
                                                         DO_REPORT_VALUE_TYPE_REST,
                                                         DO_REPORT_VALUE_TYPE_MONEY
                                                         );
                    break;
                case 2:
                    do_report_unit_model_set_header_title (model, 2,
                                                           1, "Аптека",
                                                           2, "Приход", "Расход"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_REST,
                                                         DO_REPORT_VALUE_TYPE_REST
                                                         );
                    break;
                case 3:
                    do_report_unit_model_set_header_title (model, 2,
                                                           1, "Аптека",
                                                           2, "Приход", "Расход"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_MONEY,
                                                         DO_REPORT_VALUE_TYPE_MONEY
                                                         );
                    break;
            };
            break;
        case 2 :
            if ( supplier_name )
                title = g_strdup_printf("Приход за период с %2.2d/%2.2d/%4.4d по %2.2d/%2.2d/%4.4d Поставщик: %s",
                                        date_start_day, date_start_month, date_start_year,
                                        date_end_day, date_end_month, date_end_year, supplier_name
                                        );
            else
                title = g_strdup_printf("Приход за период с %2.2d/%2.2d/%4.4d по %2.2d/%2.2d/%4.4d",
                                        date_start_day, date_start_month, date_start_year,
                                    date_end_day, date_end_month, date_end_year
                                    );
            switch ( kind_sum ) {
                case 1:
                    do_report_unit_model_set_header_title (model, 2,
                                                           1, "Аптека",
                                                           2, "Кол-во", "Закуп"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_REST,
                                                         DO_REPORT_VALUE_TYPE_MONEY
                                                         );
                    break;
                case 2:
                    do_report_unit_model_set_header_title (model, 1,
                                                           1, "Аптека"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_REST
                                                         );
                    break;
                case 3:
                    do_report_unit_model_set_header_title (model, 1,
                                                           1, "Аптека"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_MONEY
                                                         );
                    break;
            };
            break;
        case 3 :
            title = g_strdup_printf("Расход за период с %2.2d/%2.2d/%4.4d по %2.2d/%2.2d/%4.4d",
                                    date_start_day, date_start_month, date_start_year,
                                    date_end_day, date_end_month, date_end_year
                                    );
            switch ( kind_sum ) {
                case 1:
                    do_report_unit_model_set_header_title (model, 2,
                                                           1, "Аптека",
                                                           2, "Кол-во", "Розница"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_REST,
                                                         DO_REPORT_VALUE_TYPE_MONEY
                                                         );
                    break;
                case 2:
                    do_report_unit_model_set_header_title (model, 1,
                                                           1, "Аптека"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_REST
                                                         );
                    break;
                case 3:
                    do_report_unit_model_set_header_title (model, 1,
                                                           1, "Аптека"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_MONEY
                                                         );
                    break;
            };
            break;
        case 4 :
            title = g_strdup_printf("Остатки на дату с %2.2d/%2.2d/%4.4d",
                                     date_end_day, date_end_month, date_end_year
                                    );
            switch ( kind_sum ) {
                case 1:
                    do_report_unit_model_set_header_title (model, 2,
                                                           1, "Аптека",
                                                           2, "Кол-во", "Розница"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_REST,
                                                         DO_REPORT_VALUE_TYPE_MONEY
                                                         );
                    break;
                case 2:
                    do_report_unit_model_set_header_title (model, 1,
                                                           1, "Аптека"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_REST
                                                         );
                    break;
                case 3:
                    do_report_unit_model_set_header_title (model, 1,
                                                           1, "Аптека"
                                                           );
                    do_report_unit_model_set_value_type (model,
                                                         DO_REPORT_VALUE_TYPE_MONEY
                                                         );
                    break;
            };
            break;
    }
    do_meta_alias_t *meta;
    meta = domino_meta_alias_new();
    struct tm date_b;
    struct tm date_e;
    GString *title_str = g_string_new(title);
    g_free(title);

    if ( !meta ) {
        do_end_long_operation(GTK_WIDGET(dialog));
        return;
    }

    if  ( kind == 4 ) {
        time_t now;
        now = time(NULL);
        date_b = *localtime(&now);
    }
    else {
        do_date_set_ymd(&date_b, date_start_year, date_start_month, date_start_day);
        do_date_set_ymd(&date_e, date_end_year, date_end_month, date_end_day);
    }

    g_string_append_printf(title_str, " Аптеки: ");
    for ( indx = 0; indx < units->count; indx++ ) {
        do_sort_list_t *a_l = domino_meta_alias_get_list_from_sklad(meta, (char*)units->list[indx], &date_b, &date_e);
        if ( a_l && a_l->count ) {
            sklad_rec_t sklad;
            if ( do_alias_open(((do_alias_period_t*)a_l->list[0])->alias, FALSE, DO_DB_SKLAD, DO_DB_END) &&
                 domino_get_unit(((do_alias_period_t*)a_l->list[0])->alias, &sklad, (char*)units->list[indx]) ) {
                char *addr;
                addr = do_sklad_param(((do_alias_period_t*)a_l->list[0])->alias, &sklad, "25");
                g_string_append_printf(title_str, "%s: %s", (char*)units->list[indx], addr);
                do_free(addr);
            }
            do_alias_close(((do_alias_period_t*)a_l->list[0])->alias);
        }
        if ( a_l )
            do_list_free(a_l);
    }
    title = g_string_free(title_str, FALSE);
    do_report_unit_model_set_title(model, title);
    g_object_set(report, "title", title, NULL);
    g_free(title);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(report, "Name", "Наименование", 200);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    gtk_tree_view_column_set_cell_data_func(col, r,
                                            do_report_unit_model_cell_data_func_product_name,
                                            GINT_TO_POINTER(1), NULL);
    gtk_tree_view_column_set_sort_column_id (col, 1);


    for ( indx = 0, i = 2; indx <= units->count; indx++ ) {
        gchar *unit_name;
        if ( indx == units->count ) {
            if ( units->count == 1 )
                break;
            unit_name = g_strdup_printf("Итого");
        }
        else {
            unit_name = g_strdup_printf("Аптека %s", (char*)units->list[indx]);
        }
        switch ( kind ) {
            case 1:
                switch ( kind_sum ) {
                    case 1:
                        do_seller_dialog_add_col_tail(dialog, report, &i, TRUE,
                                                      g_strdup_printf("%s\nПриход\nКоличество", unit_name), show || indx == units->count);
                        do_seller_dialog_add_col_tail(dialog, report, &i, FALSE,
                                                      g_strdup_printf("\n\nСумма"), show || indx == units->count);
                        do_seller_dialog_add_col_tail(dialog, report, &i, TRUE,
                                                      g_strdup_printf("\nРасход\nКоличество"), show || indx == units->count);
                        do_seller_dialog_add_col_tail(dialog, report, &i, FALSE,
                                                      g_strdup_printf("\n\nСумма"), show || indx == units->count);
                        break;
                    case 2:
                        do_seller_dialog_add_col_tail(dialog, report, &i, TRUE,
                                                      g_strdup_printf("%s\nПриход", unit_name), show || indx == units->count);
                        do_seller_dialog_add_col_tail(dialog, report, &i, TRUE,
                                                      g_strdup_printf("\nРасход"), show || indx == units->count);
                        break;
                    case 3:
                        do_seller_dialog_add_col_tail(dialog, report, &i, FALSE,
                                                      g_strdup_printf("%s\nПриход", unit_name), show || indx == units->count);
                        do_seller_dialog_add_col_tail(dialog, report, &i, FALSE,
                                                      g_strdup_printf("\nРасход"), show || indx == units->count);
                        break;
                };
                break;
            default :
                switch ( kind_sum ) {
                    case 1:
                        do_seller_dialog_add_col_tail(dialog, report, &i, TRUE,
                                                      g_strdup_printf("%s\nКоличество", unit_name), show || indx == units->count);
                        do_seller_dialog_add_col_tail(dialog, report, &i, FALSE,
                                                      g_strdup_printf("\nСумма"), show || indx == units->count);
                        break;
                    case 2:
                        do_seller_dialog_add_col_tail(dialog, report, &i, TRUE,
                                                      g_strdup_printf("%s", unit_name), show || indx == units->count);
                        break;
                    case 3:
                        do_seller_dialog_add_col_tail(dialog, report, &i, FALSE,
                                                      g_strdup_printf("%s", unit_name), show || indx == units->count);
                        break;
                };
        }
        g_free(unit_name);
    }
    int price_purchase = DO_CONST_PRICE_PURCHASE;
    int price_fact = DO_CONST_PRICE_FACT;
    if ( kind_price == 2 )
        price_fact = DO_CONST_PRICE_PURCHASE;
    if ( kind_price == 3 )
        price_purchase = DO_CONST_PRICE_RETAIL;



    path = gtk_tree_path_new_first();



    while ( meta ) {
        gboolean next;
        if ( priv->mfr_report )
            next = gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->mfr_store), &iter, path);
        else
            next = gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->pstore), &iter, path);

        if ( !next ) break;

        GValue name = {0,};
        GValue code = {0,};
        GValue coef = {0,};
        GtkTreeIter iter_new;

        if ( priv->mfr_report ) {
            GValue mark = {0,};
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->mfr_store), &iter, 0, &mark);
            if ( !g_value_get_boolean(&mark) ) {
                gtk_tree_path_next(path);
                continue;
            }
            GValue lst = {0,};
            GValue mfr = {0,};

            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->mfr_store), &iter, 2, &lst);
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->mfr_store), &iter, 1, &mfr);

            do_sort_list_t *parcel;
            parcel = g_value_get_pointer(&lst);
            int k;
            char *name1 = "", *bar1 = "";
            int first = TRUE;

            for ( k = 0; k < parcel->count; k++ ) {
                gchar *name;
                parcel_t *p;
                p = parcel->list[k];

                name = g_strdup_printf("%s %s", p->name, g_value_get_string(&mfr));
                if ( first || strcmp(name1, p->name) || strcmp(bar1, p->bar) ) {

                    if ( first || !do_report_unit_model_is_empty(model, &iter_new) ) {

                        do_report_unit_model_append(model,
                                            p->code,
                                            name,
                                            &iter_new);
                    }
                    else {
                        do_report_unit_model_set_code( model,
                                            p->code,
                                            name,
                                            &iter_new);
                    }

                    name1=p->name;
                    bar1=p->bar;
                    first = FALSE;
                }
                do_seller_dialog_report1_code(p->code,
                                              FALSE,
                                              p->coef,
                                                  units,
                                                  priv,
                                                  kind,
                                                  kind_sum,
                                                  meta,
                                                  date_b,
                                                  date_e,
                                                  price_purchase,
                                                  price_fact,
                                                  model,
                                                  &iter_new);

            }

            if ( !first && do_report_unit_model_is_empty(model, &iter_new) ) {
                        do_report_unit_model_delete(model,
                                            &iter_new);
            }





        }
        else {

            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->pstore), &iter, 1, &name);
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->pstore), &iter, 0, &code);
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->pstore), &iter, 3, &coef);

            do_report_unit_model_append(model,
                                    g_value_get_string(&code),
                                    g_value_get_string(&name),
                                    &iter_new);
            do_seller_dialog_report1_code(g_value_get_string(&code),
                                            TRUE,
                                          g_value_get_double(&coef),
                                              units,
                                              priv,
                                              kind,
                                              kind_sum,
                                              meta,
                                              date_b,
                                              date_e,
                                              price_purchase,
                                              price_fact,
                                              model,
                                              &iter_new);
        }

        gtk_tree_path_next(path);
    }
    gtk_tree_path_free(path);
    g_object_set(report, "model", model, NULL);

    do_end_long_operation(GTK_WIDGET(dialog));
}
static void do_seller_dialog_setting_save (GtkWidget* widget)
{
    char buffer[512];
    sprintf(buffer, "%d", gtk_paned_get_position(GTK_PANED(widget)));
    DOMINO_PROFILE_SET(OBJECT_ROOT_PATH, "DoSellerDialog", "paned-position", buffer, NULL);
}

static gboolean paned_sync_position(GtkWidget *paned, GParamSpec *pspec, DoSellerDialog *view)
{
	do_seller_dialog_setting_save(paned);
	return TRUE; //!!
}
static gboolean get_stock(do_meta_alias_t *meta, const gchar *code, const gchar *sklad, struct tm *date, double *quant, double *sum)
{
    do_sort_list_t *aliases;

    aliases = domino_meta_alias_get_list_from_sklad(meta, sklad, date, date);
    if ( !aliases )
        return FALSE;
    gboolean res = FALSE;
    (*sum) = 0;
    (*quant) = 0;
    if ( aliases->count == 1 ) {
        res = TRUE;
        do_alias_period_t *sk;
        sk = aliases->list[0];
        if ( !do_alias_conected(sk->alias) &&
            !do_alias_open(sk->alias, TRUE) ) {
            res = FALSE;
        }
        if ( res ) {
            stock_key0_t stock_key0;
            product_key4_t product_key4;
            product_rec_t product;
            barcode_key1_t barcode_key1;
            do_alias_t *alias = sk->alias;
            double price = 0, main_price = 0;
            int status;

            do_text_set(alias, stock_key0.sklad, sklad);
            do_text_set(alias, product_key4.code, code);
            status= do_product_get4(alias, &product, &product_key4, DO_GET_GE);
            while ( status == DO_OK ) {
                if ( strncmp(product_key4.code, code, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)) )
                    break;
                do_cpy(barcode_key1.code, product_key4.code);

                if ( do_barcode_key1(alias, &barcode_key1, DO_GET_EQUAL) == DO_OK )
                    price = do_product_price(alias, &product, DO_CONST_PRICE_RETAIL);
                else
                    price = main_price;


                stock_rec_t stock;

                do_cpy(stock_key0.code, product_key4.code);

                status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
                if ( status == DO_ERROR )
                    break;
                if ( status == DO_OK ) {
                    if ( !main_price && !product.data.base_parcel ) {
                        main_price = do_stock_quant(alias, &stock, DO_CONST_QUANT_REST)+do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE);
                        price = main_price;
                    }

                    double q;
                    q = do_stock_quant(alias, &stock, DO_CONST_QUANT_REST)+do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE);
                    if ( q < 0.0001 || q > 0.0001) {
                        (*quant) = (*quant) + q;
                        (*sum) = (*sum) + q*price;
                    }
                }

                status= do_product_get4(alias, &product, &product_key4, DO_GET_NEXT);
            }
            if ( status == DO_ERROR )
                res = FALSE;

        }
    }
    do_list_free(aliases);
    return res;
}
typedef struct {
    char *name;
    do_sort_list_t *codes;
} mfr_list_t;

int mfr_cmp(mfr_list_t *s1, mfr_list_t *s2)
{
    return strcmp(s1->name, s2->name);
}

static void do_seller_dialog_report2(DoSellerDialog *dialog)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(dialog);
    DoTreeView *report;
    GtkTreeViewColumn *col;
    GtkCellRenderer *r;
    DoReportUnitModel *model;
    GtkNotebook  *nb;
    GtkTreeIter   iter;
    GtkTreePath  *path;
    do_string_list_t *units;
    //DoEnumValue *value;
    //!!gint        kind;
    //!!gint        kind_sum;
    //!!gint        n_values;
    gint        i, indx;
    gint date_start_year;
    gint date_start_month;
    gint date_start_day;
    gint date_end_year;
    gint date_end_month;
    gint date_end_day;
    GList *list;
    gchar *title;
    gchar *str;
    //gint k;

    do_seller_dialog_save_value(dialog);

    do_start_long_operation(GTK_WIDGET(dialog));
    nb = GTK_NOTEBOOK (do_window_get_notebook(
                    DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(dialog)))));
    report = DO_TREE_VIEW(do_report_view_new());
    g_object_set(report, "title", "Детальный приход по представителю ",NULL);
    do_notebook_add_tab(DO_NOTEBOOK(nb), DO_VIEW(report), -1, TRUE);

    g_object_get(priv->widget[SKLADS_ENTRY], "units", &str, NULL);
    units = do_string_list_new_from_csv(str);
    //g_object_unref(value);
    g_object_get(priv->entry[DATE_B_ENTRY], "year", &date_start_year,
                                            "month", &date_start_month,
                                            "day", &date_start_day,
                                            NULL);
    g_object_get(priv->entry[DATE_E_ENTRY], "year", &date_end_year,
                                            "month", &date_end_month,
                                            "day", &date_end_day,
                                            NULL);
    gboolean show = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->widget[KIND_SHOW_UNIT]));

    model = (DoReportUnitModel*)do_report_unit_model_new(units->count, 1,
                                     ((units->count > 1) ? DO_REPORT_UNIT_MODEL_FLAG_TOTAL_COLUMN : 0)
                                                        /*|
                                     DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW*/
                                     );
    do_report_unit_model_set_print_units(model, show);

    //g_object_set(report, "model", model, NULL);
    list = NULL;
    for ( indx = 0; indx < units->count; indx++ )
        list = g_list_append(list, units->list[indx]);
    do_report_unit_model_set_unit_title(model, list);
    g_list_free(list);

    title = g_strdup_printf("Приход за период с %2.2d/%2.2d/%4.4d по %2.2d/%2.2d/%4.4d",
                            date_start_day, date_start_month, date_start_year,
                            date_end_day, date_end_month, date_end_year
                            );
    do_report_unit_model_set_header_title (model, 1,
                                           1, "Аптека"
                                           );
    do_report_unit_model_set_value_type (model,
                                         DO_REPORT_VALUE_TYPE_REST
                                         );

    do_meta_alias_t *meta;
    meta = domino_meta_alias_new();
    struct tm date_b;
    struct tm date_e;
    GString *title_str = g_string_new(title);
    g_free(title);

    if ( !meta ) {
        do_end_long_operation(GTK_WIDGET(dialog));
        return;
    }

    do_date_set_ymd(&date_b, date_start_year, date_start_month, date_start_day);
    do_date_set_ymd(&date_e, date_end_year, date_end_month, date_end_day);

    for ( indx = 0; indx < units->count; indx++ ) {
        do_sort_list_t *a_l = domino_meta_alias_get_list_from_sklad(meta, (char*)units->list[indx], &date_b, &date_e);
        if ( a_l && a_l->count ) {
            sklad_rec_t sklad;
            if ( do_alias_open(((do_alias_period_t*)a_l->list[0])->alias, FALSE, DO_DB_SKLAD, DO_DB_END) &&
                 domino_get_unit(((do_alias_period_t*)a_l->list[0])->alias, &sklad, (char*)units->list[indx]) ) {
                char *addr;
                addr = do_sklad_param(((do_alias_period_t*)a_l->list[0])->alias, &sklad, "25");
                g_string_append_printf(title_str, "<br>%s: %s", (char*)units->list[indx], addr);
                do_free(addr);
            }
            do_alias_close(((do_alias_period_t*)a_l->list[0])->alias);
        }
        if ( a_l )
            do_list_free(a_l);
    }
    title = g_string_free(title_str, FALSE);
    do_report_unit_model_set_title(model, title);
    g_free(title);

    r = gtk_cell_renderer_text_new();
    col = do_tree_view_add_column(report, "Name", "Наименование", 200);
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    gtk_tree_view_column_set_cell_data_func(col, r,
                                            do_report_unit_model_cell_data_func_product_name,
                                            GINT_TO_POINTER(1), NULL);
    gtk_tree_view_column_set_sort_column_id (col, 1);


    for ( indx = 0, i = 2; indx <= units->count; indx++ ) {
        gchar *unit_name;
        if ( indx == units->count ) {
            if ( units->count == 1 )
                break;
            unit_name = g_strdup_printf("Итого");
        }
        else {
            unit_name = g_strdup_printf("Аптека %s", (char*)units->list[indx]);
        }
        do_seller_dialog_add_col_tail(dialog, report, &i, TRUE, unit_name, show || indx == units->count);
    }

    path = gtk_tree_path_new_first();
    const char *dtype_in = do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN);
    //!!const char *dtype_out = do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT);
    do_string_list_t *dtype1_in = do_string_list_new_from_csv("01");
    //!!do_string_list_t *dtype1_out = do_string_list_new_from_csv("01,02,03,04");
    while ( meta && gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->pstore), &iter, path) ) {
        do_report_product_traffic_detail_t *detail;
        GValue name = {0,};
        GValue code = {0,};
        GValue coef = {0,};
        GtkTreeIter iter_pr;
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->pstore), &iter, 1, &name);
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->pstore), &iter, 0, &code);
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->pstore), &iter, 3, &coef);

        do_report_unit_model_append(model,
                                    g_value_get_string(&code),
                                    g_value_get_string(&name),
                                    &iter_pr);
        do_report_unit_model_set_grey(model, &iter_pr, 0);

        detail = NULL;
        for ( indx = 0; indx < units->count; indx++ ) {

            detail = do_report_product_meta_traffic_detail(meta, g_value_get_string(&code),TRUE,
                                               dtype_in, dtype1_in,
                                               (char*)units->list[indx],
                                                &date_b, &date_e,
                                                DO_CONST_QUANT_REST,
                                                DO_CONST_PRICE_PURCHASE,
                                                detail);
            if ( !detail )
                break;
        }
        if ( detail ) {
            int par_indx;
            int doc_indx;
            for ( par_indx = 0; par_indx < do_report_product_traffic_partner_count(detail); par_indx++ ) {
                char *value;
                GtkTreeIter iter_par;
                value = g_strdup_printf("\t%s", do_report_product_traffic_partner_name(detail, par_indx));
                do_report_unit_model_append(model,
                                            g_value_get_string(&code),
                                            value,
                                            &iter_par);
                do_report_unit_model_set_grey(model, &iter_par, 0);

                for ( doc_indx = 0; doc_indx < do_report_product_traffic_document_for_partner_count(detail, par_indx); doc_indx++ ) {
                    int i, k;
                    double quant;
                    GtkTreeIter iter_new;
                    k = do_report_product_traffic_document_for_partner_document(detail, par_indx, doc_indx);
                    value = g_strdup_printf("\t\t%s от %2.2d-%2.2d-%4.4d", do_report_product_traffic_document_p_doc(detail, k),
                                        do_report_product_traffic_document_p_time(detail, k)->tm_mday,
                                        do_report_product_traffic_document_p_time(detail, k)->tm_mon+1,
                                        do_report_product_traffic_document_p_time(detail, k)->tm_year+1900
                                        );
                    do_report_unit_model_append(model,
                                            g_value_get_string(&code),
                                            value,
                                            &iter_new);
                    do_report_unit_model_set_no_total(model, &iter_new, 0);
                    g_free(value);

                    quant = 0;
                    for ( i = 0; i < do_report_product_traffic_document_line_count(detail, k); i++ )
                        quant += do_report_product_traffic_document_line_quant(detail, k ,i);

                    for ( indx = 0; indx < units->count; indx++ )
                        if ( !strcmp(units->list[indx], do_report_product_traffic_document_sklad(detail, k)) ) {
                            do_report_unit_model_set_unit_value(model, &iter_new,
                                                indx, 0, quant/g_value_get_double(&coef));

                            do_report_unit_model_set_unit_value(model, &iter_par, indx, 0,
                                                                do_report_unit_model_get_unit_value(model, &iter_par, indx, 0)
                                                                 + quant/g_value_get_double(&coef));
                            do_report_unit_model_set_unit_value(model, &iter_pr, indx, 0,
                                                                do_report_unit_model_get_unit_value(model, &iter_pr, indx, 0)
                                                                 + quant/g_value_get_double(&coef));


                        break;
                    }
                }

            }

            do_report_product_traffic_detail_free(detail);
        }
        gtk_tree_path_next(path);
    }
    gtk_tree_path_free(path);
    g_object_set(report, "model", model, NULL);

    do_end_long_operation(GTK_WIDGET(dialog));
}
static void do_seller_dialog_create_manufactor_view(DoSellerDialog *edit);
static gboolean unmarkall(GtkTreeModel *model,GtkTreePath *path,GtkTreeIter *iter, gpointer data);

/*
static void do_seller_dialog_clicked_manufactor(GtkButton *widget, DoSellerDialog *edit)
{

	DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (edit);
    do_meta_alias_t *meta;

    struct tm date_b;
    struct tm date_e;
    gint date_start_year;
    gint date_start_month;
    gint date_start_day;
    gint date_end_year;
    gint date_end_month;
    gint date_end_day;
    do_string_list_t *units;
    DoEnumValue *value;


    meta = domino_meta_alias_new();
    if ( !meta )
        return;

    g_object_get(priv->widget[SKLADS_ENTRY], "value", &value, NULL);
    units = do_string_list_new_from_csv(do_enum_value_get_string(value));
    g_object_unref(value);
    g_object_get(priv->entry[DATE_B_ENTRY], "year", &date_start_year,
                                            "month", &date_start_month,
                                            "day", &date_start_day,
                                            NULL);
    g_object_get(priv->entry[DATE_E_ENTRY], "year", &date_end_year,
                                            "month", &date_end_month,
                                            "day", &date_end_day,
                                            NULL);

    do_start_long_operation(GTK_WIDGET(edit));

    do_date_set_ymd(&date_b, date_start_year, date_start_month, date_start_day);
    do_date_set_ymd(&date_e, date_end_year, date_end_month, date_end_day);

    do_sort_list_t *aliases;
    GtkTreePath *path;
    GtkTreeIter iter;
    do_sort_list_t *manufactors;

    manufactors = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func) mfr_cmp, NULL);

    aliases = domino_meta_alias_get_list_from_sklads(meta, units, &date_b, &date_e);

    path = gtk_tree_path_new_first();

    gboolean res;
    res = TRUE;


    while ( gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->pstore), &iter, path) && res ) {
        GValue code = {0,};
        int i,indx;

        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->pstore), &iter, 0, &code);

        for ( i = 0; i < aliases->count && res; i++ ) {
            do_alias_period_t *p;
            do_alias_t *alias;
            int j;

            p = aliases->list[i];
            alias = p->alias;
            if ( !do_alias_conected(alias) && !do_alias_open(alias, TRUE) ) {
                res = FALSE;
                break;
            }

            for ( j = 0; j < units->count; j ++ ) {
                if ( !do_string_list_find(p->sklads, &indx, units->list[j]) )
                    continue;

                int status;
                char *other;
                prowod_key2_t prowod_key2, key;
                prowod_rec_t prowod;

                do_text_set(alias, prowod_key2.division, domino_division());
                do_text_set(alias, prowod_key2.debit_account, "ПРИХОД");
                do_text_set_empty(prowod_key2.debit_subaccount);
                do_text_set_empty(prowod_key2.debit_p_g_code);
                prowod_key2.debit_p_code=0;
                other = g_strdup_printf("%s.%s", (char*)units->list[j], g_value_get_string(&code));
                do_text_set(alias, prowod_key2.debit_other, other);
                g_free(other);
                do_date_set(&prowod_key2.date, date_b);
                prowod_key2.date = 0;//!!
                do_cpy(key, prowod_key2);
                do_date_set(&key.date, date_e);
                status = do_prowod_get2(alias, &prowod, &prowod_key2, DO_GET_GE);

                while ( status == DO_OK ) {
                    if (  prowod_key2.date > key.date ||
                          memcmp(&prowod_key2, &key, sizeof(key)-sizeof(key.date))
                          ) break;
                    char *doc, *head, *ch;
                    document_order_key2_t document_order_key2, key2;
                    document_order_rec_t document_order;

                    doc = do_prowod_param(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_DOCUMENT));
                    if ( doc[0] != '@' ) {
                        do_log(LOG_ERR, "Ошибка в описании проводки \"%s\"", do_prowod_param(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_DOCUMENT)));
                        res = FALSE;
                        break;
                    }
                    head = doc+1;
                    ch = strchr(head, '.');
                    if ( !ch ) {
                        do_log(LOG_ERR, "Ошибка в описании проводки \"%s\"", do_prowod_param(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_DOCUMENT)));
                        res = FALSE;
                        break;
                    }
                    *ch = '\0';
                    do_text_set(alias, document_order_key2.document, head);
                    head = ch+1;
                    ch = strchr(head, '/');
                    if ( !ch ) {
                        do_log(LOG_ERR, "Ошибка в описании проводки \"%s\"", do_prowod_param(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_DOCUMENT)));
                        res = FALSE;
                        break;
                    }
                    *ch = '\0';
                    do_text_set(alias, document_order_key2.dtype, head);
                    head = ch+1;
                    do_text_set(alias, document_order_key2.sklad, head);
                    do_free(doc);
                    ch = (char*)g_value_get_string(&code);
                    do_text_set(alias, document_order_key2.code, ch);
                    do_cpy(key2, document_order_key2);
                    status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_GE);
                    while ( status == DO_OK ) {
                        if ( do_cmp(key2.dtype, document_order_key2.dtype) ||
                             do_cmp(key2.sklad, document_order_key2.sklad) ||
                             do_cmp(key2.document, document_order_key2.document) ||
                             strncmp(ch, document_order_key2.code, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)) ) break;

                            mfr_list_t m, *crnt;
                            m.name = do_document_order_param(alias, &document_order, "RZ_MFR");

                            if ( m.name[0] != '\0' && !do_sort_list_find(manufactors, &indx, &m) ) {
                                m.codes = do_string_list_new();
                                do_sort_list_insert_new(manufactors, indx, &m, sizeof(m));
                            }
                            else {
                                if ( m.name[0] != '\0' ) {
                                    crnt = manufactors->list[indx];
                                    do_string_list_add(crnt->codes, do_text(alias, document_order.data.code));
                                }
                                do_free(m.name);
                            }

                        status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_NEXT);
                    }
                    if ( status == DO_ERROR ) {
                        res = FALSE;
                        break;
                    }
                    status = do_prowod_get2(alias, &prowod, &prowod_key2, DO_GET_NEXT);
                }
                if ( status == DO_ERROR ) {
                    res = FALSE;
                    break;
                }
            }
        }
        gtk_tree_path_next(path);
    }
    do_sort_list_free(aliases);
    gtk_tree_path_free(path);
    domino_meta_alias_free(meta);
    do_end_long_operation(GTK_WIDGET(edit));

    if ( !priv->mfr_view )
        do_seller_dialog_create_manufactor_view(edit);
    gtk_list_store_clear(priv->mfr_store);
    int i;
    for ( i = 0; i < manufactors->count; i++ ) {
        GtkTreeIter iter;
        mfr_list_t *crnt;
        crnt = manufactors->list[i];
        gtk_list_store_append(priv->mfr_store, &iter);
        gtk_list_store_set(priv->mfr_store, &iter, 1, crnt->name, 2, crnt->codes, -1);
    }
    gtk_tree_model_foreach(GTK_TREE_MODEL(priv->pstore), unmarkall, NULL);
    gtk_tree_view_column_set_visible(priv->mfr_col, TRUE);

}
*/
static int parcel_cmp(parcel_t *s1, parcel_t *s2)
{
    int i;
    i = strcmp(s1->name, s2->name);
    if ( i )
        return i;
    return strcmp(s1->bar, s2->bar);
}
static void do_seller_dialog_clicked_manufactor(GtkButton *widget, DoSellerDialog *edit)
{

	DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (edit);


    GtkTreePath *path;
    GtkTreeIter iter;
    do_sort_list_t *manufactors;

    do_alias_t *alias;


    manufactors = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func) mfr_cmp, NULL);

    alias = domino_alias_new("main");

    if ( !alias || !do_alias_open(alias, TRUE) ) {
        return;
    }

    do_start_long_operation(GTK_WIDGET(edit));
    path = gtk_tree_path_new_first();

    gboolean res;
    res = TRUE;


    while ( gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->pstore), &iter, path) && res ) {
        GValue code = {0,};
        GValue name = {0,};
        GValue coef = {0,};
        int indx;

        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->pstore), &iter, 0, &code);
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->pstore), &iter, 1, &name);
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->pstore), &iter, 3, &coef);

        const gchar *product_code = g_value_get_string(&code);
        product_key0_t product_key0;
        product_rec_t product;

        do_text_set(alias, product_key0.code, product_code);
        product_key0.base_parcel = 0;

        if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
            do_log(LOG_ERR, "Товар \"%s\" не найден", product_code);
            res = FALSE;
            break;
        }
        if ( do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS))
           == DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE ) {

            int status;
            product_key0.base_parcel = 1;
            status = do_product_get0(alias, &product, &product_key0, DO_GET_GE);

            while ( status == DO_OK ) {
                if (  strncmp(product_key0.code, product_code, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH) ) ||
                     product_key0.base_parcel != 1
                      ) break;

                    parcel_t parcel;

                    mfr_list_t m, *crnt;
                    m.name = do_product_param(alias, &product, "RZ_MFR");
                    parcel.bar = do_product_param(alias, &product, "BARCODE");
                    parcel.name = (char*)g_value_get_string(&name);
                    parcel.coef = g_value_get_double(&coef);
                    parcel.code = do_text(alias, product.data.code);


                    if ( strlen(m.name) > 2 && !do_sort_list_find(manufactors, &indx, &m) ) {
                        int i;
                        gboolean found;
                        found = FALSE;
                        for ( i = 0; i < manufactors->count; i++ ) {
                            int j;
                            crnt = manufactors->list[i];
                            for ( j = 0; j < crnt->codes->count; j++ ) {
                                parcel_t *p;
                                p = crnt->codes->list[j];
                                if ( !strcmp(p->bar,parcel.bar) ) {
                                    found = TRUE;
                                    do_sort_list_add_alloc(crnt->codes, &parcel, sizeof(parcel));
                                    break;
                                }
                            }
                        }
                        if ( ! found ) {
                            m.codes = do_sort_list_new(TRUE, FALSE, (do_list_cmp_func)parcel_cmp, NULL);
                            do_sort_list_add_alloc(m.codes, &parcel, sizeof(parcel));
                            do_sort_list_insert_new(manufactors, indx, &m, sizeof(m));
                        }
                    }
                    else {
                        if ( strlen(m.name) > 2 ) {
                            crnt = manufactors->list[indx];
                            do_sort_list_add_alloc(crnt->codes, &parcel, sizeof(parcel));
                        }
                        do_free(m.name);
                    }

                status = do_product_get0(alias, &product, &product_key0, DO_GET_NEXT);
            }
            if ( status == DO_ERROR ) {
                res = FALSE;
                break;
            }
        }
        gtk_tree_path_next(path);
    }
    do_alias_free(alias);
    gtk_tree_path_free(path);
    do_end_long_operation(GTK_WIDGET(edit));

    if ( !priv->mfr_view )
        do_seller_dialog_create_manufactor_view(edit);
    gtk_list_store_clear(priv->mfr_store);
    int i;
    for ( i = 0; i < manufactors->count; i++ ) {
        GtkTreeIter iter;
        mfr_list_t *crnt;
        crnt = manufactors->list[i];
        gtk_list_store_append(priv->mfr_store, &iter);
        gtk_list_store_set(priv->mfr_store, &iter, 1, crnt->name, 2, crnt->codes, -1);
    }
    gtk_tree_model_foreach(GTK_TREE_MODEL(priv->pstore), unmarkall, NULL);
    gtk_tree_view_column_set_visible(priv->mfr_col, TRUE);
    priv->mfr_report=TRUE;

}
static void cell_received(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, 0, &value);
    if (g_value_get_boolean(&value))
#if GTK_MAJOR_VERSION > 2
        g_object_set(cell, "icon-name", "object-select-symbolic", NULL);
#else
        g_object_set(cell, "icon-name", GTK_STOCK_APPLY, NULL);
#endif
    else
        g_object_set(cell, "icon-name", NULL, NULL);

}

static gboolean mark(GtkTreeModel *model,GtkTreePath *path,GtkTreeIter *iter, gpointer data)
{
    do_string_list_t *codes = data;
    int i;
    GValue code={0,};
    gtk_tree_model_get_value(model, iter, 0, &code);

    for ( i = 0; i < codes->count; i++ ) {
        parcel_t *p;
        p = codes->list[i];
        if ( !strncmp(g_value_get_string(&code), p->code, 5) ) {
            gtk_list_store_set(GTK_LIST_STORE(model), iter, 4, FALSE, -1);
            return FALSE;
        }
    }
    return FALSE;
}
static gboolean unmark(GtkTreeModel *model,GtkTreePath *path,GtkTreeIter *iter, gpointer data)
{
    DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(data);
    int i;
    GValue code={0,};
    gboolean unmark = TRUE;
    gtk_tree_model_get_value(model, iter, 0, &code);

    GtkTreePath *p;
    GtkTreeIter it;
    p = gtk_tree_path_new_first();

    while ( unmark && gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->mfr_store), &it, p) ) {
        GValue mark={0,};
        GValue lst={0,};
        do_string_list_t *codes;
        gtk_tree_model_get_value(GTK_TREE_MODEL(priv->mfr_store), &it, 0, &mark);
        if ( g_value_get_boolean(&mark) ) {
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->mfr_store), &it, 2, &lst);
            codes = g_value_get_pointer(&lst);
            for ( i = 0; i < codes->count; i++ ) {
                parcel_t *p;
                p = codes->list[i];
                if ( !strncmp(g_value_get_string(&code), p->code, 5) ) {
                    unmark = FALSE;
                    break;
                }
            }
        }
        gtk_tree_path_next(p);
    }
    gtk_tree_path_free(p);
    gtk_list_store_set(GTK_LIST_STORE(model), iter, 4, unmark, -1);
    return FALSE;
}
static gboolean unmarkall(GtkTreeModel *model,GtkTreePath *path,GtkTreeIter *iter, gpointer data)
{
    gtk_list_store_set(GTK_LIST_STORE(model), iter, 4, TRUE, -1);
    return FALSE;
}
static void row_activated(GtkTreeView *tree_view, GtkTreePath *path,  GtkTreeViewColumn *column, DoSellerDialog *view)
{
	DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE(view);
	GtkTreeIter iter;
	GValue value={0,}, lst = {0,};
	do_string_list_t *codes;

	gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->mfr_store), &iter, path);

	gtk_tree_model_get_value(GTK_TREE_MODEL(priv->mfr_store), &iter, 0, &value);
	gtk_tree_model_get_value(GTK_TREE_MODEL(priv->mfr_store), &iter, 2, &lst);

	codes = g_value_get_pointer(&lst);

	gtk_list_store_set(priv->mfr_store, &iter, 0, !g_value_get_boolean(&value), -1);

    if ( !g_value_get_boolean(&value) )
        gtk_tree_model_foreach(GTK_TREE_MODEL(priv->pstore), mark, codes);
    else
        gtk_tree_model_foreach(GTK_TREE_MODEL(priv->pstore), unmark, view);

}

static void do_seller_dialog_create_manufactor_view(DoSellerDialog *edit)
{
	DoSellerDialogPrivate *priv = DO_SELLER_DIALOG_GET_PRIVATE (edit);


    priv->mfr_store = gtk_list_store_new(3, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_POINTER);

	priv->mfr_view = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(priv->mfr_store)));

    GtkWidget *scroll;

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gtk_container_add(GTK_CONTAINER(scroll), GTK_WIDGET(priv->mfr_view));

    GtkTreeViewColumn *col;
    GtkCellRenderer *r;

    g_signal_connect (priv->mfr_view, "row-activated", G_CALLBACK(row_activated), edit);


    col = gtk_tree_view_column_new();
    r = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, r, cell_received, edit, NULL);
    g_object_set (r,
                    "xalign", (gfloat)0.5,
                    "yalign", (gfloat)0.5,
                    "mode", GTK_CELL_RENDERER_MODE_INERT,
                    NULL);
    gtk_tree_view_column_set_sort_column_id(col, 0);

    gtk_tree_view_append_column(priv->mfr_view, col);

    r = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    gtk_tree_view_column_set_title (col, "Производитель");
    gtk_tree_view_append_column(priv->mfr_view,col);
    gtk_tree_view_column_set_sort_column_id(col, 1);
    gtk_tree_view_column_set_sort_order(col, GTK_SORT_ASCENDING);
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(priv->mfr_store), 1, GTK_SORT_ASCENDING);



    do_tree_view_pack_right(DO_TREE_VIEW(edit), scroll);
    gtk_widget_show_all(scroll);

}

