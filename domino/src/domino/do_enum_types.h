#ifndef DO_ENUM_TYPES_H_INCLUDED
#define DO_ENUM_TYPES_H_INCLUDED

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum
{
	DO_DIALOG_ACTION_NONE,
	DO_DIALOG_ACTION_INSERT,
	DO_DIALOG_ACTION_EDIT,
	DO_DIALOG_ACTION_DELETE
} DoDialogAction;

GType do_dialog_action_get_type (void) G_GNUC_CONST;
#define DO_TYPE_DIALOG_ACTION (do_dialog_action_get_type ())

enum
{
    DO_GUI_PAD_SMALL = 3,
    DO_GUI_PAD = 6,
    DO_GUI_PAD_BIG = 12,
    DO_GUI_PAD_LARGE = 12
};


G_END_DECLS


#endif // DO_DIALOG_H_INCLUDED
