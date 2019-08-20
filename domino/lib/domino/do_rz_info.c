#include <domino.h>
#include <stddef.h>
#include "../../misc/define.h"

#define BARCODE_LEN 13
#define DOC_LEN 6
#define DECIMAL_SHIFT 100

DO_EXPORT rz_info_t *do_rz_info_new_from_document_order(do_alias_t *alias, document_order_rec_t *document_order)
{
    rz_info_t *retval = do_malloc(sizeof(rz_info_t));
    struct tm tm;

    retval->barcode    = do_document_order_param(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_BARCODE));
    retval->mnn        = do_document_order_param(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_MNN));
    retval->trade_name = do_document_order_param(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_TRADE_NAME));
    retval->pack_form  = do_document_order_param(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_PACK_FORM));
    retval->pack_quant = do_document_order_quant(alias, document_order, DO_CONST_QUANT_REST);
    retval->manufactor = do_document_order_param(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_MANUFACTOR));
    retval->price      = do_document_order_price(alias, document_order, DO_CONST_PRICE_REGISTER);
    retval->valuta     = do_document_order_param_int(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_VALUTA_CODE));
    do_date(document_order->data.date, &tm);
    retval->order.time = do_time(document_order->data.time, &tm);
    do_log(LOG_INFO,"%d %d %d",document_order->data.date,document_order->data.time,retval->order.time);//!!
    retval->order.number = do_document_order_param(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_SRC_FILENAME));
    retval->key.dtype = do_text(alias, document_order->data.dtype);
    retval->key.sklad = do_text(alias, document_order->data.sklad);
    retval->key.document = do_text(alias, document_order->data.document);
    retval->key.line  = document_order->data.line;
    retval->uniq_nx = do_document_order_param_int(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_UNIQ_NX));
    retval->unpack  = do_document_order_param_int(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_UNPACK));
    retval->link.type = document_order->data.accepted;
    if (retval->link.type != RZ_INFO_LINK_NO)
        retval->link.barcode = do_text(alias, document_order->data.code);
    else
        retval->link.barcode = do_strdup(retval->barcode);
    return retval;
}
DO_EXPORT rz_info_t *do_rz_info_new_from_document_order_key0(do_alias_t *alias, document_order_key0_t *document_order_key0)
{
    document_order_rec_t document_order;

    if (do_document_order_get0(alias, &document_order, document_order_key0, DO_GET_EQUAL) != DO_OK)
        return NULL;
    return do_rz_info_new_from_document_order(alias, &document_order);
}
DO_EXPORT rz_info_t *do_rz_info_new(do_alias_t *alias, const char *dtype, const char *sklad, const char *document, int line)
{
    document_order_key0_t document_order_key0;
    do_text_set(alias, document_order_key0.dtype, dtype);
    do_text_set(alias, document_order_key0.sklad, sklad);
    do_text_set(alias, document_order_key0.document, document);
    document_order_key0.line = line;

    return do_rz_info_new_from_document_order_key0(alias, &document_order_key0);
}

