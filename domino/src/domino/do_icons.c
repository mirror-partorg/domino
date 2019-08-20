#include "do_icons.h"
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf-core.h>
#include "do_stock.h"
#include <dolib.h>


void do_register_icons()
{
#ifdef _TMP
    GtkIconFactory * factory = gtk_icon_factory_new();
    GtkIconTheme *   theme = gtk_icon_theme_get_default( );

    gtk_icon_factory_add_default( factory );

    const char * name = DO_STOCK_DOMINO_APPS;

    if( !gtk_icon_theme_has_icon( theme, name ) )
    {
        int          width;
        GdkPixbuf *  p;

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_apps_48x48, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_apps_32x32, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_apps_16x16, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

    }

    name = DO_STOCK_DOMINO_CALENDAR;

    if( !gtk_icon_theme_has_icon( theme, name ) )
    {
        GtkIconSet * icon_set;
        int          width;
        GdkPixbuf *  p;
        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_calendar16x16, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        icon_set = gtk_icon_set_new_from_pixbuf( p );
        gtk_icon_factory_add( factory, name, icon_set );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_calendar_22x22, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        icon_set = gtk_icon_set_new_from_pixbuf( p );
        gtk_icon_factory_add( factory, name, icon_set );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_calendar24x24, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        icon_set = gtk_icon_set_new_from_pixbuf( p );
        gtk_icon_factory_add( factory, name, icon_set );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_calendar_32x32, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        icon_set = gtk_icon_set_new_from_pixbuf( p );
        gtk_icon_factory_add( factory, name, icon_set );
        g_object_unref( p );

    }

    name = DO_STOCK_DOMINO_CASH;

    if( !gtk_icon_theme_has_icon( theme, name ) )
    {
        int          width;
        GdkPixbuf *  p;
        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_cash_64x64, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );
    }

    name = DO_STOCK_DOMINO_GRLS;

    if( !gtk_icon_theme_has_icon( theme, name ) )
    {
        int          width;
        GdkPixbuf *  p;
        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_grls_64x64, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );
    }

    name = DO_STOCK_DOMINO_PROCESS;

    if( !gtk_icon_theme_has_icon( theme, name ) )
    {
        int          width;
        GdkPixbuf * p;
        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_process16, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_proc, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );
    }

    name = DO_STOCK_DOMINO_KEYBOARD;

    if( !gtk_icon_theme_has_icon( theme, name ) )
    {
        int          width;
        GdkPixbuf *  p;

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_keyboard_32x32, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_keyboard_16x16, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );
    }
    name = DO_STOCK_DOMINO_MAIL_SEND;

    if( !gtk_icon_theme_has_icon( theme, name ) )
    {
        int          width;
        GdkPixbuf *  p;

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_mail_send_16x16, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_mail_send_24x24, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_mail_send_48x48, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

    }

    name = DO_STOCK_DOMINO_MED;

    if( !gtk_icon_theme_has_icon( theme, name ) )
    {
        int          width;
        GdkPixbuf *  p;
        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_med_16x16, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_med_24x24, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );
    }

    name = DO_STOCK_DOMINO_WEB;

    if( !gtk_icon_theme_has_icon( theme, name ) )
    {
        int          width;
        GdkPixbuf *  p;
        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_web_16x16, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );
    }
    name = DO_STOCK_DOMINO_EVINCE;

    if( !gtk_icon_theme_has_icon( theme, name ) )
    {
        int          width;
        GdkPixbuf *  p;

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_evince_16x16, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_evince_32x32, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

        p = gdk_pixbuf_new_from_inline( -1, (guint8 *)&do_evince_64x64, FALSE, NULL );
        width = gdk_pixbuf_get_width( p );
        gtk_icon_theme_add_builtin_icon( name, width, p );
        g_object_unref( p );

    }

    g_object_unref ( G_OBJECT ( factory ) );
#endif
}
