
#ifndef DO_SETTING_VIEW_H
#define DO_SETTING_VIEW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_SETTING_VIEW         (do_setting_view_get_type ())
#define DO_SETTING_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_SETTING_VIEW, DoSettingView))
#define DO_SETTING_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_SETTING_VIEW, DoSettingViewClass))
#define DO_IS_SETTING_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_SETTING_VIEW))
#define DO_IS_SETTING_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_SETTING_VIEW))
#define DO_SETTING_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_SETTING_VIEW, DoSettingViewClass))

typedef struct _DoSettingView		DoSettingView;
typedef struct _DoSettingViewPrivate	DoSettingViewPrivate;
typedef struct _DoSettingViewClass		DoSettingViewClass;

struct _DoSettingView
{
	GtkScrolledWindow parent_instance;
};

struct _DoSettingViewClass
{
	GtkScrolledWindowClass parent_class;
};

#define DO_SETTING_VIEW_FLAGS_DEFAULT 0

GType		do_setting_view_get_type(void);

GtkWidget  *do_setting_view_new(gint kind);

G_END_DECLS

#endif /* !DO_SETTING_VIEW_H */
