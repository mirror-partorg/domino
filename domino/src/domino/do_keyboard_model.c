

#include "do_keyboard_model.h"
#include <domino.h>
#include <dolib.h>
#ifdef ARTIX
#include <artix.h>
#endif
#include <string.h>
#include <gtk/gtk.h>


#define DO_KEYBOARD_MODEL_GET_PRIVATE(object) (DO_KEYBOARD_MODEL(object)->priv)

typedef struct _DoKeyboardRecord   DoKeyboardRecord;

#define MODEL_DB  DO_DB_DOCUMENT, DO_DB_PRODUCT , DO_DB_DOCUMENT_TYPE, DO_DB_DOCUMENT_ORDER, DO_DB_PROTOCOL, DO_DB_END


// boring declarations of local functions
static void do_keyboard_model_init(DoKeyboardModel *pkg_tree);
static void do_keyboard_model_class_init(DoKeyboardModelClass *klass);
static void do_keyboard_model_tree_model_init(GtkTreeModelIface *iface);
static void do_keyboard_model_finalize(GObject *object);
static void do_keyboard_model_drag_source_init(GtkTreeDragSourceIface*iface);
static void do_keyboard_model_drag_dest_init(GtkTreeDragDestIface   *iface);

static GObject *do_keyboard_model_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params);
static GtkTreeModelFlags do_keyboard_model_get_flags(GtkTreeModel *tree_model);
static gint do_keyboard_model_get_n_columns(GtkTreeModel *tree_model);
static GType do_keyboard_model_get_column_type(GtkTreeModel *tree_model, gint index);
static gboolean do_keyboard_model_get_iter(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path);
static GtkTreePath *do_keyboard_model_get_path(GtkTreeModel *tree_model, GtkTreeIter *iter);
static void do_keyboard_model_get_value(GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value);
static gboolean do_keyboard_model_iter_next(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean do_keyboard_model_iter_children(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent);
static gboolean do_keyboard_model_iter_has_child(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gint do_keyboard_model_iter_n_children(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean do_keyboard_model_iter_nth_child(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n);
static gboolean do_keyboard_model_iter_parent(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child);

// Drag and Drop
static gboolean do_keyboard_model_drag_data_delete(GtkTreeDragSource *drag_source, GtkTreePath *path);
static gboolean do_keyboard_model_drag_data_get(GtkTreeDragSource *drag_source, GtkTreePath *path, GtkSelectionData *selection_data);
static gboolean do_keyboard_model_drag_row_draggable(GtkTreeDragSource *drag_source, GtkTreePath *path);
static gboolean do_keyboard_model_drag_data_received(GtkTreeDragDest *drag_dest, GtkTreePath *dest, GtkSelectionData *selection_data);
static gboolean do_keyboard_model_row_drop_possible(GtkTreeDragDest *drag_dest, GtkTreePath *dest_path, GtkSelectionData *selection_data);

static gboolean load_tree(DoKeyboardModel *model);
static DoKeyboardRecord *load_document(DoKeyboardModel *model, do_alias_t *alias, document_rec_t *document);

static void free_record(DoKeyboardRecord *record);
static void delete_record(DoKeyboardModel *model, DoKeyboardRecord *record);
static void normalize_record(DoKeyboardRecord *record);
static void normalize_tree(DoKeyboardModel *model);

// Edit data
static gboolean delete_document(do_alias_t *alias, gint number);
static gboolean delete_content(do_alias_t *alias, do_sort_list_t *list, gint number, gint line);
static gint insert_after_content(do_alias_t *alias, do_sort_list_t *list, gint number, gint line, const gchar *product_code);

// Document type change
static gboolean document_bcode_to_list(do_alias_t *alias, do_sort_list_t *list, gint number);
static gboolean document_list_to_bcode(do_alias_t *alias, do_sort_list_t *list, gint number);
static gboolean document_to_bcode(do_alias_t *alias, gint number, const gchar *product_code);

enum
{
	PROP_0,
	PROP_ALIAS,
	PROP_SORT_LIST,
};


struct _DoKeyboardRecord
{
    gint  kind;

    gint  number;
    gchar *code;
    gboolean childless;

    gchar *desc;
    gchar *label;

    DoKeyboardRecord *parent;
    do_sort_list_t *children;

    gint   indx;
};

struct _DoKeyboardModelPrivate
{
    do_alias_t    *alias;
    do_sort_list_t   *keys;
    gint           stamp;
    gchar         *alias_name;
    GList         *do_sort_list;
};

G_DEFINE_TYPE_WITH_CODE (DoKeyboardModel, do_keyboard_model, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL,
						do_keyboard_model_tree_model_init)
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_DRAG_SOURCE,
						do_keyboard_model_drag_source_init)
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_DRAG_DEST,
						do_keyboard_model_drag_dest_init));


static void do_sort_list_create(DoKeyboardModel *model, GList *list)
{
	DoKeyboardModelPrivate *priv = DO_KEYBOARD_MODEL_GET_PRIVATE (model);
    GList *l;
    for (l = list; l; l = l->next) {
        gint *i = g_malloc(sizeof(gint));
        *i = *((gint*)l->data);
        priv->do_sort_list = g_list_append(priv->do_sort_list, i);
    }
}