static void do_rz_info_set_key(do_alias_t *alias, const char *barcode,  BTI_CHAR *dtype, BTI_CHAR *sklad, BTI_CHAR *document, BTI_LONG *line)
{
    char barcode_buf[BARCODE_LEN + 1];
    int len;

    do_alias_text_set_(alias, dtype, do_param(DO_PARAM_DOCUMENT_TYPE_RZN), do_sizeof(document_key0_t, dtype));
    do_alias_text_set_(alias, sklad, do_param(DO_PARAM_COMMON_SKLAD), do_sizeof(document_key0_t, sklad));
    len = (strlen(barcode) > BARCODE_LEN) ? BARCODE_LEN : strlen(barcode);
    barcode_buf[BARCODE_LEN] = '\0';
    memset(barcode_buf, '0', BARCODE_LEN);
    strncpy(barcode_buf + BARCODE_LEN - len, barcode, len);
    *line = atoi(barcode_buf + DOC_LEN)*DECIMAL_SHIFT;
    barcode_buf[DOC_LEN] = '\0';
    do_alias_text_set_(alias, document, barcode_buf, do_sizeof(document_key0_t, document));
}
DO_EXPORT void do_rz_info_barcode_to_document_order_key0(rz_info_t *rz_info, do_alias_t *alias, document_order_key0_t *document_order_key0)
{
    do_rz_info_set_key(alias, rz_info->barcode,
                        document_order_key0->dtype,
                        document_order_key0->sklad,
                        document_order_key0->document,
                        &document_order_key0->line);
}
DO_EXPORT void do_rz_info_key_to_document_order_key0(rz_info_t *rz_info, do_alias_t *alias, document_order_key0_t *document_order_key0)
{
    do_text_set(alias, document_order_key0->dtype, rz_info->key.dtype);
    do_text_set(alias, document_order_key0->sklad, rz_info->key.sklad);
    do_text_set(alias, document_order_key0->document, rz_info->key.document);
    document_order_key0->line = rz_info->key.line;
}

DO_EXPORT void do_rz_info_barcode_to_document_order(rz_info_t *rz_info, do_alias_t *alias, document_order_rec_t *document_order)
{
    struct tm tm;

    memset(&document_order->data, 0, sizeof(document_order->data));
    document_order->size = sizeof(document_order->data) - sizeof(document_order->data.params);

    do_rz_info_set_key(alias, rz_info->barcode,
                        document_order->data.dtype,
                        document_order->data.sklad,
                        document_order->data.document,
                        &document_order->data.line);
    document_order->data.sort = 0;

    do_text_set(alias, document_order->data.dtype1, "1");
    tm = *localtime(&rz_info->order.time);
    do_date_set(&document_order->data.date, tm);
    do_time_set(&document_order->data.time, tm);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_MNN), rz_info->mnn);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_TRADE_NAME), rz_info->trade_name);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_PACK_FORM), rz_info->pack_form);
    do_document_order_param_int_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_VALUTA_CODE), rz_info->valuta);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_MANUFACTOR), rz_info->manufactor);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_BARCODE), rz_info->barcode);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_SRC_FILENAME), rz_info->order.number);
    do_document_order_price_set(alias, document_order, DO_CONST_PRICE_REGISTER, rz_info->price);
    do_document_order_quant_set(alias, document_order, DO_CONST_QUANT_REST, rz_info->pack_quant);
    do_document_order_param_int_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_UNIQ_NX), rz_info->uniq_nx);
    do_document_order_param_int_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_UNPACK), rz_info->unpack);
    document_order->data.accepted = rz_info->link.type;

    if (rz_info->link.type != RZ_INFO_LINK_NO)
        do_text_set(alias, document_order->data.code, rz_info->link.barcode);
    else
        do_text_set(alias, document_order->data.code, rz_info->barcode);
}

DO_EXPORT void do_rz_info_key_to_document_order(rz_info_t *rz_info, do_alias_t *alias, document_order_rec_t *document_order)
{
    struct tm tm;

    memset(&document_order->data, 0, sizeof(document_order->data));
    document_order->size = sizeof(document_order->data) - sizeof(document_order->data.params);

    do_text_set(alias, document_order->data.dtype, rz_info->key.dtype);
    do_text_set(alias, document_order->data.sklad, rz_info->key.sklad);
    do_text_set(alias, document_order->data.document, rz_info->key.document);
    document_order->data.line = rz_info->key.line;
    document_order->data.sort = 0;

    do_text_set(alias, document_order->data.dtype1, "1");
    tm = *localtime(&rz_info->order.time);
    do_date_set(&document_order->data.date, tm);
    do_time_set(&document_order->data.time, tm);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_MNN), rz_info->mnn);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_TRADE_NAME), rz_info->trade_name);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_PACK_FORM), rz_info->pack_form);
    do_document_order_param_int_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_VALUTA_CODE), rz_info->valuta);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_MANUFACTOR), rz_info->manufactor);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_BARCODE), rz_info->barcode);
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_SRC_FILENAME), rz_info->order.number);
    do_document_order_price_set(alias, document_order, DO_CONST_PRICE_REGISTER, rz_info->price);
    do_document_order_quant_set(alias, document_order, DO_CONST_QUANT_REST, rz_info->pack_quant);
    do_document_order_param_int_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_UNIQ_NX), rz_info->uniq_nx);
    do_document_order_param_int_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_UNPACK), rz_info->unpack);
    document_order->data.accepted = rz_info->link.type;

    if (rz_info->link.type != RZ_INFO_LINK_NO)
        do_text_set(alias, document_order->data.code, rz_info->link.barcode);
    else
        do_text_set(alias, document_order->data.code, rz_info->barcode);
}


