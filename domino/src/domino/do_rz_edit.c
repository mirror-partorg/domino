

#include <string.h>
#include <gtk/gtk.h>

#include "do_rz_edit.h"
#include "do_icons.h"
#include "do_utilx.h"
#include "do_entry.h"
#include "do_entry_calc.h"
#include "do_filter.h"
#include "do_rz_view.h"
#include "do_enum_box.h"
#include "do_enum_value.h"
#include "do_validate.h"
#include "do_check_dialog.h"
#include "do_product_edit.h"
#include "do_rz_select.h"
#include "domino.h"
#include "hig.h"
#include "../misc/utf8util.h"
#include <dolib.h>
#include <domino.h>


enum
{
	PROP_0,
	PROP_FLAGS,
	PROP_ACTION,
	PROP_CONTEXT,
	PROP_ALIAS,
	PROP_VIEW,
};

#define DO_RZ_EDIT_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_RZ_EDIT, DoRzEditPrivate))
#define DO_TYPE_RZ_EDIT_FLAGS do_rz_edit_flags_get_type()


#define SEPARATOR "от"
#define EXTENTION_FILE ".pdf"

GType do_rz_edit_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_rz_edit_flags_values[] = {
         { DO_RZ_EDIT_FLAGS_READ_ONLY, "DO_RZ_EDIT_FLAGS_READ_ONLY", "read-only" },
         { DO_RZ_EDIT_FLAGS_LINK_ONLY, "DO_RZ_EDIT_FLAGS_LINK", "link-only" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoRzEditFlags", _do_rz_edit_flags_values);
    }
    return type;
}

enum
{
	MNN_ENTRY,
	TRADE_ENTRY,
	PACK_FORM_ENTRY,
	PACK_QUANT_ENTRY,
	MANUFACTOR_ENTRY,
	PRICE_ENTRY,
	VALUTA_ENTRY,
	BARCODE_ENTRY,
	UNPACK_ENTRY,
	LINK_ENTRY,
	ORDER_ENTRY,
	PRODUCT_ENTRY,
	N_ENTRIES
};

struct _DoRzEditPrivate
{
	DoRzEditFlags flags;

	GtkWidget *widget[N_ENTRIES];
	GtkWidget *entry[N_ENTRIES];

	GtkWidget *link_button;

	DoView     *view;
	DoDialogAction action;
	DoContext  *context;

	gchar      *alias_name;
    rz_info_t  *rz;

};

#define LINK_DISCONNECT GTK_STOCK_DISCONNECT
#define LINK_CONNECT GTK_STOCK_CONNECT

G_DEFINE_TYPE (DoRzEdit, do_rz_edit, GTK_TYPE_BOX)

static GtkWidget *add_field(DoRzEdit *dialog, gint id, GtkWidget *widget);

static void entry_activate(GtkWidget *entry, DoRzEdit *dialog);
static gboolean do_rz_edit_fill_entry(DoRzEdit *dialog);
static gboolean do_rz_edit_accept_record(DoRzEdit *dialog);

static void do_rz_edit_init(DoRzEdit *dialog)
{

}
gboolean do_rz_edit_close(DoRzEdit *edit)
{
    gtk_widget_destroy(GTK_WIDGET(edit));
    return TRUE;
}
static void do_rz_edit_save(DoRzEdit *edit)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (edit);
	int i;
    for (i = 0; i < N_ENTRIES; ++i)
        if (DO_IS_VALIDATE(priv->widget[i]) &&
            !do_validate_valid(DO_VALIDATE(priv->widget[i]))) {
            gtk_widget_grab_focus(priv->widget[i]);
            return;
        }
    do_rz_edit_accept_record(edit);
    //do_rz_edit_close(edit);
}

