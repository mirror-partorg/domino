
#ifndef DO_PASSWORD_DIALOG_H
#define DO_PASSWORD_DIALOG_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_PASSWORD_DIALOG         (do_password_dialog_get_type ())
#define DO_PASSWORD_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_PASSWORD_DIALOG, DoPasswordDialog))
#define DO_PASSWORD_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_PASSWORD_DIALOG, DoPasswordDialogClass))
#define DO_IS_PASSWORD_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_PASSWORD_DIALOG))
#define DO_IS_PASSWORD_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_PASSWORD_DIALOG))
#define DO_PASSWORD_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_PASSWORD_DIALOG, DoPasswordDialogClass))

typedef struct _DoPasswordDialog		DoPasswordDialog;
typedef struct _DoPasswordDialogPrivate	DoPasswordDialogPrivate;
typedef struct _DoPasswordDialogClass	DoPasswordDialogClass;

typedef enum
{
	DO_PASSWORD_DIALOG_FLAGS_SHOW_USERNAME	= 1 << 0,
	DO_PASSWORD_DIALOG_FLAGS_EDIT_USERNAME	= 1 << 1,
	DO_PASSWORD_DIALOG_FLAGS_SHOW_DOMAIN	= 1 << 2,
	DO_PASSWORD_DIALOG_FLAGS_EDIT_DOMAIN	= 1 << 3,
	DO_PASSWORD_DIALOG_FLAGS_SHOW_PASSWORD	= 1 << 4

} DoPasswordDialogFlags;

struct _DoPasswordDialog
{
	GtkMessageDialog parent_instance;
	DoPasswordDialogPrivate *priv;
};

struct _DoPasswordDialogClass
{
	GtkMessageDialogClass parent_class;
};

#define DO_PASSWORD_DIALOG_FLAGS_DEFAULT DO_PASSWORD_DIALOG_FLAGS_SHOW_USERNAME	| DO_PASSWORD_DIALOG_FLAGS_EDIT_USERNAME | DO_PASSWORD_DIALOG_FLAGS_SHOW_PASSWORD

GType		do_password_dialog_get_type(void);

GtkWidget  *do_password_dialog_new(GtkWidget *parent, const char *title, DoPasswordDialogFlags flags);

void		do_password_dialog_set_label(DoPasswordDialog *dialog, const char *markup);

const char *do_password_dialog_get_username(DoPasswordDialog *dialog);

void		do_password_dialog_set_username(DoPasswordDialog *dialog, const char *text);

const char *do_password_dialog_get_domain(DoPasswordDialog *dialog);

void		do_password_dialog_set_domain(DoPasswordDialog *dialog, const char *text);

const char *do_password_dialog_get_password(DoPasswordDialog *dialog);

void		do_password_dialog_set_password(DoPasswordDialog *dialog, const char *text);


G_END_DECLS

#endif /* !DO_PASSWORD_DIALOG_H */
