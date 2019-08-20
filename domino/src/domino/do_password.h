#ifndef DO_PASSWORD_H_INCLUDED
#define DO_PASSWORD_H_INCLUDED
#include "do_password_dialog.h"


G_BEGIN_DECLS

typedef struct _DoPassword   DoPassword;
typedef struct _DoPasswordClass DoPasswordClass;

#define DO_TYPE_PASSWORD              (do_password_get_type ())
#define DO_PASSWORD(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), DO_TYPE_PASSWORD, DoPassword))
#define DO_PASSWORD_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), DO_TYPE_PASSWORD, DoPasswordClass))
#define DO_IS_PASSWORD(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), DO_TYPE_PASSWORD))
#define DO_IS_PASSWORD_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), DO_TYPE_PASSWORD))
#define DO_PASSWORD_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), DO_TYPE_PASSWORD, DoPasswordClass))

#define DO_PASSWORD_SCHEME_DOMINO "domino"

typedef gboolean (DoPasswordAuthentication)(DoPassword *password, gpointer data);

struct _DoPassword {
    GObject parent_instance;
};

struct _DoPasswordClass {
    GObjectClass parent_class;
};

GType       do_password_get_type   (void) G_GNUC_CONST;

DoPassword *do_password_get(GtkWidget *dialog, const gchar *auth_url, DoPasswordAuthentication *func, gpointer func_data); // "<scheme>://<full_name>:<password>@<host>:<port>"
void do_password_clear(const gchar *auth_url);


G_END_DECLS

#endif // DO_PASSWORD_H_INCLUDED