static void do_rz_edit_clicked_close(GtkButton *widget, DoRzEdit *edit)
{
    do_rz_edit_close(edit);
}
static void do_rz_edit_clicked_save(GtkButton *widget, DoRzEdit *edit)
{
    do_rz_edit_save(edit);
}
static void do_rz_edit_clicked_view_product(GtkButton *widget, DoRzEdit *edit)
{
    do_rz_edit_show_product(edit, "");
}
static void do_rz_edit_select_activate(GtkTreeView       *tree_view,
                                                        GtkTreePath       *path,
                                                        GtkTreeViewColumn *column,
                                                        GtkDialog *dialog)
{
    gtk_dialog_response(dialog, GTK_RESPONSE_YES);
}
static void do_rz_edit_clicked_link(GtkButton *widget, DoRzEdit *edit)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (edit);
	GtkWidget *dialog;
	GtkWidget *select;
	GtkWidget *vbox;
	GtkWidget *tree_view;
	rz_info_t *rz;

    dialog =  gtk_dialog_new_with_buttons("Оригинальные записи",
                        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(edit))),
                        GTK_DIALOG_MODAL,
                        "Выбрать", GTK_RESPONSE_YES,
                        "Отмена", GTK_RESPONSE_NO,
                        NULL);

    g_signal_connect (dialog, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), "DoRzEditSelect");

    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(dialog), "DoRzEditSelect",
              "window-position", "center-on_parent",
              "window-size", "",
               NULL);
    gtk_window_set_icon_name (GTK_WINDOW(dialog), DO_STOCK_DOMINO_APPS);
    vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    select = do_rz_select_new(priv->alias_name, gtk_entry_get_text(GTK_ENTRY(priv->entry[LINK_ENTRY])));

    g_object_get(select, "tree-view", &tree_view, NULL);
    g_signal_connect(tree_view, "row-activated", G_CALLBACK(do_rz_edit_select_activate), dialog);
    gtk_box_pack_start (GTK_BOX(vbox), GTK_WIDGET(select), TRUE, TRUE, 0);

    do_tree_view_grab_focus(DO_TREE_VIEW(select));

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
        rz = do_rz_select_get_select((DoRzSelect*)select);
        if (rz) {
            gchar *value;
            struct tm tm;

            gtk_entry_set_text(GTK_ENTRY(priv->widget[MNN_ENTRY]), rz->mnn);
            gtk_entry_set_text(GTK_ENTRY(priv->widget[TRADE_ENTRY]), rz->trade_name);
            gtk_entry_set_text(GTK_ENTRY(priv->widget[PACK_FORM_ENTRY]), rz->pack_form);
            value = g_strdup_printf("%d", rz->pack_quant);
            gtk_entry_set_text(GTK_ENTRY(priv->widget[PACK_QUANT_ENTRY]), value);
            g_free(value);
            value = g_strdup_printf("%.2f", rz->price);
            gtk_entry_set_text(GTK_ENTRY(priv->widget[PRICE_ENTRY]), value);
            g_free(value);
            gtk_entry_set_text(GTK_ENTRY(priv->widget[VALUTA_ENTRY]), do_get_valuta_name(rz->valuta));
            gtk_entry_set_text(GTK_ENTRY(priv->widget[MANUFACTOR_ENTRY]), rz->manufactor);

            tm = *localtime(&rz->order.time);
            value = g_strdup_printf("%s "SEPARATOR" %s", rz->order.number, do_datetoa_c(&tm));
            gtk_entry_set_text(GTK_ENTRY(priv->widget[ORDER_ENTRY]), value);
            g_free(value);

            DoEnumValue *enum_ = do_enum_value_new();
            do_enum_value_set_boolean(enum_, rz->unpack);
            g_object_set(priv->widget[UNPACK_ENTRY], "value", enum_, NULL);
            g_object_unref(enum_);
#if GTK_MAJOR_VERSION > 2
            gtk_entry_set_icon_from_icon_name(GTK_ENTRY(priv->widget[LINK_ENTRY]),
                       GTK_ENTRY_ICON_PRIMARY, "network-transmit-receive-symbolic");
#else
            gtk_entry_set_icon_from_stock(GTK_ENTRY(priv->widget[LINK_ENTRY]),
                       GTK_ENTRY_ICON_PRIMARY, LINK_CONNECT);
#endif
        }
    }

    gtk_widget_destroy(dialog);

    //do_rz_edit_show_product(edit, "");
}
void do_rz_edit_show_product(DoRzEdit *edit, const gchar *tab)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (edit);
    do_alias_t *alias;
    barcode_rec_t barcode;
    barcode_key0_t barcode_key0;
    gchar *code;
    GtkWidget *dialog;
	GtkNotebook *nb;

    alias = domino_alias_new(priv->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_BARCODE, DO_DB_END))
        return;

    do_text_set(alias, barcode_key0.barcode, gtk_entry_get_text((GtkEntry*)priv->entry[BARCODE_ENTRY]));
    if (do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL) != DO_OK) {
        DOMINO_SHOW_ERROR("Товар с продажный кодом <b>%s</b> не найдена", do_text(alias,barcode_key0.barcode));
        return;
    }
    code = do_text(alias, barcode.data.code);
    do_alias_free(alias);

    DoContext *context = do_context_new();
    do_context_set_kind(context, DO_CONTEXT_KIND_PRODUCT);
    g_object_set(context, "product-code", code, NULL);
    g_free(code);

    if (DO_IS_VIEW(edit)) {
        dialog= do_product_edit_new(DO_VIEW(edit), priv->alias_name,
                                            DO_PRODUCT_EDIT_FLAGS_READ_ONLY,
                                              DO_DIALOG_ACTION_NONE, context, domino_unit());
        g_object_set(dialog, "view", edit, "tab", tab, NULL);
        g_object_unref(context);

        nb = GTK_NOTEBOOK (do_window_get_notebook (
	                    DO_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(edit)))));
        do_notebook_add_tab(DO_NOTEBOOK(nb), DO_VIEW(dialog), -1, TRUE);
    }
}

