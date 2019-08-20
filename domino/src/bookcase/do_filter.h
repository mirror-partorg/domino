
#ifndef DO_FILTER_H
#define DO_FILTER_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_FILTER         (do_filter_get_type ())
#define DO_FILTER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_FILTER, DoFilter))
#define DO_FILTER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_FILTER, DoFilterClass))
#define DO_IS_FILTER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_FILTER))
#define DO_IS_FILTER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_FILTER))
#define DO_FILTER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_FILTER, DoFilterClass))

typedef struct _DoFilter		DoFilter;
typedef struct _DoFilterPrivate	DoFilterPrivate;
typedef struct _DoFilterClass		DoFilterClass;

typedef enum
{
	DO_FILTER_FLAGS_SHOW_LABEL	= 1 << 0,
	DO_FILTER_FLAGS_SHOW_BUTTON	= 1 << 1,

} DoFilterFlags;

struct _DoFilter
{
	GtkHBox parent_instance;
};

struct _DoFilterClass
{
	GtkHBoxClass parent_class;
};

GType		       do_filter_get_type(void);

GtkWidget         *do_filter_new(GtkWidget *parent, DoFilterFlags flags);

G_END_DECLS

#endif /* !DO_FILTER_H */
