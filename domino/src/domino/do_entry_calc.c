

#include "do_entry_calc.h"
#include "do_entry.h"
#include "do_icons.h"
#include "do_validate.h"
#include "../misc/simple_expr.h"
#include <stdlib.h>
#include <string.h>

enum
{
	PROP_0,
    PROP_VALUE,
    PROP_MAX_VALUE,
    PROP_MIN_VALUE,
    PROP_PRECISION,
};

#define DO_ENTRY_CALC_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_ENTRY_CALC, DoEntryCalcPrivate))

struct _DoEntryCalcPrivate
{
    gdouble value;
    gdouble max_value;
    gdouble min_value;
    gboolean max_value_set;
    gboolean min_value_set;
    gint precision;

	GtkListStore *store;
};
#define DEFAULT_PRECISION 3

static void      do_entry_calc_validate_init (DoValidateIface *iface);
static void      do_entry_calc_completion (DoValidate *iface);
static gboolean  do_entry_calc_valid(DoValidate *iface);
static gboolean  do_entry_calc_substitution(DoValidate *iface);

G_DEFINE_TYPE_WITH_CODE (DoEntryCalc, do_entry_calc, DO_TYPE_ENTRY,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VALIDATE,
						do_entry_calc_validate_init));

static void do_entry_calc_validate_init (DoValidateIface *iface)
{
    iface->valid = do_entry_calc_valid;
    iface->completion = do_entry_calc_completion;
    iface->substitution = do_entry_calc_substitution;
}

static void do_entry_calc_init(DoEntryCalc *dialog)
{
    DoEntryCalcPrivate *priv = DO_ENTRY_CALC_GET_PRIVATE (dialog);
    if (!priv->precision)
        priv->precision = DEFAULT_PRECISION;
}
static gboolean match_func(GtkEntryCompletion *completion,
                                                         const gchar *key,
                                                         GtkTreeIter *iter,
                                                         gpointer user_data)
{
    return TRUE;
}

static gboolean do_entry_calc_valid(DoValidate *iface)
{

    GtkEntry *entry = GTK_ENTRY(iface);
	DoEntryCalcPrivate *priv = DO_ENTRY_CALC_GET_PRIVATE (iface);
    int error = 0;
    double value;
    gboolean retval = FALSE;

    value = simple_expr_calc(gtk_entry_get_text(entry), &error);
    if (!error &&
         value == simple_expr_tof(gtk_entry_get_text(entry)) &&
         ((priv->max_value_set && value <= priv->max_value) || (!priv->max_value_set)) &&
         ((priv->min_value_set && value >= priv->min_value) || (!priv->min_value_set))
       ) {
        priv->value = value;
        retval = TRUE;
    }
    return retval;
}

static GObject *do_entry_calc_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;
	GtkEntry *entry;
	GtkEntryCompletion *completion;
	DoEntryCalcPrivate *priv;
	GtkListStore *store;

	object = G_OBJECT_CLASS (do_entry_calc_parent_class)->constructor(type, n_construct_properties, construct_params);

	entry = GTK_ENTRY(object);
    priv = DO_ENTRY_CALC_GET_PRIVATE (object);

    completion = gtk_entry_get_completion(entry);
    gtk_entry_completion_set_match_func(completion, match_func, NULL, NULL);

    store = priv->store = gtk_list_store_new (1, G_TYPE_STRING);
    gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(store));
    g_object_unref (store);

    /* Use model column 0 as the text column */
    gtk_entry_completion_set_text_column (completion, 0);
	return object;
}

static void do_entry_calc_finalize (GObject *object)
{
//	DoEntryCalcPrivate *priv = DO_ENTRY_CALC_GET_PRIVATE (object);
	G_OBJECT_CLASS (do_entry_calc_parent_class)->finalize (object);
}
static void set_value(DoEntryCalc *calc, gdouble value)
{
	DoEntryCalcPrivate *priv = DO_ENTRY_CALC_GET_PRIVATE (calc);
	gchar *buf;

	priv->value = value;
    buf = g_strdup_printf("%.*f", priv->precision, value);
	gtk_entry_set_text(GTK_ENTRY(calc), buf);
	g_free(buf);
}

