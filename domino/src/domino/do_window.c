
#include "do_window.h"
#include "do_view_actions.h"
#include "do_view.h"
#include "do_common_actions.h"
#include "do_notebook.h"
#include "do_tabs_menu.h"
#include "do_icons.h"
#include "domino.h"

#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <time.h>

static void do_window_class_init		(DoWindowClass *klass);
static void do_window_init			(DoWindow *gs);
static GObject *do_window_constructor		(GType type,
						 guint n_construct_properties,
						 GObjectConstructParam *construct_params);

static void do_window_finalize		(GObject *object);

static void notebook_switch_page_cb		(GtkNotebook *notebook,
						 GtkWidget *page,
						 guint page_num,
						 DoWindow *window);
static void do_window_set_active_tab (DoWindow *window, DoView *new_embed);
static void notebook_page_close_request_cb (DoNotebook *notebook,
				DoView *embed,
				DoWindow *window);

static gboolean window_state_event(GtkWidget* window, GdkEventWindowState* event);
static void sync_load_progress (DoNotebook *notebook, GParamSpec *pspec, GtkWidget *proxy);
static void set_progress(DoWindow *window, int progress);
static void ui_reload(DoWindow *window);

#define DEFAULT_WINDOW_SIZE "800x500"
#define DEFAULT_WINDOW_POSITION "0x0"
#define DEFAULT_WINDOW_STATE NULL
#define OBJECT_ROOT_PATH "MainWindow"

#define DO_WINDOW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_WINDOW, DoWindowPrivate))


struct _DoWindowPrivate
{
	GtkWidget    *main_vbox;
	GtkWidget    *menu_dock;
	GtkWidget    *toolbar;
	GtkWidget    *statusbar;
	GtkWidget    *statusbox;
	GtkAction    *clock;
	gboolean      set_label;
	GtkUIManager *manager;
	GtkNotebook  *notebook;
	DoTabsMenu   *tabs_menu;
	DoView       *active_view;
	gchar        *ui;
    GtkWidget    *progress_bar;
    guint         clock_event_source;
#if GTK_CHECK_VERSION(3,10,0)
    GtkWidget    *titlebar;
#endif
};

enum
{
	PROP_0,
	PROP_ACTIVE_CHILD,
	PROP_UI,
};

static void impl_set_active_child (GtkContainer *container,
		       GtkWidget *child)
{
	int page;
	DoWindow *window;

	window = DO_WINDOW (container);

	page = gtk_notebook_page_num
		(window->priv->notebook, GTK_WIDGET (child));
	gtk_notebook_set_current_page
		(window->priv->notebook, page);
}

static GList *
impl_get_children (DoWindow *window)
{
	//EphyWindow *window = DO_WINDOW (window);

	return gtk_container_get_children (GTK_CONTAINER (window->priv->notebook));
}

static void do_window_set_property (GObject *object,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *pspec)
{
	DoWindow *window = DO_WINDOW (object);
	DoWindowPrivate *priv = window->priv;

	switch (prop_id)
	{
		case PROP_ACTIVE_CHILD:
			impl_set_active_child (GTK_CONTAINER (window),
					       g_value_get_object (value));
			break;
		case PROP_UI:
            if (priv->ui)
                g_free(priv->ui);
			priv->ui = g_value_dup_string(value);
			ui_reload(window);
			break;
	}
}

G_DEFINE_TYPE (DoWindow, do_window, GTK_TYPE_WINDOW )

static void do_window_get_property (GObject *object,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *pspec)
{
	DoWindow *window = DO_WINDOW (object);
	DoWindowPrivate *priv = window->priv;

	switch (prop_id)
	{
		case PROP_ACTIVE_CHILD:
			g_value_set_object (value, window->priv->active_view);
			break;
		case PROP_UI:
		    g_value_set_string(value, priv->ui);
			break;
	}
}
// Signals


