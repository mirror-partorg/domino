#include "do_utilx.h"
#include "do_view.h"
#include "do_validate.h"
#include "do_notebook.h"
#include "do_entry.h"
#include "hig.h"
#include <dolib.h>
#include "../misc/print.h"
#include "../misc/iconv_.h"

#include "domino.h"
#include <iconv.h>
#include <math.h>

#define PRINT_ROOT_PATH "Print"
#include <stdarg.h>

typedef void ( PopupFunc )( GtkWidget*, GdkEventButton* );

/* if the user clicked in an empty area of the list,
 * clear all the selections. */

const char *domino_unit()
{
    return "5";
}
void gtk_tree_view_row_redraw(GtkTreeView *view, GtkTreePath *path)
{
    GdkRectangle rect, visible_rect;
    gint dx, dy;
    gtk_tree_view_get_background_area(view, path, NULL, &rect);
    gtk_tree_view_convert_bin_window_to_widget_coords(view, rect.x, rect.y, &dx, &dy);
    rect.x += dx;
    rect.y += dy;
    gtk_tree_view_get_visible_rect(view, &visible_rect);
    rect.width = visible_rect.width;
    gtk_widget_queue_draw_area(GTK_WIDGET (view), rect.x, rect.y, rect.width, rect.height);
    gtk_widget_queue_draw (GTK_WIDGET (view));
}
void gtk_tree_view_row_cursor_redraw(GtkTreeView *view)
{
    GtkTreePath *path;
    gtk_tree_view_get_cursor(view, &path, NULL);
    if (path) {
        gtk_tree_view_row_redraw(view, path);
        gtk_widget_queue_draw (GTK_WIDGET (view));
        gtk_tree_path_free(path);
    }
}
void gtk_tree_view_redraw(GtkTreeView *view)
{
    GtkAllocation allocation;
    gtk_widget_get_allocation(GTK_WIDGET(view), &allocation);
    gtk_widget_queue_draw_area (GTK_WIDGET(view), 0,0,
        allocation.width,
        allocation.height);
    gtk_widget_queue_draw (GTK_WIDGET (view));
}
void gtk_tree_view_column_redraw(GtkTreeView *view, GtkTreeViewColumn *column)
{
    GdkRectangle rect, visible_rect;
    gint dx, dy;
    gtk_tree_view_get_background_area(view, NULL, column, &rect);
    gtk_tree_view_convert_bin_window_to_widget_coords(view, rect.x, rect.y, &dx, &dy);
    rect.x += dx;
    rect.y += dy;
    gtk_tree_view_get_visible_rect(view, &visible_rect);
    rect.height = visible_rect.height;
    gtk_widget_queue_draw_area(GTK_WIDGET (view), rect.x, rect.y, rect.width, rect.height);
    gtk_widget_queue_draw (GTK_WIDGET(view));
}
static gboolean
font_description_style_equal (const PangoFontDescription *a,
			      const PangoFontDescription *b)
{
  return (pango_font_description_get_weight (a) == pango_font_description_get_weight (b) &&
	  pango_font_description_get_style (a) == pango_font_description_get_style (b) &&
	  pango_font_description_get_stretch (a) == pango_font_description_get_stretch (b) &&
	  pango_font_description_get_variant (a) == pango_font_description_get_variant (b));
}

void gtk_font_button_get_font_desc(GtkFontButton *font_button, gchar **font_family, gchar **font_style, gint *font_size)
{
    PangoFontDescription *desc;
    const gchar *family;
    gchar *style;
    gchar *family_style;

    desc = pango_font_description_from_string (gtk_font_button_get_font_name(font_button));
    family = pango_font_description_get_family (desc);
    *font_family = g_strdup(family);

    style = NULL;

    PangoFontFamily **families;
    PangoFontFace **faces;
    gint n_families, n_faces, i;

    n_families = 0;
    families = NULL;
    pango_context_list_families (gtk_widget_get_pango_context (GTK_WIDGET (font_button)),
                               &families, &n_families);
    n_faces = 0;
    faces = NULL;
    for (i = 0; i < n_families; i++)    {
        const gchar *name = pango_font_family_get_name (families[i]);

        if (!g_ascii_strcasecmp (name, family))  {
            pango_font_family_list_faces (families[i], &faces, &n_faces);
            break;
        }
    }
    g_free (families);

    for (i = 0; i < n_faces; i++) {
        PangoFontDescription *tmp_desc = pango_font_face_describe (faces[i]);

        if (font_description_style_equal (tmp_desc, desc)) {
            style = g_strdup (pango_font_face_get_face_name (faces[i]));
            pango_font_description_free (tmp_desc);
            break;
        }
        else
            pango_font_description_free (tmp_desc);
    }
    g_free (faces);

    if (style == NULL || !g_ascii_strcasecmp (style, "Regular"))
        family_style = g_strdup ("");
    else
        family_style = g_strdup(style);

    *font_style  = family_style;
    g_free (style);

    gchar *size = g_strdup_printf ("%g",
                            pango_font_description_get_size (desc) / (double)PANGO_SCALE);

    *font_size = atoi(size);
    g_free (size);

    pango_font_description_free (desc);
}
void do_print_dialog_show(GtkWidget *parent)
{
#if (0)
    GtkWidget *widget;

    GtkWidget *win = NULL;
    GtkWidget *win1 = NULL;
    if (parent)
        win1 = gtk_widget_get_toplevel(parent);

    if (win1)  {
        if (!GTK_WIDGET_VISIBLE(win1)) {
            if (gtk_window_get_transient_for (GTK_WINDOW(win1)))
                win = GTK_WIDGET(gtk_window_get_transient_for (GTK_WINDOW(win1)));
        }else
            win = win1;
    }
    if (!GTK_IS_WINDOW(win))
        win = NULL;
    widget = gtk_print_unix_dialog_new("d", GTK_WINDOW(win));
    gtk_dialog_run(GTK_DIALOG(widget));
    gtk_widget_destroy (widget);

#endif
}
/*
GtkWidget *do_message_dialog_create(GtkWidget *parent, GtkMessageType type, GtkButtonsType buttons, const gchar *msg)
{
    GtkWidget *widget;

    GtkWidget *win = NULL;
    GtkWidget *win1 = NULL;
    if (parent)
        win1 = gtk_widget_get_toplevel(parent);

    if (win1)  {
        if (!gtk_widget_get_visible(win1)) {
            if (gtk_window_get_transient_for (GTK_WINDOW(win1)))
                win = GTK_WIDGET(gtk_window_get_transient_for (GTK_WINDOW(win1)));
        }else
            win = win1;
    }
    if (!GTK_IS_WINDOW(win))
        win = NULL;
    widget = gtk_message_dialog_new (GTK_WINDOW(win), GTK_DIALOG_MODAL, type, buttons, NULL);

	gtk_window_set_icon_name (GTK_WINDOW(widget), DO_STOCK_DOMINO_APPS);
	gtk_window_set_position (GTK_WINDOW(widget), GTK_WIN_POS_CENTER);

    if ( msg )
        gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (widget), msg);
    return widget;
}
typedef struct _MessageForDialog MessageForDialog;

struct _MessageForDialog {
    GtkWidget *parent;
    GtkMessageType type;
    GtkButtonsType buttons;
    gchar *message;
    gboolean ask_exit;
};
static GList *message_dialog_list = NULL;

gint do_message_dialog_show(GtkWidget *parent, GtkMessageType type, GtkButtonsType buttons, const gchar *markup_format, ...)
{
    GtkWidget *widget;
    va_list args;
    gchar *msg = NULL;
    if (markup_format)
    {
        va_start (args, markup_format);
        msg = g_markup_vprintf_escaped (markup_format, args);
        va_end (args);
    }
    message_dialog_list = g_list_append
    widget = do_message_dialog_create(parent, type, buttons, msg);
    g_free(msg);

    gint retval = gtk_dialog_run(GTK_DIALOG(widget));
    gtk_widget_destroy (widget);
    return retval;
}
void do_message_window_show(GtkWidget *parent, GtkMessageType type, GtkButtonsType buttons, const gchar *markup_format, ...)
{
    GtkWidget *widget;
    va_list args;
    gchar *msg = NULL;
    if (markup_format)
    {
        va_start (args, markup_format);
        msg = g_markup_vprintf_escaped (markup_format, args);
        va_end (args);
    }
    widget = do_message_dialog_create(parent, type, buttons, msg);
    g_signal_connect_swapped (widget,
                             "response",
                             G_CALLBACK (gtk_widget_destroy),
                             widget);
    g_free(msg);
    gtk_widget_show(widget);
}
*/

void gtk_widget_get_font_desc(GtkWidget *widget, const gchar *font_desc,
                                       gchar **font_family, gchar **font_style, gint *font_size)
{
    PangoFontDescription *desc;
    const gchar *family;
    gchar *style;
    gchar *family_style;

    desc = pango_font_description_from_string (font_desc);
    family = pango_font_description_get_family (desc);
    *font_family = g_strdup(family);

    style = NULL;

    PangoFontFamily **families;
    PangoFontFace **faces;
    gint n_families, n_faces, i;

    n_families = 0;
    families = NULL;
    pango_context_list_families (gtk_widget_get_pango_context (GTK_WIDGET (widget)),
                               &families, &n_families);
    n_faces = 0;
    faces = NULL;
    for (i = 0; i < n_families; i++)    {
        const gchar *name = pango_font_family_get_name (families[i]);

        if (!g_ascii_strcasecmp (name, family))  {
            pango_font_family_list_faces (families[i], &faces, &n_faces);
            break;
        }
    }
    g_free (families);

    for (i = 0; i < n_faces; i++) {
        PangoFontDescription *tmp_desc = pango_font_face_describe (faces[i]);

        if (font_description_style_equal (tmp_desc, desc)) {
            style = g_strdup (pango_font_face_get_face_name (faces[i]));
            pango_font_description_free (tmp_desc);
            break;
        }
        else
            pango_font_description_free (tmp_desc);
    }
    g_free (faces);

    if (style == NULL || !g_ascii_strcasecmp (style, "Regular"))
        family_style = g_strdup ("");
    else
        family_style = g_strdup(style);

    *font_style  = family_style;
    g_free (style);

    gchar *size = g_strdup_printf ("%g",
                            pango_font_description_get_size (desc) / (double)PANGO_SCALE);

    *font_size = atoi(size);
    g_free (size);

    pango_font_description_free (desc);
}
void gtk_dialog_accept(GtkDialog *dialog)
{
    gtk_dialog_response(dialog, GTK_RESPONSE_ACCEPT);
}