static void do_keyboard_model_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoKeyboardModelPrivate *priv = DO_KEYBOARD_MODEL_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string(value);
            break;
		case PROP_SORT_LIST:
            if (!priv->do_sort_list)
                do_sort_list_create(DO_KEYBOARD_MODEL(object), g_value_get_pointer(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_keyboard_model_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoKeyboardModelPrivate *priv = DO_KEYBOARD_MODEL_GET_PRIVATE (object);
    switch (prop_id)
    {
        case PROP_ALIAS:
            g_value_set_string (value, priv->alias_name);
            break;
        case PROP_SORT_LIST:
            g_value_set_pointer (value, priv->do_sort_list);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}


static void do_keyboard_model_class_init(DoKeyboardModelClass *klass)
{
    GObjectClass   *o_class = G_OBJECT_CLASS (klass);

    o_class->finalize = do_keyboard_model_finalize;
	o_class->constructor  = do_keyboard_model_constructor;
	o_class->get_property = do_keyboard_model_get_property;
	o_class->set_property = do_keyboard_model_set_property;

	g_type_class_add_private (o_class, sizeof (DoKeyboardModelPrivate));

	g_object_class_install_property
		(o_class,
		 PROP_ALIAS,
		 g_param_spec_string ("alias-name",
				     "Алиас БД",
				     "Алиас БД",
				     NULL,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property
		(o_class,
		 PROP_SORT_LIST,
		 g_param_spec_pointer ("sort-list",
				     "список порядока сортировка",
				     "список порядока сортировка list_t (*int)",
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));
}

static void do_keyboard_model_tree_model_init (GtkTreeModelIface *iface)
{
    iface->get_flags       = do_keyboard_model_get_flags;
    iface->get_n_columns   = do_keyboard_model_get_n_columns;
    iface->get_column_type = do_keyboard_model_get_column_type;
    iface->get_iter        = do_keyboard_model_get_iter;
    iface->get_path        = do_keyboard_model_get_path;
    iface->get_value       = do_keyboard_model_get_value;
    iface->iter_next       = do_keyboard_model_iter_next;
    iface->iter_children   = do_keyboard_model_iter_children;
    iface->iter_has_child  = do_keyboard_model_iter_has_child;
    iface->iter_n_children = do_keyboard_model_iter_n_children;
    iface->iter_nth_child  = do_keyboard_model_iter_nth_child;
    iface->iter_parent     = do_keyboard_model_iter_parent;
}

static void do_keyboard_model_init (DoKeyboardModel *keyboard_model)
{
	DoKeyboardModelPrivate *priv;
	priv = keyboard_model->priv = do_malloc(sizeof(DoKeyboardModelPrivate));
	memset(priv, 0, sizeof(DoKeyboardModelPrivate));
    priv->stamp = g_random_int(); // Random int to check whether iters belong to out model
}
static void do_keyboard_model_drag_source_init (GtkTreeDragSourceIface *iface)
{
    iface->row_draggable = do_keyboard_model_drag_row_draggable;
    iface->drag_data_delete = do_keyboard_model_drag_data_delete;
    iface->drag_data_get = do_keyboard_model_drag_data_get;
}

static void do_keyboard_model_drag_dest_init(GtkTreeDragDestIface *iface)
{
    iface->drag_data_received = do_keyboard_model_drag_data_received;
    iface->row_drop_possible = do_keyboard_model_row_drop_possible;
}

static int cmp_record(void *rec, void *rec1, void *user_data)
{
	DoKeyboardModelPrivate *priv = user_data;
    if (!priv || !priv->do_sort_list)
        return ((DoKeyboardRecord*)rec)->number - ((DoKeyboardRecord*)rec1)->number;
    else {
        gint indx1 = -1 , indx2 = -1, indx;
        GList *l;
        for (l = priv->do_sort_list, indx = 0; l && (indx1 == -1 || indx2 == -1 ); l = l->next, indx++) {
            gint *number = l->data;
            if (((DoKeyboardRecord*)rec)->number == *number) {
                indx1 = indx;
            }
            if (((DoKeyboardRecord*)rec1)->number == *number) {
                indx2 = indx;
            }
        }
        if (indx1 == -1 && indx2 == -1)
            return ((DoKeyboardRecord*)rec)->number - ((DoKeyboardRecord*)rec1)->number;
        if (indx1 == -1)
            return 1;
        if (indx2 == -1)
            return -1;
        return indx1 - indx2;
    }
}

static GObject *do_keyboard_model_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject                *object;
	DoKeyboardModel        *model;
	DoKeyboardModelPrivate *priv;

	object   = G_OBJECT_CLASS (do_keyboard_model_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv     = DO_KEYBOARD_MODEL_GET_PRIVATE(object);
	model    = DO_KEYBOARD_MODEL(object);

    priv->alias = domino_alias_new(priv->alias_name);
    priv->keys = do_sort_list_new(0, 0, cmp_record, priv);

    if (priv->alias) {
        do_alias_open(priv->alias, TRUE);
        load_tree(model);
    }
    return object;
}

static void do_keyboard_model_finalize(GObject *object)
{
    DoKeyboardModelPrivate *priv = DO_KEYBOARD_MODEL_GET_PRIVATE(object);
    int i;
    for (i = 0; i < priv->keys->count; i++)
        free_record(priv->keys->list[i]);
    do_sort_list_free(priv->keys);
    do_alias_free(priv->alias);
	if (priv->alias_name)
		g_free(priv->alias_name);
    if (priv->do_sort_list) {
        g_list_foreach(priv->do_sort_list, (GFunc)g_free, NULL);
        g_list_free(priv->do_sort_list);
    }
	G_OBJECT_CLASS (do_keyboard_model_parent_class)->finalize (object);
}

static GtkTreeModelFlags do_keyboard_model_get_flags(GtkTreeModel *tree_model)
{
    g_return_val_if_fail (DO_IS_KEYBOARD_MODEL(tree_model), (GtkTreeModelFlags)0);

    return (GTK_TREE_MODEL_ITERS_PERSIST);
}

static gint do_keyboard_model_get_n_columns(GtkTreeModel *tree_model)
{
    g_return_val_if_fail (DO_IS_KEYBOARD_MODEL(tree_model), 0);

    return DO_KEYBOARD_MODEL_N_COLUMNS;
}

static GType do_keyboard_model_get_column_type(GtkTreeModel *tree_model, gint index)
{
    g_return_val_if_fail (DO_IS_KEYBOARD_MODEL(tree_model), G_TYPE_INVALID);
    g_return_val_if_fail (index < DO_KEYBOARD_MODEL_N_COLUMNS && index >= 0, G_TYPE_INVALID);
    switch (index) {

        case DO_KEYBOARD_MODEL_COL_RECORD:
            return G_TYPE_POINTER;
        case DO_KEYBOARD_MODEL_COL_KIND:
            return G_TYPE_INT;
        case DO_KEYBOARD_MODEL_COL_NUMBER:
            return G_TYPE_INT;
        case DO_KEYBOARD_MODEL_COL_DESC:
            return G_TYPE_STRING;
        case DO_KEYBOARD_MODEL_COL_LABEL:
            return G_TYPE_STRING;
        case DO_KEYBOARD_MODEL_COL_CODE:
            return G_TYPE_STRING;
        case DO_KEYBOARD_MODEL_COL_CHILDLESS:
            return G_TYPE_BOOLEAN;
        default:
            return G_TYPE_INVALID;
    }
}

static gboolean do_keyboard_model_get_iter (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path)
{
    g_assert(path != NULL);

    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(tree_model);
    DoKeyboardRecord  *record = NULL;
    gint              *indices, n, depth, i;

    indices = gtk_tree_path_get_indices(path);
    depth   = gtk_tree_path_get_depth(path);

    g_assert(depth > 0 && depth <= DO_KEYBOARD_MODEL_DEPTH_MAX); // depth 1 = top level; a model only has top level nodes and no children

    do_sort_list_t *crnt_root = NULL;

    for (i = 0; i < depth; i++) {
        n = indices[i];
        if (crnt_root)
            crnt_root = record->children;
        else
            crnt_root = priv->keys;

        if (!crnt_root || n < 0 || n >= crnt_root->count)
            return FALSE;
        record = (DoKeyboardRecord*)(crnt_root->list[n]);
    }

    g_assert(record != NULL);
    g_assert(record->indx == n);

    iter->stamp      = priv->stamp;
    iter->user_data  = record;
    iter->user_data2 = NULL;
    iter->user_data3 = NULL;

    return TRUE;
}
static GtkTreePath *get_path_from_record(DoKeyboardRecord *record)
{
    GtkTreePath *path = gtk_tree_path_new();
    DoKeyboardRecord *crnt = record;
    while (crnt) {
        gtk_tree_path_prepend_index(path, crnt->indx);
        crnt = crnt->parent;
    }
    return path;
}
static GtkTreePath *do_keyboard_model_get_path(GtkTreeModel *tree_model, GtkTreeIter  *iter)
{
    DoKeyboardRecord *record;
    //DoKeyboardModel   *do_keyboard_model;

    g_return_val_if_fail (DO_IS_KEYBOARD_MODEL(tree_model), NULL);
    g_return_val_if_fail (iter != NULL,               NULL);
    g_return_val_if_fail (iter->user_data != NULL,    NULL);

    //do_keyboard_model = DO_KEYBOARD_MODEL(tree_model);

    record = (DoKeyboardRecord*) iter->user_data;

    return get_path_from_record(record);
}

static void do_keyboard_model_get_value(GtkTreeModel *tree_model, GtkTreeIter  *iter, gint column, GValue *value)
{
    DoKeyboardRecord  *record;
//    DoKeyboardModel   *do_keyboard_model;

    g_return_if_fail (DO_IS_KEYBOARD_MODEL (tree_model));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (column < DO_KEYBOARD_MODEL_N_COLUMNS);

    g_value_init (value, do_keyboard_model_get_column_type(tree_model, column));

    //do_keyboard_model = DO_KEYBOARD_MODEL(tree_model);

    record = (DoKeyboardRecord*) iter->user_data;

    g_return_if_fail ( record != NULL );

//!!    if(record->indx >= do_keyboard_model->keys->count)
    //    g_return_if_reached();

    switch(column)    {
        case DO_KEYBOARD_MODEL_COL_RECORD:
            g_value_set_pointer(value, record); break;
        case DO_KEYBOARD_MODEL_COL_KIND:
            g_value_set_int(value, record->kind); break;
        case DO_KEYBOARD_MODEL_COL_NUMBER:
            g_value_set_int(value, record->number); break;
        case DO_KEYBOARD_MODEL_COL_DESC:
            g_value_set_string(value, record->desc); break;
        case DO_KEYBOARD_MODEL_COL_LABEL:
            g_value_set_string(value, record->label); break;
        case DO_KEYBOARD_MODEL_COL_CODE:
            g_value_set_string(value, record->code); break;
        case DO_KEYBOARD_MODEL_COL_CHILDLESS:
            g_value_set_boolean(value, record->childless); break;
    }
}

static gboolean do_keyboard_model_iter_next(GtkTreeModel  *tree_model, GtkTreeIter   *iter)
{
    DoKeyboardRecord *record, *nextrecord;
//!!    DoKeyboardModel  *model;
    do_sort_list_t       *keys;
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(tree_model);

    g_return_val_if_fail (DO_IS_KEYBOARD_MODEL (tree_model), FALSE);

    if (iter == NULL || iter->user_data == NULL)
        return FALSE;

    record = (DoKeyboardRecord *) iter->user_data;
    if (record->parent)
        keys = record->parent->children;
    else
        keys = priv->keys;

    if (!keys || (record->indx + 1) >= keys->count)
        return FALSE;

    nextrecord = keys->list[(record->indx + 1)];

    g_assert ( nextrecord != NULL );
    g_assert ( nextrecord->indx == (record->indx + 1) );

    iter->stamp     = priv->stamp;
    iter->user_data = nextrecord;

    return TRUE;
}

static gboolean do_keyboard_model_iter_children(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent)
{
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(tree_model);
    DoKeyboardRecord *record;

    g_return_val_if_fail ( parent != NULL && parent->user_data != NULL, FALSE);

    g_return_val_if_fail (DO_IS_KEYBOARD_MODEL (tree_model), FALSE);

    record = parent->user_data;

    if (!record->children || record->children->count == 0)
        return FALSE;

    iter->stamp     = priv->stamp;
    iter->user_data = record->children->list[0];

    return TRUE;
}

static gboolean do_keyboard_model_iter_has_child (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    DoKeyboardRecord *record;
    g_return_val_if_fail (DO_IS_KEYBOARD_MODEL (tree_model), FALSE);
    if (iter == NULL || iter->user_data == NULL)
        return FALSE;
    record = (DoKeyboardRecord *) iter->user_data;
    return (record->children && record->children->count > 0);
}

static gint do_keyboard_model_iter_n_children (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(tree_model);
    DoKeyboardRecord *record;
    g_return_val_if_fail (DO_IS_KEYBOARD_MODEL (tree_model), FALSE);
    if (iter == NULL || iter->user_data == NULL)
        return priv->keys->count;

    record = (DoKeyboardRecord *) iter->user_data;
    if (!record->children)
        return 0;
    return record->children->count;
}

static gboolean do_keyboard_model_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n)
{
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(tree_model);
    DoKeyboardRecord  *record;
    do_sort_list_t *keys;

    g_return_val_if_fail (DO_IS_KEYBOARD_MODEL (tree_model), FALSE);

    if (parent)
        keys = ((DoKeyboardRecord*)parent->user_data)->children;
    else
        keys = priv->keys;

    if( n >= keys->count )
        return FALSE;

    record = keys->list[n];

    g_assert( record != NULL );
    g_assert( record->indx == n );

    iter->stamp = priv->stamp;
    iter->user_data = record;

    return TRUE;
}

static gboolean do_keyboard_model_iter_parent (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child)
{
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(tree_model);

    if (child == NULL || child->user_data == NULL)
        return FALSE;

    DoKeyboardRecord  *record = child->user_data;
    if (!record->parent)
        return FALSE;

    iter->stamp = priv->stamp;
    iter->user_data = record->parent;

    return TRUE;
}

GtkTreeModel *do_keyboard_model_new (const gchar *alias_name, GList *do_sort_list)
{
	return g_object_new (DO_KEYBOARD_MODEL_TYPE,
	                     "alias-name", alias_name,
	                     "sort-list", do_sort_list,
	                     (gpointer) NULL);
}

gboolean do_keyboard_model_remove(DoKeyboardModel *model, GtkTreePath  *path)
{
    GtkTreeIter  iter;
    DoKeyboardRecord *record;
    gboolean retval = FALSE;
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(model);
    do_alias_t *alias = priv->alias;

    if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path))
        return FALSE;

    record = (DoKeyboardRecord*) iter.user_data;
    if (record->kind == DO_KEYBOARD_MODEL_KIND_KEY)
        retval = delete_document(alias, record->number);
    else
        retval = delete_content(alias, record->parent->children, record->parent->number, record->number);
    if (retval) {
        gtk_tree_model_row_deleted(GTK_TREE_MODEL(model), path);
        if (record->parent) {
            do_sort_list_delete(record->parent->children, record->indx);
            normalize_record(record->parent);
        }
        else {
            do_sort_list_delete(priv->keys, record->indx);
            normalize_tree(model);
        }
        free_record(record);
    }
    return retval;
}

gboolean do_keyboard_model_remove_from_list(DoKeyboardModel *model, GList *list)
{
    GList *l, *list_key = NULL;

    for(l = list; l; l = l->next) {
        GtkTreePath * path = l->data;
        switch (gtk_tree_path_get_depth(path)) {
            case DO_KEYBOARD_MODEL_DEPTH_KEY:
            case DO_KEYBOARD_MODEL_DEPTH_CONTENT:
                list_key = g_list_append(list_key, gtk_tree_row_reference_new(GTK_TREE_MODEL(model), path));
                break;
        }
    }
    gboolean retval = TRUE;
    GtkTreeIter iter;
    DoKeyboardRecord *record;
    do_alias_t *alias = DO_KEYBOARD_MODEL_GET_PRIVATE(model)->alias;

    for(l = list_key; l && retval; l = l->next) {
        GtkTreePath *path = gtk_tree_row_reference_get_path(l->data);
        gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path);
        record = iter.user_data;
        switch (record->kind) {
            case DO_KEYBOARD_MODEL_KIND_KEY:
                retval = delete_document(alias, record->number);
                break;
            case DO_KEYBOARD_MODEL_KIND_CONTENT:
                retval = record->parent && delete_content(alias, record->parent->children, record->parent->number, record->number);
                break;
        }
        if (retval) {
            delete_record(model, record);
            gtk_tree_model_row_deleted(GTK_TREE_MODEL(model), path);
        }
        gtk_tree_path_free(path);
        gtk_tree_row_reference_free(l->data);
    }
    g_list_free(list_key);
    normalize_tree(model);
    return retval;
}


