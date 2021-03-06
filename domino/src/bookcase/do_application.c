
#include "do_application.h"
#include "domino.h"
#include "do_client.h"
#include "do_utilx.h"
#include "do_notebook.h"
#include "do_window.h"
#include "do_common_actions.h"
#include "do_view_actions.h"

#define DEFAULT_WINDOW_SIZE "800x500"
#define DEFAULT_WINDOW_POSITION "0x0"
#define DEFAULT_WINDOW_STATE NULL

#define DEFAULT_SERVER_URL "http://hyperhost:8080/hs/ut_aptset_new/handbook"
#define DEFAULT_STORE "1"
#define DEFAULT_USER "domino"
#define DEFAULT_PASSWORD "123"

#define OBJECT_ROOT_PATH "MainWindow"
//fix meconst int min_protocol_version[] = {1,2,0};
const int min_protocol_version[] = {0,1,0};


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
	gboolean clear_cache;
	gboolean fullscreen;
	gboolean hide_control;
	GObject *client;
	gchar *store;
	gchar *protocol_server_version;
	gint connection_state;
};

G_DEFINE_TYPE_WITH_CODE (DoApplication, do_application, GTK_TYPE_APPLICATION, G_ADD_PRIVATE(DoApplication))

static void do_application_activate(GApplication *app);
static void do_application_add_acceletarors(GApplication *app);
static void quit_all (void);

//static gint do_application_command_line (GApplication *application, GApplicationCommandLine *cl);
//#ifdef DEBUG
DoApplication *application = NULL;
//#endif

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
            "clearcache", '\0', 0, G_OPTION_ARG_NONE, &priv->clear_cache, NULL,
        },
        {
            "fullscreen", 'F', 0, G_OPTION_ARG_NONE, &priv->fullscreen, NULL,
        },
        {
            "hidecontrol", 'h', 0, G_OPTION_ARG_NONE, &priv->hide_control, NULL,
        },

        {
            G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_STRING_ARRAY, &priv->actions, NULL,
            "[ACTION]"
        },
        {NULL}
    };

	g_application_add_main_option_entries (G_APPLICATION (temp), options);

}
#if GTK_CHECK_VERSION(3,12,0)
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
#endif
static GObject *do_application_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;

	//DoApplication *temp;
	//DoApplicationPrivate *priv;
	dominox_init();

	object = G_OBJECT_CLASS (do_application_parent_class)->constructor(type, n_construct_properties, construct_params);
	//priv = DO_APPLICATION_GET_PRIVATE(object);

    //g_signal_connect(object, "activate", G_CALLBACK(do_application_activate), object);
    //g_signal_connect(object, "command-line", G_CALLBACK (do_application_command_line), NULL);

//#ifdef DEBUG
    application = DO_APPLICATION(object);
//#endif
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
	if ( DO_IS_WINDOW(window) )
        g_print("window\n");

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

JsonNode *do_application_request2(DoApplication *app, const gchar *method, const gchar *func, const gchar *key, DoClientFlags flags, ...)
{
	DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE(app);
	JsonNode *node = NULL;
	va_list args;
    va_start (args, flags);
	node = do_client_request2_valist(DO_CLIENT(priv->client), method, func, key, flags, args);
    va_end (args);
	return node;
}
JsonNode *do_application_request2_async(DoApplication *app, const gchar *method, const gchar *func, const gchar *key, DoClientFlags flags, GFunc callback,gpointer data, ...)
{
	DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE(app);
	JsonNode *node = NULL;
	va_list args;
    va_start (args, data);
	node = do_client_request2_valist_async(DO_CLIENT(priv->client), method, func, key, flags, callback, data, args);
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
        gchar **version = g_strsplit(priv->protocol_server_version, ".", -1);
        gint i, n;

        for ( i = 0; i < G_N_ELEMENTS(min_protocol_version); i++ ) {
            if ( g_strv_length(version) <= i ) break;
            n = atoi(version[i]);
            if ( n < min_protocol_version[i] ) {
                DOMINO_SHOW_ERROR("Верcия сервера %s ниже допустимой %d.%d.%d", priv->protocol_server_version, min_protocol_version[0], min_protocol_version[1], min_protocol_version[2]);
            }
        }
        //json_node_unref(node);
	}
}
static void main_window_init(DoApplication *app)
{
    DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE(app);

    if ( priv->fullscreen )
        gtk_window_fullscreen(GTK_WINDOW(priv->first_window));
    if ( priv->hide_control ) {
#ifdef CURSOR_OFF
        GdkCursor *cursor = gdk_cursor_new(GDK_BLANK_CURSOR);
        gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(priv->first_window)), cursor);
