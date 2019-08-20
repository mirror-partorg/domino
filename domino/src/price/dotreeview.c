/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * do-tree-view.c
 * Copyright (C) 2015 Ришес Юрий <mir@glekar.com>
 *
 * do-tree-view.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * do-tree-view.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dotreeview.h"
#include "dosettings.h"
#include "config.h"
#include <string.h>

#define DO_TREE_VIEW_SEARCH_TIMEOUT 5000


struct _DoTreeViewPrivate
{
    gchar                *name;

    gchar                *search_text;
    //gint                  search_char_count;
    gboolean              search_only_russian;
    DoTreeViewSearchFunc  search_func;
	gpointer              search_user_data;
    GDestroyNotify        search_destroy;
    guint                 search_flush_timeout;
    gboolean              search_inline;
    GtkTreeViewColumn    *search_inline_column;

    guint                 setting_save_timeout;

    GHashTable           *widths;
    gint                  n_column;
};

enum
{
	PROP_0,

    PROP_NAME,
};

static gboolean do_tree_view_key_press_event_cb (GtkWidget *widget,
               GdkEventKey  *event,
               gpointer   user_data);
static gboolean do_tree_view_button_press_event_cb (GtkWidget *widget,
               GdkEvent  *event,
               gpointer   user_data);
static gchar*   do_tree_view_search_get_text (DoTreeView *view,
               const gchar *string,
               gint length);
static gboolean do_tree_view_search_add_text (DoTreeView *view,
               const gchar *text);
static void     do_tree_view_search_refresh_timeout (DoTreeView *view);
static gboolean do_tree_view_search_flush_timeout (DoTreeView *view);
static gboolean do_tree_view_search_clear (DoTreeView *view);
static void    do_tree_view_search_backspace (DoTreeView *view);
static gboolean do_tree_view_search_find_text (DoTreeView *view, const gchar *text);
static gboolean do_tree_view_search_equal_func (GtkTreeModel *model,
				 gint          column,
				 const gchar  *key,
				 GtkTreeIter  *iter,
				 gpointer      search_data);
static void do_tree_view_columns_changed_cb (GtkWidget *widget, gpointer data);

static void do_tree_view_search_inline_cell_data_func (GtkTreeViewColumn *tree_column,
               GtkCellRenderer *cell, GtkTreeModel *tree_model,
               GtkTreeIter *iter,
               gpointer user_data);
static gchar* do_tree_view_get_selected_markup (GtkCellRenderer *cell,
               GtkWidget *view,
               gchar *text,
               guint selected_len);
static void do_tree_view_row_cursor_redraw (DoTreeView *view);

G_DEFINE_TYPE (DoTreeView, do_tree_view, GTK_TYPE_TREE_VIEW);


static void
do_tree_view_init (DoTreeView *do_tree_view)
{
	do_tree_view->priv = G_TYPE_INSTANCE_GET_PRIVATE (do_tree_view, DO_TYPE_TREE_VIEW, DoTreeViewPrivate);

	/* TODO: Add initialization code here */
}

GtkWidget*
do_tree_view_new (const gchar *name)
{
	return g_object_new (DO_TYPE_TREE_VIEW,
                          "name", name,
	/* TODO: Add initialization code here */
	                     NULL);
}

static GObject*
do_tree_view_constructor (GType type,
                       guint n_construct_properties,
                       GObjectConstructParam *construct_params)
{
    GObject *object;

    object = G_OBJECT_CLASS (do_tree_view_parent_class)->constructor
        (type, n_construct_properties, construct_params);

	DoTreeViewPrivate *priv = DO_TREE_VIEW (object)->priv;
	/* TODO: Add initialization code here */

	priv->widths = g_hash_table_new (g_str_hash, g_str_equal);

	return object;
}

