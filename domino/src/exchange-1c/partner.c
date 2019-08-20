#include "partner.h"
#include "misc.h"
#include "do_message.h"
#include "exchange-const.h"
#include "exchange.h"
#include "do_link_dialog.h"

#include <domino.h>

#define DOMINO_DOCUMENT_TO_EXCHANGE 2
struct {
    gchar *dtype;
    gchar *dtype1;
} domino_documents[DOMINO_DOCUMENT_TO_EXCHANGE] =
{
    { "ПТ", "01" }, // Приход от поставщика
    { "51", "021" }, // Оплата поставщику (платежка)
};

static gboolean make_partner(partner_key0_t *partner_key0, GSList *aliases, GSList *objs, DoExchangeHandbook handbook);
gboolean make_partners(const gchar *aliases_name, GSList *objs)
{
    do_string_list_t *list;
    GSList *aliases = NULL;
    GSList *partner_keys = NULL;
    gboolean res;
    int i;
    struct {
        partner_key0_t partner_key0;
        DoExchangeHandbook handbook;
    } key_, *pkey;

    res = TRUE;
    list = do_string_list_new_from_csv(aliases_name);

    for ( i = 0; res && i < list->count; i++ ) {
        do_alias_t *alias;
        alias = domino_alias_new(list->list[i]);
        if ( !alias || !do_alias_open(alias, TRUE) ) {
            MY_ERROR("Невозможно открыть алиас \"%s\"", (char*)list->list[i]);
            res = FALSE;
        }
        if ( res ) {
            aliases = g_slist_append(aliases, alias);
        }
    }
    do_string_list_free(list);
    if ( res ) {
        int status, i;
        do_alias_t *alias;
        alias = aliases->data;
        for ( i = 0; i < DO_EXCHANGE_HANDBOOK_NO; i++ ) {
            partner_key0_t key;
            key_.handbook = i;
            switch ( i ) {
                case DO_EXCHANGE_HANDBOOK_PARTNER:
                    do_text_set(alias, key_.partner_key0.g_code, "03");
                    break;
/*!!                case DO_EXCHANGE_HANDBOOK_CASHIER:
                    do_text_set(alias, key_.partner_key0.g_code, "CAS");
                    break;*/
                case DO_EXCHANGE_HANDBOOK_FIRM:
                    do_text_set(alias, key_.partner_key0.g_code, "00");
                    break;
                case DO_EXCHANGE_HANDBOOK_SKLAD:
                    do_text_set(alias, key_.partner_key0.g_code, "01");
                    break;
                case DO_EXCHANGE_HANDBOOK_NO:
                    MY_ERROR("Неправильно указан тип справочника");
                    return FALSE;

            }
            do_cpy(key.g_code, key_.partner_key0.g_code);
            key_.partner_key0.code = 0;
            status = do_partner_key0(alias, &key_.partner_key0, DO_GET_GE);
            while ( status == DO_OK ) {
                if ( CANCELLED )
                    return FALSE;
                if ( do_cmp(key.g_code, key_.partner_key0.g_code) ) break;

                partner_keys = g_slist_append(partner_keys, g_memdup(&key_, sizeof(key_)));

                status = do_partner_key0(alias, &key_.partner_key0, DO_GET_NEXT);
            }
            if ( status == DO_ERROR )
                res = FALSE;
        }
    }
    if ( res ) {
        gint total, crnt;
        GSList *p;

        crnt = 0;
        total = g_slist_length(partner_keys);

        for ( p = partner_keys; res && p; p=p->next,crnt++ ) {
            if ( CANCELLED )
                return FALSE;
            PROGRESS_PROGRESS(crnt, total);
            pkey = p->data;
            res = make_partner(&pkey->partner_key0, aliases, objs, pkey->handbook);
        }
        PROGRESS_PROGRESS(total, total);
    }
    g_slist_foreach(partner_keys, (GFunc)g_free, NULL);
    g_slist_free(partner_keys);
    g_slist_foreach(aliases, (GFunc)do_alias_free, NULL);
    g_slist_free(aliases);

    return res;
}
static gboolean partner_is_empty(partner_key0_t *partner_key0, GSList *aliases, gboolean *empty);