typedef enum {
    PROP_WIDGET_WINDOW_SIZE,
    PROP_WIDGET_WINDOW_STATE,
    PROP_WIDGET_WINDOW_POSITION,
    PROP_WIDGET_PANED_POSITION,
    PROP_WIDGET_FIXED_WIDTH,
    PROP_WIDGET_UNDEFINE
}prop_widget_t;

static prop_widget_t get_prop(const gchar *name)
{
    if (!strcmp(name, "window-size"))
        return PROP_WIDGET_WINDOW_SIZE;
    if (!strcmp(name, "window-state"))
        return PROP_WIDGET_WINDOW_STATE;
    if (!strcmp(name, "window-position"))
        return PROP_WIDGET_WINDOW_POSITION;
    if (!strcmp(name, "paned-position"))
        return PROP_WIDGET_PANED_POSITION;
    if (!strcmp(name, "fixed-width"))
        return PROP_WIDGET_FIXED_WIDTH;

    return PROP_WIDGET_UNDEFINE;
}
typedef enum
{
    WINDOW_NORMAL,
    WINDOW_MINIMIZED,
    WINDOW_MAXIMIZED,
    WINDOW_FULLSCREEN,
} window_state_t;

static window_state_t get_window_state(const gchar *name)
{
    if (!strcmp(name, "Normal"))
        return WINDOW_NORMAL;
    if (!strcmp(name, "Minimized"))
        return WINDOW_MINIMIZED;
    if (!strcmp(name, "Maximized"))
        return WINDOW_MAXIMIZED;
    if (!strcmp(name, "Fullscreen"))
        return WINDOW_FULLSCREEN;

    return WINDOW_NORMAL;
}
static void get_workarea_geometry(GtkWindow *window, gint *width, gint *height)
{
#if GTK_CHECK_VERSION(3,22,0)
    GdkRectangle geom;
    GdkDisplay *display = gdk_window_get_display(gtk_widget_get_window(GTK_WIDGET(window)));
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    gdk_monitor_get_geometry(monitor, &geom);

    *width = geom.width;
    *height = geom.height;
#else
    GdkScreen *screen = gtk_window_get_screen (window);
    *width = gdk_screen_get_width(screen);
    *height = gdk_screen_get_height(screen);
#endif
}
static gboolean get_window_size(GtkWindow *window, const gchar *value, gint *width, gint *height)
{
    gchar *p;
    if (!strcmp(value, "workarea"))
    {
        get_workarea_geometry(window, width, height);
        return TRUE;
    }
    *width = strtol(value, &p, 10);
    if (*width <= 0)
        return FALSE;
    if (*p == 'x')
        *height = strtol(p + 1, &p, 10);
    if (*height <= 0)
    {
        return FALSE;
    }

    if (*p == '%')
    {
        gint s_width,s_height;
        get_workarea_geometry(window, &s_width, &s_height);

        *height = s_height*(*width/100.);
        *width  = s_width*(*width/100.);
    }
    return TRUE;
}
static gboolean get_window_position(GtkWindow *window, const gchar *value, gint *left, gint *top, GtkWindowPosition *position)
{
    //GdkScreen *screen = gtk_window_get_screen (window);
    gchar *p;
    *position = GTK_WIN_POS_NONE;
    if (!strcmp(value, "center"))
    {
        *position = GTK_WIN_POS_CENTER;
        return TRUE;
    }
    if (!strcmp(value, "center-on-parent"))
    {
        *position = GTK_WIN_POS_CENTER_ON_PARENT;
        return TRUE;
    }
    *top = 0;
    *left = strtol(value, &p, 10);
    if (*left < 0)
    {
        return FALSE;
    }

    if (*p == 'x') {
        *top = strtol(p + 1, &p, 10);
        if (*top < 0)
            return FALSE;
    }
    if (*p == '%')
    {
        gint s_width,s_height;
        get_workarea_geometry(window, &s_width, &s_height);
        *top = s_width*(*left/100.);
        *left  = s_height*(*left/100.);
    }
    return TRUE;
}

void do_widget_set_property(GObject *widget, const gchar *name, const gchar *value)
{
    prop_widget_t id;
    id = get_prop(name);
    switch (id) {
        case PROP_WIDGET_WINDOW_SIZE:
            if (GTK_IS_WINDOW(widget)) {
                gint width, height;
                if (get_window_size(GTK_WINDOW(widget), value, &width, &height)) {
                    //gtk_window_set_default_size (GTK_WINDOW(widget), width, height);
                    GtkAllocation allocation;
                    gtk_widget_get_allocation(GTK_WIDGET(widget), &allocation);
                    allocation.width = width;
                    allocation.height = height;
                    gtk_widget_set_allocation(GTK_WIDGET(widget), &allocation);
                }
            }
            break;
        case PROP_WIDGET_WINDOW_POSITION:
            if (GTK_IS_WINDOW(widget)) {
                gint left, top;
                GtkWindowPosition position;
                if (get_window_position(GTK_WINDOW(widget), value, &left, &top, &position))
                    switch (position) {
                        case GTK_WIN_POS_NONE:
                            gtk_window_move(GTK_WINDOW(widget), left, top);
                            break;
                        default:
                            gtk_window_set_position(GTK_WINDOW(widget), position);
                            break;
                    }

            }
            break;
        case PROP_WIDGET_WINDOW_STATE:
            if (GTK_IS_WINDOW(widget)) {
                switch (get_window_state(value))
                {
                    case WINDOW_MINIMIZED:
                        //!!gtk_window_iconify (GTK_WINDOW (widget));
                        break;
                    case WINDOW_MAXIMIZED:
                        gtk_window_maximize (GTK_WINDOW (widget));
                        break;
                    case WINDOW_FULLSCREEN:
                        //!!gtk_window_fullscreen (GTK_WINDOW (widget));
                        break;
                    default:
                        ;/* Do nothing. */
                }
            }
            break;
        case PROP_WIDGET_PANED_POSITION:
            if (GTK_IS_PANED(widget)) {
                gint pos = atoi(value);
                if (pos)
                    gtk_paned_set_position(GTK_PANED(widget), pos);
            }
            break;
        case PROP_WIDGET_FIXED_WIDTH:
            if (GTK_IS_TREE_VIEW_COLUMN(widget)) {
                gint width = atoi(value);
                if ( width > 0 )
                    gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(widget), width);
                else
                    gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(widget), 60);
            }
            break;
        case PROP_WIDGET_UNDEFINE:
            break;
    }

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
                                     out == add_simbol[2] ||out == add_simbol[3])

gchar *search_get_text(const gchar *string, gint length, gint real_len)
{
    if (!length)
        return NULL;

    int j;
    gchar *buf = g_locale_to_utf8 (string, -1, NULL, NULL, NULL);
    if (!buf)
        return NULL;
    gchar *result = g_malloc(sizeof (gunichar) * strlen (buf) + 1);
    gchar *p;
    gunichar out;
    gint nlen = 0;
    for (p = buf; p && *p !='\0' && (p - buf < length);
          p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        if (g_unichar_isspace(out) && !real_len) {
            g_free(result);
            g_free(buf);
            return NULL;
        }
        for (j = 0; j < 66; j++)
            if (ru_layout[j + 66] == out) {
                out = ru_layout[j];
                break;
            }
        out = g_unichar_toupper(out);
        if (!g_unichar_isalpha(out) && !g_unichar_isdigit(out) && (out != 32) && !g_unichar_isadd_simbol(out)) {
            g_free(result);
            g_free(buf);
            return NULL;
        }
        nlen += g_unichar_to_utf8(out, result + nlen);
    }
    if (!nlen) {
        g_free(result);
        g_free(buf);
        return NULL;
    }
    result[nlen] = '\0';
    g_free(buf);
    return result;
//    buf = g_locale_from_utf8 (result, -1, NULL, NULL, NULL);
  //  g_free(result);
    //return buf;
}
gchar *filter_get_text_(const gchar *string)
{
    gint length = strlen(string);

    int j;
    gchar *buf = g_locale_to_utf8 (string, -1, NULL, NULL, NULL);
    if (!buf)
        return NULL;
    gchar *result = g_malloc(sizeof (gunichar) * strlen (buf) + 1);
    gchar *p;
    gunichar out;
    gint nlen = 0;
    for (p = buf; p && *p !='\0' && (p - buf < length);
          p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        for (j = 0; j < 66; j++)
            if (ru_layout[j + 66] == out) {
                out = ru_layout[j];
                break;
            }
        out = g_unichar_toupper(out);
        if (!g_unichar_isalpha(out) && !g_unichar_isdigit(out) && (out != 32) && !g_unichar_isadd_simbol(out)) {
            g_free(result);
            g_free(buf);
            return NULL;
        }
        nlen += g_unichar_to_utf8(out, result + nlen);
    }
    result[nlen] = '\0';
    g_free(buf);
    return result;
}
gchar *filter_get_text(const gchar *string)
{
    /*if (!length)
        return NULL;*/

    int j;
#ifdef _WIN32
    gchar *buf = g_strdup(string);
#else
    gsize  bytes_read=0;
    gsize  bytes_written=0;
    gchar *buf = g_locale_to_utf8 (string, -1, &bytes_read, &bytes_written, NULL);
#endif
    if (!buf)
        return NULL;
    gchar *result = g_malloc(sizeof (gunichar) * strlen (buf) + 1);
    gchar *p;
    gunichar out;
    gint nlen = 0;
    for (p = buf; p && *p !='\0' /*&& (p - buf < bytes_written)*/;
          p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        for (j = 0; j < 66; j++)
            if (ru_layout[j + 66] == out) {
                out = ru_layout[j];
                break;
            }
        out = g_unichar_toupper(out);
        if (!g_unichar_isalpha(out) && !g_unichar_isdigit(out) && (out != 32) && !g_unichar_isadd_simbol(out)) {
            g_free(result);
            g_free(buf);
            return NULL;
        }
        nlen += g_unichar_to_utf8(out, result + nlen);
    }
    if (!nlen) {
        g_free(result);
        g_free(buf);
        return NULL;
    }
    result[nlen] = '\0';
    g_free(buf);
    return result;
//    buf = g_locale_from_utf8 (result, -1, NULL, NULL, NULL);
  //  g_free(result);
    //return buf;
}

