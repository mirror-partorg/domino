
#include "do_application.h"
#include "domino.h"
#include "do_client.h"
#include "do_utilx.h"
#include "do_notebook.h"
#include "do_window.h"
#include "do_view_actions.h"

#define DEFAULT_WINDOW_SIZE "800x500"
#define DEFAULT_WINDOW_POSITION "0x0"
#define DEFAULT_WINDOW_STATE NULL
#define DEFAULT_SERVER_URL "http://192.168.8.2:15080/bookcase"

#define OBJECT_ROOT_PATH "MainWindow"


#define DO_APPLICATION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_APPLICATION, DoApplicationPrivate))
#define DO_TYPE_APPLICATION_FLAGS do_application_flags_get_type()

enum
{
	PROP_0,
	PROP_STORE,
	PROP_CACHED,
//	PROP_FLAGS
};

/*
GType do_application_flags_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GFlagsValue _do_application_flags_values[] = {
         { DO_APPLICATION_FLAGS_NONE, "DO_APPLICATION_FLAGS_NONE", "" },
         { 0, NULL, NULL }
        };
        type = g_flags_register_static ("DoApplicationFlags", _do_application_flags_values);
    }
    return type;
}
*/

struct _DoApplicationPrivate
{
	//DoApplicationFlags flags;
	GApplicationCommandLine *command_line;
	GtkWidget *first_window;
	gchar **actions;
	GObject *client;
	gchar *store;
	gchar *protocol_server_version;
};

G_DEFINE_TYPE_WITH_CODE (DoApplication, do_application, GTK_TYPE_APPLICATION, G_ADD_PRIVATE(DoApplication))

static void do_application_activate(GApplication *app);
static void do_application_add_acceletarors(GApplication *app);

//static gint do_application_command_line (GApplication *application, GApplicationCommandLine *cl);

static void do_application_init(DoApplication *temp)
{
	DoApplicationPrivate *priv;

	priv = DO_APPLICATION_GET_PRIVATE (temp);
	//memset(priv, 0, sizeof(DoApplicationPrivate));
    GOptionEntry options[] =
    {
        {
            "version", 'V', 0, G_OPTION_ARG_NONE, NULL,
        },

        {
            G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_STRING_ARRAY, &priv->actions, NULL,
            "[ACTION]"
        },
        {NULL}
    };

	g_application_add_main_option_entries (G_APPLICATION (temp), options);

}
static gchar *do_application_store_dialog(GtkWindow *parent)
{
	return "5";//to do
}
static void add_accelerator (DoApplication *app,
                 const gchar    *action_name,
                 const gchar    *accel)
{
    const gchar *vaccels[] = {
        accel,
        NULL
    };

    gtk_application_set_accels_for_action (GTK_APPLICATION(app), action_name, vaccels);
}

static GObject *do_application_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;

	//DoApplication *temp;
	DoApplicationPrivate *priv;
	dominox_init();

	object = G_OBJECT_CLASS (do_application_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv = DO_APPLICATION_GET_PRIVATE(object);
	gchar *url = NULL;
	DOMINO_COMMON_GET("main", "url", &url, "store", &priv->store, NULL);
	if ( !url )
		url = DEFAULT_SERVER_URL;
	if ( !priv->store )
		priv->store = g_strdup(do_application_store_dialog(NULL));
	DOMINO_COMMON_SET("main", "store", priv->store, "url", url, NULL);

	priv->client = do_client_new(url,priv->store);

    //g_signal_connect(object, "activate", G_CALLBACK(do_application_activate), object);
    //g_signal_connect(object, "command-line", G_CALLBACK (do_application_command_line), NULL);

	return object;
}

static void do_application_finalize (GObject *object)
{
	DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE (object);

	G_OBJECT_CLASS (do_application_parent_class)->finalize (object);
	g_free(priv->store);
	dominox_finalize();
}