// Drag and Drop
static gboolean do_keyboard_model_drag_row_draggable (GtkTreeDragSource *drag_source,
                                                   GtkTreePath       *path)
{
/*    gint              *indices, n, depth, i;

    indices = gtk_tree_path_get_indices(path);
    depth   = gtk_tree_path_get_depth(path);
*/

    return TRUE;
}

static gboolean do_keyboard_model_drag_data_delete (GtkTreeDragSource *drag_source,
                                 GtkTreePath       *path)
{
    return FALSE;
}

static gboolean do_keyboard_model_drag_data_get (GtkTreeDragSource *drag_source,
                              GtkTreePath       *path,
                              GtkSelectionData  *selection_data)
{
    if (gtk_tree_set_row_drag_data (selection_data, GTK_TREE_MODEL(drag_source), path))
        return TRUE;
    return FALSE;
}

static gboolean do_keyboard_model_drag_data_received (GtkTreeDragDest   *drag_dest,
                                   GtkTreePath       *dest,
                                   GtkSelectionData  *selection_data)
{
    GtkTreeModel *tree_model;
    GtkTreeModel *src_model = NULL;
    GtkTreePath *src_path = NULL;
    DoKeyboardRecord *record;
    GtkTreeIter iter;
    gboolean retval = FALSE;
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(drag_dest);
    do_alias_t *alias = priv->alias;

    tree_model = GTK_TREE_MODEL (drag_dest);
    if (gtk_tree_get_row_drag_data(selection_data, &src_model, &src_path) &&
            src_model == tree_model)
    {
        retval = TRUE;
        if (retval) {
            GtkTreePath *prev = gtk_tree_path_copy (dest);
            if (!gtk_tree_path_prev (prev)) {
                if (gtk_tree_path_get_depth(prev) > DO_KEYBOARD_MODEL_DEPTH_KEY) {
                    while (gtk_tree_path_get_depth(prev) > DO_KEYBOARD_MODEL_DEPTH_KEY)
                        gtk_tree_path_up(prev);
                }
                else
                    retval = TRUE;
            }
            if (retval) {
                gtk_tree_model_get_iter(tree_model, &iter, src_path);
                record = iter.user_data;

                switch (gtk_tree_path_get_depth(src_path)) {
                    case DO_KEYBOARD_MODEL_DEPTH_CONTENT: {
                        GtkTreeRowReference *ref = gtk_tree_row_reference_new(src_model, src_path);
                        if (ref) {
                            GtkTreePath *new_path;
                            retval = do_keyboard_model_insert_product(DO_KEYBOARD_MODEL(tree_model), prev, &new_path, record->code);
                            if (new_path)
                                gtk_tree_path_free(new_path);
                            if (retval)
                                retval = delete_content(alias, record->parent->children, record->parent->number, record->number);
                            if (retval) {
                                GtkTreePath *path = gtk_tree_row_reference_get_path(ref);
                                gtk_tree_model_row_deleted(tree_model, path);
                                gtk_tree_path_free(path);
                                if (record->parent) {
                                    do_sort_list_delete(record->parent->children, record->indx);
                                    normalize_record(record->parent);
                                }
                                free_record(record);
                            }
                            gtk_tree_row_reference_free(ref);
                        }
                        break;
                    }
                    case DO_KEYBOARD_MODEL_DEPTH_KEY: {
                        gint i;

                        for (i = record->children->count - 1; i >= 0 && retval; i--) {
                            GtkTreePath *new_path;
                            retval = do_keyboard_model_insert_product(DO_KEYBOARD_MODEL(tree_model), prev, &new_path,
                                                                      ((DoKeyboardRecord*)record->children->list[i])->code);
                            if (new_path)
                                gtk_tree_path_free(new_path);
                        }
                    }
                    break;
                }
            }
            gtk_tree_path_free (prev);
        }
    }
    return retval;
}