DO_EXPORT void  do_rz_info_clear(rz_info_t *rz_info)
{
    do_free(rz_info->barcode);
    do_free(rz_info->mnn);
    do_free(rz_info->trade_name);
    do_free(rz_info->pack_form);
    do_free(rz_info->manufactor);
    do_free(rz_info->key.dtype);
    do_free(rz_info->key.sklad);
    do_free(rz_info->key.document);
    if (rz_info->link.barcode)
        do_free(rz_info->link.barcode);
}
DO_EXPORT void  do_rz_info_free(rz_info_t *rz_info)
{
    do_rz_info_clear(rz_info);
    do_free(rz_info->order.number);
    do_free(rz_info);
}


DO_EXPORT void do_rz_info_util_set_document_order_key0_from_barcode(do_alias_t *alias, document_order_key0_t *document_order_key0, const char* barcode)
{
    do_rz_info_set_key(alias, barcode,
                        document_order_key0->dtype,
                        document_order_key0->sklad,
                        document_order_key0->document,
                        &document_order_key0->line);
}
static int do_rz_info_add_list(do_list_t *list, do_alias_t *alias, const char* barcode)
{
    document_order_key0_t document_order_key0, document_order_key;
    document_order_rec_t document_order;
    int status;
    rz_info_t *rz_info;

    do_rz_info_set_key(alias, barcode,
                        document_order_key0.dtype,
                        document_order_key0.sklad,
                        document_order_key0.document,
                        &document_order_key0.line);

    memcpy(&document_order_key, &document_order_key0, sizeof(document_order_key0.dtype) +
                                                      sizeof(document_order_key0.sklad) +
                                                      sizeof(document_order_key0.document));
    document_order_key.line = document_order_key0.line + DECIMAL_SHIFT;
    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    while (status == DO_OK) {
        if (memcmp(&document_order_key, &document_order_key0, sizeof(document_order_key0.dtype) +
                                                      sizeof(document_order_key0.sklad) +
                                                      sizeof(document_order_key0.document)) ||
            document_order_key0.line >= document_order_key.line) break;

        rz_info = do_rz_info_new_from_document_order(alias, &document_order);
        do_list_add(list, rz_info);
        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
    }
    return status != DO_ERROR;
}

DO_EXPORT do_list_t *do_rz_info_do_list_from_barcode(do_alias_t *alias, const char* barcode)
{
    do_list_t *retval;
    retval = do_list_new(0);
    if (!do_rz_info_add_list(retval, alias, barcode)) {
        do_rz_info_list_free(retval);
        return NULL;
    }
    return retval;
}
DO_EXPORT void do_rz_info_list_free(do_list_t *list)
{
    int i;
    for (i = 0; i < list->count; i++)
        do_rz_info_free(list->list[i]);
    do_list_free(list);
}