static void
do_tree_view_finalize (GObject *object)
{
	DoTreeViewPrivate *priv = DO_TREE_VIEW (object)->priv;
	/* TODO: Add deinitalization code here */
    if ( priv->search_destroy && priv->search_user_data )
    {
        priv->search_destroy (priv->search_user_data);
        priv->search_user_data = NULL;
    }
	if ( priv->setting_save_timeout != 0 )
        g_source_remove (priv->setting_save_timeout);

    if ( priv->search_flush_timeout )
        g_source_remove(priv->search_flush_timeout);

    g_free (priv->name);

    g_free (priv->search_text);


	G_OBJECT_CLASS (do_tree_view_parent_class)->finalize (object);
}

static void
do_tree_view_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	g_return_if_fail (DO_IS_TREE_VIEW (object));
	DoTreeViewPrivate *priv = DO_TREE_VIEW (object)->priv;

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
do_tree_view_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	g_return_if_fail (DO_IS_TREE_VIEW (object));
	DoTreeViewPrivate *priv = DO_TREE_VIEW (object)->priv;

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
do_tree_view_class_init (DoTreeViewClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (DoTreeViewPrivate));

	object_class->constructor = do_tree_view_constructor;
	object_class->finalize = do_tree_view_finalize;
	object_class->set_property = do_tree_view_set_property;
	object_class->get_property = do_tree_view_get_property;

	g_object_class_install_property (object_class,
	                                 PROP_NAME,
	                                 g_param_spec_string ("name",
	                                                      "name",
	                                                      "name",
	                                                      "treeview",
	                                                      G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

}
static void
do_tree_view_signals_connect (DoTreeView *tree_view)
{
	DoTreeViewPrivate *priv = tree_view->priv;
    if ( !priv->search_inline )
    {
        priv->search_inline = TRUE;
        g_signal_connect (G_OBJECT (tree_view), "key-press-event",
                          G_CALLBACK (do_tree_view_key_press_event_cb), NULL);
        g_signal_connect (G_OBJECT (tree_view), "button-press-event",
                          G_CALLBACK (do_tree_view_button_press_event_cb), NULL);

    }
}
void
do_tree_view_set_search_func (DoTreeView             *tree_view,
								DoTreeViewSearchFunc  search_func,
								gpointer              search_user_data,
								GDestroyNotify        search_destroy)
{
    g_return_if_fail (GTK_IS_TREE_VIEW (tree_view));
    g_return_if_fail (search_func != NULL);

	DoTreeViewPrivate *priv = tree_view->priv;

    if ( priv->search_destroy )
         priv->search_destroy (priv->search_user_data);

    priv->search_func = search_func;
	priv->search_user_data = search_user_data;
    priv->search_destroy = search_destroy;

    do_tree_view_signals_connect (tree_view);

}

static gboolean
do_tree_view_cols_sync_width (DoTreeView *view)
{
	DoTreeViewPrivate *priv = view->priv;
    GList *cols, *col;
    cols = gtk_tree_view_get_columns (GTK_TREE_VIEW (view));
    gchar *name, *sname;
    gint width;

    for ( col = cols; col; col=col->next )
    {
        width = gtk_tree_view_column_get_width (col->data);
        name = (char*)g_object_get_data (G_OBJECT (col->data), "name");
        g_hash_table_replace (priv->widths, (gpointer)name, GINT_TO_POINTER (width));

        sname = g_strdup_printf ("column-%s-width", name);
        do_settings_set_int (priv->name, sname, width);
        g_free (sname);
    }

    priv->setting_save_timeout = 0;
	return FALSE;
}

static gboolean
do_tree_view_cols_sync_fixed_width (GtkTreeViewColumn *col, GParamSpec *pspec, DoTreeView *view)
{
	DoTreeViewPrivate *priv = view->priv;
	gchar *name;
	gint width;

	name = g_object_get_data (G_OBJECT (col), "name");

    width = GPOINTER_TO_INT (g_hash_table_lookup (priv->widths, name));


    if ( !G_UNLIKELY (priv->setting_save_timeout) && gtk_tree_view_column_get_width (col) != width ) {
        priv->setting_save_timeout = g_timeout_add (500,
               (GSourceFunc)do_tree_view_cols_sync_width, view);
    }

	return FALSE;
}

GtkTreeViewColumn*
do_tree_view_column_new (DoTreeView *view, const gchar *name, const gchar *title, const gchar *default_text)
{
	GtkTreeViewColumn *col;
	DoTreeViewPrivate *priv = view->priv;
    PangoContext *context;
    PangoLayout *layout;
    context = gtk_widget_create_pango_context (GTK_WIDGET (view));
    layout = pango_layout_new (context);
    col = gtk_tree_view_column_new();

    if ( default_text ) {
        gchar *sname;
        int width,height;

        sname = g_strdup_printf ("column-%s-width", name);
        width = do_settings_get_int (priv->name, sname, 0);
        g_free (sname);
        if ( !width )
        {
            pango_layout_set_text (layout, default_text, -1);
            pango_layout_get_pixel_size (layout, &width, &height);
            width+=16;
        }

        gtk_tree_view_column_set_resizable (col, TRUE);
        gtk_tree_view_column_set_sizing (col, GTK_TREE_VIEW_COLUMN_FIXED);
        gtk_tree_view_column_set_fixed_width (col, width);

        if ( g_hash_table_lookup (priv->widths, name) )
        {
            g_log (APPLICATION_NAME, G_LOG_LEVEL_WARNING, "Повторяющиеся имя колонки '%s'", name);
        }
        else
        {
            g_hash_table_insert (priv->widths, (gpointer)name, GINT_TO_POINTER (width));
            g_signal_connect (col, "notify::width",
                G_CALLBACK (do_tree_view_cols_sync_fixed_width), view);
        }

    }
    else {
        g_object_set (view, "fixed-height-mode", FALSE, NULL);
    }
    //g_object_set (view, "fixed-height-mode", FALSE, NULL);
    g_object_set_data (G_OBJECT (col), "name", (gpointer)name);
    //gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);
    if ( title )
        gtk_tree_view_column_set_title (col, title);
	return col;
}
static gboolean
do_tree_view_search_key_cancels_search (guint keyval)
{
  return keyval == GDK_KEY_Escape
      || keyval == GDK_KEY_Tab
      || keyval == GDK_KEY_KP_Tab
      || keyval == GDK_KEY_ISO_Left_Tab
      || keyval == GDK_KEY_Up
      || keyval == GDK_KEY_Down
      || keyval == GDK_KEY_Page_Up
      || keyval == GDK_KEY_Page_Down
      ;
}

static gboolean
do_tree_view_key_press_event_cb (GtkWidget *widget,
               GdkEventKey  *event,
               gpointer   user_data)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW (widget)->priv;


    if ( gtk_widget_has_focus (widget)
      && gtk_tree_view_get_enable_search (GTK_TREE_VIEW (widget))
      && !do_tree_view_search_key_cancels_search (event->keyval))
    {
        gchar *new_text;
        new_text = do_tree_view_search_get_text (DO_TREE_VIEW (widget), event->string, event->length);

        if ( new_text ) {
            do_tree_view_search_add_text (DO_TREE_VIEW (widget), new_text);
            g_free (new_text);
            return TRUE;
        }

    }
    if ( do_tree_view_search_key_cancels_search (event->keyval) )
        do_tree_view_search_clear (DO_TREE_VIEW (widget));
    if ( event->keyval == GDK_KEY_BackSpace && priv->search_text )
    {
        do_tree_view_search_backspace (DO_TREE_VIEW (widget));
        return TRUE;
    }
    return FALSE;
}
static const gunichar ru_layout[66*2] = {
1081,1094,1091,1082,1077,1085,1075,1096,1097,1079,1093,1098,1092,1099,1074,1072,
1087,1088,1086,1083,1076,1078,1101,1103,1095,1089,1084,1080,1090,1100,1073,1102,
1105,
1049,1062,1059,1050,1045,1053,1043,1064,1065,1047,1061,1066,1060,1067,1042,1040,
1055,1056,1054,1051,1044,1046,1069,1071,1063,1057,1052,1048,1058,1068,1041,1071,
1025,
 113, 119, 101, 114, 116, 121, 117, 105, 111, 112,  91,  93,  97, 115, 100, 102,
 103, 104, 106, 107, 108,  59,  39, 122, 120,  99, 118,  98, 110, 109,  44,  46,
  96,
  81,  87,  69,  82,  84,  89,  85,  73,  79,  80, 123, 125,  65,  83,  68,  70,
  71,  72,  74,  75,  76,  58,  34,  90,  88,  67,  86,  66,  78,  77,  60,  62,
 126};

