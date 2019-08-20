
#ifdef ARTIX
#include <domino.h>
#include "artix_send.h"

static int code_is_equal(const char *code1, const char *code2, size_t len1, size_t len2);
static int send_barcode(artix_t *artix, artix_session_t *session_product, artix_session_t *session_price, do_alias_t *alias,
                        product_rec_t *product, barcode_key0_t *barcode_key0, int nretail_price);
typedef struct PACK_STRUCT {
    BTI_CHAR barcode[DOMINO_FIELD_BCODE_SIZE];
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
} barcodes_ret;
static do_ret_list_t *get_barcodes_from_product_code_list(do_alias_t *alias, do_string_list_t *product_code_list);

static int send_products(artix_t *artix, do_alias_t *alias, const char* sklad, do_proc_state_t *proc_state, do_string_list_t *product_code_list);
static int send_cashiers(artix_t *artix, do_alias_t *alias, const char* sklad, do_proc_state_t *proc_state);
static int send_keyboard(artix_t *artix, do_alias_t *alias, const char* sklad, /*!!int cash_code,*/ do_proc_state_t *proc_state);


#define RUNNING (do_proc_state_get_state(proc_state) == DO_STATE_RUN)
static int artix_send_objects(artix_t *artix, do_alias_t *alias, do_proc_state_t *proc_state, const char *sklad, /*!!int cash_code,*/ int objs, do_string_list_t *obj_list)
{
    artix_info_upload_state_t state;
    do_proc_state_set_persent(proc_state, 0);
    if (!artix_info_get_upload_state(artix->info, &state))
        return FALSE;
    if (state != upsNone) {
        do_log(LOG_WARNING, "Another artix upload proccess is active");
        return FALSE;
    }
    if (!artix_info_start_upload(artix->info))
        return FALSE;

    artix_info_clear_upload_error_description(artix->info);

    if (!artix_multi_session_start(artix->alias)) {
        artix_info_add_upload_error_description(artix->info, "Транзакция не открывается");
        artix_info_set_upload_state(artix->info, upsNone);
        return 0;
    }

    artix_info_set_upload_progress(artix->info, 1); // just in case
    do_proc_state_set_persent(proc_state, 1);

    int result = 1;

    if (objs & ARTIX_SEND_PRODUCT)
        result = send_products(artix, alias, sklad, proc_state, obj_list);
    if (result && objs & ARTIX_SEND_CASHIER)
        result = send_cashiers(artix, alias, sklad, proc_state);
    if (result && objs & ARTIX_SEND_KEYBOARD)
        result = send_keyboard(artix, alias, sklad, /*!!cash_code,*/ proc_state);
    if (result)
        result = artix_multi_session_commit(artix->alias);
    else
        artix_multi_session_abort(artix->alias);

    do_proc_state_set_persent(proc_state, 0);
    artix_info_set_upload_progress(artix->info, 0); // just in case
    if (!result)
        artix_info_set_upload_state(artix->info, upsNone);
    else
        artix_info_success_upload(artix->info);
    return result;
}
int artix_send(artix_t *artix, do_alias_t *alias, do_proc_state_t *proc_state, const char *sklad, /*!!int cash_code,*/ int objs, do_string_list_t *obj_list)
{
    int res;
    if ( !objs )
        return TRUE;
    res = artix_send_objects(artix, alias, proc_state, sklad, objs, obj_list);
#ifndef OLD_ARTIX_INFO
    if ( !res )
        artix_info_set_alert(artix->info, ARTIX_ALERT_EXCHANGE_ERROR, NULL);
    else
        artix_info_clear_alert(artix->info, ARTIX_ALERT_EXCHANGE_ERROR);
#endif
    return res;
}
static int code_is_equal(const char *code1, const char *code2, size_t len1, size_t len2)
{
    if (memcmp(code1, code2, (len1 < len2) ? len1 : len2))
        return 0;
    return (len1 == len2) ||
         ((len1 < len2) ? (code2[len1] == ' ') : (code1[len2] == ' '));
}
#ifndef CORRECT_PRODUCT_BASE
static int is_base_and_only_parcel_send(do_alias_t *alias, product_rec_t *product)
{
    if (product->data.base_parcel != 0 ||
        do_product_param_int(alias, product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) !=
          DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE
        )
        return 0;
    product_key0_t product_key0;
    barcode_key1_t barcode_key1;
    int status;

    do_cpy(product_key0.code, product->data.code);
    product_key0.base_parcel = 1;

    status = do_product_key0(alias, &product_key0, DO_GET_GT);
    while (status == DO_OK) {
        if (memcmp(&product_key0.code, &product->data.code, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)))
            break;

        do_cpy(barcode_key1.code, product_key0.code);
        if (do_barcode_key1(alias, &barcode_key1, DO_GET_EQUAL) != DO_OK)
            return 0;

        status = do_product_key0(alias, &product_key0, DO_GET_NEXT);
    }
    if (status == DO_ERROR)
        return 0;
    return 1;
}
#else
inline static int is_base_and_only_parcel_send(do_alias_t *alias, product_rec_t *product)
{
    return !product->data.base_parcel &&
            do_product_param_int(alias, product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) ==
              DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE
}
#endif