GList *
do_application_get_main_windows (DoApplication *app)
{
	GList *res = NULL;
	GList *windows, *l;

	g_return_val_if_fail (DO_IS_APPLICATION(app), NULL);

	windows = gtk_application_get_windows (GTK_APPLICATION (app));
	for (l = windows; l != NULL; l = g_list_next (l))
	{
		if (DO_IS_WINDOW (l->data))
		{
			res = g_list_prepend (res, l->data);
		}
	}

	return g_list_reverse (res);
}
static void
quit_all (void)
{
	GList *windows;
	//GList *l;
	GApplication *app;

	app = g_application_get_default ();
	windows = do_application_get_main_windows (DO_APPLICATION(app));

	if (windows == NULL)
	{
		g_application_quit (app);
		return;
	}
    // to do close all
	g_list_free (windows);
}
void
_do_cmd_file_quit (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       user_data)
{
	DoWindow *window = DO_WINDOW (user_data);

	if (window == NULL)
	{
		quit_all ();
		return;
	}

	gtk_widget_destroy (GTK_WIDGET (window));

	GtkApplication *app;

    app = GTK_APPLICATION (g_application_get_default ());

	if (gtk_application_get_windows (app) == NULL)
	{
		g_application_quit (G_APPLICATION (app));
	}
}
static gboolean
window_delete_event (DoWindow *window,
                     GdkEvent    *event,
                     DoApplication    *app)
{

	_do_cmd_file_quit (NULL, NULL, window);

	/* Do not destroy the window */
	return TRUE;
}
static DoWindow *
do_application_create_window_impl (DoApplication *app)
{
	DoWindow *window;

	window = g_object_new (DO_TYPE_WINDOW, "application", app, NULL);

	g_signal_connect (window,
			  "delete_event",
			  G_CALLBACK (window_delete_event),
			  app);

	return window;
}
static void do_application_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_STORE:
			g_value_set_string(value, priv->store);
	        break;
	    case PROP_CACHED: {
	    	gboolean res;
	    	g_object_get(priv->client, "cached", &res, NULL);
			g_value_set_boolean(value, res);
	        break;
	    }
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}
}

static void do_application_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE(object);

	switch (prop_id)
	{
		case PROP_STORE:
			if ( g_strcmp0(priv->store, g_value_get_string(value)) ) {
				g_assert(g_value_get_string(value) != NULL);
				g_free(priv->store);
				priv->store = g_value_dup_string(value);
				if ( priv->client )
					g_object_set(priv->client, "store", priv->store, NULL);
			}
			break;
		case PROP_CACHED:
	    	g_object_set(priv->client, "cached", g_value_get_boolean(value), NULL);
	        break;
	}
}