static void do_window_class_init (DoWindowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_window_constructor;
	object_class->finalize     = do_window_finalize;
	object_class->get_property = do_window_get_property;
	object_class->set_property = do_window_set_property;
    g_object_class_install_property (object_class,
                                     PROP_ACTIVE_CHILD,
                                         g_param_spec_object ("active-child", NULL, NULL,
                                                              GTK_TYPE_WIDGET /* Can't use an interface type here */,
                                                              G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_UI,
                                         g_param_spec_string ("ui", NULL, NULL,
                                                              NULL,
                                                              G_PARAM_READWRITE ));

	g_type_class_add_private (object_class, sizeof (DoWindowPrivate));
}

static void do_window_init (DoWindow *window)
{
	window->priv = DO_WINDOW_GET_PRIVATE (window);
}

GtkWidget *do_window_get_mainmenu(DoWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	return priv->menu_dock;
}
static GObject *do_window_constructor (GType type,
			 guint n_construct_properties,
			 GObjectConstructParam *construct_params)
{
	GObject *object;
	DoWindow *window;
	DoWindowPrivate *priv;


    GtkUIManager *manager;

    //GThread *self;
    //self = g_thread_self ();

	object = G_OBJECT_CLASS (do_window_parent_class)->constructor
		(type, n_construct_properties, construct_params);

	window = DO_WINDOW (object);
    gtk_window_set_icon_name(GTK_WINDOW (window), DO_STOCK_DOMINO_APPS);

    g_signal_connect (window, "window-state-event",
                      G_CALLBACK (window_state_event), NULL);
    g_signal_connect (window, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), OBJECT_ROOT_PATH);

    ACCEL_RU_WIN_32(window);

	priv = window->priv;
    //priv->self_thread = g_thread_self();


#if GTK_MAJOR_VERSION == 2
	priv->main_vbox = gtk_vbox_new(FALSE, 0);
#else

	priv->main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#endif

#if GTK_CHECK_VERSION(3,10,0)
    priv->titlebar = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(priv->titlebar), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(priv->titlebar), "Домино");
    gtk_widget_show_all(priv->titlebar);
    gtk_window_set_titlebar(GTK_WINDOW (window), priv->titlebar);
#endif
	gtk_widget_show (priv->main_vbox);
	gtk_container_add (GTK_CONTAINER (window), priv->main_vbox);


	manager = priv->manager = gtk_ui_manager_new();
	if (priv->ui)
        gtk_ui_manager_add_ui_from_string(manager, priv->ui, -1 ,NULL);

    do_view_actions_init(manager);
    do_common_actions_init(manager, GTK_WINDOW(window));

    gtk_window_set_icon_name(GTK_WINDOW (window), DO_STOCK_DOMINO_APPS);

	priv->notebook = GTK_NOTEBOOK (g_object_new (DO_TYPE_NOTEBOOK, NULL));

	priv->menu_dock = gtk_ui_manager_get_widget(manager, "/MainMenu");
#if GTK_MAJOR_VERSION == 2
	priv->tabs_menu = do_tabs_menu_new (window);
#endif
    if (priv->menu_dock)
        gtk_box_pack_start (GTK_BOX (priv->main_vbox), priv->menu_dock, FALSE, FALSE, 0);


    priv->toolbar = gtk_ui_manager_get_widget(manager, "/MainToolbar");
    if ( priv->toolbar ) {
        //gtk_toolbar_set_style(GTK_TOOLBAR(priv->toolbar), GTK_TOOLBAR_TEXT);
        gtk_toolbar_set_style(GTK_TOOLBAR(priv->toolbar), GTK_TOOLBAR_BOTH_HORIZ);
        gtk_box_pack_start (GTK_BOX (priv->main_vbox), priv->toolbar, FALSE, TRUE, 0);
#if GTK_MAJOR_VERSION > 2
        GtkStyleContext *
        style_context =
        gtk_widget_get_style_context (priv->toolbar);
        if ( style_context )
            gtk_style_context_add_class(style_context, GTK_STYLE_CLASS_PRIMARY_TOOLBAR);
#endif // GTK_MAJOR_VERSION
    }


