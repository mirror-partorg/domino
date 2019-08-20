#include "do_enum_value.h"



static void do_enum_value_get_property  (GObject              *object,
					     guint                 param_id,
					     GValue               *value,
					     GParamSpec           *pspec);
static void do_enum_value_set_property  (GObject              *object,
					     guint                 param_id,
					     const GValue         *value,
					     GParamSpec           *pspec);
static void do_enum_value_finalize (GObject          *object);

#define DO_ENUM_VALUE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), DO_TYPE_ENUM_VALUE, DoEnumValuePrivate))

typedef enum _DoEnumValueType DoEnumValueType;
typedef struct _DoEnumValuePrivate DoEnumValuePrivate;

/*
enum _DoEnumValueType
{
    DO_ENUM_VALUE_TYPE_UNDEFINE,
    DO_ENUM_VALUE_TYPE_INT,
    DO_ENUM_VALUE_TYPE_DOUBLE,
    DO_ENUM_VALUE_TYPE_BOOLEAN,
    DO_ENUM_VALUE_TYPE_STRING,
};*/
struct _DoEnumValuePrivate
{
    //DoEnumValueType type_;
    gchar *value;
};

enum {
    PROP_0,
    PROP_VALUE,
};

G_DEFINE_TYPE (DoEnumValue, do_enum_value, G_TYPE_OBJECT)

static void do_enum_value_init (DoEnumValue *enum_value)
{
    //DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (enum_value);
//    priv->type_ = DO_ENUM_VALUE_TYPE_UNDEFINE;
}

static void do_enum_value_class_init (DoEnumValueClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->get_property = do_enum_value_get_property;
    object_class->set_property = do_enum_value_set_property;
    object_class->finalize     = do_enum_value_finalize;

// signal
// property
    g_object_class_install_property (object_class,
				   PROP_VALUE,
				   g_param_spec_string ("value",
							"Значение",
							"Строковое значение",
							NULL,
							G_PARAM_READWRITE));

    g_type_class_add_private (object_class, sizeof (DoEnumValuePrivate));
}

static void do_enum_value_get_property (GObject     *object,
				guint        param_id,
				GValue      *value,
				GParamSpec  *pspec)
{
    DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (object);

    switch (param_id)
    {
    case PROP_VALUE:
      g_value_set_string (value, priv->value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
    }
}

static void
do_enum_value_set_property (GObject      *object,
				guint         param_id,
				const GValue *value,
				GParamSpec   *pspec)
{
    DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (object);

    switch (param_id)
    {
    case PROP_VALUE:
        if ( priv->value )
            g_free(priv->value);
        priv->value = g_value_dup_string (value);
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
    }
}
static void do_enum_value_finalize (GObject          *object)
{
    DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (object);

    if (priv->value) g_free(priv->value);

    G_OBJECT_CLASS (do_enum_value_parent_class)->finalize (object);
}
DoEnumValue *do_enum_value_new()
{
    return g_object_new (DO_TYPE_ENUM_VALUE, NULL);
}
gint do_enum_value_get_int(DoEnumValue *enum_value)
{
    DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (enum_value);
    if ( priv->value )
        return atoi(priv->value);
    else
        return 0;
}
gdouble do_enum_value_get_double(DoEnumValue *enum_value)
{
    DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (enum_value);
    if ( priv->value )
        return atof(priv->value);
    else
        return 0;

}
static gchar *empty_str = "";
const gchar *do_enum_value_get_string(DoEnumValue *enum_value)
{
    DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (enum_value);
    if ( priv->value )
        return priv->value;
    else
        return empty_str;
}
gboolean do_enum_value_get_boolean(DoEnumValue *enum_value)
{
    DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (enum_value);
    if ( priv->value )
        return atoi(priv->value) != 0;
    else
        return FALSE;
}
void do_enum_value_set_int(DoEnumValue *enum_value, gint value)
{
    DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (enum_value);
    if ( priv->value )
        g_free(priv->value);
    priv->value = g_strdup_printf("%d", value);
}
void do_enum_value_set_double(DoEnumValue *enum_value, gdouble value)
{
    DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (enum_value);
    if ( priv->value )
        g_free(priv->value);
    priv->value = g_strdup_printf("%f", value);

}
void do_enum_value_set_string(DoEnumValue *enum_value, const gchar *value)
{
    DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (enum_value);
    if ( priv->value )
        g_free(priv->value);
    priv->value = g_strdup(value);
}
void do_enum_value_set_boolean(DoEnumValue *enum_value, gboolean value)
{
    DoEnumValuePrivate *priv = DO_ENUM_VALUE_GET_PRIVATE (enum_value);
    if ( priv->value )
        g_free(priv->value);
    priv->value = g_strdup_printf("%d", value == TRUE);
}