static void do_application_class_init (DoApplicationClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

	object_class->constructor  = do_application_constructor;
	object_class->finalize     = do_application_finalize;
	object_class->get_property = do_application_get_property;
	object_class->set_property = do_application_set_property;



	//app_class->command_line = do_application_command_line;
    app_class->activate = do_application_activate;

	klass->create_window = do_application_create_window_impl;

	//g_type_class_add_private (object_class, sizeof (DoApplicationPrivate));

    g_object_class_install_property
        (object_class,
         PROP_STORE,
         g_param_spec_string("store",
                      "Код аптеки",
                      "Код аптеки",
                      NULL,
                       G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
                                     PROP_CACHED,
                                     g_param_spec_boolean ("cached",
                                                          "Кэширование ответов",
                                                          "Кэширование ответов",
                                                           TRUE,
                                                           G_PARAM_READWRITE));
	/*g_object_class_install_property
		(object_class,
		 PROP_FLAGS,
		 g_param_spec_flags ("flags",
				     "flags",
				     "flags",
				     DO_TYPE_APPLICATION_FLAGS,
				     DO_APPLICATION_FLAGS_DEFAULT,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));*/
}

JsonNode *do_application_request(DoApplication *app, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache, ...)
{
	DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE(app);
	JsonNode *node = NULL;
	va_list args;
    va_start (args, nocache);
	node = do_client_request_valist(DO_CLIENT(priv->client), method, func, key, archive, nocache,args);
    va_end (args);
	return node;
}
JsonNode *do_application_request_async(DoApplication *app, const gchar *method, const gchar *func, const gchar *key, gboolean archive, gboolean nocache,GFunc callback,gpointer data, ...)
{
	DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE(app);
	JsonNode *node = NULL;
	va_list args;
    va_start (args, data);
	node = do_client_request_valist_async(DO_CLIENT(priv->client), method, func, key, archive, nocache, callback, data, args);
    va_end (args);
    return node;
}
static void do_application_set_protocol(JsonNode *node, DoApplication *app)
{
	if ( node ) {
		DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE(app);
        JsonObject *obj;
        JsonNode *member;
        obj = json_node_get_object(node);
        g_assert(obj != NULL);
        member = json_object_get_member(obj, "protocol");
        g_assert(member != NULL && json_node_get_value_type(member) == G_TYPE_STRING);
        priv->protocol_server_version = g_strdup(json_node_get_string(member));
        //json_node_unref(node);
	}
}
static gboolean run_actions(DoApplication *app)
{
	DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE(app);
	GActionGroup *group;
	GAction *action;
	GDateTime *time;
	gchar *buf;
	gint i;

	time = g_date_time_new_now_local();
	buf = do_client_strftime(time);
	do_application_request_async(app, "GET", "GetVersion", "Version", FALSE, FALSE,
					(GFunc)do_application_set_protocol, app,
	                NULL);
	g_date_time_unref(time);g_free(buf);
    group = gtk_widget_get_action_group(priv->first_window, "common-actions");
	priv = DO_APPLICATION_GET_PRIVATE (app);
    for ( i = 0; priv->actions[i]; i++ ) {
        action = g_action_map_lookup_action(G_ACTION_MAP(group),priv->actions[i]);
        if ( action )
            g_action_activate(action,NULL);
    }
    return FALSE;
}
static void do_application_activate(GApplication *app)
{
	DoApplicationPrivate *priv;
    priv = DO_APPLICATION_GET_PRIVATE(app);
    if ( !DOMINO_INIT ) {
        DOMINO_SHOW_ERROR("Окружение не инициализировано");
        return;
    }
    priv->first_window = GTK_WIDGET(do_application_create_window(DO_APPLICATION(app), NULL));
    do_application_add_acceletarors(app);
    if ( priv->actions && priv->actions[0] )
        g_idle_add((GSourceFunc)run_actions, app);
}
DoWindow *do_application_create_window (DoApplication *app, GdkScreen *screen)
{
	DoWindow *window;

	window = DO_APPLICATION_GET_CLASS (app)->create_window (app);

	if (screen != NULL)
	{
		gtk_window_set_screen (GTK_WINDOW (window), screen);
	}

	return window;
}
GtkWidget *do_application_get_focus_child(DoApplication *app)
{
	GList *windows, *l;

	windows = gtk_application_get_windows (GTK_APPLICATION (app));
	for (l = windows; l != NULL; l = g_list_next (l))
	{
		if (DO_IS_WINDOW (l->data))
		{
			return do_window_get_active_child(DO_WINDOW(l->data));
		}
	}
    return NULL;
}
gboolean do_application_cancel_request(DoApplication *app, const gchar *key)
{
	DoApplicationPrivate *priv;
    priv = DO_APPLICATION_GET_PRIVATE(app);
    return do_client_cancel_request(DO_CLIENT(priv->client), key);
}

/*
static void
clear_options (DoApplication *app)
{
	DoApplicationPrivate *priv;

	priv = DO_APPLICATION_GET_PRIVATE (app);

	g_slist_free_full (priv->action_list, g_object_unref);

	priv->command_line = NULL;
}*/
/*
static gint do_application_command_line(GApplication *application, GApplicationCommandLine *cl)
{
	DoApplicationPrivate *priv;
	GVariantDict *options;
	const gchar **remaining_args = NULL;

	priv = DO_APPLICATION_GET_PRIVATE(application);

	options = g_application_command_line_get_options_dict(cl);

    if (g_variant_dict_lookup(options, G_OPTION_REMAINING, "&s", &remaining_args))
	{
		gint i;
		for ( i = 0; remaining_args[i]; i++ )
            priv->action_list = g_slist_prepend (priv->action_list, g_strdup(remaining_args[i]));
    }
	g_application_activate (application);
	clear_options (DO_APPLICATION(application));
	return 0;
}
*/
void do_application_set_cache(DoApplication *app, const gchar *key, JsonNode *node)
{
	DoApplicationPrivate *priv;
	priv = DO_APPLICATION_GET_PRIVATE(app);
	do_client_set_cache(DO_CLIENT(priv->client), key, node);

}
JsonNode *do_application_get_cache(DoApplication *app, const gchar *key)
{
	DoApplicationPrivate *priv;
	priv = DO_APPLICATION_GET_PRIVATE(app);
	return do_client_get_cache(DO_CLIENT(priv->client), key);
}

static void do_application_add_acceletarors(GApplication *app)
{
    add_accelerator(DO_APPLICATION(app), "common-actions.Quit", "<Primary>Q");
}
