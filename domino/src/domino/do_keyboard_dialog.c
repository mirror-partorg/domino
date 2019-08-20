

#include <string.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>

#include "do_keyboard_dialog.h"
#include "do_icons.h"
#include "do_utilx.h"
#include "do_entry.h"
#include "do_keyboard_const.h"
#include "do_keyboard_view.h"
#include "hig.h"

#include <dolib.h>
#ifdef ARTIX
#include <artix.h>
#endif
#include <../misc/cash_key_command.h>
#include <domino.h>
#include "do_view.h"
#include "domino.h"


enum
{
	PROP_0,
	PROP_FLAGS,
	PROP_ACTION,
	PROP_CONTEXT,
	PROP_ALIAS,
	PROP_VIEW,
};

#define DO_KEYBOARD_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_KEYBOARD_DIALOG, DoKeyboardDialogPrivate))
#define DO_TYPE_KEYBOARD_DIALOG_FLAGS do_keyboard_dialog_flags_get_type()


#define OBJECT_ROOT_PATH "KeyboardDialog"
#define DEFAULT_WINDOW_POSITION "center-on-parent"

GType do_keyboard_dialog_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_keyboard_dialog_flags_values[] = {
         { DO_KEYBOARD_DIALOG_FLAGS_DISABLE_FUNCTION_KEYS, "DO_KEYBOARD_DIALOG_FLAGS_DISABLE_FUNCTION_KEYS", "disable-function-keys" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoKeyboardDialogFlags", _do_keyboard_dialog_flags_values);
    }
    return type;
}

enum
{
	DESC_ENTRY,
	KEY_A_RADIO,
	KEY_C_RADIO,
	KEY_A_ENTRY,
	KEY_C_ENTRY,
	COMMAND_P_RADIO,
	COMMAND_M_RADIO,
	COMMAND_M_ENTRY,
	COMMAND_M_P_ENTRY,
	LABEL_ENTRY,
	N_ENTRIES
};

#define MAX_SIZE_RADIO 10
struct _DoKeyboardDialogPrivate
{
	DoKeyboardDialogFlags flags;

	GtkWidget *widget[N_ENTRIES];
	GtkWidget *entry[N_ENTRIES];
	GtkWidget *label_html;
	GtkWidget *label_font;
	GtkWidget *label_size;

	gboolean  empty_desc_allowed;

	DoView     *view;
	DoDialogAction action;
	DoContext  *context;
	gchar      *product_code;

	gchar      *label_size_width;
	gchar      *label_size_height;

	gchar      *alias_name;

};


G_DEFINE_TYPE (DoKeyboardDialog, do_keyboard_dialog, GTK_TYPE_DIALOG)

static GtkWidget *add_field(DoKeyboardDialog *dialog, gint id, GtkWidget *widget);

static void entry_activate(GtkWidget *entry, DoKeyboardDialog *dialog);
static void key_symbol_changed(GtkWidget *entry, DoKeyboardDialog *dialog);
static void key_code_changed(GtkWidget *entry, DoKeyboardDialog *dialog);
static void key_group_changed(GtkRadioButton *style, DoKeyboardDialog *dialog);

static void key_size_clicked(GtkButton *button, DoKeyboardDialog *dialog);

static void command_group_changed(GtkRadioButton *style, DoKeyboardDialog *dialog);
static void command_command_changed(GtkWidget *entry, DoKeyboardDialog *dialog);

static gchar *get_html_view_html_string(DoKeyboardDialog *dialog);

static void key_model_group_changed(GtkRadioButton *style, DoKeyboardDialog *dialog);
static void font_set(GtkFontButton *widget, DoKeyboardDialog *dialog);

static gboolean fill_entry(DoKeyboardDialog *dialog);

static void html_view_update(DoKeyboardDialog *dialog);
static void key_size_update(DoKeyboardDialog *dialog);

static gboolean keyboard_document_save(DoKeyboardDialog *dialog);
static gboolean keyboard_document_get(DoKeyboardDialog *dialog, do_alias_t *alias, document_rec_t *document);
static gboolean keyboard_document_update(DoKeyboardDialog *dialog, do_alias_t *alias, document_rec_t *document);

static void send_update_to_updated_view(DoKeyboardDialog *dialog, do_alias_t *alias, document_rec_t *document);

