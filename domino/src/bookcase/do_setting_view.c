

#include "do_setting_view.h"
#include "do_view.h"
#include "do_utilx.h"
#include "do_view_actions.h"
#include "domino.h"

#include <string.h>

#define DO_SETTING_VIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), DO_TYPE_SETTING_VIEW, DoSettingViewPrivate))

static const gchar *title[DOMINO_CONFIG_N] =
     {"Параметры", "Настройки"};
// View interface
static void do_setting_view_view_init(DoViewIface *iface);
static gboolean view_close_request(DoView *embed);



static void do_setting_view_changed(GtkTextBuffer *textbuffer, DoSettingView *view);
static void        view_close(DoView *embed);
static const char *view_get_title(DoView *embed);
static GdkPixbuf  *view_get_icon			(DoView *embed);
//static gboolean	   view_get_load_status	(DoView *embed);

static void		command_do_save(DoView *view);
static gboolean	command_can_do_save(DoView *view);
static gboolean command_do_grab_focus(DoView *view);
static gboolean command_can_do_close_for_esc(DoView *view);

enum
{
	PROP_0,
	PROP_TITLE,
	PROP_KIND,
};

struct _DoSettingViewPrivate
{
    GtkWidget       *view;
    GdkPixbuf       *icon;
    gchar           *title;
    gchar           *file_name;
	GtkTextBuffer *buffer;
    gboolean         changed;
    gint             kind;
};

G_DEFINE_TYPE_WITH_CODE (DoSettingView, do_setting_view, GTK_TYPE_SCROLLED_WINDOW,
			 G_IMPLEMENT_INTERFACE (DO_TYPE_VIEW,
                                    do_setting_view_view_init)
             G_ADD_PRIVATE(DoSettingView)
                            );

static void do_setting_view_view_init(DoViewIface *iface)
{
    iface->do_close        = view_close;
    iface->close_request   = view_close_request;
    iface->get_title       = view_get_title;
    iface->get_icon        = view_get_icon;
    //iface->get_load_status = view_get_load_status;
    iface->do_save         = command_do_save;
    iface->can_do_save     = command_can_do_save;
    iface->do_grab_focus   = command_do_grab_focus;
    iface->can_do_close_for_esc = command_can_do_close_for_esc;
}

static void focus_in(GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
    //to do do_view_actions_set_view(user_data);
}

static void do_setting_view_init(DoSettingView *temp)
{
#if GTK_MAJOR_VERSION == 2
	DoSettingViewPrivate *priv = DO_SETTING_VIEW_GET_PRIVATE (temp);
    GtkIconTheme *icon_theme;
    icon_theme = gtk_icon_theme_get_default();
    priv->icon = gtk_icon_theme_load_icon(icon_theme, GTK_STOCK_PROPERTIES, DO_VIEW_ICON_SIZE, 0, NULL);
#endif
    //DoSettingViewPrivate *priv = DO_SETTING_VIEW_GET_PRIVATE(temp);
	//memset(priv, 0, sizeof(*priv));
}

#define LOAD_BUFFER_SIZE 65536

static GObject *do_setting_view_constructor(GType type, guint n_construct_properties, GObjectConstructParam *construct_params)
{
	GObject            *object;
	DoSettingViewPrivate  *priv;
	GtkWidget          *view;
    //GFileInputStream   *stream;
    GtkTextBuffer    *buffer;
    GtkTextIter         iter;
//	GtkSourceLanguage  *language;
//	GtkSourceLanguageManager *language_manager;
    //gchar **mime_types;

	object = G_OBJECT_CLASS (do_setting_view_parent_class)->constructor(type, n_construct_properties, construct_params);
	priv = DO_SETTING_VIEW_GET_PRIVATE (object);

//    language_manager = gtk_source_language_manager_new ();
    priv->title = do_strdup(title[priv->kind]);


//	language = gtk_source_language_manager_get_language(language_manager, "xml");
//	mime_types = gtk_source_language_get_mime_types (language);

	//buffer = priv->buffer = gtk_source_buffer_new_with_language(language);
	buffer = priv->buffer = gtk_text_buffer_new(NULL);
	//view = priv->view = gtk_source_view_new_with_buffer (buffer);
	view = priv->view = gtk_text_view_new_with_buffer (buffer);
    gtk_container_add (GTK_CONTAINER (object), view);

    g_signal_connect(priv->view,  "focus-in-event",  G_CALLBACK(focus_in), object);

    domino_config_save(priv->kind, TRUE);
    gchar *filename;

    filename = g_locale_from_utf8(domino_config_filename(priv->kind), -1, NULL, NULL, NULL);

    gchar *text = do_read_from_file(filename);
    g_free(filename);

    gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(buffer), &iter, 0);
    if (text) {
        gtk_text_buffer_insert (GTK_TEXT_BUFFER(buffer), &iter, text, -1);
        gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(buffer), &iter, 0);
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &iter);
    }

    g_signal_connect(priv->buffer,  "changed",  G_CALLBACK(do_setting_view_changed), object);

    gtk_widget_show_all(GTK_WIDGET(object));
	return object;
}