static void do_rz_edit_clicked_view_order(GtkButton *widget, DoRzEdit *edit)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (edit);
    gchar *uri;
    gchar *pdf_reader;
    GError *error = NULL;
    GFile *src;
    GFile *dst;
    gchar *temp;
    gchar *src_name;
    gchar *value;
    gchar *buf;
    gchar *tmp_name;
    gchar *ch;

    DOMINO_COMMON_GET("Rznd", "repository", &uri, NULL);
    DOMINO_LOCAL_GET("Extended", "pdf", &pdf_reader, NULL);
    if ( !uri || !pdf_reader )
	return;

    if (!do_validate_valid(DO_VALIDATE(priv->entry[ORDER_ENTRY]))) {
        gtk_widget_grab_focus(GTK_WIDGET(priv->widget[ORDER_ENTRY]));
        return;
    }
    value = g_strdup(gtk_entry_get_text(GTK_ENTRY(priv->entry[ORDER_ENTRY])));
    ch = strstr(value, SEPARATOR);
    *(ch - 1) = '\0';
    ch = strchr(value, '/');
    if (ch) {
        *ch = '\0';
        buf = g_uri_escape_string(value, NULL, FALSE);
        src_name = g_strdup_printf("%s/%s/%s"EXTENTION_FILE, uri, ch+1, buf);
        tmp_name = g_strdup_printf("%s"EXTENTION_FILE, value);
    }
    else {
        buf = g_uri_escape_string(value, NULL, FALSE);
        src_name = g_strdup_printf("%s/%s"EXTENTION_FILE, uri, buf);
        tmp_name = g_strdup_printf("%s"EXTENTION_FILE, value);
    }
    g_free(value);
    g_free(buf);

    temp = g_strdup_printf("%s/%s", g_get_tmp_dir(), tmp_name);
    src = g_file_new_for_uri(src_name);
    dst = g_file_new_for_path(temp);
    if (!g_file_copy(src,
                dst,
                G_FILE_COPY_OVERWRITE,
                NULL,
                NULL,
                NULL,
                &error)) {
                DOMINO_SHOW_ERROR("%s", error->message);
                g_error_free (error);
    }
    else {
#ifdef _WIN32
        gchar *cmdline;
        gchar *localfile;
        gsize br, bw;

        localfile = g_locale_from_utf8(temp, strlen(temp), &br, &bw, &error);
        cmdline = g_strdup_printf("%s %s", pdf_reader, localfile);
        g_free(localfile);
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        char *argv0 = NULL;
      //"p:\\windows\\system32\\calc.exe";

        memset (&si, 0, sizeof (si));
        si.cb = sizeof (&si);
        memset (&pi, 0, sizeof (pi));

        if (!CreateProcess (argv0, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            DOMINO_SHOW_ERROR("Не удалось запустить программу: %s", g_win32_error_message (GetLastError ()));

        g_free(cmdline);
        CloseHandle (pi.hThread);
#else
        gchar *argv[3] = {pdf_reader, temp, NULL};
        if (!g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
            DOMINO_SHOW_ERROR("Не удалось запустить программу: %s", error->message);
            g_error_free (error);
        }
#endif
    }
}
void  do_rz_edit_ck_link(DoRzEdit *edit)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (edit);

	if (priv->rz->link.type == RZ_INFO_LINK_FAIL)
	    do_rz_edit_clicked_link(GTK_BUTTON(priv->link_button), edit);
}
static GObject *do_rz_edit_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	DoRzEdit *rz_edit;
	DoRzEditPrivate *priv;
	GtkWidget *vbox;
    GtkWidget *l;
    GtkWidget *t;
    GtkWidget *w;

	int row = 0;
	object = G_OBJECT_CLASS (do_rz_edit_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_RZ_EDIT_GET_PRIVATE(object);
	vbox   = GTK_WIDGET(object);
	rz_edit = DO_RZ_EDIT(object);

	gtk_box_set_spacing (GTK_BOX (vbox), 2);

    g_object_set(object, "orientation", GTK_ORIENTATION_VERTICAL, NULL);

	switch (priv->action) {
	    case DO_DIALOG_ACTION_DELETE:
            priv->flags |= DO_RZ_EDIT_FLAGS_READ_ONLY;
            break;
	    case DO_DIALOG_ACTION_EDIT:
            break;
	    case DO_DIALOG_ACTION_INSERT:
            break;
	    default:
            priv->flags |= DO_RZ_EDIT_FLAGS_READ_ONLY;
            break;
	}

    t = hig_workarea_create();
    //gtk_table_set_homogeneous(GTK_TABLE(t), TRUE);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, TRUE, 0);

    hig_workarea_add_section_title(t, &row, "Предельная отпускная цена ");

	l = add_field(rz_edit, MNN_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "МНН:", l, NULL);

	l = add_field(rz_edit, TRADE_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Торговое наименование:", l, NULL);

	l = add_field(rz_edit, PACK_FORM_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Форма выпуска:", l,NULL);

	l = add_field(rz_edit, PACK_QUANT_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Количество:", l,NULL);
    g_object_set(l, "regex-mask", "^([0-9]+)$", NULL);

	l = add_field(rz_edit, MANUFACTOR_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Производитель:", l,NULL);

	l = add_field(rz_edit, UNPACK_ENTRY, do_enum_box_new("Логический"));
    hig_workarea_add_row(t, &row, "Цена за первичную упаковку:", l, NULL);

	l = add_field(rz_edit, PRICE_ENTRY, do_entry_calc_new());
    hig_workarea_add_row(t, &row, "Цена:", l,NULL);
	g_object_set(l, "precision", 2, "min-value", 0.01, NULL);

	l = add_field(rz_edit, VALUTA_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Валюта:", l,NULL);

    gchar *reg = NULL;
    gchar *buf;
    GList *val_list, *l1;
    val_list = DOMINO_COMMON_PROPERTIES("Valuta");
    for (l1 = val_list; l1; l1 = l1->next) {
        gchar *value;
        DOMINO_COMMON_GET("Valuta", l1->data, &value, NULL);
        if ( !value ) continue;
        if (!reg)
            reg = g_strdup(value);
        else {
            buf = reg;
            reg = g_strdup_printf("%s|%s", buf, value);
            g_free(buf);
        }
    }
    buf = reg;
    reg = g_strdup_printf("^(%s)$", reg);
    g_object_set(l, "regex-mask", reg, NULL);
    g_free(reg);
    g_free(buf);

	l = add_field(rz_edit, BARCODE_ENTRY, do_entry_new());
    hig_workarea_add_row(t, &row, "Продажный код:", l,NULL);
    g_object_set(l, "regex-mask", "^([0-9]+)$", "max-length", 13, NULL);

    hig_workarea_add_section_title(t, &row, "Дополнительно ");

#if GTK_MAJOR_VERSION < 3
	w = gtk_hbox_new(FALSE, 2);
#else
    w = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
#endif


	l = add_field(rz_edit, ORDER_ENTRY, do_entry_new());
	gtk_box_pack_start (GTK_BOX (w), GTK_WIDGET (l), TRUE, TRUE, 0);
    g_object_set(l, "regex-mask", "^(.+"SEPARATOR" [0-3][0-9]/[0-1][0-9]/[1-2][0-9][0-9][0-9])$", NULL);
//!!

	l = gtk_button_new_with_label("Просмотр...");
	g_signal_connect(l, "clicked", G_CALLBACK(do_rz_edit_clicked_view_order), rz_edit);
	gtk_box_pack_end (GTK_BOX (w), GTK_WIDGET (l), FALSE, TRUE, 0);

    hig_workarea_add_row(t, &row, "Приказ:", w,NULL);

#if GTK_MAJOR_VERSION < 3
	w = gtk_hbox_new(FALSE, 2);
#else
    w = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
#endif
	l = add_field(rz_edit, PRODUCT_ENTRY, do_entry_new());
#if GTK_MAJOR_VERSION == 2
    gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, NULL);
#endif
    g_object_set(l, "editable", FALSE, NULL);
	gtk_box_pack_start (GTK_BOX (w), GTK_WIDGET (l), TRUE, TRUE, 0);

	l = gtk_button_new_with_label("Просмотр...");
	g_signal_connect(l, "clicked", G_CALLBACK(do_rz_edit_clicked_view_product), rz_edit);
	gtk_box_pack_end (GTK_BOX (w), GTK_WIDGET (l), FALSE, TRUE, 0);
    hig_workarea_add_row(t, &row, "Товар:", w,NULL);


    hig_workarea_add_section_title(t, &row, "Ссылка на оригальную строку ");

#if GTK_MAJOR_VERSION < 3
	w = gtk_hbox_new(FALSE, 2);
#else
    w = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
#endif
	l = add_field(rz_edit, LINK_ENTRY, do_entry_new());
	gtk_box_pack_start (GTK_BOX (w), GTK_WIDGET (l), TRUE, TRUE, 0);
#if GTK_MAJOR_VERSION == 2
    gtk_entry_set_icon_from_stock(GTK_ENTRY(l), GTK_ENTRY_ICON_SECONDARY, NULL);
#endif
	l = priv->link_button = gtk_button_new_with_label("Заполнить...");
	g_signal_connect(l, "clicked", G_CALLBACK(do_rz_edit_clicked_link), rz_edit);
	gtk_box_pack_end (GTK_BOX (w), GTK_WIDGET (l), FALSE, TRUE, 0);
    hig_workarea_add_row(t, &row, "Штрихкод:", w,NULL);

    hig_workarea_finish( t, &row );

#if GTK_MAJOR_VERSION < 3
	t = gtk_hbox_new(FALSE, 2);
#else
    t = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
#endif
    gtk_container_set_border_width( GTK_CONTAINER( t ), GUI_PAD_BIG );
	gtk_box_pack_end (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, TRUE, 0);

	l = gtk_button_new_with_label("Закрыть");
	g_signal_connect(l, "clicked", G_CALLBACK(do_rz_edit_clicked_close), rz_edit);
	gtk_box_pack_end (GTK_BOX (t), GTK_WIDGET (l), FALSE, TRUE, 0);

	l = gtk_button_new_with_label("Сохранить");
	g_signal_connect(l, "clicked", G_CALLBACK(do_rz_edit_clicked_save), rz_edit);
	gtk_box_pack_end (GTK_BOX (t), GTK_WIDGET (l), FALSE, TRUE, 0);

    do_rz_edit_fill_entry(rz_edit);

	//g_signal_connect(object, "show", G_CALLBACK(do_rz_edit_show), rz_edit);

	gtk_widget_show_all (vbox);

	return object;
}

static void do_rz_edit_finalize (GObject *object)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (object);
	if (priv->context)
        g_object_unref(priv->context);
	if (priv->alias_name)
		g_free(priv->alias_name);
    if (priv->rz)
        do_rz_info_free(priv->rz);
	G_OBJECT_CLASS (do_rz_edit_parent_class)->finalize (object);
}
static void do_rz_edit_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_FLAGS:
			priv->flags = g_value_get_flags (value);
			break;
        case PROP_ACTION:
            priv->action = g_value_get_enum (value);
            break;
        case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string(value);
            break;
        case PROP_VIEW:
            priv->view = g_value_get_pointer (value);
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
static void do_rz_edit_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (object);
    switch (prop_id)
    {
		case PROP_FLAGS:
            g_value_set_flags(value, priv->flags);
			break;
        case PROP_ACTION:
            g_value_set_enum (value, priv->action);
            break;
        case PROP_VIEW:
            g_value_set_pointer (value, priv->view);
            break;
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
static void do_rz_edit_class_init (DoRzEditClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_rz_edit_constructor;
	object_class->finalize = do_rz_edit_finalize;
	object_class->get_property = do_rz_edit_get_property;
	object_class->set_property = do_rz_edit_set_property;

	g_type_class_add_private (object_class, sizeof (DoRzEditPrivate));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_RZ_EDIT_FLAGS,
				     DO_RZ_EDIT_FLAGS_DEFAULT,
				     G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (object_class,
				   PROP_ACTION,
				   g_param_spec_enum ("action",
 						      "Действие редактора",
 						      "Действие редактира",
 						      DO_TYPE_DIALOG_ACTION,
 						      DO_DIALOG_ACTION_NONE,
 						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
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
    g_object_class_install_property (object_class,
				   PROP_VIEW,
				   g_param_spec_pointer ("view",
							"Обьект просмотра",
							"Обьект просмотра",
							G_PARAM_READWRITE));
}

GtkWidget *do_rz_edit_new (GtkWidget *parent, const gchar *alias_name, DoRzEditFlags flags, DoDialogAction action, DoContext *context)
{
	return g_object_new (DO_TYPE_RZ_EDIT,
			     "flags", flags,
			     "transient-for", parent,
			     "action", action,
			     "context", context,
			     "alias-name", alias_name,
			     (gpointer) NULL);
}
static GtkWidget *add_field(DoRzEdit *dialog, gint id, GtkWidget *widget)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (dialog);
	g_assert (id < N_ENTRIES);
	priv->widget[id] = widget;
	if (GTK_IS_ENTRY(widget) && !GTK_IS_LABEL(widget)) {
        g_signal_connect(widget, "activate", G_CALLBACK(entry_activate), dialog);
        priv->entry[id] = widget;
	}
    else {
        if (GTK_IS_BUTTON(widget)) {
            g_signal_connect(widget, "activate", G_CALLBACK(entry_activate), dialog);
        }
        else {
            if (GTK_IS_COMBO_BOX(widget)) {
                priv->entry[id] = gtk_bin_get_child (GTK_BIN (widget));
                if (priv->entry[id])
                    g_signal_connect(priv->entry[id], "activate", G_CALLBACK(entry_activate), dialog);
            }
        }
    }
	if ((priv->flags & DO_RZ_EDIT_FLAGS_READ_ONLY) == DO_RZ_EDIT_FLAGS_READ_ONLY &&
         priv->entry[id])
         g_object_set(priv->entry[id], "editable", FALSE, NULL);

    return widget;
}
static void entry_activate(GtkWidget *entry, DoRzEdit *dialog)
{
    do_rz_edit_save(dialog);
}
static gboolean do_rz_edit_get_product(do_alias_t *alias, const char *bar_code, product_rec_t *product)
{
    barcode_key0_t barcode_key0;
    barcode_rec_t barcode;
    product_key0_t product_key0;

    char *ch = (char*)bar_code;
    do_text_set(alias, barcode_key0.barcode, ch);
    switch (do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL)) {
        case DO_OK:
            product_key0.base_parcel = 0;
            do_cpy(product_key0.code, barcode.data.code);
            return do_product_get0(alias, product, &product_key0, DO_GET_EQUAL) == DO_OK;
        case DO_KEY_NOT_FOUND:
            break;
        default:
            return FALSE;
    }
    return FALSE;
}

static gboolean do_rz_edit_fill_entry(DoRzEdit *dialog)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (dialog);
    if (do_context_get_kind(priv->context) != DO_CONTEXT_KIND_LINE)
        return FALSE;

    do_alias_t *alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_DOCUMENT_ORDER, DO_DB_BARCODE, DO_DB_END)) {
        do_alias_free(alias);
        return FALSE;
    }

    gchar *dtype;
    gchar *sklad;
    gchar *document;
    gint line;
    rz_info_t *rz;
    product_rec_t product;
    struct tm tm;

    g_object_get(priv->context, "dtype-code", &dtype, "sklad-code", &sklad, "document-number", &document, "line-number", &line, NULL);

    g_return_val_if_fail ( dtype != NULL &&
                           document != NULL &&
                           sklad != NULL &&
                           line != 0
                           , FALSE );
    gchar *value;
    DoEnumValue *enum_;

    rz = do_rz_info_new(alias, dtype, sklad, document, line);

    if (!rz) {
        DOMINO_SHOW_ERROR("Ошибка чтения БД");
        do_alias_free(alias);
        return FALSE;
    }
    if (rz->link.type != RZ_INFO_LINK_NO)
        priv->flags |= DO_RZ_EDIT_FLAGS_LINK_ONLY;

    gtk_entry_set_text(GTK_ENTRY(priv->widget[MNN_ENTRY]), rz->mnn);
    gtk_entry_set_text(GTK_ENTRY(priv->widget[TRADE_ENTRY]), rz->trade_name);
    gtk_entry_set_text(GTK_ENTRY(priv->widget[PACK_FORM_ENTRY]), rz->pack_form);
    value = g_strdup_printf("%d", rz->pack_quant);
    gtk_entry_set_text(GTK_ENTRY(priv->widget[PACK_QUANT_ENTRY]), value);
    g_free(value);
    value = g_strdup_printf("%.2f", rz->price);
    gtk_entry_set_text(GTK_ENTRY(priv->widget[PRICE_ENTRY]), value);
    g_free(value);
    gtk_entry_set_text(GTK_ENTRY(priv->widget[VALUTA_ENTRY]), do_get_valuta_name(rz->valuta));
    gtk_entry_set_text(GTK_ENTRY(priv->widget[MANUFACTOR_ENTRY]), rz->manufactor);
    if (priv->action != DO_DIALOG_ACTION_INSERT)
        gtk_entry_set_text(GTK_ENTRY(priv->widget[BARCODE_ENTRY]), rz->barcode);
    enum_ = do_enum_value_new();
    do_enum_value_set_boolean(enum_, rz->unpack);
    g_object_set(priv->widget[UNPACK_ENTRY], "value", enum_, NULL);
    g_object_unref(enum_);

    tm = *localtime(&rz->order.time);
    value = g_strdup_printf("%s "SEPARATOR" %s", rz->order.number, do_datetoa_c(&tm));
    gtk_entry_set_text(GTK_ENTRY(priv->widget[ORDER_ENTRY]), value);
    g_free(value);

    if (priv->flags & DO_RZ_EDIT_FLAGS_LINK_ONLY && priv->action == DO_DIALOG_ACTION_INSERT)
        rz->link.type = RZ_INFO_LINK_OK;

    if (rz->link.type != RZ_INFO_LINK_NO)
        gtk_entry_set_text(GTK_ENTRY(priv->widget[LINK_ENTRY]), rz->link.barcode);

#if GTK_MAJOR_VERSION > 2
    switch (rz->link.type) {
        case RZ_INFO_LINK_FAIL:
            gtk_entry_set_icon_from_icon_name(GTK_ENTRY(priv->widget[LINK_ENTRY]),
               GTK_ENTRY_ICON_PRIMARY, "network-error-symbolic");
            break;
        case RZ_INFO_LINK_OK:
            gtk_entry_set_icon_from_icon_name(GTK_ENTRY(priv->widget[LINK_ENTRY]),
               GTK_ENTRY_ICON_PRIMARY, "network-transmit-receive-symbolic");
            break;
        default:
            gtk_entry_set_icon_from_icon_name(GTK_ENTRY(priv->widget[LINK_ENTRY]),
               GTK_ENTRY_ICON_PRIMARY, NULL);
    };
#else
    switch (rz->link.type) {
        case RZ_INFO_LINK_FAIL:
            gtk_entry_set_icon_from_stock(GTK_ENTRY(priv->widget[LINK_ENTRY]),
               GTK_ENTRY_ICON_PRIMARY, LINK_DISCONNECT);
            break;
        case RZ_INFO_LINK_OK:
            gtk_entry_set_icon_from_stock(GTK_ENTRY(priv->widget[LINK_ENTRY]),
               GTK_ENTRY_ICON_PRIMARY, LINK_CONNECT);
            break;
        default:
            gtk_entry_set_icon_from_stock(GTK_ENTRY(priv->widget[LINK_ENTRY]),
               GTK_ENTRY_ICON_PRIMARY, NULL);
    };
#endif

    if (do_rz_edit_get_product(alias, rz->barcode, &product)) {
        gchar *code, *name;
        code = do_text(alias, product.data.code);
        name = do_text(alias, product.data.name);
        value = g_strdup_printf("%s %s", code, name);
        gtk_entry_set_text(GTK_ENTRY(priv->widget[PRODUCT_ENTRY]), value);
        g_free(value);
        g_free(code);
        g_free(name);
    }

    if (priv->flags & DO_RZ_EDIT_FLAGS_LINK_ONLY) {
        int i;
        for (i = 0; i < N_ENTRIES; i++)
            if (i != BARCODE_ENTRY && GTK_IS_ENTRY(priv->widget[i]))
                g_object_set(priv->widget[i], "editable", FALSE, NULL);
        if (priv->action != DO_DIALOG_ACTION_INSERT)
            g_object_set(priv->widget[BARCODE_ENTRY], "editable", FALSE, NULL);
        gtk_widget_grab_focus (priv->widget[BARCODE_ENTRY]);
    }
    gtk_widget_set_sensitive(priv->link_button, !(priv->flags & DO_RZ_EDIT_FLAGS_READ_ONLY) &&
                                           priv->flags & DO_RZ_EDIT_FLAGS_LINK_ONLY);
    gtk_widget_set_sensitive(priv->widget[LINK_ENTRY], !(priv->flags & DO_RZ_EDIT_FLAGS_READ_ONLY) &&
                                           priv->flags & DO_RZ_EDIT_FLAGS_LINK_ONLY);
    if ( !( priv->flags & DO_RZ_EDIT_FLAGS_LINK_ONLY ) &&
          priv->action != DO_DIALOG_ACTION_INSERT ) {
        g_object_set(priv->widget[BARCODE_ENTRY], "editable", FALSE, NULL);
    }
    priv->rz = rz;
    do_alias_free(alias);
    return TRUE;
}
static gboolean do_rz_load_insert_document(do_alias_t *alias, document_order_rec_t *document_order)
{
    document_rec_t document;
    document_key0_t document_key0;

    do_cpy(document_key0.dtype, document_order->data.dtype);
    do_cpy(document_key0.sklad, document_order->data.sklad);
    do_cpy(document_key0.document, document_order->data.document);

    switch (do_document_key0(alias, &document_key0, DO_GET_EQUAL)) {
        case DO_OK:
            return TRUE;
        case DO_KEY_NOT_FOUND: {
            memset(&document.data, 0, sizeof(document.data));
            document.size = sizeof(document.data) - sizeof(document.data.params);
            do_cpy(document.data.dtype, document_key0.dtype);
            do_text_set(alias, document.data.dtype1, "1");
            do_cpy(document.data.sklad, document_key0.sklad);
            do_cpy(document.data.document, document_key0.document);
            do_text_set_empty(document.data.name);
            time_t now = time(NULL);
            struct tm tm = *localtime(&now);
            do_date_set(&document.data.date, tm);
            do_time_set(&document.data.time, tm);
            do_text_set(alias, document.data.p_g_code, "00");
            document.data.p_code = 1;
            do_text_set_empty(document.data.p_doc);
            do_text_set_empty(document.data.party);
            if (do_document_insert(alias, &document) == DO_ERROR)
                return FALSE;
            return TRUE;
        }
        default :
            return FALSE;
    }
}
static gboolean do_rz_edit_send_update(DoRzEdit *dialog, do_alias_t *alias, GList *list)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (dialog);
    GList *l;
    DoContext *context;

    if (priv->view) {
        context = do_context_new_with_attributes(DO_CONTEXT_KIND_LINE, NULL);
        for (l = list; l; l = l->next) {
            do_context_set_document_order_key0(context, alias, l->data);
            do_view_update_context(priv->view, context);
            g_free(l->data);
        }
        g_object_unref(context);
    }
    g_list_free(list);
    return TRUE;

}
static gboolean do_rz_edit_delete(DoRzEdit *dialog, do_alias_t *alias, GList **do_list_update)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (dialog);
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;

    if (!priv->rz)
        return TRUE;
    do_rz_info_key_to_document_order_key0(priv->rz, alias, &document_order_key0);
    switch (do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_EQUAL)) {
        case DO_KEY_NOT_FOUND:
            return TRUE;
        case DO_ERROR:
            return FALSE;
        default: {
            gboolean retval;
            do_list_t *list;
            int i;
            retval = do_document_order_delete(alias) == DO_OK;
            if (retval && priv->rz->link.type == RZ_INFO_LINK_NO) {
                list = do_rz_info_util_mark_link(alias, priv->rz->barcode);
                if (list == NULL)
                    return FALSE;
                for (i = 0; i < list->count; i++)
                    *do_list_update = g_list_append(*do_list_update, list->list[i]);
                do_list_free(list);
            }
            if (retval)
                retval = do_protocol_document_order_add_key(alias, &document_order_key0,
                                                       DO_PROTOCOL_ACTION_DELETE_LINE, NULL);
            return retval;
        }
    }
}
static gboolean do_rz_edit_convert_order(const gchar *text, struct tm *tm, char **number)
{
    gchar *ch;
    gchar *value;
    gboolean retval = TRUE;

    value = g_strdup(text);
    ch = strstr(value, SEPARATOR);
    if (!ch)
        retval = FALSE;
    else {
        *(ch - 1) = '\0';
        ch+=strlen(SEPARATOR) + 1;
        retval = do_atodate(ch, tm);
    }
    if (retval)
        *number = g_strdup(value);
    g_free(value);
    return retval;
}
static void do_rz_edit_fill_rz (DoRzEdit *dialog, rz_info_t *rz)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (dialog);
	struct tm tm;
	GIConv conv;

	conv = do_utf8_make_compatibility_cp866_init();

    rz->mnn = do_utf8_make_compatibility_cp866(conv,
        gtk_entry_get_text(GTK_ENTRY(priv->widget[MNN_ENTRY])));
    rz->trade_name = do_utf8_make_compatibility_cp866(conv,
       gtk_entry_get_text(GTK_ENTRY(priv->widget[TRADE_ENTRY])));
    rz->pack_form =  do_utf8_make_compatibility_cp866(conv,
        gtk_entry_get_text(GTK_ENTRY(priv->widget[PACK_FORM_ENTRY])));
    rz->pack_quant = atoi(gtk_entry_get_text(GTK_ENTRY(priv->widget[PACK_QUANT_ENTRY])));
    rz->price = atof(gtk_entry_get_text(GTK_ENTRY(priv->widget[PRICE_ENTRY])));
    DoEnumValue *enum_;
    g_object_get(priv->widget[UNPACK_ENTRY], "value", &enum_, NULL);
    rz->unpack = do_enum_value_get_boolean(enum_);
    g_object_unref(enum_);
    rz->valuta = do_get_valuta_code(
            gtk_entry_get_text(GTK_ENTRY(priv->widget[VALUTA_ENTRY])));
    rz->manufactor = do_utf8_make_compatibility_cp866(conv,
        gtk_entry_get_text(GTK_ENTRY(priv->widget[MANUFACTOR_ENTRY])));
    rz->barcode = do_utf8_make_compatibility_cp866(conv,
        gtk_entry_get_text(GTK_ENTRY(priv->widget[BARCODE_ENTRY])));

    rz->link.barcode = do_utf8_make_compatibility_cp866(conv,
        gtk_entry_get_text(GTK_ENTRY(priv->widget[LINK_ENTRY])));

