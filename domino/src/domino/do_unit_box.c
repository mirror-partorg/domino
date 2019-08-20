
#include "do_unit_box.h"
#include "domino.h"
#include <stdlib.h>

enum
{
	PROP_0,
	PROP_UNITS,
	PROP_DESC,
};

#define DO_UNIT_BOX_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_UNIT_BOX, DoUnitBoxPrivate))
typedef struct _DoUnitBoxItem	    DoUnitBoxItem;

struct _DoUnitBoxPrivate
{
    GtkWidget *entry;
};

G_DEFINE_TYPE (DoUnitBox, do_unit_box, GTK_TYPE_COMBO_BOX);

static void do_unit_box_init(DoUnitBox *dialog)
{

}
static GtkTreeModel *create_model()
{
    GtkListStore *store;
    do_alias_t *alias;
    store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

    alias = domino_alias_new(NULL);
    if ( !do_alias_open(alias, TRUE) )
        return NULL;

    do_list_t *list;
    int i;

    list = do_city_get_cities(alias);
    for ( i = 0; i < list->count; i++ ) {
        char *name;
        char *units;
        GtkTreeIter iter;
        do_string_list_t *u;

        name = do_city_get_name(alias, DOPOINTER_TO_INT(list->list[i]));
        u = do_city_get_units(alias, DOPOINTER_TO_INT(list->list[i]));
        units = do_string_list_to_csv(u);

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, units, 1, name, -1);
        do_free(name);do_free(units);
        do_string_list_free(u);
    }
    do_list_free(list);
    list = do_firm_get_firms(alias);

    for ( i = 0; i < list->count; i++ ) {
        char *name;
        char *units;
        GtkTreeIter iter;
        do_string_list_t *u;

        u = do_firm_get_units(alias, DOPOINTER_TO_INT(list->list[i]));
        if ( u->count < 2 )
            continue;
        units = do_string_list_to_csv(u);
        name = do_firm_get_name(alias, DOPOINTER_TO_INT(list->list[i]));

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, units, 1, name, -1);
        do_free(name);do_free(units);
        do_string_list_free(u);
    }


    do_list_free(list);
    char *units,*u1,*u2;
    GtkTreeIter iter;
    do_string_list_t *u;

    u = do_firm_get_units(alias, 4);
    u1 = do_string_list_to_csv(u);
    u = do_firm_get_units(alias,5);
    u2 = do_string_list_to_csv(u);
    units = g_strdup_printf("%s,%s",u1,u2);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, units, 1, "Тазеева+Черемухина", -1);

    u = do_get_units(alias);
    units = do_string_list_to_csv(u);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, units, 1, "Все", -1);


    do_alias_free(alias);
    return GTK_TREE_MODEL(store);
}

static gboolean do_unit_box_set_active(DoUnitBox *box, const gchar *value)
{
	DoUnitBoxPrivate *priv = DO_UNIT_BOX_GET_PRIVATE (box);
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
	if ( !retval ) {
	    g_object_set(priv->entry, "text", value, NULL);
	    retval = TRUE;

	}
	return retval;
}
static gboolean do_unit_box_set_active_desc(DoUnitBox *box, const gchar *desc)
{
	DoUnitBoxPrivate *priv = DO_UNIT_BOX_GET_PRIVATE (box);
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
	if ( !retval  ) {
	    g_object_set(priv->entry, "text", desc, NULL);
	    retval = TRUE;

	}
	return retval;
}

static GObject *do_unit_box_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject   *object;
	DoUnitBoxPrivate *priv;

	object = G_OBJECT_CLASS (do_unit_box_parent_class)->constructor(type, n_construct_properties, construct_params);
    priv = DO_UNIT_BOX_GET_PRIVATE (object);
    //g_object_set(object, "text-column", 1, NULL);
    gtk_combo_box_set_entry_text_column (GTK_COMBO_BOX(object), 1);
    priv->entry = gtk_bin_get_child(GTK_BIN (object));
    gtk_combo_box_set_model(GTK_COMBO_BOX(object), create_model());

    return object;
}

static void do_unit_box_finalize (GObject *object)
{
	//DoUnitBoxPrivate *priv = DO_UNIT_BOX_GET_PRIVATE (object);
	G_OBJECT_CLASS (do_unit_box_parent_class)->finalize (object);
}
static gchar *empty_str="";
static void do_unit_box_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoUnitBoxPrivate *priv = DO_UNIT_BOX_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_UNITS: {
            GtkTreeIter iter;
            if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(object), &iter)) {
                gchar *text;
                g_object_get(priv->entry, "text", &text, NULL);
                g_value_set_string(value, text);
            }
            else {
                GtkTreeModel *model;
                GValue id = {0,};
                model = gtk_combo_box_get_model(GTK_COMBO_BOX(object));
                gtk_tree_model_get_value(model, &iter, 0, &id);
                g_value_set_string(value, g_value_get_string(&id));
            }
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
static void do_unit_box_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	//DoUnitBoxPrivate *priv = DO_UNIT_BOX_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_UNITS: {
            do_unit_box_set_active(DO_UNIT_BOX(object), g_value_get_string(value));
			break;
        }
        case PROP_DESC:
            do_unit_box_set_active_desc(DO_UNIT_BOX(object), g_value_get_string(value));
			break;
    }
}

static void do_unit_box_class_init (DoUnitBoxClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
    //GtkWidgetClass *widget_class = (GtkWidgetClass*)klass;

	object_class->constructor  = do_unit_box_constructor;
	object_class->finalize     = do_unit_box_finalize;
	object_class->get_property = do_unit_box_get_property;
	object_class->set_property = do_unit_box_set_property;

	g_type_class_add_private (object_class, sizeof (DoUnitBoxPrivate));

    g_object_class_install_property
		(object_class,
		 PROP_UNITS,
		 g_param_spec_string("units",
				     "units",
				     "units",
				     NULL,
				      G_PARAM_READWRITE));

	g_object_class_install_property
		(object_class,
		 PROP_DESC,
		 g_param_spec_string("desc",
				     "value description",
				     "value description",
				     NULL,
				     G_PARAM_READWRITE));

}

GtkWidget *do_unit_box_new()
{
	return g_object_new (DO_TYPE_UNIT_BOX,
                      "has-entry", TRUE,
			     (gpointer) NULL);
}

