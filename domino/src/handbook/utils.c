#include "utils.h"
#include <domino.h>
#include "messages.h"


static const gchar *error_skip[] =
{
#if GTK_MAJOR_VERSION > 2
    "gtk_device_grab_add: assertion",
    "gtk_device_grab_ungrab: assertion",
    "gtk_device_grab_remove: assertion",
#endif
};

static inline void log_gtk_func(const gchar *log_domain, GLogLevelFlags log_flags, const gchar *msg, gpointer user)
{
    gint i;
    for ( i = 0; i < G_N_ELEMENTS(error_skip); i++ )
        if ( !strncmp(error_skip[i], msg, strlen(error_skip[i])) )
            return;
    do_log(LOG_ERR, msg);
}

gboolean modules_init(int argc, char *argv[])
{
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    gtk_window_set_default_icon_name ("bookcase");

    message_init();
    do_log_set_log_func(message_log_do_func, NULL);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_CRITICAL, log_gtk_func, NULL);

    g_set_application_name("Справочник");


    if ( !domino_init(NULL, TRUE, DO_PARAM_END) ) {
        do_log(LOG_ERR, "Окружение домино  не инициализировано");
        return FALSE;
    }

    return TRUE;
}