#if GTK_MAJOR_VERSION > 2
    const gchar *icon_name;
    icon_name = gtk_entry_get_icon_name(GTK_ENTRY(priv->widget[LINK_ENTRY]), GTK_ENTRY_ICON_PRIMARY);

    if (!icon_name || *icon_name == '\0')
        rz->link.type = RZ_INFO_LINK_NO;
    else
    if (!strcmp(icon_name, "network-transmit-receive-symbolic"))
        rz->link.type = RZ_INFO_LINK_OK;
    else
    if (!strcmp(icon_name, "network-error-symbolic"))
        rz->link.type = RZ_INFO_LINK_FAIL;
    else
        rz->link.type = RZ_INFO_LINK_NO;
#else
	const gchar *icon;
    icon = gtk_entry_get_icon_stock(GTK_ENTRY(priv->widget[LINK_ENTRY]), GTK_ENTRY_ICON_PRIMARY);

    if (!icon || *icon == '\0')
        rz->link.type = RZ_INFO_LINK_NO;
    else
    if (!strcmp(icon, LINK_CONNECT))
        rz->link.type = RZ_INFO_LINK_OK;
    else
    if (!strcmp(icon, LINK_DISCONNECT))
        rz->link.type = RZ_INFO_LINK_FAIL;
    else
        rz->link.type = RZ_INFO_LINK_NO;