DO_EXPORT int do_rz_info_exist_by_code(do_alias_t *alias, const char* code)
{
    barcode_key1_t barcode_key1, barcode_key;
    barcode_rec_t  barcode;
    int status;
    char *bar_code;

    do_text_set(alias, barcode_key1.code, code);
    do_cpy(barcode_key, barcode_key1);

    status = do_barcode_get1(alias, &barcode, &barcode_key1, DO_GET_EQUAL);
    while (status == DO_OK) {
        if (do_cmp(barcode_key, barcode_key1))
            break;
        bar_code = do_text(alias, barcode.data.barcode);
        if (do_rz_info_exist_by_barcode(alias, bar_code)) {
            do_free(bar_code);
            return 1;
        }
        do_free(bar_code);
        status = do_barcode_get1(alias, &barcode, &barcode_key1, DO_GET_NEXT);
    }
    return 0;
}
DO_EXPORT int do_rz_info_exist_by_barcode(do_alias_t *alias, const char* barcode)
{
    document_order_key0_t document_order_key0, document_order_key;

    do_rz_info_set_key(alias, barcode,
                        document_order_key0.dtype,
                        document_order_key0.sklad,
                        document_order_key0.document,
                        &document_order_key0.line);
    memcpy(&document_order_key, &document_order_key0, sizeof(document_order_key0.dtype) +
                                                      sizeof(document_order_key0.sklad) +
                                                      sizeof(document_order_key0.document));
    document_order_key.line = document_order_key0.line + DECIMAL_SHIFT;

    if (do_document_order_key0(alias, &document_order_key0, DO_GET_GE) != DO_OK)
        return 0;

    return (!memcmp(&document_order_key, &document_order_key0, sizeof(document_order_key0.dtype) +
                                                      sizeof(document_order_key0.sklad) +
                                                      sizeof(document_order_key0.document))) &&
            document_order_key0.line < document_order_key.line;
}
DO_EXPORT int do_rz_info_multi_exist_by_barcode(do_alias_t *alias, const char* barcode)
{
    document_order_key0_t document_order_key0, document_order_key;
    int retval = 0;

    do_rz_info_set_key(alias, barcode,
                        document_order_key0.dtype,
                        document_order_key0.sklad,
                        document_order_key0.document,
                        &document_order_key0.line);
    memcpy(&document_order_key, &document_order_key0, sizeof(document_order_key0.dtype) +
                                                      sizeof(document_order_key0.sklad) +
                                                      sizeof(document_order_key0.document));
    document_order_key.line = document_order_key0.line + DECIMAL_SHIFT;

    if (do_document_order_key0(alias, &document_order_key0, DO_GET_GE) != DO_OK)
        return 0;

    while ((!memcmp(&document_order_key, &document_order_key0, sizeof(document_order_key0.dtype) +
                                                      sizeof(document_order_key0.sklad) +
                                                      sizeof(document_order_key0.document))) &&
            document_order_key0.line < document_order_key.line) {
        retval++;
        if (retval > 1)
            return 1;
        if (do_document_order_key0(alias, &document_order_key0, DO_GET_NEXT) != DO_OK)
            return 0;
    }
    return 0;
}
DO_EXPORT do_list_t *do_rz_info_list_from_code(do_alias_t *alias, const char* code)
{
    barcode_key1_t barcode_key1, barcode_key;
    barcode_rec_t  barcode;
    int status;
    char *bar_code;
    do_list_t *retval;

    retval = do_list_new(0);

    do_text_set(alias, barcode_key1.code, code);
    do_cpy(barcode_key, barcode_key1);

    status = do_barcode_get1(alias, &barcode, &barcode_key1, DO_GET_EQUAL);
    while (status == DO_OK) {
        if (do_cmp(barcode_key, barcode_key1))
            break;
        bar_code = do_text(alias, barcode.data.barcode);
        if (!do_rz_info_add_list(retval, alias, bar_code)) {
            do_rz_info_list_free(retval);
            return NULL;
        }
        do_free(bar_code);
        status = do_barcode_get1(alias, &barcode, &barcode_key1, DO_GET_NEXT);
    }
    return retval;
}
DO_EXPORT do_list_t *do_rz_info_util_mark_link(do_alias_t *alias, const char *barcode)
{
    do_extended_t *extended;
    document_key0_t document_key0;
    document_order_key2_t document_order_key2, key;
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    do_ret_list_t *list;
    do_list_t *retval;
    int i;
    int status;
    struct PACK_STRUCT {
        BTI_CHAR document[10];
    } *ret_;


    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_RZN));
    do_text_set(alias, document_key0.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    do_text_set_empty(document_key0.document);

    do_cpy(document_order_key2.dtype, document_key0.dtype);
    do_cpy(document_order_key2.sklad, document_key0.sklad);
    do_text_set(alias, document_order_key2.code, barcode);

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT);
    do_extended_set_key(extended, 0, &document_key0);
    do_extended_set_filter(extended, TRUE, 2, sizeof(document_key0.dtype), sizeof(document_key0.sklad));
    do_extended_set_filter_term(extended, 0, DO_EXTENDED_TYPE_STRING,
                                             offsetof(document_struct_t, dtype),
                                             DO_EXTENDED_COMP_EQUAL,
                                             DO_EXTENDED_CONNECTION_AND,
                                             document_key0.dtype);
    do_extended_set_filter_term(extended, 1, DO_EXTENDED_TYPE_STRING,
                                             offsetof(document_struct_t, sklad),
                                             DO_EXTENDED_COMP_EQUAL,
                                             DO_EXTENDED_CONNECTION_NO,
                                             document_key0.sklad);

    do_extended_set_fields(extended, 1, do_size_and_offset(document_struct_t, document));

    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    if (!list)
        return NULL;
    retval = do_list_new(0);

    for (i = 0; i < do_ret_list_count(list); i++) {
        ret_ = do_ret_list_item_data(list, i);
        do_cpy(document_order_key2.document, ret_->document);

        status = do_document_order_key2(alias, &document_order_key2, DO_GET_EQUAL);

        if (status == DO_OK) {
            do_cpy(key, document_order_key2);
            status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL);
            while (status == DO_OK) {
                if (do_cmp(key, document_order_key2)) break;

                if (document_order.data.accepted == RZ_INFO_LINK_OK) {
                    document_order.data.accepted = RZ_INFO_LINK_FAIL;

                    if ((status = do_document_order_update(alias, &document_order)) != DO_OK)
                        break;
                    do_cpy(document_order_key0.dtype, document_order.data.dtype);
                    do_cpy(document_order_key0.sklad, document_order.data.sklad);
                    do_cpy(document_order_key0.document, document_order.data.document);
                    document_order_key0.line = document_order.data.line;
                    do_list_add_alloc(retval, &document_order_key0, sizeof(document_order_key0));
                }
                status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_NEXT);
            }
            if (status == DO_ERROR) {
                do_list_free(retval);
                retval = NULL;
                break;
            }
        }
    }
    do_ret_list_free(list);
    return retval;
};
DO_EXPORT do_list_t *do_rz_info_list_from_barcode(do_alias_t *alias, const char* barcode)
{
    do_list_t *retval;
    retval = do_list_new(0);
    if (!do_rz_info_add_list(retval, alias, barcode)) {
        do_rz_info_list_free(retval);
        return NULL;
    }
    return retval;
}
DO_EXPORT int do_rz_info_cmp(rz_info_t *r1, rz_info_t *r2)
{
    int retval = 0;
    retval |= (strcmp(r1->mnn, r2->mnn)) ? 1 << 0 : 0;
    retval |= (strcmp(r1->trade_name, r2->trade_name)) ? 1 << 1 : 0;
    retval |= (strcmp(r1->pack_form, r2->pack_form)) ? 1 << 2 : 0;
    retval |= (r1->pack_quant != r2->pack_quant) ? 1 << 3 : 0;
    retval |= (strcmp(r1->manufactor, r2->manufactor)) ? 1 << 4 : 0;
    retval |= ((r1->price > r2->price + 0.001 || r1->price < r2->price - 0.001) ||
                r1->valuta != r2->valuta)  ? 1 << 5 : 0;
    return retval;
}
