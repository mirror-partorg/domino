

#include "do_keyboard.h"
#include "do_keyboard_view.h"
#include "do_product_view.h"
#include "do_product_model.h"
#include "do_view.h"
#include "do_icons.h"
#include "do_utilx.h"
#include "domino.h"
#include <webkit/webkit.h>

#define DO_KEYBOARD_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_KEYBOARD, DoKeyboardPrivate))

enum
{
	PROP_0,
	PROP_ALIAS,
	PROP_LOAD_STATUS,
	PROP_LOAD_PROGRESS,

};

#define OBJECT_ROOT_PATH "Keyboard"
#define DEFAULT_PANED_POSITION "100"

// View interface
static void do_keyboard_view_init(DoViewIface *iface);

static gboolean view_close_request(DoView *view);

static void          view_close(DoView *view);
static const char   *view_get_title(DoView *view);
static GdkPixbuf    *view_get_icon			(DoView *view);
static gboolean	     view_get_load_status	(DoView *view);
static gboolean	     view_get_load_progress	(DoView *view);
static gboolean	     view_do_grab_focus(DoView *view);
static DoView       *do_keyboard_view_get_active_child(DoView *view);

static void  go_sender(DoKeyboardView *kview, DoKeyboard *view);
static void  print_template(DoKeyboardView *kview, DoKeyboard *view);

static void do_keyboard_setting_save (DoKeyboard* widget);
static void sync_load_status (DoProductModel *model, GParamSpec *pspec, GtkWidget *proxy);
static void sync_load_progress (DoProductModel *model, GParamSpec *pspec, GtkWidget *proxy);

static gboolean paned_sync_position(GtkWidget *paned, GParamSpec *pspec, DoKeyboard *view);
struct _DoKeyboardPrivate
{
	GtkWidget    *k_view;
	GtkWidget    *p_view;
	GtkWidget    *h_view;
    GtkWidget    *notebook;
    GdkPixbuf    *icon;
    gint          p_view_indx;
    gint          h_view_indx;
    gchar        *alias_name;
};

//G_DEFINE_TYPE (DoKeyboard, do_keyboard, GTK_TYPE_VBOX)
G_DEFINE_TYPE_WITH_CODE (DoKeyboard, do_keyboard, GTK_TYPE_PANED,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_keyboard_view_init)
                            );

static void do_keyboard_view_init(DoViewIface *iface)
{
    iface->do_close        = view_close;
    iface->close_request   = view_close_request;
    iface->get_title       = view_get_title;
    iface->get_icon        = view_get_icon;
    iface->get_load_status = view_get_load_status;
    iface->get_load_progress = view_get_load_progress;
    iface->do_grab_focus   = view_do_grab_focus;
    iface->get_active_child = do_keyboard_view_get_active_child;
}
#define ICON_SIZE 16

static void do_keyboard_init(DoKeyboard *temp)
{
	DoKeyboardPrivate *priv = DO_KEYBOARD_GET_PRIVATE (temp);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, DO_STOCK_DOMINO_KEYBOARD, DO_VIEW_ICON_SIZE, 0, NULL);

}

