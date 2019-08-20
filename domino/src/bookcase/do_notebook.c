
#include "do_notebook.h"
#include "do_view_actions.h"
#include "do_utilx.h"
#include "do_window.h"
#include <gtk/gtk.h>

#define TAB_WIDTH_N_CHARS 15

#define AFTER_ALL_TABS -1
#define NOT_IN_APP_WINDOWS -2

#define INSANE_NUMBER_OF_URLS 20

#define DO_NOTEBOOK_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_NOTEBOOK, DoNotebookPrivate))

struct _DoNotebookPrivate
{
	GList *focused_pages;
	guint tabs_vis_notifier_id;
	//gboolean load_status;
	gint     load_progress;

	guint show_tabs : 1;
};

static void do_notebook_init		 (DoNotebook *notebook);
static void do_notebook_class_init	 (DoNotebookClass *klass);
static void do_notebook_finalize	 (GObject *object);
static int  do_notebook_insert_page	 (GtkNotebook *notebook,
					  GtkWidget *child,
					  GtkWidget *tab_label,
					  GtkWidget *menu_label,
					  int position);
static void do_notebook_remove	 (GtkContainer *container,
					  GtkWidget *tab_widget);
static void sync_label (DoView *view, GParamSpec *pspec, GtkWidget *label);
static void sync_load_status (DoView *view, GParamSpec *pspec, GtkWidget *proxy);
static void sync_load_progress (DoView *view, GParamSpec *pspec, GtkWidget *proxy);

enum
{
	PROP_0,
	PROP_SHOW_TABS,
	PROP_LOAD_STATUS,
	PROP_LOAD_PROGRESS
};

enum
{
	TAB_CLOSE_REQUEST,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];
static gboolean get_load_status(DoNotebook *notebook);
static gboolean get_load_progress(DoNotebook *notebook);

G_DEFINE_TYPE (DoNotebook, do_notebook, GTK_TYPE_NOTEBOOK)

static void do_notebook_get_property (GObject *object,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *pspec)
{
	DoNotebook *notebook = DO_NOTEBOOK (object);
	DoNotebookPrivate *priv = DO_NOTEBOOK_GET_PRIVATE(notebook);

	switch (prop_id)
	{
		case PROP_SHOW_TABS:
			g_value_set_boolean (value, priv->show_tabs);
			break;
        case PROP_LOAD_STATUS:
            g_value_set_boolean(value, get_load_status(notebook));
            break;
        case PROP_LOAD_PROGRESS:
            g_value_set_int(value, get_load_progress(notebook));
            break;

	}
}

static void do_notebook_set_property (GObject *object,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *pspec)
{
	DoNotebook *notebook = DO_NOTEBOOK (object);

	switch (prop_id)
	{
		case PROP_SHOW_TABS:
			do_notebook_set_show_tabs (notebook, g_value_get_boolean (value));
			break;
	}
}