static int send_barcode(artix_t *artix, artix_session_t *session_product, artix_session_t *session_price, do_alias_t *alias, product_rec_t *product, barcode_key0_t *barcode_key0, int nretail_price)
{
    artix_product_t artix_product;
    artix_price_t artix_price;

    if ((artix_product.price = do_product_price(alias, product, nretail_price)) <= 0)
        return 1;
    int result = 1;
    char *value;
    int  measure;
    double coef;

    value = do_text(alias, product->data.code);
    artix_text_set(artix->alias, artix_product.code, value);
    do_free(value);

    value = do_text(alias, product->data.name);
    artix_text_set(artix->alias, artix_product.name, value);
    do_free(value);

    value = do_text(alias, product->data.unit);
    artix_product.unpack_measure = do_unit_measure_code(value);
    do_free(value);

    value = do_text(alias, product->data.tech_unit);
    measure = do_unit_measure_code(value);
    if (!measure)
        measure = artix_product.unpack_measure;
    do_free(value);


    coef = do_product_coef(alias, product);

    value = do_text(alias, barcode_key0->barcode);
    artix_text_set(artix->alias, artix_product.bcode, value);
    do_free(value);
    artix_product.quant_limit = 0;
    if (coef > 1.001 &&
        code_is_equal(barcode_key0->barcode, product->data.code,
                      sizeof(barcode_key0->barcode), sizeof(product->data.code))
        )
        artix_product.coef = 1;
    else
        artix_product.coef = coef;

    if (artix_product.coef > 1.001) {
        artix_product.measure = measure;
        memcpy(&artix_price.bcode, artix_product.bcode, sizeof(artix_price.bcode));
        artix_price.price = artix_product.price;
        artix_price.indx = ARTIX_UNPACK_PRICE_INDEX;
        result = artix_session_add_price(session_price, &artix_price);
        artix_product.price *= artix_product.coef;
    }
    else
        artix_product.measure = artix_product.unpack_measure;

    return result && artix_session_add_product(session_product, &artix_product);
}

