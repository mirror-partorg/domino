

#include "do_rz_load.h"
#include "domino.h"
#include "do_rz_select.h"
#include "do_tree_view.h"
#include <domino.h>
#include "hig.h"
#include "do_input_csv.h"

#define DATE_NAME "DATE"
#define FILE_NAME "ORDER"

typedef struct {
    gint code;
    do_string_list_t *string;
    gchar **alias;
    gint alias_count;
} ValutaInfo;
/*
static gboolean do_rz_load_insert_document(do_alias_t *alias, document_order_rec_t *document_order)
{
    document_rec_t document;
    document_key0_t document_key0;

    do_cpy(document_key0.dtype, document_order->data.dtype);
    do_cpy(document_key0.sklad, document_order->data.sklad);
    do_cpy(document_key0.document, document_order->data.document);

    switch (do_document_key0(alias, &document_key0, DO_GET_EQUAL)) {
        case DO_OK:
            return TRUE;
        case DO_KEY_NOT_FOUND: {
            memset(&document.data, 0, sizeof(document.data));
            document.size = sizeof(document.data) - sizeof(document.data.params);
            do_cpy(document.data.dtype, document_key0.dtype);
            do_text_set(alias, document.data.dtype1, "1");
            do_cpy(document.data.sklad, document_key0.sklad);
            do_cpy(document.data.document, document_key0.document);
            do_text_set_empty(document.data.name);
            time_t now = time(NULL);
            struct tm tm = *localtime(&now);
            do_date_set(&document.data.date, tm);
            do_time_set(&document.data.time, tm);
            do_text_set(alias, document.data.p_g_code, "00");
            document.data.p_code = 1;
            do_text_set_empty(document.data.p_doc);
            do_text_set_empty(document.data.party);
            if (do_document_insert(alias, &document) == DO_ERROR)
                return FALSE;
            return TRUE;
        }
        default :
            return FALSE;
    }
}

#define DEFAULT_COL_DEFAULT_WIDTH 100
static gboolean do_rz_load_order_insert(do_alias_t *alias, rz_info_t *rz_info)
{
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    gboolean retval = FALSE;
    int indx;

    for (indx = 1; indx < 100 && !retval; indx++) {
        do_rz_info_barcode_to_document_order_key0(rz_info, alias, &document_order_key0);
        document_order_key0.line += indx;
        switch (do_document_order_key0(alias, &document_order_key0, DO_GET_EQUAL)) {
            case DO_KEY_NOT_FOUND: {
                rz_info->key.dtype = do_text(alias, document_order_key0.dtype);
                rz_info->key.sklad = do_text(alias, document_order_key0.sklad);
                rz_info->key.document = do_text(alias, document_order_key0.document);
                rz_info->key.line = document_order_key0.line;

                do_rz_info_key_to_document_order(rz_info, alias, &document_order);
                if (do_document_order_insert(alias, &document_order) == DO_OK )
                    retval = TRUE;
                else
                    return FALSE;
                break;
            }
            case DO_ERROR:
                return FALSE;
            default:
                break;
        }
    }
    if (!retval)
        return FALSE;

    return do_rz_load_insert_document(alias, &document_order) &&
           do_protocol_document_order_add_rec(alias, &document_order, DO_PROTOCOL_ACTION_INSERT_LINE);
}
static gboolean do_rz_load_order_update(do_alias_t *alias, rz_info_t *rz_info, rz_info_t *rz_old)
{
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    do_list_t *list;
    gboolean retval;

    do_rz_info_key_to_document_order_key0(rz_old, alias, &document_order_key0);
    if (do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_EQUAL) != DO_OK)
        return FALSE;
    rz_info->key.dtype = g_strdup(rz_old->key.dtype);
    rz_info->key.sklad = g_strdup(rz_old->key.sklad);
    rz_info->key.document = g_strdup(rz_old->key.document);
    rz_info->key.line = rz_old->key.line;

    do_rz_info_key_to_document_order(rz_info, alias, &document_order);
    do_cpy(document_order.data.dtype, document_order_key0.dtype);
    do_cpy(document_order.data.sklad, document_order_key0.sklad);
    do_cpy(document_order.data.document, document_order_key0.document);
    document_order.data.line = document_order_key0.line;

    retval = do_document_order_update(alias, &document_order) == DO_OK;
    if (retval) {
        list = do_rz_info_util_mark_link(alias, rz_info->barcode);
        if (list)
            do_list_free(list);
        else
            retval = FALSE;
    }
    if (retval)
        retval = do_protocol_document_order_add_rec(alias, &document_order, DO_PROTOCOL_ACTION_CHANGE_LINE);

    return retval;
}
static gboolean do_rz_load_from_file(GtkWidget *parent, const gchar *filename)
{
    DoInputCsv *csv;
    GError *error = NULL;
    ValutaInfo *val_info;
    gint val_count;
    gboolean retval = FALSE;
    rz_info_t rz;
    rz_info_t *rz_info;
    do_list_t *rec_list;
    struct tm tm;
    gchar *value;
    int line;
    struct {
        gint insert;
        gint update;
        gint change;
        gint ignore;
    } modify = {0,0,0,0};

    rz.order.number = NULL;
    rz.order.time = time(NULL);

    do_rz_load_set_valuta_alias(&val_info, &val_count);
    csv = do_input_csv_new_from_uri(filename, &error);
    if (error) {
        do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, error->message);
        g_error_free (error);
        return FALSE;
    }
    GList *list = NULL, *l, *alias_list = NULL;
    do_alias_t *default_alias;
    list = DOMINO_COMMON_PROPERTIES("MainAlias");
    //list = g_list_append(list, "net1");
    for (l = list; l ; l = l->next) {
        do_alias_t *alias;
        alias = domino_alias_new(l->data);
        if (!alias || !do_alias_open(alias, 1)) {
            do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Невозможно открыть БД");
            if (alias)
                do_alias_free(alias);
            goto do_rz_load_from_file_end;
        }
        alias_list = g_list_append(alias_list, alias);
        default_alias = alias;
    }

    while (do_input_csv_read_line(csv, &error))
    {
        if (do_input_csv_is_name(csv, DATE_NAME)) {
            value = do_input_csv_get_string(csv);
            if (!do_atodate(value, &tm))
                rz.order.time = time(NULL);
            else
                rz.order.time = mktime(&tm);
            g_free(value);
            continue;
        }
        if (do_input_csv_is_name(csv, FILE_NAME)) {
            if (rz.order.number)
                g_free(rz.order.number);
            rz.order.number = do_input_csv_get_string(csv);
            continue;
        }
        line = do_input_csv_get_int(csv);
        value = do_input_csv_get_string(csv);
        rz.mnn        = do_text_utf8_make_compatibility_cp866(value);
        g_free(value);
        value = do_input_csv_get_string(csv);
        rz.trade_name = do_text_utf8_make_compatibility_cp866(value);
        g_free(value);
        value = do_input_csv_get_string(csv);
        rz.pack_form  = do_text_utf8_make_compatibility_cp866(value);
        g_free(value);
        rz.pack_quant = do_input_csv_get_int(csv);
        rz.barcode    = do_input_csv_get_string(csv);
        if (rz.barcode[0] == '\0' ) {
            do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка в строке %d:%d неопределен продажный код", line, do_input_csv_crnt_line(csv));
            goto do_rz_load_from_file_end;
        }
        rz.price      = do_input_csv_get_double(csv, &value);
        if (!value)
            value     = do_input_csv_get_string(csv);
        rz.valuta     = do_rz_load_get_valuta(value, val_info, val_count);
        if (!rz.valuta) {
            do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка в строке %d:%d неопределена валюта %s", line,do_input_csv_crnt_line(csv), value);
            g_free(value);
            goto do_rz_load_from_file_end;
        }
        value = do_input_csv_get_string(csv);
        rz.manufactor = do_text_utf8_make_compatibility_cp866(value);
        g_free(value);
        if (rz.price < 0.00001) {
            do_log(LOG_ERR, "line %d error", do_input_csv_crnt_line(csv));
            do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка в строке %d:%d неопределена цена.", line,do_input_csv_crnt_line(csv));
            goto do_rz_load_from_file_end;
        }
        value = do_input_csv_get_string(csv);
        if (*value != '\0') {
            if (rz.order.number)
                g_free(rz.order.number);
            rz.order.number = value;
        }
        else
            g_free(value);
        value = do_input_csv_get_string(csv);
        if (*value != '\0') {
            if (!do_atodate(value, &tm))
                rz.order.time = time(NULL);
            else
                rz.order.time = mktime(&tm);
        }
        g_free(value);
        value= do_input_csv_get_string(csv);
        rz.uniq_nx = atoi(value);
        g_free(value);
        value= do_input_csv_get_string(csv);
//        if (*value != '\0')
  //          rz.unpack = 1;
        rz.unpack = !strcmp(value, "+");
        g_free(value);
        rz.link.barcode = NULL;
        rz.link.type = RZ_INFO_LINK_NO;

        if (!rz.order.number) {
            do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Неопределен номер приказа ");
            goto do_rz_load_from_file_end;
        }

        for (l = alias_list; l; l = l->next) {
            do_alias_t *alias;
            alias = l->data;
            rec_list = do_rz_info_list_from_barcode(alias, rz.barcode);
            if (!rec_list) {
                do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка чтения БД");
                goto do_rz_load_from_file_end;
            }
            if (!rec_list->count) {
                if (!do_rz_load_order_insert(alias, &rz)) {
                    do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка записи БД");
                    goto do_rz_load_from_file_end;
                }
                modify.insert++;
            }
            else {
                int i;
                rz_info = NULL;
                for (i = 0; i < rec_list->count && !rz_info; i++)
                    if (!do_rz_info_cmp(&rz, rec_list->list[i]))
                        rz_info = rec_list->list[i];
                if (!rz_info) {
                    GtkWidget *dialog;
                    GtkWidget *vbox;
                    GtkWidget *l;
                    GtkWidget *t;
                    GtkWidget *view;
                    gchar *unpack;
                    int row = 0;

                    dialog =  gtk_dialog_new_with_buttons("Повтор записи",
                                        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(parent))),
                                        GTK_DIALOG_MODAL,
                                        "Прервать", GTK_RESPONSE_CANCEL,
                                        "Добавить", GTK_RESPONSE_YES,
                                        "Заменить", GTK_RESPONSE_APPLY,
                                        "Пропустить", GTK_RESPONSE_NO,
                                        NULL);

                    g_signal_connect (dialog, "configure-event",
                                      G_CALLBACK (do_window_configure_event_cb), root_path);

                    DOMINO_PROFILE_OBJECT_INIT(GTK_OBJECT(dialog), root_path,
                              "window-position", "center-on_parent",
                              "window-size", "",
                               NULL);
                    gtk_window_set_icon_name (GTK_WINDOW(dialog), DO_STOCK_DOMINO_APPS);
                    vbox = ((GtkDialog*)dialog)->vbox;

                    t = hig_workarea_create();
                    gtk_box_pack_start (GTK_BOX(vbox), GTK_WIDGET(t), FALSE, FALSE, 0);


                    hig_workarea_add_section_title(t, &row, "Новая запись ");

                    value = g_strdup_printf("%d:%d", line, do_input_csv_crnt_line(csv));
                    l = gtk_label_new (value);
                    g_free(value);
                    gtk_label_set_line_wrap(GTK_LABEL (l), TRUE);
                    gtk_label_set_selectable (GTK_LABEL (l), TRUE);
                    hig_workarea_add_row( t, &row, "Строка:", l, NULL );

                    l = gtk_label_new (rz.mnn);
                    gtk_label_set_line_wrap(GTK_LABEL (l), TRUE);
                    gtk_label_set_selectable (GTK_LABEL (l), TRUE);
                    hig_workarea_add_row( t, &row, "МНН:", l, NULL );

                    l = gtk_label_new (rz.trade_name);
                    gtk_label_set_selectable (GTK_LABEL (l), TRUE);
                    gtk_label_set_line_wrap(GTK_LABEL (l), TRUE);
                    hig_workarea_add_row( t, &row, "Торг. наименование:", l, NULL );

                    l = gtk_label_new (rz.pack_form);
                    gtk_label_set_line_wrap(GTK_LABEL (l), TRUE);
                    gtk_label_set_selectable (GTK_LABEL (l), TRUE);
                    hig_workarea_add_row( t, &row, "Форма выпуска:", l, NULL );

                    value = g_strdup_printf("%d", rz.pack_quant);
                    l = gtk_label_new (value);
                    g_free(value);
                    gtk_label_set_selectable (GTK_LABEL (l), TRUE);
                    hig_workarea_add_row( t, &row, "Количество:", l, NULL );

                    l = gtk_label_new (rz.manufactor);
                    gtk_label_set_line_wrap(GTK_LABEL (l), TRUE);
                    gtk_label_set_selectable (GTK_LABEL (l), TRUE);
                    hig_workarea_add_row( t, &row, "Производитель:", l, NULL );

                    l = gtk_label_new (rz.barcode);
                    gtk_label_set_selectable (GTK_LABEL (l), TRUE);
                    hig_workarea_add_row( t, &row, "Прод.код:", l, NULL );

                    if (rz.unpack)
                        unpack = g_strdup("(Цена для первичной упаковки)");
                    else
                        unpack = "";
                    value = g_strdup_printf("%.2f %s %s", rz.price, do_get_valuta_name(rz.valuta), unpack);
                    l = gtk_label_new (value);
                    g_free(value);
                    g_free(unpack);
                    gtk_label_set_selectable (GTK_LABEL (l), TRUE);
                    hig_workarea_add_row( t, &row, "Цена:", l, NULL );

                    hig_workarea_add_section_title(t, &row, "Имеющиеся записи ");
                    hig_workarea_finish(t, &row);

                    view = do_rz_select_new(do_alias_get_name(alias), rz.barcode);
                    g_object_set(view, "rz-main", &rz, NULL);
                    gtk_box_pack_start (GTK_BOX(vbox), GTK_WIDGET(view), TRUE, TRUE, 0);

                    gtk_widget_show_all(dialog);
                    do_tree_view_grab_focus((DoTreeView*)view);
                    switch (gtk_dialog_run(GTK_DIALOG(dialog))) {
                        case GTK_RESPONSE_CANCEL:
                            gtk_widget_destroy(dialog);
                            goto do_rz_load_from_file_end;
                        case GTK_RESPONSE_YES:
                            if (!do_rz_load_order_insert(alias, &rz)) {
                                do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка записи БД");
                                gtk_widget_destroy(dialog);
                                goto do_rz_load_from_file_end;
                            }
                            modify.insert++;
                            break;
                        case GTK_RESPONSE_APPLY: {
                            rz_info_t *rz_update;
                            rz_update = do_rz_select_get_select((DoRzSelect*)view);
                            if (rz_update) {
                                if (!do_rz_load_order_update(alias, &rz, rz_update)) {
                                    do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка записи БД");
                                    gtk_widget_destroy(dialog);
                                    goto do_rz_load_from_file_end;
                                }
                                modify.change++;
                            }
                            else {
                                if (!do_rz_load_order_insert(alias, &rz)) {
                                    do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка записи БД");
                                    gtk_widget_destroy(dialog);
                                    goto do_rz_load_from_file_end;
                                }
                                modify.insert++;
                            }
                            break;
                        }
                        default:
                            modify.ignore++;
                            break;
                    }
                    gtk_widget_destroy(dialog);
                }
                else {
                    if (!do_rz_load_order_update(alias, &rz, rz_info)) {
                        do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка записи БД");
                        goto do_rz_load_from_file_end;
                    }
                    modify.update++;
                }
                do_rz_info_list_free(rec_list);
            }
        }
        do_rz_info_clear(&rz);
    }
    if (error) {
        do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, error->message);
        g_error_free (error);
    }
    else
        do_message_dialog_show(parent, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                      "Разбор выполнен:\nдобавлено: %d\nобновлено: %d\nизмененно: %d\nпропушено: %d\ncтрок: %d",
                       modify.insert, modify.update, modify.change, modify.ignore, do_input_csv_crnt_line(csv));
    retval = !error;

do_rz_load_from_file_end:
    if (alias_list) {
        for (l = alias_list; l; l = l->next)
            do_alias_free(l->data);
        g_list_free(alias_list);
    }

    g_object_unref(csv);
    return retval;
}
*/
/*
gboolean do_rz_load(GtkWidget *parent)
{
    GtkWidget *dialog;
    GtkWidget *win = NULL;
    GtkWidget *win1 = NULL;
    if (parent)
        win1 = gtk_widget_get_toplevel(parent);
    if (win1)  {
        if (!GTK_WIDGET_VISIBLE(win1)) {
            if (gtk_window_get_transient_for (GTK_WINDOW(win1)))
                win = GTK_WIDGET(gtk_window_get_transient_for (GTK_WINDOW(win1)));
        }else
            win = win1;
    }

    dialog = gtk_file_chooser_dialog_new ("Открыть файл загрузки",
				      GTK_WINDOW(win),
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        gboolean retval;
        //filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        filename = gtk_file_chooser_get_uri (GTK_FILE_CHOOSER (dialog));

        gtk_widget_destroy (dialog);
        //open_file (filename);
        retval = do_rz_load_from_file(parent, filename);
        g_free (filename);
        return retval;
    }
    else
        gtk_widget_destroy (dialog);

    return FALSE;
}
*/
gboolean do_rz_clear(GtkWidget *parent)
{

    if (do_message_dialog_show(parent, GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO, "Вы действительно хотите очистить список?") != GTK_RESPONSE_YES)
        return FALSE;

    GList *list = NULL, *l;

    list = DOMINO_COMMON_PROPERTIES("MainAlias");
    for (l = list; l; l = l->next) {
        do_alias_t *alias;
        document_order_key0_t document_order_key0, key;
        document_order_rec_t document_order;
        int status;

        alias = domino_alias_new(l->data);
        if (!alias || !do_alias_open(alias, 1)) {
            do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Невозможно открыть БД");
            if (alias)
                do_alias_free(alias);
            return FALSE;
        }

        do_text_set(alias, document_order_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_RZN));
        do_text_set(alias, document_order_key0.sklad, do_param(DO_PARAM_COMMON_SKLAD));
        do_text_set_empty(document_order_key0.document);
        document_order_key0.line = 0;
        memcpy(&key, &document_order_key0, sizeof(key.dtype) + sizeof(key.sklad));

        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
        while (status == DO_OK) {
            if (memcmp(&key, &document_order_key0, sizeof(key.dtype) + sizeof(key.sklad)))
                break;
            if (document_order.data.accepted == RZ_INFO_LINK_NO) {
                if ((status = do_document_order_delete(alias)) != DO_OK)
                    break;
            }
            else {
                document_order.data.accepted = RZ_INFO_LINK_FAIL;
                if ((status = do_document_order_update(alias, &document_order)) != DO_OK)
                    break;
            }
            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
        }
        do_alias_free(alias);
        if (status == DO_ERROR) {
            do_message_dialog_show(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Ошибка записи БД");
            return FALSE;
        }
    }
    do_message_dialog_show(parent, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Справочник очистен");
    return TRUE;
}
