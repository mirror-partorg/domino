#include "do_view_actions.h"
#include "do_view.h"
#include "do_application.h"
#include <string.h>
#include <gdk/gdkkeysyms.h>

//static GSimpleActionGroup * myGroup = NULL;

static void activate_action(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{

    if ( user_data && DO_IS_WINDOW(user_data) ) {

        DoView *doView = DO_VIEW(do_window_get_active_child(DO_WINDOW(user_data)));
        if ( !doView )
            return;

        gchar *action_name = (gchar*)g_action_get_name(G_ACTION(action));
        if (!strcmp( action_name, "DeleteAction"))
            do_view_do_delete(doView);
        else
        if (!strcmp( action_name, "InsertAction"))
            do_view_do_insert(doView);
        else
        if (!strcmp( action_name, "CopyAction"))
            do_view_do_copy(doView);
        else
        if (!strcmp( action_name, "EditAction"))
            do_view_do_edit(doView, NULL);
        else
        if (!strcmp( action_name, "ApplyAction"))
            do_view_do_apply(doView);
        else
        if (!strcmp( action_name, "UndoApplyAction"))
            do_view_do_unapply(doView);
        else
        if (!strcmp( action_name, "PrintAction"))
            do_view_do_print(doView);
        else
        if (!strcmp( action_name, "PrintNowAction"))
            do_view_do_print_now(doView);
        else
        if (!strcmp( action_name, "PrintActionTool"))
            do_view_do_print(doView);
        else
        if (!strcmp( action_name, "PrintNowActionTool"))
            do_view_do_print_now(doView);
        else
        if (!strcmp( action_name, "MailSendAction"))
            do_view_do_mail_send(doView);
        else
        if (!strcmp( action_name, "SaveAsAction"))
            do_view_do_save_as(doView);
        else
        if (!strcmp( action_name, "SaveAction"))
            do_view_do_save(doView);
        else
        if (!strcmp( action_name, "OpenAction"))
            do_view_do_open(doView);
        else
        if (!strcmp( action_name, "RefreshAction"))
            do_view_do_refresh(doView);
        else
        if (!strcmp( action_name, "ProductEditParcel"))
            do_view_do_edit(doView, "Parcel");
        if (!strcmp( action_name, "ProductEditStock"))
            do_view_do_edit(doView, "Stock");
        if (!strcmp( action_name, "ProductEditPrihod"))
            do_view_do_edit(doView, "Prihod");
        if (!strcmp( action_name, "ProductEditCheck"))
            do_view_do_edit(doView, "Check");
        if (!strcmp( action_name, "ProductEditRz"))
            do_view_do_edit(doView, "Rz");
        if (!strcmp( action_name, "ProductEditOrder"))
            do_view_do_edit(doView, "Order");
        if (!strcmp( action_name, "FindByBarcode"))
            do_view_do_find_barcode(doView);
    }
}

static GActionEntry entries[] =
{
    { "DeleteAction", activate_action},
    { "InsertAction", activate_action},
    { "CopyAction", activate_action},
    { "SaveAsAction", activate_action},
    { "SaveAction", activate_action},
    { "OpenAction", activate_action},
    { "RefreshAction", activate_action},
    { "EditAction", activate_action},
    { "ApplyAction", activate_action},
    { "UndoApplyAction", activate_action},
    { "PrintAction", activate_action},
    { "PrintNowAction", activate_action},
    { "FindByBarcode", activate_action},
    { "MailSendAction", activate_action},
};



void do_view_actions_init(DoWindow *window)
{
    GSimpleActionGroup *group;
    group = g_simple_action_group_new();
    g_action_map_add_action_entries(G_ACTION_MAP(group), entries, G_N_ELEMENTS (entries), window);
	gtk_widget_insert_action_group (GTK_WIDGET (window),
	                                "view-actions",
	                                G_ACTION_GROUP (group));

    do_view_actions_refresh(GTK_WIDGET(window));
}

void do_view_actions_action_sensitize(DoWindow *window, const char * name, gboolean b)
{
    GActionGroup *group = gtk_widget_get_action_group(GTK_WIDGET(window), "view-actions");
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP(group), name);
    g_assert( action != NULL );
    g_simple_action_set_enabled(G_SIMPLE_ACTION(action), b);
}

void do_view_actions_refresh(GtkWidget *widget)
{
    if ( !widget || !DO_IS_WINDOW(gtk_widget_get_toplevel(widget)) )
        return;
    DoWindow *window = DO_WINDOW(gtk_widget_get_toplevel(widget));

    const int n_entries = G_N_ELEMENTS(entries);
    DoView *view = DO_VIEW(do_window_get_active_child(window));
    int i;
    if ( view ) {
        do_view_actions_action_sensitize(window, "DeleteAction", do_view_can_do_delete(view));
        do_view_actions_action_sensitize(window, "InsertAction", do_view_can_do_insert(view));
        do_view_actions_action_sensitize(window, "CopyAction", do_view_can_do_copy(view));
        do_view_actions_action_sensitize(window, "EditAction", do_view_can_do_edit(view, NULL));
        do_view_actions_action_sensitize(window, "ApplyAction", do_view_can_do_apply(view));
        do_view_actions_action_sensitize(window, "UndoApplyAction", do_view_can_do_unapply(view));
        do_view_actions_action_sensitize(window, "MailSendAction", do_view_can_do_mail_send(view));
        do_view_actions_action_sensitize(window, "PrintAction", do_view_can_do_print(view));
        do_view_actions_action_sensitize(window, "PrintNowAction", do_view_can_do_print(view));
        //do_view_actions_action_sensitize(window, "PrintActionTool", do_view_can_do_print(view));
        //do_view_actions_action_sensitize(window, "PrintNowActionTool", do_view_can_do_print(view));
        do_view_actions_action_sensitize(window, "SaveAsAction", do_view_can_do_save_as(view));
        do_view_actions_action_sensitize(window, "SaveAction", do_view_can_do_save(view));
        do_view_actions_action_sensitize(window, "OpenAction", do_view_can_do_open(view));
        do_view_actions_action_sensitize(window, "FindByBarcode", do_view_can_do_find_barcode(view));
        do_view_actions_action_sensitize(window, "RefreshAction", do_view_can_do_refresh(view));
    }
    else
        for (i = 0; i < n_entries; i++)
            do_view_actions_action_sensitize(window, entries[i].name, FALSE);
}
/*
void  do_view_actions_set_view(GtkWidget *view)
{

    if (DO_IS_VIEW(view))
        view = do_view_get_active_child(DO_VIEW(view));
    else
        view = NULL;
    do_view_actions_refresh();
}
*/
