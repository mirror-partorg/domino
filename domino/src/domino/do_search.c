

#include "do_search.h"
#include <gtk/gtk.h>
#include "domino.h"
#include "do_entry.h"
#include "do_marshal.h"

enum
{
	PROP_0 = 0,
	PROP_FLAGS,
	PROP_PARENT,
};

#define DO_SEARCH_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_SEARCH, DoSearchPrivate))
#define DO_TYPE_SEARCH_FLAGS do_search_flags_get_type()

struct _DoSearchPrivate
{
    GtkWidget *entry;
    GtkWidget *label;
    GtkWidget *parent;
    GList     *creterions;
    gint       creterion;
    GtkWidget *menu;
    GSList    *group;
//    GtkListStore *model;
    DoSearchFlags flags;
};

GType do_search_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_search_flags_values[] = {
         { DO_SEARCH_FLAGS_SHOW_LABEL, "DO_SEARCH_FLAGS_SHOW_LABEL", "show-label" },
         { DO_SEARCH_FLAGS_SHOW_BUTTONS, "DO_SEARCH_FLAGS_SHOW_BUTTONS", "show-buttons" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoSearchFlags", _do_search_flags_values);
    }
    return type;
}

G_DEFINE_TYPE (DoSearch, do_search, GTK_TYPE_BOX);

enum
{
	FIND_SIGNAL,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];


static void do_search_init(DoSearch *dialog)
{

}
static void do_search_hide(GtkWidget *widget, DoSearch *dialog)
{
    gtk_widget_hide(GTK_WIDGET(dialog));
}
static void do_search_next(GtkWidget *widget, DoSearch *dialog)
{
	DoSearchPrivate *priv = DO_SEARCH_GET_PRIVATE (dialog);

	g_signal_emit (dialog, signals[FIND_SIGNAL], 0, TRUE, priv->creterion, gtk_entry_get_text(GTK_ENTRY(priv->entry)));
}
static void do_search_prev(GtkWidget *widget, DoSearch *dialog)
{
	DoSearchPrivate *priv = DO_SEARCH_GET_PRIVATE (dialog);

	g_signal_emit (dialog, signals[FIND_SIGNAL], 0, FALSE, priv->creterion, gtk_entry_get_text(GTK_ENTRY(priv->entry)));
}
static GObject *do_search_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject          *object;
//	DoSearch         *search;
	GtkBox           *box;
    GtkWidget        *entry;
    GtkWidget        *close_button;
    GtkWidget        *button_next;
    GtkWidget        *button_prev;
    GtkWidget        *label;
    GtkWidget        *image;
    DoSearchPrivate  *priv;
    GtkWindow        *parent;

	object = G_OBJECT_CLASS (do_search_parent_class)->constructor(type, n_construct_properties, construct_params);
	box = GTK_BOX(object);
	priv = DO_SEARCH_GET_PRIVATE(object);

    g_object_set(object, "orientation", GTK_ORIENTATION_HORIZONTAL, NULL);

    parent = (GtkWindow*)gtk_widget_get_toplevel(priv->parent);
    if (!GTK_IS_WINDOW(parent))
        parent = domino_get_main_window();

	close_button = gtk_button_new();
	gtk_button_set_relief (GTK_BUTTON (close_button),
			       GTK_RELIEF_NONE);
	/* don't allow focus on the close button */
	gtk_button_set_focus_on_click (GTK_BUTTON (close_button), FALSE);
	gtk_widget_set_name (close_button, "do-search-close-button");
#if GTK_MAJOR_VERSION > 2
	image = gtk_image_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);
#else
	image = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