static int send_products(artix_t *artix, do_alias_t *alias, const char *sklad, do_proc_state_t *proc_state, do_string_list_t *product_code_list)
{
    int status, indx, percent = 1, result = 0;
    char *value;
    do_ret_list_t *list = NULL;
    barcodes_ret *ret;
    product_key4_t product_key4;

    artix_session_t *session_product = NULL, *session_price = NULL, *session_measure = NULL;
    int nretail_price = domino_nretail_price(alias, sklad);
    product_rec_t product = {0, {"\0","\0","\0","\0"}};
    artix_measure_t measure;

    if (!product_code_list || !product_code_list->count) {
        if ((session_product = artix_session_new(artix->alias, ARTIX_OBJECT_PRODUCT, ARTIX_ACTION_REPLACE)) == NULL)
            artix_info_add_upload_error_description(artix->info, "Товарная сессия не открывается");
        else
        if ((session_price = artix_session_new(artix->alias, ARTIX_OBJECT_PRICE, ARTIX_ACTION_REPLACE)) == NULL)
            artix_info_add_upload_error_description(artix->info, "Ценовая сессия не открывается");
        else
        if ((session_measure = artix_session_new(artix->alias, ARTIX_OBJECT_MEASURE, ARTIX_ACTION_REPLACE)) == NULL)
            artix_info_add_upload_error_description(artix->info, "Сессия единиц измерения не открывается");
        else
        list = get_barcodes_from_product_code_list(alias, NULL);
        if (list) {
            for (indx = 0; result && indx < do_unit_measure_count(); indx++) {
                measure.code = indx;
                artix_text_set(artix->alias, measure.name, do_unit_measure_name(indx));
                measure.flag = 0;
                result = artix_session_add_measure(session_measure, &measure);
            }
            result = 1;
        }
    }
    else {
        if ((session_product = artix_session_new(artix->alias, ARTIX_OBJECT_PRODUCT, ARTIX_ACTION_ADD)) == NULL)
            artix_info_add_upload_error_description(artix->info, "Товарная сессия не открывается");
        else
        if ((session_price = artix_session_new(artix->alias, ARTIX_OBJECT_PRICE, ARTIX_ACTION_ADD)) == NULL)
            artix_info_add_upload_error_description(artix->info, "Ценовая сессия не открывается");
        else {
            list = get_barcodes_from_product_code_list(alias, product_code_list);
            if (list)
                result = 1;
        }
    }
    if (!list) {
        artix_info_add_upload_error_description(artix->info, "Ошибка чтения продажных кодов");
        result = 0;
    }
    for (indx = 0; result && indx < do_ret_list_count(list); indx++) {
        ret = do_ret_list_item_data(list, indx);
        do_cpy(product_key4.code, ret->code);
        if (!RUNNING) {
            result = 0;
            break;
        }
        if (do_cmp(product.data.code, product_key4.code)) {
            result = (status = do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL)) != DO_ERROR;
            if (status != DO_OK) {
                if (status == DO_KEY_NOT_FOUND) {
                    value = do_text(alias, ret->code);
                    do_log(LOG_WARNING, "Product \"%s\" not found", value);
                    do_free(value);
                }
                continue;
            }
        }
        if (result && !is_base_and_only_parcel_send(alias, &product))
            result = send_barcode(artix, session_product, session_price, alias, &product, (barcode_key0_t*)&(ret->barcode), nretail_price);
        if (percent + 1 < (indx * 100 / do_ret_list_count(list))) {
            percent =  indx * 100 / do_ret_list_count(list) + 1;
            artix_info_set_upload_progress(artix->info, percent);
            do_proc_state_set_persent(proc_state, percent);
        }
    }
    if (list)
        do_ret_list_free(list);
    if (session_product) {
        if (!result)
            artix_session_abort(session_product);
        else
            result = artix_session_commit(session_product);
        artix_session_free(session_product);
    }
    if (session_price) {
        if (!result)
            artix_session_abort(session_price);
        else
            result = artix_session_commit(session_price);
        artix_session_free(session_price);
    }
    if (session_measure) {
        if (!result)
            artix_session_abort(session_measure);
        else
            result = artix_session_commit(session_measure);
        artix_session_free(session_measure);
    }
    return result;
}
static int send_cashiers(artix_t *artix, do_alias_t *alias, const char *sklad, do_proc_state_t *proc_state)
{
    int status, result = 0;
    artix_session_t *session_cashier = NULL;

    if ((session_cashier = artix_session_new(artix->alias, ARTIX_OBJECT_CASHIER, ARTIX_ACTION_REPLACE)) == NULL)
        artix_info_add_upload_error_description(artix->info, "Cессия кассиров не открывается");
    else
        result = 1;

    if (result) {
        int count = 0;
        partner_rec_t partner;
        partner_key0_t partner_key0, partner_key;
        artix_cashier_t artix_cashier;
        do_text_set(alias, partner_key0.g_code, do_param(DO_PARAM_PARTNER_REGION_CASHIER));
        memcpy(&partner_key, &partner_key0, sizeof(partner_key0.g_code));
        partner_key0.code = 0;
        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_GE);
        char *value, *unit;
        while (status == DO_OK && result) {
            if (!RUNNING) {
                result = 0;
                break;
            }
            if (memcmp(&partner_key, &partner_key0, sizeof(partner_key0.g_code))) break;
            count++;

            unit = do_text(alias, partner.data.sklad);
            if ( !unit[0] || !strcmp(unit, sklad) ) {
        	value = (char*)do_malloc(100);
        	sprintf(value, "%d", (int)partner.data.code);
        	artix_text_set(artix->alias, artix_cashier.code, value);
        	do_free(value);
        	value = do_text(alias, partner.data.name);
        	artix_text_set(artix->alias, artix_cashier.name, value);
        	do_free(value);
        	value = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_CASHIER_LOGIN));
        	artix_text_set(artix->alias, artix_cashier.login, value);
        	do_free(value);
        	value = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_CASHIER_PASSWORD));
        	artix_text_set(artix->alias, artix_cashier.passwd, value);
        	do_free(value);
        	artix_cashier.role = do_partner_param_int(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_CASHIER_ROLE));
        	result = artix_session_add_cashier(session_cashier, &artix_cashier);
    	    }
    	    do_free(unit);
    	    if ( !result ) break;
            status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_NEXT);
        }
        if (result && !count) {
            result = 0;
            artix_info_add_upload_error_description(artix->info, "Ни одного кассира не найдено");
        }
    }
    if (session_cashier) {
        if (!result)
            artix_session_abort(session_cashier);
        else
            result = artix_session_commit(session_cashier);
        artix_session_free(session_cashier);
    }
    return result;
}
//static const char *key_layout[2] ={"qwertyuiop[]asdfghjkl;'zxcvbnm,./",
                                   //"QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?"};
