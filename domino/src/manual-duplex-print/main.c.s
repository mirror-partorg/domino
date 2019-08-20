#include <stdio.h>
#include <gtk/gtk.h>
//#include <evince-view.h>
//#include <gtk/gtkunixprint.h>
#include <poppler.h>
#include <cups/cups.h>

static GtkPrintSettings *print_settings;
static GtkPageSetup     *print_page_setup;

static gboolean default_printing = FALSE;
static gboolean print_duplex = FALSE;
static gboolean manual_duplex = FALSE;
static gchar   *source_file = NULL;
static gboolean duplex = FALSE;
static gboolean reverse = 0;


//static void show_error(const gchar *format, ...);
static void show_error_message(const gchar *format, ...);

static GOptionEntry entries[] =
{
  { "file", 'f', 0, G_OPTION_ARG_STRING, &source_file, "Файл", "<файл>" },
  { "default", 'p', 0, G_OPTION_ARG_NONE, &default_printing, "Печать по умолчанию", NULL },
  { "duplex", 'D', 0, G_OPTION_ARG_NONE, &print_duplex, "Двусторонняя печать", NULL },
  { "manual_duplex", 'M', 0, G_OPTION_ARG_NONE, &manual_duplex, "Принтер без дуплекса (принудительно)", NULL },
  { NULL }
};

static gboolean printer_can_do_duplexing(const char *name)
{
    cups_dest_t *dest;
	cups_dest_t *dests;
	int num_dests, i;

	num_dests = cupsGetDests(&dests);
    dest = cupsGetDest(name, NULL, num_dests, dests);

    if ( !dest ) return FALSE;

    for ( i = 0; i < dest->num_options; i++ )
        if ( !g_strcmp0(dest->options[i].name, "printer-type") ) {
            int ptype;
            ptype = atoi(dest->options[i].value);
            return ptype & CUPS_PRINTER_DUPLEX;
        }
    return FALSE;
}