#endif
    if (do_rz_edit_convert_order(gtk_entry_get_text(GTK_ENTRY(priv->widget[ORDER_ENTRY])),
                                   &tm, &rz->order.number))
        rz->order.time = mktime(&tm);
    else {
        rz->order.time = priv->rz->order.time;
        rz->order.number = g_strdup(priv->rz->order.number);
    }
    rz->key.dtype = strdup(priv->rz->key.dtype);
    rz->key.sklad = strdup(priv->rz->key.sklad);
    rz->key.document = strdup(priv->rz->key.document);
    rz->key.line = priv->rz->key.line;
	do_utf8_make_compatibility_cp866_close(conv);

}
static gboolean do_rz_edit_insert(DoRzEdit *dialog, do_alias_t *alias)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (dialog);
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    rz_info_t rz;
    int indx;

    do_rz_edit_fill_rz(dialog, &rz);
    for (indx = 1; indx < 100; indx++) {
        do_rz_info_barcode_to_document_order_key0(&rz, alias, &document_order_key0);
        document_order_key0.line += indx;
        switch (do_document_order_key0(alias, &document_order_key0, DO_GET_EQUAL)) {
            case DO_KEY_NOT_FOUND: {
                do_rz_info_barcode_to_document_order(&rz, alias, &document_order);
                document_order.data.line += indx;
                g_free(priv->rz->key.dtype);
                g_free(priv->rz->key.sklad);
                g_free(priv->rz->key.document);
                priv->rz->key.dtype = do_text(alias, document_order.data.dtype);
                priv->rz->key.sklad = do_text(alias, document_order.data.sklad);
                priv->rz->key.document = do_text(alias, document_order.data.document);
                priv->rz->key.line = document_order.data.line;
                if (do_document_order_insert(alias, &document_order) == DO_OK ) {
                    do_context_set_document_order_key0(priv->context, alias, &document_order_key0);
                    return
                    do_rz_load_insert_document(alias, &document_order) &&
                    do_protocol_document_order_add_key(alias, &document_order_key0, DO_PROTOCOL_ACTION_INSERT_LINE, NULL);
                }
                else
                    return FALSE;
            }
            case DO_ERROR:
                return FALSE;
            default:
                break;
        }
    }
    return FALSE;
}
static gboolean do_rz_edit_update(DoRzEdit *dialog, do_alias_t *alias, GList **do_list_update)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (dialog);
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    rz_info_t rz;

    do_rz_edit_fill_rz(dialog, &rz);
    do_rz_info_key_to_document_order_key0(priv->rz, alias, &document_order_key0);
    switch (do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_EQUAL)) {
        case DO_KEY_NOT_FOUND:
            return do_rz_edit_insert(dialog, alias);
        case DO_ERROR:
            return FALSE;
        default: {
            gboolean retval;
            int i;
            do_list_t *list;

            if (strcmp(rz.barcode, priv->rz->barcode) && priv->rz->link.type == RZ_INFO_LINK_NO) {
                list = do_rz_info_util_mark_link(alias, priv->rz->barcode);
                if (list == NULL)
                    return FALSE;
                for (i = 0; i < list->count; i++)
                    *do_list_update = g_list_append(*do_list_update, list->list[i]);
                do_list_free(list);
            }
            do_rz_info_key_to_document_order(&rz, alias, &document_order);
            retval = do_document_order_update(alias, &document_order) == DO_OK;
            if (retval && rz.link.type == RZ_INFO_LINK_NO) {
                list = do_rz_info_util_mark_link(alias, rz.barcode);
                if (list == NULL)
                    return FALSE;
                for (i = 0; i < list->count; i++)
                    *do_list_update = g_list_append(*do_list_update, list->list[i]);
                do_list_free(list);
            }
            if (retval)
                retval = do_rz_load_insert_document(alias, &document_order);

            if (retval)
                retval = do_protocol_document_order_add_key(alias, &document_order_key0, DO_PROTOCOL_ACTION_CHANGE_LINE, NULL);
            return retval;
        }
    }
}

