

#include <string.h>
#include <gtk/gtk.h>

#include "do_rate_dialog.h"
#include "do_icons.h"
#include "do_utilx.h"
#include "do_entry.h"
#include "do_entry_calc.h"
#include "do_entry_date_box.h"
#include "do_filter.h"
#include "do_validate.h"
#include "do_check_dialog.h"
#include "domino.h"
#include "hig.h"
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

#define DO_RATE_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_RATE_DIALOG, DoRateDialogPrivate))
#define DO_TYPE_RATE_DIALOG_FLAGS do_rate_dialog_flags_get_type()


static gchar *OBJECT_ROOT_PATH = "RateDialog";
#define DEFAULT_WINDOW_POSITION "center-on-parent"
#define DEFAULT_WINDOW_SIZE ""

GType do_rate_dialog_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_rate_dialog_flags_values[] = {
         { DO_RATE_DIALOG_FLAGS_READ_ONLY, "DO_RATE_DIALOG_FLAGS_READ_ONLY", "read-only" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoRateDialogFlags", _do_rate_dialog_flags_values);
    }
    return type;
}

typedef struct {
    gint code;
    GtkWidget *entry;
} DoRateDialogValuta;

struct _DoRateDialogPrivate
{
	DoRateDialogFlags flags;


	GtkWidget *entry_time;

	DoView     *view;
	DoDialogAction action;
	DoContext  *context;

	gchar      *alias_name;
    time_t      time;
    GList      *valuta_list;

};


G_DEFINE_TYPE (DoRateDialog, do_rate_dialog, GTK_TYPE_DIALOG)

static GtkWidget *add_field(DoRateDialog *dialog, gint id, GtkWidget *widget);

static void entry_activate(GtkWidget *entry, DoRateDialog *dialog);
static gboolean do_rate_dialog_fill_entry(DoRateDialog *dialog);

static void do_rate_dialog_init(DoRateDialog *dialog)
{

}

static GObject *do_rate_dialog_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	GtkWindow *window;
	GtkDialog *dialog;
	DoRateDialog *rate_dialog;
	DoRateDialogPrivate *priv;
	GtkWidget *vbox;
    GtkWidget *l;
    GtkWidget *t;
    GtkAccelGroup *accel_group;
    GList  *list, *l1;
    int i;

	int row = 0;
	object = G_OBJECT_CLASS (do_rate_dialog_parent_class)->constructor(type, n_construct_properties, construct_params);

    priv = DO_RATE_DIALOG_GET_PRIVATE(object);
	window = GTK_WINDOW (object);
	dialog = GTK_DIALOG (object);
	vbox   = dialog->vbox;
	rate_dialog = DO_RATE_DIALOG(object);
//!!
    gtk_window_set_type_hint (GTK_WINDOW (object),
			    GDK_WINDOW_TYPE_HINT_NORMAL);

    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);


	gtk_box_set_spacing (GTK_BOX (dialog->vbox), 2); /* Message has 24, we want 12 = 2 + 2 * 5 */

	gtk_window_set_icon_name (window, DO_STOCK_DOMINO_APPS);

	switch (priv->action) {
	    case DO_DIALOG_ACTION_DELETE:
            gtk_window_set_title (window, "Удалить запись");
            priv->flags |= DO_RATE_DIALOG_FLAGS_READ_ONLY;
            break;
	    case DO_DIALOG_ACTION_EDIT:
            gtk_window_set_title (window, "Изменение");
            break;
	    case DO_DIALOG_ACTION_INSERT:
            gtk_window_set_title (window, "Добавление записи");
            break;
	    default:
            gtk_window_set_title (window, "Запись о курсе валют");
            priv->flags |= DO_RATE_DIALOG_FLAGS_READ_ONLY;
            break;
	}

