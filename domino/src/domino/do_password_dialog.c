

#include <string.h>
#include <gtk/gtk.h>

#include "do_password_dialog.h"
#include "do_icons.h"
#include "domino.h"
#include "do_view.h"

enum
{
	CHECK_USER    = 1 << 0,
	CHECK_DOMAIN  = 1 << 1,
	CHECK_MASK    = 0x1f
};
enum
{
	USERNAME_ENTRY,
	DOMAIN_ENTRY,
	PASSWORD_ENTRY,
	N_ENTRIES
};
enum
{
	PROP_0,
	PROP_FLAGS,
	PROP_DOMAIN,
};
#define DO_PASSWORD_DIALOG_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_PASSWORD_DIALOG, DoPasswordDialogPrivate))
#define DO_TYPE_PASSWORD_DIALOG_FLAGS do_password_dialog_flags_get_type()

#define OBJECT_ROOT_PATH "PasswordDialog"

GType do_password_dialog_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_password_dialog_flags_values[] = {
         { DO_PASSWORD_DIALOG_FLAGS_SHOW_USERNAME, "DO_PASSWORD_DIALOG_FLAGS_SHOW_USERNAME", "show-username" },
         { DO_PASSWORD_DIALOG_FLAGS_EDIT_USERNAME, "DO_PASSWORD_DIALOG_FLAGS_EDIT_USERNAME", "edit-username" },
         { DO_PASSWORD_DIALOG_FLAGS_SHOW_DOMAIN, "DO_PASSWORD_DIALOG_FLAGS_SHOW_DOMAIN", "show-domain" },
         { DO_PASSWORD_DIALOG_FLAGS_EDIT_DOMAIN, "DO_PASSWORD_DIALOG_FLAGS_EDIT_DOMAIN", "edit-domain" },
         { DO_PASSWORD_DIALOG_FLAGS_SHOW_PASSWORD, "DO_PASSWORD_DIALOG_FLAGS_SHOW_PASSWORD", "show-password" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoPasswordDialogFlags", _do_password_dialog_flags_values);
    }
    return type;
}

struct _DoPasswordDialogPrivate
{
	DoPasswordDialogFlags flags;
	GtkWidget *entry[N_ENTRIES];
    guint checks : 5;
	guint track_capslock : 1;
};

#if GTK_MAJOR_VERSION > 2
G_DEFINE_TYPE (DoPasswordDialog, do_password_dialog, GTK_TYPE_DIALOG)
#else
G_DEFINE_TYPE (DoPasswordDialog, do_password_dialog, GTK_TYPE_MESSAGE_DIALOG)
#endif

static void entry_activate_cb(GtkWidget *entry, DoPasswordDialog *dialog)
{
	DoPasswordDialogPrivate *priv = dialog->priv;
	int i;
	for (i = 0; i < N_ENTRIES; ++i)
		if (entry == priv->entry[i]) break;
	g_assert (i < N_ENTRIES);
	++i;
	for ( ; i < N_ENTRIES; ++i)
		if (priv->entry[i] != NULL &&
		    gtk_widget_is_sensitive(priv->entry[i])) break;
	if (i < N_ENTRIES)
		gtk_widget_grab_focus (priv->entry[i]);
	else {
    	      GtkWidget *w = gtk_window_get_default_widget(GTK_WINDOW (dialog));
              if (w && gtk_widget_is_sensitive(w))
                   gtk_window_activate_default (GTK_WINDOW (dialog));
	}
}

