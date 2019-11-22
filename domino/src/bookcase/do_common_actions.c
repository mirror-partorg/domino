#include "do_common_actions.h"
#include "do_utilx.h"
#include "do_window.h"
#include "do_view.h"
#include "domino.h"
#include "do_notebook.h"
#include "do_setting_view.h"
#include "do_ads_view.h"
#include "do_obj_view.h"
#include "do_list_view.h"
#include "do_application.h"


static void do_common_actions_do_profile_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_setting_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_ads_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_goods_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_list_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_obj_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_quit(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_close(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_next(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_previous(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);

GActionEntry entries[] =
{
//    { "QuitAction", do_common_actions_do_quit,},
//    { "CloseAction", do_common_actions_do_close,},
//    { "HelpAction", do_common_actions_do_about,},
	//{ "TabsPrevious",do_common_actions_do_tabs_previous, },
	//{ "TabsNext", do_common_actions_do_tabs_next, },
	//{ "TabsMoveLeft", do_common_actions_do_tabs_move_left, },
	//{ "TabsMoveRight", do_common_actions_do_tabs_move_right, },
	//{ "FullScreen", do_common_actions_do_fullscreen, },
	{ "AdsView", do_common_actions_do_ads_view, },
	{ "GoodsView", do_common_actions_do_goods_view, },
	{ "ListView", do_common_actions_do_list_view, "s"},
	{ "ObjView", do_common_actions_do_obj_view, "s" },
	{ "ProfileView", do_common_actions_do_profile_view, },
	{ "SettingView", do_common_actions_do_setting_view, },
	{ "Next", do_common_actions_next },
	{ "Previous", do_common_actions_previous },
	{ "Quit", do_common_actions_quit },
	{ "Close", do_common_actions_close },
};
static GSimpleActionGroup *group = NULL;

#if !GTK_CHECK_VERSION(3,16,0)
GActionGroup *do_common_action_get_group()
{
    return (GActionGroup*)group;
}
#endif

void do_common_actions_init(DoWindow *window)
{
    group = g_simple_action_group_new();
    g_action_map_add_action_entries(G_ACTION_MAP(group), entries, G_N_ELEMENTS (entries), window);
	gtk_widget_insert_action_group (GTK_WIDGET (window),
	                                "common-actions",
	                                G_ACTION_GROUP (group));

    //do_common_actions_refresh(window);
}
#define PRINT_ROOT_PATH "Print"

static void do_common_actions_quit(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
{
    gtk_widget_destroy(GTK_WIDGET(window));
}
static void do_common_actions_do_profile_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    view = DO_VIEW(do_setting_view_new(DOMINO_CONFIG_PROFILE));
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    gtk_widget_grab_focus(GTK_WIDGET(nb));
    do_view_do_grab_focus(DO_VIEW(view));
}
static void do_common_actions_close(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
{
    GtkNotebook *nb;
    GtkWidget *child;
    gint page_num;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    page_num = gtk_notebook_get_current_page(GTK_NOTEBOOK(nb));

    if ( page_num != -1 &&
        (child = gtk_notebook_get_nth_page (GTK_NOTEBOOK(nb), page_num)) != NULL &&
         do_view_can_do_close_for_esc(DO_VIEW(child)) ) {
        do_view_do_close(DO_VIEW(child));
    }
}
static void do_common_actions_do_ads_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    view = DO_VIEW(do_ads_view_new());
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    gtk_widget_grab_focus(GTK_WIDGET(nb));
    do_view_do_grab_focus(DO_VIEW(view));
}
static void do_common_actions_do_list_view_(const gchar *name, DoWindow *window)
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    view = DO_VIEW(do_list_view_new(name));
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    gtk_widget_grab_focus(GTK_WIDGET(nb));
    do_view_do_grab_focus(DO_VIEW(view));
}

static void do_common_actions_do_goods_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
{
    do_common_actions_do_list_view_("goods", DO_WINDOW(window));
}
static void do_common_actions_do_list_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
{
    const gchar  *name;
    name = g_variant_get_string(parameter, NULL);
    do_common_actions_do_list_view_(name, DO_WINDOW(window));
}

void do_common_action_activate(const gchar *action_name, GVariant *parameter)
{
	g_action_group_activate_action(G_ACTION_GROUP(group), action_name, parameter);
}
static void do_common_actions_do_obj_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
{
    GtkNotebook *nb;
    DoView *view;
    const gchar  *key;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);
    key = g_variant_get_string(parameter, NULL);

    view = DO_VIEW(do_obj_view_new(key));
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    gtk_widget_grab_focus(GTK_WIDGET(nb));
    do_view_do_grab_focus(DO_VIEW(view));
}
static void do_common_actions_do_setting_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
{
    GtkApplication *app = gtk_window_get_application(
                    GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(window))));
    do_application_settings(DO_APPLICATION(app));
}
static void do_common_actions_previous(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
{
    GtkNotebook *nb;
    GtkWidget *child;
    gint page_num;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    page_num = gtk_notebook_get_current_page(GTK_NOTEBOOK(nb));

    if ( page_num != -1 &&
        (child = gtk_notebook_get_nth_page (GTK_NOTEBOOK(nb), page_num)) != NULL &&
         do_view_can_do_close_for_esc(DO_VIEW(child)) ) {
        do_view_do_close(DO_VIEW(child));
    }
}
static void do_common_actions_next(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
{
    GtkNotebook *nb;
    GtkWidget *child;
    gint page_num;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    page_num = gtk_notebook_get_current_page(GTK_NOTEBOOK(nb));

    if ( page_num != -1 &&
        (child = gtk_notebook_get_nth_page (GTK_NOTEBOOK(nb), page_num)) != NULL &&
         do_view_can_do_edit(DO_VIEW(child),NULL) ) {
        do_view_do_edit(DO_VIEW(child),NULL);
    }
}