static void do_keyboard_dialog_init(DoKeyboardDialog *dialog)
{
	DoKeyboardDialogPrivate *priv;

	priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
	memset(priv, 0, sizeof(DoKeyboardDialogPrivate));
	priv->label_size_width = g_strdup(label_size_width_default);
	priv->label_size_height = g_strdup(label_size_height_default);

}
static GObject *do_keyboard_dialog_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	GtkWindow *window;
	GtkDialog *dialog;
    GtkListStore *command_list;
	DoKeyboardDialog *keyboard_dialog;
	DoKeyboardDialogPrivate *priv;
    GtkCellRenderer * r;
	GtkWidget *vbox;
    GtkWidget *l;
    GtkWidget *w;
    GtkWidget *t;
    GtkWidget *tb;
    GSList *group;
    GtkTreeIter iter;
    do_string_list_t *list;
    int i;
	int row = 0;

	object = G_OBJECT_CLASS (do_keyboard_dialog_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_KEYBOARD_DIALOG_GET_PRIVATE(object);
	window = GTK_WINDOW (object);
	dialog = GTK_DIALOG (object);
	vbox   = gtk_dialog_get_content_area(dialog);
	keyboard_dialog = DO_KEYBOARD_DIALOG(object);
	gtk_box_set_spacing (GTK_BOX (vbox), 2); /* Message has 24, we want 12 = 2 + 2 * 5 */

	gtk_window_set_icon_name (window, DO_STOCK_DOMINO_APPS);
	gtk_window_set_title (window, "Редактирование клавиши");

    g_signal_connect (window, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), OBJECT_ROOT_PATH);
    t = hig_workarea_create();
	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (t), FALSE, FALSE, 0);

	l = add_field(keyboard_dialog, DESC_ENTRY, do_entry_new());
    hig_workarea_add_section_title_widget(t, &row, l);

    hig_workarea_add_section_title(t, &row, "Клавиша ");
    tb = add_field(keyboard_dialog, KEY_A_RADIO, gtk_radio_button_new_with_mnemonic( NULL, "Символ (название):" ));
    group = gtk_radio_button_get_group( GTK_RADIO_BUTTON( tb ) );
	g_signal_connect (tb, "toggled" , G_CALLBACK(key_group_changed), object);

    l = add_field(keyboard_dialog, KEY_A_ENTRY, do_entry_new());
	g_signal_connect (l, "changed", G_CALLBACK(key_symbol_changed), object);

    hig_workarea_add_row_w( t, &row, tb, l, NULL );

    tb = add_field(keyboard_dialog, KEY_C_RADIO, gtk_radio_button_new_with_mnemonic( group, "Код клавиши:" ));
	g_signal_connect (tb, "toggled" , G_CALLBACK(key_group_changed), object);

    w = add_field(keyboard_dialog, KEY_C_ENTRY,
                  gtk_spin_button_new_with_range(0, cash_key_code_max(), 1));

	g_signal_connect (w, "value-changed" , G_CALLBACK(key_code_changed), object);

    hig_workarea_add_row_w( t, &row, tb, w, NULL );
    key_group_changed(NULL, keyboard_dialog);

    hig_workarea_add_section_title(t, &row, "Действие ");
    tb = add_field(keyboard_dialog, COMMAND_P_RADIO, gtk_radio_button_new_with_mnemonic( NULL, "Выбор товара(ов)" ));
    group = gtk_radio_button_get_group( GTK_RADIO_BUTTON( tb ) );
	g_signal_connect (tb, "toggled" , G_CALLBACK(command_group_changed), object);
	hig_workarea_add_wide_control( t, &row, tb);

    tb = add_field(keyboard_dialog, COMMAND_M_RADIO, gtk_radio_button_new_with_mnemonic(group, "Комманда:" ));
	g_signal_connect (tb, "toggled" , G_CALLBACK(command_group_changed), object);
    command_list = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    list = cash_key_commands();
    for (i = 0; i < list->count; i++) {
        gtk_list_store_append(command_list, &iter);
        gchar *desc = cash_key_command_desc(list->list[i]);
        gtk_list_store_set(command_list, &iter, 0, list->list[i], 1, desc, -1 );
        do_free(desc);
    }
    l = add_field(keyboard_dialog, COMMAND_M_ENTRY, gtk_combo_box_new_with_model_and_entry(GTK_TREE_MODEL(command_list)));
    r = gtk_cell_renderer_text_new( );
    gtk_cell_layout_pack_start( GTK_CELL_LAYOUT( l ), r, TRUE );
    gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT( l ), r, "text", 1, NULL );
    gtk_cell_layout_reorder(GTK_CELL_LAYOUT( l ), r, 0);
	g_signal_connect (l, "changed" , G_CALLBACK(command_command_changed), object);
	gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(l), 0);

    gtk_widget_set_size_request(l, 300u, -1);
    hig_workarea_add_row_w( t, &row, tb, l, NULL );
    l = add_field(keyboard_dialog, COMMAND_M_P_ENTRY, do_entry_new());
    hig_workarea_add_row( t, &row, "Параметры:", l, NULL );

    command_group_changed(NULL, keyboard_dialog);
    g_object_unref (command_list);

    hig_workarea_add_section_title(t, &row, "Отображение на кнопке ");

    w = priv->label_html = webkit_web_view_new();
    gtk_widget_set_size_request(w, 100u, 100u);
    //!!gtk_widget_set_state(w, GTK_STATE_INSENSITIVE);
    tb = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(tb), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(tb), w);

