

#include "do_application.h"
#include "domino.h"
#include "do_utilx.h"
#include "do_notebook.h"
#include "do_html_view.h"
#include "do_window.h"
#include "do_view_actions.h"

#define DEFAULT_WINDOW_SIZE "800x500"
#define DEFAULT_WINDOW_POSITION "0x0"
#define DEFAULT_WINDOW_STATE NULL
#define OBJECT_ROOT_PATH "MainWindow"

#define DO_APPLICATION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_APPLICATION, DoApplicationPrivate))
#define DO_TYPE_APPLICATION_FLAGS do_application_flags_get_type()

enum
{
	PROP_0,
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
};

G_DEFINE_TYPE (DoApplication, do_application, GTK_TYPE_APPLICATION)

static void do_application_activate(GApplication *app);
//static gint do_application_command_line (GApplication *application, GApplicationCommandLine *cl);

static void do_application_init(DoApplication *temp)
{
	DoApplicationPrivate *priv;

	priv = temp->priv = DO_APPLICATION_GET_PRIVATE (temp);
	memset(priv, 0, sizeof(DoApplicationPrivate));
    GOptionEntry options[] =
    {
        {
            "version", 'V', 0, G_OPTION_ARG_NONE, NULL,
            "Показать версию", NULL
        },

        {
            G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_STRING_ARRAY, &priv->actions, NULL,
            "[ACTION…]"
        },
        {NULL}
    };

	g_application_add_main_option_entries (G_APPLICATION (temp), options);

}

static GObject *do_application_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject *object;

	//DoApplication *temp;
	//DoApplicationPrivate *priv;

	object = G_OBJECT_CLASS (do_application_parent_class)->constructor(type, n_construct_properties, construct_params);
	//priv = DO_APPLICATION(object)->priv;


    //g_signal_connect(object, "activate", G_CALLBACK(do_application_activate), object);
    //g_signal_connect(object, "command-line", G_CALLBACK (do_application_command_line), NULL);

	return object;
}

static void do_application_finalize (GObject *object)
{
	G_OBJECT_CLASS (do_application_parent_class)->finalize (object);
}

static void do_application_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	g_assert_not_reached ();
}

static void do_application_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	/*DoApplicationPrivate *priv = DO_APPLICATION (object)->priv;

	switch (prop_id)
	{
		case PROP_FLAGS:
			priv->flags = g_value_get_flags (value);
			break;
	}*/
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


	g_type_class_add_private (object_class, sizeof (DoApplicationPrivate));

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

GtkApplication *do_application_new (const gchar *application_id)
{
	return g_object_new (DO_TYPE_APPLICATION, //to do application_id
			     //"flags",  G_APPLICATION_HANDLES_COMMAND_LINE | G_APPLICATION_HANDLES_OPEN,
			     (gpointer) NULL);
}
static gboolean run_actions(DoApplication *app)
{
	DoApplicationPrivate *priv;
	GActionGroup *group;
	GAction *action;
	gint i;
    priv = DO_APPLICATION (app)->priv;
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
    priv = DO_APPLICATION (app)->priv;
    if ( !DOMINO_INIT ) {
        DOMINO_SHOW_ERROR("Окружение не инициализировано");
        return;
    }
    g_print("sad\n");//fix me
    priv->first_window = GTK_WIDGET(do_application_create_window(DO_APPLICATION(app), NULL));
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