static void print_begin_print(GtkPrintOperation *operation,
                                                        GtkPrintContext   *context,
                                                        PopplerDocument *doc)
{
    GtkPrintSettings *settings;
    //GtkPrintDuplex duplex;
    GtkPrintPages pages;
    gint n_pages;

    n_pages = poppler_document_get_n_pages(doc);
    settings = gtk_print_operation_get_print_settings(operation);
    //duplex = gtk_print_settings_get_duplex(print_settings);
    pages = gtk_print_settings_get_print_pages (print_settings);

	if ( print_duplex &&
	     pages == GTK_PRINT_PAGES_ALL &&
	     n_pages > 0 &&
	     ( !printer_can_do_duplexing(gtk_print_settings_get_printer(settings)) || manual_duplex )
       ) {
       duplex = TRUE;
	}
	else
        exit(1);//!!
    gtk_print_operation_set_n_pages(operation, n_pages);
}
static void print_done(GtkPrintOperation      *operation,
                                                        GtkPrintOperationResult result,
                                                        gpointer                user_data)
{
    g_timeout_add_seconds(10, (GSourceFunc)gtk_main_quit, NULL);
}
static void print_draw_page(GtkPrintOperation *operation,
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
    gint n_pages;
    n_pages = poppler_document_get_n_pages(doc);

    if ( page_nr >= n_pages ) {
        cairo = gtk_print_context_get_cairo_context(context);
        cairo_move_to (cairo, 20, 10);
        return;
    }
    if ( duplex && ( page_nr % 2 == !reverse )  ) {
        return;
    }

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
static void print_status_changed(GtkPrintOperation *operation,
                                                        PopplerDocument *doc)
{
}
static void print_request_page_setup(GtkPrintOperation *operation,
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

GtkPageSetup *get_default_page_setup()
{
    GtkPageSetup *page;
    GtkPaperSize *paper_size;

    page = gtk_page_setup_new();
    paper_size = gtk_paper_size_new(GTK_PAPER_NAME_A4);

    gdouble lm=10,tm=10,rm=10,bm=10;
    gtk_page_setup_set_orientation(page, GTK_PAGE_ORIENTATION_PORTRAIT);
    gtk_page_setup_set_paper_size(page, paper_size);
    gtk_page_setup_set_top_margin(page, tm, GTK_UNIT_MM);
    gtk_page_setup_set_left_margin(page, lm, GTK_UNIT_MM);
    gtk_page_setup_set_bottom_margin(page, bm, GTK_UNIT_MM);
    gtk_page_setup_set_right_margin(page, rm, GTK_UNIT_MM);

    return page;

}

void print_pdf_file()
{
    GtkPrintOperation *op;
    PopplerDocument *doc;
    GError *error = NULL;
    GFile  *file;
    gchar  *uri;
    gboolean res;
	GtkPrintOperationAction action = GTK_PRINT_OPERATION_ACTION_PRINT;

    file = g_file_new_for_commandline_arg (source_file);
    uri = g_file_get_uri (file);
    g_object_unref (file);

    doc = poppler_document_new_from_file(uri, NULL, &error);

    if ( !doc ) {
        if ( error )
            show_error_message("Ошибка чтения файла %s: %s", uri, error->message);
        else
            show_error_message("Ошибка чтения файла %s", uri);
        exit(1);
    }

    op = gtk_print_operation_new();

	g_signal_connect (op, "begin_print",
				  G_CALLBACK (print_begin_print),
				  doc);
	g_signal_connect (op, "done",
				  G_CALLBACK (print_done),
				  doc);
	g_signal_connect (op, "draw_page",
				  G_CALLBACK (print_draw_page),
				  doc);
	g_signal_connect (op, "status_changed",
				  G_CALLBACK (print_status_changed),
				  doc);
	g_signal_connect (op, "request_page_setup",
				  G_CALLBACK (print_request_page_setup),
				  doc);
	gtk_print_operation_set_allow_async (op, FALSE);
	gtk_print_operation_set_unit (op, GTK_UNIT_MM);
	gtk_print_operation_set_custom_tab_label (op, "Печать...");


    if ( !default_printing || !cupsGetDefault() )
        action = GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG;

    gtk_print_operation_set_print_settings(op, print_settings);
    gtk_print_operation_set_default_page_setup(op, print_page_setup);

    res = gtk_print_operation_run(op, action , NULL, &error);

    if ( res && duplex ) {
        print_settings = gtk_print_operation_get_print_settings(op);
        g_object_ref(print_settings);
        g_object_unref(op);

        op = gtk_print_operation_new();

        g_signal_connect (op, "begin_print",
                      G_CALLBACK (print_begin_print),
                      doc);
        g_signal_connect (op, "done",
                      G_CALLBACK (print_done),
                      doc);
        g_signal_connect (op, "draw_page",
                      G_CALLBACK (print_draw_page),
                      doc);
        g_signal_connect (op, "status_changed",
                      G_CALLBACK (print_status_changed),
                      doc);
        g_signal_connect (op, "request_page_setup",
                      G_CALLBACK (print_request_page_setup),
                      doc);
        gtk_print_operation_set_allow_async (op, FALSE);
        gtk_print_operation_set_unit (op, GTK_UNIT_MM);
        gtk_print_operation_set_custom_tab_label (op, "Печать...");
        gtk_print_operation_set_print_settings(op, print_settings);
        gtk_print_operation_set_default_page_setup(op, print_page_setup);


        reverse = TRUE;
        GtkPageRange range;
        int i,n_pages,count;
        n_pages = poppler_document_get_n_pages(doc);
        gtk_print_settings_set_print_pages (print_settings, GTK_PRINT_PAGES_RANGES);
        if ( n_pages % 2 != 0 )
            n_pages++;
        for ( count=1,i = n_pages; i > 0; i-=2,count++ ) {
            range.start = i;
            range.end = i;
            gtk_print_settings_set_page_ranges (print_settings, &range, count);
        }
        gtk_print_operation_set_print_settings(op, print_settings);
        gtk_print_operation_set_default_page_setup(op, print_page_setup);

        res = gtk_print_operation_run(op, GTK_PRINT_OPERATION_ACTION_PRINT, NULL, &error);
    }
    if ( !res ) {
        if ( error )
            show_error_message("Невозможно напечатать файл \"%s\":%s", uri, error->message);
        else
            show_error_message("Невозможно напечатать файл \"%s\"", uri);
        exit(1);
    }
    g_object_unref(op);

    g_timeout_add_seconds(10, (GSourceFunc)gtk_main_quit, NULL);
}

int print()
{
    print_settings = gtk_print_settings_new ();
    //print_page_setup = gtk_page_setup_new ();
    print_page_setup = get_default_page_setup();
    /*{
        GtkPageRange range;
        range.start = 1;
        range.end = 2;
        gtk_print_settings_set_print_pages (print_settings, GTK_PRINT_PAGES_RANGES);
        gtk_print_settings_set_page_ranges (print_settings,&range, 1);
    }*/
    if ( print_duplex && !manual_duplex )
        gtk_print_settings_set_duplex(print_settings, GTK_PRINT_DUPLEX_VERTICAL);
    else
        gtk_print_settings_set_duplex(print_settings, GTK_PRINT_DUPLEX_SIMPLEX);
    if ( default_printing )
        gtk_print_settings_set_printer(print_settings, cupsGetDefault());
    print_pdf_file();

    return FALSE;
}

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    GError *error = NULL;
    GOptionContext *context;

    context = g_option_context_new("- Печать");
    g_option_context_add_main_entries (context, entries, NULL);
    g_option_context_add_group (context, gtk_get_option_group (FALSE));
    g_option_context_set_help_enabled(context, FALSE);
    if ( !g_option_context_parse (context, &argc, &argv, &error) )  {
        show_error_message("Ошибка опций командной строки: %s\nИспользование: %s", error->message, g_option_context_get_help (context, TRUE, NULL));
        exit(1);
    }
    if ( !source_file ) {
        gchar *sad;
        sad = g_option_context_get_help (context, TRUE, NULL);
        show_error_message("Файл неопределен\nИспользование: %s", sad);
        exit(1);
    }
    //ev_init();
    g_timeout_add_seconds(2, print, NULL);
    gtk_main();
    return 0;
}
static void show_error_(gboolean show_exit, const gchar *messages);
static void show_error_message(const gchar *format, ...)
{
    gchar *message;
    va_list args;
    va_start (args, format);
    message = g_strdup_vprintf (format, args);
    va_end (args);
    show_error_(FALSE, message);
    g_free(message);

}
/*
static void show_error(const gchar *format, ...)
{
    gchar *message;
    va_list args;
    va_start (args, format);
    message = g_strdup_vprintf (format, args);
    va_end (args);
    show_error_(TRUE, message);
    g_free(message);

}*/
static void show_error_(gboolean show_exit, const gchar *message)
{
    gint res;
    GtkWidget *dialog;
    if ( show_exit )
        dialog = gtk_message_dialog_new (NULL,
                          GTK_DIALOG_DESTROY_WITH_PARENT,
                          GTK_MESSAGE_ERROR,
                          GTK_BUTTONS_YES_NO,
                          "%s\nЗавершить работу.", message);
    else
        dialog = gtk_message_dialog_new (NULL,
                          GTK_DIALOG_DESTROY_WITH_PARENT,
                          GTK_MESSAGE_ERROR,
                          GTK_BUTTONS_OK,
                          "%s", message);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_icon_name(GTK_WINDOW(dialog), "bookcase");
    gtk_window_set_title(GTK_WINDOW(dialog), "Ошибка");
    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if ( show_exit && res == GTK_RESPONSE_YES )
        exit(1);
    gtk_widget_destroy (dialog);
}