static int send_keyboard(artix_t *artix, do_alias_t *alias, const char *sklad, /*!!int cash_code,*/ do_proc_state_t *proc_state)
{
    int status, result = 0;
    //int i,j,f;
    artix_session_t *session_keyboard = NULL;

    if ((session_keyboard = artix_session_new(artix->alias, ARTIX_OBJECT_KEY, ARTIX_ACTION_REPLACE)) == NULL)
        artix_info_add_upload_error_description(artix->info, "Cессия горячих клавиш не открывается");
    else
        result = 1;

    document_rec_t document;
    document_key0_t document_key0, document_key;
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    artix_key_t artix_keyboard;
    /*!!partner_rec_t partner;
    partner_key0_t partner_key0;
    do_text_set(alias, partner_key0.g_code, do_param(DO_PARAM_PARTNER_REGION_CASH));
    partner_key0.code = cash_code;
    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
    if (status != DO_OK) {
        do_log(LOG_ERR, "Ошибка чтения информации о кассе с номером %d", cash_code);
        result = 0;
    }*/
    if (result) {
        do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_KEYBOARD));
        do_text_set(alias, document_key0.sklad, sklad);
        memcpy(&document_key, &document_key0, sizeof(document_key0.dtype) + sizeof(document_key0.sklad));
        do_text_set(alias, document_key0.document, "");
        //!!status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
        int kind;

        status = do_document_get0(alias, &document, &document_key0, DO_GET_GE);
        while (status == DO_OK && result) {
            if (!RUNNING) {result = 0; break;}

            if (memcmp(&document_key, &document_key0, sizeof(document_key0.dtype) + sizeof(document_key0.sklad))) break;
            kind = do_document_param_int(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND));
            //for (i = 0; i < 2; i++) {
                artix_keyboard.code = atoi(document.data.document);
               /* if (i) {
                    f = 0;
                    for (j = 0; j < 33; j++)
                        if (key_layout[0][j] == artix_keyboard.code) {
                            artix_keyboard.code = key_layout[1][j];
                            f = 1;
                            break;
                        }
                    if (!f)
                        break;
                }*/
                char *code, *list;
                int len;
                if (kind && artix_keyboard.code) {
                    switch (kind) {
                        case DO_CONST_KEY_KIND_BCODE:
                            artix_text_set(artix->alias, artix_keyboard.command, "BCODE");
                            code = do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_PRODUCT));
                            artix_text_set(artix->alias, artix_keyboard.params, code);
                            do_free(code);
                            result = artix_session_add_key(session_keyboard, &artix_keyboard);
                            break;
                        case DO_CONST_KEY_KIND_BCODELIST:
                            artix_text_set(artix->alias, artix_keyboard.command, "BCODELIST");
                            list = (char*)do_malloc(1024);
                            len = 0;
                            memcpy(&document_order_key0, &document_key0, sizeof(document_key0));
                            document_order_key0.line = 0;
                            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
                            while (status == DO_OK && result && len < 1000) {
                                if (!RUNNING) {result = 0; break;}
                                if (memcmp(&document_order_key0, &document_key0, sizeof(document_key0))) break;
                                code = do_text(alias, document_order.data.code);
                                strcpy(list + len, code);
                                len += strlen(code);
                                list[len++] = ',';
                                do_free(code);
                                status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
                            }
                            if (status == DO_ERROR) result = 0;
                            if (len > 0) {
                                list[len] = '\0';
                                artix_text_set(artix->alias, artix_keyboard.params, list);
                            }
                            do_free(list);
                            if (result && len)
                                result = artix_session_add_key(session_keyboard, &artix_keyboard);
                            break;
                        case DO_CONST_KEY_KIND_COMMAND:
                            artix_text_set(artix->alias, artix_keyboard.command, do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_COMMAND)));
                            artix_text_set(artix->alias, artix_keyboard.params, "");
                            result = artix_session_add_key(session_keyboard, &artix_keyboard);
                            break;
                        default:
                            code = NULL;
                    }
                //}
            }
            status = do_document_get0(alias, &document, &document_key0, DO_GET_NEXT);
        }
        if (status == DO_ERROR) result = 0;
    }
    if (session_keyboard) {
        if (!result)
            artix_session_abort(session_keyboard);
        else
            result = artix_session_commit(session_keyboard);
        artix_session_free(session_keyboard);
    }
    return result;
}
#define MAX_BARCODES_FOR_PRODUCT 100

