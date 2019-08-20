

#include <gtk/gtk.h>
#include "do_filter.h"
#include "domino.h"
#include "do_entry.h"

enum
{
	PROP_0 = 0,
	PROP_ENTRY,
	PROP_LABEL,
	PROP_FLAGS,
	PROP_COMBO,
	PROP_BUTTON,
	PROP_PARENT,
};

#define DO_FILTER_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_FILTER, DoFilterPrivate))
#define DO_TYPE_FILTER_FLAGS do_filter_flags_get_type()

struct _DoFilterPrivate
{
    GtkWidget *entry;
    GtkWidget *combo;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *parent;
    GtkListStore *model;
    DoFilterFlags flags;
};

GType do_filter_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_filter_flags_values[] = {
         { DO_FILTER_FLAGS_SHOW_LABEL, "DO_FILTER_FLAGS_SHOW_LABEL", "show-label" },
         { DO_FILTER_FLAGS_SHOW_BUTTON, "DO_FILTER_FLAGS_SHOW_BUTTON", "show-button" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoFilterFlags", _do_filter_flags_values);
    }
    return type;
}

G_DEFINE_TYPE (DoFilter, do_filter, GTK_TYPE_BOX);



static void do_filter_init(DoFilter *dialog)
{

}
static GObject *do_filter_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject          *object;
//	DoFilter         *filter;
	GtkBox           *box;
    GtkWidget        *entry;
    GtkWidget        *button;
    GtkWidget        *label;
    GtkWidget        *combo;
    GtkListStore     *model;
    DoFilterPrivate  *priv;
//!!    GtkAccelGroup    *accel_group = gtk_accel_group_new ();
    //!!guint keyval;
//!!    GdkModifierType   modifiers;
    GtkWindow        *parent;
//!!    const gchar      *accel = "<Ctrl>l";
    GtkWidget        *child;


	object = G_OBJECT_CLASS (do_filter_parent_class)->constructor(type, n_construct_properties, construct_params);
	box = GTK_BOX(object);
	priv = DO_FILTER_GET_PRIVATE(object);

    g_object_set(object, "orientation", GTK_ORIENTATION_HORIZONTAL, NULL);

    parent = (GtkWindow*)gtk_widget_get_toplevel(priv->parent);
    if (!GTK_IS_WINDOW(parent))
        parent = domino_get_main_window();

//!!    gtk_window_add_accel_group (parent, accel_group);

//!!    gtk_accelerator_parse (accel, &keyval, &modifiers);
//!!    g_assert (keyval);

    model = priv->model = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);

    entry = priv->entry = do_entry_new();
#if GTK_MAJOR_VERSION > 2 || GTK_MINOR_VERSION >= 24    
    combo = priv->combo = gtk_combo_box_new_with_model_and_entry(GTK_TREE_MODEL(model));
    gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(combo), 1);
#else    
    combo = priv->combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));
#endif    
    child = gtk_bin_get_child(GTK_BIN (combo));
    gtk_container_remove(GTK_CONTAINER(combo), child);
    gtk_container_add(GTK_CONTAINER(combo), entry);

//!!    gtk_widget_add_accelerator (entry, "grab-focus", accel_group,
	//!!		      keyval, modifiers, GTK_ACCEL_VISIBLE | GTK_ACCEL_LOCKED);
	button = priv->button = gtk_button_new_with_label("Дополнительно");
	label = priv->label = gtk_label_new("Фильтр: ");

	gtk_box_pack_start(box, combo, FALSE, FALSE, 2);
	if (priv->flags & DO_FILTER_FLAGS_SHOW_BUTTON)
        gtk_box_pack_end(box, button, FALSE, FALSE, 2);
	if (priv->flags & DO_FILTER_FLAGS_SHOW_LABEL)
        gtk_box_pack_end(box, label, FALSE, FALSE, 2);

	gtk_widget_show_all (GTK_WIDGET(object));
	return object;
}

static void do_filter_finalize (GObject *object)
{
	G_OBJECT_CLASS (do_filter_parent_class)->finalize (object);
}

static void do_filter_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoFilterPrivate *priv = DO_FILTER_GET_PRIVATE (object);

	switch (prop_id)
	{
        case PROP_FLAGS:
            priv->flags = g_value_get_flags(value);
            break;
        case PROP_PARENT:
            priv->parent = g_value_get_object(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_filter_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoFilterPrivate *priv = DO_FILTER_GET_PRIVATE (object);

    switch (prop_id)
    {
        case PROP_ENTRY:
            g_value_set_object(value, priv->entry);
            break;
        case PROP_LABEL:
            g_value_set_object(value, priv->label);
            break;
        case PROP_COMBO:
            g_value_set_object(value, priv->combo);
            break;
        case PROP_BUTTON:
            g_value_set_object(value, priv->button);
            break;
        case PROP_FLAGS:
            g_value_set_flags(value, priv->flags);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}
static void do_filter_class_init (DoFilterClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_filter_constructor;
	object_class->finalize = do_filter_finalize;
	object_class->get_property = do_filter_get_property;
	object_class->set_property = do_filter_set_property;

	g_type_class_add_private (object_class, sizeof (DoFilterPrivate));

    g_object_class_install_property (object_class,
                                   PROP_ENTRY,
                                   g_param_spec_object ("entry",
							"Строка ввода условия",
							"Строка ввода условия",
							DO_TYPE_ENTRY,
							G_PARAM_READABLE));

    g_object_class_install_property (object_class,
                                   PROP_COMBO,
                                   g_param_spec_object ("combo",
							"Список ввода условия",
							"Список ввода условия",
							GTK_TYPE_COMBO_BOX,
							G_PARAM_READABLE));

    g_object_class_install_property (object_class,
                                   PROP_LABEL,
                                   g_param_spec_object ("label",
							"Тест подсказки",
							"Тест подсказки",
							GTK_TYPE_LABEL,
							G_PARAM_READABLE));

    g_object_class_install_property (object_class,
                                   PROP_BUTTON,
                                   g_param_spec_object ("button",
							"Кнопка выбора условия",
							"Кнопка выбора условия",
							GTK_TYPE_BUTTON,
							G_PARAM_READABLE));

    g_object_class_install_property (object_class,
                                   PROP_PARENT,
                                   g_param_spec_object ("parent",
							"Владелец",
							"Владелец",
							GTK_TYPE_WIDGET,
							G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_FILTER_FLAGS,
				     0,
				     G_PARAM_WRITABLE | G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));


}

GtkWidget  *do_filter_new(GtkWidget *parent, DoFilterFlags flags)
{
	return g_object_new (DO_TYPE_FILTER,
                 "parent", parent,
                 "flags", flags,

			     (gpointer) NULL);
}