//    g_signal_connect (window, "configure-event "",
  //                    G_CALLBACK (do_window_configure_event), (gpointer)OBJECT_ROOT_PATH);

    gtk_dialog_set_has_separator(dialog, FALSE);

    t = hig_workarea_create();
    //gtk_table_set_homogeneous(GTK_TABLE(t), TRUE);

	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, TRUE, 0);

    hig_workarea_add_section_title(t, &row, "Курс валют ");

	l = add_field(rate_dialog, 0, do_entry_date_box_new(0));
    hig_workarea_add_row(t, &row, "Дата:", l, NULL);

    list = do_utilx_get_valuta_info_list();
    priv->valuta_list = NULL;

    for (l1 = list, i = 1; l1; l1 = l1->next) {
        DoRateDialogValuta *valuta;
        DoValutaInfo *info;
        gchar *buf;
        info = l1->data;
        if (info->code != 1) {
            valuta = g_malloc(sizeof(DoRateDialogValuta));
            valuta->code = info->code;
            priv->valuta_list = g_list_append(priv->valuta_list, valuta);
            l = add_field(rate_dialog, i, do_entry_calc_new());
            buf = g_strdup_printf("%s:", info->name);
            hig_workarea_add_row(t, &row, buf, l, NULL);
            g_free(buf);
            i++;
        }
    }

    hig_workarea_finish( t, &row );

	gtk_dialog_add_button (dialog, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (dialog, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT);
	//gtk_dialog_set_default_response (dialog, GTK_RESPONSE_ACCEPT);

    DOMINO_PROFILE_OBJECT_INIT(GTK_OBJECT(window), OBJECT_ROOT_PATH,
              "window-position", DEFAULT_WINDOW_POSITION,
              "window-size", DEFAULT_WINDOW_SIZE,
               NULL);

    do_rate_dialog_fill_entry(DO_RATE_DIALOG (object));

	gtk_widget_show_all (vbox);

	return object;
}