static void do_setting_view_finalize (GObject *object)
{
	DoSettingViewPrivate *priv = DO_SETTING_VIEW_GET_PRIVATE (object);
	if (priv->title)
        g_free(priv->title);
	G_OBJECT_CLASS (do_setting_view_parent_class)->finalize (object);
}

static void do_setting_view_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	DoSettingViewPrivate *priv = DO_SETTING_VIEW_GET_PRIVATE (object);
	switch (prop_id)
	{
        case PROP_TITLE:
            g_value_set_string(value, priv->title);
            break;
        case PROP_KIND:
            g_value_set_int(value, priv->kind);
            break;
        default :
            g_assert_not_reached ();
	}
}

static void do_setting_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	DoSettingViewPrivate *priv = DO_SETTING_VIEW_GET_PRIVATE (object);

	switch (prop_id)
	{
        case PROP_KIND:
            priv->kind = g_value_get_int(value);
            break;
        default :
            g_assert_not_reached ();
	}
}

static void do_setting_view_class_init (DoSettingViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->constructor  = do_setting_view_constructor;
	object_class->finalize     = do_setting_view_finalize;
	object_class->get_property = do_setting_view_get_property;
	object_class->set_property = do_setting_view_set_property;

	//g_type_class_add_private (object_class, sizeof (DoSettingViewPrivate));

	g_object_class_install_property
		(object_class,
		 PROP_KIND,
		 g_param_spec_int ("kind",
				     "Вид настроек",
				     "Вид настроек",
				     0,
				     DOMINO_CONFIG_N - 1,
				     DOMINO_CONFIG_PROFILE,
				     G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB |
				     G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property
        (object_class,
         PROP_TITLE,
         g_param_spec_string("title",
                      "",
                      "",
                      NULL,
                       G_PARAM_READABLE));
}

GtkWidget *do_setting_view_new (gint kind)
{
	return g_object_new (DO_TYPE_SETTING_VIEW,
                                "vscrollbar-policy", GTK_POLICY_AUTOMATIC,
                                "hscrollbar-policy", GTK_POLICY_AUTOMATIC,
                                "kind", kind,
			     (gpointer) NULL);
}

static gboolean view_close_request(DoView *embed)
{
    return TRUE;
}
static void view_close(DoView *embed)
{
    gtk_widget_destroy(GTK_WIDGET(embed));
}
static const char *view_get_title(DoView *embed)
{
	DoSettingViewPrivate  *priv;
	priv = DO_SETTING_VIEW_GET_PRIVATE (embed);

    return priv->title;
}
static GdkPixbuf    *view_get_icon			(DoView *embed)
{
	DoSettingViewPrivate  *priv;
	priv = DO_SETTING_VIEW_GET_PRIVATE (embed);

    return priv->icon;
}
/*
static gboolean	  view_get_load_status	(DoView *embed)
{
    return FALSE;
}*/

static void command_do_save(DoView *view)
{
	DoSettingViewPrivate  *priv;
	priv = DO_SETTING_VIEW_GET_PRIVATE (view);
	GtkTextIter start, end;
    char *text;

    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(priv->buffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(priv->buffer), &end);
    text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(priv->buffer),
                                    &start, &end, FALSE);

	if (domino_config_save_text(priv->kind, text)) {
	    domino_config_read(priv->kind);
        if (priv->changed) {
            priv->changed = FALSE;
            if (priv->title)
                g_free(priv->title);
            priv->title = g_strdup(title[priv->kind]);
            g_signal_emit_by_name (view, "notify::title", NULL);
            do_view_actions_refresh(GTK_WIDGET(view));
        }
	}

}
static void do_setting_view_changed(GtkTextBuffer *textbuffer, DoSettingView *view)
{
	DoSettingViewPrivate  *priv;
	priv = DO_SETTING_VIEW_GET_PRIVATE (view);

    if (!priv->changed) {
        priv->changed = TRUE;
        if (priv->title)
            g_free(priv->title);
        priv->title = g_strdup_printf("*%s", title[priv->kind]);
        g_signal_emit_by_name (view, "notify::title", NULL);
        do_view_actions_refresh(GTK_WIDGET(view));
    }
}

static gboolean	command_can_do_save(DoView *view)
{
	DoSettingViewPrivate  *priv;
	priv = DO_SETTING_VIEW_GET_PRIVATE (view);


    return priv->changed;
}

static gboolean	command_do_grab_focus(DoView *view)
{
	DoSettingViewPrivate  *priv;
	priv = DO_SETTING_VIEW_GET_PRIVATE (view);
	gtk_widget_grab_focus(GTK_WIDGET(priv->view));
	return TRUE;
}
static gboolean command_can_do_close_for_esc(DoView *view)
{
	return TRUE;
}
