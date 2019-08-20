
#include "do_print.h"

static void do_print_base_init (gpointer g_class);

GType do_print_get_type (void)
{
        static GType type = 0;

        if (G_UNLIKELY (type == 0))
        {
                const GTypeInfo our_info =
                {
                        sizeof (DoPrintIface),
                        do_print_base_init,
                        NULL,
                };

                type = g_type_register_static (G_TYPE_INTERFACE,
                                               "DoPrint",
                                               &our_info,
                                               (GTypeFlags)0);
        }
        return type;
}
static void do_print_base_init (gpointer g_class)
{
        static gboolean initialized = FALSE;

        if (!initialized)
        {
                initialized = TRUE;
        }
}

gchar *do_print_get_html(DoPrint *print)
{
        DoPrintIface *iface;
        iface = DO_PRINT_GET_IFACE (print);
        if (iface->get_html)
            return iface->get_html(print);
        return NULL;
}
/*
void do_print_set_title(DoPrint *print, const gchar *title)
{
        DoPrintIface *iface;

        iface = DO_PRINT_GET_IFACE (print);
        if (iface->set_title)
            iface->set_title(print, title);
}
void do_print_set_footer(DoPrint *print, const gchar *footer)
{
        DoPrintIface *iface;

        iface = DO_PRINT_GET_IFACE (print);
        if (iface->set_title)
            iface->set_title(print, footer);
}
*/