#if GTK_MAJOR_VERSION == 2
    w=gtk_vbox_new(FALSE, GUI_PAD);
#else
    w=gtk_box_new(GTK_ORIENTATION_VERTICAL, GUI_PAD);
#endif
    l = add_field(keyboard_dialog, LABEL_ENTRY, do_entry_new());
	g_signal_connect (l, "changed", G_CALLBACK(key_model_group_changed), object);
    gtk_box_pack_start (GTK_BOX (w), GTK_WIDGET (l), FALSE, FALSE, 0);

    l = priv->label_font = gtk_font_button_new();
    gtk_font_button_set_show_style(GTK_FONT_BUTTON(l), FALSE);
    gtk_font_button_set_font_name(GTK_FONT_BUTTON(l), label_font_default);
	g_signal_connect (l, "font-set" , G_CALLBACK(font_set), object);
    gtk_box_pack_start (GTK_BOX (w), GTK_WIDGET (l), FALSE, FALSE, 0);


    l = priv->label_size =  gtk_button_new();
    gtk_box_pack_start (GTK_BOX (w), GTK_WIDGET (l), FALSE, FALSE, 0);
	g_signal_connect (l, "clicked" , G_CALLBACK(key_size_clicked), object);
	gtk_widget_set_sensitive(l, FALSE);

    hig_workarea_add_row_w( t, &row, tb, w, NULL );

    hig_workarea_finish( t, &row );

    key_size_update(keyboard_dialog);
    html_view_update(keyboard_dialog);

	gtk_dialog_add_button (dialog, "Отмена", GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (dialog, "Ok", GTK_RESPONSE_ACCEPT);
	//gtk_dialog_set_default_response (dialog, GTK_RESPONSE_ACCEPT);

    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(window), OBJECT_ROOT_PATH,
              "window-position", DEFAULT_WINDOW_POSITION,
               NULL);

	gtk_widget_show_all (vbox);


	return object;
}

static void do_keyboard_dialog_finalize (GObject *object)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (object);
	g_free(priv->label_size_width);
	g_free(priv->label_size_height);
	if (priv->product_code)
        g_free(priv->product_code);
	if (priv->context)
        g_object_unref(priv->context);
	if (priv->alias_name)
		g_free(priv->alias_name);
	G_OBJECT_CLASS (do_keyboard_dialog_parent_class)->finalize (object);
}

static void do_keyboard_dialog_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (object);

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
            if (!priv->context) {
                priv->context = do_context_copy(g_value_get_object(value));
                fill_entry(DO_KEYBOARD_DIALOG (object));
            }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_keyboard_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (object);
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
static void do_keyboard_dialog_class_init (DoKeyboardDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_keyboard_dialog_constructor;
	object_class->finalize = do_keyboard_dialog_finalize;
	object_class->get_property = do_keyboard_dialog_get_property;
	object_class->set_property = do_keyboard_dialog_set_property;

	g_type_class_add_private (object_class, sizeof (DoKeyboardDialogPrivate));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_KEYBOARD_DIALOG_FLAGS,
				     DO_KEYBOARD_DIALOG_FLAGS_DEFAULT,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (object_class,
				   PROP_ACTION,
				   g_param_spec_enum ("action",
 						      "Действие редактора",
 						      "Действие редактира с документом клавиши",
 						      DO_TYPE_DIALOG_ACTION,
 						      DO_DIALOG_ACTION_NONE,
 						      G_PARAM_READWRITE));
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
 						      G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_VIEW,
				   g_param_spec_pointer ("view",
							"Обьект просмотра",
							"Обьект просмотра",
							G_PARAM_READWRITE));
}

