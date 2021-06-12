
#include "do_window.h"
#include "do_view_actions.h"
#include "do_common_actions.h"
#include "do_application.h"
#include "do_view.h"
#include "do_notebook.h"
#include "domino.h"
#include "do_list_view.h"

#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <json-glib/json-glib.h>
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
static gboolean do_window_configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gchar *path);
static void do_window_entry_changed(GtkEditable *editable, DoWindow *window);
static void do_window_entry_activate(GtkEntry *entry, DoWindow *window);
static gboolean do_window_entry_key_press(GtkWidget *entry, GdkEventKey *event, DoWindow *window);
#ifndef POS_MINIMAL
static void do_window_previous_clicked(GtkWidget *widget, DoWindow *window);
static void do_window_next_clicked(GtkWidget *widget, DoWindow *window);
#endif

#if !GTK_CHECK_VERSION(3,12,0)
static gboolean do_window_key_press_event_cb(GtkWidget *widget, GtkEvent *event, gpointer data);
#endif

#define DEFAULT_WINDOW_SIZE "800x500"
#define DEFAULT_WINDOW_POSITION "0x0"
#define DEFAULT_WINDOW_STATE NULL
#define OBJECT_ROOT_PATH "MainWindow"
#define SEARCH_EXTERNAL_TIMEOUT 500
#ifdef CASH
#define DEFAULT_PLACEHOLDER_TEXT  "Поиск по совпадению,МНН (нажмите →)"
#else
#define DEFAULT_PLACEHOLDER_TEXT  "Поиск по совпадению,МНН (нажмите Ctrl+L)"
#endif // CASH

#define DO_WINDOW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_WINDOW, DoWindowPrivate))

const gchar *states[] = {"network-idle-symbolic",
                         "network-receive-symbolic",
                         "network-transmit-symbolic",
                         "network-transmit-receive-symbolic",
                         "network-offline-symbolic"};

struct _DoWindowPrivate
{
    GtkWidget     *entry;
    gboolean       entry_changed;

    GtkWidget     *headerbar;
    GtkWidget     *notebook;
	GtkWidget     *gear_button;
	GtkWidget     *footer_label;
	GtkWidget     *button_prev;
	GtkWidget     *button_next;
	DoView        *goods;
    guint          clock_event_source;
    guint          search_src;
    gchar         *search_text;
    GtkWidget     *connection_icon;
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
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(container);

	page = gtk_notebook_page_num
		(GTK_NOTEBOOK(priv->notebook), GTK_WIDGET (child));
	gtk_notebook_set_current_page
		(GTK_NOTEBOOK(priv->notebook), page);
}

static GList *
impl_get_children (DoWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);

	return gtk_container_get_children (GTK_CONTAINER (priv->notebook));
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

G_DEFINE_TYPE_WITH_CODE (DoWindow, do_window, GTK_TYPE_WINDOW, G_ADD_PRIVATE(DoWindow) )

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
	//g_type_class_add_private (object_class, sizeof (DoWindowPrivate));

	/*gtk_widget_class_set_template_from_resource (widget_class,
	                                             "/org/glekar/bookcase/ui/bookcase-window.ui");
	gtk_widget_class_bind_template_child_private (widget_class, DoWindow, headerbar);
	gtk_widget_class_bind_template_child_private (widget_class, DoWindow, notebook);
	gtk_widget_class_bind_template_child_private (widget_class, DoWindow, entry);
	gtk_widget_class_bind_template_child_private (widget_class, DoWindow, gear_button);*/

}