static void do_entry_calc_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoEntryCalcPrivate *priv = DO_ENTRY_CALC_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_VALUE:
            set_value(DO_ENTRY_CALC(object), g_value_get_double(value));
			break;
        case PROP_MAX_VALUE:
            priv->max_value = g_value_get_double (value);
            priv->max_value_set = TRUE;
            break;
        case PROP_MIN_VALUE:
            priv->min_value = g_value_get_double (value);
            priv->min_value_set = TRUE;
            break;
        case PROP_PRECISION:
            priv->precision = g_value_get_int (value);
            break;
	}
}
static void do_entry_calc_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoEntryCalcPrivate *priv = DO_ENTRY_CALC_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_VALUE:
			g_value_set_double (value, priv->value);
			break;
        case PROP_MAX_VALUE:
            g_value_set_double (value, priv->max_value);
            break;
        case PROP_MIN_VALUE:
            g_value_set_double (value, priv->min_value);
            break;
        case PROP_PRECISION:
            g_value_set_int (value, priv->precision);
            break;
    }
}

static void do_entry_calc_class_init (DoEntryCalcClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
    //GtkWidgetClass *widget_class = (GtkWidgetClass*)klass;

	object_class->constructor  = do_entry_calc_constructor;
	object_class->finalize     = do_entry_calc_finalize;
	object_class->get_property = do_entry_calc_get_property;
	object_class->set_property = do_entry_calc_set_property;

	g_type_class_add_private (object_class, sizeof (DoEntryCalcPrivate));



    g_object_class_install_property (object_class,
                                   PROP_VALUE,
                                   g_param_spec_double ("value",
							"Значение",
							"Чтение или запись текущего значения",
							-G_MAXDOUBLE,
							G_MAXDOUBLE,
							0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_MAX_VALUE,
                                   g_param_spec_double ("max-value",
							"Максимальное значение",
							"Максимально доступное значение",
							-G_MAXDOUBLE,
							G_MAXDOUBLE,
							0.0,
							G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_MIN_VALUE,
                                   g_param_spec_double ("min-value",
							"Минимальное значение",
							"Минимально доступное значение",
							-G_MAXDOUBLE,
							G_MAXDOUBLE,
							0.0,
							G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_PRECISION,
                                   g_param_spec_int ("precision",
							"Точность",
							"Количество знаков после запятой",
							0,
							10,
							DEFAULT_PRECISION,
							G_PARAM_READWRITE));
}

GtkWidget *do_entry_calc_new()
{
	return g_object_new (DO_TYPE_ENTRY_CALC,
			     (gpointer) NULL);
}

static void do_entry_calc_completion (DoValidate *iface)
{
	DoEntryCalcPrivate *priv = DO_ENTRY_CALC_GET_PRIVATE (iface);
	DoEntryCalc *entry = DO_ENTRY_CALC(iface);
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    double value;
    int error = 0;
    gtk_list_store_clear(priv->store);
    value = simple_expr_calc(text, &error);
    if (!error && value != simple_expr_tof(text)) {
        GtkTreeIter iter;
        gchar *buf;
        buf = g_strdup_printf("%.*f", priv->precision, value);
        gtk_list_store_append (priv->store, &iter);
        gtk_list_store_set (priv->store, &iter, 0, buf, -1);
        g_free(buf);
    }
}
static gboolean do_entry_calc_substitution (DoValidate *iface)
{
	DoEntryCalcPrivate *priv = DO_ENTRY_CALC_GET_PRIVATE (iface);
	GtkEntry *entry = GTK_ENTRY(iface);
	const gchar *text = gtk_entry_get_text(entry);
	GtkTreePath *path;
	GtkTreeIter  iter;
	GValue  value = {0,};
	gint count = 0;
	gboolean retval = TRUE;

	do_entry_calc_completion(iface);

    path = gtk_tree_path_new_first();
    while (count < 3 &&  gtk_tree_model_get_iter(GTK_TREE_MODEL(priv->store), &iter, path)) {
        if (!count)
            gtk_tree_model_get_value(GTK_TREE_MODEL(priv->store), &iter, 0, &value);
        count++;
        gtk_tree_path_next(path);
    }
    if (count == 1 &&
        strcmp(g_value_get_string(&value), text)
        ) {
        gtk_entry_set_text(entry, g_value_get_string(&value));
        retval = FALSE;
    }
    gtk_tree_path_free(path);
    return retval;
}
