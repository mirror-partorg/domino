
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

#define DO_CLIENT_GET "GET"
#define DO_CLIENT_POST "POST"

typedef struct _DoClient		DoClient;
typedef struct _DoClientPrivate	DoClientPrivate;
typedef struct _DoClientClass		DoClientClass;
typedef enum
{
	DO_CLIENT_FLAGS_ARCHIVE = 1 << 0,
	DO_CLIENT_FLAGS_NOCACHE = 1 << 1,
	DO_CLIENT_FLAGS_MAY_IGNORE = 1 << 2,
} DoClientFlags;


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
const gchar *do_client_get_url(DoClient *client);
const gchar *do_client_get_store(DoClient *client);
gboolean   do_client_cancel_request(DoClient *client, const gchar *key);
JsonNode  *do_client_request2(DoClient *client,
		                     const gchar *method,
							 const gchar *func,
		                     const gchar *key,
							 DoClientFlags flags,
							 ...) G_GNUC_NULL_TERMINATED;
JsonNode  *do_client_request2_valist(DoClient *client,
		                            const gchar *method,
									const gchar *func,
									const gchar *key,
									DoClientFlags flags,
									va_list args);
JsonNode  *do_client_request2_async(DoClient *client,
		                     const gchar *method,
							 const gchar *func,
		                     const gchar *key,
							 DoClientFlags flags,
							 GFunc callback,
							 gpointer data,
							 ...) G_GNUC_NULL_TERMINATED;
JsonNode  *do_client_request2_valist_async(DoClient *client,
		                            const gchar *method,
									const gchar *func,
									const gchar *key,
									DoClientFlags flags,
     								GFunc callback,
								    gpointer data,
									va_list args);
gchar     *do_client_strftime(GDateTime *time);
GDateTime *do_client_strptime(const gchar* str);
void       do_client_set_cache(DoClient *client, const gchar *key, JsonNode *node, gchar *text, gint length);
JsonNode  *do_client_get_cache(DoClient *client, const gchar *key);

G_END_DECLS

#endif /* !DO_CLIENT_H */