static void add_row(GtkWidget *table, int row, const char *text, GtkWidget *widget)
{
	GtkWidget *label = gtk_label_new_with_mnemonic (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
#if GTK_MAJOR_VERSION > 2
    gtk_grid_attach(GTK_GRID(table), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(table), widget, 1, row, 1, 1);
#else
	gtk_table_attach (GTK_TABLE(table), label,  0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE(table), widget, 1, 2, row, row + 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
#endif
	gtk_label_set_mnemonic_widget (GTK_LABEL (label), widget);
}
static GtkWidget *new_entry(DoPasswordDialog *dialog, GtkWidget *table, int row, const char *text, gboolean editable, gboolean password, GCallback changed_cb)
{
	GtkWidget *entry = gtk_entry_new();
	gtk_entry_set_visibility (GTK_ENTRY (entry), !password);
	gtk_widget_set_sensitive (entry, editable);
	g_signal_connect (entry, "changed", changed_cb, dialog);
	g_signal_connect (entry, "activate", G_CALLBACK (entry_activate_cb), dialog);
	add_row (table, row, text, entry);
	return entry;
}
static void update_responses(DoPasswordDialog *dialog)
{
	DoPasswordDialogPrivate *priv = dialog->priv;
	gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT, priv->checks == 0);
}

static void entry_changed_cb(GtkWidget *entry, DoPasswordDialog *dialog)
{
	DoPasswordDialogPrivate *priv = dialog->priv;
	guint flag = 0;
	const char *text;

	if (entry == priv->entry[USERNAME_ENTRY])
		flag = CHECK_USER;
	else if (entry == priv->entry[DOMAIN_ENTRY])
		flag = CHECK_DOMAIN;

	text = gtk_entry_get_text(GTK_ENTRY (entry));
	if (text != NULL && text[0] != '\0')
		priv->checks &= ~flag;
	else
		priv->checks |= flag;
	update_responses (dialog);
}

static void do_password_dialog_init(DoPasswordDialog *dialog)
{
	DoPasswordDialogPrivate *priv;

	priv = dialog->priv = DO_PASSWORD_DIALOG_GET_PRIVATE (dialog);
	memset(priv, 0, sizeof(DoPasswordDialogPrivate));

}

static GObject *do_password_dialog_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	GtkWindow *window;
	GtkDialog *dialog;
	GtkMessageDialog *message_dialog;
	DoPasswordDialog *password_dialog;
	DoPasswordDialogPrivate *priv;
	GtkWidget *vbox;
	GtkWidget *table;
	int row = 0;

	object = G_OBJECT_CLASS (do_password_dialog_parent_class)->constructor(type, n_construct_properties, construct_params);

	priv = DO_PASSWORD_DIALOG(object)->priv;
	window = GTK_WINDOW (object);
	dialog = GTK_DIALOG (object);
	message_dialog = GTK_MESSAGE_DIALOG(object);
	password_dialog = DO_PASSWORD_DIALOG(object);

	gtk_window_set_resizable (window, FALSE);
	//!!gtk_box_set_spacing ((gtk_dialog_get_content_area(dialog)), 2); /* Message has 24, we want 12 = 2 + 2 * 5 */

	gtk_window_set_icon_name (window, DO_STOCK_DOMINO_APPS);
        gtk_window_set_position(window, GTK_WIN_POS_CENTER_ON_PARENT);
	//!!gtk_image_set_from_icon_name (GTK_IMAGE (message_dialog->image), GTK_STOCK_DIALOG_AUTHENTICATION, GTK_ICON_SIZE_DIALOG);

	vbox = gtk_dialog_get_content_area(GTK_DIALOG(message_dialog));

#if GTK_MAJOR_VERSION > 2
    table = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(table),6);
    gtk_grid_set_row_spacing(GTK_GRID(table),6);
    gtk_container_set_border_width(GTK_CONTAINER(object), 12);
    gtk_widget_set_margin_top(table, 20);
    gtk_widget_set_margin_bottom(table, 20);
#else
	table = gtk_table_new (6, 2, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE(table), 6);
	gtk_table_set_col_spacings (GTK_TABLE(table), 12);
	//gtk_table_resize (GTK_TABLE(table), row, 2);
#endif
	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (table), FALSE, FALSE, 0);


	if (priv->flags & DO_PASSWORD_DIALOG_FLAGS_SHOW_USERNAME)
	{
		priv->entry[USERNAME_ENTRY] =
			new_entry (password_dialog,
				   table,
				   row++,
				   "Пользователь:",
				   priv->flags & DO_PASSWORD_DIALOG_FLAGS_EDIT_USERNAME,
				   FALSE,
				   G_CALLBACK (entry_changed_cb));
		priv->checks |= CHECK_USER;
	}

	if (priv->flags & DO_PASSWORD_DIALOG_FLAGS_SHOW_DOMAIN)
	{
		priv->entry[DOMAIN_ENTRY] =
			new_entry (password_dialog,
				   table,
				   row++,
				   "Домен:",
				   priv->flags & DO_PASSWORD_DIALOG_FLAGS_EDIT_DOMAIN,
				   FALSE,
				   G_CALLBACK (entry_changed_cb));
		priv->checks |= CHECK_DOMAIN;
	}

	if (priv->flags & DO_PASSWORD_DIALOG_FLAGS_SHOW_PASSWORD)
	{
		priv->entry[PASSWORD_ENTRY] =
			new_entry (password_dialog,
				   table,
				   row++,
				   "Пароль:",
				   TRUE,
				   TRUE,
				   G_CALLBACK (entry_changed_cb));

	}

	if (priv->flags & (DO_PASSWORD_DIALOG_FLAGS_SHOW_PASSWORD))
	{
		priv->track_capslock = TRUE;
	}

	/* Removed unused table rows */

	gtk_dialog_add_button (dialog, "Отмена", GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (dialog, "Ok", GTK_RESPONSE_ACCEPT);
	gtk_dialog_set_default_response (dialog, GTK_RESPONSE_ACCEPT);
	update_responses (password_dialog);

	gtk_widget_show_all (vbox);

	return object;
}