typedef struct {
    gboolean event;
    gboolean res;
    gboolean no_close;

} dialog_close_t;

static gboolean check_(GtkWidget *widget)
{
    GtkWidget *d;
    gint res;
    d = gtk_message_dialog_new(GTK_WINDOW(widget), GTK_DIALOG_DESTROY_WITH_PARENT,
                               GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO,
                               "%s", "Прервать выполнение?");
    gtk_window_set_icon_name(GTK_WINDOW(d), "domino");
    res = gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);
    if ( res != GTK_RESPONSE_YES )
        return FALSE;
    return TRUE;
}
static gboolean partner_link_dialog_delete_event(GtkWidget *widget, GdkEvent *event, dialog_close_t *close)
{
    return close->no_close;
}
static void partner_link_dialog_response(GtkWidget *widget,
                                                        gint       response_id,
                                                        dialog_close_t *close)
{
    if ( response_id == GTK_RESPONSE_CANCEL ||
         response_id == GTK_RESPONSE_DELETE_EVENT ) {
        if ( !check_(widget) ) {
            if ( response_id == GTK_RESPONSE_DELETE_EVENT )
                close->no_close = TRUE;
            //gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_NONE);
            return;
        }
    }
    if ( response_id == GTK_RESPONSE_NO ||
         response_id == GTK_RESPONSE_OK ) {
        if ( !do_link_dialog_save(DO_LINK_DIALOG(widget), response_id == GTK_RESPONSE_NO) ) {
            SHOW_MESSAGE();
            return;
        }
    }
    g_object_get( widget, "saved", &close->res, NULL);
    gtk_widget_destroy(widget);
    close->event = FALSE;
}
static gboolean sad(GtkWidget *dialog)
{
    if ( get_progress() )
        gtk_window_set_transient_for (GTK_WINDOW(dialog), GTK_WINDOW(do_progress_info_get_window(get_progress())));
    gtk_widget_show(dialog);
    return FALSE;
}
static gboolean make_partner(partner_key0_t *partner_key0, GSList *aliases, GSList *objs, DoExchangeHandbook handbook)
{
    partner_data_key0_t partner_data_key0;
    partner_data_rec_t partner_data;
    int status;
    do_alias_t *alias;

    alias = aliases->data;

    do_cpy(partner_data_key0.g_code, partner_key0->g_code);
    partner_data_key0.code = partner_key0->code;
    partner_data_key0.type = PARTNER_DATA_DOMINO_TYPE;
    partner_data_key0.number = firm_guid_number;

    status = do_partner_data_get0(alias, &partner_data, &partner_data_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return FALSE;

    if ( status == DO_KEY_NOT_FOUND ) {
        if ( handbook == DO_EXCHANGE_HANDBOOK_PARTNER ) {
            gboolean empty;
            if ( !partner_is_empty(partner_key0, aliases, &empty) )
                return FALSE;
            if ( empty ) //!!
                return TRUE;
        }
        dialog_close_t close = {TRUE, FALSE, FALSE};

        SHOW_MESSAGE();

        gdk_threads_enter();
        GtkWidget *dialog;
        dialog = do_link_dialog_new(alias, objs, handbook, (void*)partner_key0);
        g_signal_connect(dialog, "response", G_CALLBACK(partner_link_dialog_response), &close);
        g_signal_connect(dialog, "delete-event", G_CALLBACK(partner_link_dialog_delete_event), &close);
        gdk_threads_leave();
        gdk_threads_add_idle((GSourceFunc)sad, dialog);

        //gtk_dialog_run(GTK_DIALOG(dialog));


        wait_for(&close.event);

        return close.res;
    }
    gchar *guid;
    gchar *name;
    gchar *account;
    ExchangeObject *obj;
    gboolean change = FALSE;

    guid = do_partner_data_param(alias, &partner_data, "GUID");
    if ( !g_strcmp0(guid, CONST_EMPTY_GUID) )
        return TRUE;

    name = do_partner_data_param(alias, &partner_data, "NAME");
    account = do_partner_data_param(alias, &partner_data, "DEFAULT_ACCOUNT");

    obj = find_exchange_object_by_guid(objs, guid);
    if ( !obj ) {
        gboolean res;
        QUESTION(res, "Объект с \"%s\":\"%s\" \"%s\" не найден. Сделать пустым? ", ELEM_1C_GUID, guid, name);
        if ( !res )
            return FALSE;
        do_partner_data_param_set(alias, &partner_data, "GUID", CONST_EMPTY_GUID);
        change = TRUE;
    }
    else {
        const gchar *account_ = NULL;

        const gchar *name_;
        ExchangeProperty *name_prop;
        name_prop = exchange_object_get_property(obj, ELEM_1C_NAME);
        if ( !name_prop || name_prop->type != EXCHANGE_TYPE_CONST_STRING || name_prop->empty ) {
            OBJ_MY_ERROR(obj, "Неопределен или неправильный тип \"%s\"", ELEM_1C_NAME);
            return FALSE;
        }
        name_ = g_value_get_string(&name_prop->value);

        if ( handbook == DO_EXCHANGE_HANDBOOK_PARTNER ) {
            gboolean many;

            account_ = get_partner_default_account(objs, obj, &many);
            if ( !account ) {
                gint res;
                if ( many )
                    QUESTION(res, "У контрагента %s не заполнен основной. Продолжить?", name_)
                else
                    QUESTION(res, "У контрагента %s нет счета. Продолжить?", name_);
                if ( res != GTK_RESPONSE_YES )
                    return FALSE;
            }
        }
        change = g_strcmp0(name, name_);
        if ( account_ )
            change = change || g_strcmp0(account, account_);

        if ( change ) {
            do_partner_data_param_set(alias, &partner_data, "NAME", name_);
            if ( account_ )
                do_partner_data_param_set(alias, &partner_data, "DEFAULT_ACCOUNT", account_);
            else
                if ( handbook == DO_EXCHANGE_HANDBOOK_PARTNER )
                    do_partner_data_param_set(alias, &partner_data, "DEFAULT_ACCOUNT", "");
        }
    }
    do_free(name);
    do_free(account);

    if ( change ) {
        if ( do_partner_data_update(alias, &partner_data) != DO_OK )
            return FALSE;
        if ( !do_protocol_partner_add_key(alias, partner_key0, DO_PROTOCOL_ACTION_CHANGE) )
            return FALSE;
    }
    return TRUE;
}
static gboolean partner_is_empty(partner_key0_t *partner_key0, GSList *aliases, gboolean *empty)
{
    GSList *l;
    *empty = TRUE;
    for ( l=aliases; l; l=l->next ) {
        do_alias_t *alias;
        document_key2_t document_key2;
        document_rec_t document;
        int status;

        alias = l->data;

        do_cpy(document_key2.p_g_code, partner_key0->g_code);
        document_key2.p_code = partner_key0->code;
        do_text_set_empty(document_key2.party);
        document_key2.date = 0;
        document_key2.time = 0;

        status = do_document_get2(alias, &document, &document_key2, DO_GET_GE);
        while ( status == DO_OK ) {
            gchar *dtype, *dtype1;
            int i;
            if ( do_cmp(document_key2.p_g_code, partner_key0->g_code) ||
                 document_key2.p_code != partner_key0->code ) break;

            dtype = do_text(alias, document.data.dtype);
            dtype1 = do_text(alias, document.data.dtype1);

            for ( i = 0; i < DOMINO_DOCUMENT_TO_EXCHANGE; i++ )
                if ( !g_strcmp0(dtype, domino_documents[i].dtype) &&
                     !g_strcmp0(dtype1, domino_documents[i].dtype1) ) {
                    *empty = FALSE;
                    break;
                }
            g_free(dtype);g_free(dtype1);
            if ( !*empty )
                return TRUE;

            status = do_document_get2(alias, &document, &document_key2, DO_GET_NEXT);
        }
        if ( status == DO_ERROR )
            return FALSE;
    }
    return TRUE;
}