static const gunichar add_simbol[4] = {45,47,37,8470};
#define g_unichar_isadd_simbol(out) (out == add_simbol[0] || out == add_simbol[1] || \
                                     out == add_simbol[2] || out == add_simbol[3] )

static gchar*
do_tree_view_search_get_text (DoTreeView *view, const gchar *string, gint length)
{
    DoTreeViewPrivate *priv = view->priv;

    if ( !length )
        return NULL;

    int j;
    gchar *buf = g_locale_to_utf8 (string, -1, NULL, NULL, NULL);
    if ( !buf )
        return NULL;

    gchar *result = g_malloc (sizeof (gunichar) * strlen (buf) + 1);
    gchar *p;
    gunichar out;
    gint nlen = 0;
    for (p = buf; p && *p !='\0' && (p - buf < length);
          p = (gchar*)g_utf8_next_char(p))
    {
        out = g_utf8_get_char(p);
        if ( g_unichar_isspace(out) && !priv->search_text )
        {
            g_free(result);
            g_free(buf);
            return NULL;
        }
        if ( priv->search_only_russian )
        {
            for ( j = 0; j < 66; j++ )
                if (ru_layout[j + 66] == out)
                {
                    out = ru_layout[j];
                    break;
                }
        }
        out = g_unichar_tolower (out);
        /*!!if ( !g_unichar_isalpha (out) && !g_unichar_isdigit (out) && (out != 32) && !g_unichar_isadd_simbol (out) )
        {
            g_free (result);
            g_free (buf);
            return NULL;
        }*/
        nlen += g_unichar_to_utf8(out, result + nlen);
    }
    if ( !nlen )
    {
        g_free(result);
        g_free(buf);
        return NULL;
    }
    result[nlen] = '\0';
    g_free (buf);
    return result;
}
static gboolean
do_tree_view_search_add_text (DoTreeView *view, const gchar *text)
{
	DoTreeViewPrivate *priv = view->priv;
	gchar *new_text;

    if ( priv->search_text )
        new_text = g_strdup_printf ("%s%s", priv->search_text, text);
    else
        new_text = g_strdup (text);
    if ( do_tree_view_search_find_text(view, new_text) )
    {
        g_free (priv->search_text);
        priv->search_text = new_text;
        do_tree_view_search_refresh_timeout (view);
        return TRUE;
    }
    if ( priv->search_flush_timeout )
        g_source_remove(priv->search_flush_timeout);
    priv->search_flush_timeout = 0;
    g_free (new_text);
    return FALSE;
}
static void
do_tree_view_search_refresh_timeout (DoTreeView *view)
{
	DoTreeViewPrivate *priv = view->priv;
    if ( priv->search_flush_timeout )
        g_source_remove (priv->search_flush_timeout);

    priv->search_flush_timeout =
        gdk_threads_add_timeout (DO_TREE_VIEW_SEARCH_TIMEOUT,
		   (GSourceFunc) do_tree_view_search_flush_timeout, view);
}
static gboolean
do_tree_view_search_flush_timeout (DoTreeView *view)
{
	DoTreeViewPrivate *priv = view->priv;
    do_tree_view_search_clear (view);
    priv->search_flush_timeout = 0;
    return FALSE;
}
static gboolean
do_tree_view_search_clear (DoTreeView *view)
{
	DoTreeViewPrivate *priv = view->priv;
	gboolean res;
	res = priv->search_text != NULL;
    g_free (priv->search_text);
    priv->search_text = NULL;
    //MESSAGE("clear %d", res);
    if ( res )
    {
        GtkAllocation allocation;
        gtk_widget_get_allocation (GTK_WIDGET (view), &allocation);
        gtk_widget_queue_draw_area (GTK_WIDGET (view), 0,0,
                                    allocation.width,
                                    allocation.height);
        gtk_widget_queue_draw (GTK_WIDGET (view));
        //MESSAGE("clear");

        if ( priv->search_flush_timeout )
        {
            g_source_remove (priv->search_flush_timeout);
            priv->search_flush_timeout = 0;
        }
    }
    return res;
}
static gboolean
do_tree_view_search_find_text (DoTreeView *view, const gchar *text)
{
	DoTreeViewPrivate *priv = view->priv;
	GtkTreePath *path;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gint search_column;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (view));
	search_column = gtk_tree_view_get_search_column (GTK_TREE_VIEW (view));
	path = NULL;

	if ( priv->search_func ) {
        path = priv->search_func (model,
                                  search_column,
                                  text,
                                  priv->search_user_data);
        if ( path )
            gtk_tree_model_get_iter (model, &iter, path);
    }
    else
    {
        if ( search_column < 0 )
            return FALSE;

        gtk_tree_model_get_iter_first (model, &iter);
        do
        {
            if ( !do_tree_view_search_equal_func (model, search_column, text, &iter, NULL) )
            {
                path = gtk_tree_model_get_path (model, &iter);
                break;
            }

        } while ( gtk_tree_model_iter_next (model, &iter));


    }
    if ( path )
    {
        //!!gtk_tree_selection_select_iter (gtk_tree_view_get_selection (GTK_TREE_VIEW (view)), &iter);
        gtk_tree_view_set_cursor (GTK_TREE_VIEW (view), path, NULL, FALSE);
        gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (view), path, priv->search_inline_column,
                TRUE, 0.5, 0.5);
        gtk_tree_path_free (path);
        return TRUE;
    }
    return FALSE;
}

