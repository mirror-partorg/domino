
#ifndef DO_REPORT_VIEW_H
#define DO_REPORT_VIEW_H

#include <gtk/gtk.h>
#include "do_tree_view.h"

G_BEGIN_DECLS

#define DO_TYPE_REPORT_VIEW         (do_report_view_get_type ())
#define DO_REPORT_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_REPORT_VIEW, DoReportView))
#define DO_REPORT_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_REPORT_VIEW, DoReportViewClass))
#define DO_IS_REPORT_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_REPORT_VIEW))
#define DO_IS_REPORT_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_REPORT_VIEW))
#define DO_REPORT_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_REPORT_VIEW, DoReportViewClass))

typedef struct _DoReportView		DoReportView;
typedef struct _DoReportViewPrivate	DoReportViewPrivate;
typedef struct _DoReportViewClass	DoReportViewClass;

struct _DoReportView
{
	DoTreeView parent_instance;
};

struct _DoReportViewClass
{
	DoTreeViewClass parent_class;
};

#define DO_REPORT_VIEW_FLAGS_DEFAULT 0

GType		do_report_view_get_type(void);

GtkWidget  *do_report_view_new();

G_END_DECLS

#endif /* !DO_REPORT_VIEW_H */
