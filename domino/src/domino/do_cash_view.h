
#ifndef DO_CASH_VIEW_H
#define DO_CASH_VIEW_H
//#ifdef ARTIX

#include <gtk/gtk.h>
#include "do_tree_view.h"

G_BEGIN_DECLS

#define DO_TYPE_CASH_VIEW         (do_cash_view_get_type ())
#define DO_CASH_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_CASH_VIEW, DoCashView))
#define DO_CASH_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_CASH_VIEW, DoCashViewClass))
#define DO_IS_CASH_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_CASH_VIEW))
#define DO_IS_CASH_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_CASH_VIEW))
#define DO_CASH_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_CASH_VIEW, DoCashViewClass))

typedef struct _DoCashView		DoCashView;
typedef struct _DoCashViewPrivate	DoCashViewPrivate;
typedef struct _DoCashViewClass		DoCashViewClass;

struct _DoCashView
{
	DoTreeView parent_instance;
};

struct _DoCashViewClass
{
	DoTreeViewClass parent_class;
};

GType		do_cash_view_get_type(void);

GtkWidget *do_cash_view_new (const gchar *alias_name);

G_END_DECLS

//#endif
#endif /* !DO_CASH_VIEW_H */
