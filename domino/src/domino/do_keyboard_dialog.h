
#ifndef DO_KEYBOARD_DIALOG_H
#define DO_KEYBOARD_DIALOG_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_KEYBOARD_DIALOG         (do_keyboard_dialog_get_type ())
#define DO_KEYBOARD_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_KEYBOARD_DIALOG, DoKeyboardDialog))
#define DO_KEYBOARD_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_KEYBOARD_DIALOG, DoKeyboardDialogClass))
#define DO_IS_KEYBOARD_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_KEYBOARD_DIALOG))
#define DO_IS_KEYBOARD_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_KEYBOARD_DIALOG))
#define DO_KEYBOARD_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_KEYBOARD_DIALOG, DoKeyboardDialogClass))

typedef struct _DoKeyboardDialog		DoKeyboardDialog;
typedef struct _DoKeyboardDialogPrivate	DoKeyboardDialogPrivate;
typedef struct _DoKeyboardDialogClass	DoKeyboardDialogClass;

typedef enum
{
	DO_KEYBOARD_DIALOG_FLAGS_DISABLE_FUNCTION_KEYS	= 1 << 0,

} DoKeyboardDialogFlags;

struct _DoKeyboardDialog
{
	GtkDialog parent_instance;
};

struct _DoKeyboardDialogClass
{
	GtkDialogClass parent_class;
};

#define DO_KEYBOARD_DIALOG_FLAGS_DEFAULT 0

GType		do_keyboard_dialog_get_type(void);

GtkWidget  *do_keyboard_dialog_new(GtkWidget *parent, const gchar *alias_name, DoKeyboardDialogFlags flags, DoDialogAction action, DoContext *context);

gint        do_keyboard_dialog_run(DoKeyboardDialog *dialog);

G_END_DECLS

#endif /* !DO_KEYBOARD_DIALOG_H */
