#include <stdio.h>
#include <stdlib.h>
#include <domino.h>
#include <domino.h>
#include <libxml/parser.h>
#include <gtk/gtk.h>
#include <locale.h>
#include <time.h>
#include "exchange-const.h"
#include "do_progress_info.h"
#include "do_message.h"
#include "exchange.h"
#include "misc.h"
#include "partner.h"
#include "document.h"
#include "version.h"


static gboolean start_parse(const char *filename);

static gchar    *file_ = NULL;
static gchar    *file_out = NULL;
static gboolean  debug = FALSE;
static gboolean  empty_link = FALSE;
static gchar    *aliases_name = NULL;
static gchar    *date_start = NULL;
static gchar    *date_end = NULL;
static gchar    *sklads = NULL;
static gchar    *user = NULL;
static gchar    *types = NULL;
static struct tm tm1,tm2;
static GSList   *units = NULL;
static gboolean  no_protocol = FALSE;
static gboolean  no_check  = FALSE;
static gboolean  bank_ = FALSE;


static const GOptionEntry options [] =
{
    { "file", 'f', 0, G_OPTION_ARG_STRING, &file_,
      "Файл XML с данными", "Данные"},

    { "aliases", 'a', 0, G_OPTION_ARG_STRING, &aliases_name,
      "Алиасы баз домино через запятую", "Алиасы"},

    { "clear", 'c', 0, G_OPTION_ARG_NONE, &empty_link,
      "Очистить все связи", "Очистить"},

    { "start", 's', 0, G_OPTION_ARG_STRING, &date_start,
      "Дата начала периода", "Начало"},

    { "end", 'e', 0, G_OPTION_ARG_STRING, &date_end,
      "Дата конца периода", "Конец"},

    { "no-protocol", 'n', 0, G_OPTION_ARG_NONE, &no_protocol,
      "Не переносить измененные документы за период выборки", "Протокол"},

    { "no-check", 'n', 0, G_OPTION_ARG_NONE, &no_check,
      "Не проверять справочники", "Справочники"},

    { "type", 't', 0, G_OPTION_ARG_STRING, &types,
      "Тип документов <goods>|<bank>", "Тип документов"},

    { "user", 'U', 0, G_OPTION_ARG_STRING, &user,
      "Код пользователя 1с", "код пользователя"},

    { "units", 'u', 0, G_OPTION_ARG_STRING, &sklads,
      "GUID подразделений через зяпятую", "Подразделения"},

    { "out", 'o', 0, G_OPTION_ARG_STRING, &file_out,
      "Файл XML с выходными данными", "Документы"},



    { "debug", 'd', 0, G_OPTION_ARG_NONE, &debug,
      "Отладка", "Отладка"},

    {NULL}
};
static int result_value = 0;

int main(int argc, char **argv)
{

    setlocale(LC_ALL, "");
    //gtk_disable_setlocale ();
    gtk_set_locale();

    //g_thread_init(NULL);
    //gdk_threads_init();
    gdk_threads_enter();

    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    config_file_init();

    do_message_init();
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, do_message_log_gtk_func, NULL);
    do_log_set_log_func(do_message_log_do_func, NULL);

    if ( !domino_init(NULL, TRUE, DO_PARAM_END) ) {
        ERROR_AND_SHOW("Домино не инициализированно");
        gdk_threads_leave();
        exit(1);
    }


    GOptionContext *context;
    context = g_option_context_new(" - Домино");
    g_option_context_add_main_entries (context, options, NULL);
    g_option_context_add_group (context, gtk_get_option_group (FALSE));
    g_option_context_set_help_enabled(context, FALSE);
    GError *error = NULL;

    if ( !g_option_context_parse (context, &argc, &argv, &error) ) {
        ERROR_AND_SHOW("%s\n%s", (error) ? error->message : "", g_option_context_get_help (context, TRUE, NULL));
        gdk_threads_leave();
        exit(1);
    }
    if ( !file_ && !empty_link && g_strcmp0(types, "bank") ) {
        ERROR_AND_SHOW("Неопределен файл загрузки.\n%s", g_option_context_get_help (context, TRUE, NULL))
        gdk_threads_leave();
        exit(1);
    }
    if ( !aliases_name ) {
        ERROR_AND_SHOW("Неопределены алиасы домино.\n%s", g_option_context_get_help (context, TRUE, NULL))
        gdk_threads_leave();
        exit(1);
    }
    if ( types ) {
        if ( g_strcmp0(types, "goods") && g_strcmp0(types, "bank") ) {
            ERROR_AND_SHOW("Неправильный тип документов\n%s", g_option_context_get_help (context, TRUE, NULL));
            gdk_threads_leave();
            exit(1);
        }
        bank_ = !g_strcmp0(types, "bank");
    }
    if ( date_start ) {
        if ( !strpdatetime(date_start, &tm1) ) {
            ERROR_AND_SHOW("Неправильная дата \"%s\"", date_start);
            gdk_threads_leave();
            exit(1);
        }
    }
    if ( date_end ) {
        if ( !strpdatetime(date_end, &tm2) ) {
            ERROR_AND_SHOW("Неправильная дата \"%s\"", date_end);
            gdk_threads_leave();
            exit(1);
        }
    }

    if ( sklads ) {
        int i;
        do_string_list_t *list=do_string_list_new_from_csv(sklads);
        for ( i = 0; i < list->count; i++ ) {
            units = g_slist_append(units, list->list[i]);
        }
    }
    if ( date_end || date_start ) {
        if ( !date_end || !date_start || !file_out || !types ) {
            ERROR_AND_SHOW("Опреленены не все параметры для документов\n%s", g_option_context_get_help (context, TRUE, NULL));
            gdk_threads_leave();
            exit(1);
        }
    }

    set_progress(do_progress_info_new());
    do_progress_info_start(get_progress());
	set_cancellable(do_progress_info_get_cancellable(get_progress()));

    gdk_threads_add_idle((GSourceFunc)start_parse, file_);

    gtk_main ();

    gdk_threads_leave();

    config_file_save();

    return result_value;
}

