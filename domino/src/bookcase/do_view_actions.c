#include "do_view_actions.h"
#include "do_view.h"
#include <string.h>
#include <gdk/gdkkeysyms.h>

static GtkUIManager   * myUIManager = NULL;
static GtkActionGroup * myGroup = NULL;
static DoView         * myCrntView = NULL;

static void action_cb(GtkAction *a, GtkWidget **view)
{
    if (*view && DO_IS_VIEW(*view)) {
        DoView *doView = DO_VIEW(*view);

        gchar *action_name = (gchar*)gtk_action_get_name(a);
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

static GtkActionEntry entries[] =
{
    { "ViewAction", NULL,( "Да_нные" ), NULL, "Основные операции с данными", NULL},
    { "DeleteAction", GTK_STOCK_REMOVE,( "Удалить" ), "Delete", "Удаление записи", G_CALLBACK( action_cb )},
    { "InsertAction", GTK_STOCK_ADD,( "Новая" ), "Insert", "Создать новую запись", G_CALLBACK( action_cb )},
    { "CopyAction", GTK_STOCK_COPY,( "Копировать" ), "<ctrl>Insert", "Копировать запись", G_CALLBACK( action_cb )},
    { "SaveAsAction", GTK_STOCK_SAVE_AS, ( "Сохранить как..." ), NULL, "Сохранить как", G_CALLBACK( action_cb )},
    { "SaveAction", GTK_STOCK_SAVE, ( "Сохранить" ), "<ctrl>S", "Сохранить", G_CALLBACK( action_cb )},
    { "OpenAction", GTK_STOCK_OPEN, ( "Открыть" ), "<ctrl>O", "Открыть", G_CALLBACK( action_cb )},
    { "RefreshAction", GTK_STOCK_REFRESH, ( "Обновить" ), "<ctrl>R", "Обновить", G_CALLBACK( action_cb )},
    { "EditAction", GTK_STOCK_EDIT,( "Редактировать" ), NULL, "Редактирование записи", G_CALLBACK( action_cb )},
#ifndef MIN_SCREEN
    { "ProductEditParcel", GTK_STOCK_PROPERTIES,( "Просмотр партий товара" ),
    "F4",
    "Просмотр партий товара", G_CALLBACK( action_cb )},
    { "ProductEditStock", GTK_STOCK_PROPERTIES,( "Просмотр остатков товара" ),"F3", "Просмотр остатков товара", G_CALLBACK( action_cb )},
    { "ProductEditPrihod", GTK_STOCK_PROPERTIES,( "Просмотр приходов товара" ),
#ifdef CASH
   NULL,
#else
"F5",
#endif
 "Просмотр приходов товара", G_CALLBACK( action_cb )},
    { "ProductEditCheck", GTK_STOCK_PROPERTIES,( "Просмотр чеков по товару" ), NULL, "Просмотр чеков по товару", G_CALLBACK( action_cb )},
    { "ProductEditOrder", GTK_STOCK_PROPERTIES,( "Просмотр заказов по товару" ), NULL, "Просмотр заказов по товару", G_CALLBACK( action_cb )},
    { "ProductEditRz", GTK_STOCK_PROPERTIES,( "Просмотр зарегистрированных цен" ), NULL, "Просмотр зарегистрированных цен", G_CALLBACK( action_cb )},
#endif
    { "ApplyAction", GTK_STOCK_APPLY,( "Выбрать" ),
#ifndef CASH_KEYBOARD
    "<shift>Return",
#else
    "F12",
#endif
    "Выбрать записи", G_CALLBACK( action_cb )},
    { "UndoApplyAction", GTK_STOCK_CANCEL,( "Отменить" ),
#ifndef CASH_KEYBOARD
    "<shift>Delete",
#else
    "F2",
#endif
    "Отмена выбора записей", G_CALLBACK( action_cb )},
    { "PrintAction", NULL, ( "Печать..." ), "<ctrl>p", "Печать...", G_CALLBACK( action_cb )},
    { "PrintNowAction", GTK_STOCK_PRINT,( "Печать" ),"F9", "Печать", G_CALLBACK( action_cb )},
    { "PrintActionTool", NULL,( "Печать (Ctrl+P)" ), NULL, "Печать...", G_CALLBACK( action_cb )},
    { "PrintNowActionTool", GTK_STOCK_PRINT,( "Печать(F9)" ),NULL, "Печать", G_CALLBACK( action_cb )},
    { "FindByBarcode", GTK_STOCK_FIND,( "Поиск по продажному коду" ),"F7", "Поиск по продажному коду", G_CALLBACK( action_cb )},
    { "MailSendAction", NULL,( "Передать" ),
#ifdef CASH
 "F5",
#else
NULL,
#endif
 "Передать записи", G_CALLBACK( action_cb )},
    { "ClockAction", NULL , ( "Время" ), NULL, "Посмотреть календать", G_CALLBACK( action_cb )},
};

void do_view_actions_init(GtkUIManager * ui_manager)
{
    const int   n_entries = G_N_ELEMENTS( entries );
    GtkActionGroup *action_group;
    myUIManager = ui_manager;

    action_group = myGroup = gtk_action_group_new("DoViewAction");
    gtk_action_group_add_actions( action_group,
                                  entries, n_entries,
                                  &myCrntView );
    gtk_ui_manager_insert_action_group( ui_manager, action_group, 0 );
    g_object_unref ( G_OBJECT( action_group ) );
    do_view_actions_refresh(NULL);
}

static GHashTable * key_to_action = NULL;
void do_view_actions_add_ui_from_string(const gchar *buffer, GError **error)
{
     gtk_ui_manager_add_ui_from_string(myUIManager, buffer, -1, error);
}


static void ensure_action_map_loaded( GtkUIManager * uim )
{
    GList * l;

    if( key_to_action != NULL )
        return;

    key_to_action =
        g_hash_table_new_full( g_str_hash, g_str_equal, g_free, NULL );
    return; //fix me
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

#if GTK_MAJOR_VERSION > 22
static GAction* get_action( const char* name )
{
    ensure_action_map_loaded( myUIManager );
    return ( GAction* ) g_hash_table_lookup( key_to_action, name );
}
#else
static GtkAction* get_action( const char* name )
{
    ensure_action_map_loaded( myUIManager );
    return ( GtkAction* ) g_hash_table_lookup( key_to_action, name );
}
#endif
void do_view_actions_action_activate( const char * name )
{
    GtkAction * action = get_action( name );

    g_assert( action != NULL );
    gtk_action_activate( action );
}

void do_view_actions_action_sensitize( const char * name,
                  gboolean     b )
{
    return; //fix me
    GtkAction * action = get_action( name );

    g_assert( action != NULL );
    g_object_set( action, "sensitive", b, NULL );
}

void do_view_actions_action_toggle( const char * name,
               gboolean     b )
{
    GtkAction * action = get_action( name );

    gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ), b );
}

GtkWidget* do_view_actions_action_get_widget( const char * path )
{
    return gtk_ui_manager_get_widget( myUIManager, path );
}

void do_view_actions_refresh()
{
    const int n_entries = G_N_ELEMENTS(entries);
    int i;
    if ( myCrntView ) {
        do_view_actions_action_sensitize("DeleteAction", do_view_can_do_delete(myCrntView));
        do_view_actions_action_sensitize("InsertAction", do_view_can_do_insert(myCrntView));
        do_view_actions_action_sensitize("CopyAction", do_view_can_do_copy(myCrntView));
        do_view_actions_action_sensitize("EditAction", do_view_can_do_edit(myCrntView, NULL));
        do_view_actions_action_sensitize("ApplyAction", do_view_can_do_apply(myCrntView));
        do_view_actions_action_sensitize("UndoApplyAction", do_view_can_do_unapply(myCrntView));
        do_view_actions_action_sensitize("MailSendAction", do_view_can_do_mail_send(myCrntView));
        do_view_actions_action_sensitize("PrintAction", do_view_can_do_print(myCrntView));
        do_view_actions_action_sensitize("PrintNowAction", do_view_can_do_print(myCrntView));
        do_view_actions_action_sensitize("PrintActionTool", do_view_can_do_print(myCrntView));
        do_view_actions_action_sensitize("PrintNowActionTool", do_view_can_do_print(myCrntView));
        do_view_actions_action_sensitize("SaveAsAction", do_view_can_do_save_as(myCrntView));
        do_view_actions_action_sensitize("SaveAction", do_view_can_do_save(myCrntView));
        do_view_actions_action_sensitize("OpenAction", do_view_can_do_open(myCrntView));
#ifndef NODOMINO
#ifndef MIN_SCREEN
        do_view_actions_action_sensitize("ProductEditParcel", DO_IS_PRODUCT_VIEW(myCrntView) &&
                                                              do_view_can_do_edit(myCrntView, "Parcel"));
        do_view_actions_action_sensitize("ProductEditStock", (DO_IS_PRODUCT_VIEW(myCrntView)
                                                               ||
                                                               DO_IS_RZ_VIEW(myCrntView) ||
#ifndef CASH
                                                               DO_IS_ORDER_VIEW(myCrntView) ||
                                                               DO_IS_LIMIT_VIEW(myCrntView) ||
#endif
                                                               DO_IS_RZ_EDIT_VIEW(myCrntView)
                                                              )&&
                                                              do_view_can_do_edit(myCrntView, "Stock"));
        do_view_actions_action_sensitize("ProductEditPrihod", (DO_IS_PRODUCT_VIEW(myCrntView)
                                                               ||
                                                               DO_IS_RZ_EDIT_VIEW(myCrntView)
                                                               )&&
                                                              do_view_can_do_edit(myCrntView, "Prohod"));
        do_view_actions_action_sensitize("ProductEditCheck", (DO_IS_PRODUCT_VIEW(myCrntView)
                                                               ||
                                                               DO_IS_RZ_EDIT_VIEW(myCrntView)
                                                               )&&
                                                              do_view_can_do_edit(myCrntView, "Check"));
        do_view_actions_action_sensitize("ProductEditOrder", (DO_IS_PRODUCT_VIEW(myCrntView)
                                                              ||
                                                               DO_IS_RZ_EDIT_VIEW(myCrntView)
                                                               )&&
                                                              do_view_can_do_edit(myCrntView, "Order"));
        do_view_actions_action_sensitize("ProductEditRz", (DO_IS_PRODUCT_VIEW(myCrntView)
                                                              ||
                                                               DO_IS_RZ_EDIT_VIEW(myCrntView)
                                                               )&&
                                                              do_view_can_do_edit(myCrntView, "Rz"));
#endif
        do_view_actions_action_sensitize("FindByBarcode", do_view_can_do_find_barcode(myCrntView));
#endif
        do_view_actions_action_sensitize("RefreshAction", do_view_can_do_refresh(myCrntView));
    }
    else
        for (i = 1; i < n_entries; i++)
            do_view_actions_action_sensitize(entries[i].name, FALSE);
    do_view_actions_action_sensitize("ClockAction", TRUE);
}
void  do_view_actions_set_view(GtkWidget *view)
{

    if (DO_IS_VIEW(view))
        myCrntView = do_view_get_active_child(DO_VIEW(view));
    else
        myCrntView = NULL;
    do_view_actions_refresh();
}
