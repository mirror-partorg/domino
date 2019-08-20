
#include "do_window.h"
#include "do_view_actions.h"
#include "do_common_actions.h"
#include "do_view.h"
#include "do_notebook.h"
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

static void notebook_switch_page_cb		(GtkNotebook *notebook,
						 GtkWidget *page,
						 guint page_num,
						 DoWindow *window);
static void notebook_page_close_request_cb (DoNotebook *notebook,
				DoView *embed,
				DoWindow *window);
static void do_window_finalize		(GObject *object);

static gboolean window_state_event(GtkWidget* window, GdkEventWindowState* event);

#define DEFAULT_WINDOW_SIZE "800x500"
#define DEFAULT_WINDOW_POSITION "0x0"
#define DEFAULT_WINDOW_STATE NULL
#define OBJECT_ROOT_PATH "MainWindow"

#define DO_WINDOW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_WINDOW, DoWindowPrivate))


struct _DoWindowPrivate
{
    GtkWidget     *entry;
    GtkWidget     *headerbar;
    GtkWidget     *notebook;
	GtkWidget     *gear_button;
    guint          clock_event_source;
};

enum
{
	PROP_0,
	PROP_ACTIVE_CHILD,
};

static void impl_set_active_child (GtkContainer *container,
		       GtkWidget *child)
{
	int page;
	DoWindow *window;

	window = DO_WINDOW (container);

	page = gtk_notebook_page_num
		(GTK_NOTEBOOK(window->priv->notebook), GTK_WIDGET (child));
	gtk_notebook_set_current_page
		(GTK_NOTEBOOK(window->priv->notebook), page);
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
	//DoWindowPrivate *priv = window->priv;

	switch (prop_id)
	{
		case PROP_ACTIVE_CHILD:
			impl_set_active_child (GTK_CONTAINER (window),
					       g_value_get_object (value));
			break;
	}
}

G_DEFINE_TYPE (DoWindow, do_window, GTK_TYPE_WINDOW )

static void do_window_get_property (GObject *object,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *pspec)
{
	//DoWindow *window = DO_WINDOW (object);
	//DoWindowPrivate *priv = window->priv;

	switch (prop_id)
	{
		case PROP_ACTIVE_CHILD:
			g_value_set_object (value, do_window_get_active_child(DO_WINDOW(object)));
			break;
	}
}
// Signals


static void do_window_class_init (DoWindowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	//GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	object_class->constructor  = do_window_constructor;
	object_class->finalize     = do_window_finalize;
	object_class->get_property = do_window_get_property;
	object_class->set_property = do_window_set_property;
    g_object_class_install_property (object_class,
                                     PROP_ACTIVE_CHILD,
                                         g_param_spec_object ("active-child", NULL, NULL,
                                                              GTK_TYPE_WIDGET /* Can't use an interface type here */,
                                                              G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));
	g_type_class_add_private (object_class, sizeof (DoWindowPrivate));

	/*gtk_widget_class_set_template_from_resource (widget_class,
	                                             "/org/glekar/bookcase/ui/bookcase-window.ui");
	gtk_widget_class_bind_template_child_private (widget_class, DoWindow, headerbar);
	gtk_widget_class_bind_template_child_private (widget_class, DoWindow, notebook);
	gtk_widget_class_bind_template_child_private (widget_class, DoWindow, entry);
	gtk_widget_class_bind_template_child_private (widget_class, DoWindow, gear_button);*/

}

static void do_window_init (DoWindow *window)
{
	window->priv = DO_WINDOW_GET_PRIVATE (window);
	//gtk_widget_init_template (GTK_WIDGET (window));
    do_view_actions_init(window);
    do_common_actions_init(window);
}