static void do_notebook_class_init (DoNotebookClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);
	GtkNotebookClass *notebook_class = GTK_NOTEBOOK_CLASS (klass);

	object_class->finalize = do_notebook_finalize;
	object_class->get_property = do_notebook_get_property;
	object_class->set_property = do_notebook_set_property;

	container_class->remove = do_notebook_remove;

	notebook_class->insert_page = do_notebook_insert_page;
	signals[TAB_CLOSE_REQUEST] =
		g_signal_new ("tab-close-request",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (DoNotebookClass, tab_close_req),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__OBJECT,
			      G_TYPE_NONE,
			      1,
			      GTK_TYPE_WIDGET /* Can't use an interface type here */);
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
							"Процент загрузки",
							"Справочник загружается",
							0,100,0,
							G_PARAM_READABLE));


	g_object_class_install_property (object_class,
					 PROP_SHOW_TABS,
					 g_param_spec_boolean ("show-tabs", NULL, NULL,
							       TRUE,
							       G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

	g_type_class_add_private (object_class, sizeof (DoNotebookPrivate));
}

static DoNotebook *find_notebook_at_pointer (gint abs_x, gint abs_y)
{
	GdkWindow *win_at_pointer, *toplevel_win;
	gpointer toplevel = NULL;
	gint x, y;

    GdkDevice *pointer;
#if GTK_CHECK_VERSION(3,20,0)
    GdkDisplay *display;
    display = gdk_display_get_default();
    pointer = gdk_seat_get_pointer(gdk_display_get_default_seat(display));
#else
    GdkDeviceManager *manager;
    manager = gdk_display_get_device_manager(gdk_display_get_default ());
    pointer = gdk_device_manager_get_client_pointer (manager);
#endif
    win_at_pointer = gdk_device_get_window_at_position (pointer, &x, &y);
	if (win_at_pointer == NULL)
	{
		/* We are outside all windows containing a notebook */
		return NULL;
	}

	toplevel_win = gdk_window_get_toplevel (win_at_pointer);

	/* get the GtkWidget which owns the toplevel GdkWindow */
	gdk_window_get_user_data (toplevel_win, &toplevel);

	/* toplevel should be an DoWindow */
	if (toplevel != NULL && GTK_IS_WINDOW (toplevel))
	{
		return DO_NOTEBOOK(do_window_get_notebook(DO_WINDOW(toplevel)));
	}

	return NULL;
}
static gboolean is_in_notebook_window (DoNotebook *notebook,
		       gint abs_x, gint abs_y)
{
	DoNotebook *nb_at_pointer;

	nb_at_pointer = find_notebook_at_pointer (abs_x, abs_y);

	return nb_at_pointer == notebook;
}

static gint find_tab_num_at_pos (DoNotebook *notebook, gint abs_x, gint abs_y)
{
	GtkPositionType tab_pos;
	int page_num = 0;
	GtkNotebook *nb = GTK_NOTEBOOK (notebook);
	GtkWidget *page;

	tab_pos = gtk_notebook_get_tab_pos (GTK_NOTEBOOK (notebook));


	if ( !gtk_notebook_get_n_pages(GTK_NOTEBOOK (notebook)) )
	{
		return AFTER_ALL_TABS;
	}

	/* For some reason unfullscreen + quick click can
	   cause a wrong click event to be reported to the tab */
	if (!is_in_notebook_window(notebook, abs_x, abs_y))
	{
		return NOT_IN_APP_WINDOWS;
	}

	while ((page = gtk_notebook_get_nth_page (nb, page_num)))
	{
		GtkWidget *tab;
		gint max_x, max_y;
		gint x_root, y_root;

		tab = gtk_notebook_get_tab_label (nb, page);
		g_return_val_if_fail (tab != NULL, -1);

		if ( !gtk_widget_get_mapped(GTK_WIDGET (tab)) )
		{
			page_num++;
			continue;
		}

		gdk_window_get_origin (GDK_WINDOW (gtk_widget_get_window(tab)),
				       &x_root, &y_root);

        GtkAllocation allocation;
        gtk_widget_get_allocation(tab, &allocation);
		max_x = x_root + allocation.x + allocation.width;
		max_y = y_root + allocation.y + allocation.height;

		if (((tab_pos == GTK_POS_TOP)
		     || (tab_pos == GTK_POS_BOTTOM))
		    &&(abs_x<=max_x))
		{
			return page_num;
		}
		else if (((tab_pos == GTK_POS_LEFT)
			  || (tab_pos == GTK_POS_RIGHT))
			 && (abs_y<=max_y))
		{
			return page_num;
		}

		page_num++;
	}
	return AFTER_ALL_TABS;
}

static gboolean
button_press_cb (DoNotebook *notebook,
		 GdkEventButton *event,
		 gpointer data)
{
	int tab_clicked;

	tab_clicked = find_tab_num_at_pos (notebook, event->x_root, event->y_root);

	if (event->type == GDK_BUTTON_PRESS &&
	    event->button == 3 &&
		   (event->state & gtk_accelerator_get_default_mod_mask ()) == 0)
	{
		if (tab_clicked == -1)
		{
			/* consume event, so that we don't pop up the context menu when
			 * the mouse if not over a tab label
			 */
			return TRUE;
		}

		/* switch to the page the mouse is over, but don't consume the event */
		gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), tab_clicked);
	}
	if (event->type == GDK_BUTTON_PRESS &&
	    event->button == 2 &&
		   (event->state & gtk_accelerator_get_default_mod_mask ()) == 0)
	{
        GtkWidget *child;
	    if ( tab_clicked != -1 &&
            (child = gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), tab_clicked)) != NULL ) {
            do_view_do_close(DO_VIEW(child));
            return TRUE;
        }
	}

	return FALSE;
}
static gboolean do_notebook_key_press_cb(GtkWidget *widget, GdkEventKey *event, gpointer data)
{

	guint mask = gtk_accelerator_get_default_mod_mask ();
	if ((event->state & mask) == 0)
	{
		switch (event->keyval)
		{
            case GDK_KEY_Escape: {
                GtkWidget *child;
                gint page_num;
                page_num = gtk_notebook_get_current_page(GTK_NOTEBOOK(widget));

                if ( page_num != -1 &&
                    (child = gtk_notebook_get_nth_page (GTK_NOTEBOOK(widget), page_num)) != NULL &&
                     do_view_can_do_close_for_esc(DO_VIEW(child)) ) {
                    do_view_do_close(DO_VIEW(child));
                    return TRUE;
                }
            	return FALSE;
            }
            default:
                break;
		}
	}
    return FALSE;
}


