
#ifndef DO_SCHEDULE_VIEW_H
#define DO_SCHEDULE_VIEW_H

#include "do_tree_view.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_SCHEDULE_VIEW         (do_schedule_view_get_type ())
#define DO_SCHEDULE_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_SCHEDULE_VIEW, DoScheduleView))
#define DO_SCHEDULE_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_SCHEDULE_VIEW, DoScheduleViewClass))
#define DO_IS_SCHEDULE_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_SCHEDULE_VIEW))
#define DO_IS_SCHEDULE_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_SCHEDULE_VIEW))
#define DO_SCHEDULE_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_SCHEDULE_VIEW, DoScheduleViewClass))

typedef struct _DoScheduleView		DoScheduleView;
typedef struct _DoScheduleViewPrivate	DoScheduleViewPrivate;
typedef struct _DoScheduleViewClass		DoScheduleViewClass;


struct _DoScheduleView
{
	DoTreeView parent_instance;

};

struct _DoScheduleViewClass
{
	DoTreeViewClass parent_class;
};

GType		do_schedule_view_get_type(void);

GtkWidget  *do_schedule_view_new (const gchar *aliases, GDate *date1, GDate *date2, gint portponement, gchar *ignore);

G_END_DECLS

#endif /* !DO_SCHEDULE_VIEW_H */
