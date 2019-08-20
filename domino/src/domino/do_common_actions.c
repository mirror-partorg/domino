#include "do_common_actions.h"
#include "do_about_dialog.h"
#include "do_utilx.h"
#include "do_window.h"
#include "do_view.h"
#include "do_icons.h"
#include "do_view.h"
#ifndef MINIMAL_WIN
#include "do_keyboard.h"
#include "do_keyboard.h"
#include "do_html_view.h"
#include <webkit/webkit.h>
#endif
#include "do_setting_view.h"
#ifndef CASH
#include "do_inventory_menu.h"
#endif
#include "do_product_view.h"
#include "do_product_embed_edit.h"
#include "do_seller_dialog.h"
#include "do_barcode_dialog.h"
#include "do_pko_dialog.h"
#ifndef CASH
#include "do_limit_view.h"
#include "do_rz_view.h"
#endif
//#include "do_rz_ch.h"
#include "do_discount_dialog.h"
#include "do_schedule_dialog.h"
#include "do_schedule_view.h"
#include "do_purchase_dialog.h"
#include "do_purchase_view.h"
#include "do_sale_dialog.h"
#include "do_sale_view.h"
#ifndef CASH
#include "do_order_view.h"
#include "do_rest_view.h"
#include "do_discount_view.h"
#include "do_unit_period_dialog.h"
//#include "do_rate_dialog.h"
//#include "do_rate_view.h"
#endif
#ifdef USE_IM
#include "../misc/send_error.c"
#endif
#ifdef CASH_VIEW
#include "do_cash_view.h"
#endif
#include "do_notebook.h"
//#include "do_upload.h"
//#include "do_home_view.h"
#include "domino.h"
#include "do_rz_load.h"
#include <domino.h>
#include "../misc/print.h"
#include <string.h>
#include "version.h"
//#include "python.h"
#ifdef USE_EVINCE
#include "do_ev_view.h"
#endif