//    gtk_window_add_accel_group(GTK_WINDOW(window), gtk_ui_manager_get_accel_group(manager));
    gtk_widget_show_all(priv->main_vbox);


	g_signal_connect (priv->notebook, "tab-close-request",  G_CALLBACK (notebook_page_close_request_cb), window);
	g_signal_connect_after (priv->notebook, "switch-page", G_CALLBACK (notebook_switch_page_cb), window);

    g_signal_connect_object (priv->notebook, "notify::load-progress",
				 G_CALLBACK (sync_load_progress), object, 0);


	gtk_box_pack_start (GTK_BOX (priv->main_vbox), GTK_WIDGET (priv->notebook),  TRUE, TRUE, 0);
	gtk_widget_show (GTK_WIDGET (priv->notebook));

   // priv->statusbar = gtk_statusbar_new();
    GtkWidget *box;
#if GTK_MAJOR_VERSION == 2
	priv->statusbox = box = gtk_vbox_new(FALSE, 0);
#else

	priv->statusbox = box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(box), 6);
    priv->statusbar = gtk_label_new("");
    gtk_label_set_line_wrap(GTK_LABEL(priv->statusbar), TRUE);
    //gtk_label_set_line_wrap_mode (GTK_LABEL(priv->statusbar), PANGO_WRAP_WORD_CHAR);
    //g_object_set(priv->statusbar, "border-width", 1, NULL);
	//gtk_widget_set_size_request (priv->statusbar, 760, -1);
	//g_signal_connect(priv->statusbar,)
	//gtk_widget_show(box);
	//gtk_widget_show(box);

    //!!gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(priv->statusbar), TRUE);

    gtk_box_pack_start (GTK_BOX (box), priv->statusbar, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (priv->main_vbox), box, FALSE, FALSE, 0);

    priv->progress_bar = gtk_progress_bar_new();
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (priv->progress_bar), " ");
	//!!gtk_progress_bar_set_orientation (
		//GTK_PROGRESS_BAR (priv->progress_bar), GTK_PROGRESS_LEFT_TO_RIGHT);

    //!!gtk_widget_set_size_request(priv->progress_bar, 100, 10);
    //gtk_box_pack_end(GTK_BOX(priv->statusbar), priv->progress_bar, FALSE, FALSE, 0);
    //gtk_widget_show(priv->statusbar);

    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(window), OBJECT_ROOT_PATH,
              "window-state", DEFAULT_WINDOW_STATE,
              "window-size", DEFAULT_WINDOW_SIZE,
              "window-position", DEFAULT_WINDOW_POSITION,
               NULL);
#if GTK_MAJOR_VERSION > 2
    gtk_window_set_hide_titlebar_when_maximized (GTK_WINDOW (window), TRUE);
#endif
	return object;
}

static void do_window_finalize (GObject *object)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(object);
    if (G_UNLIKELY (priv->clock_event_source))
        g_source_remove (priv->clock_event_source);
	G_OBJECT_CLASS (do_window_parent_class)->finalize (object);

}

DoWindow *do_window_new ()
{
	return DO_WINDOW (g_object_new (DO_TYPE_WINDOW, NULL));
}


GObject *do_window_get_ui_manager (DoWindow *window)
{
	g_return_val_if_fail (DO_IS_WINDOW (window), NULL);

	return G_OBJECT (window->priv->manager);
}

GtkWidget *do_window_get_toolbar (DoWindow *window)
{
	g_return_val_if_fail (DO_IS_WINDOW (window), NULL);

	return GTK_WIDGET (window->priv->toolbar);
}

GtkWidget *do_window_get_statusbar( DoWindow *window)
{
	g_return_val_if_fail (DO_IS_WINDOW (window), NULL);

	return GTK_WIDGET (window->priv->statusbar);
}

GtkWidget *do_window_get_notebook (DoWindow *window)
{
	g_return_val_if_fail (DO_IS_WINDOW (window), NULL);

	return GTK_WIDGET (window->priv->notebook);
}
/*
GtkWidget *do_window_get_statusbar (DoWindow *window)
{
	g_return_val_if_fail (DO_IS_WINDOW (window), NULL);

	//return GTK_WIDGET (window->priv->statusbar);
}
*/

