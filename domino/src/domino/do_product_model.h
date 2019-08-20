
#ifndef _do_product_model_h_included_
#define _do_product_model_h_included_

#include <gtk/gtk.h>
#include <domino.h>
#include "do_context.h"
#include <dolib.h>
#include "do_view.h"

#define DO_PRODUCT_MODEL_TYPE                  (do_product_model_get_type ())
#define DO_PRODUCT_MODEL(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), DO_PRODUCT_MODEL_TYPE, DoProductModel))
#define DO_PRODUCT_MODEL_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass),  DO_PRODUCT_MODEL_TYPE, DoProductModelClass))
#define DO_IS_PRODUCT_MODEL(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DO_PRODUCT_MODEL_TYPE))
#define DO_IS_PRODUCT_MODEL_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass),  DO_PRODUCT_MODEL_TYPE))
#define DO_PRODUCT_MODEL_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj),  DO_PRODUCT_MODEL_TYPE, DoProductModelClass))

enum
{
    DO_PRODUCT_MODEL_COL_RECORD = 0,
    DO_PRODUCT_MODEL_COL_CODE,
    DO_PRODUCT_MODEL_COL_NAME,
    DO_PRODUCT_MODEL_COL_COEF,
    DO_PRODUCT_MODEL_COL_PACK,
    DO_PRODUCT_MODEL_COL_PRICE,
    DO_PRODUCT_MODEL_COL_PRICE_MIN,
    DO_PRODUCT_MODEL_COL_STOCK,
    DO_PRODUCT_MODEL_COL_STOCK_MIN,
    DO_PRODUCT_MODEL_COL_MARKED,
    DO_PRODUCT_MODEL_COL_ADS,
    DO_PRODUCT_MODEL_COL_MANUFACTOR,
//    DO_PRODUCT_MODEL_COL_LIST_CODE,
//    DO_PRODUCT_MODEL_COL_LIST_NAME,
    DO_PRODUCT_MODEL_N_COLUMNS,
} ;


typedef struct _DoProductModel       DoProductModel;
typedef struct _DoProductModelClass  DoProductModelClass;

struct _DoProductModel
{
    GObject         parent;      /* this MUST be the first member */
};



/* DoProductModelClass: more boilerplate GObject stuff */

struct _DoProductModelClass
{
    GObjectClass parent_class;
    void            (* refresh_record)  (DoProductModel *model, GtkTreeIter *iter, gpointer data);
};


GType        do_product_model_get_type (void);

GtkTreeModel  *do_product_model_new(const gchar *alias_name, do_string_list_t *units, gboolean start_thread, gboolean realtime, gboolean multiprice);

void         do_product_model_append_record (DoProductModel   *do_product_model,
                                             const gchar  *code,
                                             const gchar  *sort);
gboolean     do_product_model_find_record_by_name(DoProductModel *do_product_model, const char *text, gint *indx);
gboolean     do_product_model_find_record_by_code(DoProductModel *do_product_model, const char *text, gint *indx);
gboolean     do_product_model_find_record_by_bcode(DoProductModel *do_product_model, const char *text, gint *indx);
const gchar *do_product_model_find_name_by_code(DoProductModel *do_product_model, const char *code);

void         do_product_model_set_record (DoProductModel   *do_product_model,
                                             const gchar  *code,
                                             const gchar  *sort);
void         do_product_model_set_marked(DoProductModel   *do_product_model, const char *code, DoMarked marked);

GtkTreePath *do_product_model_get_path_from_context(DoProductModel *model, DoContext *context);
GtkTreePath *do_product_model_get_path_from_code(DoProductModel *model, const gchar *code);

gboolean     do_product_model_set_filter(DoProductModel *do_product_model, do_string_list_t *lexs, do_string_list_t *nums, const gchar *inn);
gboolean     do_product_model_is_filtered(DoProductModel *do_product_model);
gboolean     do_product_model_is_filtered_and_empty(DoProductModel *do_product_model);

//gboolean     do_product_model_set_filter_exact(DoProductModel *do_product_model, int type, const char *text);
//gboolean     do_product_model_get_completion(DoProductModel *do_product_model, GtkListStore *store, const char* text);
//do_string_list_t *do_product_model_get_completion(DoProductModel *do_product_model, const char* text, int type_);


#ifdef DEBUG
void         do_product_model_test(DoProductModel *model);
#endif

#endif /* _do_product_model_h_included_ */