gchar *filter_get_text1(const gchar *string)
{
    /*if (!length)
        return NULL;*/

    int j;
#ifdef _WIN32
    gchar *buf = g_strdup(string);
#else
    gsize  bytes_read=0;
    gsize  bytes_written=0;
    gchar *buf = g_locale_to_utf8 (string, -1, &bytes_read, &bytes_written, NULL);
#endif
    if (!buf)
        return NULL;
    gchar *result = g_malloc(sizeof (gunichar) * strlen (buf) + 1);
    gchar *p;
    gunichar out;
    gint nlen = 0;
    for (p = buf; p && *p !='\0' /*&& (p - buf < bytes_written)*/;
          p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        for (j = 0; j < 66; j++)
            if (ru_layout[j + 66] == out) {
                out = ru_layout[j];
                break;
            }
        //out = g_unichar_toupper(out);
        if (!g_unichar_isalpha(out) && !g_unichar_isdigit(out) && (out != 32) && !g_unichar_isadd_simbol(out)) {
            g_free(result);
            g_free(buf);
            return NULL;
        }
        nlen += g_unichar_to_utf8(out, result + nlen);
    }
    if (!nlen) {
        g_free(result);
        g_free(buf);
        return NULL;
    }
    result[nlen] = '\0';
    g_free(buf);
    return result;
//    buf = g_locale_from_utf8 (result, -1, NULL, NULL, NULL);
  //  g_free(result);
    //return buf;
}

gchar *ru_to_en(const gchar *string, gint length)
{
    if (!length)
        return NULL;
    int j;
    gchar *result = g_malloc(sizeof (gunichar) * strlen(string) + 1);
    gchar *p;
    gunichar out;
    gint nlen = 0;
    for (p = (char*)string; p && *p !='\0' && (p - string < length);
          p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        for (j = 0; j < 66; j++)
            if (ru_layout[j] == out) {
                out = ru_layout[j + 66];
                break;
            }
        nlen += g_unichar_to_utf8(out, result + nlen);
    }
    if (!nlen) {
        g_free(result);
        return NULL;
    }
    result[nlen] = '\0';
    return result;
}
void do_window_save_setting(GtkWindow *window, const gchar *path)
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

#define RU_MONEY "₽"
gchar *do_money_format(double value, int abr)
{
    if ( value < 0.004 && value > -0.004 )
        return strdup("");

    char *buf, *ch, *retval, *ch1;
    if (abr)
        buf = g_strdup_printf("%.2f" RU_MONEY, value);
    else
        buf = g_strdup_printf("%.2f", value);
    gint len = strlen(buf);
    gint sign = (value > 0) ? 0 : 1;
    gint len_man = len - ((abr) ? strlen(RU_MONEY) : 0) - 3 - sign;
    retval = g_malloc0(len + 10 + (len_man % 3 + 1));
    ch = buf;
    ch1 = retval;
    while (*ch != '\0') {
        if (!((len_man - (ch - buf - sign)) % 3) &&
            (ch > buf + sign) &&
            (len_man - (ch - buf - sign) > 0))
            *ch1 = ' ',ch1++;
        *ch1 = *ch++, ch1++;
    }
    g_free(buf);
    return retval;
}
gchar *do_big_int_format(int value)
{
    if ( !value )
        return strdup("");

    char *buf, *ch, *retval, *ch1;
    buf = g_strdup_printf("%d", value);
    gint len = strlen(buf);
    gint sign = (value > 0) ? 0 : 1;
    gint len_man = len - sign;
    retval = g_malloc0(len + 1 + (len_man % 3 + 1));
    ch = buf;
    ch1 = retval;
    while (*ch != '\0') {
        if (!((len_man - (ch - buf - sign)) % 3) &&
            (ch > buf + sign) &&
            (len_man - (ch - buf - sign) > 0))
            *ch1 = ' ',ch1++;
        *ch1 = *ch++, ch1++;
    }
    g_free(buf);
    return retval;
}
static gchar *name_100[9] =
{"сто", "двести", "триста", "четыреста", "пятьсот",
"шестьсот", "семьсот", "восемьсот", "девятьсот"};
static gchar *name_11[10] =
{"десять", "одиннадцать", "двенадцать", "тринадцать", "четырнадцать",
"пятнадцать", "шестнадцать", "семнадцать", "восемнадцать", "девятнадцать"};
static gchar *name_10[8] =
{"двадцать", "тридцать", "сорок", "пятьдесят",
"шестьдесят", "семьдесят", "восемьдесят", "девяносто"};
static gchar *name_1[9] = {"один", "два", "три", "четыре", "пять", "шесть","семь", "восемь", "девять"};
static gchar *name_1j[9] = {"одна", "две", "три", "четыре", "пять", "шесть","семь", "восемь", "девять"};
static gchar *ind[5] = {"рубл", " тысяч", " миллион", " миллиард", " триллион"};
static gchar *suf[5][3] = {{"ь","я", "ей"},
                           {"а", "и", ""},
                           {"", "а", "ов"},
                           {"", "а", "ов"},
                           {"", "а", "ов"}};
static gchar *kopeck[] = {"копейка", "копейки", "копеек"};

gchar *triada(const gchar *in, int tn, int abr)
{
    if (!strncmp(in, "   ", 3) ||
        !strncmp(in, "000", 3) ||
        !strncmp(in, "  0", 3) )
        return NULL;

    GString *string = g_string_new(NULL);

    if (in[0] != ' ' && in[0] != '0')
        g_string_append(string, name_100[in[0] - '1']);
    if (in[1] == '1')
        g_string_append_printf(string, " %s", name_11[in[2] - '0']);
    else {
        if (in[1] != ' ' && in[1] != '0')
            g_string_append_printf(string, " %s", name_10[in[1] - '2']);
        if (in[2] != '0') {
            if (tn == 1)
                g_string_append_printf(string, " %s", name_1j[in[2] - '1']);
            else
                g_string_append_printf(string, " %s", name_1[in[2] - '1']);
        }
    }
    if (tn || abr) {
        g_string_append_printf(string, " %s", ind[tn]);
        switch (in[2]) {
            case '1':
                g_string_append(string, suf[tn][0]);
                break;
            case '2':
            case '3':
            case '4':
                g_string_append(string, suf[tn][1]);
                break;
            default :
                g_string_append(string, suf[tn][2]);
        }
    }
    return g_string_free(string, FALSE);
}
gchar *do_money_string(double value, int mantissa, int abr)
{
    gchar *buf, *ch, *val;
    gint i;
    GString *string = g_string_new(NULL);
    buf = g_strdup_printf("%18.2f", value);
    if (!mantissa)
        buf[strlen(buf)-2] = '\0';

    for (i = 4, ch = buf; i >= 0; i--, ch+=3) {
        val = triada(ch, i, abr);
        if (val) {
            if (string->len)
                g_string_append(string, " ");
            g_string_append(string, val);
        }
    }
    ch++;
    g_string_append_printf(string, " %s ", ch);
    if (mantissa) {
        switch (ch[1]) {
            case '1':
                g_string_append(string, kopeck[(ch[0] == '1') ? 2 : 0]);
                break;
            case '2':
            case '3':
            case '4':
                g_string_append(string, kopeck[(ch[0] == '1') ? 2 : 1]);
                break;
            default:
                g_string_append(string, kopeck[2]);
        }
    }
    g_free(buf);
    buf = g_string_free(string, FALSE);
    return buf;
}

