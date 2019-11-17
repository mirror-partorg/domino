
#ifndef DO_CLIENT_H
#define DO_CLIENT_H

#include <gtk/gtk.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS

#define DO_TYPE_CLIENT         (do_client_get_type ())
#define DO_CLIENT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_CLIENT, DoClient))
#define DO_CLIENT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_CLIENT, DoClientClass))
#define DO_IS_CLIENT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_CLIENT))
#define DO_IS_CLIENT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_CLIENT))
#define DO_CLIENT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_CLIENT, DoClientClass))

typedef struct _DoClient		DoClient;
typedef struct _DoClientPrivate	DoClientPrivate;
typedef struct _DoClientClass		DoClientClass;

struct _DoClient
{
	GObject parent_instance;
};

struct _DoClientClass
{
	GObjectClass parent_class;
};

#define DO_CLIENT_FLAGS_DEFAULT 0

GType	   do_client_get_type(void);

GObject   *do_client_new(const gchar *url, const gchar *store);
gboolean   do_client_cancel_request(DoClient *client, const gchar *key);
JsonNode  *do_client_request(DoClient *client,
		                     const gchar *method,
							 const gchar *func,
		                     const gchar *key,
							 gboolean archive,
							 gboolean nocache,...) G_GNUC_NULL_TERMINATED;
JsonNode  *do_client_request_valist(DoClient *client,
		                            const gchar *method,
									const gchar *func,
									const gchar *key,
									gboolean archive,
									gboolean nocache,
									va_list args);
JsonNode  *do_client_request_async(DoClient *client,
		                     const gchar *method,
							 const gchar *func,
		                     const gchar *key,
							 gboolean archive,
							 gboolean nocache,
							 GFunc callback,
							 gpointer data,
							 ...) G_GNUC_NULL_TERMINATED;
JsonNode  *do_client_request_valist_async(DoClient *client,
		                            const gchar *method,
									const gchar *func,
									const gchar *key,
									gboolean archive,
									gboolean nocache,
     								GFunc callback,
								    gpointer data,
									va_list args);
gchar     *do_client_strftime(GDateTime *time);
GDateTime *do_client_strptime(const gchar* str);
void       do_client_set_cache(DoClient *client, const gchar *key, JsonNode *node, gchar *text, gint length);
JsonNode  *do_client_get_cache(DoClient *client, const gchar *key);

G_END_DECLS

#endif /* !DO_CLIENT_H */
