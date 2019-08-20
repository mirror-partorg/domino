
#include "do_enum_box.h"
#include "do_enum_value.h"
#include "domino.h"
#include <stdlib.h>

enum
{
	PROP_0,
	PROP_VALUE,
	PROP_DESC,
	PROP_ENUM_NAME,
};

#define DO_ENUM_BOX_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_ENUM_BOX, DoEnumBoxPrivate))
typedef struct _DoEnumBoxItem	    DoEnumBoxItem;

struct _DoEnumBoxPrivate
{
    gchar *default_value;
    GtkWidget *entry;
    gchar *enum_name;
    gboolean uniq;
};

G_DEFINE_TYPE (DoEnumBox, do_enum_box, GTK_TYPE_COMBO_BOX);

static void do_enum_box_init(DoEnumBox *dialog)
{

}

static gboolean do_enum_box_set_active(DoEnumBox *box, const gchar *value)
{
	DoEnumBoxPrivate *priv = DO_ENUM_BOX_GET_PRIVATE (box);
	GtkTreePath *path;
	GtkTreeIter iter;
	gboolean retval;
	GtkTreeModel *model;
	int i;

	path = gtk_tree_path_new_first();
	retval = FALSE;
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(box));
    i = 0;

	while ( gtk_tree_model_get_iter(model, &iter, path) ) {
	    GValue code = {0,};
	    gtk_tree_model_get_value(model, &iter, 0, &code);
	    if ( !strcmp(value, g_value_get_string(&code)) ) {
	        gtk_combo_box_set_active (GTK_COMBO_BOX (box), i);
	        retval = TRUE;
	        break;
	    }
	    i++;
	    gtk_tree_path_next(path);
	}
	gtk_tree_path_free(path);
	if ( !retval && !priv->uniq ) {
	    g_object_set(priv->entry, "text", value, NULL);
	    retval = TRUE;

	}
	return retval;
}
static gboolean do_enum_box_set_active_desc(DoEnumBox *box, const gchar *desc)
{
	DoEnumBoxPrivate *priv = DO_ENUM_BOX_GET_PRIVATE (box);
	GtkTreePath *path;
	GtkTreeIter iter;
	gboolean retval;
	GtkTreeModel *model;
	int i;

	path = gtk_tree_path_new_first();
	retval = FALSE;
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(box));
    i = 0;

	while ( gtk_tree_model_get_iter(model, &iter, path) ) {
	    GValue code = {0,};
	    gtk_tree_model_get_value(model, &iter, 1, &code);
	    if ( !strcmp(desc, g_value_get_string(&code)) ) {
	        gtk_combo_box_set_active (GTK_COMBO_BOX (box), i);
	        retval = TRUE;
	        break;
	    }
	    i++;
	    gtk_tree_path_next(path);
	}
	gtk_tree_path_free(path);
	if ( !retval && !priv->uniq ) {
	    g_object_set(priv->entry, "text", desc, NULL);
	    retval = TRUE;

	}
	return retval;
}

static GObject *do_enum_box_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject   *object;
	DoEnumBoxPrivate *priv;
	gchar *var = NULL;

	object = G_OBJECT_CLASS (do_enum_box_parent_class)->constructor(type, n_construct_properties, construct_params);
    priv = DO_ENUM_BOX_GET_PRIVATE (object);
#if GTK_MAJOR_VERSION == 2
    gtk_combo_box_set_entry_text_column (GTK_COMBO_BOX(object), 1);
#else
    g_object_set(object, "text-column", 1, NULL);
#endif
    priv->entry = gtk_bin_get_child(GTK_BIN (object));
    DOMINO_COMMON_GET_ATTR(priv->enum_name, "variable", &var, NULL);
    if ( var && !strcmp(var, "TRUE") ) {
        priv->uniq = FALSE;
    }
    else {
        priv->uniq = TRUE;
        g_object_set(priv->entry, "editable", FALSE, NULL);
    }
    if ( !priv->default_value ) {
        gtk_combo_box_set_active (GTK_COMBO_BOX (object), 0);
    }
    else
    {
        do_enum_box_set_active(DO_ENUM_BOX(object), priv->default_value);
    }

	return object;
}