static gboolean do_keyboard_model_row_drop_possible (GtkTreeDragDest  *drag_dest,
                                  GtkTreePath      *dest_path,
				  GtkSelectionData *selection_data)
{
    return TRUE;
}


static void normalize_record(DoKeyboardRecord *record)
{
    int i;
    DoKeyboardRecord *child_record;
    if (!record || !record->children)
        return;
    for (i = 0; i < record->children->count; i++) {
        child_record = record->children->list[i];
        child_record->indx = i;
        normalize_record(child_record);
    }
}
static void normalize_tree(DoKeyboardModel *model)
{
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(model);
    DoKeyboardRecord  *record;
    int i;
    for (i = 0; i < priv->keys->count; i++) {
        record = priv->keys->list[i];
        record->indx = i;
        normalize_record(priv->keys->list[i]);
    }

}

static void free_record(DoKeyboardRecord *record)
{
    int i;
    if (record->desc)
        do_free(record->desc);
    if (record->label)
        do_free(record->label);
    if (record->code)
        do_free(record->code);
    for (i = 0; i < record->children->count; i++)
        free_record(record->children->list[i]);
    do_sort_list_free(record->children);
}
static void delete_record(DoKeyboardModel *model, DoKeyboardRecord *record)
{
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(model);
    do_sort_list_t *list;
    if (record->parent)
        list = record->parent->children;
    else
        list = priv->keys;

    do_sort_list_delete_node(list, record);
    int i;
    for (i = 0; i < list->count; i++)
        ((DoKeyboardRecord*)list->list[i])->indx = i;
    free_record(record);
}

