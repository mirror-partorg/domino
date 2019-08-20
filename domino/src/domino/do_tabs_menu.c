#include <gtk/gtk.h>
#if GTK_MAJOR_VERSION == 2

#include "do_tabs_menu.h"
#include "do_window.h"
#include "do_notebook.h"

#include <string.h>
#include <stdlib.h>

#define LABEL_WIDTH_CHARS 32
#define ACTION_VERB_FORMAT_PREFIX       "JmpTab"
#define ACTION_VERB_FORMAT_PREFIX_LEN   (6) /* strlen (ACTION_VERB_FORMAT_PREFIX) */
#define ACTION_VERB_FORMAT		ACTION_VERB_FORMAT_PREFIX "%x"
#define ACTION_VERB_FORMAT_LENGTH	strlen (ACTION_VERB_FORMAT) + 14 + 1
#define ACTION_VERB_FORMAT_BASE         (16) /* %x is hex */
#define ACCEL_PATH_FORMAT		"<Actions>/TabsActions/%s"
#define ACCEL_PATH_FORMAT_LENGTH	strlen (ACCEL_PATH_FORMAT) -2 + ACTION_VERB_FORMAT_LENGTH
#define DATA_KEY			"DoTabsMenu::Action"

#define DO_TABS_MENU_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_TABS_MENU, DoTabsMenuPrivate))

struct _DoTabsMenuPrivate
{
	DoWindow *window;
	GtkActionGroup *action_group;
	GtkAction *anchor_action;
	guint ui_id;
};

enum
{
	PROP_0,
	PROP_WINDOW
};

static void	do_tabs_menu_class_init	(DoTabsMenuClass *klass);
static void	do_tabs_menu_init	  	(DoTabsMenu *menu);
static void	do_tabs_menu_update		(DoTabsMenu *menu);

/* FIXME: this can be severely optimised */
static GByteArray *tabs_id_array = NULL;
static guint n_tabs = 0;

G_DEFINE_TYPE (DoTabsMenu, do_tabs_menu, G_TYPE_OBJECT)

/* We need to assign unique IDs to tabs, otherwise accels get confused in the
 * tabs menu (bug #339548). We could use a serial #, but the ID is used in the
 * action name which is stored in a GQuark and so we should allocate them
 * efficiently.
 */
static guint
allocate_tab_id (void)
{
        int bit;
        guint b, len;
        guint8 *data;
        guint8 byte, mask;

        if (n_tabs++ == 0)
        {
                g_assert (tabs_id_array == NULL);
                tabs_id_array = g_byte_array_sized_new (16);
        }

        /* Find a free ID */
        len = tabs_id_array->len;
        data = tabs_id_array->data;
        for (b = 0; b < len; ++b)
        {
                if (data[b] != 0xff)
                        break;
        }

        /* Need to append a new byte */
        if (b == len)
        {
                guint8 bytes[] = { 0 };
                g_byte_array_append (tabs_id_array, bytes, G_N_ELEMENTS (bytes));
                g_assert (tabs_id_array->len > b);
        }

        data = tabs_id_array->data + b;
        byte = 0xff ^ *data;
        /* Now find the first free bit */
        bit = g_bit_nth_lsf (byte, -1);
        mask = 1 << bit;
        g_assert (bit >= 0 && bit <= 7);
        g_assert ((*data & mask) == 0);
        /* And mark it as allocated */
        *data |= mask;

        return b * 8 + bit;
}

static void
free_tab_id (GtkAction *action)
{
        const char *name;
        guint id;
        guint8 *data;
        guint b, bit;

        name = gtk_action_get_name (action);
        id = g_ascii_strtoull (name + ACTION_VERB_FORMAT_PREFIX_LEN, NULL,
                               ACTION_VERB_FORMAT_BASE);
        g_assert (id >= 0 && id < tabs_id_array->len * 8);

        b = id >> 3;
        bit = id & 0x7;
        data = tabs_id_array->data + b;
        *data &= ~(1 << bit);

        g_assert (n_tabs > 0);
        if (--n_tabs == 0)
        {
                g_assert (tabs_id_array != NULL);
                g_byte_array_free (tabs_id_array, TRUE);
                tabs_id_array = NULL;
        }
}

static void
tab_action_activate_cb (GtkToggleAction *action,
			DoTabsMenu *menu)
{
	DoTabsMenuPrivate *priv = menu->priv;
	DoView *view;

	if (gtk_toggle_action_get_active (action) == FALSE)
	{
		return;
	}

	view = g_object_get_data (G_OBJECT (action), DATA_KEY);
	g_return_if_fail (view != NULL);

	if (do_window_get_active_child (DO_WINDOW (priv->window)) != view)
	{
        do_window_set_active_child (DO_WINDOW (priv->window), view);
	}
}

