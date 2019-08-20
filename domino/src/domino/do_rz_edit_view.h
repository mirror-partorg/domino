
#ifndef DO_RZ_EDIT_VIEW_H
#define DO_RZ_EDIT_VIEW_H

#include <gtk/gtk.h>
#include "do_rz_edit.h"
#include "do_view.h"

G_BEGIN_DECLS

#define DO_TYPE_RZ_EDIT_VIEW         (do_rz_edit_view_get_type ())
#define DO_RZ_EDIT_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_RZ_EDIT_VIEW, DoRzEditView))
#define DO_RZ_EDIT_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_RZ_EDIT_VIEW, DoRzEditViewClass))
#define DO_IS_RZ_EDIT_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_RZ_EDIT_VIEW))
#define DO_IS_RZ_EDIT_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_RZ_EDIT_VIEW))
#define DO_RZ_EDIT_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_RZ_EDIT_VIEW, DoRzEditViewClass))

typedef struct _DoRzEditView		DoRzEditView;
typedef struct _DoRzEditViewPrivate	DoRzEditViewPrivate;
typedef struct _DoRzEditViewClass		DoRzEditViewClass;

typedef enum
{
	DO_RZ_EDIT_VIEW_FLAGS_SHOW_MULTI_PRICE = 1 << 0,
	DO_RZ_EDIT_VIEW_FLAGS_SHOW_RECEIVED    = 1 << 1,
	DO_RZ_EDIT_VIEW_FLAGS_SHOW_UNPACK_PRICE = 1 << 2,
	DO_RZ_EDIT_VIEW_FLAGS_SELECTION_MULTIPLE = 1 << 3,
	DO_RZ_EDIT_VIEW_FLAGS_ROW_ACTIVATED_TO_SEND = 1 << 4,
	DO_RZ_EDIT_VIEW_FLAGS_LOAD_IN_THREAD = 1 << 5,
	DO_RZ_EDIT_VIEW_FLAGS_REALTIME = 1 << 6,
} DoRzEditViewFlags;

struct _DoRzEditView
{
	DoRzEdit parent_instance;
};

struct _DoRzEditViewClass
{
	DoRzEditClass parent_class;
};

#define DO_RZ_EDIT_VIEW_FLAGS_DEFAULT 0

GType		do_rz_edit_view_get_type(void);

GtkWidget  *do_rz_edit_view_new (DoView *parent, const gchar *alias_name, DoRzEditFlags flags, DoDialogAction action, DoContext *context);

G_END_DECLS

#endif /* !DO_RZ_EDIT_VIEW_H */