static gboolean load_tree(DoKeyboardModel *model)
{
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(model);
    DoKeyboardRecord  *record;
    document_key0_t document_key0, document_key;
    document_rec_t document;
    do_alias_t *alias = priv->alias;

    int i;
    for (i = 0; i < priv->keys->count; i++)
        free_record(priv->keys->list[i]);
    do_sort_list_clear(priv->keys);

    int status;
    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD));
    do_text_set(alias, document_key0.sklad, domino_unit());
    memcpy(&document_key, &document_key0, sizeof(document_key0.dtype) + sizeof(document_key0.sklad));
    do_text_set(alias, document_key0.document, "");
    status = do_document_get0(alias, &document, &document_key0, DO_GET_GE);

    while (status == DO_OK) {
        if (memcmp(&document_key, &document_key0, sizeof(document_key0.dtype) + sizeof(document_key0.sklad))) break;

        record = load_document(model, alias, &document);
        if (record)
            do_sort_list_add(priv->keys, record);

        status = do_document_get0(alias, &document, &document_key0, DO_GET_NEXT);
    }
    if (status == DO_ERROR)
        return FALSE;
    normalize_tree(model);
    return TRUE;
}

static DoKeyboardRecord *load_document(DoKeyboardModel *model, do_alias_t *alias, document_rec_t *document)
{
    DoKeyboardRecord *record, *parent_rec;

    document_order_key0_t document_order_key0;
    document_key0_t document_key0;
    document_order_rec_t  document_order;
    int status;

    record = (DoKeyboardRecord*)do_malloc(sizeof(DoKeyboardRecord));

    memset(record, 0, sizeof(DoKeyboardRecord));
    record->kind = DO_KEYBOARD_MODEL_KIND_KEY;
    record->number = atoi(document->data.document);
    record->desc = do_document_param(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_DESC));
    record->label = do_document_param(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_COMMENT));
    record->parent = NULL;

    record->children = do_sort_list_new(0, 0, cmp_record, NULL);

    switch (do_document_param_int(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND))) {
        case DO_CONST_KEY_KIND_EMPTY:
            record->childless = FALSE;
            break;
        case DO_CONST_KEY_KIND_BCODE:
            record->childless = FALSE;
            parent_rec = record;
            record = (DoKeyboardRecord*)do_malloc(sizeof(DoKeyboardRecord));
            memset(record, 0, sizeof(DoKeyboardRecord));
            record->kind = DO_KEYBOARD_MODEL_KIND_CONTENT;
            record->number  = 0;
            record->code = do_document_param(alias, document, do_param(DO_PARAM_PARAMETR_PRODUCT));
            if (*record->code == '\0')
                break;
            record->desc = do_product_get_name(alias, record->code);
            record->parent = parent_rec;
            record->children = do_sort_list_new(0, 0, cmp_record, NULL);
            do_sort_list_add(parent_rec->children, record);
            record = parent_rec;
            break;
        case DO_CONST_KEY_KIND_BCODELIST:
            record->childless = FALSE;
            parent_rec = record;
            do_document_key0_set(&document_key0, document);
            do_document_order_key0_set_from_document(&document_order_key0, document);
            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
            while (status == DO_OK) {
                if (memcmp(&document_order_key0, &document_key0, sizeof(document_key0))) break;
                record = (DoKeyboardRecord*)do_malloc(sizeof(DoKeyboardRecord));
                memset(record, 0, sizeof(DoKeyboardRecord));
                record->kind = DO_KEYBOARD_MODEL_KIND_CONTENT;
                record->number  = document_order.data.line;
                record->code = do_text(alias, document_order.data.code);
                record->desc = do_product_get_name(alias, record->code);
                record->parent = parent_rec;
                record->children = do_sort_list_new(0, 0, cmp_record, NULL);
                do_sort_list_add(parent_rec->children, record);
                status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
            }
            if (status == DO_ERROR)
                return FALSE;
            record = parent_rec;
            break;
        default :
            record->childless = TRUE;
    }
    return record;
}
static gboolean delete_document(do_alias_t *alias, gint number)
{
    char document[100];
    document_key0_t document_key0;
    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD));
    do_text_set(alias, document_key0.sklad, domino_unit());
    sprintf(document, "%d", number);
    do_text_set(alias, document_key0.document, document);
    return do_action_delete_document(alias, &document_key0);
}
static gboolean document_list_to_bcode(do_alias_t *alias, do_sort_list_t *list, gint number)
{
    char document_number[100];
    document_order_key0_t document_order_key0, document_order_key;
    do_text_set(alias, document_order_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD));
    do_text_set(alias, document_order_key0.sklad, domino_unit());
    sprintf(document_number, "%d", number);
    do_text_set(alias, document_order_key0.document, document_number);
    memcpy(&document_order_key, &document_order_key0, sizeof(document_order_key0.dtype) + sizeof(document_order_key0.sklad) + sizeof(document_order_key0.document));
    document_order_key0.line = 0;
    gint count = 0;
    int status = do_document_order_key0(alias, &document_order_key0, DO_GET_GE);
    while (status == DO_OK) {
        if (memcmp(&document_order_key, &document_order_key0, sizeof(document_order_key0.dtype) + sizeof(document_order_key0.sklad) + sizeof(document_order_key0.document)))
            break;
        count++;
        if (count > 1) break;
        document_order_key.line = document_order_key0.line;
        status = do_document_order_key0(alias, &document_order_key0, DO_GET_NEXT);
    }
    if (count > 1)
        return TRUE;
    document_order_rec_t document_order;
    if (do_document_order_get0(alias, &document_order, &document_order_key, DO_GET_EQUAL) != DO_OK)
        return FALSE;
    if (do_document_order_delete(alias) != DO_OK)
        return FALSE;
    int i;
    for (i = 0; i < list->count; i++)
        if (((DoKeyboardRecord *)(list->list[i]))->number == document_order_key.line) {
            ((DoKeyboardRecord *)(list->list[i]))->number = 0;
            break;
        }

    document_rec_t document;
    if (do_document_get0(alias, &document, (document_key0_t*)&document_order_key, DO_GET_EQUAL) != DO_OK)
        return FALSE;
    char *product_code = do_text(alias, document_order.data.code);
    do_document_param_set(alias, &document, do_param(DO_PARAM_PARAMETR_PRODUCT), product_code);
    do_free(product_code);

    do_document_param_int_set(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND),
                                            DO_CONST_KEY_KIND_BCODE);
    if (do_document_update(alias, &document) != DO_OK ||
        !do_protocol_document_add_rec(alias, &document, DO_PROTOCOL_ACTION_CHANGE))
        return FALSE;
    return TRUE;
}
static gboolean delete_content(do_alias_t *alias, do_sort_list_t *list, gint number, gint line)
{
    char document_number[100];
    document_order_key0_t document_order_key0;
    do_text_set(alias, document_order_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD));
    do_text_set(alias, document_order_key0.sklad, domino_unit());
    sprintf(document_number, "%d", number);
    do_text_set(alias, document_order_key0.document, document_number);
    if (!line) {
        document_rec_t document;
        if (do_document_get0(alias, &document, (document_key0_t*)&document_order_key0, DO_GET_EQUAL) != DO_OK)
            return FALSE;
        do_document_param_int_set(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND),
                                    DO_CONST_KEY_KIND_EMPTY);
        do_document_param_set(alias, &document, do_param(DO_PARAM_PARAMETR_PRODUCT), "");
        if (do_document_update(alias, &document) != DO_OK ||
            !do_protocol_document_add_rec(alias, &document, DO_PROTOCOL_ACTION_CHANGE))
            return FALSE;
        return TRUE;
    }
    else {
        document_order_key0.line = line;
        if (do_action_delete_document_order(alias, &document_order_key0))
            return document_list_to_bcode(alias, list, number);
        else
            return FALSE;
    }
}