static void
do_notebook_switch_page_cb (GtkNotebook *notebook,
			      GtkWidget *page,
			      guint page_num,
			      gpointer data)
{
	DoNotebookPrivate *priv = DO_NOTEBOOK_GET_PRIVATE(notebook);
	GtkWidget *child;

	child = gtk_notebook_get_nth_page (notebook, page_num);

	/* Remove the old page, we dont want to grow unnecessarily
	 * the list */
    //do_view_actions_set_view(child);
    do_view_actions_refresh(GTK_WIDGET(notebook));

	if ( priv->focused_pages )
	{
		priv->focused_pages =
			g_list_remove (priv->focused_pages, child);
	}

	priv->focused_pages = g_list_append (priv->focused_pages, child);

    g_object_notify (G_OBJECT (notebook), "load-status");
    g_object_notify (G_OBJECT (notebook), "load-progress");
}
static void
do_notebook_removed_page_cb (GtkNotebook *notebook,
			      GtkWidget *page,
			      guint page_num,
			      gpointer data)
{
    g_object_notify (G_OBJECT (notebook), "load-status");
    g_object_notify (G_OBJECT (notebook), "load-progress");
}

/*
 * update_tabs_visibility: Hide tabs if there is only one tab
 * and the pref is not set.
 */
static void
update_tabs_visibility (DoNotebook *nb,
			gboolean before_inserting)
{
	DoNotebookPrivate *priv = nb->priv;
	//gboolean show_tabs;
	//guint num;

	//num = gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb));

	//if (before_inserting) num++;

	//show_tabs = ( num > 1) &&
		//    priv->show_tabs;// == TRUE;

	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (nb), priv->show_tabs);
}

static void do_notebook_init (DoNotebook *notebook)
{
	DoNotebookPrivate *priv;

	priv = notebook->priv = DO_NOTEBOOK_GET_PRIVATE (notebook);

	gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), TRUE);
	gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook), FALSE);
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), FALSE);


	priv->show_tabs = TRUE;

	g_signal_connect (notebook, "button-press-event",
			  (GCallback)button_press_cb, NULL);

	g_signal_connect_after (notebook, "switch-page",
				G_CALLBACK (do_notebook_switch_page_cb),
				NULL);
	g_signal_connect_after (notebook, "page-removed",
				G_CALLBACK (do_notebook_removed_page_cb),
				NULL);

	g_signal_connect_after (notebook, "key-press-event",
				G_CALLBACK (do_notebook_key_press_cb),
				NULL);




}

static void do_notebook_finalize (GObject *object)
{
	DoNotebook *notebook = DO_NOTEBOOK (object);
	DoNotebookPrivate *priv = DO_NOTEBOOK_GET_PRIVATE(notebook);

	g_list_free (priv->focused_pages);

	G_OBJECT_CLASS (do_notebook_parent_class)->finalize (object);
}

static void close_button_clicked_cb (GtkWidget *widget, GtkWidget *tab)
{
	GtkWidget *notebook;

	notebook = gtk_widget_get_parent (tab);
	g_signal_emit (notebook, signals[TAB_CLOSE_REQUEST], 0, tab);
}

