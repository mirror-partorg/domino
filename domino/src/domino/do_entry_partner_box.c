

#include "do_entry_partner_box.h"
#include "do_icons.h"
#include "do_context.h"
#include "do_validate.h"

enum
{
	PROP_0,
	PROP_ALIAS,
	PROP_REGION,
	PROP_CONTEXT,
	PROP_ENTRY,
	PROP_FLAGS,
};

#define DO_ENTRY_PARTNER_BOX_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_ENTRY_PARTNER_BOX, DoEntryPartnerBoxPrivate))

struct _DoEntryPartnerBoxPrivate
{
	GtkWidget *entry;
	GtkWidget *button_ch;
	GtkWidget *button_new;
	gchar     *alias_name;
	gchar     *region;
	DoEntryPartnerFlags flags;
    GtkListStore *store;
};

G_DEFINE_TYPE (DoEntryPartnerBox, do_entry_partner_box, GTK_TYPE_BOX);

static void do_entry_partner_box_init(DoEntryPartnerBox *dialog)
{
}
static void do_entry_partner_box_dialog_entry_activate(GtkWidget *entry, GtkWindow *dialog)
{
    if ( do_validate_valid(DO_VALIDATE(entry)) ) {
        GtkWidget *w = gtk_window_get_default_widget(GTK_WINDOW (dialog));
        if (w && gtk_widget_get_sensitive(w))
            gtk_window_activate_default (GTK_WINDOW (dialog));
    }
}
static void do_entry_partner_box_button_add_clicked(GtkWidget *widget, DoEntryPartnerBox *box)
{
    DoEntryPartnerBoxPrivate *priv = DO_ENTRY_PARTNER_BOX_GET_PRIVATE(box);
    GtkWidget *dialog;
    GtkWindow *win;
    GtkWidget *l;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *entry;
    win = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(box)));

    dialog = gtk_dialog_new_with_buttons ("Добавление представителя",
					win,
					GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					"ok",
					GTK_RESPONSE_OK,
                    "Отмена",
                    GTK_RESPONSE_CANCEL,
					NULL);
    gtk_dialog_set_default_response (GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_icon_name (GTK_WINDOW(dialog), DO_STOCK_DOMINO_APPS);

	vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width( GTK_CONTAINER( vbox ), 12 );
#if GTK_MAJOR_VERSION < 3
    hbox = gtk_hbox_new(FALSE, 0);
#else
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
#endif
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 4 );
    l = gtk_label_new("Наименование:");
    entry = do_entry_new();
    g_object_set(entry, "regex-mask", "^(.+)$", NULL);
	g_signal_connect (entry, "activate", G_CALLBACK (do_entry_partner_box_dialog_entry_activate), dialog);
	if ( !do_validate_valid(DO_VALIDATE(priv->entry)) ) {
	    g_object_set(entry, "text", gtk_entry_get_text(GTK_ENTRY(priv->entry)), NULL);
	}

	gtk_box_pack_start (GTK_BOX (hbox), l, FALSE, FALSE, 4);
	gtk_box_pack_start (GTK_BOX (hbox), entry, FALSE, FALSE, 4);
	gtk_widget_show_all (vbox);
    while (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        gchar *text;
        g_object_get(entry, "text", &text, NULL);
        if ( do_entry_partner_add_partner_by_name(DO_ENTRY_PARTNER(priv->entry), text) ) {
            do_entry_partner_model_fill(DO_ENTRY_PARTNER(priv->entry), priv->store);
            break;
        }
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static GObject *do_entry_partner_box_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject   *object;
	GtkWidget *entry;
	GtkBox    *hbox;
	GtkWidget *combo;
	GtkWidget  *button;
	GtkWidget  *child;
	GtkListStore *store;
	DoEntryPartnerBoxPrivate *priv;

	object = G_OBJECT_CLASS (do_entry_partner_box_parent_class)->constructor(type, n_construct_properties, construct_params);

    hbox = GTK_BOX(object);
    priv = DO_ENTRY_PARTNER_BOX_GET_PRIVATE (object);
    priv->store = store = gtk_list_store_new(1, G_TYPE_STRING);

    combo = gtk_combo_box_new_with_entry();
    g_object_set(combo, "model", store, NULL);
    gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(combo), 0);

    child = gtk_bin_get_child(GTK_BIN (combo));

    entry = priv->entry = do_entry_partner_new(priv->alias_name, priv->flags);
    gtk_container_remove(GTK_CONTAINER(combo), child);
    gtk_container_add(GTK_CONTAINER(combo), entry);

	gtk_box_pack_start (hbox, combo, TRUE, TRUE, 0);

    button = priv->button_new = gtk_button_new_with_mnemonic("_Добавить");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK (do_entry_partner_box_button_add_clicked), object);
	gtk_box_pack_end (hbox, button, FALSE, TRUE, 0);

	return object;
}