gchar *do_money_buh(double value)
{
    if (!value)
        return strdup("");
    char buf[500], *ch;
    sprintf(buf, "%.2f", value);
    ch = buf + strlen(buf) - 1;
    while (ch > buf ) {
        if (*ch == ',' || *ch == '.') {
            *ch = '-';
            break;
        }
        ch--;
    }
    return g_strdup(buf);
}
gchar *do_rest_format(double value)
{
    if ( value > -0.0001 && value < 0.0001)
        return strdup("");
    char buf[500], *ch;
    sprintf(buf, "%.3f", value);
    ch = buf + strlen(buf) - 1;
    while (ch > buf && (*ch == '0' || *ch == ',' || *ch == '.')) {
        if (*ch == ',' || *ch == '.') {
            *ch = '\0';
            break;
        }
        *ch = '\0', ch--;
    }
    return g_strdup(buf);
}
gchar *do_rest_format_ei(double value, double coef, const char *ei)
{
    if ( value > -0.0001 && value < 0.0001)
        return strdup("");
    double val;
    if (( value < 0.9999)  && ( value > -0.9999))
        val = round(value*coef);
    else
        val = value;
    char buf[500], *ch;
    sprintf(buf, "%.3f", val);
    ch = buf + strlen(buf) - 1;
    while (ch > buf && (*ch == '0' || *ch == ',' || *ch == '.')) {
        if (*ch == ',' || *ch == '.') {
            *ch = '\0';
            break;
        }
        *ch = '\0', ch--;
    }
    if (( value < 0.9999)  && ( value > -0.9999)) {
        char *sad;
        sad = do_strdup_printf("%s %s",buf,ei);
        strcpy(buf,sad);
        do_free(sad);
    }
    return g_strdup(buf);
}
gchar *do_percent_format(double value)
{
    if (!value)
        return strdup("");
    char buf[500];//, *ch;
    sprintf(buf, "%.1f", value);
    /*ch = buf + strlen(buf) - 1;
    while (ch > buf && (*ch == '0' || *ch == ',' || *ch == '.')) {
        if (*ch == ',' || *ch == '.') {
            *ch = '\0';
            break;
        }
        *ch = '\0', ch--;
    }*/
    return g_strdup(buf);
}
gchar *do_first_alpha_upper(const gchar *name)
{
    gchar *buf = g_malloc(sizeof (gunichar) * strlen (name));
    gunichar  out;
    int nlen = 0, first_alpha = 1;
    gchar *p;
    for (p = (gchar*)name; p && *p !='\0'; p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        if (first_alpha && g_unichar_isalpha(out)) {
            out = g_unichar_toupper (out);
            first_alpha = 0;
        }
        nlen += g_unichar_to_utf8(out, buf + nlen);
    }
    buf[nlen] = '\0';
    return buf;
}

