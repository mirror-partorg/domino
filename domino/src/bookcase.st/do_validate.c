
#include "do_validate.h"

static void do_validate_base_init (gpointer g_class)
{
}

GType do_validate_get_type (void)
{
        static GType type = 0;

        if (G_UNLIKELY (type == 0))
        {
                const GTypeInfo our_info =
                {
                        sizeof (DoValidateIface),
                        do_validate_base_init,
                        NULL,
                };

                type = g_type_register_static (G_TYPE_INTERFACE,
                                               "DoValidate",
                                               &our_info,
                                               (GTypeFlags)0);
        }
        return type;
}

gboolean do_validate_valid (DoValidate *validate)
{
        DoValidateIface *iface;

        iface = DO_VALIDATE_GET_IFACE (validate);
        if (iface->valid)
            return iface->valid(validate);
        return TRUE;
}
gboolean do_validate_substitution (DoValidate *validate)
{
        DoValidateIface *iface;

        iface = DO_VALIDATE_GET_IFACE (validate);
        if (iface->substitution)
            return iface->substitution(validate);
        return TRUE;
}
void do_validate_completion (DoValidate *validate)
{
        DoValidateIface *iface;

        iface = DO_VALIDATE_GET_IFACE (validate);
        if (iface->completion)
            iface->completion(validate);
}