static gboolean shift_line(do_alias_t *alias,  do_sort_list_t *list, document_order_key0_t *document_order_key0)
{
    int status, i;
    document_order_rec_t document_order;
    status = do_document_order_get0(alias, &document_order, document_order_key0, DO_GET_EQUAL);
    if (status == DO_OK) {
        document_order_key0->line++;
        if (!shift_line(alias, list, document_order_key0))
            return FALSE;
        document_order_key0->line = document_order.data.line;
        if (do_document_order_get0(alias, &document_order, document_order_key0, DO_GET_EQUAL) != DO_OK)
            return FALSE;
        document_order.data.line++;
        for (i=0; i < list->count; i++)
            if (((DoKeyboardRecord*)(list->list[i]))->number == document_order_key0->line) {
                ((DoKeyboardRecord*)(list->list[i]))->number = document_order.data.line;
                break;
            }

        return do_document_order_update(alias, &document_order) == DO_OK;
    }
    return TRUE;

}
static gboolean document_bcode_to_list(do_alias_t *alias, do_sort_list_t *list, gint number)
{
    if (list->count != 1)
        return TRUE;
    document_rec_t document;
    char document_number[100];
    document_key0_t document_key0;
    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD));
    do_text_set(alias, document_key0.sklad, domino_unit());
    sprintf(document_number, "%d", number);
    do_text_set(alias, document_key0.document, document_number);

    if (do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL) != DO_OK)
        return FALSE;


    if (do_document_param_int(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND)) !=
           DO_CONST_KEY_KIND_BCODE )
        return TRUE;

    do_document_param_int_set(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND),
                            DO_CONST_KEY_KIND_BCODELIST);

    if (do_document_update(alias, &document) != DO_OK ||
        !do_protocol_document_add_rec(alias, &document, DO_PROTOCOL_ACTION_CHANGE))
        return FALSE;

    document_order_rec_t document_order;
    char *product_code = do_document_param(alias, &document, do_param(DO_PARAM_PARAMETR_PRODUCT));
    memset(&document_order.data, 0, sizeof(document_order.data));
    document_order.size = sizeof(document_order.data) - sizeof(document_order.data.params);
    memcpy(&document_order.data.dtype, &document.data.dtype, sizeof(document_order.data.dtype));
    do_text_set(alias, document_order.data.dtype1, DO_CONST_DEFAULT_DTYPE1);
    memcpy(&document_order.data.sklad, &document.data.sklad, sizeof(document_order.data.sklad));
    memcpy(&document_order.data.document, &document.data.document, sizeof(document_order.data.document));
    document_order.data.line = 1;
    document_order.data.sort = 0;
    do_text_set(alias, document_order.data.code, product_code);
    do_free(product_code);
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
    do_date_set(&document_order.data.date, tm);
    do_time_set(&document_order.data.time, tm);
    if (!do_action_insert_document_order(alias, &document_order))
        return FALSE;
    ((DoKeyboardRecord*)list->list[0])->number = document_order.data.line;

    return TRUE;
}
static gboolean document_to_bcode(do_alias_t *alias, gint number, const gchar *product_code)
{
    char document_number[100];
    document_rec_t document;
    document_key0_t document_key0;
    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD));
    do_text_set(alias, document_key0.sklad, domino_unit());
    sprintf(document_number, "%d", number);
    do_text_set(alias, document_key0.document, document_number);

    if (do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL) != DO_OK)
        return FALSE;
    do_document_param_set(alias, &document, do_param(DO_PARAM_PARAMETR_PRODUCT), product_code);
    do_document_param_int_set(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND),
                              DO_CONST_KEY_KIND_BCODE);
    if (do_document_update(alias, &document) != DO_OK ||
        !do_protocol_document_add_rec(alias, &document, DO_PROTOCOL_ACTION_CHANGE))
        return FALSE;
    return TRUE;
}

