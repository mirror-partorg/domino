/*
 * This file Copyright (C) 2007-2009 Charles Kerr <charles@transmissionbt.com>
 *
 * This file is licensed by the GPL version 2.  Works owned by the
 * Transmission project are granted a special exemption to clause 2(b)
 * so that the bulk of its code can remain under the MIT license.
 * This exemption does not extend to derived works not owned by
 * the Transmission project.
 *
 * $Id: hig.c 8097 2009-03-28 16:47:01Z charles $
 */

#include <gtk/gtk.h>
#include "hig.h"

GtkWidget*
hig_workarea_create( void )
{
    GtkWidget * t = gtk_grid_new();
    gtk_grid_set_column_spacing( GTK_GRID( t ), GUI_PAD_BIG );
    gtk_grid_set_row_spacing( GTK_GRID( t ), GUI_PAD );
    gtk_grid_set_column_homogeneous(GTK_GRID(t), FALSE);
    gtk_container_set_border_width( GTK_CONTAINER( t ), GUI_PAD_BIG );
    return t;
}


void
hig_workarea_add_section_title_widget( GtkWidget * t,
                                       int *       row,
                                       GtkWidget * w )
{
    gtk_grid_attach( GTK_GRID( t ), w, 0, *row, 2, 1);
    ++ * row;
}

GtkWidget *
hig_workarea_add_section_title( GtkWidget *  t,
                                int *        row,
                                const char * section_title )
{
    char        buf[512];
    GtkWidget * l;

    g_snprintf( buf, sizeof( buf ), "<b>%s</b>", section_title );
    l = gtk_label_new( buf );
    //!!g_object_set(l, "selectable", TRUE, NULL);
	g_object_set(G_OBJECT(l), "halign", 0.0, "valign", 0.5, NULL);
    gtk_label_set_use_markup( GTK_LABEL( l ), TRUE );
    hig_workarea_add_section_title_widget( t, row, l );
    return l;
}

static GtkWidget*
rowNew( GtkWidget * w )
{

    GtkWidget * h;
    h = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

#if GTK_CHECK_VERSION(3,14,0)
    g_object_set(G_OBJECT(w), "margin-left", 18, NULL);
    /* lhs widget */
    if( GTK_IS_LABEL( w ) )
        g_object_set(G_OBJECT(w), "halign", 0.0, "valign", 0.5, NULL);
#else
    GtkWidget * a;
    /* spacer */
    a = gtk_alignment_new( 0.0f, 0.0f, 0.0f, 0.0f );
    gtk_widget_set_size_request( a, 18u, 0u );
    gtk_box_pack_start( GTK_BOX( h ), a, FALSE, FALSE, 0 );
    /* lhs widget */
    if( GTK_IS_MISC( w ) )
        gtk_misc_set_alignment( GTK_MISC( w ), 0.0f, 0.5f );
#endif
    if( GTK_IS_LABEL( w ) )
        gtk_label_set_use_markup( GTK_LABEL( w ), TRUE );
    gtk_box_pack_start( GTK_BOX( h ), w, TRUE, TRUE, 0 );

    return h;
}

void
hig_workarea_add_wide_control( GtkWidget * t,
                               int *       row,
                               GtkWidget * w )
{
    GtkWidget * r = rowNew( w );

    gtk_grid_attach( GTK_GRID( t ), r, 0, *row, 2, 1);
    ++ * row;
}

GtkWidget *
hig_workarea_add_wide_checkbutton( GtkWidget *  t,
                                   int *        row,
                                   const char * mnemonic_string,
                                   gboolean     is_active )
{
    GtkWidget * w = gtk_check_button_new_with_mnemonic( mnemonic_string );

    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( w ), is_active );
    hig_workarea_add_wide_control( t, row, w );
    return w;
}

void
hig_workarea_add_label_w( GtkWidget * t,
                          int         row,
                          GtkWidget * l )
{
    GtkWidget * w = rowNew( l );

    gtk_grid_attach( GTK_GRID( t ), w, 0, row, 1,1);
}

GtkWidget*
hig_workarea_add_label( GtkWidget *  t,
                        int          row,
                        const char * mnemonic_string )
{
    GtkWidget * l = gtk_label_new_with_mnemonic( mnemonic_string );

    hig_workarea_add_label_w( t, row, l );
    return l;
}

void
hig_workarea_add_control( GtkWidget * t,
                          int         row,
                          GtkWidget * control )
{
    if( GTK_IS_LABEL(control) )
        g_object_set(G_OBJECT(control), "halign", 0.0, "valign", 0.5, NULL);

    gtk_grid_attach( GTK_GRID( t ), control, 1, row, 2, 1);
}

void
hig_workarea_add_row_w( GtkWidget * t,
                        int *       row,
                        GtkWidget * label,
                        GtkWidget * control,
                        GtkWidget * mnemonic )
{
    hig_workarea_add_label_w( t, *row, label );
    hig_workarea_add_control( t, *row, control );
    if( GTK_IS_LABEL( label ) )
        gtk_label_set_mnemonic_widget( GTK_LABEL( label ),
                                       mnemonic ? mnemonic : control );
    ++ * row;
}

GtkWidget*
hig_workarea_add_row( GtkWidget *  t,
                      int *        row,
                      const char * mnemonic_string,
                      GtkWidget *  control,
                      GtkWidget *  mnemonic )
{
    GtkWidget * l = gtk_label_new_with_mnemonic( mnemonic_string );

    hig_workarea_add_row_w( t, row, l, control, mnemonic );
    return l;
}

void
hig_workarea_finish( GtkWidget * t,
                     int *       row )
{
#if GTK_MAJOR_VERION == 2 // to do
    gtk_table_resize( GTK_TABLE( t ), *row, 2 );
#endif
}

void
hig_message_dialog_set_text( GtkMessageDialog * dialog,
                             const char *       primary,
                             const char *       secondary )
{
    gtk_message_dialog_set_markup ( dialog, primary );
    gtk_message_dialog_format_secondary_text ( dialog, "%s", secondary );
}

