
#ifndef DO_TEMPLATE_H
#define DO_TEMPLATE_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_TEMPLATE         (do_template_get_type ())
#define DO_TEMPLATE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_TEMPLATE, DoTemplate))
#define DO_TEMPLATE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_TEMPLATE, DoTemplateClass))
#define DO_IS_TEMPLATE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_TEMPLATE))
#define DO_IS_TEMPLATE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_TEMPLATE))
#define DO_TEMPLATE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_TEMPLATE, DoTemplateClass))

typedef struct _DoTemplate		DoTemplate;
typedef struct _DoTemplatePrivate	DoTemplatePrivate;
typedef struct _DoTemplateClass		DoTemplateClass;

typedef enum
{
	DO_TEMPLATE_FLAGS_	= 1 << 0,

} DoTemplateFlags;

struct _DoTemplate
{
	GtkVBox parent_instance;
	DoTemplatePrivate *priv;
};

struct _DoTemplateClass
{
	GtkVBoxClass parent_class;
};

#define DO_TEMPLATE_FLAGS_DEFAULT 0

GType		do_template_get_type(void);

GtkWidget  *do_template_new(GtkWidget *parent, DoTemplateFlags flags, ...);

G_END_DECLS

#endif /* !DO_TEMPLATE_H */
