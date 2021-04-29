

#include "do_template.h"
//#include "do_icons.h"

#define DO_TEMPLATE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_TEMPLATE, DoTemplatePrivate))
#define DO_TYPE_TEMPLATE_FLAGS do_template_flags_get_type()

typedef struct _DoTemplatePrivate	DoTemplatePrivate;

enum
{
	PROP_0,
	PROP_FLAGS
};

GType do_template_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_template_flags_values[] = {
         { DO_TEMPLATE_FLAGS_, "DO_TEMPLATE_FLAGS_", "" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoTemplateFlags", _do_template_flags_values);
    }
    return type;
}

struct _DoTemplatePrivate
{
	DoTemplateFlags flags;
	...
};

G_DEFINE_TYPE_WITH_CODE (DoTemplate, do_template, GTK_TYPE_OBJECT, G_ADD_PRIVATE(DoTemplate))


static void do_template_init(DoTemplate *temp)
{
	DoTemplatePrivate *priv;

	priv = temp->priv = DO_TEMPLATE_GET_PRIVATE (temp);
    //memset(priv, 0, sizeof(*priv));

}

static GObject *do_template_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;

	DoTemplate *temp;
	DoTemplatePrivate *priv;

	object = G_OBJECT_CLASS (do_template_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv = DO_TEMPLATE(object)->priv;
    ...


	gtk_widget_show_all (temp);

	return object;
}

static void do_template_finalize (GObject *object)
{
	G_OBJECT_CLASS (do_template_parent_class)->finalize (object);
}

static void do_template_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	/* no readable properties */
	g_assert_not_reached ();
}

static void do_template_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoTemplatePrivate *priv = DO_TEMPLATE (object)->priv;

	switch (prop_id)
	{
		case PROP_FLAGS:
			priv->flags = g_value_get_flags (value);
			break;
	}
}

static void do_template_class_init (DoTemplateClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_template_constructor;
	object_class->finalize     = do_template_finalize;
	object_class->get_property = do_template_get_property;
	object_class->set_property = do_template_set_property;

	//g_type_class_add_private (object_class, sizeof (DoTemplatePrivate));

	g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_TEMPLATE_FLAGS,
				     DO_TEMPLATE_FLAGS_DEFAULT,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));
}

GtkWidget *do_template_new (GtkWidget *parent,
			  const char *title,
			  DoTemplateFlags flags)
{
	return g_object_new (DO_TYPE_TEMPLATE,
	              ....
			     "flags", flags,
			     (gpointer) NULL);
}
