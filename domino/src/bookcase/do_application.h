
#ifndef DO_APPLICATION_H
#define DO_APPLICATION_H

#include <gtk/gtk.h>
#include <json-glib/json-glib.h>
#include "do_window.h"


G_BEGIN_DECLS

#define DO_TYPE_APPLICATION         (do_application_get_type ())
#define DO_APPLICATION(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_APPLICATION, DoApplication))
#define DO_APPLICATION_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_APPLICATION, DoApplicationClass))
#define DO_IS_APPLICATION(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_APPLICATION))
#define DO_IS_APPLICATION_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_APPLICATION))
#define DO_APPLICATION_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_APPLICATION, DoApplicationClass))

typedef struct _DoApplication		DoApplication;
typedef struct _DoApplicationPrivate	DoApplicationPrivate;
typedef struct _DoApplicationClass		DoApplicationClass;

/*typedef enum
{
	DO_APPLICATION_FLAGS_NONE	= 1 << 0,

} DoApplicationFlags;
*/
struct _DoApplication
{
	GtkVBox parent_instance;
};

struct _DoApplicationClass
{
	GtkVBoxClass parent_class;
	DoWindow *(*create_window)           (DoApplication    *app);
};

#define DO_APPLICATION_FLAGS_DEFAULT 0

GType		do_application_get_type(void);

GtkWidget       *do_application_get_focus_child(DoApplication *app);
DoWindow	    *do_application_create_window(DoApplication *app, GdkScreen *screen);
JsonNode        *do_application_request(DoApplication *app, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache,...) G_GNUC_NULL_TERMINATED;
JsonNode        *do_application_request_async(DoApplication *app, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache,GFunc callback,gpointer data, ...) G_GNUC_NULL_TERMINATED;
gboolean         do_application_cancel_request(DoApplication *app, const gchar *key);
void             do_application_set_cache(DoApplication *app, const gchar *key, JsonNode *node);
JsonNode        *do_application_get_cache(DoApplication *app, const gchar *key);
gboolean         do_application_settings(DoApplication *app);
GObject         *do_application_get_client(DoApplication *app);



G_END_DECLS

#endif /* !DO_APPLICATION_H */