static GtkWidget *build_tab_label (DoNotebook *nb, DoView *view)
{
	GtkWidget *hbox, *label, *close_button, *image;
	/* set hbox spacing and label padding (see below) so that there's an
	 * equal amount of space around the label */
    hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_show (hbox);
	/* setup load feedback */
   static const gchar *style =
       "* {\n"
        "-GtkButton-default-border : 0;\n"
        "-GtkButton-default-outside-border : 0;\n"
        "-GtkButton-inner-border: 0;\n"
        "-GtkWidget-focus-line-width : 0;\n"
        "-GtkWidget-focus-padding : 0;\n"
        "padding: 0;\n"
        "}";
    GtkStyleContext* context;
    GtkCssProvider* css_provider;
    context = gtk_widget_get_style_context (hbox);
    css_provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_data (css_provider, style, -1, NULL);
    gtk_style_context_add_provider (context, GTK_STYLE_PROVIDER (css_provider),
                                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	/* setup label */
	label = gtk_label_new (NULL);

	//gtk_label_set_ellipsize (GTK_LABEL (label), PANGO_ELLIPSIZE_END);

	gtk_label_set_single_line_mode (GTK_LABEL (label), TRUE);
	g_object_set(G_OBJECT(label), "halign", 0.0, "valign", 0.5, NULL);
	//to do gtk_misc_set_padding (GTK_MISC (label), 0, 0);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	gtk_widget_show (label);

	/* setup close button */
	close_button = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (close_button),
			       GTK_RELIEF_NONE);
	/* don't allow focus on the close button */
#if GTK_CHECK_VERSION(3,20,0)
	gtk_widget_set_focus_on_click (GTK_WIDGET (close_button), FALSE);
#else
	gtk_button_set_focus_on_click (GTK_BUTTON (close_button), FALSE);
#endif
	gtk_widget_set_name (close_button, "do-tab-close-button");

	image = gtk_image_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);
	gtk_widget_set_tooltip_text (close_button, "Закрыть");
	g_signal_connect (close_button, "clicked",
			  G_CALLBACK (close_button_clicked_cb), view);

	gtk_container_add (GTK_CONTAINER (close_button), image);
	gtk_widget_show (image);

	gtk_box_pack_start (GTK_BOX (hbox), close_button, FALSE, FALSE, 0);
	gtk_widget_show (close_button);

	/* Set minimal size */
	//!!g_signal_connect (hbox, "style-set",
		//	  G_CALLBACK (tab_label_style_set_cb), NULL);

	g_object_set_data (G_OBJECT (hbox), "label", label);
	g_object_set_data (G_OBJECT (hbox), "close-button", close_button);
	g_object_set_data (G_OBJECT (hbox), "notebook", nb);

	/* Hook the label up to the tab properties */
	sync_label (view, NULL, label);
	sync_load_status (view, NULL, hbox);
	sync_load_progress (view, NULL, GTK_WIDGET(nb));

	g_signal_connect_object (view, "notify::title",
				 G_CALLBACK (sync_label), label, 0);
	g_signal_connect_object (view, "notify::load-status",
				 G_CALLBACK (sync_load_status), hbox, 0);
	g_signal_connect_object (view, "notify::load-progress",
				 G_CALLBACK (sync_load_progress), nb, 0);

	return hbox;
}

void
do_notebook_set_show_tabs (DoNotebook *nb,
			     gboolean show_tabs)
{
	DoNotebookPrivate *priv = nb->priv;

	priv->show_tabs = show_tabs != FALSE;

	update_tabs_visibility (nb, FALSE);
}

GList *
do_notebook_get_focused_pages (DoNotebook *nb)
{
	g_return_val_if_fail (DO_IS_NOTEBOOK (nb), NULL);

	return nb->priv->focused_pages;
}

static int
do_notebook_insert_page (GtkNotebook *gnotebook,
			   GtkWidget *tab_widget,
			   GtkWidget *tab_label,
			   GtkWidget *menu_label,
			   int position)
{
	DoNotebook *notebook = DO_NOTEBOOK (gnotebook);

	/* Destroy passed-in tab label */
	if (tab_label != NULL)
	{
		g_object_ref_sink (tab_label);
		g_object_unref (tab_label);
	}

	g_assert (DO_IS_VIEW (tab_widget));

	tab_label = build_tab_label (notebook, DO_VIEW (tab_widget));

	update_tabs_visibility (notebook, TRUE);

	position = GTK_NOTEBOOK_CLASS (do_notebook_parent_class)->insert_page (gnotebook,
										 tab_widget,
										 tab_label,
										 menu_label,
										 position);

	gtk_notebook_set_tab_reorderable (gnotebook, tab_widget, TRUE);

	get_load_status(DO_NOTEBOOK(gnotebook));

	return position;
}

int do_notebook_add_tab (DoNotebook *notebook,
		       DoView *view,
		       int position,
		       gboolean jump_to)
{
	GtkNotebook *gnotebook = GTK_NOTEBOOK (notebook);

	g_return_val_if_fail (DO_IS_NOTEBOOK (notebook), -1);

	position = gtk_notebook_insert_page (GTK_NOTEBOOK (notebook),
					     GTK_WIDGET (view),
					     NULL,
					     position);

	/* FIXME gtk bug! */
	/* FIXME: this should be fixed in gtk 2.12; check & remove this! */
	/* The signal handler may have reordered the tabs */
	position = gtk_notebook_page_num (gnotebook, GTK_WIDGET (view));


	if (jump_to)
	{
		gtk_notebook_set_current_page (gnotebook, position);
		g_object_set_data (G_OBJECT (view), "jump_to",
				   GINT_TO_POINTER (jump_to));
	    do_view_do_grab_focus(view);


	}

	return position;
}