static GtkWidget *real_get_active_tab (DoWindow *window, int page_num)
{
	GtkWidget *embed;

	if (page_num == -1)
	{
		page_num = gtk_notebook_get_current_page (window->priv->notebook);
	}
	embed = gtk_notebook_get_nth_page (window->priv->notebook, page_num);

	return embed;
}

static void notebook_switch_page_cb (GtkNotebook *notebook,
			 GtkWidget *page,
			 guint page_num,
			 DoWindow *window)
{
	//DoWindowPrivate *priv = window->priv;
	DoView *embed;

	/* get the new tab */
	embed = DO_VIEW(real_get_active_tab (window, page_num));

	/* update new tab */
	do_window_set_active_tab (window, embed);

	/* update window controls */
	//!!update_tabs_menu_sensitivity (window);
}
static void do_window_set_active_tab (DoWindow *window, DoView *new_embed)
{
	DoView *old_embed;
	//DoView *embed;

	g_return_if_fail (DO_IS_WINDOW (window));
	g_return_if_fail (gtk_widget_get_toplevel (GTK_WIDGET (new_embed)) == GTK_WIDGET (window));

	old_embed = window->priv->active_view;

	if (old_embed == new_embed) return;

	if (old_embed != NULL)
	{
		//embed = old_embed;
/*
		g_signal_handlers_disconnect_by_func (embed,
						      G_CALLBACK (sync_tab_title),
						      window);
		g_signal_handlers_disconnect_by_func (embed,
						      G_CALLBACK (sync_tab_address),
						      window);
		g_signal_handlers_disconnect_by_func (embed,
						      G_CALLBACK (sync_tab_icon),
						      window);
		g_signal_handlers_disconnect_by_func (embed,
						      G_CALLBACK (sync_tab_message),
						      window);
		g_signal_handlers_disconnect_by_func (embed,
						      G_CALLBACK (ephy_window_visibility_cb),
						      window);

		g_signal_handlers_disconnect_by_func
			(embed, G_CALLBACK (tab_context_menu_cb), window);
		g_signal_handlers_disconnect_by_func
			(embed, G_CALLBACK (ephy_window_dom_mouse_click_cb), window);
*/
	}

	window->priv->active_view = new_embed;

	if (new_embed != NULL)
	{
		//embed = new_embed;
/*
		sync_tab_title		(embed, NULL, window);
		sync_tab_icon		(embed, NULL, window);
		sync_tab_message	(embed, NULL, window);
		sync_tab_popup_windows	(embed, NULL, window);
		sync_tab_popups_allowed	(embed, NULL, window);

		g_signal_connect_object (embed, "notify::hidden-popup-count",
					 G_CALLBACK (sync_tab_popup_windows),
					 window, 0);
		g_signal_connect_object (embed, "notify::popups-allowed",
					 G_CALLBACK (sync_tab_popups_allowed),
					 window, 0);
		g_signal_connect_object (embed, "notify::title",
					 G_CALLBACK (sync_tab_title),
					 window, 0);
		g_signal_connect_object (embed, "notify::address",
					 G_CALLBACK (sync_tab_address),
					 window, 0);
		g_signal_connect_object (embed, "notify::icon",
					 G_CALLBACK (sync_tab_icon),
					 window, 0);
		g_signal_connect_object (embed, "notify::status-message",
					 G_CALLBACK (sync_tab_message),
					 window, 0);
		g_signal_connect_object (embed, "notify::security-level",
					 G_CALLBACK (sync_tab_security),
					 window, 0);
		g_signal_connect_object (embed, "notify::document-type",
					 G_CALLBACK (sync_tab_document_type),
					 window, 0);
		g_signal_connect_object (embed, "notify::zoom",
					 G_CALLBACK (sync_tab_zoom),
					 window, 0);
		g_signal_connect_object (embed, "notify::load-status",
					 G_CALLBACK (sync_tab_load_status),
					 window, 0);
		g_signal_connect_object (embed, "notify::navigation",
					 G_CALLBACK (sync_tab_navigation),
					 window, 0);
		g_signal_connect_object (embed, "ge-context-menu",
					 G_CALLBACK (tab_context_menu_cb),
					 window, G_CONNECT_AFTER);
		g_signal_connect_object (embed, "notify::load-progress",
					 G_CALLBACK (sync_tab_load_progress),
					 window, 0);
		g_signal_connect_object (embed, "ge_dom_mouse_click",
					 G_CALLBACK (ephy_window_dom_mouse_click_cb),
					 window, 0);
		g_signal_connect_object (embed, "notify::visibility",
					 G_CALLBACK (ephy_window_visibility_cb),
					 window, 0);
*/
		g_object_notify (G_OBJECT (window), "active-child");
	}
}