GtkWidget *do_keyboard_dialog_new (GtkWidget *parent, const gchar *alias_name, DoKeyboardDialogFlags flags, DoDialogAction action, DoContext *context)
{
	return g_object_new (DO_TYPE_KEYBOARD_DIALOG,
			     "flags", flags,
			     "transient-for", parent,
			     "action", action,
			     "context", context,
			     "alias-name", alias_name,
			     (gpointer) NULL);
}
static GtkWidget *add_field(DoKeyboardDialog *dialog, gint id, GtkWidget *widget)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
	g_assert (id < N_ENTRIES);
	priv->widget[id] = widget;
	if (GTK_IS_ENTRY(widget)) {
        g_signal_connect(widget, "activate", G_CALLBACK(entry_activate), dialog);
        priv->entry[id] = widget;
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
static void entry_activate(GtkWidget *entry, DoKeyboardDialog *dialog)
{
    gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
/*	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
	int i;
	for (i = 0; i < N_ENTRIES; ++i)
		if (entry == priv->widget[i] ||
		    entry == priv->entry[i]) break;
	g_assert (i < N_ENTRIES);
	++i;
	for ( ; i < N_ENTRIES; ++i)
		if (priv->widget[i] != NULL &&
		    GTK_WIDGET_IS_SENSITIVE (priv->widget[i])) break;
	if (i < N_ENTRIES)
		gtk_widget_grab_focus (priv->widget[i]);
	else {
    	      GtkWidget *w = gtk_window_get_default_widget(GTK_WINDOW (dialog));
              if (w && GTK_WIDGET_SENSITIVE(w))
                   gtk_window_activate_default (GTK_WINDOW (dialog));
	}
*/
}

static void key_symbol_changed(GtkWidget *entry, DoKeyboardDialog *dialog)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
	if (gtk_widget_get_sensitive(priv->widget[KEY_A_ENTRY])) {

	    const gchar *symb = gtk_entry_get_text(GTK_ENTRY(priv->widget[KEY_A_ENTRY]));
	    //const gchar *conv = gtk_entry_get_text(GTK_ENTRY(priv->widget[KEY_A_ENTRY]));
	    gchar *conv = ru_to_en(symb, strlen(symb));

        gint code;
        code = cash_key_code_from_string(symb);
        if (conv) {
            if (strcmp(symb, conv))
                gtk_entry_set_text(GTK_ENTRY(priv->widget[KEY_A_ENTRY]), conv);
            code = cash_key_code_from_string(conv);
            g_free(conv);
        }
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->widget[KEY_C_ENTRY]), code);
	}
}
static void key_code_changed(GtkWidget *entry, DoKeyboardDialog *dialog)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
    gint key = gtk_spin_button_get_value(GTK_SPIN_BUTTON(priv->widget[KEY_C_ENTRY]));
	if (gtk_widget_get_sensitive(priv->widget[KEY_C_ENTRY])) {
        gchar *desc = cash_key_code_to_string(key);
        gtk_entry_set_text(GTK_ENTRY(priv->widget[KEY_A_ENTRY]), desc);
        do_free(desc);
	}
	if (priv->view && DO_IS_KEYBOARD_VIEW(priv->view)) {
	    g_object_set(priv->view, "selected-code", key, NULL);
	}
}
static void key_group_changed(GtkRadioButton *style, DoKeyboardDialog *dialog)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
	gtk_widget_set_sensitive(priv->widget[KEY_A_ENTRY],
	                         gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(priv->widget[KEY_A_RADIO])));
	gtk_widget_set_sensitive(priv->widget[KEY_C_ENTRY],
	                         gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(priv->widget[KEY_C_RADIO])));
}

static void command_group_changed(GtkRadioButton *style, DoKeyboardDialog *dialog)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
	gtk_widget_set_sensitive(priv->widget[COMMAND_M_ENTRY],
	                        gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(priv->widget[COMMAND_M_RADIO])));
	gtk_widget_set_sensitive(priv->widget[COMMAND_M_P_ENTRY],
	                        gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(priv->widget[COMMAND_M_RADIO])));

}
static void command_command_changed(GtkWidget *entry, DoKeyboardDialog *dialog)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
    GtkTreeIter iter;
    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(entry), &iter))  {
        GValue val = {0,};
        gtk_tree_model_get_value(gtk_combo_box_get_model(GTK_COMBO_BOX(entry)), &iter, 1, &val);
        if (g_value_get_string(&val))
           gtk_entry_set_text(GTK_ENTRY(priv->widget[DESC_ENTRY]), g_value_get_string(&val));
    }

}
static void html_view_update(DoKeyboardDialog *dialog)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
    webkit_web_view_load_html_string(WEBKIT_WEB_VIEW(priv->label_html),
                  get_html_view_html_string(dialog), NULL);
}