static void do_password_dialog_finalize (GObject *object)
{
	G_OBJECT_CLASS (do_password_dialog_parent_class)->finalize (object);
}

static void do_password_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	/* no readable properties */
	g_assert_not_reached ();
}

static void do_password_dialog_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoPasswordDialogPrivate *priv = DO_PASSWORD_DIALOG (object)->priv;

	switch (prop_id)
	{
		case PROP_FLAGS:
			priv->flags = g_value_get_flags (value);
			break;
	}
}

static void do_password_dialog_class_init (DoPasswordDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_password_dialog_constructor;
	object_class->finalize = do_password_dialog_finalize;
	object_class->get_property = do_password_dialog_get_property;
	object_class->set_property = do_password_dialog_set_property;

	g_type_class_add_private (object_class, sizeof (DoPasswordDialogPrivate));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_PASSWORD_DIALOG_FLAGS,
				     DO_PASSWORD_DIALOG_FLAGS_DEFAULT,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));
}

GtkWidget *do_password_dialog_new (GtkWidget *parent,
			  const char *title,
			  DoPasswordDialogFlags flags)
{
	return g_object_new (DO_TYPE_PASSWORD_DIALOG,
			     "transient-for", parent,
			     "title", title,
#if GTK_MAJOR_VERSION == 2
			     "message-type", GTK_MESSAGE_OTHER,
#endif
			     "flags", flags,
			     (gpointer) NULL);

}

const char *do_password_dialog_get_username (DoPasswordDialog *dialog)
{
	DoPasswordDialogPrivate *priv;

	g_return_val_if_fail (DO_IS_PASSWORD_DIALOG (dialog), NULL);

	priv = dialog->priv;
	if (priv->flags & DO_PASSWORD_DIALOG_FLAGS_SHOW_USERNAME)
        return gtk_entry_get_text (GTK_ENTRY (priv->entry[USERNAME_ENTRY]));
    else
        return NULL;
}

const char *do_password_dialog_get_domain (DoPasswordDialog *dialog)
{
	DoPasswordDialogPrivate *priv;

	g_return_val_if_fail (DO_IS_PASSWORD_DIALOG (dialog), NULL);

	priv = dialog->priv;
	if (priv->flags & DO_PASSWORD_DIALOG_FLAGS_SHOW_DOMAIN)
        return gtk_entry_get_text (GTK_ENTRY (priv->entry[DOMAIN_ENTRY]));
    else
        return NULL;
}

const char *do_password_dialog_get_password (DoPasswordDialog *dialog)
{
	DoPasswordDialogPrivate *priv;

	g_return_val_if_fail (DO_IS_PASSWORD_DIALOG (dialog), NULL);

	priv = dialog->priv;
	if (priv->flags & DO_PASSWORD_DIALOG_FLAGS_SHOW_PASSWORD)
        return gtk_entry_get_text (GTK_ENTRY (priv->entry[PASSWORD_ENTRY]));
    else
        return NULL;
}

void do_password_dialog_set_password (DoPasswordDialog *dialog, const char *text)
{
	DoPasswordDialogPrivate *priv;

	g_return_if_fail (DO_IS_PASSWORD_DIALOG (dialog));

	priv = dialog->priv;
	if (priv->flags & DO_PASSWORD_DIALOG_FLAGS_SHOW_PASSWORD)

	gtk_entry_set_text (GTK_ENTRY (priv->entry[PASSWORD_ENTRY]), text);
}

void do_password_dialog_set_username (DoPasswordDialog *dialog, const char *text)
{
	DoPasswordDialogPrivate *priv;

	g_return_if_fail (DO_IS_PASSWORD_DIALOG (dialog));

	priv = dialog->priv;
	if (priv->flags & DO_PASSWORD_DIALOG_FLAGS_SHOW_USERNAME)

	gtk_entry_set_text (GTK_ENTRY (priv->entry[USERNAME_ENTRY]), text);
}
void do_password_dialog_set_domain (DoPasswordDialog *dialog, const char *text)
{
	DoPasswordDialogPrivate *priv;

	g_return_if_fail (DO_IS_PASSWORD_DIALOG (dialog));

	priv = dialog->priv;
	if (priv->flags & DO_PASSWORD_DIALOG_FLAGS_SHOW_DOMAIN)

	gtk_entry_set_text (GTK_ENTRY (priv->entry[DOMAIN_ENTRY]), text);
}
