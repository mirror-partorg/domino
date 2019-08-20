#include "do_enum_types.h"

GType do_dialog_action_get_type (void)
{
    static GType etype = 0;
    if (G_UNLIKELY(etype == 0)) {
        static const GEnumValue values[] = {
            { DO_DIALOG_ACTION_NONE, "DO_DIALOG_ACTION_NONE", "none" },
            { DO_DIALOG_ACTION_INSERT, "DO_DIALOG_ACTION_INSERT", "insert" },
            { DO_DIALOG_ACTION_EDIT, "DO_DIALOG_ACTION_EDIT", "edit" },
            { DO_DIALOG_ACTION_DELETE, "DO_DIALOG_ACTION_DELETE", "delete" },
            { 0, NULL, NULL }
        };
        etype = g_enum_register_static (g_intern_static_string ("DoDialogAction"), values);
    }
    return etype;
}