static void do_window_init (DoWindow *window)
{
    //DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	//gtk_widget_init_template (GTK_WIDGET (window));
    //DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
    //memset(priv, 0, sizeof(*priv));
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
	GtkWidget *entry;
	GtkWidget *gear_button;
	GtkWidget *b;
	GtkWidget *box, *nbox;
	GtkWidget *vbox;
	GMenu *menu;
	GMenu *submenu;
	DoWindowPrivate *priv;
	GtkWidget *image;


	object = G_OBJECT_CLASS (do_window_parent_class)->constructor
		(type, n_construct_properties, construct_params);

	window = DO_WINDOW (object);
    gtk_window_set_icon_name(GTK_WINDOW (window), "bookcase");

    g_signal_connect (window, "window-state-event",
                      G_CALLBACK (window_state_event), NULL);
    g_signal_connect (window, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), OBJECT_ROOT_PATH);
#if !GTK_CHECK_VERSION(3,12,0)
    g_signal_connect (window, "key-press-event",
                      G_CALLBACK (do_window_key_press_event_cb), NULL);
#endif
    priv = DO_WINDOW_GET_PRIVATE(window);

    //do_view_actions_init(manager);
    //to do do_common_actions_init(manager, GTK_WINDOW(window));
    gtk_window_set_icon_name(GTK_WINDOW (window), "bookcase");

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    priv->notebook = notebook = GTK_WIDGET(g_object_new(DO_TYPE_NOTEBOOK, NULL));
    gtk_container_add(GTK_CONTAINER(window), vbox);
//#ifdef POS_MINIMAL
    do_notebook_set_show_tabs(DO_NOTEBOOK(notebook), FALSE); // to do
//#endif
	g_signal_connect (priv->notebook, "tab-close-request",  G_CALLBACK (notebook_page_close_request_cb), window);
	g_signal_connect_after (priv->notebook, "switch-page", G_CALLBACK (notebook_switch_page_cb), window);

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    nbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(box), nbox, FALSE, FALSE, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(nbox),"raised");
    gtk_style_context_add_class(gtk_widget_get_style_context(nbox),"linked");
    gtk_style_context_add_class(gtk_widget_get_style_context(nbox),"navigation-box");

#if GTK_CHECK_VERSION(3,12,0)
    //gtk_style_context_add_class(gtk_widget_get_style_context(box), GTK_STYLE_CLASS_INLINE_TOOLBAR);
    priv->headerbar = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(window), priv->headerbar);
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(priv->headerbar), TRUE);
    gtk_header_bar_set_custom_title(GTK_HEADER_BAR(priv->headerbar), box);
#else
    gtk_box_pack_start(GTK_BOX(vbox), box, TRUE, TRUE, 0);
#endif
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
#ifndef POS_MINIMAL
	priv->button_prev = b = gtk_button_new_from_icon_name("go-previous-symbolic", GTK_ICON_SIZE_MENU);
	gtk_style_context_add_class(gtk_widget_get_style_context(b), "left_merge");
    gtk_box_pack_start(GTK_BOX(nbox), b, FALSE, FALSE, 0);
    g_signal_connect(b, "clicked", G_CALLBACK(do_window_previous_clicked), object);
	priv->button_next = b = gtk_button_new_from_icon_name("go-next-symbolic", GTK_ICON_SIZE_MENU);
	gtk_style_context_add_class(gtk_widget_get_style_context(b), "right_merge");
    g_signal_connect(b, "clicked", G_CALLBACK(do_window_next_clicked), object);
    gtk_box_pack_start(GTK_BOX(nbox), b, FALSE, FALSE, 0);
#endif
    priv->entry = entry = gtk_entry_new();
    gtk_widget_set_name( entry, "do-search-entry");
    gtk_widget_set_size_request(entry, 500, -1);
    gtk_widget_set_hexpand(GTK_WIDGET(entry), TRUE);
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_PRIMARY, "edit-find-symbolic");
    g_signal_connect(priv->entry, "changed", G_CALLBACK(do_window_entry_changed), window);
    g_signal_connect(priv->entry, "activate", G_CALLBACK(do_window_entry_activate), window);
    g_signal_connect(priv->entry, "key-press-event", G_CALLBACK(do_window_entry_key_press), window);
    gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 6);
    gtk_widget_set_sensitive(GTK_WIDGET(priv->entry), FALSE);

    priv->footer_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), priv->footer_label, FALSE, FALSE, 6);

    priv->connection_icon = gtk_image_new();
    gtk_image_set_from_icon_name(GTK_IMAGE(priv->connection_icon), states[0], GTK_ICON_SIZE_MENU);
    gtk_box_pack_end(GTK_BOX(box), priv->connection_icon, FALSE, FALSE, 6);


    menu = g_menu_new();
