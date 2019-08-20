/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * do-window.c
 * Copyright (C) 2015 Ришес Юрий <mir@glekar.com>
 *
 * price is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * price is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dowindow.h"
#include "dosettings.h"
#include "config.h"
#include <stdlib.h>

#define GEOMETRY_REGEX "(?P<width>[0-9]+)x(?P<height>[0-9]+)\\+(?P<left>[0-9]+)\\+(?P<top>[0-9]+)"

struct _DoWindowPrivate
{
	 gchar *name;
};


enum
{
	PROP_0,

	PROP_NAME
};



G_DEFINE_TYPE (DoWindow, do_window, GTK_TYPE_WINDOW);

static gboolean
do_window_configure_event_cb (GtkWidget *widget, GdkEventConfigure *event);
static gboolean
do_window_state_event_cb (GtkWidget* window, GdkEventWindowState* event);

static void
do_window_init (DoWindow *do_window)
{
	do_window->priv = G_TYPE_INSTANCE_GET_PRIVATE (do_window, DO_TYPE_WINDOW, DoWindowPrivate);
	g_signal_connect (do_window, "window-state-event",
                      G_CALLBACK (do_window_state_event_cb), NULL);
    g_signal_connect (do_window, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), NULL);
}

static GObject*
do_window_constructor (GType type,
                       guint n_construct_properties,
                       GObjectConstructParam *construct_params)
{
    GObject *object;

    object = G_OBJECT_CLASS (do_window_parent_class)->constructor
        (type, n_construct_properties, construct_params);

	gtk_window_load_geometry (GTK_WINDOW (object));

	return object;
}
static void
do_window_finalize (GObject *object)
{
	G_OBJECT_CLASS (do_window_parent_class)->finalize (object);
}


static void
do_window_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	g_return_if_fail (DO_IS_WINDOW (object));
	DoWindowPrivate *priv = DO_WINDOW (object)->priv;

	switch (prop_id)
	{
	case PROP_NAME:
		g_free (priv->name);
			priv->name = g_value_dup_string (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
do_window_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	g_return_if_fail (DO_IS_WINDOW (object));
	DoWindowPrivate *priv = DO_WINDOW (object)->priv;

	switch (prop_id)
	{
	case PROP_NAME:
		g_value_set_string (value, priv->name);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
do_window_class_init (DoWindowClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (DoWindowPrivate));

	object_class->constructor = do_window_constructor;
	object_class->finalize = do_window_finalize;
	object_class->set_property = do_window_set_property;
	object_class->get_property = do_window_get_property;

	g_object_class_install_property (object_class,
	                                 PROP_NAME,
	                                 g_param_spec_string ("name",
	                                                      "name",
	                                                      "name",
	                                                      "window",
	                                                      G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}


GtkWidget *
do_window_new (GtkWindowType type, const gchar *name)
{
	return g_object_new (DO_TYPE_WINDOW,
                         "type", type,
	                     "name", name,
	                     NULL);
}

void
gtk_window_load_geometry (GtkWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW (window)->priv;
    gchar *geometry;
    gint  width, height, x ,y;
    gboolean is_maximized;
    GRegex *re = NULL;
    GMatchInfo *match_info = NULL;

    is_maximized =do_settings_get_boolean (priv->name, "maximized", FALSE);
	if ( is_maximized )
        gtk_window_maximize (window);
    else
        gtk_window_unmaximize (window);

	geometry = do_settings_get_string (priv->name, "geometry", NULL);
	if ( geometry ) {
        re = g_regex_new(GEOMETRY_REGEX,
                         G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, 0, NULL);
        g_regex_match (re, geometry, 0, &match_info);
        if ( g_match_info_matches (match_info) ) {
            char *value;
            value = g_match_info_fetch_named (match_info, "width");
            if ( value )
                width = atoi (value);
            value = g_match_info_fetch_named (match_info, "height");
            if ( value )
                height = atoi (value);
            value = g_match_info_fetch_named (match_info, "left");
            if ( value )
                x = atoi(value);
            value = g_match_info_fetch_named (match_info, "top");
            if ( value )
                y = atoi (value);
            gtk_window_move (window, x, y);

            gtk_window_set_default_geometry (window, width, height);
        }
		g_free (geometry);
    }
}

void
gtk_window_save_geometry (GtkWindow *window)
{
	DoWindowPrivate *priv = DO_WINDOW (window)->priv;
    gchar *geometry;
    gint  width, height, x ,y;
    gboolean is_maximized;

    if ( gtk_widget_get_window (GTK_WIDGET (window)) ) {

        gtk_window_get_size (window, &width, &height);
        gtk_window_get_position (window, &x, &y);
        is_maximized = gdk_window_get_state (gtk_widget_get_window (GTK_WIDGET (window))) & GDK_WINDOW_STATE_MAXIMIZED;

        geometry = g_strdup_printf ("%dx%d+%d+%d",width,height,x,y);

		if ( !is_maximized )
            do_settings_set_string (priv->name, "geometry", geometry);
        do_settings_set_boolean (priv->name, "maximized", is_maximized);
		g_free (geometry);
    }
}
static gboolean
do_window_configure_event_cb (GtkWidget *widget, GdkEventConfigure *event)
{
	gtk_window_save_geometry (GTK_WINDOW (widget));
	return FALSE;
}
static gboolean
do_window_state_event_cb (GtkWidget* window, GdkEventWindowState* event)
{
	DoWindowPrivate *priv = DO_WINDOW (window)->priv;
    do_settings_set_boolean (priv->name, "maximized", event->new_window_state & GDK_WINDOW_STATE_ICONIFIED);
    return FALSE;
}