gchar *do_product_name_format(const gchar *name)
{
    if ( !name )
        return g_strdup("");
    gchar *buf = g_malloc(sizeof (gunichar) * strlen (name) + 10);
    gunichar  out;
    gboolean tolower = TRUE;
    int nlen = 0, first_alpha = 1;
    gchar *p;
    for (p = (gchar*)name; p && *p !='\0'; p = (gchar*)g_utf8_next_char(p)) {
	if ( *p == '\n' ) tolower = FALSE;
        out = g_utf8_get_char(p);
        if (first_alpha && g_unichar_isalpha(out))
            first_alpha = 0;
        else
            if ( tolower )
                out = g_unichar_tolower (out);
        nlen += g_unichar_to_utf8(out, buf + nlen);
    }
    buf[nlen] = '\0';
    return buf;
}
#ifdef USE_DEPRECATE_PRINT
gboolean do_print_html(GtkWidget *widget, const gchar *text)
{
    gboolean retval = FALSE;
    gchar *host;
    DOMINO_LOCAL_GET(PRINT_ROOT_PATH, "host", &host, NULL);
    if (!host) {
        DOMINO_SHOW_INFO("Неопределен принтер печати");

        return FALSE;
    }
    if ((retval = print_html(host, text)) == FALSE)
        DOMINO_SHOW_ERROR("Ошибка печати");

    return retval;
}
#endif
void do_tree_cell_data_func_product_name(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_product_name_format(g_value_get_string(&value));
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}
void do_tree_cell_data_func_rest(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_rest_format(g_value_get_double(&value));
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}
void do_tree_cell_data_func_big_int(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_big_int_format(g_value_get_int(&value));
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}
void do_tree_cell_data_func_big_int_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf, *v;
    buf = do_big_int_format(g_value_get_int(&value));
    v = g_strdup_printf("<b>%s</b>", buf);
    g_object_set(cell, "markup", v, NULL);
    g_free (buf);g_free(v);
}
void do_tree_cell_data_func_percent(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf, *text;
    buf = do_percent_format(g_value_get_double(&value)*100);
    if ( buf[0] != '\0') {
        text = g_strdup_printf("%s%%", buf);
        g_object_set(cell, "text", text, NULL);
        g_free (text);
    }
    else
        g_object_set(cell, "text", "", NULL);
    g_free (buf);
}
void do_tree_cell_data_func_percent_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf, *text;
    buf = do_percent_format(g_value_get_double(&value)*100);
    if ( buf[0] != '\0') {
        text = g_strdup_printf("<b>%s%%</b>", buf);
        g_object_set(cell, "markup", text, NULL);
        g_free (text);
    }
    else
        g_object_set(cell, "text", "", NULL);
    g_free (buf);
}
void do_tree_cell_data_func_rest_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);

    gchar *buf;
    gchar *markup;
    buf = do_rest_format(g_value_get_double(&value));
    markup = g_markup_printf_escaped("<b>%s</b>", buf);
    g_object_set(cell, "markup", markup, NULL);
    g_free (markup);
    g_free (buf);
}
void do_tree_cell_data_func_money(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_money_format(g_value_get_double(&value), TRUE);
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}
void do_tree_cell_data_func_money_no_negavite(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    if ( g_value_get_double(&value) > 0 ) {
        buf = do_money_format(g_value_get_double(&value), TRUE);
        g_object_set(cell, "text", buf, NULL);
        g_free (buf);
    }
    else
        g_object_set(cell, "text", "", NULL);
}
void do_tree_cell_data_func_money_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    gchar *markup;
    buf = do_money_format(g_value_get_double(&value), TRUE);
    markup = g_markup_printf_escaped("<b>%s</b>", buf);
    g_object_set(cell, "markup", markup, NULL);
    g_free (markup);
    g_free (buf);
}
void do_tree_cell_data_func_big_int_bold_depth1(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;

    path = gtk_tree_model_get_path(tree_model, iter);
    if ( gtk_tree_path_get_depth(path) == 1 )
         do_tree_cell_data_func_big_int_bold(tree_column, cell,tree_model,iter,data);
    else
         do_tree_cell_data_func_big_int(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void do_tree_cell_data_func_big_int_bold_total(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;
    GtkTreeIter new_iter;

    path = gtk_tree_model_get_path(tree_model, iter);
    gtk_tree_path_next(path);
    if ( gtk_tree_path_get_depth(path) == 1 &&
         !gtk_tree_model_get_iter(tree_model, &new_iter, path) )
         do_tree_cell_data_func_big_int_bold(tree_column, cell,tree_model,iter,data);
    else
         do_tree_cell_data_func_big_int(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void do_tree_cell_data_func_money_bold_total(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;
    GtkTreeIter new_iter;

    path = gtk_tree_model_get_path(tree_model, iter);
    gtk_tree_path_next(path);
    if ( gtk_tree_path_get_depth(path) == 1 &&
         !gtk_tree_model_get_iter(tree_model, &new_iter, path) )
         do_tree_cell_data_func_money_bold(tree_column, cell,tree_model,iter,data);
    else
         do_tree_cell_data_func_money(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void do_tree_cell_data_func_money_bold_depth1(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;

    path = gtk_tree_model_get_path(tree_model, iter);
    if ( gtk_tree_path_get_depth(path) == 1 )
         do_tree_cell_data_func_money_bold(tree_column, cell,tree_model,iter,data);
    else
         do_tree_cell_data_func_money(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void do_tree_cell_data_func_percent_bold_total(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;
    GtkTreeIter new_iter;

    path = gtk_tree_model_get_path(tree_model, iter);
    gtk_tree_path_next(path);
    if ( gtk_tree_path_get_depth(path) == 1 &&
         !gtk_tree_model_get_iter(tree_model, &new_iter, path) )
         do_tree_cell_data_func_percent_bold(tree_column, cell,tree_model,iter,data);
    else
         do_tree_cell_data_func_percent(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void do_tree_cell_data_func_percent_bold_depth1(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;

    path = gtk_tree_model_get_path(tree_model, iter);
    if ( gtk_tree_path_get_depth(path) == 1 )
         do_tree_cell_data_func_percent_bold(tree_column, cell,tree_model,iter,data);
    else
         do_tree_cell_data_func_percent(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void do_tree_cell_data_func_money_bold_no_negative(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    gchar *markup;
    if ( g_value_get_double(&value) > 0 ) {
        buf = do_money_format(g_value_get_double(&value), TRUE);
        markup = g_markup_printf_escaped("<b>%s</b>", buf);
        g_object_set(cell, "markup", markup, NULL);
        g_free (markup);
        g_free (buf);
    }
    else
        g_object_set(cell, "markup", "", NULL);
}

void do_tree_cell_data_func_money_with_sign(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gchar *buf;

    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);

    buf = do_money_format(g_value_get_double(&value), TRUE);
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}

void do_tree_cell_data_func_money_no_abr(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_money_format(g_value_get_double(&value), FALSE);
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}

void do_tree_cell_data_func_date_time(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    struct tm tm;
    time_t now;
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    now = g_value_get_long(&value);
    tm = *localtime(&now);
    buf = g_strdup_printf("%2.2d/%2.2d/%2.2d %2.2d:%2.2d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year - 100, tm.tm_hour, tm.tm_min);
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}
void do_tree_cell_data_func_date_time_date_only(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    struct tm tm;
    time_t now;
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    now = g_value_get_long(&value);
    if ( now != -1 && now != 0 ) {
        tm = *localtime(&now);
        buf = g_strdup_printf("%2.2d/%2.2d/%2.2d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year - 100);
        g_object_set(cell, "text", buf, NULL);
        g_free (buf);
    }
    else
        g_object_set(cell, "text", "", NULL);
}
gchar *do_date_time_format(time_t t)
{
    struct tm tm;
    tm = *localtime(&t);
    return g_strdup_printf("%2.2d/%2.2d/%2.2d %2.2d:%2.2d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year - 100, tm.tm_hour, tm.tm_min);
}
void do_tree_cell_data_func_pixbuf_bool(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    if (g_value_get_boolean(&value))
        g_object_set(cell, "icon-name", "object-select-symbolic", NULL);
    else
        g_object_set(cell, "icon-name", NULL, NULL);
}
void do_tree_cell_data_func_pixbuf_warning(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    if (g_value_get_boolean(&value))
        g_object_set(cell, "icon-name", "dialog-warning-symbolic", NULL);
    else
        g_object_set(cell, "icon-name", NULL, NULL);
}

void do_tree_cell_data_func_pixbuf_connect(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    switch (g_value_get_int(&value)) {
        case 0 :
            g_object_set(cell, "icon-name", "network-wired-acquiring-symbolic", NULL);
            break;
        case 1:
            g_object_set(cell, "icon-name", "network-wired-symbolic", NULL);
            break;
        case -1:
            g_object_set(cell, "icon-name", "network-wired-disconnected-symbolic", NULL);
            break;
        default:
            g_object_set(cell, "icon-name", NULL, NULL);
            break;
    };
}
static void do_find_barcode_dialog_entry_activate_cb(GtkWidget *entry, GtkWindow *dialog)
{
    GtkWidget *w = gtk_window_get_default_widget(GTK_WINDOW (dialog));
    if (w && gtk_widget_get_sensitive(w))
         gtk_window_activate_default (GTK_WINDOW (dialog));
}

gchar *do_find_barcode_dialog(GtkWidget *parent)
{
    GtkDialog *dialog;
    GtkWidget *l;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *entry;
    GtkWindow *win = NULL;
    gchar *retval = NULL;

    if (parent)
        win = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(parent)));

    dialog =  (GtkDialog*)gtk_dialog_new_with_buttons(
                        "Поиск по продажному коду",
                        win,
                        GTK_DIALOG_MODAL ,
                        "Ok",
                        GTK_RESPONSE_ACCEPT,
                        "Отмена",
                        GTK_RESPONSE_REJECT,
                        NULL);

    gtk_dialog_set_default_response (dialog, GTK_RESPONSE_ACCEPT);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_icon_name(GTK_WINDOW(dialog), "bookcase");

	vbox = gtk_dialog_get_content_area(dialog);

    gtk_container_set_border_width( GTK_CONTAINER( vbox ), 12 );

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 4 );

    l = gtk_label_new("Штрихкод:");
    entry = do_entry_new();
    g_object_set(entry, "regex-mask", "^([0-9]+)$", NULL);
	g_signal_connect (entry, "activate", G_CALLBACK (do_find_barcode_dialog_entry_activate_cb), dialog);

	gtk_box_pack_start (GTK_BOX (hbox), l, FALSE, FALSE, 4);
	gtk_box_pack_start (GTK_BOX (hbox), entry, FALSE, FALSE, 4);

	gtk_widget_show_all (vbox);


    while (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        if (do_validate_valid(DO_VALIDATE(entry))) {
            retval = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
            break;
        }
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
    return retval;
}
#ifndef MIN_SCREEN
static void do_file_set(GtkFileChooserButton *widget, GtkWidget *entry)
{
    g_object_set(entry, "text",
      gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (widget)),
      NULL);

}
#endif
const gchar *do_get_valuta_name(gint code)
{
    gchar *str = g_strdup_printf("%d", code);
    gchar *retval;
    DOMINO_COMMON_GET("Valuta", str, &retval, NULL);
    g_free(str);
    return retval;
}
int  do_get_valuta_code(const char *name)
{
    GList *val_list, *l;
    val_list = DOMINO_COMMON_PROPERTIES("Valuta");
    for (l = val_list; l; l = l->next) {
        gchar *value;
        DOMINO_COMMON_GET("Valuta", l->data, &value, NULL);
        if (value && !strcmp(value, name))
            return atoi(l->data);
    }
    return 0;
}
#ifndef MIN_SCREEN
static gchar *text_to_xml_value(const gchar *text)
{
    gchar *ch;
    GString *retval;
    ch = (char*)text;
    retval = g_string_new("");
    while ( *ch != '\0' ) {
        switch ( *ch ) {
            case '<':
                g_string_append(retval, "&lt;");
                break;
            case '>':
                g_string_append(retval, "&gt;");
                break;
            default:
                g_string_append_c(retval, *ch);
        }
        ch++;
    }
    return g_string_free(retval, FALSE);
}
#endif
enum {
    DO_KEY_ENTRY_PRINTER_HOST = 0,
    DO_KEY_ENTRY_PRINTER_DUPLEX,
    DO_KEY_ENTRY_PRINTER_ZEBRA,
    DO_KEY_ENTRY_WEBKIT_STARTPAGE,
    DO_KEY_ENTRY_WEBKIT_PROXY,
    DO_KEY_ENTRY_WEBKIT_PROXY_IGNORE,
    DO_KEY_ENTRY_WEBKIT_ZOOM_LEVEL,
    DO_KEY_ENTRY_EXTENDED_PPDF,
    DO_KEY_ENTRY_EXTENDED_EMAIL,
    //DO_KEY_ENTRY_EXTENDED_SSH,

   /* DO_KEY_ENTRY_PAGE_TOP_MARGIN,
    DO_KEY_ENTRY_PAGE_LEFT_MARGIN,
    DO_KEY_ENTRY_PAGE_RIGHT_MARGIN,
    DO_KEY_ENTRY_PAGE_BOTTOM_MARGIN,*/

    DO_KEY_ENTRY_STYLE_DEFAULT,
    DO_KEY_ENTRY_FONT1,
    DO_KEY_ENTRY_GRID,

    N_DO_KEY_ENTRY_,
};

typedef struct {
    gchar *path;
    gchar *name;
} DoKeyInfo;

static const DoKeyInfo do_key_info[N_DO_KEY_ENTRY_] =
{
    {"Print", "host"},
    {"Print", "duplex"},
    {"Print", "zebra"},
    {"Webkit", "start-page"},
    {"Webkit", "proxy-string"},
    {"Webkit", "proxy-ignore"},
    {"Webkit", "zoom-level"},
    {"Extended", "pdf"},
    {"Extended", "email"},
    //{"Extended", "ssh"},

    /*{"PageSetup", "top-margin"},
    {"PageSetup", "left-margin"},
    {"PageSetup", "right-margin"},
    {"PageSetup", "bottom-margin"},*/

    {"Style", "default"},
    {"Style", "font"},
    {"Style", "grid"},

};

void domino_font_style_apply(const gchar *fontname)
{
    if ( !fontname )
        return;
#if GTK_MAJOR_VERSION == 2 // to do
	gchar *style;
	style = g_strdup_printf(
    "style \"myfont\"{\n font_name = \"%s\"\n}\nwidget_class \"*\" style \"myfont\"\ngtk-font-name = \"%s\"\n",
        fontname,
        fontname
    );
    gtk_rc_parse_string(style);
    g_free(style);
    GtkSettings *setting = NULL;

    setting = gtk_settings_get_default();
    gtk_rc_reset_styles(setting);

#endif // GTK_MAJOR_VERSION

}
#ifndef MIN_SCREEN
static void  font_changed(GtkFontButton *widget, gpointer user_data)
{
    domino_font_style_apply(gtk_font_button_get_font_name(widget));
}
#endif
void  style_changed(GtkButton *button,
                                                        gpointer   user_data)
{
    GtkWidget **entry = user_data;
    gboolean active;
    active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
    gtk_widget_set_sensitive(entry[DO_KEY_ENTRY_FONT1], active);
    if ( active  )
        domino_font_style_apply(gtk_font_button_get_font_name(GTK_FONT_BUTTON(entry[DO_KEY_ENTRY_FONT1])));
    else
        domino_font_style_apply("");
}

#ifndef MIN_SCREEN
gboolean do_common_edit(GtkWidget *widget)
{
    GtkDialog *dialog;
    GtkWidget *l;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *entry[N_DO_KEY_ENTRY_];
	GtkWidget *t;
	GtkWidget *w;
    GtkWindow *win = NULL;
	GtkWidget *notebook;
	gint row;
	gint i;
    gchar *text;
    gboolean retval;
#ifdef _WIN32
	GtkWidget *unit_entry;
	GtkWidget *local_entry;
	GtkWidget *main_entry;
#endif
    if (widget)
        win = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(widget)));

    dialog =  (GtkDialog*)gtk_dialog_new_with_buttons(
                        "Парaметры",
                        win,
                        GTK_DIALOG_MODAL,
                        "Ok",
                        GTK_RESPONSE_ACCEPT,
                        "Отмена",
                        GTK_RESPONSE_REJECT,
                        NULL);
    g_signal_connect (dialog, "configure-event",
                      G_CALLBACK (do_window_configure_event_cb), "CommonEditDialog");

    DOMINO_PROFILE_OBJECT_INIT(G_OBJECT(dialog), "CommonEditDialog",
              "window-position", "",
              "window-size", "400x200",
               NULL);
    gtk_dialog_set_default_response (dialog, GTK_RESPONSE_ACCEPT);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_icon_name (GTK_WINDOW(dialog), "bookcase");

	vbox = gtk_dialog_get_content_area(dialog);
	notebook = gtk_notebook_new();
	gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);

#ifdef _WIN32
    l =  gtk_label_new (NULL);
    gtk_label_set_markup_with_mnemonic (GTK_LABEL (l), "_Системные");
    row = 0;

    t = hig_workarea_create();
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), t, l);

    hig_workarea_add_section_title(t, &row, "Сервера");
    hig_workarea_finish( t, &row );

    local_entry = do_entry_new();
    hig_workarea_add_row( t, &row, "Локальный(host):", local_entry, NULL );
    main_entry = do_entry_new();
    hig_workarea_add_row( t, &row, "Главный(host):", main_entry, NULL );

    hig_workarea_add_section_title(t, &row, "Локальные");
    hig_workarea_finish( t, &row );

    unit_entry = do_entry_new();
    hig_workarea_add_row( t, &row, "Аптека:", unit_entry, NULL );
    gtk_entry_set_text(GTK_ENTRY(unit_entry), domino_unit());

#endif
    l =  gtk_label_new (NULL);
    gtk_label_set_markup_with_mnemonic (GTK_LABEL (l), "_Пользовательские");
    row = 0;

    t = hig_workarea_create();
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), t, l);

    hig_workarea_add_section_title(t, &row, "Принтер");
    hig_workarea_finish( t, &row );

    entry[DO_KEY_ENTRY_PRINTER_HOST] = do_entry_new();
    hig_workarea_add_row( t, &row, "Компьютер(host):", entry[DO_KEY_ENTRY_PRINTER_HOST], NULL );
    entry[DO_KEY_ENTRY_PRINTER_DUPLEX] = do_entry_new();
    g_object_set(entry[DO_KEY_ENTRY_PRINTER_DUPLEX], "regex-mask", "^(Да|Нет)$", NULL);
    hig_workarea_add_row( t, &row, "Дуплекс(Да|Нет):", entry[DO_KEY_ENTRY_PRINTER_DUPLEX], NULL );

    entry[DO_KEY_ENTRY_PRINTER_ZEBRA] = do_entry_new();
    hig_workarea_add_row( t, &row, "Принтер штрих кодов:", entry[DO_KEY_ENTRY_PRINTER_ZEBRA], NULL );

    /*hig_workarea_add_section_title(t, &row, "Параметры страницы (мм)");
    hig_workarea_finish( t, &row );

    entry[DO_KEY_ENTRY_PAGE_TOP_MARGIN] = do_entry_new();
    hig_workarea_add_row( t, &row, "Верхний отступ:", entry[DO_KEY_ENTRY_PAGE_TOP_MARGIN], NULL );

    entry[DO_KEY_ENTRY_PAGE_LEFT_MARGIN] = do_entry_new();
    hig_workarea_add_row( t, &row, "Леный отступ:", entry[DO_KEY_ENTRY_PAGE_LEFT_MARGIN], NULL );

    entry[DO_KEY_ENTRY_PAGE_RIGHT_MARGIN] = do_entry_new();
    hig_workarea_add_row( t, &row, "Правый отступ:", entry[DO_KEY_ENTRY_PAGE_RIGHT_MARGIN], NULL );

    entry[DO_KEY_ENTRY_PAGE_BOTTOM_MARGIN] = do_entry_new();
    hig_workarea_add_row( t, &row, "Нижний отступ:", entry[DO_KEY_ENTRY_PAGE_BOTTOM_MARGIN], NULL );
*/
    hig_workarea_add_section_title(t, &row, "Внешний вид");
    hig_workarea_finish( t, &row );

    entry[DO_KEY_ENTRY_STYLE_DEFAULT] = gtk_check_button_new() ;
    hig_workarea_add_row( t, &row, "Ручная настройка: ", entry[DO_KEY_ENTRY_STYLE_DEFAULT], NULL );
    g_signal_connect(entry[DO_KEY_ENTRY_STYLE_DEFAULT], "clicked", G_CALLBACK(style_changed), entry);

    entry[DO_KEY_ENTRY_FONT1] = gtk_font_button_new ();
    hig_workarea_add_row( t, &row, "Шрифт:", entry[DO_KEY_ENTRY_FONT1], NULL );
    gtk_widget_set_sensitive(entry[DO_KEY_ENTRY_FONT1], FALSE);
    g_signal_connect(entry[DO_KEY_ENTRY_FONT1], "font-set", G_CALLBACK(font_changed), win);

    entry[DO_KEY_ENTRY_GRID] = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(entry[DO_KEY_ENTRY_GRID]), "Нет");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(entry[DO_KEY_ENTRY_GRID]), "Горизонтально");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(entry[DO_KEY_ENTRY_GRID]), "Вертикально");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(entry[DO_KEY_ENTRY_GRID]), "Все");
    hig_workarea_add_row( t, &row, "Обрамление :", entry[DO_KEY_ENTRY_GRID], NULL );
    //g_signal_connect(entry[DO_KEY_ENTRY_GRID], "changed", G_CALLBACK(grid_changed), win);



    hig_workarea_add_section_title(t, &row, "Интернет");
    hig_workarea_finish( t, &row );

    entry[DO_KEY_ENTRY_WEBKIT_STARTPAGE] = do_entry_new();
    hig_workarea_add_row( t, &row, "Поисковик:", entry[DO_KEY_ENTRY_WEBKIT_STARTPAGE], NULL );

    entry[DO_KEY_ENTRY_WEBKIT_PROXY] = do_entry_new();
    hig_workarea_add_row( t, &row, "Прокси:", entry[DO_KEY_ENTRY_WEBKIT_PROXY], NULL );

    entry[DO_KEY_ENTRY_WEBKIT_PROXY_IGNORE] = do_entry_new();
    hig_workarea_add_row( t, &row, "Исключения для прокси:", entry[DO_KEY_ENTRY_WEBKIT_PROXY_IGNORE], NULL );

    entry[DO_KEY_ENTRY_WEBKIT_ZOOM_LEVEL] = do_entry_new();
    hig_workarea_add_row( t, &row, "Увеличение страниц:", entry[DO_KEY_ENTRY_WEBKIT_ZOOM_LEVEL], NULL );

    entry[DO_KEY_ENTRY_EXTENDED_PPDF] = gtk_entry_new();
    hig_workarea_add_section_title(t, &row, "Внешние модули");
    hig_workarea_finish( t, &row );
    w = gtk_file_chooser_button_new("", GTK_FILE_CHOOSER_ACTION_OPEN);
    g_signal_connect (w, "file-set",
                      G_CALLBACK (do_file_set), entry[DO_KEY_ENTRY_EXTENDED_PPDF]);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	gtk_box_pack_start (GTK_BOX (hbox), entry[DO_KEY_ENTRY_EXTENDED_PPDF], TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), w, TRUE, TRUE, 0);

    hig_workarea_add_row( t, &row, "Pdf(приложение):", hbox, NULL );

    entry[DO_KEY_ENTRY_EXTENDED_EMAIL] = gtk_entry_new();
    w = gtk_file_chooser_button_new("", GTK_FILE_CHOOSER_ACTION_OPEN);
    g_signal_connect (w, "file-set",
                      G_CALLBACK (do_file_set), entry[DO_KEY_ENTRY_EXTENDED_EMAIL]);


	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (hbox), entry[DO_KEY_ENTRY_EXTENDED_EMAIL], TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), w, TRUE, TRUE, 0);

    hig_workarea_add_row( t, &row, "Почтовое приложение:", hbox, NULL );