#if GTK_CHECK_VERSION(3,12,0)
    image = gtk_image_new_from_icon_name("open-menu-symbolic", GTK_ICON_SIZE_MENU);
#else
    image = gtk_image_new_from_icon_name("view-list-symbolic", GTK_ICON_SIZE_MENU);
#endif
    priv->gear_button = gear_button = gtk_menu_button_new();
    gtk_box_pack_end(GTK_BOX(box), gear_button, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(3,12,0)
    gtk_menu_button_set_use_popover(GTK_MENU_BUTTON(gear_button), TRUE);
#else
    gtk_box_pack_end(GTK_BOX(headerbar), gear_button, FALSE, FALSE, 0);
#endif
    gtk_button_set_image(GTK_BUTTON(gear_button), image);
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(gear_button), G_MENU_MODEL(menu));


    g_menu_append(G_MENU(menu), "Акции и рекомендации", "common-actions.AdsView");
    g_menu_append(G_MENU(menu), "Товары", "common-actions.GoodsView");
#if (defined(WEBKIT) || defined(WEBKIT2))
    g_menu_append(G_MENU(menu), "Веб", "common-actions.HtmlView");
#endif
    submenu = g_menu_new();
    g_menu_append_submenu(G_MENU(menu), "Настройки", G_MENU_MODEL(submenu));
    g_menu_append(G_MENU(submenu), "Пользовательские", "common-actions.ProfileView");
    g_menu_append(G_MENU(submenu), "Общие", "common-actions.SettingView");

    submenu = g_menu_new();
    g_menu_append_submenu(G_MENU(menu), "Действия", G_MENU_MODEL(submenu));
    g_menu_append(G_MENU(submenu), "Добавить", "view-actions.InsertAction");
    g_menu_append(G_MENU(submenu), "Копировать", "view-actions.CopyAction");
    g_menu_append(G_MENU(submenu), "Удалить", "view-actions.DeleteAction");
    g_menu_append(G_MENU(submenu), "Сохранить", "view-actions.SaveAction");
    g_menu_append(G_MENU(submenu), "Сохранить как...", "view-actions.SaveAsAction");
    g_menu_append(G_MENU(submenu), "Открыть...", "view-actions.OpenAction");
    g_menu_append(G_MENU(submenu), "Обновить", "view-actions.ResfreshAction");
    g_menu_append(G_MENU(submenu), "Редактировать", "view-actions.EditAction");
    g_menu_append(G_MENU(submenu), "Применить", "view-actions.ApplyAction");
    g_menu_append(G_MENU(submenu), "Отменить", "view-actions.UndoApplyAction");
    g_menu_append(G_MENU(submenu), "Печать...", "view-actions.PrintAction");
    g_menu_append(G_MENU(submenu), "Печать", "view-actions.PrintNowAction");
    g_menu_append(G_MENU(submenu), "Поиск...", "view-actions.FindByBarcodeAction");
    g_menu_append(G_MENU(submenu), "Послать по почте", "view-actions.MailSendAction");

    g_menu_append(G_MENU(menu), "Закрыть", "common-actions.Close");
    g_menu_append(G_MENU(menu), "О приложении", "common-actions.About");
    g_menu_append(G_MENU(menu), "Выход", "common-actions.Quit");

    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(window), OBJECT_ROOT_PATH,
              "window-state", DEFAULT_WINDOW_STATE,
              "window-size", DEFAULT_WINDOW_SIZE,
              "window-position", DEFAULT_WINDOW_POSITION,
               NULL);
    gtk_window_set_hide_titlebar_when_maximized (GTK_WINDOW (window), TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(window), 600, 400);
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
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	g_return_val_if_fail (DO_IS_WINDOW (window), NULL);

	return GTK_WIDGET (priv->notebook);
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
static void do_window_save_setting(GtkWindow *window, const gchar *path)
{

    GdkWindowState state = gdk_window_get_state (gtk_widget_get_window(GTK_WIDGET(window)));

    if (!(state &
        (GDK_WINDOW_STATE_MAXIMIZED | GDK_WINDOW_STATE_FULLSCREEN))) {
        GtkAllocation allocation;
        gint x, y;
        char buffer[512];
        gtk_widget_get_allocation(GTK_WIDGET(window), &allocation);
        sprintf(buffer, "%dx%d", allocation.width, allocation.height);
        DOMINO_PROFILE_SET(path, "GtkWindow", "window-size", buffer, NULL);
        //GtkWindowPosition position;
        //g_object_get(window, "window-position", &position, NULL);
        //switch (position) {
           // case GTK_WIN_POS_NONE:
                gtk_window_get_position(window, &x, &y);
                sprintf(buffer, "%dx%d", x, y);
          //      break;
/*            case GTK_WIN_POS_CENTER:
                sprintf(buffer, "center");
                break;
            case GTK_WIN_POS_CENTER_ON_PARENT:
                sprintf(buffer, "center-on-parent");
                break;
            default:
                buffer[0] = '\0';
                break;
        }*/
        if (buffer[0]) {
            DOMINO_PROFILE_SET(path, "GtkWindow", "window-position", buffer, NULL);
        }
    }
}