static GObject *do_keyboard_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject            *object;
	DoKeyboardPrivate  *priv;
	GtkPaned           *hpaned;
	GtkWidget          *s;
	GtkWidget          *l;

	object = G_OBJECT_CLASS (do_keyboard_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv = DO_KEYBOARD_GET_PRIVATE (object);
	hpaned = GTK_PANED(object);

    g_object_set(object, "orientation", GTK_ORIENTATION_HORIZONTAL, NULL);

	priv->k_view = do_keyboard_view_new(priv->alias_name);
    g_signal_connect(priv->k_view,  "go-sender", G_CALLBACK(go_sender), object);
    g_signal_connect(priv->k_view,  "print-template", G_CALLBACK(print_template), object);
    gtk_paned_add1(GTK_PANED (hpaned), priv->k_view);
    if (FALSE)
       gtk_widget_set_size_request(priv->k_view, 350u, 500u);
    gtk_widget_show(priv->k_view);

    priv->notebook = gtk_notebook_new();
    gtk_paned_add2(GTK_PANED (hpaned), priv->notebook);

	g_signal_connect(hpaned, "notify::position",
			  G_CALLBACK (paned_sync_position), object);
    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(hpaned), OBJECT_ROOT_PATH,
                               "paned-position", DEFAULT_PANED_POSITION, NULL);

    priv->p_view = do_product_view_new(priv->alias_name, domino_unit(),
                             //DO_PRODUCT_VIEW_FLAGS_LOAD_IN_THREAD |
                             DO_PRODUCT_VIEW_FLAGS_SELECTION_MULTIPLE |
                             DO_PRODUCT_VIEW_FLAGS_ROW_ACTIVATED_TO_SEND |
                             DO_PRODUCT_VIEW_FLAGS_SHOW_RECEIVED );
    g_object_set(priv->p_view, "receiver-object", priv->k_view, NULL);
    g_object_set(priv->k_view, "sender-object", priv->p_view, NULL);
    g_object_set(priv->k_view, "flags", DO_KEYBOARD_VIEW_FLAGS_PRINT_TEMPLATE, NULL);

    s = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(s), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    priv->h_view = webkit_web_view_new();
    if (FALSE)
       gtk_widget_set_size_request(priv->h_view, 650u, -1);
    gtk_container_add(GTK_CONTAINER (s),  priv->h_view);

    g_object_set(priv->k_view, "template-object", priv->h_view, NULL);

    gtk_widget_show(priv->h_view);
    gtk_widget_show(s);
    gtk_widget_show(priv->p_view);

    l = gtk_label_new("Клавиатура ");
    priv->h_view_indx = gtk_notebook_append_page(GTK_NOTEBOOK(priv->notebook), s, l);
    l = gtk_label_new("Товары ");
    priv->p_view_indx = gtk_notebook_append_page(GTK_NOTEBOOK(priv->notebook), priv->p_view, l);

	g_signal_connect_object (priv->k_view, "notify::load-status",
				 G_CALLBACK (sync_load_status), object, 0);
	g_signal_connect_object (priv->p_view, "notify::load-status",
				 G_CALLBACK (sync_load_status), object, 0);
	g_signal_connect_object (priv->p_view, "notify::load-progress",
				 G_CALLBACK (sync_load_progress), object, 0);

    gtk_widget_show(priv->notebook);
    gtk_widget_show(GTK_WIDGET(hpaned));
    do_keyboard_view_show_template(DO_KEYBOARD_VIEW(priv->k_view));

	return object;
}

static void do_keyboard_finalize (GObject *object)
{
	DoKeyboardPrivate *priv = DO_KEYBOARD_GET_PRIVATE (object);
	if (priv->alias_name)
        g_free(priv->alias_name);
	G_OBJECT_CLASS (do_keyboard_parent_class)->finalize (object);
}
static void do_keyboard_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoKeyboardPrivate *priv = DO_KEYBOARD_GET_PRIVATE (object);

	switch (prop_id)
	{
		case PROP_ALIAS:
            if (!priv->alias_name)
                priv->alias_name = g_value_dup_string(value);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
	}

}
static void do_keyboard_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoKeyboardPrivate *priv = DO_KEYBOARD_GET_PRIVATE (object);
    switch (prop_id)
    {
		case PROP_ALIAS:
			g_value_set_string (value, priv->alias_name);
			break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
        case PROP_LOAD_STATUS: {
            g_value_set_boolean(value, do_view_get_load_status(DO_VIEW(priv->h_view)) ||
                                       do_view_get_load_status(DO_VIEW(priv->p_view)));
            break;
        }
        case PROP_LOAD_PROGRESS: {
            g_value_set_boolean(value,
                                       do_view_get_load_progress(DO_VIEW(priv->p_view)));
            break;
        }
    }
}