static do_ret_list_t *get_barcodes_from_product_code_list(do_alias_t *alias, do_string_list_t *product_code_list)
{
    barcode_key1_t barcode_key1;
    do_extended_t *extended;
    do_ret_list_t *retval = NULL;
    int indx;

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_BARCODE);
    do_extended_set_fields(extended, 2,
                           do_size_and_offset(barcode_struct_t, barcode),
                           do_size_and_offset(barcode_struct_t, code));
    if (!product_code_list) {
        do_text_set_empty(barcode_key1.code);
        do_extended_set_key(extended, 1, &barcode_key1);
        retval = do_extended_get(extended, -1);
    }
    else {
        do_extended_set_filter(extended, TRUE, 1, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH));
        do_extended_set_filter_term(extended, 0, DO_EXTENDED_TYPE_STRING,
                                offsetof(barcode_struct_t, code), DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO, &barcode_key1.code);
        for (indx = 0; indx < product_code_list->count; indx++) {
            do_text_set(alias, barcode_key1.code, product_code_list->list[indx]);
            do_extended_set_key(extended, 1, &barcode_key1);
            do_extended_set_filter_term_set_value(extended, 0, &barcode_key1.code);
            retval = do_ret_list_add_list_and_free(retval,
                                        do_extended_get(extended, MAX_BARCODES_FOR_PRODUCT));
        }
    }
    return retval;
}
#endif