static gboolean
do_tree_view_search_equal_func (GtkTreeModel *model,
				 gint          column,
				 const gchar  *key,
				 GtkTreeIter  *iter,
				 gpointer      search_data)
{
  gboolean retval = TRUE;
  const gchar *str;
  gchar *normalized_string;
  gchar *normalized_key;
  gchar *case_normalized_string = NULL;
  gchar *case_normalized_key = NULL;
  GValue value = G_VALUE_INIT;
  GValue transformed = G_VALUE_INIT;

  gtk_tree_model_get_value (model, iter, column, &value);

  g_value_init (&transformed, G_TYPE_STRING);

  if (!g_value_transform (&value, &transformed))
    {
      g_value_unset (&value);
      return TRUE;
    }

  g_value_unset (&value);

  str = g_value_get_string (&transformed);
  if (!str)
    {
      g_value_unset (&transformed);
      return TRUE;
    }

  normalized_string = g_utf8_normalize (str, -1, G_NORMALIZE_ALL);
  normalized_key = g_utf8_normalize (key, -1, G_NORMALIZE_ALL);

  if (normalized_string && normalized_key)
    {
      case_normalized_string = g_utf8_casefold (normalized_string, -1);
      case_normalized_key = g_utf8_casefold (normalized_key, -1);

      if (strncmp (case_normalized_key, case_normalized_string, strlen (case_normalized_key)) == 0)
        retval = FALSE;
    }

  g_value_unset (&transformed);
  g_free (normalized_key);
  g_free (normalized_string);
  g_free (case_normalized_key);
  g_free (case_normalized_string);

  return retval;
}
void
do_tree_view_set_search_inline_column (DoTreeView *view, GtkTreeViewColumn *column, GtkCellRenderer *render)
{
	DoTreeViewPrivate *priv = view->priv;

    if ( priv->search_inline_column )
        gtk_tree_view_column_set_cell_data_func (priv->search_inline_column, NULL, NULL, NULL, NULL);
    priv->search_inline_column = column;
    gtk_tree_view_column_set_cell_data_func (priv->search_inline_column, render, do_tree_view_search_inline_cell_data_func, view, NULL);

    do_tree_view_signals_connect (view);
}
GtkTreeViewColumn*
do_tree_view_get_search_inline_column (DoTreeView *view)
{
	DoTreeViewPrivate *priv = view->priv;
    return priv->search_inline_column;
}
static void
do_tree_view_search_inline_cell_data_func (GtkTreeViewColumn *tree_column,
               GtkCellRenderer *cell, GtkTreeModel *tree_model,
               GtkTreeIter *iter,
               gpointer user_data)
{
    DoTreeViewPrivate *priv = DO_TREE_VIEW (user_data)->priv;
    GtkTreePath *path, *path1;
    path = gtk_tree_model_get_path (tree_model, iter);
    gtk_tree_view_get_cursor (GTK_TREE_VIEW (user_data), &path1, NULL);
    if ( path1 && path && priv->search_text && !gtk_tree_path_compare(path, path1) )
    {
        gchar *text, *markup, *p;
        gint len;
        g_object_get (G_OBJECT (cell), "text", &text, NULL);

        for ( len = 0, p = priv->search_text; p && *p != '\0'; p = (gchar*)g_utf8_next_char(p), len++ );

        markup = do_tree_view_get_selected_markup (cell, GTK_WIDGET(user_data), text, len);
        g_object_set (G_OBJECT (cell), "markup", markup, NULL);
    }

}
static gchar*
do_tree_view_get_selected_markup (GtkCellRenderer *cell, GtkWidget *view, gchar *text, guint selected_len)
{
#if GTK_MAJOR_VERSION > 2
    GdkRGBA color_bg, color_fg;
    GtkStyleContext *context;
    context = gtk_widget_get_style_context (view);
    gtk_style_context_get_background_color (GTK_STYLE_CONTEXT (context), GTK_STATE_FLAG_NORMAL,&color_bg);
    gtk_style_context_get_color (GTK_STYLE_CONTEXT (context), GTK_STATE_FLAG_NORMAL, &color_fg);

    gint color_bg_red,color_bg_green,color_bg_blue;
    gint color_fg_red,color_fg_green,color_fg_blue;
    color_bg_red=color_bg.red*255;color_bg_green=color_bg.green*255;color_bg_blue=color_bg.blue*255;
    color_fg_red=color_fg.red*255;color_fg_green=color_fg.green*255;color_fg_blue=color_fg.blue*255;

#else
    GtkStyle *style = gtk_widget_get_style(GTK_WIDGET(view));
#endif
    int i;
    gchar *selected_text = g_strdup(text);
    gchar *tail;

    for (i = 0, tail = text; tail && *tail !='\0' && i < selected_len;
        tail = (gchar*)g_utf8_next_char(tail), i++);

    selected_text[tail - text] = '\0';
    gchar *markup;
#if GTK_MAJOR_VERSION == 2
    if ( !style )
        markup = g_markup_printf_escaped("<span background=\"white\" foreground=\"black\">%s</span>%s",
                             selected_text, tail ? tail : "");
    else
#endif
#if GTK_MAJOR_VERSION >2
        markup = g_markup_printf_escaped("<span background=\"#%2.2hX%2.2hX%2.2hX\" foreground=\"#%2.2hX%2.2hX%2.2hX\">%s</span>%s",
                             color_bg_red,color_bg_green,color_bg_blue,
                             color_fg_red,color_fg_green,color_fg_blue,
                             selected_text, tail ? tail : "");
#else
        markup = g_markup_printf_escaped("<span background=\"#%2.2hX%2.2hX%2.2hX\" foreground=\"#%2.2hX%2.2hX%2.2hX\">%s</span>%s",
                             style->base[GTK_STATE_NORMAL].red,style->base[GTK_STATE_NORMAL].green,style->base[GTK_STATE_NORMAL].blue,
                             style->fg[GTK_STATE_NORMAL].red,style->fg[GTK_STATE_NORMAL].green,style->fg[GTK_STATE_NORMAL].blue,
                             selected_text, tail ? tail : "");
#endif
    g_free(selected_text);
    return markup;

}