static void do_keyboard_class_init (DoKeyboardClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_keyboard_constructor;
	object_class->finalize     = do_keyboard_finalize;
	object_class->get_property = do_keyboard_get_property;
	object_class->set_property = do_keyboard_set_property;

	g_type_class_add_private (object_class, sizeof (DoKeyboardPrivate));

    g_object_class_install_property
        (object_class,
         PROP_ALIAS,
         g_param_spec_string("alias-name",
                      "Алиас БД",
                      "Алиас БД",
                      NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property (object_class,
				   PROP_LOAD_STATUS,
				   g_param_spec_boolean("load-status",
							"Статус загрузки",
							"Справочник загружается",
							FALSE,
							G_PARAM_READABLE));
    g_object_class_install_property (object_class,
				   PROP_LOAD_PROGRESS,
				   g_param_spec_int("load-progress",
							"Статус загрузки",
							"Справочник загружается",
							0,100,0,
							G_PARAM_READABLE));
}

GtkWidget *do_keyboard_new (const gchar *alias_name)
{
	return g_object_new (DO_TYPE_KEYBOARD,
	                     "alias-name", alias_name,
			             (gpointer) NULL);
}

static gboolean view_close_request(DoView *view)
{
    return TRUE;
}
static void view_close(DoView *view)
{
    gtk_widget_destroy(GTK_WIDGET(view));
}
static const char *view_get_title(DoView *view)
{
    return "Горячие клавиши терминалов";
}
static GdkPixbuf    *view_get_icon			(DoView *view)
{
	DoKeyboardPrivate  *priv;
	priv = DO_KEYBOARD_GET_PRIVATE (view);

    return priv->icon;
}
static gboolean	  view_get_load_status	(DoView *view)
{
	DoKeyboardPrivate  *priv;
	priv = DO_KEYBOARD_GET_PRIVATE (view);

    return do_view_get_load_status(DO_VIEW(priv->p_view)) ||
           do_view_get_load_status(DO_VIEW(priv->k_view));
}
static gboolean	  view_get_load_progress	(DoView *view)
{
	DoKeyboardPrivate  *priv;
	priv = DO_KEYBOARD_GET_PRIVATE (view);

    return do_view_get_load_progress(DO_VIEW(priv->p_view));
}
static gboolean	  view_do_grab_focus(DoView *view)
{
	DoKeyboardPrivate  *priv;
	priv = DO_KEYBOARD_GET_PRIVATE (view);

    return do_view_do_grab_focus(DO_VIEW(priv->k_view));
}
static void  go_sender(DoKeyboardView *kview, DoKeyboard *view)
{
	DoKeyboardPrivate  *priv;
	priv = DO_KEYBOARD_GET_PRIVATE (view);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), priv->p_view_indx);
}
static void  print_template(DoKeyboardView *kview, DoKeyboard *view)
{
	DoKeyboardPrivate  *priv;
	priv = DO_KEYBOARD_GET_PRIVATE (view);

    GtkPrintOperation *operation;
    GError *error = NULL;

    operation = gtk_print_operation_new ();

    gtk_print_operation_set_use_full_page (operation, FALSE);
    gtk_print_operation_set_unit (operation, GTK_UNIT_POINTS);

    webkit_web_frame_print_full(webkit_web_view_get_main_frame (
        WEBKIT_WEB_VIEW (priv->h_view)),
                                     operation,
                                     GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
                                     &error);

    g_object_unref (operation);

    if (error)  {
        DOMINO_SHOW_ERROR("%s", error->message);
        g_error_free (error);

    }

}
static gboolean paned_sync_position(GtkWidget *paned, GParamSpec *pspec, DoKeyboard *view)
{
	do_keyboard_setting_save(view);
	return TRUE; //!!
}
static void do_keyboard_setting_save (DoKeyboard* widget)
{
    char buffer[512];
    sprintf(buffer, "%d", gtk_paned_get_position(GTK_PANED(widget)));
    DOMINO_PROFILE_SET(OBJECT_ROOT_PATH, "DoKeyboard", "paned-position", buffer, NULL);
}

static void sync_load_status (DoProductModel *model, GParamSpec *pspec, GtkWidget *proxy)
{
    g_object_notify (G_OBJECT (proxy), "load-status");
}
static void sync_load_progress (DoProductModel *model, GParamSpec *pspec, GtkWidget *proxy)
{
    g_object_notify (G_OBJECT (proxy), "load-progress");
}
static DoView  *do_keyboard_view_get_active_child(DoView *view)
{
	DoKeyboardPrivate  *priv;
	GtkWidget *focus;
	priv = DO_KEYBOARD_GET_PRIVATE (view);
	focus = gtk_container_get_focus_child(GTK_CONTAINER(view));
	if (focus == priv->k_view)
        return (DoView*)priv->k_view;
	if (focus == priv->notebook) {
        focus = gtk_container_get_focus_child(GTK_CONTAINER(priv->notebook));
        if (focus == priv->p_view)
            return (DoView*)priv->p_view;
        if (focus == priv->h_view)
            return (DoView*)priv->h_view;
        return view;
	}
	else
        return view;
}