static void notebook_page_close_request_cb (DoNotebook *notebook,
				DoView *embed,
				DoWindow *window)
{
	//DoWindowPrivate *priv = window->priv;

//!!	if ( gtk_notebook_get_n_pages (priv->notebook) == 1)
//	{
	//	return;
	//}

//	if (!ephy_embed_has_modified_forms (embed) ||
//	    confirm_close_with_modified_forms (window))
	{
		do_view_do_close (embed);
	}
}
DoView *do_window_get_active_child(DoWindow *window)
{
    return window->priv->active_view;
}
void do_window_set_active_child(DoWindow *window, DoView *view)
{
    impl_set_active_child (GTK_CONTAINER (window), GTK_WIDGET(view));
}
GList  *do_window_get_children (DoWindow *window)
{
    return impl_get_children(window);
}
static gboolean window_state_event(GtkWidget* window, GdkEventWindowState* event)
{
    gchar *new_state = NULL;
    if (event->new_window_state & GDK_WINDOW_STATE_ICONIFIED)
        new_state = g_strdup("Minimized");
    else if (event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED)
        new_state = g_strdup("Maximized");
    else if (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)
        new_state = g_strdup("Fullscreen");
    else
        new_state = g_strdup("Normal");
    DOMINO_PROFILE_SET(OBJECT_ROOT_PATH, "DoWindow", "window-state", new_state, NULL);
    g_free(new_state);
    return FALSE;
}

gboolean do_window_set_clock(DoWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);

	if ( !priv->clock )
        return FALSE;

    struct tm tm;
    time_t t;
    char buf[256];
    t = time(NULL);
    tm = *localtime(&t);
#ifdef _WIN32
    strftime(buf, sizeof(buf), "%d.%m.%y %H:%M", &tm);
#else
    strftime(buf, sizeof(buf), "%a %d %b %T", &tm);
#endif // _WIN32

    gtk_action_set_label(priv->clock, buf);

    return TRUE;
}

