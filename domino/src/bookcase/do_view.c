
#include "do_view.h"
#include "do_marshal.h"



static void do_view_base_init (gpointer g_class);

#define DO_TYPE_MARKED do_marked_get_type()

GType do_marked_get_type(void)
{
    static GType type = 0;
    if (G_UNLIKELY (type == 0))
    {
        static const GEnumValue _do_marked_values[] = {
         {DO_MARKED_UNMARK, "DO_MARKED_UNMARK", "unmarked"},
         {DO_MARKED, "DO_MARKED", "marked"},
         {DO_MARKED_AND_ACCEPT, "DO_MARKED_AND_ACCEPT", "accept"},
         {DO_MARKED_AND_REJECT, "DO_MARKED_AND_REJECT", "reject"}         ,

         { 0, NULL, NULL }
        };
        type = g_enum_register_static ("DoMarked", _do_marked_values);
    }
    return type;
}


GType do_view_get_type (void)
{
        static GType type = 0;

        if (G_UNLIKELY (type == 0))
        {
                const GTypeInfo our_info =
                {
                        sizeof (DoViewIface),
                        do_view_base_init,
                        NULL,
                };

                type = g_type_register_static (G_TYPE_INTERFACE,
                                               "DoView",
                                               &our_info,
                                               (GTypeFlags)0);
        }
        return type;
}
static void do_view_base_init (gpointer g_class)
{
        static gboolean initialized = FALSE;

        if (!initialized)
        {
		g_signal_new ("close-request",
			      DO_TYPE_VIEW,
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (DoViewIface, close_request),
			      g_signal_accumulator_true_handled, NULL,
			      do_marshal_BOOLEAN__VOID,
			      G_TYPE_BOOLEAN,
			      0);

                initialized = TRUE;
        }
}

void do_view_do_delete (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->do_delete)
            iface->do_delete (view);
}

gboolean do_view_can_do_delete (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->do_delete)
            return iface->can_do_delete(view);
        return FALSE;
}

void do_view_do_open (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->do_open)
            iface->do_open (view);

}

gboolean do_view_can_do_open (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->can_do_open)
            return iface->can_do_open(view);
        else
            return FALSE;
}

void do_view_do_insert (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->do_insert)
            iface->do_insert (view);

}

gboolean do_view_can_do_insert (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->can_do_insert)
            return iface->can_do_insert(view);
        else
            return FALSE;
}

void do_view_do_copy (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->do_copy)
            iface->do_copy (view);
}

gboolean do_view_can_do_copy (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->can_do_copy)
            return iface->can_do_copy(view);
        else
            return FALSE;
}

void do_view_do_edit (DoView *view, const gchar *tab)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->do_edit)
            iface->do_edit (view, tab);
}

gboolean do_view_can_do_edit (DoView *view, const gchar *tab)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->can_do_edit)
            return iface->can_do_edit(view, tab);
        else
            return FALSE;
}

void do_view_do_apply (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->do_apply)
            iface->do_apply (view);
}

gboolean do_view_can_do_apply (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->can_do_apply)
            return iface->can_do_apply(view);
        else
            return FALSE;
}

void do_view_do_unapply (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->do_unapply)
            iface->do_unapply (view);

}
gboolean do_view_can_do_unapply (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->can_do_unapply)
            return iface->can_do_unapply(view);
        else
            return FALSE;
}

void do_view_do_mail_send (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->do_mail_send)
            iface->do_mail_send (view);
}

gboolean do_view_can_do_mail_send (DoView *view)
{
        DoViewIface *iface;

        iface = DO_VIEW_GET_IFACE (view);
        if (iface->can_do_mail_send)
            return iface->can_do_mail_send(view);
        else
            return FALSE;
}