static gboolean do_rz_edit_accept_record(DoRzEdit *dialog)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (dialog);
    gboolean retval = TRUE;
    gboolean skeep = FALSE;

    if (priv->action != DO_DIALOG_ACTION_NONE) {
        gint roles;
        if (priv->flags & DO_RZ_EDIT_FLAGS_LINK_ONLY)
            roles = DOMINO_ROLE_SUPERVISOR | DOMINO_ROLE_MAIN_MANAGER;
        else
            roles = DOMINO_ROLE_SUPERVISOR;

        if (!domino_check_access(roles))
            return FALSE;

        do_alias_t *alias = NULL;
        GList *list = NULL, *l, *ul = NULL;
        list = DOMINO_COMMON_PROPERTIES("MainAlias");
        list = g_list_append(list, priv->alias_name);

        for (l = list; l && !skeep; l = l->next) {
            alias = domino_alias_new(l->data);
            if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_PROTOCOL, DO_DB_DOCUMENT, DO_DB_BARCODE, DO_DB_END)) {
                if (alias)
                    do_alias_free(alias);
                g_list_free(list);
                DOMINO_SHOW_ERROR("Ошибка открытия БД");
                return FALSE;
            }
            if (do_alias_tran_begin(alias)) {
                switch (priv->action) {
                    case DO_DIALOG_ACTION_DELETE: {
                        gint res;
                        DOMINO_SHOW_QUESTION(res, GTK_BUTTONS_YES_NO, "Удалить запись?");
                        if (res == GTK_RESPONSE_NO) {
                            skeep = TRUE;
                        }
                        else
                            retval = do_rz_edit_delete(dialog, alias, &ul);
                        break;
                    }
                    case DO_DIALOG_ACTION_EDIT:
                        if (l == list && priv->view) {
                            do_view_update_context(priv->view, priv->context);
                        }
                        retval = do_rz_edit_update(dialog, alias, &ul);
                        break;
                    case DO_DIALOG_ACTION_INSERT:
                        retval = do_rz_edit_insert(dialog, alias);
                        break;
                    default:
                        retval = TRUE;
                }
                if (retval && !skeep)
                    retval = do_alias_tran_end(alias);
                else
                    do_alias_tran_abort(alias);

                if (retval && !l->next)
                    do_rz_edit_send_update(dialog, alias, ul);

            }
            else
                retval = FALSE;

            do_alias_free(alias);
            if (!retval) {
                DOMINO_SHOW_ERROR("Ошибка записи БД");
                break;
            }
        }
        g_list_free(list);

        if (priv->view) {
            do_view_update_context(priv->view, priv->context);
            if (priv->action != DO_DIALOG_ACTION_DELETE)
                do_view_set_cursor_to_context(priv->view, priv->context);
        }
    }
    if (retval) {
        gtk_widget_destroy(GTK_WIDGET(dialog));
        if (priv->view && priv->action != DO_DIALOG_ACTION_DELETE)
            do_view_do_grab_focus(priv->view);
    }
    return retval;
}
gboolean do_rz_edit_accept(DoRzEdit *dialog)
{
    return do_rz_edit_accept_record(dialog);
};
G_CONST_RETURN gchar *do_rz_edit_get_title(DoRzEdit *edit)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (edit);
	return gtk_entry_get_text(GTK_ENTRY(priv->entry[TRADE_ENTRY]));
}
void do_rz_edit_grab_focus(DoRzEdit *edit)
{
	DoRzEditPrivate *priv = DO_RZ_EDIT_GET_PRIVATE (edit);
	switch (priv->action) {
	    case DO_DIALOG_ACTION_INSERT:
            if (priv->flags & DO_RZ_EDIT_FLAGS_LINK_ONLY) {
                gtk_widget_grab_focus (priv->widget[BARCODE_ENTRY]);
                break;
            }
	    default:
            gtk_widget_grab_focus(priv->widget[MNN_ENTRY]);
	}
}
