
#ifndef _do_keyboard_model_h_included_
#define _do_keyboard_model_h_included_

#include <gtk/gtk.h>
#include "do_context.h"
#include <domino.h>

G_BEGIN_DECLS

#define DO_KEYBOARD_MODEL_TYPE                  (do_keyboard_model_get_type ())
#define DO_KEYBOARD_MODEL(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), DO_KEYBOARD_MODEL_TYPE, DoKeyboardModel))
#define DO_KEYBOARD_MODEL_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass),  DO_KEYBOARD_MODEL_TYPE, DoKeyboardModelClass))
#define DO_IS_KEYBOARD_MODEL(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DO_KEYBOARD_MODEL_TYPE))
#define DO_IS_KEYBOARD_MODEL_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass),  DO_KEYBOARD_MODEL_TYPE))
#define DO_KEYBOARD_MODEL_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj),  DO_KEYBOARD_MODEL_TYPE, DoKeyboardModelClass))

#define DO_KEYBOARD_MODEL_DEPTH_MAX 4
#define DO_KEYBOARD_MODEL_DEPTH_KEY 1
#define DO_KEYBOARD_MODEL_DEPTH_CONTENT 2


enum
{
    DO_KEYBOARD_MODEL_COL_RECORD = 0,
    DO_KEYBOARD_MODEL_COL_KIND,
    DO_KEYBOARD_MODEL_COL_NUMBER,
    DO_KEYBOARD_MODEL_COL_CHILDLESS,
    DO_KEYBOARD_MODEL_COL_DESC,
    DO_KEYBOARD_MODEL_COL_LABEL,
    DO_KEYBOARD_MODEL_COL_CODE,
    DO_KEYBOARD_MODEL_N_COLUMNS,
};

enum
{
    DO_KEYBOARD_MODEL_KIND_KEY,
    DO_KEYBOARD_MODEL_KIND_CONTENT,
};

typedef struct _DoKeyboardModel       DoKeyboardModel;
typedef struct _DoKeyboardModelClass  DoKeyboardModelClass;
typedef struct _DoKeyboardModelPrivate	DoKeyboardModelPrivate;

struct _DoKeyboardModel
{
    GObject         parent;      /* this MUST be the first member */

    DoKeyboardModelPrivate *priv;
    gint            stamp;
};

/* DoKeyboardModelClass: more boilerplate GObject stuff */

struct _DoKeyboardModelClass
{
    GObjectClass parent_class;
};


GType        do_keyboard_model_get_type (void);

GtkTreeModel   *do_keyboard_model_new(const gchar *alias_name, GList *sort_list);

//void         do_keyboard_model_append_record (DoKeyboardModel   *do_keyboard_model,
//                                             const gchar  *code,
                                             //const gchar  *sort);
gboolean     do_keyboard_model_remove(DoKeyboardModel *model, GtkTreePath *path);
gboolean     do_keyboard_model_remove_from_list(DoKeyboardModel *model, GList *list);
gboolean     do_keyboard_model_insert_product(DoKeyboardModel *model, GtkTreePath *path, GtkTreePath **new_path, const gchar *product_code);

gboolean     do_keyboard_model_update_context(DoKeyboardModel *model, DoContext *context);
GtkTreePath *do_keyboard_model_get_path_from_context(DoKeyboardModel *model, DoContext *context);

gchar       *do_keyboard_model_get_label_from_number(DoKeyboardModel *model, gint number);
gboolean     do_keyboard_model_exist_number(DoKeyboardModel *model, gint number);
GList       *do_keyboard_model_get_product_codes(DoKeyboardModel *model, GtkTreeIter *iter);

G_END_DECLS


#endif /* _do_keyboard_model_h_included_ */