static void
sync_tab_title (DoView *view,
		GParamSpec *pspec,
		GtkAction *action)
{
	const char *title;

	title = do_view_get_title (view);

	g_object_set (action, "label", title, NULL);
}

static void
notebook_page_added_cb (DoNotebook *notebook,
			DoView *view,
			guint position,
			DoTabsMenu *menu)
{
	DoTabsMenuPrivate *priv = menu->priv;
	GtkAction *action;
	char verb[ACTION_VERB_FORMAT_LENGTH];
	GSList *group;

	g_snprintf (verb, sizeof (verb), ACTION_VERB_FORMAT, allocate_tab_id ());

	action = g_object_new (GTK_TYPE_RADIO_ACTION,
			       "name", verb,
			       "tooltip", "Switch to this tab",
			       NULL);

	sync_tab_title (view, NULL, action);
	/* make sure the action is alive when handling the signal, see bug #169833 */
	g_signal_connect_object (view, "notify::title",
				 G_CALLBACK (sync_tab_title), action, 0);

	gtk_action_group_add_action_with_accel (priv->action_group, action, NULL);

	group = gtk_radio_action_get_group (GTK_RADIO_ACTION (priv->anchor_action));
	gtk_radio_action_set_group (GTK_RADIO_ACTION (action), group);

	/* set this here too, since tab-added comes after notify::active-child */
	if (do_window_get_active_child (DO_WINDOW (priv->window)) == view)
	{
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);
	}

	g_object_set_data (G_OBJECT (view), DATA_KEY, action);
	g_object_set_data (G_OBJECT (action), DATA_KEY, view);

	g_signal_connect (action, "activate",
			  G_CALLBACK (tab_action_activate_cb), menu);

	g_object_unref (action);

	do_tabs_menu_update (menu);
}

static void
notebook_page_removed_cb (DoNotebook *notebook,
			  DoView *view,
			  guint position,
			  DoTabsMenu *menu)
{
	DoTabsMenuPrivate *priv = menu->priv;
	GtkAction *action;


	action = g_object_get_data (G_OBJECT (view), DATA_KEY);
	g_return_if_fail (action != NULL);

        free_tab_id (action);

	g_signal_handlers_disconnect_by_func
		(view, G_CALLBACK (sync_tab_title), action);

	g_signal_handlers_disconnect_by_func
		(action, G_CALLBACK (tab_action_activate_cb), menu);

	g_object_set_data (G_OBJECT (view), DATA_KEY, NULL);
 	gtk_action_group_remove_action (priv->action_group, action);

	do_tabs_menu_update (menu);
}

static void
notebook_page_reordered_cb (DoNotebook *notebook,
			    DoView *view,
			    guint position,
			    DoTabsMenu *menu)
{

	do_tabs_menu_update (menu);
}

static void
connect_proxy_cb (GtkActionGroup *action_group,
		  GtkAction *action,
		  GtkWidget *proxy,
		  gpointer dummy)
{
	if (GTK_IS_MENU_ITEM (proxy))
	{
		GtkLabel *label;

		label = GTK_LABEL (gtk_bin_get_child(GTK_BIN (proxy)));

		gtk_label_set_use_underline (label, FALSE);
		gtk_label_set_ellipsize (label, PANGO_ELLIPSIZE_END);
		gtk_label_set_max_width_chars (label, LABEL_WIDTH_CHARS);
	}
}

static void
sync_active_tab (DoWindow *window,
		 GParamSpec *pspec,
		 DoTabsMenu *menu)
{
	DoView *view;
	GtkAction *action;

	view = do_window_get_active_child (DO_WINDOW (window));
	if (view == NULL) return;


	action = g_object_get_data (G_OBJECT (view), DATA_KEY);
	/* happens initially, since the ::active-child comes before
	* the ::tab-added signal
	*/
	/* FIXME that's not true with gtk+ 2.9 anymore */
	if (action == NULL) return;

	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);
}

static void
do_tabs_menu_set_window (DoTabsMenu *menu,
			   DoWindow *window)
{
	DoTabsMenuPrivate *priv = menu->priv;
	GtkWidget *notebook;
	GtkUIManager *manager;

	priv->window = window;

	manager = GTK_UI_MANAGER (do_window_get_ui_manager (window));
	priv->action_group = gtk_action_group_new ("TabsActions");
	gtk_ui_manager_insert_action_group (manager, priv->action_group, -1);
	g_object_unref (priv->action_group);

	priv->anchor_action = g_object_new (GTK_TYPE_RADIO_ACTION,
					    "name", "TabsMenuAnchorAction",
					    NULL);
	gtk_action_group_add_action (priv->action_group, priv->anchor_action);
	g_object_unref (priv->anchor_action);

	g_signal_connect (priv->action_group, "connect-proxy",
			  G_CALLBACK (connect_proxy_cb), NULL);

	g_signal_connect (window, "notify::active-child",
			  G_CALLBACK (sync_active_tab), menu);

	notebook = do_window_get_notebook (window);
	g_signal_connect_object (notebook, "page-added",
				 G_CALLBACK (notebook_page_added_cb), menu, 0);
	g_signal_connect_object (notebook, "page-removed",
				 G_CALLBACK (notebook_page_removed_cb), menu, 0);
	g_signal_connect_object (notebook, "page-reordered",
				 G_CALLBACK (notebook_page_reordered_cb), menu, 0);
}