void do_view_do_popup (DoView *view, GdkEventButton* event)
{
        DoViewIface *iface;
        iface = DO_VIEW_GET_IFACE (view);
        if (iface->do_popup)
            iface->do_popup (view, event);
}
gboolean do_view_do_grab_focus(DoView *view)
{
    DoViewIface *iface;
    iface = DO_VIEW_GET_IFACE (view);
    if (iface->do_grab_focus)
        return iface->do_grab_focus(view);
    else
        return FALSE;
}
void do_view_do_print (DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->do_print)
        iface->do_print (view);
}
void do_view_do_save_as (DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->do_save_as)
        iface->do_save_as(view);
}
void do_view_do_save (DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->do_save)
        iface->do_save(view);
}
gboolean do_view_can_do_save_as (DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->can_do_save_as)
        return iface->can_do_save_as(view);
    else
        return FALSE;
}
gboolean do_view_can_do_save(DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->can_do_save)
        return iface->can_do_save(view);
    else
        return FALSE;
}
void do_view_do_print_now (DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->do_print_now)
        iface->do_print_now(view);
}
gboolean do_view_can_do_print (DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->can_do_print)
        return iface->can_do_print(view);
    else
        return FALSE;
}
void do_view_do_close (DoView *view)
{
     DoViewIface *iface = DO_VIEW_GET_IFACE (view);
     if (iface->do_close)
	     iface->do_close (view);
}

const char *
do_view_get_title (DoView *view)
{
	DoViewIface *iface = DO_VIEW_GET_IFACE (view);
    if (iface->get_title)
        return iface->get_title (view);
    else
        return g_strdup("");
}

GdkPixbuf *
do_view_get_icon (DoView *view)
{
	DoViewIface *iface = DO_VIEW_GET_IFACE (view);
    if (iface->get_icon)
        return iface->get_icon (view);
    else
        return NULL;
}
gboolean
do_view_get_load_status (DoView *view)
{
	DoViewIface *iface = DO_VIEW_GET_IFACE (view);
	if (iface->get_load_status)
        return iface->get_load_status (view);
    else
        return FALSE;
}
gint
do_view_get_load_progress (DoView *view)
{
	DoViewIface *iface = DO_VIEW_GET_IFACE (view);
	if (iface->get_load_progress)
        return iface->get_load_progress (view);
    else
        return 0;
}
void  do_view_update_context(DoView *view, DoContext *context)
{
	DoViewIface *iface = DO_VIEW_GET_IFACE (view);
	if (iface->update_context)
        iface->update_context(view, context);
}
gboolean do_view_set_marked(DoView *view, DoContext *context, DoMarked marked)
{
	DoViewIface *iface = DO_VIEW_GET_IFACE (view);
	if (iface->set_marked)
        return iface->set_marked (view, context, marked);
    else
        return FALSE;
}
DoMarked do_view_get_marked(DoView *view, DoContext *context)
{
	DoViewIface *iface = DO_VIEW_GET_IFACE (view);
	if (iface->get_marked)
        return iface->get_marked (view, context);
    else
        return DO_MARKED_UNMARK;
}
gboolean do_view_toggle_marked(DoView *view, DoContext *context)
{
	DoViewIface *iface = DO_VIEW_GET_IFACE (view);
	if (iface->toggle_marked)
        return iface->toggle_marked (view, context);
    else
        return FALSE;
}
void do_view_do_find_barcode (DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->do_find_barcode)
        iface->do_find_barcode(view);
}
gboolean do_view_can_do_find_barcode (DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->can_do_find_barcode)
        return iface->can_do_find_barcode(view);
    else
        return FALSE;
}
gboolean do_view_can_do_close_for_esc(DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->can_do_close_for_esc)
        return iface->can_do_close_for_esc(view);
    else
        return FALSE;
}
void do_view_do_refresh (DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->do_refresh)
        iface->do_refresh(view);
}
gboolean do_view_can_do_refresh (DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->can_do_refresh)
        return iface->can_do_refresh(view);
    else
        return FALSE;
}
void do_view_set_cursor_to_context (DoView *view, DoContext *context)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->set_cursor_to_context &&
        do_view_can_set_cursor_to_context(view, context))
        iface->set_cursor_to_context(view, context);
}
gboolean do_view_can_set_cursor_to_context (DoView *view, DoContext *context)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->can_set_cursor_to_context)
        return iface->can_set_cursor_to_context(view, context);
    else
        return FALSE;
}
GtkWidget	   *do_view_get_active_child(DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->get_active_child)
        return GTK_WIDGET(iface->get_active_child(view));
    else
        return GTK_WIDGET(view);
}
gboolean do_view_sender_do_close (DoView *view)
{
    DoViewIface *iface;

    iface = DO_VIEW_GET_IFACE (view);
    if (iface->sender_do_close)
        return iface->sender_do_close(view);
    else
        return TRUE;
}
