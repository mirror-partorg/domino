
#ifndef _do_list_model_h_included_
#define _do_list_model_h_included_

#include <gtk/gtk.h>

#define DO_LIST_MODEL_TYPE                  (do_list_model_get_type ())
#define DO_LIST_MODEL(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), DO_LIST_MODEL_TYPE, DoListModel))
#define DO_LIST_MODEL_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass),  DO_LIST_MODEL_TYPE, DoListModelClass))
#define DO_IS_LIST_MODEL(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DO_LIST_MODEL_TYPE))
#define DO_IS_LIST_MODEL_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass),  DO_LIST_MODEL_TYPE))
#define DO_LIST_MODEL_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj),  DO_LIST_MODEL_TYPE, DoListModelClass))

#define DO_LIST_MODEL_N_KEYS 3


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

GtkTreeModel *do_list_model_new(const gchar *name, const gchar *fields, GObject *client);

#endif /* _do_list_model_h_included_ */