static void
do_tabs_menu_set_property (GObject *object,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *pspec)
{
	DoTabsMenu *menu = DO_TABS_MENU (object);

	switch (prop_id)
	{
		case PROP_WINDOW:
			do_tabs_menu_set_window (menu, g_value_get_object (value));
			break;
	}
}

static void
do_tabs_menu_get_property (GObject *object,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *pspec)
{
	/* no readable properties */
	g_return_if_reached ();
}

static void
do_tabs_menu_class_init (DoTabsMenuClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = do_tabs_menu_set_property;
	object_class->get_property = do_tabs_menu_get_property;

	g_object_class_install_property (object_class,
					 PROP_WINDOW,
					 g_param_spec_object ("window", NULL, NULL,
							      DO_TYPE_WINDOW,
							      G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
							      G_PARAM_CONSTRUCT_ONLY));

	g_type_class_add_private (object_class, sizeof (DoTabsMenuPrivate));

	/* We don't want to save accels, so skip them */
	gtk_accel_map_add_filter ("<Actions>/TabsActions/JmpTab*");
}

static void
do_tabs_menu_init (DoTabsMenu *menu)
{
	menu->priv = DO_TABS_MENU_GET_PRIVATE (menu);
}

static void
do_tabs_menu_clean (DoTabsMenu *menu)
{
	DoTabsMenuPrivate *p = menu->priv;
	GtkUIManager *manager = GTK_UI_MANAGER (do_window_get_ui_manager (p->window));

	if (p->ui_id != 0 && GDK_IS_WINDOW(gtk_widget_get_window(GTK_WIDGET(p->window))))
	{
		gtk_ui_manager_remove_ui (manager, p->ui_id);
		gtk_ui_manager_ensure_update (manager);
		p->ui_id = 0;
	}
}

DoTabsMenu *
do_tabs_menu_new (DoWindow *window)
{
	return DO_TABS_MENU (g_object_new (DO_TYPE_TABS_MENU,
					     "window", window,
					     NULL));
}

static void
tab_set_action_accelerator (GtkActionGroup *action_group,
			    GtkAction *action,
			    guint tab_number)
{
	const char *verb;
	char accel_path[ACCEL_PATH_FORMAT_LENGTH];
	char accel[7];
	gint accel_number;
	guint accel_key;
	GdkModifierType accel_mods;

	verb = gtk_action_get_name (action);

	/* set the accel path for the menu item */
	g_snprintf (accel_path, sizeof (accel_path),
		    ACCEL_PATH_FORMAT, verb);
	gtk_action_set_accel_path (action, accel_path);

	/* Only the first ten tabs get accelerators starting from 1 through 0 */
	if (tab_number < 10)
	{
		accel_key = 0;
		accel_number = (tab_number + 1) % 10;

		g_snprintf (accel, sizeof (accel), "<alt>%d", accel_number);

		gtk_accelerator_parse (accel, &accel_key, &accel_mods);

		if (accel_key != 0)
		{
			gtk_accel_map_change_entry (accel_path, accel_key,
						    accel_mods, TRUE);
		}
	}
	else
	{
		gtk_accel_map_change_entry (accel_path, 0, 0, TRUE);
	}
}

static void
do_tabs_menu_update (DoTabsMenu *menu)
{
	DoTabsMenuPrivate *p = menu->priv;
	GtkUIManager *manager;
	GtkAction *action;
	GList *tabs = NULL, *l;
	guint i = 0;
	const char *verb;

	do_tabs_menu_clean (menu);

	tabs = do_window_get_children (DO_WINDOW (p->window));

	if (g_list_length (tabs) == 0) return;

	manager =  GTK_UI_MANAGER (do_window_get_ui_manager (p->window));
	p->ui_id = gtk_ui_manager_new_merge_id (manager);

	for (l = tabs; l != NULL; l = l->next)
	{
		action = g_object_get_data (G_OBJECT (l->data), DATA_KEY);
		g_return_if_fail (action != NULL);

		verb = gtk_action_get_name (action);

		tab_set_action_accelerator (p->action_group, action, i++);

		gtk_ui_manager_add_ui (manager, p->ui_id,
				       "/MainMenu/Tabs/TabsOpen",
				       verb, verb,
				       GTK_UI_MANAGER_MENUITEM, FALSE);
	}

	g_list_free (tabs);

}
#endif