static GObject *do_window_constructor (GType type,
			 guint n_construct_properties,
			 GObjectConstructParam *construct_params)
{
	GObject *object;
	DoWindow *window;
	GtkWidget *notebook;
	GtkWidget *headerbar;
	GtkWidget *entry;
	GtkWidget *gear_button;
	GtkWidget *image;
	GMenu *menu;
	GMenu *submenu;
	DoWindowPrivate *priv;


	object = G_OBJECT_CLASS (do_window_parent_class)->constructor
		(type, n_construct_properties, construct_params);

	window = DO_WINDOW (object);
    gtk_window_set_icon_name(GTK_WINDOW (window), "bookcase");

    g_signal_connect (window, "window-state-event",
                      G_CALLBACK (window_state_event), NULL);
    g_signal_connect (window, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), OBJECT_ROOT_PATH);

    priv = window->priv;

    //do_view_actions_init(manager);
    //to do do_common_actions_init(manager, GTK_WINDOW(window));
    gtk_window_set_icon_name(GTK_WINDOW (window), "bookcase");

    priv->notebook = notebook = GTK_WIDGET(g_object_new(DO_TYPE_NOTEBOOK, NULL));
    gtk_container_add(GTK_CONTAINER(window), notebook);
    //do_notebook_set_show_tabs(DO_NOTEBOOK(notebook), FALSE); // to do
	g_signal_connect (priv->notebook, "tab-close-request",  G_CALLBACK (notebook_page_close_request_cb), window);
	g_signal_connect_after (priv->notebook, "switch-page", G_CALLBACK (notebook_switch_page_cb), window);

    priv->headerbar = headerbar = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(window), headerbar);
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), TRUE);

    priv->entry = entry = gtk_entry_new();
    gtk_widget_set_hexpand(GTK_WIDGET(entry), TRUE);
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_PRIMARY, "edit-find-symbolic");
    gtk_header_bar_set_custom_title(GTK_HEADER_BAR(headerbar), entry);

    menu = g_menu_new();
   	image = gtk_image_new_from_icon_name("open-menu-symbolic", GTK_ICON_SIZE_MENU);
    priv->gear_button = gear_button = gtk_menu_button_new();
    gtk_header_bar_pack_end(GTK_HEADER_BAR(headerbar), gear_button);
    gtk_menu_button_set_use_popover(GTK_MENU_BUTTON(gear_button), TRUE);
    gtk_button_set_image(GTK_BUTTON(gear_button), image);
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(gear_button), G_MENU_MODEL(menu));

    g_menu_append(G_MENU(menu), "Акции и рекомендации", "common-actions.AdsView");
    g_menu_append(G_MENU(menu), "Просмотр настроек", "common-actions.ProfileView");
    submenu = g_menu_new();
    g_menu_append_submenu(G_MENU(menu), "Действия", G_MENU_MODEL(submenu));
    g_menu_append(G_MENU(submenu), "Добавить", "view-actions.InsertAction");
    g_menu_append(G_MENU(submenu), "Копировать", "view-actions.CopyAction");
    g_menu_append(G_MENU(submenu), "Удалить", "view-actions.DeleteAction");
    g_menu_append(G_MENU(submenu), "Сохранить", "view-actions.SaveAction");
    g_menu_append(G_MENU(submenu), "Сохранить как...", "view-actions.SaveAsAction");
    g_menu_append(G_MENU(submenu), "Открыть...", "view-actions.OpenAction");
    g_menu_append(G_MENU(submenu), "Обновить", "view-actions.ResfreshAction");
    g_menu_append(G_MENU(submenu), "Ректировать", "view-actions.EditAction");
    g_menu_append(G_MENU(submenu), "Применить", "view-actions.ApplyAction");
    g_menu_append(G_MENU(submenu), "Отменить", "view-actions.UndoApplyAction");
    g_menu_append(G_MENU(submenu), "Печать...", "view-actions.PrintAction");
    g_menu_append(G_MENU(submenu), "Печать", "view-actions.PrintNowAction");
    g_menu_append(G_MENU(submenu), "Поиск...", "view-actions.FindByBarcodeAction");
    g_menu_append(G_MENU(submenu), "Послать по почте", "view-actions.MailSendAction");

    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(window), OBJECT_ROOT_PATH,
              "window-state", DEFAULT_WINDOW_STATE,
              "window-size", DEFAULT_WINDOW_SIZE,
              "window-position", DEFAULT_WINDOW_POSITION,
               NULL);
    gtk_window_set_hide_titlebar_when_maximized (GTK_WINDOW (window), TRUE);
    gtk_widget_show_all(GTK_WIDGET(window));
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


GtkWidget *do_window_get_notebook (DoWindow *window)
{
	g_return_val_if_fail (DO_IS_WINDOW (window), NULL);

	return GTK_WIDGET (window->priv->notebook);
}
/*
GtkWidget *do_window_get_notebook(GtkWidget *widget)
{
    GList *l;
    for (l = gtk_container_get_children(GTK_CONTAINER(widget)); l; l=l->next)
        if DO_IS_NOTEBOOK(l->data)
            return GTK_WIDGET(l->data);
    return NULL;
}
*/
/*
GtkWidget *do_window_get_statusbar (DoWindow *window)
{
	g_return_val_if_fail (DO_IS_WINDOW (window), NULL);

	//return GTK_WIDGET (window->priv->statusbar);
}
*/
/*
static GtkWidget *real_get_active_tab (DoWindow *window, int page_num)
{
	GtkWidget *embed;

	if (page_num == -1)
	{
		page_num = gtk_notebook_get_current_page (GTK_NOTEBOOK(window->priv->notebook));
	}
	embed = gtk_notebook_get_nth_page (GTK_NOTEBOOK(window->priv->notebook), page_num);

	return embed;
}
*/
GtkWidget *do_window_get_active_child(DoWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
    GtkWidget *view = NULL, *child, *page;
    gint page_num;
    if ( !priv->notebook )
        return NULL;
    page_num = gtk_notebook_get_current_page(GTK_NOTEBOOK(priv->notebook));
    if ( page_num != -1 ) {
        page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(priv->notebook), page_num);
        if ( page ) {
            if ( DO_IS_VIEW(page) ) {
                view = page;
            }
            else
            if ( GTK_IS_CONTAINER(page) ) {
                child = gtk_container_get_focus_child(GTK_CONTAINER(page));
                if ( DO_IS_VIEW(child) ) {
                    view = child;
                }
                else {
                    GList *l;
                    for ( l = gtk_container_get_children(GTK_CONTAINER(page)); l; l=l->next ) {
                        if ( DO_IS_VIEW(l->data) ) {
                            view = l->data;
                            break;
                        }
                    }
                }
            }
        }
    }
    if ( view )
        view = do_view_get_active_child(DO_VIEW(view));
    return view;
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
/*
static GtkWidget *real_get_active_tab (DoWindow *window, int page_num)
{
	GtkWidget *embed;

	if (page_num == -1)
	{
		page_num = gtk_notebook_get_current_page (GTK_NOTEBOOK(window->priv->notebook));
	}
	embed = gtk_notebook_get_nth_page (GTK_NOTEBOOK(window->priv->notebook), page_num);

	return embed;
}
*/
static void notebook_switch_page_cb (GtkNotebook *notebook,
			 GtkWidget *page,
			 guint page_num,
			 DoWindow *window)
{
	//DoWindowPrivate *priv = window->priv;
	do_view_actions_refresh(GTK_WIDGET(notebook));
	/*
	DoView *embed;

	embed = DO_VIEW(real_get_active_tab (window, page_num));

	do_window_set_active_tab (window, embed);

	//!!update_tabs_menu_sensitivity (window);
	*/
}
