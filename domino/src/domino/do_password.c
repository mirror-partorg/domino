#include "do_password.h"
#include <dolib.h>
#include "do_utilx.h"
#include "domino.h"

static void do_password_get_property  (GObject              *object,
					     guint                 param_id,
					     GValue               *value,
					     GParamSpec           *pspec);
static void do_password_set_property  (GObject              *object,
					     guint                 param_id,
					     const GValue         *value,
					     GParamSpec           *pspec);
static void do_password_finalize (GObject          *object);

#define DO_PASSWORD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), DO_TYPE_PASSWORD, DoPasswordPrivate))


#define OBJECT_ROOT_PATH "Password"

typedef struct _DoPasswordPrivate DoPasswordPrivate;
typedef struct _DoPasswordCache DoPasswordCache;

//"<scheme>://<full_name>:<password>@<host>:<port>"

struct _DoPasswordPrivate
{
    gchar *name;
    gchar *domain;
    gchar *password;
};

struct _DoPasswordCache
{
    gchar *scheme;
    gchar *host;
    gint   port;
    gchar *name;
    gchar *domain;
    gchar *password;
};
enum {
    PROP_0,
    PROP_NAME,
    PROP_PASSWORD,
    PROP_DOMAIN,
};

G_DEFINE_TYPE (DoPassword, do_password, G_TYPE_OBJECT)

static DoPasswordCache *get_password_cache_from_url(const gchar *url);

static void do_password_init (DoPassword *password)
{
    DoPasswordPrivate *priv = DO_PASSWORD_GET_PRIVATE (password);
	memset(priv, 0, sizeof(DoPasswordPrivate));
}

