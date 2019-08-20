#include "do_context.h"


static void do_context_get_property  (GObject              *object,
					     guint                 param_id,
					     GValue               *value,
					     GParamSpec           *pspec);
static void do_context_set_property  (GObject              *object,
					     guint                 param_id,
					     const GValue         *value,
					     GParamSpec           *pspec);
static void do_context_finalize (GObject          *object);

#define DO_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), DO_TYPE_CONTEXT, DoContextPrivate))

typedef struct _DoContextPrivate DoContextPrivate;
struct _DoContextPrivate
{
    DoContextKind kind;
    gchar *product_code;
    gchar *sklad_code;
    gchar *document_number;
    gchar *dtype_code;
    gchar *region_code;
    gint partner_code;
    gint line_number;
    gint cash;
    gint shift;
    gint check;
    // do_context_copy
};


enum {
    PROP_0,
    PROP_KIND,
    PROP_PRODUCT_CODE,
    PROP_SKLAD_CODE,
    PROP_PARTNER_CODE,
    PROP_DTYPE_CODE,
    PROP_REGION_CODE,
    PROP_DOCUMENT_NUMBER,
    PROP_LINE_NUMBER,
    PROP_CASH,
    PROP_SHIFT,
    PROP_CHECK,
};

G_DEFINE_TYPE (DoContext, do_context, G_TYPE_OBJECT)

GType do_context_kind_get_type(void)
{
    static GType etype = 0;
    if (G_UNLIKELY(etype == 0)) {
        static const GEnumValue values[] = {
            { DO_CONTEXT_KIND_UNDEFINE, "DO_CONTEXT_KIND_UNDEFINE", "undefine" },
            { DO_CONTEXT_KIND_PRODUCT, "DO_CONTEXT_KIND_PRODUCT", "product" },
            { DO_CONTEXT_KIND_SKLAD, "DO_CONTEXT_KIND_SKLAD", "sklad" },
            { DO_CONTEXT_KIND_REGION, "DO_CONTEXT_KIND_REGION", "region" },
            { DO_CONTEXT_KIND_PARTNER, "DO_CONTEXT_KIND_PARTNER", "partner" },
            { DO_CONTEXT_KIND_DTYPE, "DO_CONTEXT_KIND_DTYPE", "dtype" },
            { DO_CONTEXT_KIND_DOCUMENT, "DO_CONTEXT_KIND_DOCUMENT", "document" },
            { DO_CONTEXT_KIND_LINE, "DO_CONTEXT_KIND_LINE", "line" },
            { DO_CONTEXT_KIND_CHECK, "DO_CONTEXT_KIND_CHECK", "check" },
            { 0, NULL, NULL }
        };
        etype = g_enum_register_static (g_intern_static_string ("DoContextKind"), values);
    }
    return etype;
}

static void do_context_init (DoContext *context)
{
 //   DoContextPrivate *priv = DO_CONTEXT_GET_PRIVATE (context);
	//memset(priv, 0, sizeof(DoContextPrivate));
}