#endif
    }
}

static gboolean run_actions(DoApplication *app)
{
	DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE(app);
	GActionGroup *group;
	GAction *action;
	GDateTime *time;
	gchar *buf;
	gchar *username = NULL;
	gchar *password = NULL;
	gint i;


    gchar *url = NULL;
    main_window_init(app);
    DOMINO_LOCAL_GET("main", "url", &url, "store", &priv->store, NULL);

	if ( !url )
        DOMINO_COMMON_GET("main", "url", &url, NULL);
	if ( !priv->store )
        DOMINO_COMMON_GET("main", "store", &priv->store, NULL);


	if ( url && priv->store )
        priv->client = do_client_new(url, priv->store, username ? username : DEFAULT_USER, password ? password : DEFAULT_PASSWORD);
    else {
        if ( !do_application_settings(app) )
            quit_all();
        DOMINO_LOCAL_GET("main", "url", &url, "store", &priv->store, NULL);
        priv->client = do_client_new(url, priv->store, username ? username : DEFAULT_USER, password ? password : DEFAULT_PASSWORD);
    }
    if ( priv->clear_cache ) {
        do_client_clear_cache(DO_CLIENT(priv->client), NULL);
    }
	time = g_date_time_new_now_local();
	buf = do_client_strftime(time);
	do_application_request2_async(app, "GET", "GetVersion", "Version", 0,
					(GFunc)do_application_set_protocol, app,
	                NULL);
	g_date_time_unref(time);g_free(buf);
#if GTK_CHECK_VERSION(3,16,0)
        group = gtk_widget_get_action_group(priv->first_window, "common-actions");
#else
	group = do_common_action_get_group();
#endif
	priv = DO_APPLICATION_GET_PRIVATE (app);
	if ( priv->actions )
        for ( i = 0; priv->actions[i]; i++ ) {
            gchar **val;
            GVariant *parameter;
            val = g_strsplit(priv->actions[i],".",-1);
            action = g_action_map_lookup_action(G_ACTION_MAP(group), val[0]);
            if ( action ) {
                parameter = NULL;
                if ( val[1] )
                    parameter = g_variant_new_string(val[1]);
                g_action_activate(action, parameter);
            }
            g_strfreev(val);
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
    domino_set_main_window(GTK_WINDOW(priv->first_window));
    do_application_add_acceletarors(app);
    //if ( priv->actions && priv->actions[0] )
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
GtkWindow *do_application_get_window(DoApplication *app, gboolean *open_dialog)
{
	GList *windows, *l;
	GtkWindow *window = NULL;
    if ( open_dialog ) *open_dialog = FALSE;
	windows = gtk_application_get_windows (GTK_APPLICATION (app));
	for (l = windows; l != NULL; l = g_list_next (l))
	{
		if ( DO_IS_WINDOW(l->data) ) {
            window = l->data;
		}
        if ( GTK_IS_DIALOG(l->data) && open_dialog ) {
            *open_dialog = TRUE;
        }
	}
    return window;
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
	do_client_set_cache(DO_CLIENT(priv->client), key, node, NULL, -1);

}

JsonNode *do_application_get_cache(DoApplication *app, const gchar *key, GDateTime  **time)
{
	DoApplicationPrivate *priv;
	priv = DO_APPLICATION_GET_PRIVATE(app);
	return do_client_get_cache(DO_CLIENT(priv->client), key, time);
}

static void do_application_add_acceletarors(GApplication *app)
{
#if GTK_CHECK_VERSION(3,12,0)
    add_accelerator(DO_APPLICATION(app), "common-actions.HtmlView", "<Primary>T");//fix me
    add_accelerator(DO_APPLICATION(app), "common-actions.Close", "<Primary>W");
    add_accelerator(DO_APPLICATION(app), "common-actions.Quit", "<Primary>Q");
    add_accelerator(DO_APPLICATION(app), "common-actions.Search", "<Primary>L");
#ifdef CASH
    add_accelerator(DO_APPLICATION(app), "view-actions.ApplyAction", "F12");
    add_accelerator(DO_APPLICATION(app), "view-actions.UndoApplyAction", "F2");
#else
    add_accelerator(DO_APPLICATION(app), "view-actions.ApplyAction", "<Shift>Return");
    add_accelerator(DO_APPLICATION(app), "view-actions.UndoApplyAction", "<Shift>Delete");
#endif
#endif
}
gboolean do_application_settings(DoApplication *app)
{
	DoApplicationPrivate *priv;
	priv = DO_APPLICATION_GET_PRIVATE(app);
    GtkWindow *win = NULL;
	if ( app ) {
        priv = DO_APPLICATION_GET_PRIVATE(app);
        if ( priv->first_window )
            win = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(priv->first_window)));
    }

#define N_DO_KEY_ENTRY_ 4
    GtkDialog *dialog;
    GtkWidget *l;
	GtkWidget *vbox;
	GtkWidget *entry[N_DO_KEY_ENTRY_];
	GtkWidget *notebook;
	GtkWidget *grid, *e;
	gint row;
	gboolean ret = FALSE;


    dialog =  (GtkDialog*)gtk_dialog_new_with_buttons(
                        "Парaметры",
                        win,
                        GTK_DIALOG_MODAL,
                        "Ok",
                        GTK_RESPONSE_ACCEPT,
                        "Отмена",
                        GTK_RESPONSE_REJECT,
                        NULL);
    /*g_signal_connect (dialog, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), "CommonEditDialog");*/

    gtk_dialog_set_default_response (dialog, GTK_RESPONSE_ACCEPT);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_icon_name (GTK_WINDOW(dialog), "bookcase");

	vbox = gtk_dialog_get_content_area(dialog);
	gtk_box_set_spacing(GTK_BOX(vbox), 6);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 6);
	notebook = gtk_notebook_new();
	gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);

	grid = gtk_grid_new();
	gtk_widget_set_size_request(grid, 500, -1);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), grid, gtk_label_new("Основные"));
	gtk_grid_set_row_spacing (GTK_GRID(grid), 6);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
	gtk_container_set_border_width(GTK_CONTAINER(grid), 6);

	row = 0;
    entry[0] = e = gtk_entry_new();
    l = gtk_label_new("Адрес сервера (url):");