static gchar *get_html_view_html_string(DoKeyboardDialog *dialog)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
	gchar *retval = g_malloc(1024);
	gchar *font_family;
	gchar *font_style;
	const gchar *label;
	const gchar *key_label;
	int i;
	gint   font_size;
	gtk_font_button_get_font_desc(GTK_FONT_BUTTON(priv->label_font),
	                      &font_family, &font_style, &font_size);

    label = gtk_entry_get_text(GTK_ENTRY(priv->widget[LABEL_ENTRY]));
    i = gtk_spin_button_get_value(GTK_SPIN_BUTTON(priv->widget[KEY_C_ENTRY]));
    key_label = do_keyboard_view_str_get_key_name(
                 i);

	if (*label == '\0') {
	    if (key_label)
            sprintf(retval, do_keyboard_str_head_html
                            do_keyboard_str_table_head_html
                            do_keyboard_str_tr_head_html
                            do_keyboard_str_td_html_empty
                            do_keyboard_str_tr_tail_html
                            do_keyboard_str_table_tail_html
                            do_keyboard_str_tail_html,
                            priv->label_size_width, priv->label_size_height,
                            font_size, font_family,
                            key_label);
	}
	else {
	    if (key_label)
            sprintf(retval, do_keyboard_str_head_html
                            do_keyboard_str_table_head_html
                            do_keyboard_str_tr_head_html
                            do_keyboard_str_td_html_aplha1
                            do_keyboard_str_tr_tail_html
                            do_keyboard_str_table_tail_html
                            do_keyboard_str_tail_html,
                            priv->label_size_width, priv->label_size_height, 4, //atoi(priv->label_size_height) - 3,
                            font_size, font_family,
                            key_label,
                            font_size, font_family,
                            label
                            );
        else
            sprintf(retval, do_keyboard_str_head_html
                            do_keyboard_str_table_head_html
                            do_keyboard_str_tr_head_html
                            do_keyboard_str_td_html
                            do_keyboard_str_tr_tail_html
                            do_keyboard_str_table_tail_html
                            do_keyboard_str_tail_html,
                            priv->label_size_width, priv->label_size_height,
                            font_size, font_family,
                            label);
	}
    g_free(font_family);
    g_free(font_style);
    return retval;

}
static void key_model_group_changed(GtkRadioButton *style, DoKeyboardDialog *dialog)
{
    html_view_update(dialog);
}
static void font_set(GtkFontButton *widget, DoKeyboardDialog *dialog)
{
    html_view_update(dialog);
}
static gboolean fill_entry(DoKeyboardDialog *dialog)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
    if (do_context_get_kind(priv->context) != DO_CONTEXT_KIND_DOCUMENT)
        return FALSE;
    gchar *number;
    gchar *dtype;
    gchar *sklad;
    g_object_get(priv->context, "dtype-code", &dtype, "sklad-code", &sklad, "document-number", &number, NULL);
    g_return_val_if_fail ( dtype != NULL, FALSE );
    g_return_val_if_fail ( sklad != NULL, FALSE );

    do_alias_t *alias = domino_alias_new(priv->alias_name);
    if (!alias)
        return FALSE;
    if (!do_alias_open(alias, 0, DO_DB_DOCUMENT, DO_DB_SKLAD, DO_DB_END)) {
        do_alias_free(alias);
        return FALSE;
    }
    gboolean retval = TRUE;
    document_key0_t document_key0;
    document_rec_t document;
    int status;
    if (number) {
        do_text_set(alias, document_key0.dtype, dtype);
        do_text_set(alias, document_key0.sklad, sklad);
        do_text_set(alias, document_key0.document, number);
        status = do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL);
        if (status == DO_KEY_NOT_FOUND) {
            gchar *val_string;
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->widget[KEY_C_ENTRY]), atoi(number));
            val_string = cash_key_code_to_string(atoi(number));
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(priv->widget[KEY_A_RADIO]), *val_string != '\0');
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(priv->widget[KEY_C_RADIO]), *val_string == '\0');
            gtk_entry_set_text(GTK_ENTRY(priv->widget[KEY_A_ENTRY]), val_string);
            do_free(val_string);
        }
        if (retval && status == DO_OK) {
            gchar *val_string;
            gint   val_int;
            val_string = do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_DESC));
            gtk_entry_set_text(GTK_ENTRY(priv->widget[DESC_ENTRY]), val_string);
            do_free(val_string);
            val_int = atoi(document.data.document);
            if (!val_int) val_int = 97;
            val_string = cash_key_code_to_string(val_int);
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(priv->widget[KEY_A_RADIO]), *val_string != '\0');
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(priv->widget[KEY_C_RADIO]), *val_string == '\0');
            gtk_entry_set_text(GTK_ENTRY(priv->widget[KEY_A_ENTRY]), val_string);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->widget[KEY_C_ENTRY]), val_int);
            do_free(val_string);

            if (priv->product_code) {
                g_free(priv->product_code);
                priv->product_code = NULL;
            }
            val_int = do_document_param_int(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND));
            if (val_int != DO_CONST_KEY_KIND_COMMAND) {
                gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(priv->widget[COMMAND_P_RADIO]), TRUE);
                if (val_int == DO_CONST_KEY_KIND_BCODE)
                    priv->product_code = do_document_param(alias, &document,
                                             do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_PRODUCT));
            }
            else {
                gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(priv->widget[COMMAND_M_RADIO]), TRUE);
                val_string = do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_COMMAND));
                gtk_entry_set_text(GTK_ENTRY(priv->entry[COMMAND_M_ENTRY]), val_string);
                do_free(val_string);
                val_string = do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_COMMAND_PARAMETERS));
                gtk_entry_set_text(GTK_ENTRY(priv->widget[COMMAND_M_P_ENTRY]), val_string);
                do_free(val_string);
            }
            val_string = do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_COMMENT));
            gtk_entry_set_text(GTK_ENTRY(priv->widget[LABEL_ENTRY]), val_string);
            do_free(val_string);
            val_string = do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_FONT));
            if (*val_string != '\0')
                gtk_font_button_set_font_name(GTK_FONT_BUTTON(priv->label_font), val_string);
            do_free(val_string);
        }
    }
    if (retval) {
        sklad_rec_t sklad_rec;
        sklad_key0_t sklad_key0;
        do_text_set(alias, sklad_key0.code, sklad);
        if ((status = do_sklad_get0(alias, &sklad_rec, &sklad_key0, DO_GET_EQUAL)) != DO_OK)
            retval = FALSE;
        if (status == DO_KEY_NOT_FOUND)
            DOMINO_SHOW_ERROR("Склад %s не найден", sklad);
        if (retval && status == DO_OK) {
            gchar *value;
            value = do_sklad_param(alias, &sklad_rec, do_param(DO_PARAM_SKLAD_PARAMETR_KEYBOARD_WIDTH));
            if (*value != '\0') {
                g_free(priv->label_size_width);
                priv->label_size_width = g_strdup(value);
            }
            do_free(value);
            value = do_sklad_param(alias, &sklad_rec, do_param(DO_PARAM_SKLAD_PARAMETR_KEYBOARD_HEIGHT));
            if (*value != '\0') {
                g_free(priv->label_size_height);
                priv->label_size_height = g_strdup(value);
            }
            do_free(value);
            key_size_update(dialog);
        }
    }
    do_alias_free(alias);
    return retval;
}
static void key_size_update(DoKeyboardDialog *dialog)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
    gchar label[1024];
    sprintf(label, label_size_label, priv->label_size_width, priv->label_size_height);
    gtk_button_set_label(GTK_BUTTON(priv->label_size), label);
    html_view_update(dialog);
};
static void key_size_clicked(GtkButton *button, DoKeyboardDialog *dialog)
{
/*    gint key = get_key_from_cursor(DO_KEYBOARD_VIEW(view));
    if (!key)
        return;
    gchar document[100];
    sprintf(document, "%d", key);
    const gchar *dtype = do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD);
    const gchar *sklad = domino_local_unit();

    DoContext *context = do_context_new_with_attributes(DO_CONTEXT_KIND_DOCUMENT,
                                          "sklad-code", sklad,
                                          "dtype-code", dtype,
                                          "document-number", document,
                                          NULL);
    GtkWidget *dialog = do_keyboard_dialog_new(gtk_widget_get_toplevel(GTK_WIDGET(view)),
                                               DO_DIALOG_ACTION_EDIT, context);
    g_object_unref(context);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
    */
}

