#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <iconv.h>
#include <string.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libgnumeric.h>
#include "do_report_plugin.h"
#include "do_progress_info.h"

#define ADD_CHILD(c,a,b) xmlNewChild(c, NULL, BAD_CAST a, BAD_CAST b)

#define SHOW_ERROR(...) { gchar *v = g_strdup_printf(__VA_ARGS__); show_error(v); g_free(v);}

static gboolean show_error(gchar *msg)
{
    fprintf(stderr, "%s\n", msg);
    GtkWidget *dialog = gtk_message_dialog_new (NULL,
                          GTK_DIALOG_DESTROY_WITH_PARENT,
                          GTK_MESSAGE_ERROR,
                          GTK_BUTTONS_YES_NO,
                          "%s\nЗавершить работу.", msg);
    g_free(msg);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ALWAYS);
    //gtk_window_set_icon_name(GTK_WINDOW(dialog), DO_STOCK_DOMINO_APPS);
    gtk_window_set_title(GTK_WINDOW(dialog), "Ошибка");
    if ( gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES )
        exit(1);
    gtk_widget_destroy (dialog);
    return FALSE;
}

int do_gtk_log(const char *str)
{
    gchar *utf8;
    utf8 = g_strdup(str);//g_locale_to_utf8(str, -1, NULL, NULL, NULL);
    gdk_threads_add_idle((GSourceFunc)show_error, utf8);
    return TRUE;
}
static gchar *template_ = NULL;
static gchar *encoding = "CP866";
static gchar *file_ = NULL;
static gchar *title_ = NULL;
static gboolean readonly=FALSE;
static gboolean debug=FALSE;

static const GOptionEntry options [] =
{
    { "template", 't', 0, G_OPTION_ARG_STRING, &template_,
      "Файл шаблона элетронной таблицы", "Шаблон"},

    { "file", 'f', 0, G_OPTION_ARG_STRING, &file_,
      "Файл XML с данными", "Данные"},

    { "title", 's', 0, G_OPTION_ARG_STRING, &title_,
      "Наименование отчета", "Наименование"},

    { "notreadonly", 'r', 0, G_OPTION_ARG_NONE, &readonly,
      "Снять защиту с листов", "Снять защиту"},

    { "debug", 'd', 0, G_OPTION_ARG_NONE, &debug,
      "Отладка", "Отладка"},

    { "encoding", 'e', 0, G_OPTION_ARG_STRING, &encoding,
      "Кодовая страница", "Кодовая страница"},

    {NULL}
};

DoProgressInfo *progress;

static gboolean report_complect(DoReportPlugin *plugin)
{
    GtkWindow *win;
    win = do_report_plugin_show(plugin);
    g_signal_connect(win, "destroy", gtk_main_quit, plugin);
    do_progress_info_finish(progress);


    if ( !do_report_plugin_is_done(plugin) ) {
        do_gtk_log("Отчет не выполнен");
    }
    return FALSE;
}
static char* iconv_with_error(iconv_t conv, const char *str)
{
    if (conv == (iconv_t)-1)
        return g_strdup(str);
    size_t result;
    size_t len = strlen(str);
    char ansb[len];
    char* ansa = (char*) g_malloc(len * 2 + 1);
    char *ansbptr = ansb;
    char *ansaptr = ansa;
    size_t len1 = 2 * len;
    size_t length = len1;

    strncpy(ansb, str, len);
    result = iconv(conv, (char**)&ansbptr, &len, &ansaptr, &len1);
    if (result == (size_t) - 1)
    {
        g_free(ansa);
        return NULL;
    }
    else
	  ansa[length-len1] = '\0';
    return ansa;
}
static char *read_file(const char *filename, const char *encoding)
{
    FILE *fp;
    char *retval;
    if ((fp = fopen(filename, "r")) == NULL)  {
        SHOW_ERROR("Error opening file (%s): %s", filename, strerror(errno));
        return NULL;
    }
    fseek (fp, 0L, SEEK_END);
    long len = ftell (fp), l;
    if ((retval = g_malloc(len + 1)) == NULL && len) {
        SHOW_ERROR("Could not allocate memory %d byte", (int)len);
        return NULL;
    }
    l = fseek (fp, 0L, SEEK_SET);
    errno = 0;
    l = fread(retval, 1, len, fp);
    if (errno) {
        SHOW_ERROR( "Error read file (%s): %s", filename, strerror(errno));
        fclose(fp);
        g_free(retval);
        return NULL;
    }
    retval[l] = '\0';
    fclose(fp);
    iconv_t conv = iconv_open("UTF-8", encoding);
    if ( conv == (iconv_t)-1 )
        return retval;

    char *ret;
    ret = iconv_with_error(conv, retval);
    if ( ret ) {
        g_free(retval);
        retval = ret;
    }
    iconv_close(conv);


    return retval;
}
int main(int argc, gchar *argv[])
{

    setlocale(LC_ALL, "");
    DoReportPlugin *plugin;

    xmlDoc  *doc;
    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    gnm_pre_parse_init (argc, (gchar const **)argv);


    GOptionContext *context;
    context = g_option_context_new(" - Домино");
    g_option_context_add_main_entries (context, options, NULL);
    g_option_context_add_group (context, gtk_get_option_group (FALSE));
    g_option_context_set_help_enabled(context, FALSE);
    GError *error = NULL;

//    update_action_set_arg(argv, argc);

    if ( !g_option_context_parse (context, &argc, &argv, &error) || !file_ || !template_ ) {

	gchar *msg = g_strdup_printf("%s\n%s",
                        (error) ? error->message : "", g_option_context_get_help (context, TRUE, NULL));
        show_error(msg);
        return 1;
    }

    doc = NULL;
    plugin = do_report_plugin_new(argv[0]);
    if ( g_strcmp0(encoding, "UTF-8") ) {
        char *buffer;
        buffer = read_file(file_, encoding);
        if ( buffer )
            doc = xmlReadMemory(buffer, strlen(buffer), NULL, NULL, 0);
    }
    else
        doc = xmlReadFile(file_, NULL, 0);
    if ( !doc ) {
        xmlErrorPtr x_error = 0;
        x_error = xmlGetLastError();
        show_error(g_strdup_printf("Файл \"%s\" не распознается.\n%s", file_, x_error->message));
        return 1;
    }

    progress = do_progress_info_new();
    do_progress_info_start(progress);


    g_object_set(plugin, "template-filename", template_, "values", doc, "progress-info", progress, "debug", debug,
                   "title", title_, "readonly", readonly,
                 NULL);

    do_report_plugin_make_in_thread(plugin, (GSourceFunc)report_complect, plugin);
    //g_idle_add((GSourceFunc)make, plugin);
    gtk_main ();


    g_object_unref(plugin);

    //gnm_pre_parse_shutdown ();


    return 0;

}