static void do_password_class_init (DoPasswordClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->get_property = do_password_get_property;
    object_class->set_property = do_password_set_property;
    object_class->finalize     = do_password_finalize;

// signal
// property

    g_object_class_install_property (object_class,
				   PROP_NAME,
				   g_param_spec_string ("name",
							"Имя",
							"Имя",
							NULL,
							G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
				   PROP_DOMAIN,
				   g_param_spec_string ("domain",
							"Домен",
							"Домен",
							NULL,
							G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
				   PROP_PASSWORD,
				   g_param_spec_string ("password",
							"Пароль",
							"Пароль",
							NULL,
							G_PARAM_READWRITE));

    g_type_class_add_private (object_class, sizeof (DoPasswordPrivate));
}
static void do_password_get_property (GObject     *object,
				guint        param_id,
				GValue      *value,
				GParamSpec  *pspec)
{
    DoPasswordPrivate *priv = DO_PASSWORD_GET_PRIVATE (object);

    switch (param_id)
    {
    case PROP_NAME:
      g_value_set_string (value, priv->name);
      break;
    case PROP_DOMAIN:
      g_value_set_string (value, priv->domain);
      break;
    case PROP_PASSWORD:
      g_value_set_string (value, priv->password);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
    }
}

static void
do_password_set_property (GObject      *object,
				guint         param_id,
				const GValue *value,
				GParamSpec   *pspec)
{
    DoPasswordPrivate *priv = DO_PASSWORD_GET_PRIVATE (object);

    switch (param_id)
    {
    case PROP_NAME:
      if (priv->name) g_free(priv->name);
      priv->name = g_value_dup_string (value);
      break;
    case PROP_DOMAIN:
      if (priv->domain) g_free(priv->domain);
      priv->domain = g_value_dup_string (value);
      break;
    case PROP_PASSWORD:
      if (priv->password) g_free(priv->password);
      priv->password = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
    }
}
static void do_password_finalize (GObject          *object)
{
    DoPasswordPrivate *priv = DO_PASSWORD_GET_PRIVATE (object);

    if (priv->name) g_free(priv->name);
    if (priv->domain) g_free(priv->domain);
    if (priv->password) g_free(priv->password);

    G_OBJECT_CLASS (do_password_parent_class)->finalize (object);
}
DoPassword *do_password_new()
{
    return g_object_new (DO_TYPE_PASSWORD, NULL);
}

static GList *cache = NULL;

static void cache_item_free(DoPasswordCache *ch)
{
    if (ch->scheme)
        g_free(ch->scheme);
    if (ch->host)
        g_free(ch->host);
    if (ch->name)
        g_free(ch->name);
    if (ch->domain)
        g_free(ch->domain);
    if (ch->password)
        g_free(ch->password);
    g_free(ch);
}
static gboolean cache_item_find(DoPasswordCache *ch, GList **l)
{
    DoPasswordCache *item;
    for (*l = cache; *l; *l = (*l)->next) {
        item = (*l)->data;
        if ( (!ch->name || strcmp(ch->name, item->name)) &&
             ((!ch->domain && !item->domain) || (ch->domain && item->domain && !strcmp(ch->domain, item->domain))) &&
             ((!ch->scheme && !item->scheme) || (ch->scheme && item->scheme && !strcmp(ch->scheme, item->scheme))) &&
             ((!ch->host && !item->host) || (ch->host && item->host && !strcmp(ch->host, item->host))) &&
             (ch->port == item->port) ) {
                 return TRUE;
             }
    }
    return FALSE;
}
static void cache_item_add(DoPasswordCache *ch)
{
    DoPasswordCache *item;
    if (!ch->password)
        return;
    GList *l;
    if (cache_item_find(ch, &l)) {
        item = l->data;
        if (item->password) g_free(item->password);
        item->password = g_strdup(ch->password);
        return;
    }
    item = g_malloc0(sizeof(DoPasswordCache));
    if (ch->scheme)
        item->scheme = g_strdup(ch->scheme);
    if (ch->host)
        item->host = g_strdup(ch->host);
    item->port = ch->port;
    item->name = g_strdup(ch->name);
    if (ch->domain)
        item->domain = g_strdup(ch->domain);
    item->password = g_strdup(ch->password);
    cache = g_list_append(cache, item);
}
static void cache_item_delete(DoPasswordCache *ch)
{
    GList *l;
    DoPasswordCache *item;
    if (cache_item_find(ch, &l)) {
        item = l->data;
        cache = g_list_delete_link(cache, l);
        cache_item_free(item);
        return;
    }
}

DoPassword *do_password_get(GtkWidget *dialog, const gchar *auth_url, DoPasswordAuthentication *func, gpointer func_data)
{
    GList *l;
    DoPasswordCache *item;
    DoPasswordCache *ch;

    if (!auth_url) {
        gchar buf[1024] = {0, };
        gchar *user, *domain;
        DOMINO_PROFILE_GET(OBJECT_ROOT_PATH, "user-name", &user, "domain-name", &domain, NULL);
        if (user) {
            if (domain)
                sprintf(buf, "//%s.%s@", user, domain);
            else
                sprintf(buf, "//%s@", user);
        }
        ch = get_password_cache_from_url(buf);
    }
    else
        ch = get_password_cache_from_url(auth_url);

    if (cache_item_find(ch, &l)) {
        item = l->data;
        cache_item_free(ch);
        return g_object_new(DO_TYPE_PASSWORD, "name", item->name, "domain",
                                    item->domain, "password", item->password, NULL);
    }

	GtkWidget *w = dialog;
	if (!w || !DO_IS_PASSWORD_DIALOG(w))
        w = do_password_dialog_new(w, "Ввод пароля", DO_PASSWORD_DIALOG_FLAGS_SHOW_USERNAME	|
                                                                    DO_PASSWORD_DIALOG_FLAGS_EDIT_USERNAME |
                                                                    DO_PASSWORD_DIALOG_FLAGS_SHOW_PASSWORD);
    DoPassword *retval = NULL;
    gint response;

    if (ch->name)
        do_password_dialog_set_username(DO_PASSWORD_DIALOG(w), ch->name);
    if (ch->domain)
        do_password_dialog_set_domain(DO_PASSWORD_DIALOG(w), ch->domain);
    if (ch->password)
        do_password_dialog_set_password(DO_PASSWORD_DIALOG(w), ch->password);

    while ((response = gtk_dialog_run(GTK_DIALOG(w)) == GTK_RESPONSE_ACCEPT)) {
	    const gchar *val;
	    if (ch->name) g_free(ch->name);
	    ch->name = g_strdup(do_password_dialog_get_username(DO_PASSWORD_DIALOG(w)));
	    if (ch->domain) g_free(ch->domain);
	    val = do_password_dialog_get_domain(DO_PASSWORD_DIALOG(w));
	    if ( val ) {
            ch->domain = g_strdup(val);
        }
        else {
            ch->domain = NULL;
        }
	    if (ch->password) g_free(ch->password);
	    val = do_password_dialog_get_password(DO_PASSWORD_DIALOG(w));
	    if (val) {
            ch->password = g_strdup(val);
        }
        else {
            ch->password = NULL;
        }
	    cache_item_add(ch);
	    retval = g_object_new(DO_TYPE_PASSWORD, "name", ch->name, "domain",
                                    ch->domain, "password", ch->password, NULL);
        if (!func) {
            break;
        }

        if (func(retval, func_data)) {
            break;
        }

	    if (retval) {
            g_object_unref(retval);
        }
        retval = NULL;
	}
    if (!auth_url && retval)
        DOMINO_PROFILE_SET(OBJECT_ROOT_PATH, "", "user-name", ch->name, "domain-name", ch->domain, NULL);

	cache_item_free(ch);

    if (!dialog || !DO_IS_PASSWORD_DIALOG(dialog))
        gtk_widget_destroy (w);

    return retval;
}
void do_password_clear(const gchar *auth_url)
{
    DoPasswordCache *ch;

    ch = get_password_cache_from_url(auth_url);
    cache_item_delete(ch);
    cache_item_free(ch);
}

static DoPasswordCache *get_password_cache_from_url(const gchar *url)
{
    DoPasswordCache *retval;
    retval = g_malloc0(sizeof(DoPasswordCache));

    gchar *uri;
    gchar *ch;
    uri = (char*)url;

 	ch = strchr (uri, ':');
 	if (ch) {
        retval->scheme = g_ascii_strdown (uri, ch - uri);
        uri = ch + 1;
 	}
 	if ((*uri != '\\' && *uri != '/') &&
 	    (*(uri+1) != '\\' && *(uri+1) != '/'))
            return retval;
    uri+=2;
 	ch = strchr (uri, '@');
 	if (ch) {
 	    char *auth;
 	    auth = g_strndup(uri, ch - uri + 1);
 	    auth[ch - uri] = '\0';
 	    uri = ch + 1;
 	    ch = strchr(auth, '.');
 	    if (ch) {
            retval->name = g_strndup(auth, ch - auth + 1);
            retval->name[ch - auth] = '\0';
            auth = ch + 1;
 	    }
 	    ch = strchr(auth, ':');
 	    if (ch) {
 	        if (retval->name) {
                retval->domain = g_strndup(auth, ch - auth + 1);
                retval->domain[ch - auth] = '\0';
 	        }
 	        else {
                retval->name = g_strndup(auth, ch - auth + 1);
                retval->name[ch - auth] = '\0';
 	        }
            retval->password = g_strdup(ch + 1);
 	    }
 	    else {
 	        if (retval->name)
                retval->domain = g_strdup(auth);
 	        else
                retval->name = g_strdup(auth);
 	    }
        g_free(auth);
 	}
    if (*uri != '\0') {
        ch = strchr(uri, ':');
        if (ch) {
            retval->host = g_strndup(uri, ch - uri + 1);
            retval->host[ch - uri] = '\0';
            retval->port = atoi(ch + 1);
        }
        else
            retval->host = g_strdup(uri);
    }
    return retval;
}