static void do_context_class_init (DoContextClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->get_property = do_context_get_property;
    object_class->set_property = do_context_set_property;
    object_class->finalize     = do_context_finalize;

// signal
// property

    g_object_class_install_property (object_class,
				   PROP_KIND,
				   g_param_spec_enum ("kind",
						      "Вид контекста",
						      "Вид контекста",
						      DO_TYPE_CONTEXT_KIND,
						      DO_CONTEXT_KIND_UNDEFINE,
						      G_PARAM_READWRITE));


    g_object_class_install_property (object_class,
				   PROP_PRODUCT_CODE,
				   g_param_spec_string ("product-code",
							"Код товара",
							"Код товара",
							NULL,
							G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_SKLAD_CODE,
				   g_param_spec_string ("sklad-code",
							"Код склада",
							"Код склада",
							NULL,
							G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_PARTNER_CODE,
				   g_param_spec_int ("partner-code",
							"Код партнера",
							"Код партера",
						     0,
						     G_MAXINT,
						     0,
						     G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_DTYPE_CODE,
				   g_param_spec_string ("dtype-code",
							"Код типа документа",
							"Код типа документа",
							NULL,
							G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_REGION_CODE,
				   g_param_spec_string ("region-code",
							"Код региона",
							"Код региона",
							NULL,
							G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
				   PROP_DOCUMENT_NUMBER,
				   g_param_spec_string ("document-number",
							"Номер документа",
							"Номер документа",
							NULL,
							G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_LINE_NUMBER,
				   g_param_spec_int ("line-number",
							"Номер строки",
							"Номер строки",
						     0,
						     G_MAXINT,
						     0,
						     G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_CASH,
				   g_param_spec_int ("cash",
							"Номер кассы",
							"Номер кассы",
						     0,
						     G_MAXINT,
						     0,
						     G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_SHIFT,
				   g_param_spec_int ("shift",
							"Номер смены",
							"Номер смены",
						     0,
						     G_MAXINT,
						     0,
						     G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
				   PROP_CHECK,
				   g_param_spec_int ("check",
							"Номер чека",
							"Номер чека",
						     0,
						     G_MAXINT,
						     0,
						     G_PARAM_READWRITE));


    g_type_class_add_private (object_class, sizeof (DoContextPrivate));
}

static void do_context_get_property (GObject     *object,
				guint        param_id,
				GValue      *value,
				GParamSpec  *pspec)
{
    DoContextPrivate *priv = DO_CONTEXT_GET_PRIVATE (object);

    switch (param_id)
    {
    case PROP_KIND:
      g_value_set_enum (value, priv->kind);
      break;
    case PROP_PRODUCT_CODE:
      g_value_set_string (value, priv->product_code);
      break;
    case PROP_SKLAD_CODE:
      g_value_set_string (value, priv->sklad_code);
      break;
    case PROP_PARTNER_CODE:
      g_value_set_int (value, priv->partner_code);
      break;
    case PROP_DTYPE_CODE:
      g_value_set_string (value, priv->dtype_code);
      break;
    case PROP_REGION_CODE:
      g_value_set_string (value, priv->region_code);
      break;
    case PROP_DOCUMENT_NUMBER:
      g_value_set_string (value, priv->document_number);
      break;
    case PROP_LINE_NUMBER:
      g_value_set_int (value, priv->line_number);
      break;
    case PROP_CASH:
      g_value_set_int (value, priv->cash);
      break;
    case PROP_SHIFT:
      g_value_set_int (value, priv->shift);
      break;
    case PROP_CHECK:
      g_value_set_int (value, priv->check);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
    }
}

static void
do_context_set_property (GObject      *object,
				guint         param_id,
				const GValue *value,
				GParamSpec   *pspec)
{
    DoContextPrivate *priv = DO_CONTEXT_GET_PRIVATE (object);

    switch (param_id)
    {
    case PROP_KIND:
      priv->kind = g_value_get_enum (value);
      break;
    case PROP_PRODUCT_CODE:
      if (priv->product_code) g_free(priv->product_code);
      priv->product_code = g_value_dup_string (value);
      break;
    case PROP_SKLAD_CODE:
      if (priv->sklad_code) g_free(priv->sklad_code);
      priv->sklad_code = g_value_dup_string (value);
      break;
    case PROP_PARTNER_CODE:
      priv->partner_code = g_value_get_int (value);
      break;
    case PROP_DTYPE_CODE:
      if (priv->dtype_code) g_free(priv->dtype_code);
      priv->dtype_code = g_value_dup_string (value);
      break;
    case PROP_REGION_CODE:
      if (priv->region_code) g_free(priv->region_code);
      priv->region_code = g_value_dup_string (value);
      break;
    case PROP_DOCUMENT_NUMBER:
      if (priv->document_number) g_free(priv->document_number);
      priv->document_number = g_value_dup_string (value);
      break;
    case PROP_LINE_NUMBER:
      priv->line_number = g_value_get_int(value);
      break;
    case PROP_CASH:
      priv->cash = g_value_get_int(value);
      break;
    case PROP_SHIFT:
      priv->shift = g_value_get_int(value);
      break;
    case PROP_CHECK:
      priv->check = g_value_get_int(value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
    }
}
static void do_context_finalize (GObject          *object)
{
    DoContextPrivate *priv = DO_CONTEXT_GET_PRIVATE (object);

    if (priv->product_code) g_free(priv->product_code);
    if (priv->sklad_code) g_free(priv->sklad_code);
    if (priv->dtype_code) g_free(priv->dtype_code);
    if (priv->region_code) g_free(priv->region_code);
    if (priv->document_number) g_free(priv->document_number);

    G_OBJECT_CLASS (do_context_parent_class)->finalize (object);
}
DoContext *do_context_new()
{
    return g_object_new (DO_TYPE_CONTEXT, NULL);
}

DoContext *do_context_new_with_attributes(DoContextKind kind, const gchar *firt_property_name, ...)
{
    va_list args;
    DoContext* object = g_object_new(DO_TYPE_CONTEXT, "kind", kind, NULL);
    va_start (args, firt_property_name);
    g_object_set_valist (G_OBJECT(object), firt_property_name, args);
    va_end (args);
    return object;
}

DoContextKind do_context_get_kind(DoContext *context)
{
    DoContextPrivate *priv = DO_CONTEXT_GET_PRIVATE (context);
    if (!context)
        return DO_CONTEXT_KIND_UNDEFINE;
    return priv->kind;
}
void do_context_set_kind(DoContext *context, DoContextKind kind)
{
    DoContextPrivate *priv = DO_CONTEXT_GET_PRIVATE (context);
    priv->kind = kind;
}

DoContext * do_context_copy(DoContext *context)
{
    if (!context)
        return NULL;
    DoContext *object = g_object_new (DO_TYPE_CONTEXT, NULL);
    DoContextPrivate *priv = DO_CONTEXT_GET_PRIVATE (context);
    DoContextPrivate *priv_new = DO_CONTEXT_GET_PRIVATE (object);
    priv_new->kind = priv->kind;
    priv_new->product_code = g_strdup(priv->product_code);
    priv_new->sklad_code = g_strdup(priv->sklad_code);
    priv_new->document_number = g_strdup(priv->document_number);
    priv_new->dtype_code = g_strdup(priv->dtype_code);
    priv_new->region_code = g_strdup(priv->region_code);
    priv_new->partner_code = priv->partner_code;
    priv_new->line_number = priv->line_number;
    priv_new->cash = priv->cash;
    priv_new->shift = priv->shift;
    priv_new->check = priv->check;
    return object;
}
gint do_context_compare(DoContext *a, DoContext *b)
{
    DoContextPrivate *priv_a = DO_CONTEXT_GET_PRIVATE (a);
    DoContextPrivate *priv_b = DO_CONTEXT_GET_PRIVATE (b);

    g_return_val_if_fail (priv_a->kind == priv_b->kind, -1);
    g_return_val_if_fail (priv_a->kind != DO_CONTEXT_KIND_UNDEFINE, -1);
    gint retval;

    switch (priv_a->kind) {
        case DO_CONTEXT_KIND_PRODUCT:
            g_return_val_if_fail (priv_a->product_code != NULL &&
                                  priv_b->product_code != NULL, -1);
            retval = g_strcmp0(priv_a->product_code, priv_b->product_code);
            break;
        case DO_CONTEXT_KIND_LINE:
        case DO_CONTEXT_KIND_DOCUMENT:
            g_return_val_if_fail (priv_a->dtype_code != NULL &&
                                  priv_b->dtype_code != NULL &&
                                  priv_a->sklad_code != NULL &&
                                  priv_b->sklad_code != NULL &&
                                  priv_a->document_number != NULL &&
                                  priv_b->document_number != NULL
                                  , -1);
            retval = g_strcmp0(priv_a->dtype_code, priv_b->dtype_code);
            if (retval) break;
            retval = g_strcmp0(priv_a->sklad_code, priv_b->sklad_code);
            if (retval) break;
            retval = g_strcmp0(priv_a->document_number, priv_b->document_number);
            if (retval || priv_a->kind == DO_CONTEXT_KIND_DOCUMENT)
                break;
            retval = priv_a->line_number - priv_b->line_number;
            break;
        case DO_CONTEXT_KIND_REGION:
            g_return_val_if_fail (priv_a->region_code != NULL &&
                                  priv_b->region_code != NULL, -1);
            retval = g_strcmp0(priv_a->region_code, priv_b->region_code);
            break;
        case DO_CONTEXT_KIND_DTYPE:
            g_return_val_if_fail (priv_a->dtype_code != NULL &&
                                  priv_b->dtype_code != NULL, -1);
            retval = g_strcmp0(priv_a->dtype_code, priv_b->dtype_code);
            break;
        case DO_CONTEXT_KIND_PARTNER:
            g_return_val_if_fail (priv_a->region_code != NULL &&
                                  priv_b->region_code != NULL, -1);
            retval = g_strcmp0(priv_a->region_code, priv_b->region_code);
            if (retval) break;
            retval = priv_a->partner_code - priv_b->partner_code;
            break;
        case DO_CONTEXT_KIND_SKLAD:
            g_return_val_if_fail (priv_a->sklad_code != NULL &&
                                  priv_b->sklad_code != NULL, -1);
            retval = g_strcmp0(priv_a->sklad_code, priv_b->sklad_code);
            break;
        default:
            retval = -1;
    }
    return retval;
}
void  do_context_set_document_order_key0(DoContext *context, do_alias_t *alias, document_order_key0_t *document_order_key0)
{
    DoContextPrivate *priv = DO_CONTEXT_GET_PRIVATE (context);

    if (priv->sklad_code) g_free(priv->sklad_code);
    priv->sklad_code = do_text(alias, document_order_key0->sklad);
    if (priv->dtype_code) g_free(priv->dtype_code);
    priv->dtype_code = do_text(alias, document_order_key0->dtype);
    if (priv->document_number) g_free(priv->document_number);
    priv->document_number = do_text(alias, document_order_key0->document);
    priv->line_number = document_order_key0->line;
}
void  do_context_get_document_order_key0(DoContext *context, do_alias_t *alias, document_order_key0_t *document_order_key0)
{
    DoContextPrivate *priv = DO_CONTEXT_GET_PRIVATE (context);

    do_text_set(alias, document_order_key0->dtype, priv->dtype_code);
    do_text_set(alias, document_order_key0->sklad, priv->sklad_code);
    do_text_set(alias, document_order_key0->document, priv->document_number);
    document_order_key0->line = priv->line_number;
}
