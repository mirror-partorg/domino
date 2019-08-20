

#include "do_entry_box.h"
#include "do_icons.h"

enum
{
	PROP_0,
	PROP_ENTRY,
	PROP_BUTTON,
	PROP_ENTRY_TYPE,
};

#define DO_ENTRY_BOX_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_ENTRY_BOX, DoEntryBoxPrivate))

struct _DoEntryBoxPrivate
{
	GtkWidget *entry;
	GtkWidget *button;
	GType      entry_type;

};

G_DEFINE_TYPE (DoEntryBox, do_entry_box, GTK_TYPE_BOX);

static void do_entry_box_init(DoEntryBox *dialog)
{
}

static GObject *do_entry_box_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject   *object;
	GtkWidget *entry;
	GtkBox    *hbox;
	GtkWidget  *button;
	DoEntryBoxPrivate *priv;

	object = G_OBJECT_CLASS (do_entry_box_parent_class)->constructor(type, n_construct_properties, construct_params);

    hbox = GTK_BOX(object);
    priv = DO_ENTRY_BOX_GET_PRIVATE (object);

    if (priv->entry_type) {
        entry = priv->entry = g_object_new(priv->entry_type, NULL);
    }
    else {
        entry = priv->entry = do_entry_new();
    }

	gtk_box_pack_start (hbox, entry, TRUE, TRUE, 0);

    button = priv->button = gtk_button_new_with_mnemonic("_Выбрать");

	gtk_box_pack_end (hbox, button, FALSE, TRUE, 0);

	return object;
}

static void do_entry_box_finalize (GObject *object)
{
//	DoEntryBoxPrivate *priv = DO_ENTRY_BOX_GET_PRIVATE (object);
	G_OBJECT_CLASS (do_entry_box_parent_class)->finalize (object);
}

static void do_entry_box_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoEntryBoxPrivate *priv = DO_ENTRY_BOX_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_ENTRY: {
            g_value_set_object(value, priv->entry);
			break;
        }
        case PROP_ENTRY_TYPE: {
            g_value_set_gtype(value, priv->entry_type);
			break;
        }
        case PROP_BUTTON: {
            g_value_set_object(value, priv->button);
            break;
        }
    }
}
static void do_entry_box_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoEntryBoxPrivate *priv = DO_ENTRY_BOX_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ENTRY_TYPE:
			priv->entry_type = g_value_get_gtype (value);
			break;
	}
}

static void do_entry_box_class_init (DoEntryBoxClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
    //GtkWidgetClass *widget_class = (GtkWidgetClass*)klass;

	object_class->constructor  = do_entry_box_constructor;
	object_class->finalize     = do_entry_box_finalize;
	object_class->get_property = do_entry_box_get_property;
	object_class->set_property = do_entry_box_set_property;

	g_type_class_add_private (object_class, sizeof (DoEntryBoxPrivate));

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
		 PROP_BUTTON,
		 g_param_spec_object("button",
				     "GtkButton",
				     "GtkButton",
				     GTK_TYPE_BUTTON,
				     G_PARAM_READABLE));

	g_object_class_install_property
		(object_class,
		 PROP_ENTRY_TYPE,
		 g_param_spec_gtype ("entry-type",
				     "entry-type",
				     "entry-type",
				     DO_TYPE_ENTRY,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));

}

GtkWidget *do_entry_box_new(DoEntryDateFlags flags)
{
	return g_object_new (DO_TYPE_ENTRY_BOX,
	                     "flags", flags,
	                     "spacing", 6,
	                     "homogeneous", FALSE,
			     (gpointer) NULL);
}

GtkWidget  *do_entry_box_entry(DoEntryBox *box)
{
	DoEntryBoxPrivate *priv = DO_ENTRY_BOX_GET_PRIVATE (box);
	return priv->entry;

}
GtkWidget  *do_entry_box_button(DoEntryBox *box)
{
	DoEntryBoxPrivate *priv = DO_ENTRY_BOX_GET_PRIVATE (box);
	return priv->button;
}
