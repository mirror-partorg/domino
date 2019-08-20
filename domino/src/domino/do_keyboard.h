
#ifndef DO_KEYBOARD_H
#define DO_KEYBOARD_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_KEYBOARD         (do_keyboard_get_type ())
#define DO_KEYBOARD(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_KEYBOARD, DoKeyboard))
#define DO_KEYBOARD_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_KEYBOARD, DoKeyboardClass))
#define DO_IS_KEYBOARD(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_KEYBOARD))
#define DO_IS_KEYBOARD_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_KEYBOARD))
#define DO_KEYBOARD_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_KEYBOARD, DoKeyboardClass))

typedef struct _DoKeyboard		DoKeyboard;
typedef struct _DoKeyboardPrivate	DoKeyboardPrivate;
typedef struct _DoKeyboardClass		DoKeyboardClass;

typedef enum
{
	DO_KEYBOARD_FLAGS_	= 1 << 0,

} DoKeyboardFlags;

struct _DoKeyboard
{
	GtkHPaned parent_instance;
};

struct _DoKeyboardClass
{
	GtkHPanedClass parent_class;
};

#define DO_KEYBOARD_FLAGS_DEFAULT 0

GType		do_keyboard_get_type(void);

GtkWidget  *do_keyboard_new(const gchar *alias_name);

G_END_DECLS

#endif /* !DO_KEYBOARD_H */
