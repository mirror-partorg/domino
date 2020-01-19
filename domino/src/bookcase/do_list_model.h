
#ifndef _do_list_model_h_included_
#define _do_list_model_h_included_

#include <gtk/gtk.h>
#include "do_client.h"
#include <json-glib/json-glib.h>

#define DO_LIST_MODEL_TYPE                  (do_list_model_get_type ())
#define DO_LIST_MODEL(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), DO_LIST_MODEL_TYPE, DoListModel))
#define DO_LIST_MODEL_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass),  DO_LIST_MODEL_TYPE, DoListModelClass))
#define DO_IS_LIST_MODEL(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DO_LIST_MODEL_TYPE))
#define DO_IS_LIST_MODEL_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass),  DO_LIST_MODEL_TYPE))
#define DO_LIST_MODEL_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj),  DO_LIST_MODEL_TYPE, DoListModelClass))

#define DO_LIST_MODEL_N_KEYS 3
#define DO_LIST_MODEL_COL_CODE 2
#define DO_LIST_MODEL_COL_SORT 1
#define DO_LIST_MODEL_COL_KEY 0


typedef struct _DoListModel       DoListModel;
typedef struct _DoListModelClass  DoListModelClass;

struct _DoListModel
{
    GObject parent;      /* this MUST be the first member */
};



/* DoListModelClass: more boilerplate GObject stuff */

struct _DoListModelClass
{
    GObjectClass parent_class;
    void (*refresh_record) (DoListModel *model, GtkTreeIter *iter, gpointer data);
};

GType do_list_model_get_type (void);

GtkTreeModel *do_list_model_new(const gchar *name, const gchar *fields, DoClient *client);
void do_list_model_empty_col_change_depricated(DoListModel *model, gint code_to_col, gint sort_to_col, gint key_to_col);

gboolean do_list_model_set_filter(DoListModel *tree_model, JsonNode *node);
gboolean do_list_model_is_filtered(DoListModel *tree_model, GStrv *lexems);

void do_list_model_full_read(DoListModel *tree_model);
gboolean do_list_model_full_readed(DoListModel *tree_model, guint *percent);
void do_list_model_set_updated(DoListModel *tree_model, gboolean updated);

gboolean do_list_model_record_is_relevant(DoListModel *tree_model, GtkTreeIter  *iter);
gboolean do_list_model_find_record_by_sort(DoListModel *model, GtkTreeIter *iter, const gchar *text);
gboolean do_list_model_find_record_by_code(DoListModel *model, GtkTreeIter *iter, const gchar *text);
gboolean do_list_model_find_record_by_key(DoListModel *model, GtkTreeIter *iter, const gchar *text);
#ifdef SEARCH_BY_BCODE
gboolean do_list_model_find_record_by_bcode(DoListModel *model, GtkTreeIter *iter, const gchar *text);
#endif

#endif /* _do_list_model_h_included_ */
