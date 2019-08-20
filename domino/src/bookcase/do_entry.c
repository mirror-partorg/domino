

#include "do_entry.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdlib.h>
#include <string.h>
#include "do_validate.h"
#include "do_utilx.h"

enum {
    SUBSTITUTION,
    LAST_SIGNAL
};

enum
{
	PROP_0,
	PROP_REGEX_MASK,
	PROP_VALID,
	PROP_CLEAR_BUTTON,
};

static guint signals[LAST_SIGNAL] = { 0 };

#define DO_ENTRY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_ENTRY, DoEntryPrivate))
#define DO_TYPE_ENTRY_FLAGS do_entry_flags_get_type()
#ifndef GDK_KEY_Return
#define GDK_KEY_Return GDK_Return
#endif

struct _DoEntryPrivate
{
    DoValidateFunc      validate_func;
    gpointer            validate_data;
    gchar              *regex_mask;
};

static void do_entry_editable_init (GtkEditableInterface *iface);
static void do_entry_validate_init (DoValidateIface *iface);
static void set_background (DoEntry *entry);
static gboolean do_entry_validate(DoValidate *iface);
static void do_entry_substitution(DoEntry *entry);

G_DEFINE_TYPE_WITH_CODE (DoEntry, do_entry, GTK_TYPE_ENTRY,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_EDITABLE,
						do_entry_editable_init)
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VALIDATE,
						do_entry_validate_init)
						);

static void do_entry_changed (GtkEditable *editable)
{
    do_validate_completion(DO_VALIDATE(editable));
    set_background(DO_ENTRY(editable));
}

static void do_entry_editable_init (GtkEditableInterface *iface)
{
    iface->changed = do_entry_changed;
}

static void do_entry_validate_init (DoValidateIface *iface)
{
    iface->valid = do_entry_validate;
}

static void do_entry_init(DoEntry *dialog)
{
}

static void entry_icon_press(GtkEntry *entry,  GtkEntryIconPosition icon_pos, GdkEvent  *event, DoEntry *object)
{
    if (icon_pos == GTK_ENTRY_ICON_SECONDARY) {
        gboolean editable;
        g_object_get(entry, "editable", &editable, NULL);
        if (editable)
            gtk_entry_set_text(entry, "");
    }
}
static GObject *do_entry_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	DoEntry *entry;
	GtkEntryCompletion *completion;

	object = G_OBJECT_CLASS (do_entry_parent_class)->constructor(type, n_construct_properties, construct_params);
	entry = (DoEntry*)object;

    completion = gtk_entry_completion_new ();
    gtk_entry_set_completion (GTK_ENTRY (entry), completion);
    g_object_unref (completion);
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "edit-clear-all-symbolic");

    g_signal_connect(entry, "icon-press",
			G_CALLBACK (entry_icon_press), object);
	set_background(entry);

	return object;
}
static void do_entry_finalize (GObject *object)
{
    DoEntryPrivate *priv = DO_ENTRY_GET_PRIVATE (object);
    if (priv->regex_mask)
        g_free(priv->regex_mask);
	G_OBJECT_CLASS (do_entry_parent_class)->finalize (object);
}


static void do_entry_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
//	DoEntryPrivate *priv = DO_ENTRY_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_REGEX_MASK:
            do_entry_set_regex_mask(DO_ENTRY(object), g_value_get_string(value));
			break;
	}

}
static void do_entry_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoEntryPrivate *priv = DO_ENTRY_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_REGEX_MASK:
            g_value_set_string(value, priv->regex_mask);
            break;
        case PROP_VALID:
            g_value_set_boolean(value, do_validate_valid(DO_VALIDATE(object)));
            break;
    }
}
static void do_entry_class_init (DoEntryClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_entry_constructor;
	object_class->finalize     = do_entry_finalize;
	object_class->get_property = do_entry_get_property;
	object_class->set_property = do_entry_set_property;

    klass->substitution = do_entry_substitution;

	g_type_class_add_private (object_class, sizeof (DoEntryPrivate));

    GtkBindingSet *binding_set;
    binding_set = gtk_binding_set_by_class (&klass->parent_class);
    gtk_binding_entry_add_signal (binding_set, GDK_KEY_Return, 0,
				"substitution", 0);
    //gtk_binding_entry_remove (binding_set, GDK_Return, 0);
				//"activate", 0);

	g_object_class_install_property
		(object_class,
		 PROP_VALID,
		 g_param_spec_boolean ("valid",
				     "valid",
				     "valid",
				     FALSE,
				     G_PARAM_READABLE));

	g_object_class_install_property
		(object_class,
		 PROP_REGEX_MASK,
		 g_param_spec_string ("regex-mask",
				     "Маска регулярного выражения",
				     "Маска регулярного выражения для проверки валидности",
				     NULL,
				     G_PARAM_READWRITE));

    signals[SUBSTITUTION] =
        g_signal_new ("substitution",
		  G_OBJECT_CLASS_TYPE (object_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (DoEntryClass, substitution),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

}

GtkWidget *do_entry_new ()
{
	return g_object_new (DO_TYPE_ENTRY,
			     (gpointer) NULL);
}

void do_entry_set_regex_mask(DoEntry  *entry, const char* mask)
{
	DoEntryPrivate *priv = DO_ENTRY_GET_PRIVATE (entry);
    if (priv->regex_mask)
        g_free(priv->regex_mask);
    if (mask)
        priv->regex_mask = g_strdup(mask);
    else
        priv->regex_mask = NULL;
    set_background(entry);
}
void do_entry_set_validate_func(DoEntry *entry, DoValidateFunc func, gpointer data)
{
	DoEntryPrivate *priv = DO_ENTRY_GET_PRIVATE (entry);
	priv->validate_func = func;
	priv->validate_data = data;
	set_background(entry);
}
gboolean do_entry_valid(DoEntry  *entry)
{
    gboolean res;
    res = do_entry_validate(DO_VALIDATE(entry));
    set_background(entry);
    return res;
}


static gboolean do_entry_validate(DoValidate *iface)
{
    DoEntryPrivate *priv = DO_ENTRY_GET_PRIVATE(iface);
    const gchar *text;

    text = gtk_entry_get_text (GTK_ENTRY (iface));

    if ( priv->validate_func )
        if ( !priv->validate_func(text, priv->validate_data) )
            return FALSE;

    if ( priv->regex_mask )
        return g_regex_match_simple (priv->regex_mask, text, 0, 0);


    return TRUE;
}

static void set_background (DoEntry *entry)
{
#if GTK_MAJOR_VERSION > 2 //to do
#else
    static const GdkColor error_color = { 0, 65535, 60000, 60000 };

    if (!do_validate_valid(DO_VALIDATE(entry)))
        gtk_widget_modify_base (GTK_WIDGET (entry), GTK_STATE_NORMAL, &error_color);
    else
        gtk_widget_modify_base (GTK_WIDGET (entry), GTK_STATE_NORMAL, NULL);
#endif
}
static void do_entry_substitution(DoEntry *entry)
{
    if ( do_validate_substitution(DO_VALIDATE(entry)) )
        g_signal_emit_by_name (GTK_ENTRY(entry), "activate", NULL);
}
