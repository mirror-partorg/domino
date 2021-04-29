#include <gtk/gtk.h>
#include "version.h"
#include "do_application.h"
#include <dolib.h>

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;
    do_log_set_type(APPLICATION_ID, DO_LOG_TYPE_SYSLOG, NULL);
    //do_log_set_type(APPLICATION_ID, DO_LOG_TYPE_FILE, "c:/temp/bookcase.log");
	app = g_object_new(DO_TYPE_APPLICATION,
                        "application-id", APPLICATION_ID,
	                    NULL);
    g_set_prgname(APPLICATION_ID);
    g_log_set_handler (NULL, G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, do_application_log_func, NULL);
    status = g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app);
    return status;
}