#if GTK_MAJOR_VERSION > 222
static void do_common_actions_start_long(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_close(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_quit(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_about(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_update(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_tabs_previous(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
#ifdef USE_IM
static void do_common_actions_do_im(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
#endif
#ifndef MINAMAL_WIN
static void do_common_actions_do_google(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
#endif
static void do_common_actions_do_tabs_next(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_tabs_move_left(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_tabs_move_right(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_report_saller1(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_report_saller2(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
/*
static void do_common_actions_do_pko_dialog(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);*/
#ifdef INVENTORY
static void do_common_actions_do_inventory(GtkAction *action, DoWindow *window);
#endif
static void do_common_actions_do_discount_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_schedule_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_purchase_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_sale_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_order_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_limit_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_rest_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_profile_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_common_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_common_dialog(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_fullscreen(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_mainmenu(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_statusbar(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_toolbar(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
static void do_common_actions_do_barcode_dialog(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data);
//static void do_common_actions_do_rz_check (GtkAction *action, DoWindow *window);
#ifdef ARTIX
//static void do_common_actions_upload (GtkAction *action, DoWindow *window);
#endif
//!!static
GActionEntry entries[] =
{
    { "QuitAction", do_common_actions_do_quit,},
    { "CloseAction", do_common_actions_do_close,},
    { "HelpAction", do_common_actions_do_about,},
    { "UpdateAction", do_common_actions_do_update,},
	{ "TabsPrevious",do_common_actions_do_tabs_previous, },
	{ "TabsNext", do_common_actions_do_tabs_next, },
	{ "TabsMoveLeft", do_common_actions_do_tabs_move_left, },
	{ "TabsMoveRight", do_common_actions_do_tabs_move_right, },
#ifndef MINIMAL_WIN
	{ "KeyboardView", do_common_actions_start_long, },
#endif
	{ "RzView",do_common_actions_start_long, },
	{ "ProductView", do_common_actions_start_long, },
#ifdef USE_IM
	{ "IMView", do_common_actions_do_im, },
#endif
#ifndef MINIMAL_WIN
	{ "GoogleView", do_common_actions_do_google, },
	{ "GoogleViewTool", do_common_actions_do_google, },
#endif
	{ "ProductViewRub", do_common_actions_start_long, },
	{ "SellerInOutReport", do_common_actions_do_report_saller1, },
	{ "SellerInDetailReport", do_common_actions_do_report_saller2, },
#ifndef MINIMAL_WIN
	{ "PKOPrint", do_common_actions_start_long, },
	{ "PKOPrintTool", do_common_actions_start_long, },
#endif
	{ "DiscountReport", do_common_actions_do_discount_report, },
	{ "ScheduleReport", do_common_actions_do_schedule_report, },
	{ "PurchaseReport", do_common_actions_do_purchase_report, },
	{ "SaleReport", do_common_actions_do_sale_report, },
	{ "OrderReport", do_common_actions_do_order_report, },
	{ "LimitReport", do_common_actions_do_limit_report, },
	{ "RestReport", do_common_actions_do_rest_report, },
	{ "BarcodePrint", do_common_actions_do_barcode_dialog, },
	{ "ProfileView", do_common_actions_do_profile_view, },
	{ "CommonView", do_common_actions_do_common_view, },
	{ "CashView", do_common_actions_start_long, },
	{ "CommonEdit", do_common_actions_do_common_dialog, },
	{ "ProductEmbedEdit", do_common_actions_start_long, },
	{ "ProductViewExtended", do_common_actions_start_long, },
	{ "FullScreen", do_common_actions_do_fullscreen, },
	{ "MainMenu", do_common_actions_do_mainmenu, },
	{ "StatusBar", do_common_actions_do_statusbar },
	{ "ToolBar", do_common_actions_do_toolbar },
};
#else
static GtkUIManager * myUIManager = NULL;
static GtkActionGroup * myGroup = NULL;

static void do_common_actions_start_long(GtkAction *a, GtkWidget *view);
static void do_common_actions_do_close(GtkAction *action, DoWindow *window);
static void do_common_actions_do_quit(GtkAction *a, GtkWindow *win);
static void do_common_actions_do_about(GtkAction *a, GtkWindow *win);
static void do_common_actions_do_update(GtkAction *a, GtkWindow *win);
static void do_common_actions_do_tabs_previous(GtkAction *action, DoWindow *window);
#ifdef USE_IM
static void do_common_actions_do_im(GtkAction *action, DoWindow *window);
#endif
#ifndef MINIMAL_WIN
static void do_common_actions_do_google(GtkAction *action, DoWindow *window);
#endif
#ifdef INVENTORY
static void do_common_actions_do_inventory(GtkAction *action, DoWindow *window);
#endif
#ifdef USE_EVINCE
static void do_common_actions_do_evince(GtkAction *action, DoWindow *window);
#endif
static void do_common_actions_do_tabs_next(GtkAction *action, DoWindow *window);
static void do_common_actions_do_tabs_move_left(GtkAction *action, DoWindow *window);
static void do_common_actions_do_tabs_move_right(GtkAction *action, DoWindow *window);
#ifndef CASH
static void do_common_actions_do_report_saller1(GtkAction *action, DoWindow *window);
static void do_common_actions_do_report_saller2(GtkAction *action, DoWindow *window);
static void do_common_actions_do_discount_report(GtkAction *action, DoWindow *window);
static void do_common_actions_do_schedule_report(GtkAction *action, DoWindow *window);
static void do_common_actions_do_purchase_report(GtkAction *action, DoWindow *window);
static void do_common_actions_do_sale_report(GtkAction *action, DoWindow *window);
static void do_common_actions_do_order_report(GtkAction *action, DoWindow *window);
static void do_common_actions_do_limit_report(GtkAction *action, DoWindow *window);
static void do_common_actions_do_rest_report(GtkAction *action, DoWindow *window);
#endif
static void do_common_actions_do_profile_view(GtkAction *action, DoWindow *window);
static void do_common_actions_do_common_view(GtkAction *action, DoWindow *window);
#ifndef MIN_SCREEN
static void do_common_actions_do_common_dialog(GtkAction *action, DoWindow *window);
#endif
static void do_common_actions_do_fullscreen(GtkAction *action, DoWindow *window);
static void do_common_actions_do_mainmenu(GtkAction *action, DoWindow *window);
static void do_common_actions_do_statusbar(GtkAction *action, DoWindow *window);
static void do_common_actions_do_toolbar(GtkAction *action, DoWindow *window);
static void do_common_actions_do_barcode_dialog(GtkAction *action, DoWindow *window);
//static void do_common_actions_do_rz_check (GtkAction *action, DoWindow *window);
#ifdef ARTIX
//static void do_common_actions_upload (GtkAction *action, DoWindow *window);
#endif

static GtkActionEntry entries[] =
{   // Top level
    { "File", NULL, "_Файл" },
	{ "Tabs", NULL, "В_кладки" },
	{ "Handbook", NULL, "_Справочники" },
	{ "Action", NULL, "Д_ействия" },
	{ "Report", NULL, "_Отчеты" },
	{ "Admin", NULL, "А_дминистрирование" },
    	{ "View", NULL, "_Вид" },
	{ "Tools", NULL, "_Инструменты" },

    { "QuitAction", GTK_STOCK_QUIT,( "Выход" ), NULL, NULL,
        G_CALLBACK(do_common_actions_do_quit)},
    { "CloseAction", GTK_STOCK_CLOSE,( "Закрыть" ), "<control>W", "Закрыть вкладку",
        G_CALLBACK(do_common_actions_do_close)},
    { "HelpAction", NULL,( "_Справка" ), NULL, NULL, NULL},
    { "AboutAction", GTK_STOCK_ABOUT,( "О программе..." ), NULL, NULL,
        G_CALLBACK(do_common_actions_do_about)},
    { "UpdateAction", NULL,( "Проверка обновлений..." ), NULL, NULL,
        G_CALLBACK(do_common_actions_do_update)},
	{ "TabsPrevious", NULL, "На пред_ыдущую вкладку", "<control>Page_Up",
	  "Перейти на предыдущую вкладку",
	  G_CALLBACK (do_common_actions_do_tabs_previous) },
	{ "TabsNext", NULL, "На след_ующую вкладку", "<control>Page_Down",
	  "Перейти на следующую вкладку",
	  G_CALLBACK (do_common_actions_do_tabs_next) },
	{ "TabsMoveLeft", NULL, "Передвинуть вкладку в_лево", "<shift><control>Page_Up",
	  "Передвинуть текущую вкладку влево",
	  G_CALLBACK (do_common_actions_do_tabs_move_left) },
	{ "TabsMoveRight", NULL, "Передвинуть вкладку в_право", "<shift><control>Page_Down",
	  "Передвинуть текущую вкладку вправо",
	  G_CALLBACK (do_common_actions_do_tabs_move_right) },
#ifndef MINIMAL_WIN
	{ "KeyboardView", DO_STOCK_DOMINO_KEYBOARD, "Горячие клавиши для кассовых терминалов", NULL,
	  "Горячие клавиши для кассовых терминалов",
	  G_CALLBACK (do_common_actions_start_long) },
#endif
	{ "RzView", DO_STOCK_DOMINO_GRLS, "Просмотр зарегистрированныж ЖВЛС", NULL,
	  "Просмотр зарегистрированныж ЖВЛС",
	  G_CALLBACK (do_common_actions_start_long) },
	{ "ProductView", DO_STOCK_DOMINO_MED, "Товары", NULL,
	  "Справочник товаров",
	  G_CALLBACK (do_common_actions_start_long) },
#ifdef USE_IM
	{ "IMView", DO_STOCK_DOMINO_WEB, "Отправка сообщения", "<control>F10",
	  "Отправка сообщения",
	  G_CALLBACK (do_common_actions_do_im) },
#endif
#ifndef MINIMAL_WIN
	{ "GoogleView", DO_STOCK_DOMINO_WEB, "Поиск в интернете", "<control>T",
	  "Поиск в интернете",
	  G_CALLBACK (do_common_actions_do_google) },
	{ "GoogleViewTool", DO_STOCK_DOMINO_WEB, "Интернет (Ctrl+T)", NULL,
	  "Поиск в интернете",
	  G_CALLBACK (do_common_actions_do_google) },
#endif
#ifdef USE_EVINCE
	{ "EvView", DO_STOCK_DOMINO_EVINCE, "Просмотр документа", NULL,
	  "Просмотр документа",
	  G_CALLBACK (do_common_actions_do_evince) },
#endif
	{ "ProductViewRub", DO_STOCK_DOMINO_MED, "Товары по городу", NULL,
	  "Справочник товаров по городу",
	  G_CALLBACK (do_common_actions_start_long) },
#ifndef CASH
#ifndef MINIAL_WIN
	{ "PKOPrint", GTK_STOCK_FILE, "Приходный кассовый ордер", "F11",
	  "Печать приходного кассового ордера",
      //G_CALLBACK (do_common_actions_do_pko_dialog) },
      G_CALLBACK (do_common_actions_start_long) },
#endif
	{ "SellerInOutReport", GTK_STOCK_FILE, "Приход,расход,остаток по представителю", NULL,
	  "Приход,расход,остаток по представителю",
	  G_CALLBACK (do_common_actions_do_report_saller1) },
	{ "SellerInDetailReport", GTK_STOCK_FILE, "Детальный приход по представителю", NULL,
	  "Детальный приход по представителю",
	  G_CALLBACK (do_common_actions_do_report_saller2) },
	{ "DiscountReport", GTK_STOCK_FILE, "Скидки", NULL,
	  "Скидки за период",
	  G_CALLBACK (do_common_actions_do_discount_report) },
	{ "ScheduleReport", GTK_STOCK_FILE, "График платежей", NULL,
	  "График платежей",
	  G_CALLBACK (do_common_actions_do_schedule_report) },
	{ "PurchaseReport", GTK_STOCK_FILE, "Приход", NULL,
	  "Приход за период",
	  G_CALLBACK (do_common_actions_do_purchase_report) },
	{ "SaleReport", GTK_STOCK_FILE, "Выручка", NULL,
	  "Выручка за период",
	  G_CALLBACK (do_common_actions_do_sale_report) },
	{ "OrderReport", GTK_STOCK_FILE, "Заказы", NULL,
	  "Заказы за период",
	  G_CALLBACK (do_common_actions_do_order_report) },
	{ "LimitReport", GTK_STOCK_FILE, "Остатки меньше реализации", NULL,
	  "Остатки меньше реализации",
	  G_CALLBACK (do_common_actions_do_limit_report) },
	{ "RestReport", GTK_STOCK_FILE, "Остатки", NULL,
	  "Остатки за период",
	  G_CALLBACK (do_common_actions_do_rest_report) },
	{ "ProductEmbedEdit", DO_STOCK_DOMINO_MED, "Товары (редактирование параметров)", NULL,
	  "Редактирование товаров в справочнике",
	  G_CALLBACK (do_common_actions_start_long) },
	{ "PKOPrintTool", GTK_STOCK_FILE, "ПКО (F11)", NULL,
	  "Печать приходного кассового ордера",
         //G_CALLBACK (do_common_actions_do_pko_dialog) },
      G_CALLBACK (do_common_actions_start_long) },
#ifdef INVENTORY
	{ "Inventory", GTK_STOCK_FILE, "Инвентаризация", NULL,
	  "Инвентаризация",
      G_CALLBACK (do_common_actions_do_inventory) },
#endif
#endif
	{ "BarcodePrint", GTK_STOCK_FILE, "Печать продажных кодов", NULL,
	  "Печать продажных кодов",
	  G_CALLBACK (do_common_actions_do_barcode_dialog) },
	{ "ProfileView", GTK_STOCK_PROPERTIES, "Редактор настроек", NULL,
	  "Редактор пользовательских настроек",
	  G_CALLBACK (do_common_actions_do_profile_view) },
	{ "CommonView", GTK_STOCK_PROPERTIES, "Редактор параметров", NULL,
	  "Редактор общих настроек",
	  G_CALLBACK (do_common_actions_do_common_view) },
	{ "CashView", GTK_STOCK_FILE, "Просмотр состояния касс", NULL,
	  "Просмотр состояния касс",
	  G_CALLBACK (do_common_actions_start_long) },
#ifndef MIN_SCREEN
	{ "CommonEdit", GTK_STOCK_PROPERTIES, "Параметры", NULL,
	  "Редактор параметров",
	  G_CALLBACK (do_common_actions_do_common_dialog) },
#endif
	{ "ProductViewExtended", DO_STOCK_DOMINO_MED, "Товары (расширенный)", NULL,
	  "Справочник товаров (расширеный вид)",
	  G_CALLBACK (do_common_actions_start_long) },
	{ "FullScreen", GTK_STOCK_FULLSCREEN, "Полноэкранный режим", NULL,
	  "Полноэкранный режим",
	  G_CALLBACK (do_common_actions_do_fullscreen) },
	{ "MainMenu", NULL, "Показывать главное меню", "F10",
	  "Показывать главное меню",
	  G_CALLBACK (do_common_actions_do_mainmenu) },
	{ "StatusBar", NULL, "Показывать строку состояния", NULL,
	  "Показывать строку состояния",
	  G_CALLBACK (do_common_actions_do_statusbar) },
	{ "ToolBar", NULL, "Показывать панель инструментов", NULL,
	  "Показывать панель инструментов",
	  G_CALLBACK (do_common_actions_do_toolbar) },
};
inline GtkUIManager *do_common_actions_get_ui_manager()
{
    return myUIManager;
}
#endif
#if GTK_MAJOR_VERSION > 22
void do_common_actions_init(GtkUIManager *ui_manager, GtkWindow *win)
{
//!!
}
#else
void do_common_actions_init(GtkUIManager *ui_manager, GtkWindow *win)
{
    const int   n_entries = G_N_ELEMENTS( entries );
    //g_action_map_add_action_entries()


    GtkActionGroup *action_group;
    myUIManager = ui_manager;


    action_group = myGroup = gtk_action_group_new("DoCommonAction");
    gtk_action_group_add_actions( action_group,
                                  entries, n_entries,
                                  win );

    gtk_ui_manager_insert_action_group( ui_manager, action_group, 0 );
    g_object_unref ( G_OBJECT( action_group ) );
}
#endif
static GHashTable * key_to_action = NULL;

#if GTK_MAJOR_VERSION > 22
static void ensure_action_map_loaded( )
{
//!!
}
#else
static void ensure_action_map_loaded(  )
{
    GList * l;
    GtkUIManager * uim = myUIManager;

    if( key_to_action != NULL )
        return;

    key_to_action =
        g_hash_table_new_full( g_str_hash, g_str_equal, g_free, NULL );

    for( l = gtk_ui_manager_get_action_groups( uim ); l != NULL;
         l = l->next )
    {
        GtkActionGroup * action_group = GTK_ACTION_GROUP( l->data );
        GList *          ait, *actions = gtk_action_group_list_actions(
            action_group );
        for( ait = actions; ait != NULL; ait = ait->next )
        {
            GtkAction *  action = GTK_ACTION( ait->data );
            const char * name = gtk_action_get_name( action );
            g_hash_table_insert( key_to_action, g_strdup( name ), action );
        }
        g_list_free( actions );
    }
}
#endif
#if GTK_MAJOR_VERSION > 22
static GAction* get_action( const char* name )
{
    ensure_action_map_loaded( );
    return ( GAction* ) g_hash_table_lookup( key_to_action, name );
}
#else
static GtkAction* get_action( const char* name )
{
    ensure_action_map_loaded( myUIManager );
    return ( GtkAction* ) g_hash_table_lookup( key_to_action, name );
}
#endif
void do_common_actions_action_activate( const char * name )
{
#if GTK_MAJOR_VERSION > 22
    GAction * action = get_action( name );
    g_assert( action != NULL );
    g_action_activate(action, NULL);
#else
    GtkAction * action = get_action( name );
    g_assert( action != NULL );
    gtk_action_activate( action );
#endif
}

void do_common_actions_action_sensitize( const char * name,
                  gboolean     b )
{
#if GTK_MAJOR_VERSION > 22
    GAction * action = get_action( name );
#else
    GtkAction * action = get_action( name );
#endif
    g_assert( action != NULL );
    g_object_set( action, "sensitive", b, NULL );
}
#ifdef ARTIX
/*
static void do_common_actions_upload (GtkAction *action,
		      DoWindow *window)
{
	DoContext *sklad = do_context_new_with_attributes(DO_CONTEXT_KIND_SKLAD,
	                       "sklad-code", domino_unit(), NULL);
	GtkWidget *win = do_upload_new(GTK_WIDGET(window), NULL,
	                               DO_UPLOAD_FLAGS_SEND_PRODUCT | DO_UPLOAD_FLAGS_SEND_CASHIER | DO_UPLOAD_FLAGS_SEND_KEYBOARD
	                               , sklad);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(win), TRUE);
    g_object_unref(sklad);

    gtk_widget_show(win);
}*/
#endif
/*!!
void do_common_actions_action_toggle( const char * name,
               gboolean     b )
{
#if GTK_MAJOR_VERSION > 22
    GAction * action = get_action( name );
    g_toggle_action_set_active( G_TOGGLE_ACTION( action ), b );
#else
    GtkAction * action = get_action( name );
    gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ), b );
#enfi
}
*/
/*!!
GtkWidget* do_common_actions_action_get_widget( const char * path )
{

    return gtk_ui_manager_get_widget( myUIManager, path );
}
*/
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_quit(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       win)
#else
static void do_common_actions_do_quit(GtkAction *a, GtkWindow *win)
#endif
{
    gtk_widget_destroy (GTK_WIDGET(win));
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_about(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       win)
#else
static void do_common_actions_do_about(GtkAction *a, GtkWindow *win)
#endif
{
    do_about_dialog(win, "", VERSION);
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_update(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       win)
#else
static void do_common_actions_do_update(GtkAction *a, GtkWindow *win)
#endif
{
#if defined(BINARY_UPDATE)
    update_action_run(GTK_WIDGET(win), FALSE);
#else
    DOMINO_SHOW_ERROR("Обновление в данной версии недоступно");
#endif
}
#ifndef CASH
static gboolean do_common_actions_rz_view(DoWindow *window)
{
	GtkNotebook *nb;
	DoView *view;

	nb = GTK_NOTEBOOK (do_window_get_notebook (window));
	g_return_val_if_fail (nb != NULL, FALSE);

	view = DO_VIEW(do_rz_view_new(NULL));
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return FALSE;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    return FALSE;

}
#endif
static gboolean do_common_actions_product_view(DoWindow *window)
{
	GtkNotebook *nb;
	DoView *view;

	nb = GTK_NOTEBOOK (do_window_get_notebook (window));
	g_return_val_if_fail (nb != NULL, FALSE);
	view = DO_VIEW(do_product_view_new(NULL, domino_unit(),
                                     0 //| DO_PRODUCT_VIEW_FLAGS_LOAD_IN_THREAD
	                                         ));
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return FALSE;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    return FALSE;
}
#ifndef CASH
static void row(GtkTreeView       *tree_view,
                                                        GtkTreePath       *path,
                                                        GtkTreeViewColumn *column,
                                                        GtkDialog *msg)
{
    gtk_dialog_response(msg, GTK_RESPONSE_OK);
}
#endif
#ifndef CASH
static gboolean do_common_actions_product_view_r(DoWindow *window)
{
	GtkNotebook *nb;
	DoView *view;
    GtkWidget *w;
    gint res;

#ifdef gtk_message_dialog_new
#undef gtk_message_dialog_new
#endif // gtk_message_dialog_new
    w = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK, "%s", "Выберите город");
#define gtk_message_dialog_new dead
    gtk_window_set_icon_name(GTK_WINDOW(w), "domino");
    gtk_window_set_position(GTK_WINDOW(w), GTK_WIN_POS_CENTER);
    GtkWidget *area = gtk_message_dialog_get_message_area (GTK_MESSAGE_DIALOG (w));
    //GtkWidget *box = gtk_widget_get_parent (area);
    GtkListStore *store;
    GtkWidget *tree;
    do_list_t *cities;
    do_alias_t *alias;
    GtkTreeIter iter;
    int i, city;
    alias = domino_alias_new(NULL);
    if ( !do_alias_open(alias, TRUE ) ) {
        do_alias_free(alias);
        return FALSE;
    }

    store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    cities = do_city_get_cities(alias);
    if ( !cities ) {
        do_alias_free(alias);
        return FALSE;
    }
    for ( i = 0; i < cities->count; i++ ) {
        char *name;
        city = DOPOINTER_TO_INT(cities->list[i]);
        name = do_city_get_name(alias, city);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, city, 1, name, -1);
        do_free(name);
    }
    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    GtkTreeViewColumn *col;
    GtkCellRenderer *r;

    r = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start (col, r, TRUE);
    gtk_tree_view_column_add_attribute (col, r, "text", 1);
    gtk_tree_view_column_set_title (col, "Города");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree),col);
    g_signal_connect(tree, "row-activated", G_CALLBACK(row), w);
    gtk_box_pack_start (GTK_BOX (area), tree, FALSE, FALSE, 0);
    gtk_widget_show(tree);
    GValue value ={0,};

    res = gtk_dialog_run(GTK_DIALOG(w));

    if ( res == GTK_RESPONSE_OK ) {
        GtkTreePath *path;
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(tree), &path, NULL);
        gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path);
        gtk_tree_model_get_value(GTK_TREE_MODEL(store), &iter, 0, &value);
        city = g_value_get_int(&value);
    }

    gtk_widget_destroy(w);
    if ( res == GTK_RESPONSE_OK ) {
        do_string_list_t *units;
        char *str;
        units = do_city_get_units(alias, city);
        str = do_string_list_to_csv(units);
        nb = GTK_NOTEBOOK (do_window_get_notebook (window));
        g_return_val_if_fail (nb != NULL, FALSE);
        view = DO_VIEW(do_product_view_new(NULL, str,
                                         0 //| DO_PRODUCT_VIEW_FLAGS_LOAD_IN_THREAD
                                                 ));
        do_end_long_operation(GTK_WIDGET(window));
        if ( !view )
            return FALSE;
        do_free(str);
        do_string_list_free(units);
        do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    }
    do_alias_free(alias);
    return FALSE;
}
#endif
static gboolean do_common_actions_product_view_extended(DoWindow *window)
{
	GtkNotebook *nb;
	DoView *view;

	nb = GTK_NOTEBOOK (do_window_get_notebook (window));
	g_return_val_if_fail (nb != NULL, FALSE);

	view = DO_VIEW(do_product_view_new(NULL, domino_unit(),
	                                         DO_PRODUCT_VIEW_FLAGS_REALTIME |
	                                         DO_PRODUCT_VIEW_FLAGS_SHOW_MULTI_PRICE |
	                                         DO_PRODUCT_VIEW_FLAGS_SHOW_RECEIVED |
	                                         DO_PRODUCT_VIEW_FLAGS_SHOW_UNPACK_PRICE |
	                                         DO_PRODUCT_VIEW_FLAGS_REALTIME
#ifdef ADS_RECOMMEND
	                                         |
	                                         DO_PRODUCT_VIEW_FLAGS_SHOW_RECOMMEND
#endif
#ifdef FULL_SCREEN
	                                         |
	                                         DO_PRODUCT_VIEW_FLAGS_SHOW_CLOCK
#endif
	                                         ));
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return FALSE;
    g_object_set(view, "receiver-object", view, NULL);
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    return FALSE;
}
#ifndef CASH
static gboolean do_common_actions_product_embed_edit(DoWindow *window)
{
	GtkNotebook *nb;
	DoView *view;

	nb = GTK_NOTEBOOK (do_window_get_notebook (window));
	g_return_val_if_fail (nb != NULL, FALSE);

	view = DO_VIEW(do_product_embed_edit_new(NULL, "main"));
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return FALSE;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    return FALSE;
}
#ifndef MINIMAL_WIN
static gboolean do_common_actions_keyboard_view(DoWindow *window)
{
	GtkNotebook *nb;
	DoView *view;

	nb = GTK_NOTEBOOK (do_window_get_notebook (window));
	g_return_val_if_fail (nb != NULL, FALSE);

	view = DO_VIEW(do_keyboard_new(NULL));
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return FALSE;
    do_view_do_grab_focus(DO_VIEW(view));
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    return FALSE;
}
#endif
#endif
#ifdef CASH_VIEW
static gboolean do_common_actions_cash_view(DoWindow *window)
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_val_if_fail (nb != NULL, FALSE);

    view = DO_VIEW(do_cash_view_new(DOMINO_CONFIG_COMMON));
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return FALSE;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    do_view_do_grab_focus(DO_VIEW(view));
    return FALSE;
}
#endif
#ifndef CASH
#ifndef MINIMAL_WIN
static gboolean do_common_actions_pko_dialog (DoWindow *window)
{
	DoContext *sklad = do_context_new_with_attributes(DO_CONTEXT_KIND_SKLAD,
	                       "sklad-code", domino_unit(), NULL);

	GtkWidget *dialog = do_pko_dialog_new(GTK_WIDGET(window), NULL, sklad);
	//gint result;

    do_end_long_operation(GTK_WIDGET(window));
    gtk_widget_show(GTK_WIDGET(dialog));
    return FALSE;
}
#endif
#endif
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_barcode_dialog(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_barcode_dialog(GtkAction *action, DoWindow *window)
#endif
{
    //py_run("test55", GTK_WINDOW(window));
	GtkWidget *dialog = do_barcode_dialog_new(GTK_WIDGET(window), NULL, "");

    do_end_long_operation(GTK_WIDGET(window));
    gtk_widget_show(GTK_WIDGET(dialog));
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_start_long(GSimpleAction *a,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_start_long(GtkAction *a, GtkWidget *window)
#endif
{
#if GTK_MAJOR_VERSION > 22
    gchar *action_name = (gchar*)g_action_get_name(G_ACTION(a));
#else
    gchar *action_name = (gchar*)gtk_action_get_name(a);
#endif
    GSourceFunc func = NULL;
    do_start_long_operation(GTK_WIDGET(window));
#ifndef CASH
    if (!strcmp( action_name, "RzView"))
        func = (GSourceFunc)do_common_actions_rz_view;
    else
#endif
    if (!strcmp( action_name, "ProductView"))
        func = (GSourceFunc)do_common_actions_product_view;
    else
#ifndef CASH
    if (!strcmp( action_name, "ProductViewRub"))
        func = (GSourceFunc)do_common_actions_product_view_r;
    else
#endif
    if (!strcmp( action_name, "ProductViewExtended"))
        func = (GSourceFunc)do_common_actions_product_view_extended;
#ifndef CASH
    else
    if (!strcmp( action_name, "ProductEmbedEdit"))
        func = (GSourceFunc)do_common_actions_product_embed_edit;
#ifndef MINIMAL_WIN
    else
    if (!strcmp( action_name, "KeyboardView"))
        func = (GSourceFunc)do_common_actions_keyboard_view;
#endif
#endif
#ifdef CASH_VIEW

    else
    if (!strcmp( action_name, "CashView"))
        func = (GSourceFunc)do_common_actions_cash_view;
    //else
#endif
    /*if (!strcmp( action_name, "RzCheck"))
        func = (GSourceFunc)do_common_actions_rz_check;
    else
    */
#ifndef CASH
#ifndef MINIMAL_WIN
    else
    if (!strcmp( action_name, "PKOPrint")) {
        func = (GSourceFunc)do_common_actions_pko_dialog;
    }
    else
    if (!strcmp( action_name, "PKOPrintTool")) {
        func = (GSourceFunc)do_common_actions_pko_dialog;
    }
#endif
#endif
    if ( func != NULL )
        g_idle_add_full (G_PRIORITY_HIGH_IDLE, func, window, NULL);
    else {
        do_end_long_operation(GTK_WIDGET(window));
        DOMINO_SHOW_INFO("Действие \"%s\" не реализованно", action_name);
    }
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_close(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_close(GtkAction *action, DoWindow *window)
#endif
{
	GtkWidget *notebook;
	DoView *embed;

	notebook = do_window_get_notebook (window);

	embed = do_window_get_active_child (window);
	if (embed)
        g_signal_emit_by_name (notebook, "tab-close-request", embed, NULL);
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_tabs_next(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_tabs_next(GtkAction *action, DoWindow *window)
#endif
{
	GtkNotebook *nb;
	gint page;

	nb = GTK_NOTEBOOK (do_window_get_notebook (window));
	g_return_if_fail (nb != NULL);

	page = gtk_notebook_get_current_page (nb);
	g_return_if_fail (page != -1);

	if (page < gtk_notebook_get_n_pages (nb) - 1)
	{
		gtk_notebook_set_current_page (nb, page + 1);
	}
}
#ifndef CASH
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_report_saller1(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_report_saller1(GtkAction *action, DoWindow *window)
#endif
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    view = DO_VIEW(do_seller_dialog_new(1));
    if ( !view )
        return;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    do_view_do_grab_focus(DO_VIEW(view));
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_report_saller2(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_report_saller2(GtkAction *action, DoWindow *window)
#endif
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    view = DO_VIEW(do_seller_dialog_new(2));
    if ( !view )
        return;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    do_view_do_grab_focus(DO_VIEW(view));
}
#endif
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_fullscreen(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_fullscreen(GtkAction *action, DoWindow *window)
#endif
{
    GdkWindowState state = gdk_window_get_state (gtk_widget_get_window(GTK_WIDGET(window)));

    if (state & GDK_WINDOW_STATE_FULLSCREEN)
        gtk_window_unfullscreen(GTK_WINDOW(window));
    else
        gtk_window_fullscreen(GTK_WINDOW(window));
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_mainmenu(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_mainmenu(GtkAction *action, DoWindow *window)
#endif
{
    GtkWidget *menu;
    menu = do_window_get_mainmenu(window);
    if ( !menu ) return;
    if ( !gtk_widget_get_visible(menu) ) {
        gtk_widget_set_visible(menu, TRUE);
        gtk_widget_grab_focus(menu);
        gtk_menu_shell_select_first(GTK_MENU_SHELL(menu), FALSE);
    }
    else
        gtk_widget_set_visible(menu, FALSE);
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_statusbar(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_statusbar(GtkAction *action, DoWindow *window)
#endif
{
    GtkWidget *menu;
    menu = do_window_get_statusbar(window);
    if ( menu )
        gtk_widget_set_visible(menu, !gtk_widget_get_visible(menu));
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_toolbar(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_toolbar(GtkAction *action, DoWindow *window)
#endif
{
    GtkWidget *menu;
    menu = do_window_get_toolbar(window);
    if ( menu )
        gtk_widget_set_visible(menu, !gtk_widget_get_visible(menu));
}
#ifndef MIN_SCREEN
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_common_dialog(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_common_dialog(GtkAction *action, DoWindow *window)
#endif
{
    do_common_edit(GTK_WIDGET(window));
}
#endif
#define PRINT_ROOT_PATH "Print"
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_profile_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_profile_view(GtkAction *action, DoWindow *window)
#endif
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
    do_view_do_grab_focus(DO_VIEW(view));
}

#ifdef INVENTORY
static void do_common_actions_do_inventory(GtkAction *action, DoWindow *window)
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    view = DO_VIEW(do_inventory_menu_new());
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    do_view_do_grab_focus(DO_VIEW(view));
}
#endif
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_common_view(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_common_view(GtkAction *action, DoWindow *window)
#endif
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    view = DO_VIEW(do_setting_view_new(DOMINO_CONFIG_COMMON));
    if ( !view )
        return;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    do_view_do_grab_focus(DO_VIEW(view));
}

static void do_common_open_web(DoWindow *window, const gchar *uri);
/*
static WebKitNavigationResponse do_common_open_web_navi_response(WebKitWebView *web_view,
                                              WebKitWebFrame       *frame,
                                              WebKitNetworkRequest *request,
                                              DoWindow       *window)
{
    const gchar *new_uri;
    new_uri = webkit_network_request_get_uri(request);
    return WEBKIT_NAVIGATION_RESPONSE_ACCEPT;
    //WEBKIT_NAVIGATION_RESPONSE_IGNORE;
}*/
/*
static WebKitWebView* do_common_open_web_new(WebKitWebView  *web_view,
                                              WebKitWebFrame *frame,
                                              DoWindow *window)
{
    const gchar *uri, *u;
    uri =  webkit_web_frame_get_uri (frame);
    u = webkit_web_view_get_uri(web_view);
    //do_common_open_web(window, uri);
    return NULL;

}
*/
#ifndef MINIMAL_WIN
static void do_common_open_web_title_change(WebKitWebView  *web_view,
                                                        WebKitWebFrame *frame,
                                                        gchar          *title,
                                                        DoView *window)
{
    g_object_set(window, "title", title, NULL);
}
static gboolean do_common_open_web_new_tab_request(WebKitWebView             *web_view,
                                                        WebKitWebFrame            *frame,
                                                        WebKitNetworkRequest      *request,
                                                        WebKitWebNavigationAction *navigation_action,
                                                        WebKitWebPolicyDecision   *policy_decision,
                                                        DoWindow *window)
{
    const gchar *uri;
    uri =  webkit_network_request_get_uri (request);
    do_common_open_web(window, uri);
    return TRUE;
}
void do_open_web(const gchar *uri)
{
   do_common_open_web(DO_WINDOW(domino_get_main_window()), uri);

}

static void do_common_open_web(DoWindow *window, const gchar *uri)
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    view = DO_VIEW(do_html_view_new());
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    do_view_do_grab_focus(DO_VIEW(view));
    g_object_set(view, "uri", uri, "title", "Поиск в интернет", NULL);
    /*g_signal_connect(WEBKIT_WEB_VIEW(do_html_get_view(DO_HTML_VIEW(view))),
                     "navigation-requested",
                      G_CALLBACK(do_common_open_web_navi_response), window);*/
    g_signal_connect(WEBKIT_WEB_VIEW(do_html_get_view(DO_HTML_VIEW(view))),
                      "title-changed",
                      G_CALLBACK(do_common_open_web_title_change), view);

    g_signal_connect(WEBKIT_WEB_VIEW(do_html_get_view(DO_HTML_VIEW(view))),
    "new-window-policy-decision-requested",
                      G_CALLBACK(do_common_open_web_new_tab_request), window);
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_google(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_google(GtkAction *action, DoWindow *window)
#endif
{
    gchar *url;

    DOMINO_LOCAL_GET("Webkit", "start-page", &url, NULL);
    if ( url && url[0] != '\0' )
        do_common_open_web(window, url);//
    else
        do_common_open_web(window, "http://google.ru");//
    //url?sa=t&rct=j&q=%D0%BF%D1%80%D0%B8%D1%81%D0%BB%D1%83%D0%B3%D0%B0&source=web&cd=2&ved=0CDgQFjAB&url=http%3A%2F%2Fru.wikipedia.org%2Fwiki%2F%25D0%259F%25D1%2580%25D0%25B8%25D1%2581%25D0%25BB%25D1%2583%25D0%25B3%25D0%25B0_(%25D1%2584%25D0%25B8%25D0%25BB%25D1%258C%25D0%25BC)&ei=8Z8tUYaEJ6eq4ASF4IGADA&usg=AFQjCNEv6HF5GfPlNxXvKPyPtn1gJB4jKw&bvm=bv.42965579,d.bGE&kb=1&cad=rjt");
                       //"http://www.google.ru");
}
#endif
#ifdef USE_IM
static void do_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, GtkWidget *widget)
{
    gtk_widget_grab_focus(widget);
}
static void do_changed(GtkTextBuffer *buffer, gpointer data)
{
    GtkTextIter start, end;
    char *text, *text1;
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
    text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer),
                                    &start, &end, FALSE);
    if ( text ) {
        text1 =  filter_get_text1(text);
        text1 =  filter_get_text1(text);
        if ( text1 && strcmp(text1, text) ) {
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer),text1,-1);

        }
    }
}
#if GTK_MAJOR_VERSION == 2
#include <gdk/gdkkeysyms.h>
#endif

#ifndef GDK_KEY_Return
#define GDK_KEY_Return GDK_Return
#endif

static gboolean do_key_press(GtkWidget *widget, GdkEventKey *event, GtkWidget *dialog)
{
	guint mask = gtk_accelerator_get_default_mod_mask ();
	if ( (event->state & mask) == 0 )
	{
		switch (event->keyval)
		{
		    case GDK_KEY_Return: {
                GtkWidget *w = gtk_window_get_default_widget(GTK_WINDOW (dialog));
                if (w && gtk_widget_get_sensitive(w))
                    gtk_window_activate_default (GTK_WINDOW (dialog));
                return TRUE;
            }
              //  mask = gtk_accelerator_get_default_mod_mask ();
		default:
			break;
		}
	}
	else if ((event->state & mask) == GDK_CONTROL_MASK &&
             (event->keyval == GDK_KEY_Return ))
                return FALSE;
    return FALSE;
}


#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_im(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_im(GtkAction *action, DoWindow *window)
#endif
{
    gchar *user,*domain,*password,*recipients;
    DOMINO_LOCAL_GET("Jabber", "user", &user, NULL);
    DOMINO_LOCAL_GET("Jabber", "domain", &domain, NULL);
    DOMINO_LOCAL_GET("Jabber", "password", &password, NULL);
    DOMINO_LOCAL_GET("Jabber", "recipients", &recipients, NULL);

    if ( !user ) user = "bot";
    if ( !domain ) domain = "jabber.glekar.ru";
    if ( !password ) password = "123";
    if ( !recipients ) recipients = "buh,oper,it";

    GtkWidget *dialog;
    gint result;
    GtkWidget *win;
    GtkWidget *vbox;

    win = (GtkWidget*)window;

    dialog = gtk_dialog_new_with_buttons(
            "Отправка сообщения", GTK_WINDOW(win),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            "Отмета",
            1,
            "Послать",
            GTK_RESPONSE_ACCEPT,
            NULL);

    gtk_window_set_icon_name(GTK_WINDOW(dialog), DO_STOCK_DOMINO_APPS);

    vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *view;
    GtkListStore *store;

    store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    do_string_list_t *rec;
    int i;
    rec = do_string_list_new_from_csv(recipients);

    for ( i = 0; i < rec->count; i++ ) {
        gchar *nic=rec->list[i];
        gchar *name,*ch,*value;
        GtkTreeIter iter;
        DOMINO_LOCAL_GET("Jabber", nic, &value, NULL);


        if ( !value )
            DOMINO_SHOW_ERROR("Неопределен ник %s",nic);
        name = g_strdup(value);

        ch = strchr(name, ':');
        if ( !ch ) {
            DOMINO_SHOW_ERROR("Неправольно описан ник %s %s",nic,name);
        }
        else {
            ch[0] = '\0';
            ch++;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, 0, name, 1, ch, -1);
        }
    }
    view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), FALSE);
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *col;

    renderer = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes ("Получатель", renderer, "text", 0, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);


    gtk_box_pack_start(GTK_BOX(vbox), view, TRUE, TRUE, 0);

    GtkTextBuffer    *buffer;
    GtkWidget       *text_view;

    buffer = gtk_text_buffer_new(NULL);
    text_view = gtk_text_view_new_with_buffer (buffer);

    gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 0);

    gtk_widget_set_size_request(text_view, 300, 80);

    gtk_box_set_spacing (GTK_BOX(vbox), 2);
    gtk_widget_show_all(dialog);


    g_signal_connect(view, "row-activated",  G_CALLBACK(do_activated), text_view);
    g_signal_connect(buffer,  "changed",  G_CALLBACK(do_changed), text_view);
    g_signal_connect(text_view, "key-press-event",  G_CALLBACK(do_key_press), dialog);
    gtk_dialog_set_default_response (GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
    result = gtk_dialog_run(GTK_DIALOG(dialog));

    if ( result == GTK_RESPONSE_ACCEPT ) {
        GtkTreePath *path = NULL;
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(view), &path, NULL);
        if ( path ) {
            GtkTreeIter iter;
            GValue address = {0,};
            gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path);
            gtk_tree_model_get_value(GTK_TREE_MODEL(store), &iter, 1, &address);
            do_string_list_t *adr;
            adr = do_string_list_new_from_csv(g_value_get_string(&address));
            GtkTextIter start, end;
            char *text;
            gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
            gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
            text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer),
                                            &start, &end, FALSE);
            if ( text )
                for ( i = 0; i < adr->count; i++ )
                    if  ( !send_im_message(user,domain,password,adr->list[i],text) )
                        DOMINO_SHOW_ERROR("Не отправлено");
        }
    }

    gtk_widget_destroy(dialog);
}
#endif
#ifdef USE_EVINCE
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_evince(GtkAction *action, DoWindow *window)
#else
static void do_common_actions_do_evince(GtkAction *action, DoWindow *window)
#endif
{

    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new("Открыть файл", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                          NULL);
    if ( gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT ) {
        gtk_widget_destroy(dialog);
        return;
    }


    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);

    view = DO_VIEW(do_ev_view_new());
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    do_view_do_grab_focus(DO_VIEW(view));

    char *uri;

    uri =  gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog));

    do_ev_view_open_uri(DO_EV_VIEW(view), uri);
    g_free(uri);
    gtk_widget_destroy(dialog);

}
#endif
#ifndef CASH
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_discount_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_discount_report(GtkAction *action, DoWindow *window)
#endif
{
    GtkNotebook *nb;
    DoView *view;
    GtkWidget *dialog;
    gint res;

    dialog = do_discount_dialog_new(GTK_WIDGET(window));
    res = gtk_dialog_run(GTK_DIALOG(dialog));

    if ( res == GTK_RESPONSE_ACCEPT ) {
        GDate *date1 = NULL, *date2 = NULL;
        gchar *units;
        gchar *range;
        gint group;
        g_object_get(dialog, "units", &units, "date1", &date1, "date2", &date2, "discount-range-name", &range, "discount-group", &group, NULL);


        nb = GTK_NOTEBOOK (do_window_get_notebook (window));
        g_return_if_fail (nb != NULL);
        view = DO_VIEW(do_discount_view_new(units, date1, date2, range, group));
        do_end_long_operation(GTK_WIDGET(window));
        if ( !view )
            return;
        do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
        do_view_do_grab_focus(DO_VIEW(view));
    }

    gtk_widget_destroy(dialog);
}
#endif
#ifndef CASH
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_schedule_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_schedule_report(GtkAction *action, DoWindow *window)
#endif
{
    GtkNotebook *nb;
    DoView *view;
    GtkWidget *dialog;
    gint res;

    dialog = do_schedule_dialog_new(GTK_WIDGET(window));
    res = gtk_dialog_run(GTK_DIALOG(dialog));

    if ( res == GTK_RESPONSE_ACCEPT ) {
        GDate *date1 = NULL, *date2 = NULL;
        gchar *aliases;
        gint postponement;
        gchar *ignore;
        g_object_get(dialog, "firms", &aliases, "date1", &date1, "date2", &date2, "schedule-postponement", &postponement, "supplier-ignore", &ignore, NULL);

        nb = GTK_NOTEBOOK (do_window_get_notebook (window));
        g_return_if_fail (nb != NULL);
        view = DO_VIEW(do_schedule_view_new(aliases, date1, date2, postponement, ignore));
        do_end_long_operation(GTK_WIDGET(window));
        if ( !view )
            return;
        do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
        do_view_do_grab_focus(DO_VIEW(view));
    }

    gtk_widget_destroy(dialog);
}
#endif
#ifndef CASH
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_purchase_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_purchase_report(GtkAction *action, DoWindow *window)
#endif
{
    GtkNotebook *nb;
    DoView *view;
    GtkWidget *dialog;
    gint res;

    dialog = do_purchase_dialog_new(GTK_WIDGET(window));
    res = gtk_dialog_run(GTK_DIALOG(dialog));

    if ( res == GTK_RESPONSE_ACCEPT ) {
        GDate *date1 = NULL, *date2 = NULL;
        gchar *units;
        g_object_get(dialog, "units", &units, "date1", &date1, "date2", &date2, NULL);


        nb = GTK_NOTEBOOK (do_window_get_notebook (window));
        g_return_if_fail (nb != NULL);
        view = DO_VIEW(do_purchase_view_new(units, date1, date2));
        do_end_long_operation(GTK_WIDGET(window));
        if ( !view )
            return;
        do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
        do_view_do_grab_focus(DO_VIEW(view));
    }

    gtk_widget_destroy(dialog);
}
#endif
#ifndef CASH
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_sale_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_sale_report(GtkAction *action, DoWindow *window)
#endif
{
    GtkNotebook *nb;
    DoView *view;
    GtkWidget *dialog;
    gint res;

    dialog = do_sale_dialog_new(GTK_WIDGET(window));
    res = gtk_dialog_run(GTK_DIALOG(dialog));

    if ( res == GTK_RESPONSE_ACCEPT ) {
        GDate *date1 = NULL, *date2 = NULL;
        gchar *units;
        g_object_get(dialog, "units", &units, "date1", &date1, "date2", &date2, NULL);


        nb = GTK_NOTEBOOK (do_window_get_notebook (window));
        g_return_if_fail (nb != NULL);
        view = DO_VIEW(do_sale_view_new(units, date1, date2));
        do_end_long_operation(GTK_WIDGET(window));
        if ( !view )
            return;
        do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
        do_view_do_grab_focus(DO_VIEW(view));
    }

    gtk_widget_destroy(dialog);
}
#endif
#ifndef CASH
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_limit_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_limit_report(GtkAction *action, DoWindow *window)
#endif
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);
    view = DO_VIEW(do_limit_view_new());
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    do_view_do_grab_focus(DO_VIEW(view));
}
#endif
#ifndef CASH
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_order_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_order_report(GtkAction *action, DoWindow *window)
#endif
{
    GtkNotebook *nb;
    DoView *view;

    nb = GTK_NOTEBOOK (do_window_get_notebook (window));
    g_return_if_fail (nb != NULL);
    view = DO_VIEW(do_order_view_new());
    do_end_long_operation(GTK_WIDGET(window));
    if ( !view )
        return;
    do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
    do_view_do_grab_focus(DO_VIEW(view));
}
#endif
#ifndef CASH
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_rest_report(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_rest_report(GtkAction *action, DoWindow *window)
#endif
{
    GtkNotebook *nb;
    DoView *view;
    GtkWidget *dialog;
    gint res;

    dialog = do_unit_period_dialog_new(GTK_WIDGET(window), "RestUnitPeriodDialog", "Остатки по аптекам");
    res = gtk_dialog_run(GTK_DIALOG(dialog));

    if ( res == GTK_RESPONSE_ACCEPT ) {
        GDate *date1 = NULL, *date2 = NULL;
        gchar *units;
        g_object_get(dialog, "units", &units, "date1", &date1, "date2", &date2, NULL);


        nb = GTK_NOTEBOOK (do_window_get_notebook (window));
        g_return_if_fail (nb != NULL);
        view = DO_VIEW(do_rest_view_new(units, date1, date2));
        do_end_long_operation(GTK_WIDGET(window));
        if ( !view )
            return;
        do_notebook_add_tab(DO_NOTEBOOK(nb), view, -1, TRUE);
        do_view_do_grab_focus(DO_VIEW(view));
    }

    gtk_widget_destroy(dialog);

}
#endif
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_tabs_previous(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_tabs_previous(GtkAction *action, DoWindow *window)
#endif
{
	GtkNotebook *nb;
	gint page;

	nb = GTK_NOTEBOOK (do_window_get_notebook (window));
	g_return_if_fail (nb != NULL);

	page = gtk_notebook_get_current_page (nb);
	g_return_if_fail (page != -1);

	if ( page > 0 )
	{
		gtk_notebook_set_current_page (nb, page - 1);
	}
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_tabs_move_left(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_tabs_move_left(GtkAction *action, DoWindow *window)
#endif
{
	GtkWidget *child;
	GtkNotebook *notebook;
	int page;

	notebook = GTK_NOTEBOOK (do_window_get_notebook (window));
	page = gtk_notebook_get_current_page (notebook);
	if (page < 1) return;

	child = gtk_notebook_get_nth_page (notebook, page);
	gtk_notebook_reorder_child (notebook, child, page - 1);
}
#if GTK_MAJOR_VERSION > 22
static void do_common_actions_do_tabs_move_right(GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       window)
#else
static void do_common_actions_do_tabs_move_right(GtkAction *action, DoWindow *window)
#endif
{
	GtkWidget *child;
	GtkNotebook *notebook;
	int page, n_pages;

	notebook = GTK_NOTEBOOK (do_window_get_notebook (window));
	page = gtk_notebook_get_current_page (notebook);
	n_pages = gtk_notebook_get_n_pages (notebook) - 1;
	if (page > n_pages - 1) return;

	child = gtk_notebook_get_nth_page (notebook, page);
	gtk_notebook_reorder_child (notebook, child, page + 1);
}