static gpointer read_file_thread(gchar *filename);

static void end_parse(gboolean res)
{
    result_value=0;
    if ( CANCELLED ) {
        MY_ERROR("Выполнение было прервано");
        result_value=1;
    }
    if ( !res )
	result_value=1;
    SHOW_MESSAGE();
    gdk_threads_enter();
    do_progress_info_finish(get_progress());
    gtk_main_quit();
    gdk_threads_leave();
}

static gpointer wait_thread(GThread *thread)
{
    gboolean res;
    res = GPOINTER_TO_INT(g_thread_join(thread));
    end_parse(res);
    return NULL;
}
static gboolean start_parse(const char *filename)
{
    GError *error = NULL;
    gchar *file_name;
    GThread *thread;

    if ( filename )
        file_name = g_strdup(filename);
    else
        file_name = NULL;

    thread = g_thread_create((GThreadFunc)read_file_thread, file_name, TRUE, &error);
    if ( !thread ) {
        MY_ERROR("Невозможно создать процесс: %s\n", error->message);
        g_error_free (error);
        end_parse(FALSE);
        return FALSE;
    }
    g_thread_create((GThreadFunc)wait_thread, thread, FALSE, NULL);
    return FALSE;
}

static GSList  *read_file(const gchar *filename);
static gboolean clear_link(const gchar *aliases_name);
static gpointer read_file_thread(gchar *filename)
{
    gboolean res = TRUE;
    gint crnt = 0, total = 0;
    ExchangeObject *user_obj = NULL;

    if ( !bank_ )
        total++;
    if ( date_end || date_start )
        total++;
    if ( !no_check )
        total++;


    if ( empty_link ) {
        res = clear_link(aliases_name);
        if ( total == 1 )
	   return GINT_TO_POINTER(res);
    }

    if ( res && filename && !bank_ ) {
        GSList *objs;
        PROGRESS_DETAIL("(%d/%d) Загрузка справочников 1с", ++crnt, total);
        objs = read_file(filename);
        g_free(filename);
        if ( !objs )
            return FALSE;
        if ( user ) {
            //gchar *user_name;
            //user_name =  g_locale_from_utf8(user, -1, NULL, NULL, NULL);
            GSList *list;
            list = find_exchange_objects_by_code(objs, EXCHANGE_TYPE_HANDBOOK_USER, user);
            if ( !list ) {
                MY_ERROR("Не найдено пользователя с кодом \"%s\"", user);
                return FALSE;
            }
            if ( g_slist_length(list) > 1 ) {
                MY_ERROR("Для пользователя с кодом \"%s\" более одной записи", user);
                return FALSE;
            }
            user_obj = list->data;
        }
        if ( !no_check ) {
           PROGRESS_DETAIL("(%d/%d) Проверка справочников", ++crnt, total);
           res = make_partners(aliases_name, objs);
        };

        if ( res && (date_end || date_start ) ) {
            PROGRESS_STATUS("Выгрузка документов");
            PROGRESS_DETAIL("(%d/%d) Выгрузка документов с <b>%s</b> по <b>%s</b>", ++crnt, total, date_start, date_end);
            res = make_goods_documents(aliases_name, objs, &tm1, &tm2, units, !no_protocol, file_out, user_obj);

        }
        exchange_objects_free(objs);
    }
    if ( bank_ ) {
        PROGRESS_STATUS("Выгрузка банковских документов");
        PROGRESS_DETAIL("(%d/%d) Выгрузка документов с <b>%s</b> по <b>%s</b>", ++crnt, total, date_start, date_end);
        res = make_bank_documents(aliases_name, &tm1, &tm2, file_out, user);
    }

    return GINT_TO_POINTER(res);
}
static GSList *read_file(const gchar *filename)
{
    xmlDoc *doc = NULL;

    PROGRESS_STATUS("Загрузка информации об объектах 1с");//, filename);

    doc = xmlReadFile(filename, NULL, 0);
    if ( !doc ) {
        xmlErrorPtr x_error = 0;
        x_error = xmlGetLastError();
        do_log(LOG_ERR, "Файл \"%s\" не распознается.\n%s", file_, x_error->message);
        return FALSE;
    }

    if ( CANCELLED )
        return FALSE;

    xmlNode *root;

    root = xmlDocGetRootElement(doc);
    if ( !root || !str_iqual((gchar*)root->name, ELEM_1C_ROOT) ) {
        do_log(LOG_ERR, "Файл \"%s\" не является файлом обмена", file_);
        return FALSE;
    }

    xmlNode *child;
    GSList  *objs = NULL, *l;
    ExchangeObject *obj;

    child = root->xmlChildrenNode;
    gint object_total=0, object_count=0;

    while ( child ) {
        child = child->next;
        object_total++;
    }

    child = root->xmlChildrenNode;
    while ( child ) {
        if ( CANCELLED )
            return FALSE;
        if ( !parse_obj(child, &obj) ) {
            exchange_objects_free(objs);
            return FALSE;
        }
        if ( obj )
            objs = g_slist_append(objs, obj);
        child = child->next;
        object_count++;
        PROGRESS_PROGRESS(object_count, object_total);
    }
    for ( l=objs; l; l=l->next ) {
        if ( CANCELLED )
            return FALSE;
        GSList *p;
        obj = l->data;
        for ( p=obj->properties; p; p=p->next ) {
            ExchangeProperty *prop;
            prop = p->data;
            if ( prop->kind == EXCHANGE_TYPE_KIND_HANDBOOK ) {
                ExchangeKey *key;
                if ( prop->empty )
                    continue;
                key = g_value_get_pointer(&prop->value);
                if ( !find_exchange_object_by_key(objs, key) ) {//!!
                    MY_ERROR("Строка %d. Элемент с %s \"%s\" не найден. ", prop->line, ELEM_1C_GUID, key->guid);
                    exchange_objects_free(objs);
                    return FALSE;
                }

            }
        }
    }
    return objs;

}
static gboolean clear_link(const gchar *aliases_name)
{
    if ( !do_message_question("Вы действительно хотите очистить все данные?") )
        return FALSE;

    do_string_list_t *l;
    l = do_string_list_new_from_csv(aliases_name);

    do_alias_t *alias;
    alias = domino_alias_new(l->list[0]);

    if ( !alias || !do_alias_open(alias, TRUE) ) {
        MY_ERROR("Невозможно открыть алиас \"%s\"", (char*)l->list[0]);
        do_string_list_free(l);
        return FALSE;
    }
    if ( !do_alias_tran_begin(alias) ) {
        MY_ERROR("Невозможно начать транзакцию \"%s\"", (char*)l->list[0]);
        do_string_list_free(l);
        return FALSE;
    }
    gboolean res;
    res = TRUE;
    int status;
    partner_data_key1_t partner_data_key1;
    partner_data_rec_t partner_data;
    partner_key0_t partner_key0;

    partner_data_key1.type = PARTNER_DATA_DOMINO_TYPE;
    do_text_set_empty(partner_data_key1.g_code);
    partner_data_key1.code = 0;

    status = do_partner_data_get1(alias, &partner_data, &partner_data_key1, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( CANCELLED ) {
            res = FALSE;
            break;
        }
        if ( partner_data_key1.type != PARTNER_DATA_DOMINO_TYPE ) break;

        if ( do_partner_data_delete(alias) != DO_OK ) {
            res = FALSE;
            break;
        }
        do_cpy(partner_key0.g_code, partner_data_key1.g_code);
        partner_key0.code = partner_data_key1.code;

        if ( !do_protocol_partner_add_key(alias, &partner_key0, DO_PROTOCOL_ACTION_CHANGE) ) {
            res = FALSE;
            break;
        }

        status = do_partner_data_get1(alias, &partner_data, &partner_data_key1, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        res = FALSE;

    if ( res )
        res = do_alias_tran_end(alias);
    else
        do_alias_tran_abort(alias);

    do_alias_free(alias);
    do_string_list_free(l);


    return TRUE;
}
