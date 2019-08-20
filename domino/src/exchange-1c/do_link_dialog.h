
#ifndef DO_LINK_DIALOG_H
#define DO_LINK_DIALOG_H

#include <gtk/gtk.h>
#include <domino.h>
#include "exchange.h"

G_BEGIN_DECLS

#define DO_TYPE_LINK_DIALOG         (do_link_dialog_get_type ())
#define DO_LINK_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_LINK_DIALOG, DoLinkDialog))
#define DO_LINK_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_LINK_DIALOG, DoLinkDialogClass))
#define DO_IS_LINK_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_LINK_DIALOG))
#define DO_IS_LINK_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_LINK_DIALOG))
#define DO_LINK_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_LINK_DIALOG, DoLinkDialogClass))

GType do_exchange_handbook_get_type (void) G_GNUC_CONST;
#define DO_TYPE_EXCHANGE_HANDBOOK (do_exchange_handbook_get_type())


typedef struct _DoLinkDialog		 DoLinkDialog;
typedef struct _DoLinkDialogPrivate	 DoLinkDialogPrivate;
typedef struct _DoLinkDialogClass	 DoLinkDialogClass;

struct _DoLinkDialog
{
	GtkDialog parent_instance;
};

struct _DoLinkDialogClass
{
	GtkDialogClass parent_class;
};

GType		do_link_dialog_get_type(void);

GtkWidget  *do_link_dialog_new(do_alias_t *alias, GSList *objs_1c, DoExchangeHandbook handbook, partner_key0_t *partner_key);
gboolean    do_link_dialog_save(DoLinkDialog *dialog, gboolean empty);

G_END_DECLS

#endif /* !DO_LINK_DIALOG_H */