#if GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(l), 0.0);
#else
    g_object_set(l,"xalign", 0.0, NULL);
#endif
    gtk_widget_set_hexpand(e, TRUE);
    gtk_grid_attach(GTK_GRID(grid), l, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), e, 1, row++, 1, 1);
    entry[1] = e = gtk_entry_new();
    l = gtk_label_new("Номер аптеки:");
#if GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(l), 0.0);
#else
    g_object_set(l,"xalign", 0.0, NULL);
#endif
    gtk_widget_set_hexpand(e, TRUE);
    gtk_grid_attach(GTK_GRID(grid), l, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), e, 1, row++, 1, 1);

    entry[2] = e = gtk_entry_new();
    l = gtk_label_new("Имя пользователя:");
#if GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(l), 0.0);
#else
    g_object_set(l,"xalign", 0.0, NULL);
#endif
    gtk_widget_set_hexpand(e, TRUE);
    gtk_grid_attach(GTK_GRID(grid), l, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), e, 1, row++, 1, 1);

    entry[3] = e = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(e), FALSE);
    l = gtk_label_new("Пароль пользователя:");
#if GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(l), 0.0);
#else
    g_object_set(l,"xalign", 0.0, NULL);
#endif
    gtk_widget_set_hexpand(e, TRUE);
    gtk_grid_attach(GTK_GRID(grid), l, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), e, 1, row++, 1, 1);

	gtk_widget_show_all (vbox);
	gchar *url = NULL, *store = NULL, *user = NULL, *password = NULL;
    DOMINO_LOCAL_GET("main", "url", &url, "store", &store, "user", &user, "password", &password, NULL);
	if ( !url )
        DOMINO_COMMON_GET("main", "url", &url, NULL);
	if ( !store )
        DOMINO_COMMON_GET("main", "store", &store, NULL);
    gtk_entry_set_text(GTK_ENTRY(entry[0]), url ? url : DEFAULT_SERVER_URL);
    gtk_entry_set_text(GTK_ENTRY(entry[1]), store ? store : DEFAULT_STORE);
    gtk_entry_set_text(GTK_ENTRY(entry[2]), user ? user : DEFAULT_USER);
    gtk_entry_set_text(GTK_ENTRY(entry[3]), password ? password : DEFAULT_PASSWORD);

    while ( gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT ) {
        g_object_get(entry[0], "text", &url, NULL);
        g_object_get(entry[1], "text", &store, NULL);
        g_object_get(entry[2], "text", &user, NULL);
        g_object_get(entry[3], "text", &password, NULL);
        DOMINO_LOCAL_SET("main","url", url, "store", store, "user", user, "password", password, NULL);
        if ( domino_config_save(DOMINO_CONFIG_LOCAL, TRUE) ) {
            if ( !priv->client )
                priv->client = do_client_new(url, priv->store, user, password);
            else
                if ( g_strcmp0(url, do_client_get_url(DO_CLIENT(priv->client))) ||
                     g_strcmp0(store, do_client_get_url(DO_CLIENT(priv->client)))
                    ) {
                    DOMINO_SHOW_INFO("Требуется перезапуск программы");
                }
            ret = TRUE;
            break;
        }
        else {
            // to do
            DOMINO_SHOW_ERROR("Невозможно сохранить параметры. Возможно нехватает прав, попробуйте запустить первый раз под администратором.");
        }

    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
    return ret;
}
GObject *do_application_get_client(DoApplication *app)
{
    DoApplicationPrivate *priv;
	priv = DO_APPLICATION_GET_PRIVATE(app);
    return priv->client;
}
void do_application_log_func(const gchar *log_domain, GLogLevelFlags log_flags, const gchar *msg, gpointer user_data)
{
    int level;
    if ( log_flags & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION | G_LOG_FLAG_FATAL) )
        level = LOG_ERR;
    else if ( log_flags & G_LOG_LEVEL_WARNING )
        level = LOG_WARNING;
    else
        level = LOG_INFO;
    if ( msg && strlen(msg) > 0 && msg[strlen(msg)-1] == '\n' ) {
        gchar *buf;
        buf = g_strdup(msg);
        buf[strlen(buf) - 1] = '\0';
        do_log(level, buf);
        g_free(buf);
    }
    else
        do_log(level, msg);
    g_print("%s", msg);
}
inline DoApplication *do_application_get_default()
{
    return application;
}
#ifdef DEBUG
void do_application_set_info_label(DoApplication *app, const gchar *label)
{
    GList *l;
    for ( l = gtk_application_get_windows(GTK_APPLICATION(app)); l; l = l->next ) {
        if ( DO_IS_WINDOW(l->data) ) {
            do_window_set_footerbar_text(DO_WINDOW(l->data), label);
            break;
        }
    }

}
#endif
void do_application_set_connection_state(DoApplication *app, gint state)
{
    DoApplicationPrivate *priv = DO_APPLICATION_GET_PRIVATE(app);
    if ( priv->connection_state != state ) {
        DoWindow *window;
        priv->connection_state = state;
        window = DO_WINDOW(do_application_get_window(app, NULL));
        if ( window ) {
            do_window_show_connection_state(window, priv->connection_state);
        }
    }
}
