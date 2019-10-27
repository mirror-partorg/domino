#include <gtk/gtk.h>
#include "version.h"
#include "do_application.h"


int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;
	app = g_object_new(DO_TYPE_APPLICATION,
                        "application-id", "com.glekar.bookcase",
	                    NULL);
    g_set_prgname("com.glekar.bookcase");
    status = g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app);
    return status;
}