/*
    entry[DO_KEY_ENTRY_EXTENDED_SSH] = gtk_entry_new();
    hig_workarea_add_row( t, &row, "Терминал:", entry[DO_KEY_ENTRY_EXTENDED_SSH], NULL );
*/
    for (i = 0; i < N_DO_KEY_ENTRY_; i++) {
        text = NULL;
        domino_config_property_get(DOMINO_CONFIG_LOCAL,
                      do_key_info[i].path, do_key_info[i].name,
                      &text, NULL);
        if ( !text )
            continue;

        if GTK_IS_ENTRY(entry[i])
            g_object_set(entry[i], "text", text, NULL);
        else
        if GTK_IS_FONT_BUTTON(entry[i])
            gtk_font_button_set_font_name(GTK_FONT_BUTTON(entry[i]), text);
        else
        if GTK_IS_TOGGLE_BUTTON(entry[i])
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(entry[i]), !strcmp(text,"1"));
        else
        if GTK_IS_COMBO_BOX(entry[i])
            gtk_combo_box_set_active(GTK_COMBO_BOX(entry[i]), atoi(text));



    }
	gtk_widget_show_all (vbox);


    retval = FALSE;
    while ( gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT ) {
        for (i = 0; i < N_DO_KEY_ENTRY_; i++) {
            gchar *value;
            gchar bud[50];
            if GTK_IS_ENTRY(entry[i])
                g_object_get(entry[i], "text", &text, NULL);
            else
            if GTK_IS_FONT_BUTTON(entry[i]) {
                text = (char*)gtk_font_button_get_font_name(GTK_FONT_BUTTON(entry[i]));
            }
            else
            if GTK_IS_TOGGLE_BUTTON(entry[i]) {
                sprintf(bud, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(entry[i])) ? "1" : "0");
                text = bud;
            }
            else
            if GTK_IS_COMBO_BOX(entry[i]) {
                sprintf(bud, "%d", gtk_combo_box_get_active(GTK_COMBO_BOX(entry[i])));
                text = bud;
            }

            value = text_to_xml_value(text);
            domino_config_property_set(DOMINO_CONFIG_LOCAL,
                          do_key_info[i].path, do_key_info[i].name,
                          value, NULL);
            g_free(value);
        }
        domino_config_save(DOMINO_CONFIG_LOCAL, TRUE);
        retval = TRUE;
        break;
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
    return retval;
}
#endif
gboolean do_window_configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gchar *path)
{
	do_window_save_setting(GTK_WINDOW(widget), path);
	return FALSE;
}
#define ISLEAP(year)	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#define	SECS_PER_HOUR (60 * 60)
static const unsigned short int __mon_yday[2][13] = {
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

static void day_of_the_week (struct tm *tm)
{
  int corr_year = 1900 + tm->tm_year - (tm->tm_mon < 2);
  int wday = (-473
	      + (365 * (tm->tm_year - 70))
	      + (corr_year / 4)
	      - ((corr_year / 4) / 25) + ((corr_year / 4) % 25 < 0)
	      + (((corr_year / 4) / 25) / 4)
	      + __mon_yday[0][tm->tm_mon]
	      + tm->tm_mday - 1);
  tm->tm_wday = ((wday % 7) + 7) % 7;
}

static void day_of_the_year (struct tm *tm)
{
  tm->tm_yday = (__mon_yday[ISLEAP(1900 + tm->tm_year)][tm->tm_mon]
		 + (tm->tm_mday - 1));
}

int do_atodate(const char *str, struct tm *tm)
{
    tm->tm_isdst = 0; // no daylight
    memset(tm, 0, sizeof(*tm));

    char *cp = (char*)str;
    tm->tm_mday = atoi(cp) ;
    while (*cp && *cp != '/' && *cp != '.')
      ++cp;
    if (*cp) ++cp;
    tm->tm_mon = atoi(cp) - 1;
    while (*cp && *cp != '/' && *cp != '.')
      ++cp;
    if (*cp) ++cp;
    tm->tm_year = atoi(cp);
    if (tm->tm_year > 1000)
        tm->tm_year -= 1900;
    else
        tm->tm_year += 100;

    day_of_the_week(tm);
    day_of_the_year(tm);
    return (tm->tm_year > -70 && tm->tm_year < 10000) && (tm->tm_mon >=0 && tm->tm_mon <12) && (tm->tm_mday > 0 && tm->tm_mday < 32);
}
time_t do_date_set_ymd(struct tm *tm, int year, int month, int day)
{
    memset(tm, 0, sizeof(*tm));
    tm->tm_mday = day;
    tm->tm_mon = month - 1;
    tm->tm_year = year - 1900;
    day_of_the_week(tm);
    day_of_the_year(tm);
    return mktime(tm);
}

static time_t do_text_search_date(const gchar *text)
{
    gchar *ch, *head, buf[5];
    head = (gchar*)text;
    struct tm tm;

    int day = 0,month = 0,year = 0;

    ch = head;
    for (; *ch >= '0' && *ch <='9' && ch - head < 2; ch++)
        buf[ch - head] = *ch;
    if (ch - head < 2)
        return -1;
    buf[ch - head] = '\0';
    day = atoi(buf);
    if (*ch == '.' || *ch == '/') ch++;

    head = ch;
    for (; *ch >= '0' && *ch <='9' && ch - head < 2; ch++)
        buf[ch - head] = *ch;
    if (ch - head < 2)
        goto do_text_search_date_mktime;
    buf[ch - head] = '\0';
    month = atoi(buf);
    if (*ch == '.' || *ch == '/') ch++;

    head = ch;
    for (; *ch >= '0' && *ch <='9' && ch - head < 4; ch++)
        buf[ch - head] = *ch;
    if (ch - head < 2)
        goto do_text_search_date_mktime;
    buf[ch - head] = '\0';
    year = atoi(buf);
do_text_search_date_mktime:
    if (!month || !year) {
        time_t now = time(NULL);
        tm = *localtime(&now);
        if (!month)
            month = tm.tm_mon + 1;
        if (!year)
            year = tm.tm_year + 1900;
    }
    if (year < 1000)
        year +=2000;

    return do_date_set_ymd(&tm, year, month, day);
}

gboolean do_tree_view_search_equal_func_date_time(GtkTreeModel *model,
                                                         gint column,
                                                         const gchar *key,
                                                         GtkTreeIter *iter,
                                                         gpointer search_data)
{
    GValue value = { 0, };
    time_t time;

    time = do_text_search_date(key);
    if (time == -1)
        return TRUE;

    gtk_tree_model_get_value(model, iter, GPOINTER_TO_INT(search_data), &value);

    if (g_value_get_long(&value) == time)
        return FALSE;

    return TRUE;
}
gboolean do_text_utf8_find_str(const char *haystack, const char *needle)
{
    gboolean retval = FALSE;
    gchar *normalized_string;
    gchar *normalized_key;
    gchar *case_normalized_string = NULL;
    gchar *case_normalized_key = NULL;

    normalized_string = g_utf8_normalize (haystack, -1, G_NORMALIZE_ALL);
    normalized_key = g_utf8_normalize (needle, -1, G_NORMALIZE_ALL);

    if (normalized_string && normalized_key) {
        case_normalized_string = g_utf8_casefold (normalized_string, -1);
        case_normalized_key = g_utf8_casefold (normalized_key, -1);

        if (strstr (case_normalized_string, case_normalized_key))
            retval = TRUE;
    }
    g_free (normalized_key);
    g_free (normalized_string);
    g_free (case_normalized_key);
    g_free (case_normalized_string);
    return retval;
}
GtkTreePath *gtk_tree_path_new_last(GtkTreeModel *model)
{
    GtkTreePath *path;
    GtkTreeIter iter;

    path = gtk_tree_path_new_first();

    while (gtk_tree_model_get_iter(model, &iter, path))
        gtk_tree_path_next(path);

    gtk_tree_path_prev(path);

    return path;
}
/*static GThread* main_thread = NULL;
void do_set_main_thread()
{
     main_thread = g_thread_self();
}

static GList *error_list = NULL;
static gboolean show_error(gchar *message)
{
    do_message_dialog_show(NULL, GTK_MESSAGE_ERROR, GTK_BUTTON)
    error_list = g_list_append(error_list, message);
    if ( g_list_length(error_list) > 1 )
        return FALSE;

    while ( g_list_length(error_list) > 0 ) {
        gchar *msg;
        msg = error_list->data;

        GtkWidget *dialog = gtk_message_dialog_new (NULL,
                              GTK_DIALOG_DESTROY_WITH_PARENT,
                              GTK_MESSAGE_ERROR,
                              GTK_BUTTONS_YES_NO,
                              "%s\nЗавершить работу.", msg);
        gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ALWAYS);
        gtk_window_set_icon_name(GTK_WINDOW(dialog), DO_STOCK_DOMINO_APPS);
        gtk_window_set_title(GTK_WINDOW(dialog), "Ошибка");
        if ( gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES )
            exit(1);
        gtk_widget_destroy (dialog);
        error_list = g_list_remove(error_list, msg);
        g_free(msg);
    }
    return FALSE;
}
*/
void do_utf8_log(int level, const gchar *format, ...)
{
    va_list args;
    gchar *string;
    gchar *message;

    g_return_if_fail (format != NULL);

    va_start (args, format);
    string = g_strdup_vprintf (format, args);
    va_end (args);

    message =  g_locale_from_utf8(string, -1, NULL, NULL, NULL);
    g_free (string);
    do_log(level, "%s", message);
    g_free (message);
}
static do_list_t *gtk_thread_log_off = NULL;
static GMutex mutex;
void do_gtk_log_init()
{
    gtk_thread_log_off = do_list_new(FALSE);
    g_mutex_init(&mutex);

}
void do_gtk_log_off()
{
    int i;

    GThread *self_thread;
    self_thread = g_thread_self();
    g_mutex_lock(&mutex);
    for ( i = 0; i < gtk_thread_log_off->count; i++ )
        if ( gtk_thread_log_off->list[i] == self_thread )
            return;
    do_list_add(gtk_thread_log_off, self_thread);
    g_mutex_unlock(&mutex);
}
void do_gtk_log_on()
{
    int i;
    GThread *self_thread;
    self_thread = g_thread_self();

    g_mutex_lock(&mutex);
    for ( i = 0; i < gtk_thread_log_off->count; i++ )
        if ( gtk_thread_log_off->list[i] == self_thread ) {
            do_list_delete(gtk_thread_log_off, i);
            break;
        }
    g_mutex_unlock(&mutex);
}
/*
int do_gtk_log(const char *str, int level, void *user_data)
{
    if ( level == LOG_ERR ) {
        if ( gtk_thread_log_off ) {
            int i;
            GThread *self_thread;
            self_thread = g_thread_self();

            for ( i = 0; i < gtk_thread_log_off->count; i++ )
                if ( gtk_thread_log_off->list[i] == self_thread )
                    return TRUE;

        }
        gchar *utf8;
        utf8 = g_locale_to_utf8(str, -1, NULL, NULL, NULL);
        gdk_threads_add_idle((GSourceFunc)show_error, utf8);
    }
    return TRUE;
}
*/

gint do_text_utf8_str_cmp_no_case(const char *str1, const char *str2)
{
    gint retval = FALSE;
    gchar *normalized_string;
    gchar *normalized_key;
    gchar *case_normalized_string = NULL;
    gchar *case_normalized_key = NULL;

    normalized_string = g_utf8_normalize (str1, -1, G_NORMALIZE_ALL);
    normalized_key = g_utf8_normalize (str2, -1, G_NORMALIZE_ALL);

    if (normalized_string && normalized_key) {
        case_normalized_string = g_utf8_casefold (normalized_string, -1);
        case_normalized_key = g_utf8_casefold (normalized_key, -1);

        retval = strcmp (case_normalized_string, case_normalized_key);
    }
    else
        retval = strcmp(str1, str2);

    g_free (normalized_key);
    g_free (normalized_string);
    g_free (case_normalized_key);
    g_free (case_normalized_string);
    return retval;
}
static GdkCursor* cursor_wait = NULL;
//!!static GdkCursor* cursor_pre_wait = NULL;
#define GDK_LEFT_PTR_WATCH_NAME "left_ptr_watch"
//static gboolean sad_read = FALSE;

void do_start_long_operation(GtkWidget *widget)
{
    if ( !cursor_wait ) {
#if GTK_CHECK_VERSION(3,16,0)
        GdkDisplay *display = gdk_window_get_display(gtk_widget_get_window(GTK_WIDGET(widget)));
        gdk_cursor_new_for_display(display,GDK_WATCH);
#else
        cursor_wait = gdk_cursor_new(GDK_WATCH);
#endif
    }
    if ( widget && GDK_IS_WINDOW(gtk_widget_get_window(widget)) )
        gdk_window_set_cursor(gtk_widget_get_window(widget), cursor_wait);
}
void do_end_long_operation(GtkWidget *widget)
{
    if ( widget && GDK_IS_WINDOW(gtk_widget_get_window(widget)) )
        gdk_window_set_cursor(gtk_widget_get_window(widget), NULL);
}
GList *util_string_to_colon(const gchar *text, gint colwidth)
{
    GList *retval = NULL;

    gunichar  out;
    int pos, spos = 0;
    gchar *head = (gchar*)text, *sep = NULL;
    gchar *p;
    gchar *str;
    for (p = (gchar*)text, pos = 0; p && *p !='\0'; p = (gchar*)g_utf8_next_char(p), pos++) {
        out = g_utf8_get_char(p);
        if ( g_unichar_isspace(out) ) {
            sep = p;
            spos = 0;
        }
        else
            spos++;
        if ( pos == colwidth ) {
            if ( !sep ) {
                str = g_strndup(head, p - head);
                head = (gchar*)g_utf8_next_char(p);
            } else {
                str = g_strndup(head, sep - head);
                head = (gchar*)g_utf8_next_char(sep);
                sep = NULL;
                pos = spos;
                spos = 0;
            }
            retval = g_list_append(retval, str);
        }
    }
    if ( head )
        retval = g_list_append(retval, g_strdup(head));
    if ( !retval )
        retval = g_list_append(retval, g_strdup(""));

    return retval;
}
#ifdef USE_POPPLER
static void do_unitx_print_begin_print(GtkPrintOperation *operation,
                                                        GtkPrintContext   *context,
                                                        PopplerDocument *doc)
{
    gtk_print_operation_set_n_pages(operation, poppler_document_get_n_pages(doc));
}
static void do_utilx_print_done(GtkPrintOperation      *operation,
                                                        GtkPrintOperationResult result,
                                                        gpointer                user_data)
{
}
static void do_utilx_print_draw_page(GtkPrintOperation *operation,
                                                        GtkPrintContext   *context,
                                                        gint               page_nr,
                                                        PopplerDocument *doc)
{
    PopplerPage *page;
    cairo_t     *cairo;
    gdouble x_scale, y_scale, scale;
    gdouble cr_width, cr_height;
    gdouble width, height;
    gdouble top = 0, bottom = 0, left = 0, right = 0;

    cairo = gtk_print_context_get_cairo_context(context);
    page = poppler_document_get_page(doc, page_nr);
    //if ( !gtk_print_context_get_hard_margins (context, &top, &bottom, &left, &right) ) {
        top = 0;
        bottom = 0;
        left = 0;
        right = 0;
    //}
    cr_width = gtk_print_context_get_width (context);
    cr_height = gtk_print_context_get_height (context);
    poppler_page_get_size (page, &width, &height);
    //x_scale = (cr_width - left - right) / width;
    if ( TRUE ) {
        x_scale = (cr_width + left + right ) / width;
    //y_scale = (cr_height - top - bottom) / height;
        y_scale = (cr_height +top + bottom) / height;
        scale = ( x_scale < y_scale ) ? x_scale : y_scale;
        cairo_translate (cairo, -left, -top);
    }


    if (/*print->page_scale == EV_SCALE_FIT_TO_PRINTABLE_AREA || */
         scale < 1.0 ) {
        cairo_scale (cairo, scale, scale);
    }
    poppler_page_render_for_printing(page, cairo);

}
static void do_utilx_print_status_changed(GtkPrintOperation *operation,
                                                        PopplerDocument *doc)
{
}
static void do_utilx_print_request_page_setup(GtkPrintOperation *operation,
                                                        GtkPrintContext   *context,
                                                        gint               page_nr,
                                                        GtkPageSetup      *setup,
                                                        PopplerDocument *doc)
{
    /*GtkPaperSize     *paper_size;
    PopplerPage *page;
    gdouble width, height;

    page = poppler_document_get_page(doc, page_nr);
    poppler_page_get_size (page, &width, &height);

    paper_size = gtk_paper_size_new_custom ("custom", "custom",
                    width, height, GTK_UNIT_POINTS);
    gtk_page_setup_set_paper_size_and_default_margins (setup, paper_size);
    gtk_paper_size_free (paper_size);
    */
}

GtkPageSetup *do_utilx_get_default_page_setup(gboolean margin)
{
    GtkPageSetup *page;
    GtkPaperSize *paper_size;

    page = gtk_page_setup_new();
    paper_size = gtk_paper_size_new(GTK_PAPER_NAME_A4);

    gdouble lm=0,tm=0,rm=0,bm=0;
    if ( margin ) {
        gchar *left_margin, *top_margin, *right_margin, *bottom_margin;
        DOMINO_LOCAL_GET("PageSetup", "top-margin", &top_margin,
                                      "left-margin", &left_margin,
                                      "right-margin", &right_margin,
                                      "bottom-margin", &bottom_margin,
                                    NULL);
        if ( top_margin )
            tm = atof(top_margin);
        if ( left_margin )
            lm = atof(left_margin);
        if ( right_margin )
            rm = atof(right_margin);
        if ( bottom_margin )
            bm = atof(bottom_margin);
        if ( !tm ) tm = 10;
        if ( !lm ) lm = 10;
        if ( !rm ) rm = 10;
        if ( !bm ) bm = 10;
    }

    gtk_page_setup_set_orientation(page, GTK_PAGE_ORIENTATION_PORTRAIT);
    gtk_page_setup_set_paper_size(page, paper_size);
    gtk_page_setup_set_top_margin(page, tm, GTK_UNIT_MM);
    gtk_page_setup_set_left_margin(page, lm, GTK_UNIT_MM);
    gtk_page_setup_set_bottom_margin(page, bm, GTK_UNIT_MM);
    gtk_page_setup_set_right_margin(page, rm, GTK_UNIT_MM);

    return page;

}

gboolean do_utilx_print_pdf_file(GtkWindow *window, const gchar *uri, gboolean show_dialog, gboolean duplex, gboolean margin)
{
    GtkPrintOperation *op;
    PopplerDocument *doc;
    GError *error = NULL;

    doc = poppler_document_new_from_file(uri, NULL, &error);

    if ( !doc ) {
        DOMINO_SHOW_ERROR("Невозможно открыть файл \"%s\":%s", uri, error->message);
        g_object_unref(error);
        return FALSE;
    }

    op = gtk_print_operation_new();

	g_signal_connect (op, "begin_print",
				  G_CALLBACK (do_unitx_print_begin_print),
				  doc);
	g_signal_connect (op, "done",
				  G_CALLBACK (do_utilx_print_done),
				  doc);
	g_signal_connect (op, "draw_page",
				  G_CALLBACK (do_utilx_print_draw_page),
				  doc);
	g_signal_connect (op, "status_changed",
				  G_CALLBACK (do_utilx_print_status_changed),
				  doc);
	g_signal_connect (op, "request_page_setup",
				  G_CALLBACK (do_utilx_print_request_page_setup),
				  doc);
	gtk_print_operation_set_allow_async (op, FALSE);
	gtk_print_operation_set_unit (op, GTK_UNIT_MM);
	gtk_print_operation_set_custom_tab_label (op, "Печать...");

	GtkPrintOperationAction action = GTK_PRINT_OPERATION_ACTION_PRINT;
	if ( show_dialog || duplex )
        action = GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG;

    gboolean res;
    GtkPrintSettings *settings;
    GtkPageSetup *page;

    page = do_utilx_get_default_page_setup(margin);

    settings = gtk_print_settings_new();
    if ( duplex )
        gtk_print_settings_set_duplex(settings, GTK_PRINT_DUPLEX_HORIZONTAL);
    gtk_print_operation_set_print_settings(op, settings);

    gtk_print_operation_set_default_page_setup(op, page);
    res = gtk_print_operation_run(op, action , window, &error);

    if ( !res ) {
        DOMINO_SHOW_ERROR("Невозможно напечатать файл \"%s\":%s", uri, error->message);
        g_object_unref(error);
        return FALSE;
    }
    g_object_unref(op);
    g_object_unref(settings);


    return TRUE;
}
#endif
const gchar *season_name(int season)
{
    switch (season) {
        case 0:
            return "Зима";
        case 1:
            return "Весна";
        case 2:
            return "Лето";
        case 3:
            return "Осень";
        default:
            return "";
    }
}
gchar *to_ru_upper_text(const gchar *text)
{
    int j,nlen=0;
    const gchar *buf = text;
    gchar *result = g_malloc(sizeof (gunichar) * strlen (buf) + 1);
    gchar *p;
    gunichar out;

    for (p = (gchar*)buf; p && *p !='\0' ; p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        for (j = 0; j < 66; j++)
            if (ru_layout[j + 66] == out) {
                out = ru_layout[j];
                break;
            }
        out = g_unichar_toupper(out);
        nlen += g_unichar_to_utf8(out, result + nlen);
    }
    result[nlen] = '\0';
    return result;
}

gboolean gtk_tree_view_seach_by_product_name_func(GtkTreeModel *model,
                                                         gint column,
                                                         const gchar *key,
                                                         GtkTreeIter *iter,
                                                         gpointer search_data)
{
    gchar *search_text;
    const gchar *text;
    GValue name={0,};
    gboolean result;

    gtk_tree_model_get_value(model, iter, column, &name);
    text = g_value_get_string(&name);
    search_text = to_ru_upper_text(key);
    if ( strlen(search_text) <= strlen(text) )
        result = strncmp(search_text, text, strlen(search_text));
    else
        result = TRUE;
    g_free(search_text);


    return result;
}
#ifdef MINIMAL_WIN
int do_show_html(const gchar *html_string)
{
#ifdef _WIN32

    gchar *email;
    GError *error = NULL;
    char *templ;
    char *filename;
    char *name;
    char *cmdline;
    int fd;

    templ = g_build_filename (g_get_tmp_dir (), "reportXXXXXX", NULL);
    fd = g_mkstemp (templ);
    close (fd);
    filename = g_strconcat (templ, ".html", NULL);
    g_free (templ);
    name = g_locale_from_utf8(filename, -1, NULL, NULL, &error);
    if (!do_save_to_file(name, (void*)html_string, strlen(html_string))) {
        DOMINO_SHOW_ERROR("Файл \"%s\" не сохранен", filename);
        return FALSE;
    }
    cmdline = g_strdup_printf("start %s", name);
    popen (cmdline, "r");
    g_free(cmdline);
#endif

    return TRUE;
}
#endif