static gboolean keyboard_document_save(DoKeyboardDialog *dialog)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
	if (priv->action == DO_DIALOG_ACTION_NONE)
        return TRUE;

    do_alias_t *alias;
    gboolean retval = TRUE;
    document_rec_t document;

    if ((alias = domino_alias_new(priv->alias_name)) == NULL)
        return FALSE;

    if (!do_alias_open(alias, 0, DO_DB_DOCUMENT, DO_DB_DOCUMENT_ORDER, DO_DB_PROTOCOL, DO_DB_END))
        retval = FALSE;
    if (retval && !keyboard_document_get(dialog, alias, &document))
        retval = FALSE;
    if (retval) {
        if (do_alias_tran_begin(alias)) {
            if (priv->action == DO_DIALOG_ACTION_INSERT)
                retval = do_action_insert_document(alias, &document);
            else
                retval = keyboard_document_update(dialog, alias, &document);
            if (retval) {
                do_alias_tran_end(alias);
                send_update_to_updated_view(dialog, alias, &document);
            }
            else
                do_alias_tran_abort(alias);
        }
        if (!retval)
           DOMINO_SHOW_ERROR("Ошибка во время сохранения документа");
    }
    if (retval) {
        gchar *number;
        number = do_text(alias, document.data.document);
        g_object_set(priv->context, "document-number", number, NULL);
        do_free(number);
    }
    do_alias_free(alias);
    return retval;
}
gint do_keyboard_dialog_run(DoKeyboardDialog *dialog)
{
    gint response = 0;
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
    while (!response) {
        response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_ACCEPT &&
            priv->action != DO_DIALOG_ACTION_NONE) {
            if (!keyboard_document_save(dialog))
                response = 0;
        }
    }
    return response;
}
static gboolean keyboard_document_get(DoKeyboardDialog *dialog, do_alias_t *alias, document_rec_t *document)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
    if (do_context_get_kind(priv->context) != DO_CONTEXT_KIND_DOCUMENT) {
        DOMINO_SHOW_ERROR("Контекст документа не определен");
        return FALSE;
    }
    gchar *sklad;
    gchar *dtype;
    gchar *number;
    const gchar *value_str;
    gchar  new_number[100];
    gint   value_int;
    int    status;

    g_object_get(priv->context, "sklad-code", &sklad, "dtype-code", &dtype, "document-number", &number, NULL);
    g_return_val_if_fail ( dtype != NULL, FALSE );
    g_return_val_if_fail ( sklad != NULL, FALSE );
    g_return_val_if_fail ( number != NULL || priv->action != DO_DIALOG_ACTION_EDIT, FALSE );

    do_text_set(alias, document->data.dtype, dtype);
    do_text_set(alias, document->data.dtype1, DO_CONST_DEFAULT_DTYPE1);
    do_text_set(alias, document->data.name, "");
    do_text_set(alias, document->data.sklad, sklad);
    document->data.p_date = 0;
    document->data.p_time = 0;
    do_text_set(alias, document->data.p_g_code, DO_CONST_DEFAULT_REGION);
    document->data.p_code = DO_CONST_DEFAULT_PARTNER;
    do_text_set(alias, document->data.p_doc, "");
    do_text_set(alias, document->data.party, "");
    document->data.accepted = 0;
    do_document_params_clear(alias, document);


    value_str = gtk_entry_get_text(GTK_ENTRY(priv->widget[DESC_ENTRY]));
    if (!priv->empty_desc_allowed && *value_str == '\0') {
        gint res;
        DOMINO_SHOW_QUESTION(res,GTK_BUTTONS_YES_NO,
                                   "Хотите оставить описание клавиши пустым?");
        if (res != GTK_RESPONSE_YES) {
            gtk_widget_grab_focus(priv->widget[DESC_ENTRY]);
            return FALSE;
        }
        priv->empty_desc_allowed = TRUE;
    }
    do_document_param_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_DESC), value_str);
    value_int = gtk_spin_button_get_value(GTK_SPIN_BUTTON(priv->widget[KEY_C_ENTRY]));
    if (!value_int) {
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(priv->widget[KEY_A_RADIO])))
            gtk_widget_grab_focus(priv->widget[KEY_A_ENTRY]);
        else
            gtk_widget_grab_focus(priv->widget[KEY_C_ENTRY]);
        return FALSE;
    }
    value_str = gtk_entry_get_text(GTK_ENTRY(priv->entry[LABEL_ENTRY]));
    if (*value_str == '\0' && !gtk_widget_has_focus(priv->widget[LABEL_ENTRY])) {
        gtk_widget_grab_focus(priv->widget[LABEL_ENTRY]);
        return FALSE;
    }
    sprintf(new_number, "%d", value_int);
    document->data.date = value_int;
    document->data.time = value_int;
    do_text_set(alias, document->data.document, new_number);
    if (priv->action != DO_DIALOG_ACTION_EDIT ||
                   strcmp(number, new_number)) {
        document_key0_t document_key0;
        do_document_key0_set(&document_key0, document);
        status = do_document_key0(alias, &document_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            DOMINO_SHOW_INFO("Запись о кнопке уже существует");
            if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(priv->widget[KEY_A_RADIO])))
                gtk_widget_grab_focus(priv->widget[KEY_A_ENTRY]);
            else
                gtk_widget_grab_focus(priv->widget[KEY_C_ENTRY]);
            return FALSE;
        }
    }
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(priv->widget[COMMAND_P_RADIO]))) {
        if (priv->action == DO_DIALOG_ACTION_EDIT) {
            if (priv->product_code) {
                do_document_param_int_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND),
                                                      DO_CONST_KEY_KIND_BCODE);
                do_document_param_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_PRODUCT),
                                                      priv->product_code);
            }
            else {
                document_order_key2_t document_order_key2;
                document_key0_t document_key0;
                do_text_set(alias, document_order_key2.dtype, dtype);
                do_text_set(alias, document_order_key2.sklad, sklad);
                do_text_set(alias, document_order_key2.document, number);
                do_text_set(alias, document_order_key2.code, "");
                memcpy(&document_key0, &document_order_key2, sizeof (document_key0));
                int count = 0;
                status = do_document_order_key2(alias, &document_order_key2, DO_GET_GE);
                while (status == DO_OK && count < 2) {
                    if (memcmp(&document_key0, &document_order_key2, sizeof (document_key0)))
                        break;
                    count++;
                    status = do_document_order_key2(alias, &document_order_key2, DO_GET_NEXT);
                }
                if (status == DO_ERROR)
                    return FALSE;

                switch (count) {
                    gchar *product_code;
                    case 0 : do_document_param_int_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND),
                                                DO_CONST_KEY_KIND_EMPTY);
                        break;
                    case 1 : do_document_param_int_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND),
                                                DO_CONST_KEY_KIND_BCODE);
                        product_code = do_text(alias, document_order_key2.code);
                        do_document_param_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_PRODUCT),
                                                            product_code);
                        do_free(product_code);
                        break;
                    default :
                        do_document_param_int_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND),
                                                DO_CONST_KEY_KIND_BCODELIST);
                }

            }
        }
        else
            do_document_param_int_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND),
                                                DO_CONST_KEY_KIND_EMPTY);
    }
    else {
        do_document_param_int_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND),
                                DO_CONST_KEY_KIND_COMMAND);
        value_str = gtk_entry_get_text(GTK_ENTRY(priv->entry[COMMAND_M_ENTRY]));
        do_document_param_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_COMMAND),
                                value_str);
        value_str = gtk_entry_get_text(GTK_ENTRY(priv->entry[COMMAND_M_P_ENTRY]));
        do_document_param_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_COMMAND_PARAMETERS),
                                value_str);
    }
    value_str = gtk_entry_get_text(GTK_ENTRY(priv->entry[LABEL_ENTRY]));
    do_document_param_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_COMMENT),
                            value_str);
    value_str = gtk_font_button_get_font_name(GTK_FONT_BUTTON(priv->label_font));
    if (strcmp(value_str, label_font_default))
        do_document_param_set(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_FONT),
                            value_str);
    return TRUE;
}
static gboolean keyboard_document_update(DoKeyboardDialog *dialog, do_alias_t *alias, document_rec_t *document)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
	gboolean retval = FALSE;

    document_key0_t document_key0_old, document_key0_new;
    document_rec_t document_old;
    gchar *sklad, *dtype, *number;
    int  status;
    do_document_key0_set(&document_key0_new, document);
    g_object_get(priv->context, "sklad-code", &sklad,
                                "dtype-code", &dtype,
                                "document-number", &number, NULL);
    do_text_set(alias, document_key0_old.dtype, dtype);
    do_text_set(alias, document_key0_old.sklad, sklad);
    do_text_set(alias, document_key0_old.document, number);

    status = do_document_get0(alias, &document_old, &document_key0_old, DO_GET_EQUAL);
    switch (status) {
        case DO_KEY_NOT_FOUND:
            retval = do_action_insert_document(alias, document);
            break;
        case DO_ERROR:
            retval = FALSE;
            break;
        case DO_OK: {
            gboolean kind_bcodelist;
            gboolean key_changed;
            gboolean line_changed = FALSE;
            retval = TRUE;

            if (do_document_update(alias, document) != DO_OK ||
                !do_protocol_document_add_rec(alias, document, DO_PROTOCOL_ACTION_CHANGE)) {
                retval = FALSE;
                break;
            }
            key_changed = memcmp(&document_key0_new, &document_key0_old, sizeof(document_key0_old));
            kind_bcodelist = do_document_param_int(alias, document,
                                  do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND)) ==
                               DO_CONST_KEY_KIND_BCODELIST;

            if ( !kind_bcodelist || key_changed ) {

                document_order_key0_t document_order_key0;
                document_order_rec_t document_order;
                memcpy(&document_order_key0, &document_key0_old, sizeof(document_key0_old));
                document_order_key0.line = 0;
                status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
                retval = TRUE;
                while (status == DO_OK) {
                    if (memcmp(&document_order_key0, &document_key0_old, sizeof(document_key0_old)))
                        break;

                    if (!kind_bcodelist) {
                        if (do_document_order_delete(alias) != DO_OK) {
                            retval = FALSE;
                            break;
                        }
                    }
                    else {
                        memcpy(&document_order.data.dtype, &document_key0_new.dtype, sizeof(document_key0_new.dtype));
                        memcpy(&document_order.data.sklad, &document_key0_new.sklad, sizeof(document_key0_new.sklad));
                        memcpy(&document_order.data.document, &document_key0_new.document, sizeof(document_key0_new.document));
                        if (do_document_order_update(alias, &document_order) != DO_OK) {
                            retval = FALSE;
                            break;
                        }
                    }
                    line_changed = TRUE;

                    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
                }
                if (status == DO_ERROR)
                    retval = FALSE;
            }
            if (retval && line_changed)
                retval = do_protocol_document_add_rec(alias, document, DO_PROTOCOL_ACTION_CHANGE_LINE);
            if (retval && key_changed)
                retval = do_protocol_document_add_rec(alias, document, DO_PROTOCOL_ACTION_CHANGE_KEY);
        }
    }
    return retval;
}
static void send_update_to_updated_view(DoKeyboardDialog *dialog, do_alias_t *alias, document_rec_t *document)
{
	DoKeyboardDialogPrivate *priv = DO_KEYBOARD_DIALOG_GET_PRIVATE (dialog);
    if (priv->view) {
        DoContext *context;
        gchar *dtype, *sklad, *number;
        if (priv->action == DO_DIALOG_ACTION_EDIT)
            do_view_update_context(priv->view, priv->context);

        dtype = do_text(alias, document->data.dtype);
        sklad = do_text(alias, document->data.sklad);
        number = do_text(alias, document->data.document);
        context = do_context_new_with_attributes(DO_CONTEXT_KIND_DOCUMENT,
                              "dtype-code", dtype,
                              "sklad-code", sklad,
                              "document-number", number , NULL);

        if (priv->action != DO_DIALOG_ACTION_EDIT || do_context_compare(priv->context, context))
            do_view_update_context(priv->view, context);

        g_object_unref(context);
        do_free(dtype);
        do_free(sklad);
        do_free(number);

    }
}