static void do_entry_partner_box_finalize (GObject *object)
{
	DoEntryPartnerBoxPrivate *priv = DO_ENTRY_PARTNER_BOX_GET_PRIVATE (object);
	if ( priv->alias_name )
        g_free(priv->alias_name);
    if ( priv->region )
        g_free(priv->region);
	G_OBJECT_CLASS (do_entry_partner_box_parent_class)->finalize (object);
}

static void do_entry_partner_box_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoEntryPartnerBoxPrivate *priv = DO_ENTRY_PARTNER_BOX_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_ENTRY: {
            g_value_set_object(value, priv->entry);
			break;
        }
        case PROP_FLAGS:
			g_value_set_flags (value, priv->flags);
			break;
        case PROP_CONTEXT: {
            DoContext *s;
            g_object_get(priv->entry, "context", &s, NULL);
            g_value_set_object(value, s);
            break;
        }
    }
}
static void do_entry_partner_box_model_fill(DoEntryPartnerBox *object)
{
	DoEntryPartnerBoxPrivate *priv = DO_ENTRY_PARTNER_BOX_GET_PRIVATE (object);
	do_entry_partner_model_fill(DO_ENTRY_PARTNER(priv->entry), priv->store);

}
static void do_entry_partner_box_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoEntryPartnerBoxPrivate *priv = DO_ENTRY_PARTNER_BOX_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_FLAGS:
			priv->flags = g_value_get_flags (value);
			break;
        case PROP_ALIAS:
            priv->alias_name = g_value_dup_string(value);
            break;
        case PROP_REGION:
            priv->region = g_value_dup_string(value);
            g_object_set(priv->entry, "region-code", priv->region, NULL);
            do_entry_partner_box_model_fill(DO_ENTRY_PARTNER_BOX(object));
            break;
	}
}

static void do_entry_partner_box_class_init (DoEntryPartnerBoxClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
    //GtkWidgetClass *widget_class = (GtkWidgetClass*)klass;

	object_class->constructor  = do_entry_partner_box_constructor;
	object_class->finalize     = do_entry_partner_box_finalize;
	object_class->get_property = do_entry_partner_box_get_property;
	object_class->set_property = do_entry_partner_box_set_property;

	g_type_class_add_private (object_class, sizeof (DoEntryPartnerBoxPrivate));

	g_object_class_install_property
		(object_class,
		 PROP_ENTRY,
		 g_param_spec_object("entry",
				     "DoEntry",
				     "DoEntry",
				     DO_TYPE_ENTRY,
				     G_PARAM_READABLE));
    g_object_class_install_property
        (object_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property
        (object_class,
         PROP_REGION,
         g_param_spec_string("region-code",
                      "region-code",
                      "region-code",
                      NULL,
                       G_PARAM_WRITABLE));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_ENTRY_PARTNER_FLAGS,
				     0,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (object_class,
				   PROP_CONTEXT,
				   g_param_spec_object ("context",
 						      "Контекст партнера",
 						      "Контекст партнера",
 						      DO_TYPE_CONTEXT,
 						      G_PARAM_READABLE));

}

GtkWidget *do_entry_partner_box_new(const gchar *alias_name, DoEntryPartnerFlags flags)
{
	return g_object_new (DO_TYPE_ENTRY_PARTNER_BOX,
                         "alias-name", alias_name,
	                     "flags", flags,
	                     "spacing", 6,
	                     "homogeneous", FALSE,
			     (gpointer) NULL);
}

GtkWidget  *do_entry_partner_box_entry(DoEntryPartnerBox *box)
{
	DoEntryPartnerBoxPrivate *priv = DO_ENTRY_PARTNER_BOX_GET_PRIVATE (box);
	return priv->entry;
}
GtkWidget  *do_entry_partner_box_button(DoEntryPartnerBox *box)
{
	DoEntryPartnerBoxPrivate *priv = DO_ENTRY_PARTNER_BOX_GET_PRIVATE (box);
	return priv->button_new;
}

