#ifndef DO_VIEW_H
#define DO_VIEW_H

#include <glib-object.h>
#include <glib.h>
#include "do_context.h"

G_BEGIN_DECLS

#define DO_TYPE_VIEW		(do_view_get_type ())
#define DO_VIEW(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_VIEW, DoView))
#define DO_VIEW_IFACE(k)		(G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_VIEW, DoViewIface))
#define DO_IS_VIEW(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_VIEW))
#define DO_IS_VIEW_IFACE(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_VIEW))
#define DO_VIEW_GET_IFACE(inst)	(G_TYPE_INSTANCE_GET_INTERFACE ((inst), DO_TYPE_VIEW, DoViewIface))

#define DO_VIEW_ICON_SIZE 16

typedef struct _DoView	DoView;
typedef struct _DoViewIface	DoViewIface;
//typedef enum _DoMarked DoMarked;


typedef enum
{
    DO_MARKED_UNMARK = 0,
    DO_MARKED = 1,
    DO_MARKED_AND_ACCEPT = 2,
    DO_MARKED_AND_REJECT = 3,
} DoMarked;



struct _DoViewIface
{
	GTypeInterface base_iface;
	gboolean        (* close_request) (DoView *view);

	void		(* do_delete)	  (DoView *view);
	gboolean	(* can_do_delete) (DoView *view);

	void		(* do_insert)	  (DoView *view);
	gboolean	(* can_do_insert) (DoView *view);

	void		(* do_copy)  	  (DoView *view);
	gboolean	(* can_do_copy)	  (DoView *view);

	void		(* do_edit)	      (DoView *view, const gchar *tab);
	gboolean	(* can_do_edit)   (DoView *view, const gchar *tab);

	void		(* do_apply)	   (DoView *view);
	gboolean	(* can_do_apply)   (DoView *view);

	void		(* do_unapply)	  (DoView *view);
	gboolean	(* can_do_unapply) (DoView *view);

	void		(* do_mail_send)	   (DoView *view);
	gboolean	(* can_do_mail_send)   (DoView *view);


	void		(* do_popup)	  (DoView *view, GdkEventButton* event);
	gboolean    (* do_grab_focus) (DoView *view);

	void		(* do_print)	  (DoView *view);
	void		(* do_print_now)	 (DoView *view);
	gboolean    (* can_do_print)  (DoView *view);

	void		(* do_open)  	  (DoView *view);
	gboolean    (* can_do_open)   (DoView *view);
	void		(* do_save)  	  (DoView *view);
	void		(* do_save_as)	  (DoView *view);
	gboolean    (* can_do_save)    (DoView *view);
	gboolean    (* can_do_save_as)  (DoView *view);

	void	    (* do_close)	     (DoView *view);
	const char *(* get_title)        (DoView *view);
	GdkPixbuf  *(* get_icon)	     (DoView *view);
	const gchar*(* get_load_status)  (DoView *view);
	gint     	(* get_load_progress)  (DoView *view);

	DoMarked	(* get_marked)          (DoView *view, DoContext *context);
	gboolean  	(* set_marked)          (DoView *view, DoContext *context, DoMarked marked);
	gboolean  	(* toggle_marked)       (DoView *view, DoContext *context);


	gboolean    (* sender_do_close)	 (DoView *view);


	void	    (* update_context) (DoView *view, DoContext *context);
	void	    (* set_cursor_to_context) (DoView *view, DoContext *context);
	gboolean    (* can_set_cursor_to_context) (DoView *view, DoContext *context);

	void		(* do_find_barcode)  	  (DoView *view);
	gboolean	(* can_do_find_barcode)	  (DoView *view);
	gboolean    (* can_do_close_for_esc)  (DoView *view);

	void        (* do_refresh)  (DoView *view);
	gboolean    (* can_do_refresh)  (DoView *view);

	DoView     *(* get_active_child) (DoView *view);

	/* Signals */

};

GType		do_view_get_type		(void);

void		do_view_do_delete(DoView *view);
gboolean	do_view_can_do_delete(DoView *view);

void		do_view_do_insert(DoView *view);
gboolean	do_view_can_do_insert(DoView *view);

void		do_view_do_copy(DoView *view);
gboolean	do_view_can_do_copy(DoView *view);

void		do_view_do_edit(DoView *view, const gchar *tab);
gboolean	do_view_can_do_edit(DoView *view, const gchar *tab);

void		do_view_do_apply(DoView *view);
gboolean	do_view_can_do_apply(DoView *view);

void		do_view_do_unapply(DoView *view);
gboolean	do_view_can_do_unapply(DoView *view);

void		do_view_do_mail_send(DoView *view);
gboolean	do_view_can_do_mail_send(DoView *view);

void		do_view_do_popup(DoView *view,  GdkEventButton* event);
gboolean        do_view_do_grab_focus(DoView *view);

void		do_view_do_print(DoView *view);
gboolean	do_view_can_do_print(DoView *view);
void		do_view_do_print_now(DoView *view);

void		do_view_do_open(DoView *view);
gboolean	do_view_can_do_open(DoView *view);

void		do_view_do_save(DoView *view);
void		do_view_do_save_as(DoView *view);
gboolean	do_view_can_do_save(DoView *view);
gboolean	do_view_can_do_save_as(DoView *view);

void		do_view_do_close(DoView *view);
const char     *do_view_get_title(DoView *embed);
GdkPixbuf      *do_view_get_icon(DoView *embed);
const gchar	   *do_view_get_load_status(DoView *embed);
gint    	do_view_get_load_progress(DoView *embed);

gboolean    do_view_set_marked(DoView *view, DoContext *context, DoMarked merked);
gboolean    do_view_toggle_marked(DoView *view, DoContext *context);
DoMarked    do_view_get_marked(DoView *view, DoContext *context);

void		do_view_update_context(DoView *view, DoContext *context);
void		do_view_set_cursor_to_context(DoView *view, DoContext *context);
gboolean	do_view_can_set_cursor_to_context(DoView *view, DoContext *context);

void		do_view_do_find_barcode(DoView *view);
gboolean	do_view_can_do_find_barcode(DoView *view);
gboolean	do_view_can_do_close_for_esc(DoView *view);

void		do_view_do_refresh(DoView *view);
gboolean	do_view_can_do_refresh(DoView *view);

GtkWidget  *do_view_get_active_child(DoView *view);
gboolean    do_view_sender_do_close (DoView *view);

G_END_DECLS

#endif
