
#ifndef _do_report_unit_model_h_included_
#define _do_report_unit_model_h_included_

#include <gtk/gtk.h>
#include <domino.h>
#include "do_context.h"
#include <dolib.h>

#define DO_REPORT_UNIT_MODEL_TYPE                  (do_report_unit_model_get_type ())
#define DO_REPORT_UNIT_MODEL(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), DO_REPORT_UNIT_MODEL_TYPE, DoReportUnitModel))
#define DO_REPORT_UNIT_MODEL_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass),  DO_REPORT_UNIT_MODEL_TYPE, DoReportUnitModelClass))
#define DO_IS_REPORT_UNIT_MODEL(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DO_REPORT_UNIT_MODEL_TYPE))
#define DO_IS_REPORT_UNIT_MODEL_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass),  DO_REPORT_UNIT_MODEL_TYPE))
#define DO_REPORT_UNIT_MODEL_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj),  DO_REPORT_UNIT_MODEL_TYPE, DoReportUnitModelClass))

typedef struct _DoReportUnitModel       DoReportUnitModel;
typedef struct _DoReportUnitModelClass  DoReportUnitModelClass;
typedef enum _DoReportValueType  DoReportValueType;

#define DO_REPORT_UNIT_MODEL_COL_CODE 0
#define DO_REPORT_UNIT_MODEL_COL_DESC 1
#define DO_REPORT_UNIT_MODEL_N_COL 2

enum _DoReportValueType
{
    DO_REPORT_VALUE_TYPE_DOUBLE = 0,
    DO_REPORT_VALUE_TYPE_MONEY,
    DO_REPORT_VALUE_TYPE_REST,
};
struct _DoReportUnitModel
{
    GObject         parent;      /* this MUST be the first member */
};

typedef enum
{
	DO_REPORT_UNIT_MODEL_FLAG_TOTAL_COLUMN = 1 << 0,
	DO_REPORT_UNIT_MODEL_FLAG_TOTAL_ROW = 1 << 1,
} DoReportUnitModelFlags;

struct _DoReportUnitModelClass
{
    GObjectClass parent_class;
};

GType        do_report_unit_model_get_type (void);

GtkTreeModel  *do_report_unit_model_new(gint n_units, gint n_values, DoReportUnitModelFlags flags);

void         do_report_unit_model_append(DoReportUnitModel   *model,
                                             const gchar  *code,
                                             const gchar  *desc,
                                             GtkTreeIter *iter);
void         do_report_unit_model_delete(DoReportUnitModel   *model,
                                             GtkTreeIter *iter);
void         do_report_unit_model_set_code(DoReportUnitModel   *model,
                                             const gchar  *code,
                                             const gchar  *desc,
                                             GtkTreeIter *iter);
int          do_report_unit_model_is_empty(DoReportUnitModel   *model,
                                             GtkTreeIter *iter);

void         do_report_unit_model_set_title (DoReportUnitModel   *model,
                                             const gchar *title);
void         do_report_unit_model_set_footer (DoReportUnitModel   *model,
                                             const gchar *footer);
void         do_report_unit_model_set_value_type (DoReportUnitModel   *model,
                                             ... );
void         do_report_unit_model_set_header_title (DoReportUnitModel   *model,
                                                    guint deep,
                                             ... );
void         do_report_unit_model_set_unit_title (DoReportUnitModel   *model,
                                                  GList *list);

void         do_report_unit_model_set_unit_value (DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint i_unit,
                                             gint i_value,
                                             gdouble value);

void         do_report_unit_model_add_unit_value (DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint i_unit,
                                             gint i_value,
                                             gdouble value);

void         do_report_unit_model_set_no_total (DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint i_value);
void         do_report_unit_model_set_grey (DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint i_value);

void         do_report_unit_model_set_value(DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint column,
                                             gdouble value);
void         do_report_unit_model_set_print_units(DoReportUnitModel   *model, gboolean print);

void         do_report_unit_model_cell_data_func_product_name(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void         do_report_unit_model_cell_data_func_rest(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void         do_report_unit_model_cell_data_func_money(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
gdouble do_report_unit_model_get_unit_value (DoReportUnitModel   *model,
                                             GtkTreeIter *iter,
                                             gint i_unit,
                                             gint i_value);


#endif /* _do_report_unit_model_h_included_ */
