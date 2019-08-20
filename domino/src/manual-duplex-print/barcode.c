#include <stdio.h>
#include <gtk/gtk.h>
#include <cups/cups.h>

static GtkPrintSettings *print_settings;
static GtkPageSetup     *print_page_setup;

static gchar   *source_file = NULL;
static gchar   *printer = "Zebra";

//static void show_error(const gchar *format, ...);
static void show_error_message(const gchar *format, ...);

static GOptionEntry entries[] =
{
  { "file", 'f', 0, G_OPTION_ARG_STRING, &source_file, "Файл", "<файл>" },
  { "printer", 'p', 0, G_OPTION_ARG_STRING, &printer, "Принтер", "<принтер>" },
  { NULL }
};

static int print()
{
    gboolean retval;
    cups_option_t *options;
    options     = NULL;
    cupsAddOption("raw", "true", 0, &options);
    retval = cupsPrintFile(printer, source_file, "", 1, options);
    cupsFreeOptions(1, options);
    if ( !retval )
        show_error_message("Ошибка печати на \"%s\":%s", printer, cupsLastErrorString());
    g_timeout_add_seconds(10, (GSourceFunc)gtk_main_quit, NULL);
    return FALSE;
}


int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    GError *error = NULL;
    GOptionContext *context;

    context = g_option_context_new("- Печать штрихкодов");
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