static gboolean
do_tree_view_button_press_event_cb (GtkWidget *widget,
               GdkEvent  *event,
               gpointer   user_data)
{
	//DoTreeViewPrivate *priv = tree_view->priv;
	do_tree_view_search_clear (DO_TREE_VIEW (widget));
	return FALSE;

}
static void
do_tree_view_search_backspace (DoTreeView *view)
{
	DoTreeViewPrivate *priv = view->priv;
    if ( priv->search_text ) {
        gint len;
        gchar *p;
        for ( len = 0, p = priv->search_text; p && *p != '\0'; p = (gchar*)g_utf8_next_char(p), len++ );
        len--;
        for ( p = priv->search_text; len; len--, p = (gchar*)g_utf8_next_char(p) );
        *p = '\0';
        if (p == priv->search_text)
            do_tree_view_search_clear (view);
        else
        {
            do_tree_view_search_refresh_timeout (view);
            do_tree_view_row_cursor_redraw (DO_TREE_VIEW (view));
        }
    }
}
static void
do_tree_view_row_redraw (DoTreeView *view, GtkTreePath *path)
{
    GdkRectangle rect, visible_rect;
    gint dx, dy;
    gtk_tree_view_get_background_area (GTK_TREE_VIEW (view), path, NULL, &rect);
    gtk_tree_view_convert_bin_window_to_widget_coords(GTK_TREE_VIEW (view), rect.x, rect.y, &dx, &dy);
    rect.x += dx;
    rect.y += dy;
    gtk_tree_view_get_visible_rect(GTK_TREE_VIEW (view), &visible_rect);
    rect.width = visible_rect.width;
    gtk_widget_queue_draw_area(GTK_WIDGET (view), rect.x, rect.y, rect.width, rect.height);
    gtk_widget_queue_draw (GTK_WIDGET (view));
}
static void
do_tree_view_row_cursor_redraw (DoTreeView *view)
{
    GtkTreePath *path;
    gtk_tree_view_get_cursor (GTK_TREE_VIEW (view), &path, NULL);
    if ( path ) {
        do_tree_view_row_redraw (view, path);
        gtk_widget_queue_draw (GTK_WIDGET (view));
        gtk_tree_path_free (path);
    }
}
static void
do_tree_view_columns_changed_cb (GtkWidget *widget, gpointer data)
{
    GList *cols, *col;
    cols = gtk_tree_view_get_columns (GTK_TREE_VIEW (widget));
    gchar *names = NULL, *name;
    DoTreeViewPrivate *priv = DO_TREE_VIEW(widget)->priv;

    if ( priv->n_column == g_list_length(cols) )
    {

        for ( col = cols; col; col=col->next ) {
            gchar *buf;
            name = (char*)g_object_get_data (G_OBJECT(col->data), "name");
            if ( names ) {
                buf = g_strdup_printf("%s,%s", names, name);
                g_free(names);
                names = buf;
            }
            else
                names = g_strdup(name);
        }
        do_settings_set_string (priv->name, "columns", names);
    }
    g_list_free (cols);
}
gint
do_tree_view_append_column (DoTreeView *tree_view, GtkTreeViewColumn *column)
{
    tree_view->priv->n_column +=1;
    if ( tree_view->priv->n_column == 1 )
        g_signal_connect (tree_view, "columns-changed",
                        G_CALLBACK (do_tree_view_columns_changed_cb), column);
    return gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), column);
}
gboolean
do_tree_view_grab_focus (DoTreeView *view)
{
    GtkTreePath *path = NULL;

    gtk_widget_grab_focus (GTK_WIDGET (view));
    gtk_tree_view_get_cursor (GTK_TREE_VIEW(view), &path, NULL);

    if ( !path )
        path = gtk_tree_path_new_first ();
    gtk_tree_view_set_cursor (GTK_TREE_VIEW(view), path, NULL, FALSE);
    gtk_tree_path_free (path);

    return TRUE;
}