static gint insert_after_content(do_alias_t *alias,  do_sort_list_t *list, gint number, gint line, const gchar *product_code)
{
    if (!list->count) {
        if (!document_to_bcode(alias, number, product_code))
            return -1;
        return 0;
    }

    gint retval = line + 1;
    char document_number[100];
    document_order_key0_t document_order_key0;
    do_text_set(alias, document_order_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD));
    do_text_set(alias, document_order_key0.sklad, domino_unit());
    sprintf(document_number, "%d", number);
    do_text_set(alias, document_order_key0.document, document_number);
    document_order_key0.line = retval;

    if (!document_bcode_to_list(alias, list, number))
        return -1;

    if (!shift_line(alias, list, &document_order_key0))
        return -1;

    document_order_rec_t document_order;

    memset(&document_order.data, 0, sizeof(document_order.data));
    document_order.size = sizeof(document_order.data) - sizeof(document_order.data.params);

    memcpy(&document_order.data.dtype, &document_order_key0.dtype, sizeof(document_order.data.dtype));
    do_text_set(alias, document_order.data.dtype1, DO_CONST_DEFAULT_DTYPE1);
    memcpy(&document_order.data.sklad, &document_order_key0.sklad, sizeof(document_order.data.sklad));
    memcpy(&document_order.data.document, &document_order_key0.document, sizeof(document_order.data.document));
    document_order.data.line = retval;
    document_order.data.sort = 0;
    do_text_set(alias, document_order.data.code, product_code);
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
    do_date_set(&document_order.data.date, tm);
    do_time_set(&document_order.data.time, tm);
    if (!do_action_insert_document_order(alias, &document_order))
        retval = -1;
    return retval;
}