static void do_enum_box_finalize (GObject *object)
{
	//DoEnumBoxPrivate *priv = DO_ENUM_BOX_GET_PRIVATE (object);
	G_OBJECT_CLASS (do_enum_box_parent_class)->finalize (object);
}
static gchar *empty_str="";
static void do_enum_box_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoEnumBoxPrivate *priv = DO_ENUM_BOX_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_VALUE: {
            DoEnumValue *res;
            res = do_enum_value_new();
            GtkTreeIter iter;
            if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(object), &iter)) {
                if ( priv->uniq )
                    g_object_set(res, "value", empty_str, NULL);
                else {
                    gchar *text;
                    g_object_get(priv->entry, "text", &text, NULL);
                    g_object_set(res, "value", text, FALSE, NULL);
                }
            }
            else {
                GtkTreeModel *model;
                GValue id = {0,};
                model = gtk_combo_box_get_model(GTK_COMBO_BOX(object));
                gtk_tree_model_get_value(model, &iter, 0, &id);
                g_object_set(res, "value", g_value_get_string(&id), NULL);
            }
            g_value_set_object(value, res);
			break;
        }
        case PROP_DESC: {
            GtkTreeIter iter;
            if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(object), &iter))
                g_value_set_string(value, empty_str);
            else {
                GtkTreeModel *model;
                GValue id = {0,};
                model = gtk_combo_box_get_model(GTK_COMBO_BOX(object));
                gtk_tree_model_get_value(model, &iter, 1, &id);
                g_value_set_string(value, g_value_get_string(&id));

            }
			break;
        }
    }
}
static void do_enum_box_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoEnumBoxPrivate *priv = DO_ENUM_BOX_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_ENUM_NAME: {
            GtkListStore *model;
            model = do_property_enum_create_store(g_value_get_string(value), &priv->default_value);
            gtk_combo_box_set_model(GTK_COMBO_BOX(object), GTK_TREE_MODEL(model));
            priv->enum_name = g_value_dup_string(value);

            break;
        }
        case PROP_VALUE: {
            do_enum_box_set_active(DO_ENUM_BOX(object), do_enum_value_get_string(g_value_get_object(value)));
			break;
        }
        case PROP_DESC:
            do_enum_box_set_active_desc(DO_ENUM_BOX(object), do_enum_value_get_string(g_value_get_object(value)));
			break;
    }
}

static void do_enum_box_class_init (DoEnumBoxClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
    //GtkWidgetClass *widget_class = (GtkWidgetClass*)klass;

	object_class->constructor  = do_enum_box_constructor;
	object_class->finalize     = do_enum_box_finalize;
	object_class->get_property = do_enum_box_get_property;
	object_class->set_property = do_enum_box_set_property;

	g_type_class_add_private (object_class, sizeof (DoEnumBoxPrivate));
    g_object_class_install_property
		(object_class,
		 PROP_VALUE,
		 g_param_spec_object("value",
				     "value",
				     "value",
				      DO_TYPE_ENUM_VALUE,
				      G_PARAM_READWRITE));

    g_object_class_install_property
		(object_class,
		 PROP_ENUM_NAME,
		 g_param_spec_string("enum",
				     "enum",
				     "enum",
				     NULL,
				      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property
		(object_class,
		 PROP_DESC,
		 g_param_spec_string("desc",
				     "value description",
				     "value description",
				     NULL,
				     G_PARAM_READWRITE));

}

GtkWidget *do_enum_box_new(const gchar *enum_name)
{
	return g_object_new (DO_TYPE_ENUM_BOX,
                      "enum", enum_name,
                      "has-entry", TRUE,
			     (gpointer) NULL);
}