static gboolean do_window_configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gchar *path)
{
	do_window_save_setting(GTK_WINDOW(widget), path);
	return FALSE;
}
void do_window_set_toolbar_progress(DoWindow *window, int proc)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	if ( proc )
        gtk_entry_set_progress_fraction(GTK_ENTRY(priv->entry), proc/100.);
    else {
        if ( gtk_entry_get_progress_fraction(GTK_ENTRY(priv->entry)) > 0 ) {
            gtk_entry_set_progress_fraction(GTK_ENTRY(priv->entry), 0.0);
            gtk_entry_set_progress_pulse_step(GTK_ENTRY(priv->entry), 0);
        }
    }
}
int do_window_get_toolbar_progress(DoWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	int ret;
	ret = gtk_entry_get_progress_fraction(GTK_ENTRY(priv->entry)) *100;
	return ret;
}
void do_window_set_toolbar_text(DoWindow *window, const gchar *markup)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	gtk_entry_set_placeholder_text(GTK_ENTRY(priv->entry), markup? markup : DEFAULT_PLACEHOLDER_TEXT);
}
const gchar *do_window_get_toolbar_text(DoWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	return gtk_entry_get_placeholder_text(GTK_ENTRY(priv->entry));
}
void  do_window_update_toolbar(DoWindow *window)
{
    GtkWidget *widget;
    widget = do_window_get_active_child(window);
    const gchar *text = NULL;
    gint proc = 0;
    if ( DO_IS_VIEW(widget) ) {
        text = do_view_get_load_status(DO_VIEW(widget));
        proc = do_view_get_load_progress(DO_VIEW(widget));
    }
    do_window_set_toolbar_text(window, text);
    do_window_set_toolbar_progress(window, proc);
}
static gboolean do_window_external_search_end(JsonNode *node, DoWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	if ( priv->goods ) {
        impl_set_active_child(GTK_CONTAINER(window), GTK_WIDGET(priv->goods));
        do_list_view_external_search(DO_LIST_VIEW(priv->goods), node);
	}
	return FALSE;
}
static gchar *mystrip(const gchar *text)
{
    gchar **bufv;
    gchar *ret = NULL, *buf;
    int j;
    bufv = g_strsplit(text," ", -1);
    for ( j = 0; j < g_strv_length(bufv); j++ ) {
        if ( !bufv[j] || bufv[j][0] == '\0') continue;
        if ( ret ) {
            buf = ret;
            ret = g_strdup_printf("%s %s", buf, bufv[j]);
            g_free(buf);
        }
        else
            ret = g_strdup(bufv[j]);

    }
    //ret = g_strjoinv(" ", bufv);
    g_strfreev(bufv);
    return ret;
}
/*
g_free(buf);

    char *p;
    gunichar out;
    int count;
    for (count = 0, p = (gchar*)text; p && *p !='\0'; p = (gchar*)g_utf8_next_char(p), count++);
    for (; p && p > text && count > 0; p = (gchar*)g_utf8_prev_char(p), --count) {
        out = g_utf8_get_char(p);
        if ( !g_unichar_isspace(out) )
            break;
    }
    if ( p )
        p
        priv->search_text[0] = '\0';
    else
        ext[p - text] = '\0';
    g_free(text);
}*/
static gboolean do_window_external_search(DoWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	gchar *text;
    priv->search_src = 0;
    if ( gtk_widget_is_focus(GTK_WIDGET(priv->entry)) ) {
        text = mystrip(gtk_entry_get_text(GTK_ENTRY(priv->entry)));
        if ( !g_strcmp0(text, priv->search_text) ) {
            g_free(text);
            text = NULL;
        }
        if ( text ) {
            GtkApplication *app = gtk_window_get_application(
                    GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(window))));
            gchar *buf;
            buf = g_strdup_printf("{\"string\":\"%s\"}", text);
            do_application_request2_async(DO_APPLICATION(app), "POST", "Search", NULL, 0, (GFunc)do_window_external_search_end, window, "body", buf, "name", "goods", NULL);
            g_free(buf);
            g_free(priv->search_text);
            priv->search_text = text;
        }
    }
    return FALSE;
}
static void do_window_entry_changed(GtkEditable *editable, DoWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);

    if ( !gtk_widget_get_sensitive(priv->entry) )
        return;
    if ( priv->entry_changed )
        return;
    if ( priv->search_src ) {
        g_source_remove(priv->search_src);
    }
	priv->search_src = g_timeout_add(SEARCH_EXTERNAL_TIMEOUT, (GSourceFunc)do_window_external_search, window);
}
#if !GTK_CHECK_VERSION(3,12,0)
static gboolean do_window_key_press_event_cb(GtkWidget *widget, GtkEvent *event, gpointer data)
{
    gboolean retval = FALSE;
#ifndef CASH
    DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
    guint mask = gtk_accelerator_get_default_mod_mask ();

    if ((event->state & mask) == GDK_CONTROL_MASK) {
        gchar *keyname;
        keyname = gdk_keyval_name(event->keyval);
        if ( !g_strcmp0(keyname, "L") || !g_strcmp0(keyname, "l") ||
             !g_strcmp0(keyname, "Cyrillic_DE") || !g_strcmp0(keyname, "Cyrillic_de") ) {

            if ( g_strcmp0(gtk_widget_get_name(gtk_container_get_focus_child(GTK_CONTAINER(widget)), "do-search-entry") )
                gtk_widget_grab_focus(GTK_WIDGET(priv->entry));

            retval = TRUE;
        }
        g_free(keyname);
    }
#endif
    return retval;
}
#endif
#ifndef POS_MINIMAL
static void do_window_next_clicked(GtkWidget *widget, DoWindow *window)
{
	GActionGroup *group;
	//DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	GAction *action;
#if GTK_CHECK_VERSION(3,16,0)
    group = gtk_widget_get_action_group(GTK_WIDGET(window), "common-actions");
#else
	group = do_common_action_get_group();
#endif

    action = g_action_map_lookup_action(G_ACTION_MAP(group), "Next");
    if ( action )
        g_action_activate(action,NULL);
}
void do_window_search_end(DoWindow *window)
{
	GActionGroup *group;
	//DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
	GAction *action;
#if GTK_CHECK_VERSION(3,16,0)
    group = gtk_widget_get_action_group(GTK_WIDGET(window), "common-actions");
#else
	group = do_common_action_get_group();
#endif

    action = g_action_map_lookup_action(G_ACTION_MAP(group), "SearchEnd");
    if ( action )
        g_action_activate(action,NULL);
}
static void do_window_previous_clicked(GtkWidget *widget, DoWindow *window)
{
	GActionGroup *group;
	GAction *action;
	//DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
#if GTK_CHECK_VERSION(3,16,0)
    group = gtk_widget_get_action_group(GTK_WIDGET(window), "common-actions");
#else
	group = do_common_action_get_group();
#endif

    action = g_action_map_lookup_action(G_ACTION_MAP(group), "Previous");
    if ( action )
        g_action_activate(action,NULL);
}
#endif
void do_window_set_goods(DoWindow *window, DoView *view)
{
    DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
    if ( !priv->goods ) {
        priv->goods = view;
        gtk_widget_set_sensitive(GTK_WIDGET(priv->entry), TRUE);
    }
}
static void do_window_entry_activate(GtkEntry *entry, DoWindow *window)
{
    DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
    if ( priv->search_src ) {
        g_source_remove(priv->search_src);
        do_window_external_search(window);
    }
    if ( priv->goods ) {
        impl_set_active_child(GTK_CONTAINER(window), GTK_WIDGET(priv->goods));
        do_view_do_grab_focus(priv->goods);
    }
}
void do_window_end_search(DoWindow *window)
{
    DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
    priv->entry_changed = TRUE;
    gtk_entry_set_text(GTK_ENTRY(priv->entry), "");
    priv->entry_changed = FALSE;
    if ( priv->goods ) {
        impl_set_active_child(GTK_CONTAINER(window), GTK_WIDGET(priv->goods));
        do_list_view_external_search(DO_LIST_VIEW(priv->goods), NULL);
    }
}
static gboolean do_window_entry_key_press(GtkWidget *entry, GdkEventKey *event, DoWindow *window)
{
    //DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
    guint mask = gtk_accelerator_get_default_mod_mask ();
    if ( (event->state & mask) == 0 )
    {
    	switch (event->keyval)
    	{
    	    case GDK_KEY_Right:
                {
                    if ( !gtk_editable_get_selection_bounds(GTK_EDITABLE(entry), NULL, NULL) ) {
                        GVariant *parameter;
                        gchar *uri, *url;
                            DOMINO_LOCAL_GET("main", "websearch", &url, NULL);
                        uri = g_strdup_printf("%s/search?q=%s", url, gtk_entry_get_text(GTK_ENTRY(entry)));
                        parameter = g_variant_new_string(uri);
                        do_common_action_activate("HtmlViewGo", parameter);
                        g_free(uri);
                        return TRUE;
                    }
                    else {
                        break;
                    }
                }
    	    case GDK_KEY_Down:
                do_window_entry_activate(GTK_ENTRY(entry), window);
                return TRUE;
            case GDK_KEY_Escape:
                do_window_end_search(window);
                return TRUE;
        }
    }
    return FALSE;
}
void do_window_set_footerbar_text(DoWindow *window, const gchar *text)
{
    DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
    gtk_label_set_text(GTK_LABEL(priv->footer_label), text);
}
const gchar  *do_window_get_footerbar_text(DoWindow *window)
{
    DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
    return gtk_label_get_text(GTK_LABEL(priv->footer_label));
}
void do_window_show_connection_state(DoWindow *window, gint state)
{
    DoWindowPrivate *priv = DO_WINDOW_GET_PRIVATE(window);
    g_return_if_fail (state >= 0 && state < G_N_ELEMENTS(states));
    gtk_image_set_from_icon_name(GTK_IMAGE(priv->connection_icon), states[state], GTK_ICON_SIZE_MENU);
}