static void
smart_tab_switching_on_closure (DoNotebook *notebook,
				GtkWidget *tab)
{
	DoNotebookPrivate *priv = DO_NOTEBOOK_GET_PRIVATE(notebook);
	gboolean jump_to;

	jump_to = GPOINTER_TO_INT (g_object_get_data
				   (G_OBJECT (tab), "jump_to"));

	if (!jump_to || !priv->focused_pages)
	{
		gtk_notebook_next_page (GTK_NOTEBOOK (notebook));
	}
	else
	{
		GList *l;
		GtkWidget *child;
		int page_num;

		/* activate the last focused tab */
		l = g_list_last (priv->focused_pages);
		child = GTK_WIDGET (l->data);
		page_num = gtk_notebook_page_num (GTK_NOTEBOOK (notebook),
						  child);
		gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook),
					       page_num);
	}
}

static void
do_notebook_remove (GtkContainer *container,
		      GtkWidget *tab_widget)
{
	GtkNotebook *gnotebook = GTK_NOTEBOOK (container);
	DoNotebook *notebook = DO_NOTEBOOK (container);
	DoNotebookPrivate *priv = DO_NOTEBOOK_GET_PRIVATE(notebook);
	//GtkWidget *tab_label;//, *tab_label_label, *tab_label_icon;
	int position, curr;

	/* Remove the page from the focused pages list */
	priv->focused_pages =  g_list_remove (priv->focused_pages, tab_widget);

	position = gtk_notebook_page_num (gnotebook, tab_widget);
	curr = gtk_notebook_get_current_page (gnotebook);

	if (position == curr)
	{
		smart_tab_switching_on_closure (notebook, tab_widget);
	}

	/* Prepare tab label for destruction */
	//tab_label = gtk_notebook_get_tab_label (gnotebook, tab_widget);
	//tab_label_icon = g_object_get_data (G_OBJECT (tab_label), "icon");
	//tab_label_label = g_object_get_data (G_OBJECT (tab_label), "label");

	GTK_CONTAINER_CLASS (do_notebook_parent_class)->remove (container, tab_widget);

	update_tabs_visibility (notebook, FALSE);

	/* if that was the last tab, destroy the window */
	if (FALSE)//!!gtk_notebook_get_n_pages (gnotebook) == 0)
	{
		gtk_widget_destroy (gtk_widget_get_toplevel (GTK_WIDGET (notebook)));
	}
	if (gtk_notebook_get_n_pages (gnotebook) == 0) {
        do_view_actions_refresh(GTK_WIDGET(container));
	}
}
static void
sync_label (DoView *view, GParamSpec *pspec, GtkWidget *label)
{
	const char *title;

	title = do_view_get_title (view);

	gtk_label_set_text (GTK_LABEL (label), title);

	/* Set the tooltip on the label's parent (the tab label hbox),
	 * so it covers all of the tab label.
	 */
	gtk_widget_set_tooltip_text (gtk_widget_get_parent(label), title);
}
static void
sync_load_status (DoView *view, GParamSpec *pspec, GtkWidget *proxy)
{
	GtkWidget *nb;
	nb = GTK_WIDGET (g_object_get_data (G_OBJECT (proxy), "notebook"));
	get_load_status(DO_NOTEBOOK(nb));
}
static void
sync_load_progress (DoView *view, GParamSpec *pspec, GtkWidget *proxy)
{

    get_load_progress(DO_NOTEBOOK(proxy));

}

static gboolean get_load_status(DoNotebook *notebook)
{
	gboolean load_status = FALSE;
    int page_num = 0;
	GtkNotebook *nb = GTK_NOTEBOOK (notebook);
	GtkWidget *page;

    page_num = gtk_notebook_get_current_page(nb);
    page = gtk_notebook_get_nth_page (nb, page_num);

    if ( DO_IS_VIEW(page) ) {
        load_status = do_view_get_load_status(DO_VIEW(page));
    }
    else {
        load_status = FALSE;
    }
	return load_status;
}
static gint get_load_progress(DoNotebook *notebook)
{
	DoNotebookPrivate *priv = notebook->priv;
	gint load_progress = 0;
    int page_num = 0;
    GtkNotebook *nb = GTK_NOTEBOOK(notebook);
	GtkWidget *page;
    page_num = gtk_notebook_get_current_page(nb);
    page = gtk_notebook_get_nth_page (nb, page_num);

    if ( DO_IS_VIEW(page) ) {
        load_progress = do_view_get_load_progress(DO_VIEW(page));
    }
    else {
        load_progress = 100;
    }

	if (priv->load_progress != load_progress) {
	    priv->load_progress = load_progress;
        g_object_notify (G_OBJECT (notebook), "load-progress");
	}
	return priv->load_progress;
}