static void ui_reload(DoWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	GtkUIManager *manager = priv->manager;

	if (priv->ui)
        gtk_ui_manager_add_ui_from_string(manager, priv->ui, -1 ,NULL);

	if (priv->menu_dock)
        g_object_unref(priv->menu_dock);
    priv->menu_dock = gtk_ui_manager_get_widget(manager, "/MainMenu");
    if (priv->menu_dock) {
        gtk_box_pack_start (GTK_BOX (priv->main_vbox), priv->menu_dock, FALSE, FALSE, 0);
        gtk_box_reorder_child(GTK_BOX (priv->main_vbox), priv->menu_dock, 0);
    }
    if (priv->toolbar)
        g_object_unref(priv->toolbar);

    priv->toolbar = gtk_ui_manager_get_widget(manager, "/MainToolbar");
    if (priv->toolbar) {
        int i, n;
        n = gtk_toolbar_get_n_items(GTK_TOOLBAR(priv->toolbar));
        for ( i = 0; i < n; i++ ) {
            GtkToolItem *item;
            item = gtk_toolbar_get_nth_item(GTK_TOOLBAR(priv->toolbar), i);
            g_object_set (G_OBJECT (item), "is-important", TRUE, NULL);
        }

        /*GtkWidget *p;
        GtkToolItem *item;
        p = priv->toolbar;
        priv->toolbar = gtk_toolbar_new();

        item = gtk_tool_button_new_from_stock (GTK_STOCK_SAVE_AS);
        g_object_set (G_OBJECT (item), "is-important", TRUE, NULL);
        gtk_toolbar_insert (GTK_TOOLBAR (priv->toolbar), item, -1);

        gtk_widget_show_all(priv->toolbar);*/


        //gtk_toolbar_set_style(GTK_TOOLBAR(priv->toolbar), GTK_TOOLBAR_TEXT);
        gtk_toolbar_set_style(GTK_TOOLBAR(priv->toolbar), GTK_TOOLBAR_BOTH_HORIZ);
        gtk_box_pack_start (GTK_BOX (priv->main_vbox), priv->toolbar, FALSE, TRUE, 0);
        gtk_box_reorder_child(GTK_BOX (priv->main_vbox), priv->toolbar, 1);
#if GTK_MAJOR_VERSION > 2
        GtkStyleContext *
        style_context =
        gtk_widget_get_style_context (priv->toolbar);
        if ( style_context )
            gtk_style_context_add_class(style_context, GTK_STYLE_CLASS_PRIMARY_TOOLBAR);
#endif // GTK_MAJOR_VERSION
        /*GtkWidget *icon;
        icon = gtk_icon
        priv->clock = gtk_tool_button_new(NULL, "Вс 12 май 13 10:27");
        //gtk_tool_button_set_s
        gtk_toolbar_insert(GTK_TOOLBAR(priv->toolbar), priv->clock, -1);
        gtk_widget_show(GTK_WIDGET(priv->clock));*/

    }
    priv->clock = gtk_ui_manager_get_action(manager, "/MainToolbar/ClockAction");
    if ( priv->clock ) {
        do_window_set_clock(window);
        priv->clock_event_source = g_timeout_add(500, (GSourceFunc)do_window_set_clock, window);
    }
    GtkAccelGroup *accel = gtk_ui_manager_get_accel_group(manager);
    gtk_window_add_accel_group(GTK_WINDOW(window), accel);
    domino_update_main_window_title();
}
void do_window_add_ui_from_string(DoWindow *window, const char *ui, gsize length)
{
    gchar *str;
    if (length != -1) {
        str = g_malloc(length + 1);
        strncpy(str, ui, length);
        str[length] = '\0';
    }
    else
        str = (char*)ui;
	g_object_set(window, "ui", str, NULL);
    if (length != -1)
        g_free(str);
}

static void sync_load_progress (DoNotebook *notebook, GParamSpec *pspec, GtkWidget *proxy)
{
	gint load_progress;
	g_object_get(notebook, "load-progress", &load_progress, NULL);
	set_progress(DO_WINDOW(proxy), load_progress);
}
static void set_progress(DoWindow *window, int progress)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(priv->progress_bar),
                                  (float) progress / 100.0);

    if (progress < 100) {
        if (!gtk_widget_get_visible(priv->progress_bar))
            gtk_widget_show(priv->progress_bar);
    }
    else
        if (gtk_widget_get_visible(priv->progress_bar))
            gtk_widget_hide(priv->progress_bar);
}
void set_grid_lines(GtkWidget *widget, gpointer data)
{
    if (GTK_IS_TREE_VIEW(widget)) {
        gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(widget), GPOINTER_TO_INT(data));
    }
    if (GTK_IS_CONTAINER(widget))
        gtk_container_forall(GTK_CONTAINER(widget), set_grid_lines, data);
}
void do_windows_set_tree_view_lines(DoWindow *window, GtkTreeViewGridLines grid_lines)
{
    gtk_container_forall(GTK_CONTAINER(window), set_grid_lines, GINT_TO_POINTER(grid_lines));
    //gtk_tree_view_set_grid_lines()
}
void do_windows_set_toolbar_markup(DoWindow *windows, const gchar *markup)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(windows);

	if ( markup ) {
        gtk_label_set_markup(GTK_LABEL(priv->statusbar), markup);
        gtk_widget_show_all(priv->statusbox);
        priv->set_label = TRUE;
    }
    else {
        if ( priv->set_label ) {
            priv->set_label = FALSE;
            gtk_label_set_text(GTK_LABEL(priv->statusbar), "");
            gtk_widget_hide(priv->statusbox);
        }
    }

}