#endif
	gtk_widget_set_tooltip_text (close_button, "Закрыть");
	g_signal_connect (close_button, "clicked",
			  G_CALLBACK (do_search_hide), object);
	gtk_container_add (GTK_CONTAINER (close_button), image);
	gtk_box_pack_start (box, close_button, FALSE, FALSE, 0);

    entry = priv->entry = do_entry_new();

	label = priv->label = gtk_label_new("Найти:");

	if (priv->flags & DO_SEARCH_FLAGS_SHOW_LABEL)
        gtk_box_pack_start(box, label, FALSE, FALSE, 2);
	gtk_box_pack_start(box, entry, FALSE, FALSE, 2);

	button_prev = gtk_button_new_with_label("Предыдущее");
	gtk_button_set_relief (GTK_BUTTON (button_prev),
			       GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click (GTK_BUTTON (button_prev), FALSE);
	gtk_widget_set_name (button_prev, "do-search-prev-button");
	gtk_widget_set_tooltip_text (button_prev, "Предыдущее");
	g_signal_connect (button_prev, "clicked",
			  G_CALLBACK (do_search_prev), object);

	button_next = gtk_button_new_with_label("Следующий");
	gtk_button_set_relief (GTK_BUTTON (button_next),
			       GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click (GTK_BUTTON (button_next), FALSE);
	gtk_widget_set_name (button_next, "do-search-next-button");
	gtk_widget_set_tooltip_text (button_next, "Следующее");
	g_signal_connect (button_next, "clicked",
			  G_CALLBACK (do_search_next), object);

	g_signal_connect (entry, "activate",
			  G_CALLBACK (do_search_next), object);

    priv->menu = gtk_menu_new();


	if (priv->flags & DO_SEARCH_FLAGS_SHOW_BUTTONS) {
        gtk_box_pack_start(box, button_prev, FALSE, FALSE, 2);
        gtk_box_pack_start(box, button_next, FALSE, FALSE, 2);
	}

	gtk_widget_show_all (GTK_WIDGET(object));
	return object;
}

static void do_search_finalize (GObject *object)
{
	DoSearchPrivate *priv = DO_SEARCH_GET_PRIVATE (object);
	g_list_foreach(priv->creterions, (GFunc)g_free, NULL);

	G_OBJECT_CLASS (do_search_parent_class)->finalize (object);
}

static void do_search_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoSearchPrivate *priv = DO_SEARCH_GET_PRIVATE (object);

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
static void do_search_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoSearchPrivate *priv = DO_SEARCH_GET_PRIVATE (object);

    switch (prop_id)
    {
        case PROP_FLAGS:
            g_value_set_flags(value, priv->flags);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}
static void do_search_class_init (DoSearchClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor = do_search_constructor;
	object_class->finalize = do_search_finalize;
	object_class->get_property = do_search_get_property;
	object_class->set_property = do_search_set_property;

	g_type_class_add_private (object_class, sizeof (DoSearchPrivate));

	signals[FIND_SIGNAL] =
		g_signal_new ("find",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (DoSearchClass, find),
			      NULL, NULL,
			      do_marshal_VOID__BOOLEAN_INT_STRING,
			      G_TYPE_NONE,
			      3,
			      G_TYPE_BOOLEAN,
			      G_TYPE_INT,
			      G_TYPE_STRING
			      );

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
				     DO_TYPE_SEARCH_FLAGS,
				     0,
				     G_PARAM_WRITABLE | G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));


}

GtkWidget  *do_search_new(GtkWidget *parent, DoSearchFlags flags)
{
	return g_object_new (DO_TYPE_SEARCH,
                 "parent", parent,
                 "flags", flags,
			     (gpointer) NULL);
}
void do_search_show(DoSearch *search, gint creterion)
{
	DoSearchPrivate *priv = DO_SEARCH_GET_PRIVATE (search);
	GList *l;

	priv->creterion = creterion;

    if (!gtk_widget_get_visible(GTK_WIDGET(search)))
        gtk_widget_show(GTK_WIDGET(search));
    l = g_list_nth(priv->creterions, creterion);
    if (!l)
        gtk_label_set_text(GTK_LABEL(priv->label), "Найти:");
    else
        gtk_label_set_text(GTK_LABEL(priv->label), l->data);
    gtk_widget_grab_focus(GTK_WIDGET(priv->entry));
}
static void do_search_entry_icon_released(GtkEntry *entry,
                                          GtkEntryIconPosition icon_pos,
                                          GdkEvent  *event,
                                          DoSearch *search)
{
	DoSearchPrivate *priv = DO_SEARCH_GET_PRIVATE (search);

    if (icon_pos == GTK_ENTRY_ICON_PRIMARY)
        gtk_menu_popup(GTK_MENU(priv->menu),
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  0,
                                  gtk_get_current_event_time());

}
static void do_search_creterion_toggled(GtkCheckMenuItem *item, DoSearch *search)
{
	DoSearchPrivate *priv = DO_SEARCH_GET_PRIVATE (search);
	GList *l;

	priv->creterion = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(item), "creterion"));
    l = g_list_nth(priv->creterions, priv->creterion);
    if (!l)
        gtk_label_set_text(GTK_LABEL(priv->label), "Найти:");
    else
        gtk_label_set_text(GTK_LABEL(priv->label), l->data);
}

gint do_search_add_criterion(DoSearch *search, const gchar *title)
{
	DoSearchPrivate *priv = DO_SEARCH_GET_PRIVATE (search);
	gint retval;
	GtkWidget *w;

    priv->creterions = g_list_append(priv->creterions, g_strdup(title));
    retval = g_list_length(priv->creterions);

    if (retval > 1) {
        //!!gtk_entry_set_icon_from_stock (GTK_ENTRY(priv->entry), GTK_ENTRY_ICON_PRIMARY, GTK_STOCK_FIND);
        g_signal_connect(priv->entry, "icon-release" , G_CALLBACK(do_search_entry_icon_released), search);
    }
    else
        gtk_label_set_text(GTK_LABEL(priv->label), title);

    w = gtk_radio_menu_item_new_with_label(priv->group, title);
    priv->group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(w));
    g_object_set_data(G_OBJECT(w), "creterion",
                       GINT_TO_POINTER(retval-1) );
    g_signal_connect(w, "toggled",
                      G_CALLBACK(do_search_creterion_toggled), search);
    gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu), w );
    gtk_widget_show(w);

    return retval - 1;
}