gboolean do_keyboard_model_insert_product(DoKeyboardModel *model, GtkTreePath *path, GtkTreePath **new_path, const gchar *product_code)
{
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(model);
    do_alias_t *alias = priv->alias;
    *new_path = NULL;

    DoKeyboardRecord *record, *new_record;
    gboolean retval = FALSE;
    gint line;
    GtkTreeIter iter;
    GtkTreePath *parent;

    parent = gtk_tree_path_copy(path);
    while (gtk_tree_path_get_depth(parent) > DO_KEYBOARD_MODEL_DEPTH_KEY)
        gtk_tree_path_up(parent);

    gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, parent);
    gtk_tree_path_free(parent);
    record = iter.user_data;
    if (record->childless)
        return FALSE;


    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path)) {
        record = (DoKeyboardRecord*) iter.user_data;
        if (record->kind == DO_KEYBOARD_MODEL_KIND_KEY)
            line = insert_after_content(alias, record->children, record->number, 0, product_code);
        else {
            line = insert_after_content(alias, record->parent->children, record->parent->number, record->number, product_code);
            record = record->parent;
            gtk_tree_path_up(path);
        }
        if (line != -1) {
            new_record = (DoKeyboardRecord*)do_malloc(sizeof(DoKeyboardRecord));
            memset(new_record, 0, sizeof(DoKeyboardRecord));
            new_record->kind = DO_KEYBOARD_MODEL_KIND_CONTENT;
            new_record->number  = line;
            new_record->code = do_strdup(product_code);
            new_record->desc = do_product_get_name(alias, product_code);
            new_record->parent = record;
            new_record->children = do_sort_list_new(0, 0, cmp_record, NULL);
            new_record->childless = TRUE;
            do_sort_list_add(record->children, new_record);
            normalize_record(record);

            GtkTreeIter iter;
            iter.stamp      = priv->stamp;
            iter.user_data  = new_record;
            iter.user_data2 = NULL;
            iter.user_data3 = NULL;

            *new_path = gtk_tree_path_copy (path);
            gtk_tree_path_append_index(*new_path, new_record->indx);
            gtk_tree_model_row_inserted(GTK_TREE_MODEL(model), *new_path, &iter);
            retval = TRUE;
        }

    }
    return retval;
}
static gint get_number_from_context(DoKeyboardModel *model, DoContext *context)
{
    if (do_context_get_kind(context) != DO_CONTEXT_KIND_DOCUMENT)
        return 0;

    gchar *dtype, *sklad, *number;
    g_object_get(context, "dtype-code", &dtype, "sklad-code", &sklad, "document-number", &number, NULL);
    if (!dtype || !sklad || !number)
        return 0;
    if (strcmp(dtype, do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD)) ||
         strcmp(sklad, domino_unit()))
        return 0;
    return atoi(number);
}

static gboolean get_document_from_context(DoContext *context, do_alias_t *alias, document_rec_t *document, gboolean *found)
{

    if (do_context_get_kind(context) != DO_CONTEXT_KIND_DOCUMENT)
        return FALSE;

    gchar *dtype, *sklad, *number;
    g_object_get(context, "dtype-code", &dtype, "sklad-code", &sklad, "document-number", &number, NULL);
    if (!dtype || !sklad || !number)
        return FALSE;
    if (strcmp(dtype, do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD)) ||
         strcmp(sklad, domino_unit()))
        return FALSE;

    document_key0_t document_key0;
    do_text_set(alias, document_key0.dtype, dtype);
    do_text_set(alias, document_key0.sklad, sklad);
    do_text_set(alias, document_key0.document, number);

    int status = do_document_get0(alias, document, &document_key0, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return FALSE;

    *found = status == DO_OK;
    return TRUE;
}


gboolean  do_keyboard_model_update_context(DoKeyboardModel *model, DoContext *context)
{
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(model);
    do_alias_t *alias = priv->alias;
    int indx;
    GtkTreeIter iter;
    GtkTreePath *path;
    document_rec_t document;
    DoKeyboardRecord *record;
//    gint number;

/*    number = get_number_from_context(model, context);
    if (!number)
        return FALSE;
*/

    path = do_keyboard_model_get_path_from_context(model, context);

    if (path) {
        do_keyboard_model_get_iter(GTK_TREE_MODEL(model), &iter, path);
        gtk_tree_model_row_deleted(GTK_TREE_MODEL(model), path);
        record = iter.user_data;
        do_sort_list_delete(priv->keys, record->indx);
        free_record(record);
        normalize_tree(model);
        gtk_tree_path_free(path);
    }

    gboolean found;
    if (!get_document_from_context(context, alias, &document, &found))
        return FALSE;

    if (found) {
        record = load_document(model, alias, &document);
        if (record) {
            do_sort_list_add(priv->keys, record);
            normalize_tree(model);

            path = get_path_from_record(record);
            gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path);
            gtk_tree_model_row_inserted(GTK_TREE_MODEL(model), path, &iter);
            gtk_tree_path_free(path);

            do_sort_list_t *children = record->children;
            for (indx = 0; indx < children->count; indx++) {
                record = children->list[indx];
                path = get_path_from_record(record);
                gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path);
                gtk_tree_model_row_inserted(GTK_TREE_MODEL(model), path, &iter);
                gtk_tree_path_free(path);
            }
        }
    }
    return TRUE;
}
GtkTreePath *do_keyboard_model_get_path_from_context(DoKeyboardModel *model, DoContext *context)
{
    gint indx;
    DoKeyboardRecord record;
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(model);

    record.number = get_number_from_context(model, context);
    if (record.number && do_sort_list_find(priv->keys, &indx, &record))
        return get_path_from_record(priv->keys->list[indx]);

    return NULL;
}
gboolean     do_keyboard_model_exist_number(DoKeyboardModel *model, gint number)
{
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(model);
    int i;
    DoKeyboardRecord *record;
    for (i = 0; i < priv->keys->count; i++) {
        record = priv->keys->list[i];
        if (record->number == number)
            return TRUE;
    }
    return FALSE;
}

gchar  *do_keyboard_model_get_label_from_number(DoKeyboardModel *model, gint number)
{
    DoKeyboardModelPrivate *priv  = DO_KEYBOARD_MODEL_GET_PRIVATE(model);
    int i;
    DoKeyboardRecord *record;
    for (i = 0; i < priv->keys->count; i++) {
        record = priv->keys->list[i];
        if (record->number == number) {
            if (record->label && *record->label != '\0')
                return g_strdup(record->label);
            else
                return g_strdup(record->desc);
        }
    }
    return NULL;
}
GList *do_keyboard_model_get_product_codes(DoKeyboardModel *model, GtkTreeIter *iter)
{
    GList *list = NULL;
    DoKeyboardRecord *record = iter->user_data;
    int j;
    if (record->children)
        for (j =0; j < record->children->count; j++)
            list = g_list_append(list,
                    g_strdup(((DoKeyboardRecord *)record->children->list[j])->code));
    return list;
}
