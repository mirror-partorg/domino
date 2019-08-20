
#ifndef DO_RZ_EDIT_H
#define DO_RZ_EDIT_H

#include <gtk/gtk.h>
#include "do_enum_types.h"
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_RZ_EDIT         (do_rz_edit_get_type ())
#define DO_RZ_EDIT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_RZ_EDIT, DoRzEdit))
#define DO_RZ_EDIT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_RZ_EDIT, DoRzEditClass))
#define DO_IS_RZ_EDIT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_RZ_EDIT))
#define DO_IS_RZ_EDIT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_RZ_EDIT))
#define DO_RZ_EDIT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_RZ_EDIT, DoRzEditClass))

typedef struct _DoRzEdit		DoRzEdit;
typedef struct _DoRzEditPrivate	DoRzEditPrivate;
typedef struct _DoRzEditClass	DoRzEditClass;

typedef enum
{
	DO_RZ_EDIT_FLAGS_READ_ONLY	= 1 << 0,
	DO_RZ_EDIT_FLAGS_LINK_ONLY	= 1 << 1,

} DoRzEditFlags;

struct _DoRzEdit
{
	GtkVBox parent_instance;
};

struct _DoRzEditClass
{
	GtkVBoxClass parent_class;
};

#define DO_RZ_EDIT_FLAGS_DEFAULT 0

GType		do_rz_edit_get_type(void);

GtkWidget  *do_rz_edit_new(GtkWidget *parent, const gchar *alias_name, DoRzEditFlags flags, DoDialogAction action, DoContext *context);

gboolean    do_rz_edit_accept(DoRzEdit *dialog);
gboolean    do_rz_edit_close(DoRzEdit *edit);
G_CONST_RETURN gchar *do_rz_edit_get_title(DoRzEdit *edit);
void        do_rz_edit_grab_focus(DoRzEdit *edit);
void        do_rz_edit_show_product(DoRzEdit *edit, const gchar *tab);
void        do_rz_edit_ck_link(DoRzEdit *edit);


G_END_DECLS

#endif /* !DO_RZ_EDIT_H */