static void do_rate_dialog_finalize (GObject *object)
{
	DoRateDialogPrivate *priv = DO_RATE_DIALOG_GET_PRIVATE (object);
	if (priv->context)
        g_object_unref(priv->context);
	if (priv->alias_name)
		g_free(priv->alias_name);
    if (priv->valuta_list) {
        g_list_foreach(priv->valuta_list, (GFunc)g_free, NULL);
        g_list_free(priv->valuta_list);
    }
	G_OBJECT_CLASS (do_rate_dialog_parent_class)->finalize (object);
}
static void do_rate_dialog_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoRateDialogPrivate *priv = DO_RATE_DIALOG_GET_PRIVATE (object);

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
static void do_rate_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoRateDialogPrivate *priv = DO_RATE_DIALOG_GET_PRIVATE (object);
    switch (prop_id)
    {
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
static void do_rate_dialog_class_init (DoRateDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_rate_dialog_constructor;
	object_class->finalize = do_rate_dialog_finalize;
	object_class->get_property = do_rate_dialog_get_property;
	object_class->set_property = do_rate_dialog_set_property;

	g_type_class_add_private (object_class, sizeof (DoRateDialogPrivate));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_RATE_DIALOG_FLAGS,
				     DO_RATE_DIALOG_FLAGS_DEFAULT,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (object_class,
				   PROP_ACTION,
				   g_param_spec_enum ("action",
 						      "Действие редактора",
 						      "Действие редактира",
 						      DO_TYPE_DIALOG_ACTION,
 						      DO_DIALOG_ACTION_NONE,
 						      GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
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
 						      GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (object_class,
				   PROP_VIEW,
				   g_param_spec_pointer ("view",
							"Обьект просмотра",
							"Обьект просмотра",
							GTK_PARAM_READWRITE));
}

GtkWidget *do_rate_dialog_new (GtkWidget *parent, const gchar *alias_name, DoRateDialogFlags flags, DoDialogAction action, DoContext *context)
{
	return g_object_new (DO_TYPE_RATE_DIALOG,
			     "flags", flags,
			     "transient-for", parent,
			     "action", action,
			     "context", context,
			     "alias-name", alias_name,
			     (gpointer) NULL);
}
static GtkWidget *add_field(DoRateDialog *dialog, gint id, GtkWidget *widget)
{
	DoRateDialogPrivate *priv = DO_RATE_DIALOG_GET_PRIVATE (dialog);
	DoRateDialogValuta *valuta;
	g_assert (id < g_list_length(priv->valuta_list) + 1);
	if (!id) {
        g_object_get(widget, "entry", &priv->entry_time, NULL);
        g_signal_connect(priv->entry_time, "activate", G_CALLBACK(entry_activate), dialog);

        if ((priv->flags & DO_RATE_DIALOG_FLAGS_READ_ONLY) == DO_RATE_DIALOG_FLAGS_READ_ONLY)
            g_object_set(priv->entry_time, "editable", FALSE, NULL);
	}
    else {
        valuta = g_list_nth_data(priv->valuta_list, id - 1);
        valuta->entry = widget;
        g_signal_connect(widget, "activate", G_CALLBACK(entry_activate), dialog);

        if ((priv->flags & DO_RATE_DIALOG_FLAGS_READ_ONLY) == DO_RATE_DIALOG_FLAGS_READ_ONLY)
             g_object_set(widget, "editable", FALSE, NULL);
    }

    return widget;
}
static void entry_activate(GtkWidget *entry, DoRateDialog *dialog)
{
    gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
}

static gboolean do_rate_dialog_fill_entry(DoRateDialog *dialog)
{
	DoRateDialogPrivate *priv = DO_RATE_DIALOG_GET_PRIVATE (dialog);

    if (priv->action == DO_DIALOG_ACTION_INSERT) {
        if (!priv->context) {
            priv->context = do_context_new();
            do_context_set_kind(priv->context, DO_CONTEXT_KIND_LINE);
        }
        return TRUE;
    }
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
    GList *l;
    DoRateDialogValuta *valuta;

    document_order_key0_t document_order_key0;
    document_order_rec_t  document_order;
    struct tm tm;

    g_object_get(priv->context, "dtype-code", &dtype, "sklad-code", &sklad, "document-number", &document, "line-number", &line, NULL);

    g_return_val_if_fail ( dtype != NULL &&
                           document != NULL &&
                           sklad != NULL &&
                           line != 0
                           , FALSE );
    do_text_set(alias, document_order_key0.dtype, dtype);
    do_text_set(alias, document_order_key0.sklad, sklad);
    do_text_set(alias, document_order_key0.document, document);
    document_order_key0.line = line;
    time_t time;

    if (do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_EQUAL) != DO_OK) {
        do_message_dialog_show(GTK_WIDGET(dialog), GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка чтения БД");
        do_alias_free(alias);
        return FALSE;
    }

    time = do_date(document_order.data.line, &tm);
    //tm = *localtime(&time);

    g_object_set(priv->entry_time, "year", tm.tm_year + 1900, "month", tm.tm_mon + 1, "day", tm.tm_mday, NULL);
    g_object_set(priv->entry_time, "editable", FALSE, NULL);

    for (l = priv->valuta_list; l; l = l->next) {
        valuta = l->data;
        g_object_set(valuta->entry, "value", do_document_order_price(alias, &document_order, valuta->code), NULL);
        if (l == priv->valuta_list)
            gtk_widget_grab_focus (valuta->entry);
    }
    do_alias_free(alias);
    return TRUE;
}

static gboolean do_rate_dialog_delete(DoRateDialog *dialog, do_alias_t *alias)
{
	DoRateDialogPrivate *priv = DO_RATE_DIALOG_GET_PRIVATE (dialog);
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;

    gchar *dtype;
    gchar *sklad;
    gchar *document;
    gint line;
    g_object_get(priv->context, "dtype-code", &dtype, "sklad-code", &sklad, "document-number", &document, "line-number", &line, NULL);

    g_return_val_if_fail ( dtype != NULL &&
                           document != NULL &&
                           sklad != NULL &&
                           line != 0
                           , FALSE );
    do_text_set(alias, document_order_key0.dtype, dtype);
    do_text_set(alias, document_order_key0.sklad, sklad);
    do_text_set(alias, document_order_key0.document, document);
    document_order_key0.line = line;
    switch (do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_EQUAL)) {
        case DO_KEY_NOT_FOUND:
            return TRUE;
        case DO_ERROR:
            return FALSE;
        default:
            if (do_document_order_delete(alias) == DO_OK) {
                return do_protocol_document_order_add_key(alias, &document_order_key0, DO_PROTOCOL_ACTION_DELETE_LINE);
                //return TRUE;
            }
            return FALSE;
    }
}

static gboolean do_rate_dialog_set_order(DoRateDialog *dialog, do_alias_t *alias,
        document_order_rec_t *document_order,
        document_order_key0_t *document_order_key0)
{
	DoRateDialogPrivate *priv = DO_RATE_DIALOG_GET_PRIVATE (dialog);
	gint year, month, day;
	struct tm tm;
	g_object_get(priv->entry_time, "year", &year, "month", &month, "day", &day, NULL);

	do_date_set_ymd(&tm, year, month, day);
	GList *l;

	do_text_set(alias, document_order_key0->dtype, do_param(DO_PARAM_DOCUMENT_TYPE_RATE));
	do_text_set(alias, document_order_key0->sklad, do_param(DO_PARAM_COMMON_SKLAD));
	do_text_set(alias, document_order_key0->document, DO_CONST_DEFAULT_DOCUMENT);
	do_date_set(&document_order_key0->line, tm);
    g_object_set(priv->context,
                     "dtype-code", do_param(DO_PARAM_DOCUMENT_TYPE_RATE),
                     "sklad-code", do_param(DO_PARAM_COMMON_SKLAD),
                     "document-number", DO_CONST_DEFAULT_DOCUMENT,
                     "line-number", document_order_key0->line, NULL);

    document_order->size = sizeof(document_order->data) - sizeof(document_order->data.params);
    memset(&document_order->data, 0, document_order->size);

	do_cpy(document_order->data.dtype, document_order_key0->dtype);
	do_cpy(document_order->data.sklad, document_order_key0->sklad);
	do_cpy(document_order->data.document, document_order_key0->document);
	do_text_set(alias, document_order->data.dtype1, DO_CONST_DEFAULT_DTYPE1);
	document_order->data.line = document_order_key0->line;
	do_document_order_price_set(alias, document_order, 1, 1);
	for (l = priv->valuta_list; l; l = l->next) {
	    DoRateDialogValuta *valuta;
	    valuta = l->data;
	    gdouble value;
	    g_object_get(valuta->entry, "value", &value, NULL);
        do_document_order_price_set(alias, document_order, valuta->code, value);
	}
    return TRUE;

}
static gboolean do_rate_dialog_update(DoRateDialog *dialog, do_alias_t *alias, gint *update_mode);
static gboolean do_rate_dialog_insert(DoRateDialog *dialog, do_alias_t *alias, gint *update_mode)
{
	//DoRateDialogPrivate *priv = DO_RATE_DIALOG_GET_PRIVATE (dialog);
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;

    do_rate_dialog_set_order(dialog, alias, &document_order, &document_order_key0);


    switch (do_document_order_key0(alias, &document_order_key0, DO_GET_EQUAL)) {
        case DO_ERROR:
            return FALSE;
        case DO_OK:
            if (!*update_mode == -1) {
                if (do_message_dialog_show(GTK_WIDGET(dialog), GTK_MESSAGE_ERROR, GTK_BUTTONS_YES_NO, "Запись уже существует. Перезаписать?")
                    == GTK_RESPONSE_YES)
                    *update_mode = 1;
                else
                    *update_mode = 0;
            }
            if (*update_mode)
                return do_rate_dialog_update(dialog, alias, update_mode);

            return TRUE;
        case DO_KEY_NOT_FOUND: {
            if (do_document_order_insert(alias, &document_order) != DO_OK)
                return FALSE;
            return do_protocol_document_order_add_key(alias, &document_order_key0, DO_PROTOCOL_ACTION_INSERT_LINE);
        }
    }
    return FALSE;
}
static gboolean do_rate_dialog_update(DoRateDialog *dialog, do_alias_t *alias, gint *update_mode)
{
    document_order_rec_t document_order, tmp;
    document_order_key0_t document_order_key0;

    do_rate_dialog_set_order(dialog, alias, &document_order, &document_order_key0);

    switch (do_document_order_get0(alias, &tmp, &document_order_key0, DO_GET_EQUAL)) {
        case DO_ERROR:
            return FALSE;
        case DO_OK:
            if (do_document_order_update(alias, &document_order) != DO_OK)
                return FALSE;
            return do_protocol_document_order_add_key(alias, &document_order_key0, DO_PROTOCOL_ACTION_CHANGE_LINE);
        case DO_KEY_NOT_FOUND:
            return do_rate_dialog_insert(dialog, alias, update_mode);
    }
    return FALSE;
}

static gboolean do_rate_dialog_accept_record(DoRateDialog *dialog)
{
	DoRateDialogPrivate *priv = DO_RATE_DIALOG_GET_PRIVATE (dialog);

    if (priv->action == DO_DIALOG_ACTION_NONE)
        return TRUE;

    if (!domino_check_access(GTK_WIDGET(dialog), DOMINO_ROLE_ADMINISTRATOR))
        return FALSE;

    do_alias_t *alias = NULL;
    gboolean retval = TRUE;
    gint update_mode = -1;
    gint delete_mode = -1;

    GList *list, *l;
    list = DOMINO_COMMON_PROPERTIES("MainAlias");
    list = g_list_append(list, priv->alias_name);
    for (l = list; l; l = l->next) {
        alias = domino_alias_new(l->data);
        if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_PROTOCOL, DO_DB_BARCODE, DO_DB_END)) {
            if (alias)
                do_alias_free(alias);
            g_list_free(list);
            do_message_dialog_show(GTK_WIDGET(dialog), GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка открытия БД");
            return FALSE;
        }
        switch (priv->action) {
            case DO_DIALOG_ACTION_DELETE:
                if (delete_mode == -1) {
                    if (do_message_dialog_show(GTK_WIDGET(dialog), GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO, "Удалить запись?") == GTK_RESPONSE_YES)
                       delete_mode = 1;
                    else
                       delete_mode = 0;
                }
                if (delete_mode)
                    retval = do_rate_dialog_delete(dialog, alias);
                else
                    retval = TRUE;
                break;
            case DO_DIALOG_ACTION_EDIT:
                retval = do_rate_dialog_update(dialog, alias, &update_mode);
                break;
            case DO_DIALOG_ACTION_INSERT:
                retval = do_rate_dialog_insert(dialog, alias, &update_mode);
                break;
            default:
                retval = TRUE;
        }
        do_alias_free(alias);
        if (!retval) {
            do_message_dialog_show(GTK_WIDGET(dialog), GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка записи БД");
            break;
        }
    }
    g_list_free(list);
    return retval;
}

gint do_rate_dialog_run(DoRateDialog *dialog)
{
    gint response = 0;
	DoRateDialogPrivate *priv = DO_RATE_DIALOG_GET_PRIVATE (dialog);
	GList *l;
    while (!response) {
        response = gtk_dialog_run(GTK_DIALOG(dialog));
        switch (response) {
            case GTK_RESPONSE_ACCEPT:
                if (!do_validate_valid(DO_VALIDATE(priv->entry_time))) {
                    gtk_widget_grab_focus (priv->entry_time);
                    response = 0;
                    continue;
                }
                for (l = priv->valuta_list; l ; l = l->next) {
                    DoRateDialogValuta *valuta;
                    valuta = l->data;
                    if (!do_validate_valid(DO_VALIDATE(valuta->entry))) {
                        gtk_widget_grab_focus (valuta->entry);
                        response = 0;
                        continue;
                    }
                }
                if (!do_rate_dialog_accept_record(dialog))
                    response = 0;
                break;
        }
    }
    return response;
}
