
#ifndef DO_ADS_VIEW_H
#define DO_ADS_VIEW_H

#include <gtk/gtk.h>
#include "do_view.h"

G_BEGIN_DECLS

#define DO_TYPE_ADS_VIEW         (do_ads_view_get_type ())
#define DO_ADS_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_ADS_VIEW, DoAdsView))
#define DO_ADS_VIEW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_ADS_VIEW, DoAdsViewClass))
#define DO_IS_ADS_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_ADS_VIEW))
#define DO_IS_ADS_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_ADS_VIEW))
#define DO_ADS_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_ADS_VIEW, DoAdsViewClass))

typedef struct _DoAdsView		DoAdsView;
typedef struct _DoAdsViewPrivate	DoAdsViewPrivate;
typedef struct _DoAdsViewClass		DoAdsViewClass;

typedef enum
{
	DO_ADS_VIEW_FLAGS_NONE = 1 << 0,
} DoAdsViewFlags;

struct _DoAdsView
{
	GtkScrolledWindow parent_instance;
};

struct _DoAdsViewClass
{
	GtkScrolledWindowClass parent_class;
};

#define DO_ADS_VIEW_FLAGS_DEFAULT 0

GType		do_ads_view_get_type(void);

GtkWidget  *do_ads_view_new(); // to do

G_END_DECLS

#endif /* !DO_ADS_VIEW_H */
