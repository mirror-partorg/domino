//!!#ifndef USE_MINIMAL
#include "replic.h"
#include "util.h"
#include <domino.h>
#include <stddef.h>
#include <limits.h>

#ifdef DEBUG
#define debug_assert(a) {if (!(a)) {do_log(LOG_ERR, "not valid exchanged"); return 0;}}
#define debug_assert_break(a) {if (!(a)) {do_log(LOG_ERR, "not valid exchanged"); err = 1; break;}}
#else
#define debug_assert(a)
#define debug_assert_break(a)
#endif

#define PRODUCT_DATA_FIRST_TYPE 78
#define PRODUCT_FIRST_CODE "00001"
#define REPLIC_NAME "replic"

static int product_is_equal(product_struct_t *new_, int newsize, product_struct_t *old, int oldsize);
static void product_set_l(do_alias_t *alias, product_struct_t *new_, product_struct_t *old);
static int protocol_write_change_stock(do_alias_t *alias, stock_key0_t *key);
static int protocol_write_change_document(do_alias_t *alias, document_struct_t *document_st, int changed);
int do_protocol_key1_date_set(protocol_key1_t *key, struct tm *tm);
int do_protocol_date_set(protocol_struct_t *key, struct tm *tm);
int do_document_key3_date_set(document_key3_t *key, struct tm *tm);
int do_protocol_time_set(protocol_struct_t *key, struct tm *tm);


#define RUNNING_ (!break_func || !break_func(NULL))

static const char *handbook_document[] = {"MR","LT"};

static int product_base_to_parcel(do_alias_t *alias, product_rec_t *base, product_rec_t *parcel)
{
    int res = FALSE;
    if (do_cmp(parcel->data.class_id, base->data.class_id)) {
        do_cpy(parcel->data.class_id, base->data.class_id);
        res = TRUE;
    }
    if (do_cmp(parcel->data.group_id, base->data.group_id)) {
        do_cpy(parcel->data.group_id, base->data.group_id);
        res = TRUE;
    }
    if (do_cmp(parcel->data.subgroup_id, base->data.subgroup_id)) {
        do_cpy(parcel->data.subgroup_id, base->data.subgroup_id);
        res = TRUE;
    }
    if (do_cmp(parcel->data.name, base->data.name)) {
        do_cpy(parcel->data.name, base->data.name);
        res = TRUE;
    }
    if (do_cmp(parcel->data.article, base->data.article)) {
        do_cpy(parcel->data.article, base->data.article);
        res = TRUE;
    }
    if (do_cmp(parcel->data.variant, base->data.variant)) {
        do_cpy(parcel->data.variant, base->data.variant);
        res = TRUE;
    }
    if (do_cmp(parcel->data.WES, base->data.WES)) {
        do_cpy(parcel->data.WES, base->data.WES);
        res = TRUE;
    }
    if (do_cmp(parcel->data.unit, base->data.unit)) {
        do_cpy(parcel->data.unit, base->data.unit);
        res = TRUE;
    }
    if (do_cmp(parcel->data.tech_unit, base->data.tech_unit)) {
        do_cpy(parcel->data.tech_unit, base->data.tech_unit);
        res = TRUE;
    }
    if (do_cmp(parcel->data.mfr_region, base->data.mfr_region)) {
        do_cpy(parcel->data.mfr_region, base->data.mfr_region);
        res = TRUE;
    }
    if (do_cmp(parcel->data.mfr_code, base->data.mfr_code)) {
        do_cpy(parcel->data.mfr_code, base->data.mfr_code);
        res = TRUE;
    }

    if (do_cmp(parcel->data.ptype, base->data.ptype)) {
        do_cpy(parcel->data.ptype, base->data.ptype);
        res = TRUE;
    }
    if (do_cmp(parcel->data.tech_unit_rate, base->data.tech_unit_rate)) {
        do_cpy(parcel->data.tech_unit_rate, base->data.tech_unit_rate);
        res = TRUE;
    }

    if ( do_product_param_int(alias, parcel, do_param(DO_PARAM_PRODUCT_PARAMETR_NO_QUANT_LIMIT)) !=
         do_product_param_int(alias, base, do_param(DO_PARAM_PRODUCT_PARAMETR_NO_QUANT_LIMIT)) ) {

        do_product_param_int_set(alias, parcel, do_param(DO_PARAM_PRODUCT_PARAMETR_NO_QUANT_LIMIT),
                                 do_product_param_int(alias, base, do_param(DO_PARAM_PRODUCT_PARAMETR_NO_QUANT_LIMIT)));
        res = TRUE;
    }
    /*!!char *c1, *c2;
    c1 = do_text(alias, base->data.code);
    c2 = do_text(alias, parcel->data.code);
    //do_log(LOG_INFO, "check %s %s %d", c1, c2, res);
    do_free(c1);do_free(c2);*/
    return res;
}

static int product_update(do_alias_t *alias, product_rec_t *product, do_extended_break_func break_func)
{
    int status;
    status = do_product_update(alias, product);
    if ( status != DO_OK )
        return status;
    if ( product->data.base_parcel != 0 )
        return DO_OK;

    product_key0_t product_key0;
    product_rec_t record;
    int len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE);
    do_cpy(product_key0.code, product->data.code);
    product_key0.base_parcel = 1;
    char *value;

    status = do_product_get0(alias, &record, &product_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( record.data.base_parcel != 1 ||
             strncmp(record.data.code, product->data.code, len) )
            break;
	if (!RUNNING_) {
	        status = DO_ERROR;
	        break;
	}
        if ( product_base_to_parcel(alias, product, &record) ) {
                value = do_text(alias, record.data.code);
                do_log(LOG_INFO, "update product \"%s\"", value);
                if (!do_protocol_product_add(alias, value, DO_PROTOCOL_ACTION_CHANGE))
                    status = DO_ERROR;
                else
                    status = do_product_update(alias, &record);
                do_free(value);
                if (status != DO_OK)
                    return status;
        }
        status = do_product_get0(alias, &record, &product_key0, DO_GET_NEXT);
    }
    if (status == DO_ERROR)
        return DO_ERROR;
    return DO_OK;
}

int replic_product(do_alias_t *alias, const char *code, void *data, size_t size, do_extended_break_func break_func)
{
    int status;
    product_rec_t product, product_new;
    product_key0_t product_key0;
    product_key4_t product_key4;
    do_extended_t *extended;
    do_ret_list_t *list;
    void         *crnt = NULL;
    do_size_t    *sz;
    char         *value;

    if (!size) { // delete

        do_text_set(alias, product_key4.code, code);
        status = do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL);
        if (status == DO_OK) {
            do_log(LOG_INFO, "delete product \"%s\"", code);
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_DELETE))
                status = DO_ERROR;
            else
                status = do_product_delete(alias);
        }
        else {
            do_cpy(product_new.data.code, product_key4.code);
            if (status == DO_KEY_NOT_FOUND)
                status = DO_OK;
        }
    }
    else {

        crnt = data;
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);

        debug_assert(*sz >= sizeof(product_struct_t) - sizeof(product.data.params) &&
                     *sz <= sizeof(product_struct_t));

        product_new.size = *sz;
        memcpy(&product_new.data, crnt, product_new.size);
        crnt += product_new.size;
        debug_assert(crnt <= data + size);

        do_cpy(product_key0.code, product_new.data.code);
        product_key0.base_parcel = product_new.data.base_parcel;

        status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);

        if (status == DO_ERROR)
            return FALSE;

        if (status == DO_KEY_NOT_FOUND) {
            product_set_l(alias, &product_new.data, NULL);
            value = do_text(alias, product_new.data.code);
            do_log(LOG_INFO, "insert product \"%s\"", value);
            if (!do_protocol_product_add(alias, value, DO_PROTOCOL_ACTION_INSERT))
                status = DO_ERROR;
            else
                status = do_product_insert(alias, &product_new);
            do_free(value);
        }
        else {
            if (!product_is_equal(&product_new.data, product_new.size, &product.data, product.size)) {
                product_set_l(alias, &product_new.data, &product.data);
                value = do_text(alias, product_new.data.code);
                do_log(LOG_INFO, "update product \"%s\"", value);
                if (!do_protocol_product_add(alias, value, DO_PROTOCOL_ACTION_CHANGE))
                    status = DO_ERROR;
                else
                    status = product_update(alias, &product_new, break_func);
                do_free(value);
            }
        }
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    if (status != DO_OK)
        return FALSE;

    int count, i, j, del;
    do_list_t *do_list_ins, *do_list_ch, *do_list_del;
    size_t size_new;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

// product_view
    product_view_key1_t   product_view_key1;
    product_view_key0_t   product_view_key0;
    product_view_rec_t    product_view;
    product_view_struct_t *product_view_st;
    product_view_struct_t *product_view_new_st;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz < 1000);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(product_view_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    memcpy(&product_view_key1.code, &product_new.data.code, sizeof(product_new.data.code));
    do_text_set_empty(product_view_key1.sklad);

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PRODUCT_VIEW);
    do_extended_set_key(extended, 1, &product_view_key1);
    do_extended_set_filter(extended, TRUE, 1, sizeof(product_view_key1.code));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(product_view_struct_t, code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &product_view_key1.code);
    do_extended_set_fields_full_record(extended, sizeof(product_view_struct_t));
    list = do_extended_get(extended, -1);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        del = TRUE;
        product_view_st = do_ret_list_item_data(list, i);
        for (j =0; j < do_list_ins->count; j++) {
            product_view_new_st = do_list_ins->list[j];
            if (product_view_st->type == product_view_new_st->type &&
                !memcmp(&product_view_st->sklad, &product_view_new_st->sklad, sizeof(product_view_st->sklad)) &&
                !memcmp(&product_view_st->sort, &product_view_new_st->sort, sizeof(product_view_st->sort)) &&
                !memcmp(&product_view_st->code, &product_view_new_st->code, sizeof(product_view_st->code))) {

                del = FALSE;
                size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
                do_list_delete(do_list_ins, j);

                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(product_view_st, product_view_new_st, size_new)) {
                    do_list_add(do_list_ch, product_view_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, product_view_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        do_log(LOG_INFO, "insert product_view \"%s\"", code);

        product_view.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&product_view.data, do_list_ins->list[i], product_view.size);

        if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
            status = DO_ERROR;
        else
            status = do_product_view_insert(alias, &product_view);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        do_log(LOG_INFO, "update product_view \"%s\"", code);

        product_view_st = do_list_ch->list[i];
        product_view_key0.type = product_view_st->type;
        memcpy(&product_view_key0.sklad, &product_view_st->sklad, sizeof(product_view_key0.sklad));
        memcpy(&product_view_key0.sort,  &product_view_st->sort,  sizeof(product_view_key0.sort));
        memcpy(&product_view_key0.code,  &product_view_st->code,  sizeof(product_view_key0.code));
        status = do_product_view_get0(alias, &product_view, &product_view_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            product_view.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&product_view.data, do_list_ch->list[i], product_view.size);
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_product_view_update(alias, &product_view);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        do_log(LOG_INFO, "delete product_view \"%s\"", code);

        product_view_st = do_list_del->list[i];
        product_view_key0.type = product_view_st->type;
        memcpy(&product_view_key0.sklad, &product_view_st->sklad, sizeof(product_view_key0.sklad));
        memcpy(&product_view_key0.sort,  &product_view_st->sort,  sizeof(product_view_key0.sort));
        memcpy(&product_view_key0.code,  &product_view_st->code,  sizeof(product_view_key0.code));
        status = do_product_view_get0(alias, &product_view, &product_view_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_product_view_delete(alias);
        }
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_list_clear(do_list_ins);
    do_list_clear(do_list_del);
    do_list_clear(do_list_ch);
    do_ret_list_free(list);

    if (status != DO_OK) {
        do_list_free(do_list_ins);
        do_list_free(do_list_del);
        do_list_free(do_list_ch);
        return FALSE;
    }
// product data

    product_data_key0_t   product_data_key0;
    product_data_rec_t    product_data;
    product_data_struct_t *product_data_st;
    product_data_struct_t *product_data_new_st;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz < 1000);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz >= sizeof(product_data_struct_t) - sizeof(product_data.data.params) &&
                         *sz <= sizeof(product_data_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    memcpy(&product_data_key0.code, &product_new.data.code, sizeof(product_new.data.code));
    if ( !strcmp(code, PRODUCT_FIRST_CODE) ) // skip local data
        product_data_key0.type = PRODUCT_DATA_FIRST_TYPE;
    else
        product_data_key0.type = 0;
    product_data_key0.number = 0;


    do_extended_set_file_db(extended, DO_DB_PRODUCT_DATA);
    do_extended_set_key(extended, 0, &product_data_key0);
    do_extended_set_filter(extended, TRUE, 1, sizeof(product_data_key0.code));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(product_data_struct_t, code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &product_data_key0.code);
    do_extended_set_fields_full_record(extended, sizeof(product_data_struct_t));
    list = do_extended_get(extended, -1);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        del = TRUE;
        product_data_st = do_ret_list_item_data(list, i);
        for (j =0; j < do_list_ins->count; j++) {
            product_data_new_st = do_list_ins->list[j];
            if (product_data_st->type == product_data_new_st->type &&
                product_data_st->number == product_data_new_st->number &&
                !memcmp(&product_data_st->code, &product_data_new_st->code, sizeof(product_data_st->code))) {

                del = FALSE;
                size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
                do_list_delete(do_list_ins, j);

                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(product_data_st, product_data_new_st, size_new)) {
                    do_list_add(do_list_ch, product_data_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, product_data_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }


        product_data.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&product_data.data, do_list_ins->list[i], product_data.size);

        //if ( product_data.data.type == PRODUCT_DATA_PROTOCOL_TYPE ) // just in case
        //    continue;

        do_log(LOG_INFO, "insert product_data \"%s\"", code);
        status = do_product_data_insert(alias, &product_data);
        if ( status == DO_OK ) {
            if ( !do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE) )
                status = DO_ERROR;
        }
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        product_data_st = do_list_ch->list[i];

        //if ( product_data_st->type == PRODUCT_DATA_PROTOCOL_TYPE ) // just in case
        //    continue;

        do_log(LOG_INFO, "update product_data \"%s\"", code);

        product_data_key0.type = product_data_st->type;
        product_data_key0.number = product_data_st->number;
        memcpy(&product_data_key0.code,  &product_data_st->code,  sizeof(product_data_key0.code));
        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            product_data.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&product_data.data, do_list_ch->list[i], product_data.size);
            status = do_product_data_update(alias, &product_data);
            if ( status == DO_OK ) {
                if ( !do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE) )
                    status = DO_ERROR;
            }
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        product_data_st = do_list_del->list[i];
        //if ( product_data_st->type == PRODUCT_DATA_PROTOCOL_TYPE ) // just in case
        //    continue;

        do_log(LOG_INFO, "delete product_data \"%s\" %d %d", code, product_data_st->type, product_data_st->number);
        //exit(1);//!!


        product_data_key0.type = product_data_st->type;
        product_data_key0.number = product_data_st->number;
        memcpy(&product_data_key0.code,  &product_data_st->code,  sizeof(product_data_key0.code));
        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_EQUAL);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }

    do_list_clear(do_list_ins);
    do_list_clear(do_list_del);
    do_list_clear(do_list_ch);
    do_ret_list_free(list);

    if (status != DO_OK) {
        do_list_free(do_list_ins);
        do_list_free(do_list_del);
        do_list_free(do_list_ch);
        return FALSE;
    }

// barcodes
    barcode_key1_t   barcode_key1;
    barcode_key0_t   barcode_key0;
    barcode_rec_t    barcode;
    barcode_struct_t *barcode_st;
    barcode_struct_t *barcode_new_st;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz < 1000);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(barcode_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    memcpy(&barcode_key1.code, &product_new.data.code, sizeof(product_new.data.code));

    do_extended_set_file_db(extended, DO_DB_BARCODE);
    do_extended_set_key(extended, 1, &barcode_key1);
    do_extended_set_filter(extended, TRUE, 1, sizeof(barcode_key1.code));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(barcode_struct_t, code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &barcode_key1.code);
    do_extended_set_fields_full_record(extended, sizeof(barcode_struct_t));
    list = do_extended_get(extended, -1);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        del = TRUE;
        barcode_st = do_ret_list_item_data(list, i);
        for (j =0; j < do_list_ins->count; j++) {
            barcode_new_st = do_list_ins->list[j];
            if (!memcmp(&barcode_st->barcode, &barcode_new_st->barcode, sizeof(barcode_st->barcode))) {

                del = FALSE;
                size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
                do_list_delete(do_list_ins, j);

                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(barcode_st, barcode_new_st, size_new)) {
                    do_list_add(do_list_ch, barcode_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, barcode_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        barcode.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&barcode.data, do_list_ins->list[i], barcode.size);
        value = do_text(alias, barcode.data.barcode);
        do_log(LOG_INFO, "insert barcode \"%s\" \"%s\"", value, code);
        do_free(value);

        if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
            status = DO_ERROR;
        else {
            barcode_key0_t barcode_key0;
            barcode_rec_t bar1;
            do_cpy(barcode_key0.barcode,barcode.data.barcode);
            status = do_barcode_get0(alias, &bar1, &barcode_key0, DO_GET_EQUAL);
            if ( status == DO_KEY_NOT_FOUND )
                 status = do_barcode_insert(alias, &barcode);
            else
                 status = do_barcode_update(alias, &barcode);
        }
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        barcode_st = do_list_ch->list[i];
        memcpy(&barcode_key0.barcode,  &barcode_st->barcode,  sizeof(barcode_key0.barcode));

        value = do_text(alias, barcode_key0.barcode);
        do_log(LOG_INFO, "update barcode \"%s\" \"%s\"", value, code);
        do_free(value);

        status = do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            barcode.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&barcode.data, do_list_ch->list[i], barcode.size);
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_barcode_update(alias, &barcode);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        barcode_st = do_list_del->list[i];
        memcpy(&barcode_key0.barcode, &barcode_st->barcode, sizeof(barcode_key0.barcode));

        value = do_text(alias, barcode_key0.barcode);
        do_log(LOG_INFO, "delete barcode \"%s\" \"%s\"", value, code);
        do_free(value);

        status = do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_barcode_delete(alias);
        }
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }

    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);
    do_ret_list_free(list);
    do_extended_free(extended);

    return status == DO_OK;
}

static int product_is_equal(product_struct_t *new_, int newsize, product_struct_t *old, int oldsize)
{
    if (newsize != oldsize)
        return 0;
    if (new_->base_parcel)
        return !memcmp(new_, old, newsize);
    else {
        return  !memcmp(&new_->class_id, &old->class_id,
                    sizeof(new_->class_id)+
                    sizeof(new_->group_id)+
                    sizeof(new_->subgroup_id)+
                    sizeof(new_->code)+
                    sizeof(new_->name)+
                    sizeof(new_->article)+
                    sizeof(new_->variant)+
                    sizeof(new_->WES)+
                    sizeof(new_->unit)+
                    sizeof(new_->tech_unit)) &&
               !memcmp(&new_->mfr_region, &old->mfr_region,
                    sizeof(new_->mfr_region)+
                    sizeof(new_->mfr_code)+
                    sizeof(new_->ptype)) &&
               !memcmp(&new_->tech_unit_rate, &old->tech_unit_rate,
                    sizeof(new_->tech_unit_rate)+
                    sizeof(new_->date)+
                    sizeof(new_->time)+
                    sizeof(new_->amor_date_mk)+
                    sizeof(new_->amor_date_vd)+
                    sizeof(new_->amor_code)+
                    sizeof(new_->party)+
                    sizeof(new_->base_parcel)) &&
                !memcmp(&new_->params, &old->params,
                    newsize - (sizeof(product_struct_t) - sizeof(new_->params))) &&
                    old->supplier_code != 0;
    }
}
static void product_set_l(do_alias_t *alias, product_struct_t *new_, product_struct_t *old)
{
    if ( !new_->base_parcel ) {
        if (old) {
            if (old->supplier_code)
                memcpy(&new_->supplier_region, &old->supplier_region,
                       sizeof(new_->supplier_region)+
                       sizeof(new_->supplier_code));
            else {
                do_text_set(alias, new_->supplier_region, "00");
                new_->supplier_code = 1;
            }
            memcpy(&new_->price, &old->price,
                   sizeof(new_->price));
            memcpy(&new_->price_filled, &old->price_filled,
                   sizeof(new_->price_filled)+
                   sizeof(new_->total_price));
            memcpy(&new_->nprice, &old->nprice,
                   sizeof(new_->nprice));
        }
        else {
            do_text_set(alias, new_->supplier_region, "00");
            new_->supplier_code = 1;
            memset(&new_->price, 0,
                   sizeof(new_->price));
            new_->price_filled = 1;
            new_->total_price = 0;
            memset(&new_->nprice, 0,
                   sizeof(new_->nprice));
        }
    }
}

do_ret_list_t *replic_select_product_base_parcel(do_alias_t *alias,  const char *sklad, int base_parcel, product_key4_t *key1, product_key4_t *key2, do_extended_break_func break_func)
{
#ifndef DOMINO78
    int base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE);
#endif
    int n = 0,i = 0;
    do_ret_list_t *retval;
    product_key0_t product_key0;
    do_extended_t *extended;
#ifndef DOMINO78
    int code_size = 0;
    char *prefix = NULL;
    if ( base_parcel ) {
        prefix = domino_get_unit_prefix(alias, sklad);
        if (!prefix || *prefix == '\0') {
            do_log(LOG_ERR, "local product's prefix are empty");
            if (prefix)
                do_free(prefix);
            return NULL;
        }
        code_size = strlen(prefix);
    }
#endif
    product_key0.base_parcel = base_parcel;
    if ( do_text_is_empty(key1->code) )
        do_text_set_empty(product_key0.code);
    else
        do_cpy(product_key0.code,key1->code);

    n = 1;
#ifndef DOMINO78
    if ( base_parcel )
        n++;
#endif
    if ( !do_text_is_empty(key2->code) )
        n++;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PRODUCT);
    do_extended_set_key(extended, 0, &product_key0);
#ifndef DOMINO78
    if ( base_parcel )
        do_extended_set_filter(extended, !base_parcel, n, sizeof(product_key0.base_parcel), code_size, sizeof(product_key0.code));
    else
#endif
        do_extended_set_filter(extended, !base_parcel, n, sizeof(product_key0.base_parcel), sizeof(product_key0.code));

    do_extended_set_filter_term(extended, i,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(product_struct_t, base_parcel),
                                DO_EXTENDED_COMP_EQUAL,
                                (n > i+1) ?
                                  DO_EXTENDED_CONNECTION_AND :
                                  DO_EXTENDED_CONNECTION_NO,
                                &product_key0.base_parcel);
    i+=1;
#ifndef DOMINO78
    if ( base_parcel ) {
        do_extended_set_filter_term(extended, i,
                                    DO_EXTENDED_TYPE_STRING,
                                    offsetof(product_struct_t, code) + base_len,
                                    DO_EXTENDED_COMP_EQUAL,
                                    (n > i+1) ?
                                     DO_EXTENDED_CONNECTION_AND :
                                     DO_EXTENDED_CONNECTION_NO,
                                    prefix);
        i++;
    }
#endif
    if ( !do_text_is_empty(key2->code) )
        do_extended_set_filter_term(extended, i,
                                    DO_EXTENDED_TYPE_STRING,
                                    offsetof(product_struct_t, code),
                                    DO_EXTENDED_COMP_LT,
                                    (n > i+1) ?
                                     DO_EXTENDED_CONNECTION_AND :
                                     DO_EXTENDED_CONNECTION_NO,
                                    key2->code);
    do_extended_set_fields_full_record(extended, sizeof(product_struct_t));
    retval = do_extended_get(extended, -1);
#ifndef DOMINO78
    if (prefix)
        do_free(prefix);
#endif
    do_extended_free(extended);
    return retval;
}
do_ret_list_t *replic_select_product_view_base_parcel(do_alias_t *alias, const char *sklad, int base_parcel, product_key4_t *key1, product_key4_t *key2, do_extended_break_func break_func)
{
    int  n = 0;
#ifndef DOMINO78
    int   code_size = 0
    char *prefix = NULL;
#endif
    int   base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE);
    char  parcel_separator = do_param(DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR)[0];
    do_ret_list_t       *retval;
    product_view_key1_t  product_view_key1;
    do_extended_t       *extended;
#ifndef DOMINO78
    if (base_parcel) {
        prefix = domino_get_unit_prefix(alias, sklad);
        if (!prefix || *prefix == '\0') {
            do_log(LOG_ERR, "local product's prefix are empty");
            if (prefix)
                do_free(prefix);
            return NULL;
        }
        code_size = strlen(prefix);
    }
#endif
    n++;
    if ( do_text_is_empty(key1->code) )
        do_text_set_empty(product_view_key1.code);
    else
        do_cpy(product_view_key1.code,key1->code);
    do_text_set_empty(product_view_key1.sklad);
    if ( !do_text_is_empty(key2->code) )
        n++;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PRODUCT_VIEW);
    do_extended_set_key(extended, 1, &product_view_key1);

#ifndef DOMINO78
    do_extended_set_filter(extended, FALSE, 2, (base_parcel) ? code_size : sizeof(parcel_separator), sizeof(product_view_key1.code));
#else
    do_extended_set_filter(extended, FALSE, 2, sizeof(parcel_separator), sizeof(product_view_key1.code) );
#endif
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(product_view_struct_t, code) + base_len,
                                ( base_parcel ) ?
                                  DO_EXTENDED_COMP_EQUAL :
                                  DO_EXTENDED_COMP_NOT_EQUAL,

                                (n < 2) ? DO_EXTENDED_CONNECTION_NO : DO_EXTENDED_CONNECTION_AND,
#ifndef DOMINO78
                                (base_parcel) ?
                                  prefix :
#endif
                                  &parcel_separator
                                  );
    if ( n > 1 ) {
        do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(product_view_struct_t, code),
                                DO_EXTENDED_COMP_LT,
                                DO_EXTENDED_CONNECTION_NO,
                                key2->code
                                  );
    }
    do_extended_set_fields_full_record(extended, sizeof(product_view_struct_t));
    retval = do_extended_get(extended, -1);
#ifndef DOMINO78
    if (prefix)
        do_free(prefix);
#endif // DOMINO78
    do_extended_free(extended);
    return retval;
}
do_ret_list_t *replic_select_barcode_base_parcel(do_alias_t *alias, const char *sklad, int base_parcel, product_key4_t *key1, product_key4_t *key2, do_extended_break_func break_func)
{
    int n = 0;
#ifndef DOMINO78
    int   code_size = 0;
    char *prefix = NULL;
#endif
    char  parcel_separator = do_param(DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR)[0];
    int   base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE);
    do_ret_list_t  *retval;
    barcode_key1_t  barcode_key1;
    do_extended_t  *extended;
#ifndef DOMINO78
    if (base_parcel) {
        prefix = domino_get_unit_prefix(alias, sklad);
        if (!prefix || *prefix == '\0') {
            do_log(LOG_ERR, "local product's prefix are empty");
            if (prefix)
                do_free(prefix);
            return NULL;
        }
        code_size = strlen(prefix);
        do_log(LOG_INFO, "prefix %s", prefix);
    }
#endif
    n++;
    if ( do_text_is_empty(key1->code) )
        do_text_set_empty(barcode_key1.code);
    else
        do_cpy(barcode_key1.code,key1->code);
    if ( !do_text_is_empty(key2->code) )
        n++;
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_BARCODE);
    do_extended_set_key(extended, 1, &barcode_key1);
#ifndef DOMINO78
    do_extended_set_filter(extended, FALSE, n, (base_parcel) ? code_size : sizeof(parcel_separator), sizeof(barcode_key1.code));
#else
    do_extended_set_filter(extended, FALSE, n, sizeof(parcel_separator), sizeof(barcode_key1.code));
#endif
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(barcode_struct_t, code) + base_len,
                                (base_parcel) ?
                                  DO_EXTENDED_COMP_EQUAL :
                                  DO_EXTENDED_COMP_NOT_EQUAL,
                                (n > 1) ?  DO_EXTENDED_CONNECTION_AND :  DO_EXTENDED_CONNECTION_NO,
#ifndef DOMINO78
                                (base_parcel) ?
                                  prefix :
#endif
                                  &parcel_separator);
    if ( n > 1 ) {
        do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(barcode_struct_t, code),
                                DO_EXTENDED_COMP_LT,
                                DO_EXTENDED_CONNECTION_NO,
                                key2->code
                                  );
    }
    do_extended_set_fields_full_record(extended, sizeof(barcode_struct_t));
    retval = do_extended_get(extended, -1);
#ifndef DOMINO78
    if ( prefix )
        do_free(prefix);
#endif
    do_extended_free(extended);
    return retval;
}
#ifndef DOMINO78
do_ret_list_t *replic_select_shift_check(do_alias_t *alias, shift_struct_t *shift_st, do_extended_break_func break_func)
{
    do_ret_list_t *retval;
    check_key0_t check_key0;
    do_extended_t *extended;

    do_set_empty(check_key0);
    check_key0.cash = shift_st->cash;
    check_key0.shift = shift_st->shift;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_CHECK);
    do_extended_set_key(extended, 0, &check_key0);
    do_extended_set_filter(extended, TRUE, 2, sizeof(check_key0.cash), sizeof(check_key0.shift));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(check_struct_t, cash),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &check_key0.cash);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(check_struct_t, shift),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &check_key0.shift);

    do_extended_set_fields_full_record(extended, sizeof(check_struct_t));
    retval = do_extended_get(extended, -1);
    do_extended_free(extended);

    return retval;
}
do_ret_list_t *replic_select_shift_checksum(do_alias_t *alias, shift_struct_t *shift_st, do_extended_break_func break_func)
{
    do_ret_list_t *retval;
    checksum_key0_t checksum_key0;
    do_extended_t *extended;

    do_set_empty(checksum_key0);
    checksum_key0.cash = shift_st->cash;
    checksum_key0.shift = shift_st->shift;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_CHECKSUM);
    do_extended_set_key(extended, 0, &checksum_key0);
    do_extended_set_filter(extended, TRUE, 2, sizeof(checksum_key0.cash), sizeof(checksum_key0.shift));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(checksum_struct_t, cash),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &checksum_key0.cash);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(checksum_struct_t, shift),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &checksum_key0.shift);

    do_extended_set_fields_full_record(extended, sizeof(checksum_struct_t));
    retval = do_extended_get(extended, -1);
    do_extended_free(extended);

    return retval;
}
#endif
do_ret_list_t *replic_select_document_order(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    document_order_key0_t document_order_key0;
    do_extended_t        *extended;
    do_ret_list_t        *retval;

    memcpy(&document_order_key0, document_key0, sizeof(*document_key0));
    document_order_key0.line = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);
    do_extended_set_key(extended, 0, &document_order_key0);
    do_extended_set_filter(extended, TRUE, 3, sizeof(document_order_key0.dtype), sizeof(document_order_key0.sklad), sizeof(document_order_key0.document));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, dtype),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_order_key0.dtype);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_order_key0.sklad);
    do_extended_set_filter_term(extended, 2,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, document),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &document_order_key0.document);
    do_extended_set_fields_full_record(extended, sizeof(document_order_struct_t));
    retval = do_extended_get(extended, -1);
    do_extended_free(extended);

    return retval;
}

do_ret_list_t *replic_select_document_parcel(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    document_rec_t document;
    do_doctype_t *doctype;

    switch (do_document_get0(alias, &document, document_key0, DO_GET_EQUAL)) {
        case DO_ERROR:
            return NULL;
        case DO_KEY_NOT_FOUND:
            return do_ret_list_static_new();
        default:
            break;
    }
    if ((doctype = do_doctype_new_from_document(alias, &document)) == NULL)
        return NULL;

    if (!doctype->auto_parcel) {
        do_doctype_free(doctype);
        return do_ret_list_static_new();
    }
    do_doctype_free(doctype);

    document_order_key0_t document_order_key0;
    product_rec_t product;
    product_key4_t product_key4;
    do_extended_t *extended;
    do_ret_list_t  *retval;
    do_ret_list_t  *lines;
    struct PACK_STRUCT {
        BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    } *_ret;
    int i;
    int status;

    memcpy(&document_order_key0, document_key0, sizeof(*document_key0));
    document_order_key0.line = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);
    do_extended_set_key(extended, 0, &document_order_key0);
    do_extended_set_filter(extended, TRUE, 3, sizeof(document_order_key0.dtype), sizeof(document_order_key0.sklad), sizeof(document_order_key0.document));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, dtype),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_order_key0.dtype);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_order_key0.sklad);
    do_extended_set_filter_term(extended, 2,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, document),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &document_order_key0.document);
    do_extended_set_fields(extended, 1, do_size_and_offset(document_order_struct_t, code));
    lines = do_extended_get(extended, -1);
    do_extended_free(extended);
    if (!lines)
        return NULL;
    status = DO_OK;
    retval = do_ret_list_static_new();
    for (i = 0; i < do_ret_list_count(lines); i++) {
        _ret = do_ret_list_item_data(lines, i);
        do_cpy(product_key4.code, _ret->code);
        status = do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL);
        if (status == DO_ERROR)
            break;
        if (status == DO_KEY_NOT_FOUND)
            continue;
        if (!product.data.base_parcel)
            continue;
        do_ret_list_static_add(retval, &product.data, product.size);
    }
    do_ret_list_free(lines);
    if (status == DO_ERROR) {
        do_ret_list_free(retval);
        return NULL;
    }
    return retval;
}

do_ret_list_t *replic_select_document_data(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    document_data_key0_t document_data_key0;
    do_extended_t        *extended;
    do_ret_list_t        *retval;

    memcpy(&document_data_key0, document_key0, sizeof(*document_key0));
    document_data_key0.type = 0;
    document_data_key0.number = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_DATA);
    do_extended_set_key(extended, 0, &document_data_key0);
    do_extended_set_filter(extended, TRUE, 3, sizeof(document_data_key0.dtype), sizeof(document_data_key0.sklad), sizeof(document_data_key0.document));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_data_struct_t, dtype),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_data_key0.dtype);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_data_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_data_key0.sklad);
    do_extended_set_filter_term(extended, 2,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_data_struct_t, document),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &document_data_key0.document);
    do_extended_set_fields_full_record(extended, sizeof(document_data_struct_t));
    retval = do_extended_get(extended, -1);
    do_extended_free(extended);

    return retval;
}
do_ret_list_t *replic_select_document_view(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    document_view_key1_t document_view_key1;
    do_extended_t        *extended;
    do_ret_list_t        *retval;

    memcpy(&document_view_key1, document_key0, sizeof(*document_key0));
    document_view_key1.type = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_VIEW);
    do_extended_set_key(extended, 1, &document_view_key1);
    do_extended_set_filter(extended, TRUE, 3, sizeof(document_view_key1.dtype), sizeof(document_view_key1.sklad), sizeof(document_view_key1.document));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_view_struct_t, dtype),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_view_key1.dtype);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_view_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_view_key1.sklad);
    do_extended_set_filter_term(extended, 2,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_view_struct_t, document),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &document_view_key1.document);
    do_extended_set_fields_full_record(extended, sizeof(document_view_struct_t));
    retval = do_extended_get(extended, -1);
    do_extended_free(extended);

    return retval;
}
do_ret_list_t *replic_select_document_link(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    document_link_key0_t document_link_key0;
    do_extended_t        *extended;
    do_ret_list_t        *retval;

    memcpy(&document_link_key0, document_key0, sizeof(*document_key0));
    document_link_key0.type = 0;
    document_link_key0.sort = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_LINK);
    do_extended_set_key(extended, 0, &document_link_key0);
    do_extended_set_filter(extended, TRUE, 3, sizeof(document_link_key0.m_dtype), sizeof(document_link_key0.m_sklad), sizeof(document_link_key0.m_document));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_link_struct_t, m_dtype),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_link_key0.m_dtype);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_link_struct_t, m_sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_link_key0.m_sklad);
    do_extended_set_filter_term(extended, 2,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_link_struct_t, m_document),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &document_link_key0.m_document);
    do_extended_set_fields_full_record(extended, sizeof(document_link_struct_t));
    retval = do_extended_get(extended, -1);
    do_extended_free(extended);

    return retval;
}
do_ret_list_t *replic_select_document_prowod(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    prowod_key4_t prowod_key4;
    prowod_struct_t *prowod_st;
    char *prowodkey, *params, *param_doc;
    char *dtype, *sklad, *document;
    int  i;
    do_ret_list_t *list;
    do_extended_t *extended;

    dtype = do_text(alias, document_key0->dtype);
    sklad = do_text(alias, document_key0->sklad);
    document = do_text(alias, document_key0->document);
    prowodkey = (char*)do_malloc(strlen(dtype) + strlen(sklad) + strlen(document) + 3 + 1);
    sprintf(prowodkey, "@%s.%s/%s", document, dtype, sklad);

    do_text_set(alias, prowod_key4.division, domino_division());
    do_text_set(alias, prowod_key4.document, prowodkey);
    prowod_key4.date = INT_MIN;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PROWOD);
    do_extended_set_key(extended, 4, &prowod_key4);
    do_extended_set_filter(extended, TRUE, 2, sizeof(prowod_key4.division), sizeof(prowod_key4.document));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(prowod_struct_t, division),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &prowod_key4.division);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(prowod_struct_t, document),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &prowod_key4.document);
    do_extended_set_fields_full_record(extended, sizeof(prowod_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    if (list && strlen(prowodkey) > sizeof(prowod_key4.document)) {
        for (i = 0; i < do_ret_list_count(list); ) {
            prowod_st = do_ret_list_item_data(list, i);
            params = do_alias_text_(alias, prowod_st->params,
                do_ret_list_item_len(list, i) - (sizeof(*prowod_st) - sizeof(prowod_st->params)));
            param_doc = do_fromparam(params, do_param(DO_PARAM_PROWOD_PARAMETR_DOCUMENT), do_alias_utf8(alias));
            if (!strcmp(param_doc, prowodkey))
                i++;
            else
                do_ret_list_delete(list, i);
            do_free(params);
            do_free(param_doc);
        }
    }
    do_free(prowodkey);
    do_free(dtype);
    do_free(sklad);
    do_free(document);
    return list;
}
do_ret_list_t *replic_select_document_prowod2(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    prowod2_key0_t prowod2_key0;
    do_extended_t        *extended;
    do_ret_list_t        *retval;

    memcpy(&prowod2_key0, document_key0, sizeof(*document_key0));
    prowod2_key0.number = 0;
    do_text_set_empty(prowod2_key0.otdel);

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PROWOD2);
    do_extended_set_key(extended, 0, &prowod2_key0);
    do_extended_set_filter(extended, TRUE, 3, sizeof(prowod2_key0.dtype), sizeof(prowod2_key0.sklad), sizeof(prowod2_key0.document));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(prowod2_struct_t, dtype),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &prowod2_key0.dtype);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(prowod2_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &prowod2_key0.sklad);
    do_extended_set_filter_term(extended, 2,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(prowod2_struct_t, document),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &prowod2_key0.document);
    do_extended_set_fields_full_record(extended, sizeof(prowod2_struct_t));
    retval = do_extended_get(extended, -1);
    do_extended_free(extended);

    return retval;
}
do_ret_list_t *replic_select_document_protocol(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    protocol_key0_t protocol_key0;
    protocol_struct_t *protocol_st;

    char *namekey;
    char *dtype, *sklad, *document;
    char dor = '.';
    int namelen;
    do_ret_list_t *list;
    do_extended_t *extended;

    dtype = do_text(alias, document_key0->dtype);
    sklad = do_text(alias, document_key0->sklad);
    document = do_text(alias, document_key0->document);
    namekey = (char*)do_malloc(strlen(dtype) + strlen(sklad) + strlen(document) + 40);
    sprintf(namekey, "ДОК.%s.%s.%s", dtype, sklad, document);
    namelen = (strlen(namekey) > sizeof(protocol_st->objectName)) ? sizeof(protocol_st->objectName) : strlen(namekey);

    do_text_set(alias, protocol_key0.objectName, namekey);
    protocol_key0.date = INT_MIN;
    protocol_key0.time = INT_MIN;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PROTOCOL);
    do_extended_set_key(extended, 0, &protocol_key0);
    if ( namelen <  sizeof(protocol_st->objectName) )
        do_extended_set_filter(extended, TRUE, 2, namelen, 1);
    else
        do_extended_set_filter(extended, TRUE, 1, namelen);
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(protocol_struct_t, objectName),
                                DO_EXTENDED_COMP_EQUAL,
                                (namelen <  sizeof(protocol_st->objectName)) ? DO_EXTENDED_CONNECTION_AND : DO_EXTENDED_CONNECTION_NO,
                                 &protocol_key0.objectName);
    if ( namelen <  sizeof(protocol_st->objectName) ) {
        do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(protocol_struct_t, objectName)+namelen,
                                DO_EXTENDED_COMP_LE,
                                DO_EXTENDED_CONNECTION_NO,
                                &dor);
    }
    do_extended_set_fields_full_record(extended, sizeof(protocol_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    do_free(namekey);
    do_free(dtype);
    do_free(sklad);
    do_free(document);
    return list;
}

/*
do_ret_list_t *replic_select_stocks(do_alias_t *alias, const char *sklad, do_extended_break_func break_func)
{
    do_ret_list_t *retval;
    stock_key0_t stock_key0;
    do_extended_t *extended;
    BTI_BYTE quant[6][8];

    do_text_set(alias, stock_key0.sklad, sklad);
    do_text_set_empty(stock_key0.code);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_STOCK);
    do_extended_set_key(extended, 0, &stock_key0);
    do_extended_set_filter(extended, TRUE, 2, sizeof(stock_key0.sklad), sizeof(quant));
    memset(&quant,0,sizeof(quant));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(stock_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &stock_key0.sklad);

    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_UNSIGNED_BINARY,
                                offsetof(stock_struct_t, quant),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &quant);

    do_extended_set_fields_full_record(extended, sizeof(stock_struct_t));

    retval = do_extended_get(extended, -1);
    do_extended_free(extended);
    return retval;

}
*/
do_ret_list_t *replic_select_stocks(do_alias_t *alias, const char *sklad, do_extended_break_func break_func)
{
    do_ret_list_t *retval;
    stock_key0_t stock_key0,key;
    stock_rec_t stock;
    int status;
    retval = do_ret_list_static_new();

    do_text_set(alias, stock_key0.sklad, sklad);
    do_text_set_empty(stock_key0.code);
    do_cpy(key.sklad,stock_key0.sklad);
    status = do_stock_get0(alias,&stock,&stock_key0,DO_GET_GE);
    //!!sadint dd=0;
    while ( status == DO_OK ) {
    	if ( do_cmp(key.sklad,stock_key0.sklad) ) break;
    	if ( stock.data.code[5] != '.' || do_stock_quant(alias, &stock, 1) || do_stock_quant(alias, &stock, 2) ) {
    	     //do_log(LOG_INFO, "%s %f %f",do_text(alias,stock.data.code),
    	     //            do_stock_quant(alias, &stock, 1),
    	     //            do_stock_quant(alias, &stock, 2));//!!
     	     do_ret_list_static_add(retval, &stock.data, sizeof(stock_struct_t));
    	}
    	status = do_stock_get0(alias,&stock,&stock_key0,DO_GET_NEXT);
    	//!!saddd+=1;
    	//!!sadif (dd > 100) break;
    }
    return retval;
}
/*!!
int replic_select_stock(do_alias_t *alias, stock_key0_t *key, do_list_t *retval, do_extended_break_func break_func)
{
    stock_key0_t stock_key0;
    stock_rec_t stock;
    int status, count;

    do_cpy(stock_key0.sklad, key->sklad);
    do_cpy(stock_key0.code, key->code);

    status = do_stock_get0(alias,&stock,&stock_key0,DO_GET_GE);
    count = 0;
    while ( status == DO_OK ) {
    	if ( strncmp(key->code,stock_key0.code,do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE))
    	      || do_cmp(key->sklad,stock_key0.sklad) ) break;

    	if ( stock.data.code[5] != '.' || do_stock_quant(alias, &stock, 1) || do_stock_quant(alias, &stock, 2) || do_stock_quant(alias, &stock, 2) ) {
     	     do_list_add_alloc(retval, &stock.data, sizeof(stock_struct_t));
     	     count++;
    	}
    	status = do_stock_get0(alias,&stock,&stock_key0,DO_GET_NEXT);
    }
    if ( !count ) {
        do_cpy(stock.data.sklad,key->sklad);
        do_cpy(stock.data.code,key->code);

        do_stock_quant_clear(alias, &stock);
        do_list_add_alloc(retval, &stock.data, sizeof(stock_struct_t));
    }
    return status != DO_ERROR;
}
*/

int replic_products(do_alias_t *alias, void *data, size_t size, const char *sklad, int base_parcel, int clone, product_key4_t *key1, product_key4_t *key2, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    int            size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    product_key0_t    product_key0;
    product_rec_t     product;
    product_struct_t *product_st;
    product_struct_t *product_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz <= sizeof(product_struct_t) && *sz >= sizeof(product_struct_t) - sizeof(product.data.params));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    list = replic_select_product_base_parcel(alias, sklad, base_parcel, key1, key2, break_func);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        product_st = do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            product_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
            if (product_st->base_parcel == product_new_st->base_parcel &&
                !memcmp(&product_st->code, &product_new_st->code, sizeof(product_st->code))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if ( clone ) {
                    if ( (size_new != do_ret_list_item_len(list, i)) ||
                         memcmp(product_new_st, product_st, size_new) ) {
                        //product_set_l(alias, product_new_st, product_st);
                        do_list_add(do_list_ch, product_new_st);
                    }
                }
                else {
                    if ( !product_is_equal((product_struct_t*)product_new_st, size_new, (product_struct_t*)product_st, do_ret_list_item_len(list, i))) {
                        product_set_l(alias, product_new_st, product_st);
                        do_list_add(do_list_ch, product_new_st);
                    }
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, product_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        product.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&product.data, do_list_ins->list[i], product.size);

        product_set_l(alias, &product.data, NULL);
        code = do_text(alias, product.data.code);
        do_log(LOG_INFO, "insert product \"%s\"", code);
        if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_INSERT))
            status = DO_ERROR;
        else
            status = do_product_insert(alias, &product);

        do_free(code);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        product_st = do_list_ch->list[i];
        product_key0.base_parcel = product_st->base_parcel;
        memcpy(&product_key0.code,  &product_st->code,  sizeof(product_key0.code));

        code = do_text(alias, product_key0.code);
        do_log(LOG_INFO, "update product \"%s\"", code);

        status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            product.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&product.data, do_list_ch->list[i], product.size);
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                if ( clone )
                    status = do_product_update(alias, &product);
                else
                    status = product_update(alias, &product, break_func);
        }
        do_free(code);
    }
    if ( do_list_del->count > 200 && status == DO_OK ) {
         do_log(LOG_ERR, "delete %d products ",do_list_del->count);
         status = DO_ERROR;
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        product_st = do_list_del->list[i];
        product_key0.base_parcel = product_st->base_parcel;
        memcpy(&product_key0.code,  &product_st->code,  sizeof(product_key0.code));

        code = do_text(alias, product_key0.code);
        do_log(LOG_INFO, "delete product \"%s\"", code);

        status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_DELETE))
                status = DO_ERROR;
            else
                status = do_product_delete(alias);
        }
        do_free(code);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    if (list)
        do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}

int replic_products_view(do_alias_t *alias, void *data, size_t size, const char *sklad, int base_parcel, int clone, product_key4_t *key1, product_key4_t *key2, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    product_view_key0_t    product_view_key0;
    product_view_rec_t     product_view;
    product_view_struct_t *product_view_st;
    product_view_struct_t *product_view_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(product_view_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }


    list = replic_select_product_view_base_parcel(alias, sklad, base_parcel, key1, key2, break_func);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        product_view_st = do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            product_view_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if (product_view_st->type == product_view_new_st->type &&
                !memcmp(&product_view_st->sklad, &product_view_new_st->sklad, sizeof(product_view_st->sklad)) &&
                !memcmp(&product_view_st->sort, &product_view_new_st->sort, sizeof(product_view_st->sort)) &&
                !memcmp(&product_view_st->code, &product_view_new_st->code, sizeof(product_view_st->code))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(product_view_st, product_view_new_st, size_new)) {
                    do_list_add(do_list_ch, product_view_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, product_view_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        product_view.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&product_view.data, do_list_ins->list[i], product_view.size);

        code = do_text(alias, product_view.data.code);
        do_log(LOG_INFO, "insert product_view \"%s\"", code);

        if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
            status = DO_ERROR;
        else
            status = do_product_view_insert(alias, &product_view);
        do_free(code);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        product_view_st = do_list_ch->list[i];
        product_view_key0.type = product_view_st->type;
        memcpy(&product_view_key0.sklad, &product_view_st->sklad, sizeof(product_view_key0.sklad));
        memcpy(&product_view_key0.sort,  &product_view_st->sort,  sizeof(product_view_key0.sort));
        memcpy(&product_view_key0.code,  &product_view_st->code,  sizeof(product_view_key0.code));
        status = do_product_view_get0(alias, &product_view, &product_view_key0, DO_GET_EQUAL);

        code = do_text(alias, product_view_key0.code);
        do_log(LOG_INFO, "update product_view \"%s\"", code);

        status = do_product_view_get0(alias, &product_view, &product_view_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            product_view.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&product_view.data, do_list_ch->list[i], product_view.size);
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_product_view_update(alias, &product_view);
        }
        do_free(code);
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        product_view_st = do_list_del->list[i];
        product_view_key0.type = product_view_st->type;
        memcpy(&product_view_key0.sklad, &product_view_st->sklad, sizeof(product_view_key0.sklad));
        memcpy(&product_view_key0.sort,  &product_view_st->sort,  sizeof(product_view_key0.sort));
        memcpy(&product_view_key0.code,  &product_view_st->code,  sizeof(product_view_key0.code));

        code = do_text(alias, product_view_key0.code);
        do_log(LOG_INFO, "delete product_view \"%s\"", code);

        status = do_product_view_get0(alias, &product_view, &product_view_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_product_view_delete(alias);
        }
        do_free(code);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    if (list)
        do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}

int replic_products_data_2(do_alias_t *alias, void *data, size_t size, const char *sklad, int base_parcel, int clone, product_key4_t *key1, product_key4_t *key2, do_extended_break_func break_func)
{
    int            count, i, j, del, tr = 0;
    do_ret_list_t *list1;
    do_extended_t *extended;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    product_data_key0_t    product_data_key0;
    product_data_key0_t    product_data_key0_;
    product_data_key0_t    *product_data_key;
    product_key4_t         product_key4;
    product_data_rec_t     product_data;
    product_data_struct_t *product_data_st;
    product_data_struct_t *product_data_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz <= sizeof(product_data_struct_t) && *sz >= sizeof(product_data_struct_t) - sizeof(product_data.data.params));
            if ((key1 == NULL || do_cmp(key1->code,((product_data_struct_t *)crnt)->code) <= 0) &&
               (key2 == NULL || do_cmp(key2->code,((product_data_struct_t *)crnt)->code) > 0))
                 do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }
    do_text_set(alias,product_key4.code,PRODUCT_FIRST_CODE);

    char *prefix = NULL;
    int   code_size = 0;
    char  parcel_separator = do_param(DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR)[0];
    int   base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE);

    if ( base_parcel ) {
        prefix = domino_get_unit_prefix(alias, sklad);
        if (!prefix || *prefix == '\0') {
            do_log(LOG_ERR, "local product's prefix are empty");
            if (prefix)
                do_free(prefix);
            return FALSE;
        }
        code_size = strlen(prefix);
    }
    do_text_set_empty(product_data_key0_.code);
    product_data_key0_.type = 0;
    product_data_key0_.number = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PRODUCT_DATA);
    do_extended_set_key(extended, 0, &product_data_key0_);
    do_extended_set_filter(extended, FALSE, 1, (base_parcel) ? code_size : sizeof(parcel_separator));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(product_data_struct_t, code) + base_len,
                                (base_parcel) ?
                                  DO_EXTENDED_COMP_EQUAL :
                                  DO_EXTENDED_COMP_NOT_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                (base_parcel) ?
                                  prefix :
                                  &parcel_separator);
    do_extended_set_fields_full_record(extended, sizeof(product_data_struct_t));

    status = DO_OK;

    while ( ((list1 = do_extended_get(extended, 100)) != NULL) &&
            do_ret_list_count(list1) ) {
            tr += 100;
        for (i = 0; status == DO_OK && i < do_ret_list_count(list1); i++) {
            if (!RUNNING_) {
                status = DO_ERROR;
                break;
            }

            product_data_st = do_ret_list_item_data(list1, i);
            if ( !do_cmp(product_data_st->code,product_key4.code) )
                continue;
            code = do_text(alias, product_data_st->code);
            del = TRUE;
            for (j =0; j < do_list_ins->count; j++) {
                product_data_new_st = do_list_ins->list[j];
                size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

                if (product_data_st->type == product_data_new_st->type &&
                    product_data_st->number == product_data_new_st->number &&
                    !memcmp(&product_data_st->code, &product_data_new_st->code, sizeof(product_data_st->code))) {
                    del = FALSE;
                    do_list_delete(do_list_ins, j);
                    if (size_new != do_ret_list_item_len(list1, i) ||
                        memcmp(product_data_st, product_data_new_st, size_new)) {
                        do_list_add(do_list_ch, product_data_new_st);
                    }
                    break;
                }
            }
            if (del) {
                //do_list_add(do_list_del, product_data_st);
                product_data_key0.type = product_data_st->type;
                product_data_key0.number = product_data_st->number;
                do_cpy(product_data_key0.code,  product_data_st->code);
                do_list_add_alloc(do_list_del, &product_data_key0, sizeof(product_data_key0));
            }
        }
        do_ret_list_free(list1);
    }
    if (list1)
        do_ret_list_free(list1);

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        product_data.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&product_data.data, do_list_ins->list[i], product_data.size);

        //if ( product_data.data.type == PRODUCT_DATA_RESERVE_TYPE )
        //    continue;

        code = do_text(alias, product_data.data.code);
        do_log(LOG_INFO, "insert product_data \"%s\"", code);
        status = do_product_data_insert(alias, &product_data);
        if ( status == DO_OK ) {
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
        }
        do_free(code);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        product_data_st = do_list_ch->list[i];
        //if ( product_data_st->type == PRODUCT_DATA_RESERVE_TYPE )
        //    continue;


        product_data_key0.type = product_data_st->type;
        product_data_key0.number = product_data_st->number;
        memcpy(&product_data_key0.code,  &product_data_st->code,  sizeof(product_data_key0.code));
        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_EQUAL);

        code = do_text(alias, product_data_key0.code);
        do_log(LOG_INFO, "update product_data \"%s\"", code);

        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            product_data.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&product_data.data, do_list_ch->list[i], product_data.size);
            status = do_product_data_update(alias, &product_data);
            if ( status == DO_OK ) {
                if ( !do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE) )
                    status = DO_ERROR;
            }

        }
        do_free(code);
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }


        product_data_key = do_list_del->list[i];

        //product_data_key0.type = product_data_st->type;
        //product_data_key0.number = product_data_st->number;
        //memcpy(&product_data_key0.code,  &product_data_st->code,  sizeof(product_data_key0.code));

        code = do_text(alias, product_data_key->code);
        do_log(LOG_INFO, "delete product_data \"%s\" %d %d", code, product_data_key->type, product_data_key->number);
        //exit(1);//!!


        status = do_product_data_get0(alias, &product_data, product_data_key, DO_GET_EQUAL);
        if (status == DO_OK) {
            status = do_product_data_delete(alias);
            if ( status == DO_OK ) {
                if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                    status = DO_ERROR;
            }
        }
        do_free(code);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    //if (list)
    //    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}


int replic_barcodes(do_alias_t *alias, void *data, size_t size, const char *sklad, int base_parcel, int clone, product_key4_t *key1, product_key4_t *key2, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    barcode_key0_t    barcode_key0;
    barcode_rec_t     barcode;
    barcode_struct_t *barcode_st;
    barcode_struct_t *barcode_new_st;

#ifdef USE_OLD_BARCODE
    int base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE);
    char buf[14];
    buf[base_len + 2 + 5] = '\0';
#endif
    crnt = data;
    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(barcode_struct_t));
#ifdef USE_OLD_BARCODE
            if (base_parcel) {
                barcode_st = (barcode_struct_t*)crnt;
                if (strncmp(barcode_st->barcode + base_len + 2, barcode_st->code + base_len + 1, 2)) {
                    strncpy(buf, barcode_st->barcode, base_len + 2);
                    strncpy(buf + base_len + 2, barcode_st->code + base_len + 1, 5);
                    code = do_ean13(buf);
                    do_text_set(alias, barcode_st->barcode, code);
                    do_free(code);
                }
            }
#endif
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    list = replic_select_barcode_base_parcel(alias, sklad, base_parcel, key1,key2, break_func);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        barcode_st = do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            barcode_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
            if (!memcmp(&barcode_st->barcode, &barcode_new_st->barcode, sizeof(barcode_st->barcode))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(barcode_st, barcode_new_st, size_new)) {
                    do_list_add(do_list_ch, barcode_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, barcode_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        char *product_code;
        barcode_key0_t sad;
        barcode.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&barcode.data, do_list_ins->list[i], barcode.size);

        do_cpy(sad.barcode,barcode.data.barcode);
        if ( do_barcode_key0(alias, &sad, DO_GET_EQUAL) == DO_KEY_NOT_FOUND ) {
            code = do_text(alias, barcode.data.barcode);
            product_code = do_text(alias, barcode.data.code);
            do_log(LOG_INFO, "insert barcode \"%s\" product \"%s\"", code, product_code);
            do_free(code);
            do_free(product_code);
            code = do_text(alias, barcode.data.code);
            if ( !do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else {
                barcode_key0_t barcode_key0;
                barcode_rec_t bar1;
                do_cpy(barcode_key0.barcode,barcode.data.barcode);
                status = do_barcode_get0(alias, &bar1, &barcode_key0, DO_GET_EQUAL);
                if ( status == DO_KEY_NOT_FOUND )
                    status = do_barcode_insert(alias, &barcode);
                else
                    status = do_barcode_update(alias, &barcode);
            }
            do_free(code);
        }
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        barcode_st = do_list_ch->list[i];
        memcpy(&barcode_key0.barcode,  &barcode_st->barcode,  sizeof(barcode_key0.barcode));
        status = do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL);

        code = do_text(alias, barcode_key0.barcode);
        do_log(LOG_INFO, "update barcode \"%s\"", code);
        do_free(code);

        status = do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            barcode.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&barcode.data, do_list_ch->list[i], barcode.size);
            code = do_text(alias, barcode.data.code);
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_barcode_update(alias, &barcode);
            do_free(code);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        barcode_st = do_list_del->list[i];
        memcpy(&barcode_key0.barcode,  &barcode_st->barcode,  sizeof(barcode_key0.barcode));

        code = do_text(alias, barcode_key0.barcode);
        do_log(LOG_INFO, "delete barcode \"%s\"", code);
        do_free(code);

        status = do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            code = do_text(alias, barcode.data.code);
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_barcode_delete(alias);
            do_free(code);
        }
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    if (list)
        do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}

int replic_partner(do_alias_t *alias, const char *g_code, int code, void *data, size_t size, do_extended_break_func break_func)
{
    int status;
    partner_rec_t partner, partner_new;
    partner_key0_t partner_key0;

    void         *crnt = NULL;
    do_size_t  *sz;
    char         *value;

    if (!size) { // delete
        do_text_set(alias, partner_key0.g_code, g_code);
        partner_key0.code = code;

        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            do_log(LOG_INFO, "delete partner \"%s.%d\"", g_code, code);
            if (!do_protocol_partner_add_rec(alias, &partner, DO_PROTOCOL_ACTION_DELETE))
                status = DO_ERROR;
            else
                status = do_partner_delete(alias);
        }
        else {
            memcpy(&partner_new.data.g_code, &partner_key0.g_code, sizeof(partner_new.data.g_code));
            partner_new.data.code = partner_key0.code;
            if (status == DO_KEY_NOT_FOUND)
                status = DO_OK;
        }
    }
    else {
        crnt = data;
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);

        debug_assert(*sz >= sizeof(partner_struct_t) - sizeof(partner.data.params) &&
                     *sz <= sizeof(partner_struct_t));

        partner_new.size = *sz;
        memcpy(&partner_new.data, crnt, partner_new.size);
        crnt += partner_new.size;
        debug_assert(crnt <= data + size);

        memcpy(&partner_key0.g_code, &partner_new.data.g_code, sizeof(partner_new.data.g_code));
        partner_key0.code = partner_new.data.code;

        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);

        if (status == DO_ERROR)
            return 0;

        if (status == DO_KEY_NOT_FOUND) {
            value = do_text(alias, partner_new.data.g_code);
            do_log(LOG_INFO, "insert partner \"%s.%d\"", value, partner_new.data.code);
            do_free(value);

            if (!do_protocol_partner_add_rec(alias, &partner_new, DO_PROTOCOL_ACTION_INSERT))
                status = DO_ERROR;
            else
                status = do_partner_insert(alias, &partner_new);
        }
        else
            if (partner_new.size != partner.size ||
                memcmp(&partner_new.data, &partner.data, partner.size)) {
                value = do_text(alias, partner_new.data.g_code);
                do_log(LOG_INFO, "update partner \"%s.%d\"", value, partner_new.data.code);
                do_free(value);
                if (!do_protocol_partner_add_rec(alias, &partner_new, DO_PROTOCOL_ACTION_CHANGE))
                    status = DO_ERROR;
                else
                    status = do_partner_update(alias, &partner_new);
            }
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }

    if (status != DO_OK)
        return FALSE;

    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    do_extended_t *extended = NULL;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

#ifdef USE_DB_PARNER_VIEW
// partner_view
    partner_view_key1_t   partner_view_key1;
    partner_view_key0_t   partner_view_key0;
    partner_view_rec_t    partner_view;
    partner_view_struct_t *partner_view_st;
    partner_view_struct_t *partner_view_new_st;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz < 1000);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(partner_view_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    do_cpy(partner_view_key1.g_code, partner_new.data.g_code);
    do_cpy(partner_view_key1.code, partner_new.data.code);
    partner_view_key1.type = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PARTNER_VIEW);
    do_extended_set_key(extended, 1, &partner_view_key1);
    do_extended_set_filter(extended, TRUE, 2, sizeof(partner_view_key1.g_code), sizeof(partner_view_key1.code));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(partner_view_struct_t, g_code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &partner_view_key1.g_code);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(partner_view_struct_t, code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &partner_view_key1.code);

    do_extended_set_fields_full_record(extended, sizeof(partner_view_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);
    extended = NULL;

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        del = TRUE;
        partner_view_st = do_ret_list_item_data(list, i);
        for (j =0; j < do_list_ins->count; j++) {
            partner_view_new_st = do_list_ins->list[j];
            if (partner_view_st->type == partner_view_new_st->type &&
                partner_view_st->code == partner_view_new_st->code &&
                !memcmp(&partner_view_st->g_code, &partner_view_new_st->g_code, sizeof(partner_view_st->g_code)) &&
                !memcmp(&partner_view_st->sort, &partner_view_new_st->sort, sizeof(partner_view_st->sort))) {

                del = FALSE;
                size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
                do_list_delete(do_list_ins, j);

                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(partner_view_st, partner_view_new_st, size_new)) {
                    do_list_add(do_list_ch, partner_view_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, partner_view_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        do_log(LOG_INFO, "insert partner_view \"%s.%d\"", g_code, code);

        partner_view.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&partner_view.data, do_list_ins->list[i], partner_view.size);
        if (!do_protocol_partner_add_keyv(alias, &partner_view, DO_PROTOCOL_ACTION_CHANGE))
            status = DO_ERROR;
        else
            status = do_partner_view_insert(alias, &partner_view);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        do_log(LOG_INFO, "update partner_view \"%s.%d\"", g_code, code);

        partner_view_st = do_list_ch->list[i];
        partner_view_key0.type = partner_view_st->type;
        partner_view_key0.code = partner_view_st->code;
        memcpy(&partner_view_key0.sort,  &partner_view_st->sort,  sizeof(partner_view_key0.sort));
        memcpy(&partner_view_key0.g_code,  &partner_view_st->g_code,  sizeof(partner_view_key0.g_code));
        status = do_partner_view_get0(alias, &partner_view, &partner_view_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            partner_view.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&partner_view.data, do_list_ch->list[i], partner_view.size);
            if (!do_protocol_partner_add_recv(alias, &partner_view, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_partner_view_update(alias, &partner_view);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        do_log(LOG_INFO, "delete partner_view \"%s.%d\"", g_code, code);

        partner_view_st = do_list_del->list[i];
        partner_view_key0.type = partner_view_st->type;
        partner_view_key0.code = partner_view_st->code;
        memcpy(&partner_view_key0.sort,  &partner_view_st->sort,  sizeof(partner_view_key0.sort));
        memcpy(&partner_view_key0.g_code,  &partner_view_st->g_code,  sizeof(partner_view_key0.g_code));
        status = do_partner_view_get0(alias, &partner_view, &partner_view_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            if (!do_protocol_partner_add_recv(alias, &partner_view, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_partner_view_delete(alias);
        }
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    do_list_clear(do_list_ins);
    do_list_clear(do_list_del);
    do_list_clear(do_list_ch);
    do_ret_list_free(list);
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }

    if (status != DO_OK) {
        do_list_free(do_list_ins);
        do_list_free(do_list_del);
        do_list_free(do_list_ch);
        return FALSE;
    }
#else
    crnt += sizeof(*sz);
#endif
// partner data

    partner_data_key0_t   partner_data_key0;
    partner_data_rec_t    partner_data;
    partner_data_struct_t *partner_data_st;
    partner_data_struct_t *partner_data_new_st;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz < 1000);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz >= sizeof(partner_data_struct_t) - sizeof(partner_data.data.params) &&
                     *sz <= sizeof(partner_data_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    memcpy(&partner_data_key0.g_code, &partner_new.data.g_code, sizeof(partner_new.data.g_code));
    partner_data_key0.code = partner_new.data.code;
    partner_data_key0.type = 0;
    partner_data_key0.number = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PARTNER_DATA);
    do_extended_set_key(extended, 0, &partner_data_key0);
    do_extended_set_filter(extended, TRUE, 2, sizeof(partner_data_key0.g_code), sizeof(partner_data_key0.code));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(partner_data_struct_t, g_code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &partner_data_key0.g_code);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(partner_data_struct_t, code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &partner_data_key0.code);

    do_extended_set_fields_full_record(extended, sizeof(partner_data_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        del = TRUE;
        partner_data_st = do_ret_list_item_data(list, i);
        for (j =0; j < do_list_ins->count; j++) {
            partner_data_new_st = do_list_ins->list[j];
            if (partner_data_st->type == partner_data_new_st->type &&
                partner_data_st->number == partner_data_new_st->number &&
                partner_data_st->code == partner_data_new_st->code &&
                !memcmp(&partner_data_st->g_code, &partner_data_new_st->g_code, sizeof(partner_data_st->g_code))) {

                del = FALSE;
                size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
                do_list_delete(do_list_ins, j);

                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(partner_data_st, partner_data_new_st, size_new)) {
                    do_list_add(do_list_ch, partner_data_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, partner_data_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        do_log(LOG_INFO, "insert partner_data \"%s.%d\"", g_code, code);

        partner_data.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&partner_data.data, do_list_ins->list[i], partner_data.size);
        if (!do_protocol_partner_add_recd(alias, &partner_data, DO_PROTOCOL_ACTION_CHANGE))
            status = DO_ERROR;
        else
            status = do_partner_data_insert(alias, &partner_data);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        do_log(LOG_INFO, "update partner_data \"%s.%d\"", g_code, code);

        partner_data_st = do_list_ch->list[i];
        partner_data_key0.type = partner_data_st->type;
        partner_data_key0.number = partner_data_st->number;
        partner_data_key0.code  = partner_data_st->code;
        memcpy(&partner_data_key0.g_code,  &partner_data_st->g_code,  sizeof(partner_data_key0.g_code));
        status = do_partner_data_get0(alias, &partner_data, &partner_data_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            partner_data.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&partner_data.data, do_list_ch->list[i], partner_data.size);
            if (!do_protocol_partner_add_recd(alias, &partner_data, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_partner_data_update(alias, &partner_data);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        do_log(LOG_INFO, "delete partner_data \"%s.%d\"", g_code, code);

        partner_data_st = do_list_del->list[i];
        partner_data_key0.type = partner_data_st->type;
        partner_data_key0.number = partner_data_st->number;
        partner_data_key0.code = partner_data_st->code;
        memcpy(&partner_data_key0.g_code,  &partner_data_st->g_code,  sizeof(partner_data_key0.g_code));
        status = do_partner_data_get0(alias, &partner_data, &partner_data_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            if (!do_protocol_partner_add_recd(alias, &partner_data, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_partner_data_delete(alias);
        }
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);
    do_ret_list_free(list);

    return status == DO_OK;

}
int replic_document_order(do_alias_t *alias, const char *dtype, const char *sklad, const char *document, int line, void *data, size_t size, int *changed, do_extended_break_func break_func)
{
    int status;
    document_order_rec_t document_order, document_order_new;
    document_order_key0_t document_order_key0;

    void         *crnt;
    do_size_t  *sz;

    if (!size) { // delete
        do_text_set(alias, document_order_key0.dtype, dtype);
        do_text_set(alias, document_order_key0.sklad, sklad);
        do_text_set(alias, document_order_key0.document, document);
        document_order_key0.line = line;

        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            do_log(LOG_INFO, "delete document_order \"%s.%s.%s.%d\"", dtype, sklad, document, line);
            status = do_document_order_delete(alias);
            *changed = TRUE;
        }
        else
            status = DO_OK;
    }
    else {
        crnt = data;
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);

        debug_assert(*sz >= sizeof(document_order_struct_t) - sizeof(document_order.data.params) &&
                     *sz <= sizeof(document_order_struct_t));

        document_order_new.size = *sz;
        memcpy(&document_order_new.data, crnt, document_order_new.size);
        crnt += document_order_new.size;
        debug_assert(crnt <= data + size);

        do_cpy(document_order_key0.dtype, document_order_new.data.dtype);
        do_cpy(document_order_key0.sklad, document_order_new.data.sklad);
        do_cpy(document_order_key0.document, document_order_new.data.document);
        document_order_key0.line = document_order_new.data.line;

        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_EQUAL);

        if (status == DO_ERROR)
            return 0;

        if (status == DO_KEY_NOT_FOUND) {
            do_log(LOG_INFO, "insert document_order \"%s.%s.%s.%d\"", dtype, sklad, document, line);
            status = do_document_order_insert(alias, &document_order_new);
            *changed = TRUE;
        }
        else
            if (document_order_new.size != document_order.size ||
                memcmp(&document_order_new.data, &document_order.data, document_order.size)) {
                do_log(LOG_INFO, "update document_order \"%s.%s.%s.%d\"", dtype, sklad, document, line);
                status = do_document_order_update(alias, &document_order_new);
                *changed = TRUE;
            }
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }

    return status == DO_OK;

}

int replic_regions(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    region_key0_t    region_key0;
    region_rec_t     region;
    region_struct_t *region_st;
    region_struct_t *region_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz <= sizeof(region_struct_t) && *sz >= sizeof(region_struct_t) - sizeof(region.data.params));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    do_text_set_empty(region_key0.code);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_REGION);
    do_extended_set_key(extended, 0, &region_key0);
    do_extended_set_fields_full_record(extended, sizeof(region_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        region_st = (region_struct_t*)do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            region_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if (!memcmp(&region_st->code, &region_new_st->code, sizeof(region_st->code))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(region_st, region_new_st, size_new)) {
                    do_list_add(do_list_ch, region_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, region_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        region.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&region.data, do_list_ins->list[i], region.size);

        code = do_text(alias, region.data.code);
        do_log(LOG_INFO, "insert region \"%s\"", code);
        do_free(code);

        status = do_region_insert(alias, &region);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        region_st = do_list_ch->list[i];
        memcpy(&region_key0.code,  &region_st->code,  sizeof(region_key0.code));
        status = do_region_get0(alias, &region, &region_key0, DO_GET_EQUAL);

        code = do_text(alias, region_key0.code);
        do_log(LOG_INFO, "update region \"%s\"", code);
        do_free(code);

        status = do_region_get0(alias, &region, &region_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            region.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&region.data, do_list_ch->list[i], region.size);
            status = do_region_update(alias, &region);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        region_st = do_list_del->list[i];
        memcpy(&region_key0.code,  &region_st->code,  sizeof(region_key0.code));

        code = do_text(alias, region_key0.code);
        do_log(LOG_INFO, "delete region \"%s\"", code);
        do_free(code);

        status = do_region_get0(alias, &region, &region_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            status = do_region_delete(alias);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}
int replic_partners(do_alias_t *alias, void *data, size_t size, const char *g_code, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    partner_key0_t    partner_key0;
    partner_rec_t     partner;
    partner_struct_t *partner_st;
    partner_struct_t *partner_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz <= sizeof(partner_struct_t) && *sz >= sizeof(partner_struct_t) - sizeof(partner.data.params));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    if (g_code)
        do_text_set(alias, partner_key0.g_code, g_code);
    else
        do_text_set(alias, partner_key0.g_code, "");
    partner_key0.code = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PARTNER);
    do_extended_set_key(extended, 0, &partner_key0);

    if (g_code) {
        do_extended_set_filter(extended, TRUE, 1, sizeof(partner_key0.g_code));
        do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(partner_struct_t, g_code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &partner_key0.g_code);
    }
    do_extended_set_fields_full_record(extended, sizeof(partner_struct_t));

    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        partner_st = do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            partner_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
            if (partner_st->code == partner_new_st->code &&
                !memcmp(&partner_st->g_code, &partner_new_st->g_code, sizeof(partner_st->g_code))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(partner_st, partner_new_st, size_new)) {
                    do_list_add(do_list_ch, partner_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, partner_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        partner.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&partner.data, do_list_ins->list[i], partner.size);

        code = do_text(alias, partner.data.g_code);
        do_log(LOG_INFO, "insert partner \"%s.%d\"", code, partner.data.code);
        do_free(code);

        if (!do_protocol_partner_add_rec(alias, &partner, DO_PROTOCOL_ACTION_INSERT))
            status = DO_ERROR;
        else
            status = do_partner_insert(alias, &partner);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        partner_st = do_list_ch->list[i];
        partner_key0.code = partner_st->code;
        memcpy(&partner_key0.g_code,  &partner_st->g_code,  sizeof(partner_key0.g_code));

        code = do_text(alias, partner_key0.g_code);
        do_log(LOG_INFO, "update partner \"%s.%d\"", code, partner_key0.code);
        do_free(code);

        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            partner.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&partner.data, do_list_ch->list[i], partner.size);
            if (!do_protocol_partner_add_rec(alias, &partner, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_partner_update(alias, &partner);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        partner_st = do_list_del->list[i];
        partner_key0.code = partner_st->code;
        memcpy(&partner_key0.g_code,  &partner_st->g_code,  sizeof(partner_key0.g_code));


        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
        if ( status == DO_OK && do_partner_param_int(alias, &partner, "MANUL") != 1 ) {
            code = do_text(alias, partner_key0.g_code);
            do_log(LOG_INFO, "delete partner \"%s.%d\"", code, partner_key0.code);
            do_free(code);
            if (!do_protocol_partner_add_rec(alias, &partner, DO_PROTOCOL_ACTION_DELETE))
                status = DO_ERROR;
            else
                status = do_partner_delete(alias);
        }
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}

int replic_partners_view(do_alias_t *alias, void *data, size_t size, const char *g_code, do_extended_break_func break_func)
{
#ifdef USE_DB_PARNER_VIEW
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    partner_view_key1_t    partner_view_key1;
    partner_view_rec_t     partner_view;
    partner_view_struct_t *partner_view_st;
    partner_view_struct_t *partner_view_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(partner_view_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    if (g_code)
        do_text_set(alias, partner_view_key1.g_code, g_code);
    else
        do_text_set(alias, partner_view_key1.g_code, "");
    partner_view_key1.code = 0;
    partner_view_key1.type = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PARTNER_VIEW);
    do_extended_set_key(extended, 1, &partner_view_key1);

    if (g_code) {
        do_extended_set_filter(extended, TRUE, 1, sizeof(partner_view_key1.g_code));
        do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(partner_view_struct_t, g_code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &partner_view_key1.g_code);
    }
    do_extended_set_fields_full_record(extended, sizeof(partner_view_struct_t));

    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        partner_view_st = do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            partner_view_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
            if (partner_view_st->code == partner_view_new_st->code &&
                !memcmp(&partner_view_st->g_code, &partner_view_new_st->g_code, sizeof(partner_view_st->g_code))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(partner_view_st, partner_view_new_st, size_new)) {
                    do_list_add(do_list_ch, partner_view_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, partner_view_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        partner_view.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&partner_view.data, do_list_ins->list[i], partner_view.size);

        code = do_text(alias, partner_view.data.g_code);
        do_log(LOG_INFO, "insert partner_view \"%s.%d\"", code, partner_view.data.code);
        do_free(code);

        if (!do_protocol_partner_view_add_rec(alias, &partner_view, DO_PROTOCOL_ACTION_INSERT))
            status = DO_ERROR;
        else
            status = do_partner_view_insert(alias, &partner_view);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        partner_view_st = do_list_ch->list[i];
        partner_view_key0.code = partner_view_st->code;
        memcpy(&partner_view_key0.g_code,  &partner_view_st->g_code,  sizeof(partner_view_key0.g_code));

        code = do_text(alias, partner_view_key0.g_code);
        do_log(LOG_INFO, "update partner_view \"%s.%d\"", code, partner_view_key0.code);
        do_free(code);

        status = do_partner_view_get0(alias, &partner_view, &partner_view_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            partner_view.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&partner_view.data, do_list_ch->list[i], partner_view.size);
            if (!do_protocol_partner_view_add_rec(alias, &partner_view, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_partner_view_update(alias, &partner_view);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        partner_view_st = do_list_del->list[i];
        partner_view_key0.code = partner_view_st->code;
        memcpy(&partner_view_key0.g_code,  &partner_view_st->g_code,  sizeof(partner_view_key0.g_code));

        code = do_text(alias, partner_view_key0.g_code);
        do_log(LOG_INFO, "delete partner_view \"%s.%d\"", code, partner_view_key0.code);
        do_free(code);

        status = do_partner_view_get0(alias, &partner_view, &partner_view_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            if (!do_protocol_partner_view_add_rec(alias, &partner_view, DO_PROTOCOL_ACTION_DELETE))
                status = DO_ERROR;
            else
                status = do_partner_view_delete(alias);
        }
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }

    return status == DO_OK;
#else
    return TRUE;
#endif
}

int replic_partners_data(do_alias_t *alias, void *data, size_t size, const char *g_code, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    partner_data_key0_t    partner_data_key0;
    partner_data_rec_t     partner_data;
    partner_data_struct_t *partner_data_st;
    partner_data_struct_t *partner_data_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz <= sizeof(partner_data_struct_t) && *sz >= sizeof(partner_data_struct_t) - sizeof(partner_data.data.params));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    if (g_code)
        do_text_set(alias, partner_data_key0.g_code, g_code);
    else
        do_text_set(alias, partner_data_key0.g_code, "");
    partner_data_key0.code = 0;
    partner_data_key0.type = 0;
    partner_data_key0.number = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PARTNER_DATA);
    do_extended_set_key(extended, 0, &partner_data_key0);

    if (g_code) {
        do_extended_set_filter(extended, TRUE, 1, sizeof(partner_data_key0.g_code));
        do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(partner_data_struct_t, g_code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &partner_data_key0.g_code);
    }
    do_extended_set_fields_full_record(extended, sizeof(partner_data_struct_t));

    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        partner_data_st = do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            partner_data_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
            if (partner_data_st->code == partner_data_new_st->code &&
                !do_cmp(partner_data_st->g_code, partner_data_new_st->g_code) &&
                partner_data_st->type == partner_data_new_st->type &&
                partner_data_st->number == partner_data_new_st->number
                ) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(partner_data_st, partner_data_new_st, size_new)) {
                    do_list_add(do_list_ch, partner_data_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, partner_data_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        partner_data.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&partner_data.data, do_list_ins->list[i], partner_data.size);

        code = do_text(alias, partner_data.data.g_code);
        do_log(LOG_INFO, "insert partner_data \"%s.%d\"", code, partner_data.data.code);

        if (!do_protocol_partner_add(alias, code, partner_data.data.code, DO_PROTOCOL_ACTION_INSERT))
            status = DO_ERROR;
        else
            status = do_partner_data_insert(alias, &partner_data);
        do_free(code);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        partner_data_st = do_list_ch->list[i];
        do_cpy(partner_data_key0.g_code, partner_data_st->g_code);
        partner_data_key0.code = partner_data_st->code;
        partner_data_key0.type = partner_data_st->type;
        partner_data_key0.number = partner_data_st->number;

        code = do_text(alias, partner_data_key0.g_code);
        do_log(LOG_INFO, "update partner_data \"%s.%d\"", code, partner_data_key0.code);

        status = do_partner_data_get0(alias, &partner_data, &partner_data_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            partner_data.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&partner_data.data, do_list_ch->list[i], partner_data.size);

            if (!do_protocol_partner_add(alias, code, partner_data.data.code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
            else
                status = do_partner_data_update(alias, &partner_data);
        }
        do_free(code);
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        partner_data_st = do_list_del->list[i];
        do_cpy(partner_data_key0.g_code, partner_data_st->g_code);
        partner_data_key0.code = partner_data_st->code;
        partner_data_key0.type = partner_data_st->type;
        partner_data_key0.number = partner_data_st->number;

        code = do_text(alias, partner_data_key0.g_code);
        do_log(LOG_INFO, "delete partner_data \"%s.%d\"", code, partner_data_key0.code);

        status = do_partner_data_get0(alias, &partner_data, &partner_data_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            if (!do_protocol_partner_add(alias, code, partner_data.data.code, DO_PROTOCOL_ACTION_DELETE))
                status = DO_ERROR;
            else
                status = do_partner_data_delete(alias);
        }
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
        do_free(code);
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}

int replic_class(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    class_key0_t    class_key0;
    class_rec_t     class_;
    class_struct_t *class_st;
    class_struct_t *class_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(class_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    do_text_set_empty(class_key0.class_id);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_CLASS);
    do_extended_set_key(extended, 0, &class_key0);
    do_extended_set_fields_full_record(extended, sizeof(class_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        class_st = (class_struct_t*)do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            class_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if (!memcmp(&class_st->class_id, &class_new_st->class_id, sizeof(class_st->class_id))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(class_st, class_new_st, size_new)) {
                    do_list_add(do_list_ch, class_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, class_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        class_.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&class_.data, do_list_ins->list[i], class_.size);

        code = do_text(alias, class_.data.class_id);
        do_log(LOG_INFO, "insert class \"%s\"", code);
        do_free(code);

        status = do_class_insert(alias, &class_);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        class_st = do_list_ch->list[i];
        memcpy(&class_key0.class_id,  &class_st->class_id,  sizeof(class_key0.class_id));
        status = do_class_get0(alias, &class_, &class_key0, DO_GET_EQUAL);

        code = do_text(alias, class_key0.class_id);
        do_log(LOG_INFO, "update class \"%s\"", code);
        do_free(code);

        status = do_class_get0(alias, &class_, &class_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            class_.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&class_.data, do_list_ch->list[i], class_.size);
            status = do_class_update(alias, &class_);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        class_st = do_list_del->list[i];
        memcpy(&class_key0.class_id,  &class_st->class_id,  sizeof(class_key0.class_id));

        code = do_text(alias, class_key0.class_id);
        do_log(LOG_INFO, "delete class \"%s\"", code);
        do_free(code);

        status = do_class_get0(alias, &class_, &class_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            status = do_class_delete(alias);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}
int replic_groups(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code,*value;
    void          *crnt;
    do_size_t   *sz;
    int            status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    group_key0_t    group_key0;
    group_rec_t     group_;
    group_struct_t *group_st;
    group_struct_t *group_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(group_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    do_text_set_empty(group_key0.class_id);
    do_text_set_empty(group_key0.group_id);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_GROUP);
    do_extended_set_key(extended, 0, &group_key0);
    do_extended_set_fields_full_record(extended, sizeof(group_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        group_st = (group_struct_t*)do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            group_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if ( !memcmp(&group_st->class_id, &group_new_st->class_id, sizeof(group_st->class_id)) &&
                 !memcmp(&group_st->group_id, &group_new_st->group_id, sizeof(group_st->group_id))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(group_st, group_new_st, size_new)) {
                    do_list_add(do_list_ch, group_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, group_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        group_.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&group_.data, do_list_ins->list[i], group_.size);

        code = do_text(alias, group_.data.class_id);
        value = do_text(alias, group_.data.group_id);
        do_log(LOG_INFO, "insert group \"%s.%s\"", code, value);
        do_free(code);
        do_free(value);

        status = do_group_insert(alias, &group_);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        group_st = do_list_ch->list[i];
        memcpy(&group_key0.class_id,  &group_st->class_id,  sizeof(group_key0.class_id));
        memcpy(&group_key0.group_id,  &group_st->group_id,  sizeof(group_key0.group_id));
        status = do_group_get0(alias, &group_, &group_key0, DO_GET_EQUAL);

        code = do_text(alias, group_key0.class_id);
        value = do_text(alias, group_key0.group_id);
        do_log(LOG_INFO, "update group \"%s.%s\"", code, value);
        do_free(code);
        do_free(value);

        status = do_group_get0(alias, &group_, &group_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            group_.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&group_.data, do_list_ch->list[i], group_.size);
            status = do_group_update(alias, &group_);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        group_st = do_list_del->list[i];
        do_cpy(group_key0.class_id,  group_st->class_id);
        do_cpy(group_key0.group_id,  group_st->group_id);

        code = do_text(alias, group_key0.class_id);
        value = do_text(alias, group_key0.group_id);
        do_log(LOG_INFO, "delete group \"%s.%s\"", code, value);
        do_free(code);
        do_free(value);

        status = do_group_get0(alias, &group_, &group_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            status = do_group_delete(alias);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}
int replic_subgroups(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code,*value, *val;
    void          *crnt;
    do_size_t   *sz;
    int            status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    subgroup_key0_t    subgroup_key0;
    subgroup_rec_t     subgroup_;
    subgroup_struct_t *subgroup_st;
    subgroup_struct_t *subgroup_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz >= sizeof(subgroup_struct_t) - sizeof(subgroup_.data.params) &&
                     *sz <= sizeof(subgroup_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    do_text_set_empty(subgroup_key0.class_id);
    do_text_set_empty(subgroup_key0.group_id);
    do_text_set_empty(subgroup_key0.subgroup_id);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_SUBGROUP);
    do_extended_set_key(extended, 0, &subgroup_key0);
    do_extended_set_fields_full_record(extended, sizeof(subgroup_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        subgroup_st = (subgroup_struct_t*)do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            subgroup_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if ( !memcmp(&subgroup_st->class_id, &subgroup_new_st->class_id, sizeof(subgroup_st->class_id)) &&
                 !memcmp(&subgroup_st->group_id, &subgroup_new_st->group_id, sizeof(subgroup_st->group_id)) &&
                 !memcmp(&subgroup_st->subgroup_id, &subgroup_new_st->subgroup_id, sizeof(subgroup_st->subgroup_id))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(subgroup_st, subgroup_new_st, size_new)) {
                    do_list_add(do_list_ch, subgroup_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, subgroup_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        subgroup_.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&subgroup_.data, do_list_ins->list[i], subgroup_.size);

        code = do_text(alias, subgroup_.data.class_id);
        value = do_text(alias, subgroup_.data.group_id);
        val   = do_text(alias, subgroup_.data.subgroup_id);
        do_log(LOG_INFO, "insert subgroup \"%s.%s.%s\"", code, value, val);
        do_free(code);
        do_free(value);
        do_free(val);

        status = do_subgroup_insert(alias, &subgroup_);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        subgroup_st = do_list_ch->list[i];
        memcpy(&subgroup_key0.class_id,  &subgroup_st->class_id,  sizeof(subgroup_key0.class_id));
        memcpy(&subgroup_key0.group_id,  &subgroup_st->group_id,  sizeof(subgroup_key0.group_id));
        memcpy(&subgroup_key0.subgroup_id,  &subgroup_st->subgroup_id,  sizeof(subgroup_key0.subgroup_id));
        status = do_subgroup_get0(alias, &subgroup_, &subgroup_key0, DO_GET_EQUAL);

        code = do_text(alias, subgroup_key0.class_id);
        value = do_text(alias, subgroup_key0.group_id);
        val = do_text(alias, subgroup_key0.subgroup_id);
        do_log(LOG_INFO, "update subgroup \"%s.%s\"", code, value);
        do_free(code);
        do_free(value);
        do_free(val);

        status = do_subgroup_get0(alias, &subgroup_, &subgroup_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            subgroup_.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&subgroup_.data, do_list_ch->list[i], subgroup_.size);
            status = do_subgroup_update(alias, &subgroup_);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        subgroup_st = do_list_del->list[i];
        do_cpy(subgroup_key0.class_id,  subgroup_st->class_id);
        do_cpy(subgroup_key0.group_id,  subgroup_st->group_id);
        do_cpy(subgroup_key0.subgroup_id,  subgroup_st->subgroup_id);

        code = do_text(alias, subgroup_key0.class_id);
        value = do_text(alias, subgroup_key0.group_id);
        val = do_text(alias, subgroup_key0.subgroup_id);
        do_log(LOG_INFO, "delete subgroup \"%s.%s.%s\"", code, value, val);
        do_free(code);
        do_free(value);
        do_free(val);

        status = do_subgroup_get0(alias, &subgroup_, &subgroup_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            status = do_subgroup_delete(alias);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}
int replic_operations(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code,*value;
    void          *crnt;
    do_size_t   *sz;
    int            status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    document_type_key0_t    document_type_key0;
    document_type_rec_t     document_type_;
    document_type_struct_t *document_type_st;
    document_type_struct_t *document_type_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz >= sizeof(document_type_struct_t) - sizeof(document_type_.data.params) &&
                     *sz <= sizeof(document_type_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    do_text_set_empty(document_type_key0.dtype);
    do_text_set_empty(document_type_key0.dtype1);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_TYPE);
    do_extended_set_key(extended, 0, &document_type_key0);
    do_extended_set_fields_full_record(extended, sizeof(document_type_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        document_type_st = (document_type_struct_t*)do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            document_type_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if ( !memcmp(&document_type_st->dtype, &document_type_new_st->dtype, sizeof(document_type_st->dtype)) &&
                 !memcmp(&document_type_st->dtype1, &document_type_new_st->dtype1, sizeof(document_type_st->dtype1))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(document_type_st, document_type_new_st, size_new)) {
                    do_list_add(do_list_ch, document_type_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, document_type_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        document_type_.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&document_type_.data, do_list_ins->list[i], document_type_.size);

        code = do_text(alias, document_type_.data.dtype);
        value = do_text(alias, document_type_.data.dtype1);
        do_log(LOG_INFO, "insert operation \"%s.%s\"", code, value);
        do_free(code);
        do_free(value);

        status = do_document_type_insert(alias, &document_type_);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        document_type_st = do_list_ch->list[i];
        memcpy(&document_type_key0.dtype,  &document_type_st->dtype,  sizeof(document_type_key0.dtype));
        memcpy(&document_type_key0.dtype1,  &document_type_st->dtype1,  sizeof(document_type_key0.dtype1));
        status = do_document_type_get0(alias, &document_type_, &document_type_key0, DO_GET_EQUAL);

        code = do_text(alias, document_type_key0.dtype);
        value = do_text(alias, document_type_key0.dtype1);
        do_log(LOG_INFO, "update operation \"%s.%s\"", code, value);
        do_free(code);
        do_free(value);

        status = do_document_type_get0(alias, &document_type_, &document_type_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            document_type_.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&document_type_.data, do_list_ch->list[i], document_type_.size);
            status = do_document_type_update(alias, &document_type_);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        document_type_st = do_list_del->list[i];
        do_cpy(document_type_key0.dtype,  document_type_st->dtype);
        do_cpy(document_type_key0.dtype1,  document_type_st->dtype1);

        code = do_text(alias, document_type_key0.dtype);
        value = do_text(alias, document_type_key0.dtype1);
        do_log(LOG_INFO, "delete operation \"%s.%s\"", code, value);
        do_free(code);
        do_free(value);

        status = do_document_type_get0(alias, &document_type_, &document_type_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            status = do_document_type_delete(alias);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_ret_list_free(list);
    do_list_clear(do_list_ins);
    do_list_clear(do_list_del);
    do_list_clear(do_list_ch);

// document_prow
    document_prow_key0_t    document_prow_key0;
    document_prow_rec_t     document_prow_;
    document_prow_struct_t *document_prow_st;
    document_prow_struct_t *document_prow_new_st;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz >= sizeof(document_prow_struct_t) - sizeof(document_prow_.data.params) &&
                     *sz <= sizeof(document_prow_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    do_text_set_empty(document_prow_key0.dtype);
    do_text_set_empty(document_prow_key0.dtype1);
    document_prow_key0.number = 0;
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_PROW);
    do_extended_set_key(extended, 0, &document_prow_key0);
    do_extended_set_fields_full_record(extended, sizeof(document_prow_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        document_prow_st = (document_prow_struct_t*)do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            document_prow_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if ( !memcmp(&document_prow_st->dtype, &document_prow_new_st->dtype, sizeof(document_prow_st->dtype)) &&
                 !memcmp(&document_prow_st->dtype1, &document_prow_new_st->dtype1, sizeof(document_prow_st->dtype1)) &&
                 document_prow_st->number == document_prow_new_st->number ) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(document_prow_st, document_prow_new_st, size_new)) {
                    do_list_add(do_list_ch, document_prow_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, document_prow_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        document_prow_.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&document_prow_.data, do_list_ins->list[i], document_prow_.size);

        code = do_text(alias, document_prow_.data.dtype);
        value = do_text(alias, document_prow_.data.dtype1);
        do_log(LOG_INFO, "insert operation prow \"%s.%s.%d\"", code, value, document_prow_.data.number);
        do_free(code);
        do_free(value);

        status = do_document_prow_insert(alias, &document_prow_);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        document_prow_st = do_list_ch->list[i];
        memcpy(&document_prow_key0.dtype,  &document_prow_st->dtype,  sizeof(document_prow_key0.dtype));
        memcpy(&document_prow_key0.dtype1,  &document_prow_st->dtype1,  sizeof(document_prow_key0.dtype1));
        document_prow_key0.number = document_prow_st->number;
        status = do_document_prow_get0(alias, &document_prow_, &document_prow_key0, DO_GET_EQUAL);

        code = do_text(alias, document_prow_key0.dtype);
        value = do_text(alias, document_prow_key0.dtype1);
        do_log(LOG_INFO, "update operation prow \"%s.%s.%d\"", code, value, document_prow_key0.number);
        do_free(code);
        do_free(value);

        status = do_document_prow_get0(alias, &document_prow_, &document_prow_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            document_prow_.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&document_prow_.data, do_list_ch->list[i], document_prow_.size);
            status = do_document_prow_update(alias, &document_prow_);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        document_prow_st = do_list_del->list[i];
        do_cpy(document_prow_key0.dtype,  document_prow_st->dtype);
        do_cpy(document_prow_key0.dtype1,  document_prow_st->dtype1);
        document_prow_key0.number = document_prow_st->number;

        code = do_text(alias, document_prow_key0.dtype);
        value = do_text(alias, document_prow_key0.dtype1);
        do_log(LOG_INFO, "delete operation prow \"%s.%s.%d\"", code, value, document_prow_key0.number);
        do_free(code);
        do_free(value);

        status = do_document_prow_get0(alias, &document_prow_, &document_prow_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            status = do_document_prow_delete(alias);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}
int replic_users(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *value;
    void          *crnt;
    do_size_t   *sz;
    int            status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    user_key0_t    user_key0;
    user_rec_t     user_;
    user_struct_t *user_st;
    user_struct_t *user_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz >= sizeof(user_struct_t) - sizeof(user_.data.params) &&
                     *sz <= sizeof(user_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    user_key0.type = ' ';
    do_text_set_empty(user_key0.name);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_USER);
    do_extended_set_key(extended, 0, &user_key0);
    do_extended_set_fields_full_record(extended, sizeof(user_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        user_st = (user_struct_t*)do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            user_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if ( !memcmp(&user_st->type, &user_new_st->type, sizeof(user_st->type)) &&
                 !memcmp(&user_st->name, &user_new_st->name, sizeof(user_st->name))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(user_st, user_new_st, size_new)) {
                    do_list_add(do_list_ch, user_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, user_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        user_.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&user_.data, do_list_ins->list[i], user_.size);

        value = do_text(alias, user_.data.name);
        do_log(LOG_INFO, "insert user \"%c.%s\"", user_.data.type, value);
        do_free(value);

        status = do_user_insert(alias, &user_);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        user_st = do_list_ch->list[i];
        memcpy(&user_key0.type,  &user_st->type,  sizeof(user_key0.type));
        memcpy(&user_key0.name,  &user_st->name,  sizeof(user_key0.name));
        status = do_user_get0(alias, &user_, &user_key0, DO_GET_EQUAL);

        value = do_text(alias, user_key0.name);
        do_log(LOG_INFO, "update user \"%c.%s\"", user_key0.type, value);
        do_free(value);

        status = do_user_get0(alias, &user_, &user_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            user_.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&user_.data, do_list_ch->list[i], user_.size);
            status = do_user_update(alias, &user_);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        user_st = do_list_del->list[i];
        do_cpy(user_key0.type,  user_st->type);
        do_cpy(user_key0.name,  user_st->name);

        value = do_text(alias, user_key0.name);
        do_log(LOG_INFO, "delete user \"%c.%s\"", user_key0.type, value);
        do_free(value);

        status = do_user_get0(alias, &user_, &user_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            status = do_user_delete(alias);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}
int replic_sklads(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    sklad_key0_t    sklad_key0;
    sklad_key0_t    sklad_key_local;
    sklad_rec_t     sklad_;
    sklad_struct_t *sklad_st;
    sklad_struct_t *sklad_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz >= sizeof(sklad_struct_t) - sizeof(sklad_.data.params) &&
                     *sz <= sizeof(sklad_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }
    do_text_set(alias, sklad_key_local.code, domino_unit());
    do_text_set_empty(sklad_key0.code);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_SKLAD);
    do_extended_set_key(extended, 0, &sklad_key0);
    do_extended_set_fields_full_record(extended, sizeof(sklad_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        sklad_st = (sklad_struct_t*)do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            sklad_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if (!memcmp(&sklad_st->code, &sklad_new_st->code, sizeof(sklad_st->code))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(sklad_st, sklad_new_st, size_new)) {
                    do_list_add(do_list_ch, sklad_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, sklad_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        sklad_.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&sklad_.data, do_list_ins->list[i], sklad_.size);

        code = do_text(alias, sklad_.data.code);
        do_log(LOG_INFO, "insert sklad \"%s\"", code);
        do_free(code);

        status = do_sklad_insert(alias, &sklad_);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        sklad_st = do_list_ch->list[i];
        memcpy(&sklad_key0.code,  &sklad_st->code,  sizeof(sklad_key0.code));
        status = do_sklad_get0(alias, &sklad_, &sklad_key0, DO_GET_EQUAL);

        code = do_text(alias, sklad_key0.code);
        do_log(LOG_INFO, "update sklad \"%s\"", code);
        do_free(code);

        status = do_sklad_get0(alias, &sklad_, &sklad_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            BTI_LONG reporting_period_date;
            if ( !do_cmp(sklad_.data.code,sklad_key_local.code) )
                reporting_period_date = sklad_.data.reporting_period_date;
            sklad_.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&sklad_.data, do_list_ch->list[i], sklad_.size);
            if ( !do_cmp(sklad_.data.code,sklad_key_local.code) )
                sklad_.data.reporting_period_date = reporting_period_date;
            status = do_sklad_update(alias, &sklad_);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        sklad_st = do_list_del->list[i];
        memcpy(&sklad_key0.code,  &sklad_st->code,  sizeof(sklad_key0.code));

        code = do_text(alias, sklad_key0.code);
        do_log(LOG_INFO, "delete sklad \"%s\"", code);
        do_free(code);

        status = do_sklad_get0(alias, &sklad_, &sklad_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            status = do_sklad_delete(alias);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}

int replic_otdels(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    otdel_key0_t    otdel_key0;
    otdel_rec_t     otdel_;
    otdel_struct_t *otdel_st;
    otdel_struct_t *otdel_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz >= sizeof(otdel_struct_t) - sizeof(otdel_.data.params) &&
                     *sz <= sizeof(otdel_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    do_text_set_empty(otdel_key0.code);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_OTDEL);
    do_extended_set_key(extended, 0, &otdel_key0);
    do_extended_set_fields_full_record(extended, sizeof(otdel_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        otdel_st = (otdel_struct_t*)do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            otdel_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if (!memcmp(&otdel_st->code, &otdel_new_st->code, sizeof(otdel_st->code))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(otdel_st, otdel_new_st, size_new)) {
                    do_list_add(do_list_ch, otdel_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, otdel_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        otdel_.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&otdel_.data, do_list_ins->list[i], otdel_.size);

        code = do_text(alias, otdel_.data.code);
        do_log(LOG_INFO, "insert otdel \"%s\"", code);
        do_free(code);

        status = do_otdel_insert(alias, &otdel_);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        otdel_st = do_list_ch->list[i];
        memcpy(&otdel_key0.code,  &otdel_st->code,  sizeof(otdel_key0.code));
        status = do_otdel_get0(alias, &otdel_, &otdel_key0, DO_GET_EQUAL);

        code = do_text(alias, otdel_key0.code);
        do_log(LOG_INFO, "update otdel \"%s\"", code);
        do_free(code);

        status = do_otdel_get0(alias, &otdel_, &otdel_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            otdel_.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&otdel_.data, do_list_ch->list[i], otdel_.size);
            status = do_otdel_update(alias, &otdel_);
        }
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        otdel_st = do_list_del->list[i];
        memcpy(&otdel_key0.code,  &otdel_st->code,  sizeof(otdel_key0.code));

        code = do_text(alias, otdel_key0.code);
        do_log(LOG_INFO, "delete otdel \"%s\"", code);
        do_free(code);

        status = do_otdel_get0(alias, &otdel_, &otdel_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            status = do_otdel_delete(alias);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}


static int document_equal(void *d1, void *d2)
{
    document_struct_t *doc1 = d1, *doc2 = d2;
    return !memcmp(&doc1->dtype, &doc2->dtype, sizeof(doc2->dtype)) &&
           !memcmp(&doc1->sklad, &doc2->sklad, sizeof(doc2->sklad)) &&
           !memcmp(&doc1->document, &doc2->document, sizeof(doc2->document));
}
static do_ret_list_t *get_documents(do_alias_t *alias,
                     char *dtype, char *sklad, int date_start, int date_end, int all,
                     do_extended_break_func break_func)
{
    do_ret_list_t  *list;
    document_key3_t document_key3;
    do_extended_t  *extended;

    do_text_set(alias, document_key3.dtype, dtype);
    do_text_set(alias, document_key3.sklad, sklad);
    if (!all) {
        document_key3.date = date_start;
        document_key3.time = 0;
    }
    else {
        document_key3.date = INT_MIN;
        document_key3.time = 0;
    }

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT);

    do_extended_set_key(extended, 3, &document_key3);
    do_extended_set_filter(extended, TRUE, (all) ? 2 : 4,
                            sizeof(document_key3.dtype),sizeof(document_key3.sklad),
                            sizeof(document_key3.date),sizeof(document_key3.date));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_struct_t, dtype),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_key3.dtype);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                (all) ?
                                  DO_EXTENDED_CONNECTION_NO :
                                  DO_EXTENDED_CONNECTION_AND,
                                &document_key3.sklad);
    if (!all) {
        document_key3.date = date_start;
        do_extended_set_filter_term(extended, 2,
                                    DO_EXTENDED_TYPE_INTEGER,
                                    offsetof(document_struct_t, date),
                                    DO_EXTENDED_COMP_GE,
                                    DO_EXTENDED_CONNECTION_AND,
                                    &document_key3.date);
        document_key3.date = date_end;
        do_extended_set_filter_term(extended, 3,
                                    DO_EXTENDED_TYPE_INTEGER,
                                    offsetof(document_struct_t, date),
                                    DO_EXTENDED_COMP_LE,
                                    DO_EXTENDED_CONNECTION_NO,
                                    &document_key3.date);
        document_key3.date = date_start;
    }

    do_extended_set_fields_full_record(extended, sizeof(document_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    return list;
}

static do_ret_list_t **get_document_content(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{

    do_ret_list_t **l_content;
    l_content = (do_ret_list_t**)do_malloc0(sizeof(*l_content)*DOCUMENT_CONTEXT_COUNT);
    int err = 0, j;

    for (j =0; !err && j < DOCUMENT_CONTEXT_COUNT; j++) {
        if (!RUNNING_) {
            err = 1;
            break;
        }
        switch (j) {
            case 0:
                l_content[j] = replic_select_document_view(alias, document_key0, break_func);
                break;
            case 1:
                l_content[j] = replic_select_document_data(alias, document_key0, break_func);
                break;
            case 2:
                l_content[j] = replic_select_document_order(alias, document_key0, break_func);
                break;
            case 3:
                l_content[j] = replic_select_document_link(alias, document_key0, break_func);
                break;
            case 4:
                l_content[j] = replic_select_document_prowod(alias, document_key0, break_func);
                break;
            case 5:
                l_content[j] = replic_select_document_prowod2(alias, document_key0, break_func);
                break;
            case 6:
                l_content[j] = replic_select_document_parcel(alias, document_key0, break_func);
                break;
            case 7:
                l_content[j] = replic_select_document_protocol(alias, document_key0, break_func);
                break;
        }
        if (!l_content[j]) {
            err = TRUE;
            break;
        }
    }
    if (err) {
        for (j = 0; j < DOCUMENT_CONTEXT_COUNT; j++)
            if (l_content[j])
                do_ret_list_free(l_content[j]);
        do_free(l_content);
        l_content = NULL;
    }
    return l_content;
}
static do_list_t **get_document_content_from_data(void *data, size_t size, void **ptr)
{
    void *crnt;
    crnt = *ptr;
    int count;
    do_size_t *sz;
    do_list_t **l_content;
    int err = 0, i, j;

    l_content = (do_list_t**)do_malloc0(sizeof(*l_content)*DOCUMENT_CONTEXT_COUNT);

    for (j = 0; j < DOCUMENT_CONTEXT_COUNT; j++)
        l_content[j] = do_list_new(0);

    if (!size)
        return l_content;

    for (j = 0; !err && j < DOCUMENT_CONTEXT_COUNT; j++) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert_break(crnt <= data + size);
        debug_assert_break(*sz >= 0);

        count = *sz;
        for (i = 0; !err && i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert_break(crnt <= data + size);
            switch (j) {
                case 0:
                    debug_assert(*sz <= sizeof(document_view_struct_t));
                    break;
                case 1:
                    debug_assert(*sz <= sizeof(document_data_struct_t));
                    break;
                case 2:
                    debug_assert(*sz <= sizeof(document_order_struct_t));
                    break;
                case 3:
                    debug_assert(*sz <= sizeof(document_link_struct_t));
                    break;
                case 4:
                    debug_assert(*sz <= sizeof(prowod_struct_t));
                    break;
                case 5:
                    debug_assert(*sz <= sizeof(prowod2_struct_t));
                    break;
                case 6:
                    debug_assert(*sz <= sizeof(product_struct_t));
                    break;
                case 7:
                    debug_assert(*sz <= sizeof(protocol_struct_t));
                    break;
            }
            if ( err ) break;
            if (! err )
                do_list_add(l_content[j], crnt);
            crnt += *sz;
            debug_assert_break(crnt <= data + size);
        }
    }
    *ptr = crnt;
    if (err) {
        for (j = 0; j < DOCUMENT_CONTEXT_COUNT; j++)
            if (l_content[j])
                do_list_free(l_content[j]);
        do_free(l_content);
        l_content = NULL;
    }
    return l_content;
}
static int document_insert(do_alias_t *alias, document_struct_t *document_st, size_t size)
{
    BTI_WORD len;
    len = size;
    char *dtype, *sklad, *number;
    int status;
    dtype = do_text(alias, document_st->dtype);
    sklad = do_text(alias, document_st->sklad);
    number = do_text(alias, document_st->document);
    do_log(LOG_INFO, "insert document \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    status = do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT, document_st, &len, NULL, -1);
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    return status == DO_OK;

}
static int delete_document_view(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    document_view_key0_t document_view_key0;
    document_view_rec_t document_view;
    int status;
    memcpy(&document_view_key0.dtype, &document_key0->dtype, sizeof(document_view_key0.dtype));
    memcpy(&document_view_key0.sklad, &document_key0->sklad, sizeof(document_view_key0.sklad));
    memcpy(&document_view_key0.document, &document_key0->document, sizeof(document_view_key0.document));
	document_view_key0.type = 0;
	do_text_set(alias, document_view_key0.sort, "");

	status = do_document_view_get0(alias, &document_view, &document_view_key0, DO_GET_GE);
	while (status == DO_OK) {
	    if (!RUNNING_) {
	        status = DO_ERROR;
	        break;
	    }
	    if (
	        memcmp(&document_view_key0.dtype, &document_key0->dtype, sizeof(document_view_key0.dtype)) ||
            memcmp(&document_view_key0.sklad, &document_key0->sklad, sizeof(document_view_key0.sklad)) ||
            memcmp(&document_view_key0.document, &document_key0->document, sizeof(document_view_key0.document))
           )
           break;

        status = do_document_view_delete(alias);
        if (status != DO_OK)
            break;
        status = do_document_view_get0(alias, &document_view, &document_view_key0, DO_GET_NEXT);
	}
	return status != DO_ERROR;
}
static int delete_document_data(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    document_data_key0_t document_data_key0;
    document_data_rec_t document_data;
    int status;
    memcpy(&document_data_key0.dtype, &document_key0->dtype, sizeof(document_data_key0.dtype));
    memcpy(&document_data_key0.sklad, &document_key0->sklad, sizeof(document_data_key0.sklad));
    memcpy(&document_data_key0.document, &document_key0->document, sizeof(document_data_key0.document));
	document_data_key0.type = 0;
	document_data_key0.number = 0;

	status = do_document_data_get0(alias, &document_data, &document_data_key0, DO_GET_GE);
	while (status == DO_OK) {
	    if (!RUNNING_) {
	        status = DO_ERROR;
	        break;
	    }
	    if (
	        memcmp(&document_data_key0.dtype, &document_key0->dtype, sizeof(document_data_key0.dtype)) ||
            memcmp(&document_data_key0.sklad, &document_key0->sklad, sizeof(document_data_key0.sklad)) ||
            memcmp(&document_data_key0.document, &document_key0->document, sizeof(document_data_key0.document))
           )
           break;

        status = do_document_data_delete(alias);
        if (status != DO_OK)
            break;
        status = do_document_data_get0(alias, &document_data, &document_data_key0, DO_GET_NEXT);
	}
	return status != DO_ERROR;
}
static int delete_document_link(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    document_link_key0_t document_link_key0;
    document_link_rec_t document_link;
    int status;
    memcpy(&document_link_key0.m_dtype, &document_key0->dtype, sizeof(document_link_key0.m_dtype));
    memcpy(&document_link_key0.m_sklad, &document_key0->sklad, sizeof(document_link_key0.m_sklad));
    memcpy(&document_link_key0.m_document, &document_key0->document, sizeof(document_link_key0.m_document));
	document_link_key0.type = 0;
	document_link_key0.sort = 0;

	status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_GE);
	while (status == DO_OK) {
	    if (!RUNNING_) {
	        status = DO_ERROR;
	        break;
	    }
	    if (
	        memcmp(&document_link_key0.m_dtype, &document_key0->dtype, sizeof(document_link_key0.m_dtype)) ||
            memcmp(&document_link_key0.m_sklad, &document_key0->sklad, sizeof(document_link_key0.m_sklad)) ||
            memcmp(&document_link_key0.m_document, &document_key0->document, sizeof(document_link_key0.m_document))
           )
           break;

        status = do_document_link_delete(alias);
        if (status != DO_OK)
            break;
        status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_NEXT);
	}
	return status != DO_ERROR;
}
static int delete_document_order(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    int status;
    memcpy(&document_order_key0.dtype, &document_key0->dtype, sizeof(document_order_key0.dtype));
    memcpy(&document_order_key0.sklad, &document_key0->sklad, sizeof(document_order_key0.sklad));
    memcpy(&document_order_key0.document, &document_key0->document, sizeof(document_order_key0.document));
	document_order_key0.line = 0;

	status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
	while (status == DO_OK) {
	    if (!RUNNING_) {
	        status = DO_ERROR;
	        break;
	    }
	    if (
	        memcmp(&document_order_key0.dtype, &document_key0->dtype, sizeof(document_order_key0.dtype)) ||
            memcmp(&document_order_key0.sklad, &document_key0->sklad, sizeof(document_order_key0.sklad)) ||
            memcmp(&document_order_key0.document, &document_key0->document, sizeof(document_order_key0.document))
           )
           break;

        status = do_document_order_delete(alias);
        if (status != DO_OK)
            break;
        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
	}
	return status != DO_ERROR;
}
static int delete_document_prowod2(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    prowod2_key0_t prowod2_key0;
    prowod2_rec_t prowod2;
    int status;
    memcpy(&prowod2_key0.dtype, &document_key0->dtype, sizeof(prowod2_key0.dtype));
    memcpy(&prowod2_key0.sklad, &document_key0->sklad, sizeof(prowod2_key0.sklad));
    memcpy(&prowod2_key0.document, &document_key0->document, sizeof(prowod2_key0.document));
    prowod2_key0.number = 0;
    do_text_set_empty(prowod2_key0.otdel);

	status = do_prowod2_get0(alias, &prowod2, &prowod2_key0, DO_GET_GE);
	while (status == DO_OK) {
	    if (!RUNNING_) {
	        status = DO_ERROR;
	        break;
	    }
	    if (
	        memcmp(&prowod2_key0.dtype, &document_key0->dtype, sizeof(prowod2_key0.dtype)) ||
            memcmp(&prowod2_key0.sklad, &document_key0->sklad, sizeof(prowod2_key0.sklad)) ||
            memcmp(&prowod2_key0.document, &document_key0->document, sizeof(prowod2_key0.document))
           )
           break;

        status = do_prowod2_delete(alias);
        if (status != DO_OK)
            break;
        status = do_prowod2_get0(alias, &prowod2, &prowod2_key0, DO_GET_NEXT);
	}
	return status != DO_ERROR;
}
static int delete_document_prowod(do_alias_t *alias, document_key0_t *document_key0, do_sort_list_t *accounts, do_extended_break_func break_func)
{
    prowod_key4_t prowod_key4, prowod_key;
    prowod_rec_t prowod;
    int status, err = 0;
    char *prowodkey, *param_doc;
    char *dtype, *sklad, *document;
    dtype = do_text(alias, document_key0->dtype);
    sklad = do_text(alias, document_key0->sklad);
    document = do_text(alias, document_key0->document);
    prowodkey = (char*)do_malloc(strlen(dtype) + strlen(sklad) + strlen(document) + 3 + 1);
    sprintf(prowodkey, "@%s.%s/%s", document, dtype, sklad);

    do_text_set(alias, prowod_key4.division, domino_division());
    do_text_set(alias, prowod_key4.document, prowodkey);
    prowod_key4.date = INT_MIN;
    memcpy(&prowod_key, &prowod_key4, sizeof(prowod_key4.division) + sizeof(prowod_key4.document));

	status = do_prowod_get4(alias, &prowod, &prowod_key4, DO_GET_GE);
	while (status == DO_OK) {
	    if (!RUNNING_) {
	        status = DO_ERROR;
	        break;
	    }
	    if (
            memcmp(&prowod_key, &prowod_key4, sizeof(prowod_key4.division) + sizeof(prowod_key4.document))
           )
           break;

        param_doc = do_prowod_param(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_DOCUMENT));
        if (!strcmp(param_doc, prowodkey)) {
            do_balance_key_t key;
            if (!err && do_balance_get_key_from_prowod(alias, &prowod.data, &key, DO_BALANCE_SIDE_DEBIT, &err))
                do_sort_list_add_alloc(accounts, &key, sizeof(key));
            if (!err && do_balance_get_key_from_prowod(alias, &prowod.data, &key, DO_BALANCE_SIDE_CREDIT, &err))
                do_sort_list_add_alloc(accounts, &key, sizeof(key));
            if (err)
                break;
            status = do_prowod_delete(alias);
            if (status != DO_OK) {
                do_free(param_doc);
                break;
            }
        }
        do_free(param_doc);


        status = do_prowod_get4(alias, &prowod, &prowod_key4, DO_GET_NEXT);
	}
	return status != DO_ERROR && !err;
}

static int document_delete(do_alias_t *alias, document_key0_t *document_key0, do_sort_list_t *accounts, do_extended_break_func break_func)
{
    int status;
    char *dtype, *sklad, *number;

    dtype = do_text(alias, document_key0->dtype);
    sklad = do_text(alias, document_key0->sklad);
    number = do_text(alias, document_key0->document);
    do_log(LOG_INFO, "delete document \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    document_rec_t document;

    status = do_document_get0(alias, &document, document_key0, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return FALSE;
    if (status != DO_KEY_NOT_FOUND)
        status = do_document_delete(alias);
    else
        status = DO_OK;
    if (status != DO_OK)
        return FALSE;
    return
        delete_document_view(alias, document_key0, break_func) &&
        delete_document_data(alias, document_key0, break_func) &&
        delete_document_order(alias, document_key0, break_func) &&
        delete_document_link(alias, document_key0, break_func) &&
        delete_document_prowod(alias, document_key0, accounts, break_func) &&
        delete_document_prowod2(alias, document_key0, break_func) ;
}
static int document_delete_(do_alias_t *alias, document_key0_t *document_key0, do_sort_list_t *accounts, do_extended_break_func break_func)
{
    int status;
    char *dtype, *sklad, *number;

    dtype = do_text(alias, document_key0->dtype);
    sklad = do_text(alias, document_key0->sklad);
    number = do_text(alias, document_key0->document);
    do_log(LOG_INFO, "delete document \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);

    status = do_document_delete(alias);
    if (status != DO_OK)
        return FALSE;
    return
        delete_document_view(alias, document_key0, break_func) &&
        delete_document_data(alias, document_key0, break_func) &&
        delete_document_order(alias, document_key0, break_func) &&
        delete_document_link(alias, document_key0, break_func) &&
        delete_document_prowod(alias, document_key0, accounts, break_func) &&
        delete_document_prowod2(alias, document_key0, break_func) ;
}
static int document_change(do_alias_t *alias, document_struct_t *document_st, size_t size)
{
    BTI_WORD len;
    len = size;
    int status;
    char *dtype, *sklad, *number;
    dtype = do_text(alias, document_st->dtype);
    sklad = do_text(alias, document_st->sklad);
    number = do_text(alias, document_st->document);
    do_log(LOG_INFO, "change document \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    document_key0_t document_key0;
    document_rec_t document;
    memcpy(&document_key0.dtype, document_st->dtype, sizeof(document_st->dtype));
    memcpy(&document_key0.sklad, document_st->sklad, sizeof(document_st->sklad));
    memcpy(&document_key0.document, document_st->document, sizeof(document_st->document));

    status = do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL);
    if (status != DO_OK)
        return FALSE;
    status = do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT, document_st, &len, NULL, -1);
    if ( status != DO_OK )
        return FALSE;
    return TRUE;
}

// views
static int document_view_equal(document_view_struct_t *d1, document_view_struct_t *d2)
{
    return d1->type == d2->type &&
           !memcmp(&d1->sort, &d2->sort, sizeof(d1->sort)) &&
           !memcmp(&d1->dtype, &d2->dtype, sizeof(d1->dtype)) &&
           !memcmp(&d1->sklad, &d2->sklad, sizeof(d1->sklad)) &&
           !memcmp(&d1->document, &d2->document, sizeof(d1->document));
}
static int document_view_insert(do_alias_t *alias, document_view_struct_t *document_view_st, size_t size)
{
    BTI_WORD len;
    len = size;
    char *dtype, *sklad, *number;
    dtype = do_text(alias, document_view_st->dtype);
    sklad = do_text(alias, document_view_st->sklad);
    number = do_text(alias, document_view_st->document);
    do_log(LOG_INFO, "insert document_view \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    return do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT_VIEW, document_view_st, &len, NULL, -1) == DO_OK;

}
static int document_view_delete(do_alias_t *alias, document_view_key0_t *document_view_key0)
{
    int status;
    char *dtype, *sklad, *number;

    dtype = do_text(alias, document_view_key0->dtype);
    sklad = do_text(alias, document_view_key0->sklad);
    number = do_text(alias, document_view_key0->document);
    do_log(LOG_INFO, "delete document_view \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);

    document_view_rec_t document_view;
    status = do_document_view_get0(alias, &document_view, document_view_key0, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return FALSE;
    if (status != DO_KEY_NOT_FOUND)
        status = do_document_view_delete(alias);
    else
        status = DO_OK;
    if (status != DO_OK)
        return FALSE;
    return TRUE;
}
static int document_view_change(do_alias_t *alias, document_view_struct_t *document_view_st, size_t size)
{
    BTI_WORD len;
    len = size;
    int status;
    char *dtype, *sklad, *number;
    dtype = do_text(alias, document_view_st->dtype);
    sklad = do_text(alias, document_view_st->sklad);
    number = do_text(alias, document_view_st->document);
    do_log(LOG_INFO, "change document_view \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    document_view_key0_t document_view_key0;
    document_view_rec_t document_view;
    memcpy(&document_view_key0.type, &document_view_st->type, sizeof(document_view_st->type));
    memcpy(&document_view_key0.sort, &document_view_st->sort, sizeof(document_view_st->sort));
    memcpy(&document_view_key0.dtype, &document_view_st->dtype, sizeof(document_view_st->dtype));
    memcpy(&document_view_key0.sklad, &document_view_st->sklad, sizeof(document_view_st->sklad));
    memcpy(&document_view_key0.document, &document_view_st->document, sizeof(document_view_st->document));

    status = do_document_view_get0(alias, &document_view, &document_view_key0, DO_GET_EQUAL);
    if (status != DO_OK)
        return FALSE;
    return do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT_VIEW, document_view_st, &len, NULL, -1) == DO_OK;
}

static int replic_document_view(do_alias_t *alias, do_list_t *do_list_new, do_ret_list_t *list, int *changed, do_extended_break_func break_func)
{
    document_view_struct_t *document_view_st, *document_view_st_new;
    document_view_key0_t document_view_key0;

    size_t document_view_size, document_view_size_new;
    int i, j;
    for (i = 0; i < do_list_new->count; i++) {
        if (!RUNNING_)
            return FALSE;

        document_view_st_new = (document_view_struct_t*)do_list_new->list[i];
        document_view_size_new = *((do_size_t*)(do_list_new->list[i] - sizeof(do_size_t)));
        document_view_size = 0;
        for (j = 0; j < do_ret_list_count(list); j++)
            if (document_view_equal(document_view_st_new, do_ret_list_item_data(list, j))) {
                document_view_size = do_ret_list_item_len(list, j);
                document_view_st = do_ret_list_item_data(list, j);
                do_ret_list_delete(list, j);
                break;
            }
        if (!document_view_size) {
            if (!document_view_insert(alias, document_view_st_new, document_view_size_new))
                return FALSE;
            *changed = TRUE;

        }
        else
            if (document_view_size != document_view_size_new ||
                memcmp(document_view_st, document_view_st_new, document_view_size)) {
                if (!document_view_change(alias, document_view_st_new, document_view_size_new))
                    return FALSE;
                *changed = TRUE;
            }
    }
    for (j = 0; j < do_ret_list_count(list); j++) {
        document_view_st = do_ret_list_item_data(list, j);
        do_cpy(document_view_key0.type, document_view_st->type);
        do_cpy(document_view_key0.sort, document_view_st->sort);
        do_cpy(document_view_key0.dtype, document_view_st->dtype);
        do_cpy(document_view_key0.sklad, document_view_st->sklad);
        do_cpy(document_view_key0.document, document_view_st->document);
        if (!document_view_delete(alias, &document_view_key0))
            return FALSE;
        *changed = TRUE;
    }
    return TRUE;
}

//data

static int document_data_equal(document_data_struct_t *d1, document_data_struct_t *d2)
{
    return d1->type == d2->type &&
           !memcmp(&d1->number, &d2->number, sizeof(d1->number)) &&
           !memcmp(&d1->dtype, &d2->dtype, sizeof(d1->dtype)) &&
           !memcmp(&d1->sklad, &d2->sklad, sizeof(d1->sklad)) &&
           !memcmp(&d1->document, &d2->document, sizeof(d1->document));
}
static int document_data_insert(do_alias_t *alias, document_data_struct_t *document_data_st, size_t size)
{
    BTI_WORD len;
    len = size;
    char *dtype, *sklad, *number;
    dtype = do_text(alias, document_data_st->dtype);
    sklad = do_text(alias, document_data_st->sklad);
    number = do_text(alias, document_data_st->document);
    do_log(LOG_INFO, "insert document_data \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    return do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT_DATA, document_data_st, &len, NULL, -1) == DO_OK;

}
static int document_data_delete(do_alias_t *alias, document_data_key0_t *document_data_key0)
{
    int status;
    char *dtype, *sklad, *number;
    document_data_rec_t document_data;

    dtype = do_text(alias, document_data_key0->dtype);
    sklad = do_text(alias, document_data_key0->sklad);
    number = do_text(alias, document_data_key0->document);
    do_log(LOG_INFO, "delete document_data \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);

    status = do_document_data_get0(alias, &document_data, document_data_key0, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return FALSE;
    if (status != DO_KEY_NOT_FOUND)
        status = do_document_data_delete(alias);
    else
        status = DO_OK;
    if (status != DO_OK)
        return FALSE;
    return TRUE;
}
static int document_data_change(do_alias_t *alias, document_data_struct_t *document_data_st, size_t size)
{
    BTI_WORD len;
    len = size;
    int status;
    char *dtype, *sklad, *number;
    dtype = do_text(alias, document_data_st->dtype);
    sklad = do_text(alias, document_data_st->sklad);
    number = do_text(alias, document_data_st->document);
    do_log(LOG_INFO, "change document_data \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    document_data_key0_t document_data_key0;
    document_data_rec_t document_data;
    memcpy(&document_data_key0.type, &document_data_st->type, sizeof(document_data_st->type));
    memcpy(&document_data_key0.number, &document_data_st->number, sizeof(document_data_st->number));
    memcpy(&document_data_key0.dtype, &document_data_st->dtype, sizeof(document_data_st->dtype));
    memcpy(&document_data_key0.sklad, &document_data_st->sklad, sizeof(document_data_st->sklad));
    memcpy(&document_data_key0.document, &document_data_st->document, sizeof(document_data_st->document));

    status = do_document_data_get0(alias, &document_data, &document_data_key0, DO_GET_EQUAL);
    if (status != DO_OK)
        return FALSE;
    return do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT_DATA, document_data_st, &len, NULL, -1) == DO_OK;
}

static int replic_document_data(do_alias_t *alias, do_list_t *do_list_new, do_ret_list_t *list, int *changed, do_extended_break_func break_func)
{
    document_data_struct_t *document_data_st, *document_data_st_new;
    document_data_key0_t document_data_key0;

    size_t document_data_size, document_data_size_new;
    int i, j;
    for (i = 0; i < do_list_new->count; i++) {
        if (!RUNNING_)
            return FALSE;

        document_data_st_new = (document_data_struct_t*)do_list_new->list[i];
        document_data_size_new = *((do_size_t*)(do_list_new->list[i] - sizeof(do_size_t)));
        document_data_size = 0;
        for (j = 0; j < do_ret_list_count(list); j++)
            if (document_data_equal(document_data_st_new, do_ret_list_item_data(list, j))) {
                document_data_size = do_ret_list_item_len(list, j);
                document_data_st = do_ret_list_item_data(list, j);
                do_ret_list_delete(list, j);
                break;
            }
        if (!document_data_size) {
            if (!document_data_insert(alias, document_data_st_new, document_data_size_new))
                return FALSE;
            *changed = TRUE;
        }
        else
            if (document_data_size != document_data_size_new ||
                memcmp(document_data_st, document_data_st_new, document_data_size)) {
                if (!document_data_change(alias, document_data_st_new, document_data_size_new))
                    return FALSE;
                *changed = TRUE;
            }
    }
    for (j = 0; j < do_ret_list_count(list); j++) {
        document_data_st = do_ret_list_item_data(list, j);
        do_cpy(document_data_key0.dtype, document_data_st->dtype);
        do_cpy(document_data_key0.sklad, document_data_st->sklad);
        do_cpy(document_data_key0.document, document_data_st->document);
        do_cpy(document_data_key0.type, document_data_st->type);
        do_cpy(document_data_key0.number, document_data_st->number);
        if (!document_data_delete(alias, &document_data_key0))
            return FALSE;
        *changed = TRUE;
    }

    return TRUE;
}

// link
static int document_link_equal(document_link_struct_t *d1, document_link_struct_t *d2)
{
    return d1->type == d2->type &&
           !memcmp(&d1->sort, &d2->sort, sizeof(d1->sort)) &&
           !memcmp(&d1->m_dtype, &d2->m_dtype, sizeof(d1->m_dtype)) &&
           !memcmp(&d1->m_sklad, &d2->m_sklad, sizeof(d1->m_sklad)) &&
           !memcmp(&d1->m_document, &d2->m_document, sizeof(d1->m_document));
}
static int document_link_insert(do_alias_t *alias, document_link_struct_t *document_link_st, size_t size)
{
    BTI_WORD len;
    len = size;
    char *m_dtype, *m_sklad, *number;
    m_dtype = do_text(alias, document_link_st->m_dtype);
    m_sklad = do_text(alias, document_link_st->m_sklad);
    number = do_text(alias, document_link_st->m_document);
    do_log(LOG_INFO, "insert document_link \"%s.%s.%s\"", m_dtype, m_sklad, number);
    do_free(m_dtype);do_free(m_sklad);do_free(number);
    return do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT_LINK, document_link_st, &len, NULL, -1) == DO_OK;

}
static int document_link_delete(do_alias_t *alias, document_link_key0_t *document_link_key0)
{
    int status;
    char *m_dtype, *m_sklad, *number;

    m_dtype = do_text(alias, document_link_key0->m_dtype);
    m_sklad = do_text(alias, document_link_key0->m_sklad);
    number = do_text(alias, document_link_key0->m_document);
    do_log(LOG_INFO, "delete document_link \"%s.%s.%s\"", m_dtype, m_sklad, number);
    do_free(m_dtype);do_free(m_sklad);do_free(number);
    document_link_rec_t document_link;
    status = do_document_link_get0(alias, &document_link, document_link_key0, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return FALSE;
    if (status != DO_KEY_NOT_FOUND)
        status = do_document_link_delete(alias);
    else
        status = DO_OK;
    if (status != DO_OK)
        return FALSE;
    return TRUE;
}
static int document_link_change(do_alias_t *alias, document_link_struct_t *document_link_st, size_t size)
{
    BTI_WORD len;
    len = size;
    int status;
    char *m_dtype, *m_sklad, *number;
    m_dtype = do_text(alias, document_link_st->m_dtype);
    m_sklad = do_text(alias, document_link_st->m_sklad);
    number = do_text(alias, document_link_st->m_document);
    do_log(LOG_INFO, "change document_link \"%s.%s.%s\"", m_dtype, m_sklad, number);
    do_free(m_dtype);do_free(m_sklad);do_free(number);
    document_link_key0_t document_link_key0;
    document_link_rec_t document_link;
    memcpy(&document_link_key0.type, &document_link_st->type, sizeof(document_link_st->type));
    memcpy(&document_link_key0.sort, &document_link_st->sort, sizeof(document_link_st->sort));
    memcpy(&document_link_key0.m_dtype, &document_link_st->m_dtype, sizeof(document_link_st->m_dtype));
    memcpy(&document_link_key0.m_sklad, &document_link_st->m_sklad, sizeof(document_link_st->m_sklad));
    memcpy(&document_link_key0.m_document, &document_link_st->m_document, sizeof(document_link_st->m_document));

    status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_EQUAL);
    if (status != DO_OK)
        return FALSE;
    return do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT_LINK, document_link_st, &len, NULL, -1) == DO_OK;
}

static int replic_document_link(do_alias_t *alias, do_list_t *do_list_new, do_ret_list_t *list, int *changed, do_extended_break_func break_func)
{
    document_link_struct_t *document_link_st, *document_link_st_new;
    document_link_key0_t document_link_key0;

    size_t document_link_size, document_link_size_new;
    int i, j;
    for (i = 0; i < do_list_new->count; i++) {
        if (!RUNNING_)
            return FALSE;

        document_link_st_new = (document_link_struct_t*)do_list_new->list[i];
        document_link_size_new = *((do_size_t*)(do_list_new->list[i] - sizeof(do_size_t)));
        document_link_size = 0;
        for (j = 0; j < do_ret_list_count(list); j++)
            if (document_link_equal(document_link_st_new, do_ret_list_item_data(list, j))) {
                document_link_size = do_ret_list_item_len(list, j);
                document_link_st = do_ret_list_item_data(list, j);
                do_ret_list_delete(list, j);
                break;
            }
        if (!document_link_size) {
            if (!document_link_insert(alias, document_link_st_new, document_link_size_new))
                return FALSE;
            *changed = TRUE;
        }
        else
            if (document_link_size != document_link_size_new ||
                memcmp(document_link_st, document_link_st_new, document_link_size)) {
                if (!document_link_change(alias, document_link_st_new, document_link_size_new))
                    return FALSE;
                *changed = TRUE;
            }
    }
    for (j = 0; j < do_ret_list_count(list); j++) {
        document_link_st = do_ret_list_item_data(list, j);
        do_cpy(document_link_key0.m_dtype, document_link_st->m_dtype);
        do_cpy(document_link_key0.m_sklad, document_link_st->m_sklad);
        do_cpy(document_link_key0.m_document, document_link_st->m_document);
        do_cpy(document_link_key0.type, document_link_st->type);
        do_cpy(document_link_key0.sort, document_link_st->sort);
        if (!document_link_delete(alias, &document_link_key0))
            return FALSE;
        *changed = TRUE;
    }
    return TRUE;
}

// order

static int document_order_equal(document_order_struct_t *d1, document_order_struct_t *d2)
{
    return d1->line == d2->line &&
           !memcmp(&d1->dtype, &d2->dtype, sizeof(d1->dtype)) &&
           !memcmp(&d1->sklad, &d2->sklad, sizeof(d1->sklad)) &&
           !memcmp(&d1->document, &d2->document, sizeof(d1->document));
}
static int document_order_insert(do_alias_t *alias, document_order_struct_t *document_order_st, size_t size)
{
    BTI_WORD len;
    len = size;
    char *dtype, *sklad, *number;
    dtype = do_text(alias, document_order_st->dtype);
    sklad = do_text(alias, document_order_st->sklad);
    number = do_text(alias, document_order_st->document);
    do_log(LOG_INFO, "insert document_order \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    return do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT_ORDER, document_order_st, &len, NULL, -1) == DO_OK;

}
static int document_order_delete(do_alias_t *alias, document_order_key0_t *document_order_key0)
{
    int status;
    char *dtype, *sklad, *number;
    dtype = do_text(alias, document_order_key0->dtype);
    sklad = do_text(alias, document_order_key0->sklad);
    number = do_text(alias, document_order_key0->document);
    do_log(LOG_INFO, "delete document_order \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    document_order_rec_t document_order;

    status = do_document_order_get0(alias, &document_order, document_order_key0, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return FALSE;
    if (status != DO_KEY_NOT_FOUND)
        status = do_document_order_delete(alias);
    else
        status = DO_OK;
    if (status != DO_OK)
        return FALSE;
    return TRUE;
}
static int document_order_change(do_alias_t *alias, document_order_struct_t *document_order_st, size_t size)
{
    BTI_WORD len;
    len = size;
    int status;
    char *dtype, *sklad, *number;
    dtype = do_text(alias, document_order_st->dtype);
    sklad = do_text(alias, document_order_st->sklad);
    number = do_text(alias, document_order_st->document);
    do_log(LOG_INFO, "change document_order \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    memcpy(&document_order_key0.line, &document_order_st->line, sizeof(document_order_st->line));
    memcpy(&document_order_key0.dtype, &document_order_st->dtype, sizeof(document_order_st->dtype));
    memcpy(&document_order_key0.sklad, &document_order_st->sklad, sizeof(document_order_st->sklad));
    memcpy(&document_order_key0.document, &document_order_st->document, sizeof(document_order_st->document));

    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_EQUAL);
    if (status != DO_OK)
        return FALSE;
    return do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT_ORDER, document_order_st, &len, NULL, -1) == DO_OK;
}

static int replic_document_orders(do_alias_t *alias, do_list_t *do_list_new, do_ret_list_t *list, int *changed, do_extended_break_func break_func)
{
    document_order_struct_t *document_order_st, *document_order_st_new;
    document_order_key0_t document_order_key0;

    size_t document_order_size, document_order_size_new;
    int i, j;
    for (i = 0; i < do_list_new->count; i++) {
        if (!RUNNING_)
            return FALSE;

        document_order_st_new = (document_order_struct_t*)do_list_new->list[i];
        document_order_size_new = *((do_size_t*)(do_list_new->list[i] - sizeof(do_size_t)));
        document_order_size = 0;
        for (j = 0; j < do_ret_list_count(list); j++)
            if (document_order_equal(document_order_st_new, do_ret_list_item_data(list, j))) {
                document_order_size = do_ret_list_item_len(list, j);
                document_order_st = do_ret_list_item_data(list, j);
                do_ret_list_delete(list, j);
                break;
            }
        if (!document_order_size) {
            if (!document_order_insert(alias, document_order_st_new, document_order_size_new))
                return FALSE;
            *changed = TRUE;
        }
        else
            if (document_order_size != document_order_size_new ||
                memcmp(document_order_st, document_order_st_new, document_order_size)) {
                if (!document_order_change(alias, document_order_st_new, document_order_size_new))
                    return FALSE;
                *changed = TRUE;
            }
    }
    for (j = 0; j < do_ret_list_count(list); j++) {
        document_order_st = do_ret_list_item_data(list, j);
        do_cpy(document_order_key0.dtype, document_order_st->dtype);
        do_cpy(document_order_key0.sklad, document_order_st->sklad);
        do_cpy(document_order_key0.document, document_order_st->document);
        do_cpy(document_order_key0.line, document_order_st->line);
        if (!document_order_delete(alias, &document_order_key0))
            return FALSE;
        *changed = TRUE;
    }

    return TRUE;
}
static int replic_document_parcel(do_alias_t *alias, do_list_t *do_list_new, do_ret_list_t *list, do_extended_break_func break_func)
{
    product_key0_t product_key0;
    product_rec_t product;
    product_rec_t product_new;
    int status;
    char *value;
    int i;
    for (i = 0; i < do_list_new->count; i++) {
        if (!RUNNING_)
            return FALSE;

        product_new.size = *((do_size_t*)(do_list_new->list[i] - sizeof(do_size_t)));
        memcpy(&product_new.data, (document_order_struct_t*)do_list_new->list[i], product_new.size);

        do_cpy(product_key0.code, product_new.data.code);
        product_key0.base_parcel = product_new.data.base_parcel;

        status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);

        if (status == DO_ERROR)
            return FALSE;

        if (status == DO_KEY_NOT_FOUND) {
            product_set_l(alias, &product_new.data, NULL);
            value = do_text(alias, product_new.data.code);
            do_log(LOG_INFO, "insert product \"%s\"", value);
            if (!do_protocol_product_add(alias, value, DO_PROTOCOL_ACTION_INSERT))
                status = DO_ERROR;
            else
                status = do_product_insert(alias, &product_new);
            do_free(value);
        }
        else {
            if (!product_is_equal(&product_new.data, product_new.size, &product.data, product.size)) {
                product_set_l(alias, &product_new.data, &product.data);
                value = do_text(alias, product_new.data.code);
                do_log(LOG_INFO, "update product \"%s\"", value);
                if (!do_protocol_product_add(alias, value, DO_PROTOCOL_ACTION_CHANGE))
                    status = DO_ERROR;
                else
                    status = product_update(alias, &product_new, break_func);
                do_free(value);
            }
        }
        if (status == DO_ERROR)
            return FALSE;
    }

    return TRUE;
}

static int replic_document_protocol(do_alias_t *alias, do_list_t *do_list_new, do_ret_list_t *list, do_extended_break_func break_func)
{

    protocol_struct_t *r1, *r2;
    protocol_rec_t protocol;
    protocol_key0_t protocol_key0;
    char *value;
    int status;
    int i, j;
    for (i = 0; i < do_list_new->count; i++) {
        if (!RUNNING_)
            return FALSE;
        r1 = (protocol_struct_t *)do_list_new->list[i];
        for ( j = 0; j < do_ret_list_count(list); j++ ) {
            r2 = do_ret_list_item_data(list, j);
            if ( r2->objectName[0] ) {
                if ( !memcmp(r1, r2, sizeof(protocol_struct_t)) ) {
                    r1->objectName[0] = '\0';
                    r2->objectName[0] = '\0';
                    break;
                }
            }
        }
    }
    for ( j = 0; j < do_ret_list_count(list); j++ ) {
        r2 = do_ret_list_item_data(list, j);
        if ( r2->objectName[0] ) {
            //int flag;
            do_cpy(protocol_key0.objectName, r2->objectName);
            protocol_key0.date = r2->date;
            protocol_key0.time = r2->time;
            status = do_protocol_get0(alias, &protocol, &protocol_key0, DO_GET_EQUAL);
            //flag = FALSE;
            while  ( status == DO_OK ) {
                if ( do_cmp(protocol_key0.objectName, r2->objectName) ||
                    protocol_key0.date != r2->date ||
                    protocol_key0.time != r2->time )  break;
                if ( !do_cmp(protocol.data, *r2) && strncmp(protocol.data.userName, REPLIC_NAME, strlen(REPLIC_NAME)) ) {
                    value = do_text(alias, r2->objectName);
                    do_log(LOG_INFO, "delete protocol %s %d %d", value, r2->date, r2->time);
                    do_free(value);
                    status = do_protocol_delete(alias);
                    if ( status != DO_OK )
                        return FALSE;
                    //flag = TRUE;
                }
                status = do_protocol_get0(alias, &protocol, &protocol_key0, DO_GET_NEXT);
            }
            /*if ( !flag ) { //just in case
                value = do_text(alias, r2->objectName);
                do_log(LOG_ERR, "protocol not found %s %d %d", value, r2->date, r2->time);
                do_free(value);
            }*/
        }
    }
    for (i = 0; i < do_list_new->count; i++) {
        if (!RUNNING_)
            return FALSE;
        r1 = (protocol_struct_t *)do_list_new->list[i];
        if ( r1->objectName[0] ) {
            do_cpy(protocol.data, *r1);
            protocol.size = sizeof(protocol.data);
            value = do_text(alias, r1->objectName);
            do_log(LOG_INFO, "insert protocol %s %d %d", value, r1->date, r1->time);
            do_free(value);
            status = do_protocol_insert(alias, &protocol);
            if ( status != DO_OK )
                return FALSE;
        }
    }


    return TRUE;
}


// prowod2

static int prowod2_equal(prowod2_struct_t *d1, prowod2_struct_t *d2)
{
    return
           !memcmp(&d1->otdel, &d2->otdel, sizeof(d1->otdel)) &&
           !memcmp(&d1->number, &d2->number, sizeof(d1->number)) &&
           !memcmp(&d1->dtype, &d2->dtype, sizeof(d1->dtype)) &&
           !memcmp(&d1->sklad, &d2->sklad, sizeof(d1->sklad)) &&
           !memcmp(&d1->document, &d2->document, sizeof(d1->document));
}
static int prowod2_insert(do_alias_t *alias, prowod2_struct_t *prowod2_st, size_t size)
{
    BTI_WORD len;
    len = size;
    char *dtype, *sklad, *number;
    dtype = do_text(alias, prowod2_st->dtype);
    sklad = do_text(alias, prowod2_st->sklad);
    number = do_text(alias, prowod2_st->document);
    do_log(LOG_INFO, "insert prowod2 \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    return do_alias_btr(alias, B_INSERT, DO_DB_PROWOD2, prowod2_st, &len, NULL, -1) == DO_OK;

}
static int prowod2_delete(do_alias_t *alias, prowod2_key0_t *prowod2_key0)
{
    int status;
    char *dtype, *sklad, *number;

    dtype = do_text(alias, prowod2_key0->dtype);
    sklad = do_text(alias, prowod2_key0->sklad);
    number = do_text(alias, prowod2_key0->document);
    do_log(LOG_INFO, "delete prowod2 \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    prowod2_rec_t prowod2;
    status = do_prowod2_get0(alias, &prowod2, prowod2_key0, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return FALSE;
    if (status != DO_KEY_NOT_FOUND)
        status = do_prowod2_delete(alias);
    else
        status = DO_OK;
    if (status != DO_OK)
        return FALSE;
    return TRUE;
}
static int prowod2_change(do_alias_t *alias, prowod2_struct_t *prowod2_st, size_t size)
{
    BTI_WORD len;
    len = size;
    int status;
    char *dtype, *sklad, *number;
    dtype = do_text(alias, prowod2_st->dtype);
    sklad = do_text(alias, prowod2_st->sklad);
    number = do_text(alias, prowod2_st->document);
    do_log(LOG_INFO, "change prowod2 \"%s.%s.%s\"", dtype, sklad, number);
    do_free(dtype);do_free(sklad);do_free(number);
    prowod2_key0_t prowod2_key0;
    prowod2_rec_t prowod2;
    memcpy(&prowod2_key0.number, &prowod2_st->number, sizeof(prowod2_st->number));
    memcpy(&prowod2_key0.dtype, &prowod2_st->dtype, sizeof(prowod2_st->dtype));
    memcpy(&prowod2_key0.sklad, &prowod2_st->sklad, sizeof(prowod2_st->sklad));
    memcpy(&prowod2_key0.document, &prowod2_st->document, sizeof(prowod2_st->document));
    memcpy(&prowod2_key0.otdel, &prowod2_st->otdel, sizeof(prowod2_st->otdel));

    status = do_prowod2_get0(alias, &prowod2, &prowod2_key0, DO_GET_EQUAL);
    if (status != DO_OK)
        return FALSE;
    return do_alias_btr(alias, B_UPDATE, DO_DB_PROWOD2, prowod2_st, &len, NULL, -1) == DO_OK;
}

static int replic_document_prowod2(do_alias_t *alias, do_list_t *do_list_new, do_ret_list_t *list, int *changed, do_extended_break_func break_func)
{
    prowod2_struct_t *prowod2_st, *prowod2_st_new;
    prowod2_key0_t prowod2_key0;

    size_t prowod2_size, prowod2_size_new;
    int i, j;
    for (i = 0; i < do_list_new->count; i++) {
        if (!RUNNING_)
            return FALSE;

        prowod2_st_new = (prowod2_struct_t*)do_list_new->list[i];
        prowod2_size_new = *((do_size_t*)(do_list_new->list[i] - sizeof(do_size_t)));
        prowod2_size = 0;
        for (j = 0; j < do_ret_list_count(list); j++)
            if (prowod2_equal(prowod2_st_new, (prowod2_struct_t*)do_ret_list_item_data(list, j))) {
                prowod2_size = do_ret_list_item_len(list, j);
                prowod2_st = (prowod2_struct_t*)do_ret_list_item_data(list, j);
                do_ret_list_delete(list, j);
                break;
            }
        if (!prowod2_size) {
            if (!prowod2_insert(alias, prowod2_st_new, prowod2_size_new))
                return FALSE;
            *changed = TRUE;
        }
        else
            if (prowod2_size != prowod2_size_new ||
                memcmp(prowod2_st, prowod2_st_new, prowod2_size)) {
                if (!prowod2_change(alias, prowod2_st_new, prowod2_size_new))
                    return FALSE;
                *changed = TRUE;
            }
    }
    for (j = 0; j < do_ret_list_count(list); j++) {
        prowod2_st = (prowod2_struct_t*)do_ret_list_item_data(list, j);
        do_cpy(prowod2_key0.dtype, prowod2_st->dtype);
        do_cpy(prowod2_key0.sklad, prowod2_st->sklad);
        do_cpy(prowod2_key0.document, prowod2_st->document);
        do_cpy(prowod2_key0.number, prowod2_st->number);
        do_cpy(prowod2_key0.otdel, prowod2_st->otdel);
        if (!prowod2_delete(alias, &prowod2_key0))
            return FALSE;
        *changed = TRUE;
    }

    return TRUE;
}


// prowod
static int prowod_insert(do_alias_t *alias, prowod_struct_t *prowod_st, size_t size, do_sort_list_t *accounts)
{
    BTI_WORD len;
    len = size;
    char *number;
    number = do_text(alias, prowod_st->document);
    do_log(LOG_INFO, "insert prowod \"%s\"", number);
    do_free(number);
    do_balance_key_t key;
    int err = 0;
    if (!err && do_balance_get_key_from_prowod(alias, prowod_st, &key, DO_BALANCE_SIDE_DEBIT, &err))
        do_sort_list_add_alloc(accounts, &key, sizeof(key));
    if (!err && do_balance_get_key_from_prowod(alias, prowod_st, &key, DO_BALANCE_SIDE_CREDIT, &err))
        do_sort_list_add_alloc(accounts, &key, sizeof(key));

    return !err && (do_alias_btr(alias, B_INSERT, DO_DB_PROWOD, prowod_st, &len, NULL, -1) == DO_OK);

}
static int prowod_delete(do_alias_t *alias, int recpos, prowod_struct_t *prowod_st, do_sort_list_t *accounts)
{
    BTI_WORD len;
    BTI_LONG recPos;
    int status;
    char *number;
    prowod_struct_t prowod_;

    number = do_text(alias, prowod_st->document);
    do_log(LOG_INFO, "delete prowod \"%s\"", number);
    do_free(number);

    recPos = recpos;
    memcpy(&prowod_, &recPos, sizeof(recPos));
    do_balance_key_t key;
    int err = 0;
    if (!err && do_balance_get_key_from_prowod(alias, prowod_st, &key, DO_BALANCE_SIDE_DEBIT, &err))
        do_sort_list_add_alloc(accounts, &key, sizeof(key));
    if (!err && do_balance_get_key_from_prowod(alias, prowod_st, &key, DO_BALANCE_SIDE_CREDIT, &err))
        do_sort_list_add_alloc(accounts, &key, sizeof(key));
    if (err)
        return FALSE;
    len = sizeof(prowod_);
    status = do_alias_btr(alias, B_GET_DIRECT, DO_DB_PROWOD, &prowod_, &len, NULL, -1) == DO_OK;
    if (status == DO_ERROR)
        return FALSE;
    if (status != DO_KEY_NOT_FOUND)
        status = do_alias_btr(alias, B_DELETE, DO_DB_PROWOD, prowod_st, &len, NULL, -1);
    else
        status = DO_OK;
    if (status != DO_OK)
        return FALSE;
    return TRUE;
}
static int replic_document_prowod(do_alias_t *alias, do_list_t *do_list_new, do_ret_list_t *list, do_sort_list_t *accounts, int *changed, do_extended_break_func break_func)
{
    prowod_struct_t *prowod_st, *prowod_st_new;

    size_t prowod_size, prowod_size_new;
    int i, j;
    for (i = 0; i < do_list_new->count; i++) {
        if (!RUNNING_)
            return FALSE;

        prowod_st_new = (prowod_struct_t*)do_list_new->list[i];
        prowod_size_new = *((do_size_t*)(do_list_new->list[i] - sizeof(do_size_t)));
        prowod_size = 0;
        for (j = 0; j < do_ret_list_count(list); j++) {
            prowod_size = do_ret_list_item_len(list, j);
            prowod_st = (prowod_struct_t*)do_ret_list_item_data(list, j);
            if (prowod_size == prowod_size_new &&
                !memcmp(prowod_st, prowod_st_new, prowod_size)) {
                do_ret_list_delete(list, j);
                break;
            }
    	    prowod_size = 0;
        }
        if (!prowod_size) {
            if (!prowod_insert(alias, prowod_st_new, prowod_size_new, accounts))
                return FALSE;
            *changed = TRUE;
        }
    }
    for (j = 0; j < do_ret_list_count(list); j++) {
        if (!prowod_delete(alias, do_ret_list_item_pos(list, j), do_ret_list_item_data(list, j), accounts))
            return FALSE;
         *changed = TRUE;
    }

    return TRUE;
}

static int replic_document_content(do_alias_t *alias, do_list_t **do_list_new, do_ret_list_t **list, do_sort_list_t *accounts, int *changed, do_extended_break_func break_func)
{
    return
        replic_document_view(alias, do_list_new[0], list[0], changed, break_func) &&
        replic_document_data(alias, do_list_new[1], list[1], changed, break_func) &&
        replic_document_orders(alias, do_list_new[2], list[2], changed, break_func) &&
        replic_document_link(alias, do_list_new[3], list[3], changed, break_func) &&
        replic_document_prowod(alias, do_list_new[4], list[4], accounts, changed, break_func) &&
        replic_document_prowod2(alias, do_list_new[5], list[5], changed, break_func) &&
        replic_document_parcel(alias, do_list_new[6], list[6], break_func) &&
        replic_document_protocol(alias, do_list_new[7], list[7], break_func);
}
static int balance_key_cmp(do_balance_key_t *s1, do_balance_key_t *s2, void *user_data)
{
    int retval;
    retval = do_cmp(s1->division, s2->division);
    if ( retval ) return retval;
    retval = do_cmp(s1->account, s2->account);
    if ( retval ) return retval;
    retval = do_cmp(s1->subaccount, s2->subaccount);
    if ( retval ) return retval;
    retval = do_cmp(s1->p_g_code, s2->p_g_code);
    if ( retval ) return retval;
    retval = do_cmp(s1->p_code, s2->p_code);
    if ( retval ) return retval;
    retval = do_cmp(s1->other, s2->other);
    return retval;
}

static int replic_documents(do_alias_t *alias, do_ret_list_t *list, int all, void *data, size_t size, do_extended_break_func break_func)
{

    int err = 0;

    document_rec_t document;
    document_key0_t document_key0;
    document_struct_t *document_st = NULL;
    document_struct_t *document_st_new;
    size_t document_size, document_size_new;
    do_list_t **l_content_new;
    do_ret_list_t **l_content;

    void *record;
    int i, j;//, k;
    do_sort_list_t *accounts;
    accounts = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)balance_key_cmp, NULL);

    debug_assert(size >= 0);
    /*!typedef struct PACK_STRUCT {
        do_size_t size;
        document_struct_t document;
    } exchange_t;*/

    if (size > 0) {
        void *crnt;
        int count;
        int changed;
        do_size_t *sz;

        crnt = data;
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        count = *sz;


        for (i = 0; i < count; i++) {
            if (!RUNNING_) {
                err = 1;
                break;
            }
            sz = crnt;
            record = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert((*sz >= sizeof(document_struct_t) - sizeof(document_st->params) &&  *sz <= sizeof(document_struct_t)) ||
                          (*sz == sizeof(document_key0_t)) );
            crnt += *sz;
            debug_assert(crnt <= data + size);

            document_size_new = *((do_size_t*)record);

            if (document_size_new == sizeof(document_key0_t)) {
                memcpy(&document_key0, record + sizeof(do_size_t), sizeof(document_key0_t));
                document_st_new = NULL;
            }
            else {
                document_st_new = (record + sizeof(do_size_t));
                do_cpy(document_key0.dtype, document_st_new->dtype);
                do_cpy(document_key0.sklad, document_st_new->sklad);
                do_cpy(document_key0.document, document_st_new->document);
            }

            l_content = get_document_content(alias, &document_key0, break_func);
            if (!l_content) {
                err = 1;
                break;
            }
            l_content_new = get_document_content_from_data(data, size, &crnt);
            if (!l_content_new) {
                err = 1;
                break;
            }
            if (document_st_new) {
                changed = 0;
                document_size = 0;
                for (j = 0; list && j < do_ret_list_count(list); j++)
                    if (document_equal(document_st_new, do_ret_list_item_data(list, j))) {
                        document_size = do_ret_list_item_len(list, j);
                        document_st = (document_struct_t*)do_ret_list_item_data(list, j);
                        do_ret_list_delete(list, j);
                        break;
                }
                if (!document_size && !all) {
                    switch (do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL)) {
                        case DO_OK:
                            document_size = document.size;
                            document_st   = &document.data;
                            break;
                        case DO_KEY_NOT_FOUND:
                            break;
                        default:
                            err = TRUE;
                    }
                }
                if (!err) {
                    if ( !document_size ) {
                        err = !document_insert(alias, document_st_new, document_size_new);
                        changed = 1;
                    }
                    else {
                        if (document_size != document_size_new ||
                            memcmp(document_st, document_st_new, document_size)) {
                            err = !document_change(alias, document_st_new, document_size_new);
                            changed = 2;
                        }
                    }
                }
                if ( !err )
                    err = !replic_document_content(alias, l_content_new, l_content, accounts, &changed, break_func);
                if ( !err && changed ) {
                    err = !protocol_write_change_document(alias, document_st_new, changed);
                }
            }
            else { // need delete
                switch (do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL)) {
                    case DO_OK:
                        if (!document_delete_(alias, &document_key0, accounts, break_func))
                            err = 1;
                        if ( !err )
                            err = !protocol_write_change_document(alias, &document.data, 3);
                        break;
                    case DO_KEY_NOT_FOUND:
                        break;
                    default:
                        err = TRUE;
                }
            }

            for (j = 0; j < DOCUMENT_CONTEXT_COUNT; j++) {
                if (l_content[j])
                    do_ret_list_free(l_content[j]);
                if (l_content_new[j])
                    do_list_free(l_content_new[j]);

            }
            do_free (l_content);
            do_free (l_content_new);
        }
        debug_assert(crnt == data + size);
    }

    for (i = 0; list && i < do_ret_list_count(list); i++) {
        document_st = do_ret_list_item_data(list, i);
        do_cpy(document_key0.dtype, document_st->dtype);
        do_cpy(document_key0.sklad, document_st->sklad);
        do_cpy(document_key0.document, document_st->document);

        if (!document_delete(alias, &document_key0, accounts, break_func)) {
            err = 1;
            break;
        }
        if ( !err )
            err = !protocol_write_change_document(alias, document_st, 3);
    }
    if (list)
        do_ret_list_free(list);
    if (!err) {
        for (i = 0; i < accounts->count; i++)
            if (!do_balance_rebuild(alias, accounts->list[i], break_func)) {
                err = 1;
                break;
            }

    }
    do_sort_list_free(accounts);

    return !err;
}

int replic_document(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    if (do_alias_tran_begin(alias) != DO_OK)
        return FALSE;

    if (replic_documents(alias, NULL, FALSE, data, size, break_func))
        return do_alias_tran_end(alias) == DO_OK;
    else {
        do_alias_tran_abort(alias);
        return FALSE;
    }
}

int replic_document_period(do_alias_t *alias,
                     char *dtype, char *sklad, struct tm *date_start, struct tm *date_end, int all,
                     void *data, size_t size,
                     do_extended_break_func break_func)
{

    do_ret_list_t *list;
    int err = 0;

    if (do_alias_tran_begin(alias) != DO_OK)
        return FALSE;

    BTI_LONG date1, date2;

    do_date_set(&date1,*date_start);
    do_date_set(&date2,*date_end);

    list = get_documents(alias, dtype, sklad, date1, date2, all, break_func);

    if (list)
        err = !replic_documents(alias, list, all, data, size, break_func);
    else
        err = 1;

    if (!err)
        err = do_alias_tran_end(alias) != DO_OK;
    else
        do_alias_tran_abort(alias);

    return !err;
}
int replic_stocks(do_alias_t *alias, const char *sklad, void *data, size_t size, do_extended_break_func break_func)
{
    int           count, i, j, del;
    do_ret_list_t *list;
    do_list_t           *do_list_ins, *do_list_ch, *do_list_del;
    size_t size_new;
    char *code;
    void         *crnt;
    do_size_t  *sz;
    int status;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    stock_key0_t    stock_key0;
    stock_rec_t     stock;
    stock_rec_t     stock1;
    stock_struct_t *stock_st;
    stock_struct_t *stock_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(stock_struct_t));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }
	do_log(LOG_INFO, "read from local");
    list = replic_select_stocks(alias, sklad, break_func);
	do_log(LOG_INFO, "end read from local");

    status = (list != NULL) ? DO_OK : DO_ERROR;

	do_log(LOG_INFO, "check");//!!
	do_log(LOG_INFO, "%d %d",do_ret_list_count(list),do_list_ins->count);
	int indx_c=0;
	j=0;
    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
		/*if ( !(i % 100) )
		   usleep(10000);*/
        stock_st = (stock_struct_t*)do_ret_list_item_data(list, i);
        del = TRUE;
        for (; j < do_list_ins->count; j++) {
            int cmp;
            stock_new_st = do_list_ins->list[j];
            if ( stock_new_st->code[1]=='5' && stock_new_st->code[3]=='7')
                size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));
			indx_c++;
			if ( !strncmp(stock_new_st->code,"28065",5) ) {
                cmp=memcmp(&stock_st->code, &stock_new_st->code, sizeof(stock_st->code));
			}
			cmp=memcmp(&stock_st->code, &stock_new_st->code, sizeof(stock_st->code));
			//do_log(LOG_INFO, "\"%s\" \"%s\" %d %d %d",do_text(alias,stock_new_st->code),do_text(alias,stock_st->code), cmp,i,j);
            if ( !cmp ) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(stock_st, stock_new_st, size_new)) {
                    do_list_add(do_list_ch, stock_new_st);
                }
                break;
            }
            if ( cmp < 0 ) break;
        }
        if (del)
            do_list_add(do_list_del, stock_st);
    }
	do_log(LOG_INFO, "%d",indx_c);

	do_log(LOG_INFO, "check for insert");//!!
    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        stock.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&stock.data, do_list_ins->list[i], stock.size);
        //usleep(10000);//!!
        code = do_text(alias, stock.data.code);
        do_log(LOG_INFO, "insert stock \"%s\"", code);
        do_free(code);
#ifndef CH1
        do_cpy(stock_key0.sklad,stock.data.sklad);
        do_cpy(stock_key0.code,stock.data.code);
        if (do_stock_get0(alias,&stock1,&stock_key0,DO_GET_EQUAL) == DO_KEY_NOT_FOUND )
	        status = do_stock_insert(alias, &stock);
	    else
	        status = do_stock_update(alias, &stock);
        if ( !protocol_write_change_stock(alias, &stock_key0) )
            status = DO_ERROR;
#endif
    }
    do_log(LOG_INFO, "check for update");
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        stock_st = do_list_ch->list[i];
        memcpy(&stock_key0.sklad,  &stock_st->sklad,  sizeof(stock_key0.sklad));
        memcpy(&stock_key0.code,  &stock_st->code,  sizeof(stock_key0.code));
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);

        code = do_text(alias, stock_key0.code);
        do_log(LOG_INFO, "update stock \"%s\"", code);
        do_free(code);
        //usleep(10000);//!!
#ifndef CH1

        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            stock.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            if (memcmp(&stock.data, do_list_ch->list[i], stock.size)) {
            memcpy(&stock.data, do_list_ch->list[i], stock.size);
            status = do_stock_update(alias, &stock);
            }
        }
        if ( !protocol_write_change_stock(alias, &stock_key0) )
            status = DO_ERROR;
#endif
    }
	do_log(LOG_INFO, "check for update");
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        stock_st = do_list_del->list[i];
        memcpy(&stock_key0.sklad,  &stock_st->sklad,  sizeof(stock_key0.sklad));
        memcpy(&stock_key0.code,  &stock_st->code,  sizeof(stock_key0.code));

        code = do_text(alias, stock_key0.code);
        do_log(LOG_INFO, "delete stock 15  \"%s\"", code);
#ifdef DEB_EXIT1
        exit(15);
#endif
        do_free(code);
        //usleep(10000);//!!
#ifndef CH1

        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            status = do_stock_delete(alias);
            if ( !protocol_write_change_stock(alias, &stock_key0) )
                status = DO_ERROR;
        }
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
#endif
    }
	do_log(LOG_INFO, "end");
    if (list)
        do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);
	do_log(LOG_INFO, "return");

    return status == DO_OK;
}
int add_protocol(do_alias_t *alias,
                     struct tm *date_start, struct tm *date_end,
                     void *data, size_t size,
                     do_extended_break_func break_func)

{

    int           count, i, j;
    do_ret_list_t *list;
    do_list_t           *do_list_ins;
    //size_t size_new;
    char *username;
    void         *crnt;
    do_size_t  *sz;
    int status;
    do_extended_t *extended;

    do_list_ins = do_list_new(0);

    protocol_key1_t    protocol_key1;
    protocol_rec_t     protocol;
    protocol_struct_t *protocol_st;
    protocol_struct_t *protocol_new_st;

    crnt = data;

    do_protocol_context_t context;
    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(protocol_struct_t));
            protocol_new_st = crnt;
            username = do_text(alias, protocol_new_st->userName);

			context.object = DO_PROTOCOL_OBJECT_NONE;
			do_protocol_parse_st(alias, protocol_new_st, &context);
            if ( context.object == DO_PROTOCOL_OBJECT_DOCUMENT )
                do_list_add(do_list_ins, crnt);

            do_free(username);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }

    do_protocol_key1_date_set(&protocol_key1, date_end);
    //do_date_set2(&protocol_key1.date, date_end);
    protocol_key1.time = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PROTOCOL);
    do_extended_set_key(extended, 1, &protocol_key1);
    do_extended_set_filter(extended, TRUE, 1, sizeof(protocol_key1.date));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(protocol_struct_t, date),
                                DO_EXTENDED_COMP_LE,
                                DO_EXTENDED_CONNECTION_NO,
                                &protocol_key1.date);

    do_extended_set_fields_full_record(extended, sizeof(protocol_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        protocol_st = (protocol_struct_t*)do_ret_list_item_data(list, i);
        for (j =0; j < do_list_ins->count; j++) {
            protocol_new_st = do_list_ins->list[j];
            //size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if (!memcmp(protocol_st, protocol_new_st, sizeof(*protocol_st))) {
                do_list_delete(do_list_ins, j);
                break;
            }
        }
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        protocol.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&protocol.data, do_list_ins->list[i], protocol.size);

        username = do_text(alias, protocol.data.userName);
        do_log(LOG_INFO, "insert protocol \"%s\"", username);
        do_free(username);

        status = do_protocol_insert(alias, &protocol);
    }
    do_ret_list_free(list);
    do_list_free(do_list_ins);

    return status == DO_OK;
}

#ifndef DOMINO78
do_ret_list_t *replic_select_shifts(do_alias_t *alias, const char *sklad, int date_start, int date_end, do_extended_break_func break_func)
{
    shift_key3_t shift_key3;

    do_ret_list_t *list;
    do_extended_t *extended;

    do_set_empty(shift_key3);
    do_text_set(alias, shift_key3.sklad, sklad);
    shift_key3.date = date_start;
    shift_key3.time = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_SHIFT);
    do_extended_set_key(extended, 3, &shift_key3);
    do_extended_set_filter(extended, TRUE, 2, sizeof(shift_key3.sklad), sizeof(shift_key3.date));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(shift_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &shift_key3.sklad);
    shift_key3.date = date_end;
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(shift_struct_t, date),
                                DO_EXTENDED_COMP_LE,
                                DO_EXTENDED_CONNECTION_NO,
                                &shift_key3.date);
    shift_key3.date = date_start;

    do_extended_set_fields_full_record(extended, sizeof(shift_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);

    return list;
}
static int get_shift_content_from_data(void *data, size_t size, void **ptr, do_list_t **l_content)
{
    void *crnt;
    crnt = *ptr;
    int count;
    do_size_t *sz;
    int err = 0, i, j;

    for (j = 0; j < 2; j++)
        l_content[j] = do_list_new(0);

    if (!size)
        return TRUE;

    for (j = 0; !err && j < 2; j++) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert_break(crnt <= data + size);
        debug_assert_break(*sz >= 0);

        count = *sz;
        for (i = 0; !err && i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert_break(crnt <= data + size);
            switch (j) {
                case 0:
                    debug_assert_break(*sz == sizeof(checksum_struct_t));
                    break;
                case 1:
                    debug_assert_break(*sz == sizeof(check_struct_t));
                    break;
            }
            if (!err)
                do_list_add(l_content[j], crnt);
            crnt += *sz;
            debug_assert_break(crnt <= data + size);
        }
    }
    *ptr = crnt;
    if (err) {
        for (j = 0; j < 2; j++)
            if (l_content[j])
                do_list_free(l_content[j]);
    }
    return !err;
}

static int shift_insert(do_alias_t *alias, shift_struct_t *shift_st, size_t size)
{
    BTI_WORD len;
    len = size;
    do_log(LOG_INFO, "insert shift \"%d.%d\"", shift_st->cash, shift_st->shift);
    return do_alias_btr(alias, B_INSERT, DO_DB_SHIFT, shift_st, &len, NULL, -1) == DO_OK;

}
static int delete_shift_sum(do_alias_t *alias, shift_key0_t *shift_key0, do_extended_break_func break_func)
{
    checksum_key0_t checksum_key0;
    checksum_rec_t checksum;
    int status;
    do_set_empty(checksum_key0);
    checksum_key0.cash = shift_key0->cash;
    checksum_key0.shift = shift_key0->shift;

	status = do_checksum_get0(alias, &checksum, &checksum_key0, DO_GET_GE);
	while (status == DO_OK) {
	    if (!RUNNING_) {
	        status = DO_ERROR;
	        break;
	    }
	    if (
            checksum_key0.cash != shift_key0->cash ||
            checksum_key0.shift != shift_key0->shift
           )
           break;

        status = do_checksum_delete(alias);
        if (status != DO_OK)
            break;
        status = do_checksum_get0(alias, &checksum, &checksum_key0, DO_GET_NEXT);
	}
	return status != DO_ERROR;
}
static int delete_shift_check(do_alias_t *alias, shift_key0_t *shift_key0, do_extended_break_func break_func)
{
    check_key0_t check_key0;
    check_rec_t checksum;
    int status;
    do_set_empty(check_key0);
    check_key0.cash = shift_key0->cash;
    check_key0.shift = shift_key0->shift;

	status = do_check_get0(alias, &checksum, &check_key0, DO_GET_GE);
	while (status == DO_OK) {
	    if (!RUNNING_) {
	        status = DO_ERROR;
	        break;
	    }
	    if (
            check_key0.cash != shift_key0->cash ||
            check_key0.shift != shift_key0->shift
           )
           break;

        status = do_check_delete(alias);
        if (status != DO_OK)
            break;
        status = do_check_get0(alias, &checksum, &check_key0, DO_GET_NEXT);
	}
	return status != DO_ERROR;
}

static int shift_change(do_alias_t *alias, shift_struct_t *shift_st, size_t size)
{
    BTI_WORD len;
    len = size;
    int status;
    do_log(LOG_INFO, "change shift \"%d.%d\"", shift_st->cash, shift_st->shift);
    shift_key0_t shift_key0;
    shift_rec_t shift;
    memset(&shift_key0,0,sizeof(shift_key0));
    shift_key0.cash = shift_st->cash;
    shift_key0.shift = shift_st->shift;
    status = do_shift_get0(alias, &shift, &shift_key0, DO_GET_EQUAL);
    if (status != DO_OK)
        return FALSE;
    return do_alias_btr(alias, B_UPDATE, DO_DB_SHIFT, shift_st, &len, NULL, -1) == DO_OK;
}
static int checksum_equal(checksum_struct_t *d1, checksum_struct_t *d2)
{

    return d1->cash == d2->cash &&
           d1->shift == d2->shift &&
           d1->date == d2->date &&
           d1->check == d2->check &&
           d1->time == d2->time;
}

static int checksum_insert(do_alias_t *alias, checksum_struct_t *checksum_st, size_t size)
{
    BTI_WORD len;
    len = size;
    do_log(LOG_INFO, "insert checksum \"%d.%d\"", checksum_st->cash, checksum_st->shift);
    return do_alias_btr(alias, B_INSERT, DO_DB_CHECKSUM, checksum_st, &len, NULL, -1) == DO_OK;
}
static int checksum_delete(do_alias_t *alias, checksum_key0_t *checksum_key0)
{
    int status;

    do_log(LOG_INFO, "delete checksum \"%d.%d\"", checksum_key0->cash, checksum_key0->shift);

    checksum_rec_t checksum;
    status = do_checksum_get0(alias, &checksum, checksum_key0, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return FALSE;
    if (status != DO_KEY_NOT_FOUND)
        status = do_checksum_delete(alias);
    else
        status = DO_OK;
    if (status != DO_OK)
        return FALSE;
    return TRUE;
}
static int checksum_change(do_alias_t *alias, checksum_struct_t *checksum_st, size_t size)
{
    BTI_WORD len;
    len = size;
    int status;
    do_log(LOG_INFO, "change checksum \"%d.%d\"", checksum_st->cash, checksum_st->shift);

    checksum_key0_t checksum_key0;
    checksum_rec_t checksum;
    memset(&checksum_key0, 0 ,sizeof(checksum_key0));
    checksum_key0.cash = checksum_st->cash;
    checksum_key0.shift = checksum_st->shift;
    checksum_key0.date = checksum_st->date;
    checksum_key0.check = checksum_st->check;
    checksum_key0.time = checksum_st->time;

    status = do_checksum_get0(alias, &checksum, &checksum_key0, DO_GET_EQUAL);
    if (status != DO_OK)
        return FALSE;
    return do_alias_btr(alias, B_UPDATE, DO_DB_CHECKSUM, checksum_st, &len, NULL, -1) == DO_OK;
}


static int replic_shift_sum(do_alias_t *alias, do_list_t *do_list_new, do_ret_list_t *list, do_extended_break_func break_func)
{
    checksum_struct_t *checksum_st, *checksum_st_new;
    checksum_key0_t checksum_key0;

    size_t checksum_size, checksum_size_new;
    int i, j;
    for (i = 0; i < do_list_new->count; i++) {
        if (!RUNNING_)
            return FALSE;

        checksum_st_new = (checksum_struct_t*)do_list_new->list[i];
        checksum_size_new = *((do_size_t*)(do_list_new->list[i] - sizeof(do_size_t)));
        checksum_size = 0;
        for (j = 0; j < do_ret_list_count(list); j++)
            if (checksum_equal(checksum_st_new, (checksum_struct_t*)do_ret_list_item_data(list, j))) {
                checksum_size = do_ret_list_item_len(list, j);
                checksum_st = (checksum_struct_t*)do_ret_list_item_data(list, j);
                do_ret_list_delete(list, j);
                break;
            }
        if (!checksum_size) {
            if (!checksum_insert(alias, checksum_st_new, checksum_size_new))
                return FALSE;
        }
        else
            if (checksum_size != checksum_size_new ||
                memcmp(checksum_st, checksum_st_new, checksum_size)) {
                if (!checksum_change(alias, checksum_st_new, checksum_size_new))
                    return FALSE;
            }
    }
    for (j = 0; j < do_ret_list_count(list); j++) {
        checksum_st = (checksum_struct_t*)do_ret_list_item_data(list, j);
        do_set_empty(checksum_key0);
        checksum_key0.cash = checksum_st->cash;
        checksum_key0.shift = checksum_st->shift;
        checksum_key0.date = checksum_st->date;
        checksum_key0.time = checksum_st->time;
        checksum_key0.check = checksum_st->check;
        if (!checksum_delete(alias, &checksum_key0))
            return FALSE;
    }

    return TRUE;
}


static int check_equal(check_struct_t *d1, check_struct_t *d2)
{

    return d1->cash == d2->cash &&
           d1->shift == d2->shift &&
           d1->date == d2->date &&
           d1->time == d2->time &&
           d1->check == d2->check &&
           d1->position == d2->position;

}

static int check_insert(do_alias_t *alias, check_struct_t *check_st, size_t size)
{
    BTI_WORD len;
    len = size;
    do_log(LOG_INFO, "insert check \"%d.%d\"", check_st->cash, check_st->shift);
    return do_alias_btr(alias, B_INSERT, DO_DB_CHECK, check_st, &len, NULL, -1) == DO_OK;
}
static int check_delete(do_alias_t *alias, check_key0_t *check_key0)
{
    int status;

    do_log(LOG_INFO, "delete check \"%d.%d\"", check_key0->cash, check_key0->shift);

    check_rec_t checksum;
    status = do_check_get0(alias, &checksum, check_key0, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return FALSE;
    if (status != DO_KEY_NOT_FOUND)
        status = do_check_delete(alias);
    else
        status = DO_OK;
    if (status != DO_OK)
        return FALSE;
    return TRUE;
}
static int check_change(do_alias_t *alias, check_struct_t *check_st, size_t size)
{
    BTI_WORD len;
    len = size;
    int status;
    do_log(LOG_INFO, "change check \"%d.%d\"", check_st->cash, check_st->shift);

    check_key0_t check_key0;
    check_rec_t checksum;
    memset(&check_key0,0,sizeof(check_key0));
    check_key0.cash = check_st->cash;
    check_key0.shift = check_st->shift;
    check_key0.date = check_st->date;
    check_key0.check = check_st->check;
    check_key0.position = check_st->position;
    check_key0.time = check_st->time;

    status = do_check_get0(alias, &checksum, &check_key0, DO_GET_EQUAL);
    if (status != DO_OK)
        return FALSE;
    return do_alias_btr(alias, B_UPDATE, DO_DB_CHECK, check_st, &len, NULL, -1) == DO_OK;
}

static int replic_shift_check(do_alias_t *alias, do_list_t *do_list_new, do_ret_list_t *list, do_extended_break_func break_func)
{
    check_struct_t *check_st, *check_st_new;
    check_key0_t check_key0;

    size_t check_size, check_size_new;
    int i, j;
    for (i = 0; i < do_list_new->count; i++) {
        if (!RUNNING_)
            return FALSE;

        check_st_new = (check_struct_t*)do_list_new->list[i];
        check_size_new = *((do_size_t*)(do_list_new->list[i] - sizeof(do_size_t)));
        check_size = 0;
        for (j = 0; j < do_ret_list_count(list); j++)
            if (check_equal(check_st_new, (check_struct_t*)do_ret_list_item_data(list, j))) {
                check_size = do_ret_list_item_len(list, j);
                check_st = (check_struct_t*)do_ret_list_item_data(list, j);
                do_ret_list_delete(list, j);
                break;
            }
        if (!check_size) {
            if (!check_insert(alias, check_st_new, check_size_new))
                return FALSE;
        }
        else
            if (check_size != check_size_new ||
                memcmp(check_st, check_st_new, check_size)) {
                if (!check_change(alias, check_st_new, check_size_new))
                    return FALSE;
            }
    }
    for (j = 0; j < do_ret_list_count(list); j++) {
        check_st = (check_struct_t*)do_ret_list_item_data(list, j);
        do_set_empty(check_key0);

        check_key0.cash = check_st->cash;
        check_key0.shift = check_st->shift;
        check_key0.date = check_st->date;
        check_key0.check = check_st->check;
        check_key0.position = check_st->position;
        check_key0.time = check_st->time;

        if (!check_delete(alias, &check_key0))
            return FALSE;
    }

    return TRUE;
}


static int shift_delete(do_alias_t *alias, shift_key0_t *shift_key0, do_extended_break_func break_func)
{
    int status;

    do_log(LOG_INFO, "delete shift \"%d.%d\"", shift_key0->cash, shift_key0->shift);
    shift_rec_t shift;

    status = do_shift_get0(alias, &shift, shift_key0, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return FALSE;
    if (status != DO_KEY_NOT_FOUND)
        status = do_shift_delete(alias);
    else
        status = DO_OK;
    if (status != DO_OK)
        return FALSE;
    return
        delete_shift_sum(alias, shift_key0, break_func) &&
        delete_shift_check(alias, shift_key0, break_func);
}
int replic_checks(do_alias_t *alias,
                     char *sklad, struct tm *date_start, struct tm *date_end,
                     void *data, size_t size,
                     do_extended_break_func break_func)

{

    do_ret_list_t *list;
    shift_key0_t shift_key0;
    int err = 0;

    list = replic_select_shifts(alias, sklad, do_datetosql(*date_start), do_datetosql(*date_end), break_func);

    if (list) {
        shift_struct_t *shift_st = NULL;
        shift_struct_t *shift_st_new;
        size_t shift_size, shift_size_new;
        do_list_t *l_content_new[2];
        do_ret_list_t *l_content[2];

        void *record;
        int i, j;

        debug_assert(size >= 0);
        /*!!typedef struct PACK_STRUCT {
            do_size_t size;
            shift_struct_t shift;
        } exchange_t;*/

        if (size > 0) {
            void *crnt;
            int count;
            do_size_t *sz;
            shift_rec_t shift;
            shift_key0_t shift_key0;

            crnt = data;
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            count = *sz;

            for (i = 0; i < count; i++) {
                if (!RUNNING_) {
                    err = 1;
                    break;
                }
                sz = crnt;
                record = crnt;
                crnt += sizeof(*sz);
                debug_assert(crnt <= data + size);
                debug_assert(*sz == sizeof(shift_struct_t));
                crnt += *sz;
                debug_assert(crnt <= data + size);

                shift_size_new = *((do_size_t*)record);
                shift_st_new = (record + sizeof(do_size_t));

                l_content[0] = replic_select_shift_checksum(alias, shift_st_new, break_func);
                l_content[1] = replic_select_shift_check(alias, shift_st_new, break_func);
                if (!l_content[0] || !l_content[1]) {
                    err = 1;
                    break;
                }
                if (!get_shift_content_from_data(data, size, &crnt, l_content_new)) {
                    err = 1;
                    break;
                }
                shift_size = 0;
                for (j = 0; j < do_ret_list_count(list); j++) {
                    shift_st = (shift_struct_t*)do_ret_list_item_data(list, j);
                    if (shift_st_new->cash == shift_st->cash &&
                        shift_st_new->shift == shift_st->shift) {
                        shift_size = do_ret_list_item_len(list, j);
                        do_ret_list_delete(list, j);
                        break;
                    }
                }
                if (!shift_size) {
                    shift_key0.cash = shift_st_new->cash;
                    shift_key0.shift = shift_st_new->shift;
                    switch (do_shift_get0(alias, &shift, &shift_key0, DO_GET_EQUAL)) {
                        case DO_OK:
                            shift_size = shift.size;
                            shift_st = &shift.data;
                            break;
                        case DO_KEY_NOT_FOUND:
                            break;
                        default:
                            err = 1;
                    }
                }
                if (!err) {
                    if (!shift_size)
                        err = !shift_insert(alias, shift_st_new, shift_size_new);
                    else {
                        if (shift_size != shift_size_new ||
                            memcmp(shift_st, shift_st_new, shift_size)) {
                            err = !shift_change(alias, shift_st_new, shift_size_new);
                        }
                    }
                }
                if (!err)
                    err = !replic_shift_sum(alias, l_content_new[0], l_content[0], break_func) ||
                          !replic_shift_check(alias, l_content_new[1], l_content[1], break_func);

                for (j = 0; j < 2; j++) {
                    if (l_content[j])
                        do_ret_list_free(l_content[j]);
                    if (l_content_new[j])
                        do_list_free(l_content_new[j]);
                }
            }
            debug_assert(crnt == data + size);
        }

        for (i = 0; i < do_ret_list_count(list); i++) {
            shift_st = (shift_struct_t*)do_ret_list_item_data(list, i);
            shift_key0.cash = shift_st->cash;
            shift_key0.shift = shift_st->shift;

            if (!shift_delete(alias, &shift_key0, break_func)) {
                err = 1;
                break;
            }
        }
        do_ret_list_free(list);
    }
    else
        err = 1;

    return !err;
}
#endif
static int do_list_to_data(do_data_t *out, do_ret_list_t *list)
{
    int i;
    size_t size = 0;
    size_t old_size = do_data_size(out);
    void *crnt;
    do_size_t *psize;

    for (i = 0; i < do_ret_list_count(list); i++)
        size += do_ret_list_item_len(list, i);

    size += do_ret_list_count(list)*sizeof(*psize) + sizeof(*psize);
    size += old_size;

    if (!do_data_realloc(out, size)) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", size);
        return FALSE;
    }
    else {
        crnt = do_data_get(out) + old_size;
        psize = crnt;
        crnt += sizeof(*psize);
        *psize = do_ret_list_count(list);
        for (i = 0; i < do_ret_list_count(list); i++) {
            psize = crnt;
            crnt += sizeof(*psize);
            *psize = do_ret_list_item_len(list, i);
            memcpy(crnt, do_ret_list_item_data(list, i), do_ret_list_item_len(list, i));
            crnt += do_ret_list_item_len(list, i);
        }
    }
    return TRUE;
}
int replic_get_product(do_alias_t *alias, const char *code, do_data_t *out, do_extended_break_func break_func)
{
    product_key4_t product_key4;
    product_rec_t  product;
    do_extended_t *extended = NULL;
    do_ret_list_t *list;
    int            status;

    do_text_set(alias, product_key4.code, code);
    status = do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL);
    switch (status) {
        case DO_KEY_NOT_FOUND:
            return TRUE;
        case DO_ERROR:
            return FALSE;
        default:
            break;
    }
    do_size_t size;
    int err = 0;
    size = product.size;
    do_data_add(out, &size, sizeof(size));
    do_data_add(out, &product.data, size);

// views
    product_view_key1_t  product_view_key1;
    memcpy(&product_view_key1.code, &product.data.code, sizeof(product.data.code));
    do_text_set(alias, product_view_key1.sklad, "");

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PRODUCT_VIEW);
    do_extended_set_key(extended, 1, &product_view_key1);
    do_extended_set_filter(extended, TRUE, 1, sizeof(product_view_key1.code));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(product_view_struct_t, code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &product_view_key1.code);
    do_extended_set_fields_full_record(extended, sizeof(product_view_struct_t));
    list = do_extended_get(extended, -1);

    if (!list)
        err = 1;
    else {
        err = !do_list_to_data(out, list);
        do_ret_list_free(list);
    }

// data
    if (!err) {
        product_data_key0_t  product_data_key0;//, key_tmp;
        memcpy(&product_data_key0.code, &product.data.code, sizeof(product.data.code));
        if ( !strcmp(code, PRODUCT_FIRST_CODE) ) // skip local data
            product_data_key0.type = PRODUCT_DATA_FIRST_TYPE;
        else
            product_data_key0.type = 0;
        product_data_key0.number = 0;
        //key_tmp.type = PRODUCT_DATA_RESERVE_TYPE;

        do_extended_set_file_db(extended, DO_DB_PRODUCT_DATA);
        do_extended_set_key(extended, 0, &product_data_key0);
        do_extended_set_filter(extended, TRUE, 1, sizeof(product_data_key0.code));//, sizeof(product_data_key0.type));
        do_extended_set_filter_term(extended, 0,
                                    DO_EXTENDED_TYPE_STRING,
                                    offsetof(product_data_struct_t, code),
                                    DO_EXTENDED_COMP_EQUAL,
                                    DO_EXTENDED_CONNECTION_NO,// DO_EXTENDED_CONNECTION_AND,
                                    &product_data_key0.code);
        /*do_extended_set_filter_term(extended, 1,
                                    DO_EXTENDED_TYPE_INTEGER,
                                    offsetof(product_data_struct_t, type),
                                    DO_EXTENDED_COMP_NOT_EQUAL,
                                    DO_EXTENDED_CONNECTION_NO,
                                    &key_tmp.type);*/
        do_extended_set_fields_full_record(extended, sizeof(product_data_struct_t));
        list = do_extended_get(extended, -1);
        if (!list)
            err = 1;
        else {
            err = !do_list_to_data(out, list);
            do_ret_list_free(list);
        }
    }

// barcodes
    if (!err) {
        barcode_key1_t  barcode_key1;
        memcpy(&barcode_key1.code, &product.data.code, sizeof(product.data.code));

        do_extended_set_file_db(extended, DO_DB_BARCODE);
        do_extended_set_key(extended, 1, &barcode_key1);
        do_extended_set_filter(extended, TRUE, 1, sizeof(barcode_key1.code));
        do_extended_set_filter_term(extended, 0,
                                    DO_EXTENDED_TYPE_STRING,
                                    offsetof(barcode_struct_t, code),
                                    DO_EXTENDED_COMP_EQUAL,
                                    DO_EXTENDED_CONNECTION_NO,
                                    &barcode_key1.code);
        do_extended_set_fields_full_record(extended, sizeof(barcode_struct_t));
        list = do_extended_get(extended, -1);
        if (!list)
            err = 1;
        else {
            do_list_to_data(out, list);
            do_ret_list_free(list);
        }
    }
    if (extended)
        do_extended_free(extended);

    return !err;
}
int replic_get_partner(do_alias_t *alias, const char *g_code, int code, do_data_t *out, do_extended_break_func break_func)
{
    int err = 0;
    partner_key0_t       partner_key0;
    partner_rec_t        partner;
    int status;
    do_text_set(alias, partner_key0.g_code, g_code);
    partner_key0.code = code;
    do_extended_t *extended = NULL;
    do_ret_list_t *list;

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
    switch (status) {
        case DO_KEY_NOT_FOUND:
            return TRUE;
        case DO_ERROR:
            return FALSE;
        default:
            break;
    }
    do_size_t size;
    size = partner.size;
    do_data_add(out, &size, sizeof(size));
    do_data_add(out, &partner.data, size);

// views

#ifdef USE_DB_PARNER_VIEW
    partner_view_key1_t  partner_view_key1;
    memcpy(&partner_view_key1.g_code, &partner.data.g_code, sizeof(partner.data.g_code));
    memcpy(&partner_view_key1.code, &partner.data.code, sizeof(partner.data.code));
    partner_view_key1.type = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PARTNER_VIEW);
    do_extended_set_key(extended, 1, &partner_view_key1);
    do_extended_set_filter(extended, TRUE, 2, sizeof(partner_view_key1.g_code), sizeof(partner_view_key1.code));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(partner_view_struct_t, g_code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &partner_view_key1.g_code);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(partner_view_struct_t, code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &partner_view_key1.code);

    do_extended_set_fields_full_record(extended, sizeof(partner_view_struct_t));
    list = do_extended_get(extended, -1);
    if (!list)
        err = 1;
    else {
        do_list_to_data(out, list);
        do_ret_list_free(list);
    }
    do_select_term_free(term);
    do_select_key_free(key);
#else
    do_size_t size_null = 0;
    do_data_add(out, &size_null, sizeof(size_null));
#endif
// data
    if (!err) {
        partner_data_key0_t  partner_data_key0;
        memcpy(&partner_data_key0.g_code, &partner.data.g_code, sizeof(partner.data.g_code));
        memcpy(&partner_data_key0.code, &partner.data.code, sizeof(partner.data.code));
        partner_data_key0.type = 0;
        partner_data_key0.number = 0;

        if (!extended)
            extended = do_extended_new(alias);
        do_extended_set_file_db(extended, DO_DB_PARTNER_DATA);
        do_extended_set_key(extended, 0, &partner_data_key0);
        do_extended_set_filter(extended, TRUE, 2, sizeof(partner_data_key0.g_code), sizeof(partner_data_key0.code));
        do_extended_set_filter_term(extended, 0,
                                    DO_EXTENDED_TYPE_STRING,
                                    offsetof(partner_data_struct_t, g_code),
                                    DO_EXTENDED_COMP_EQUAL,
                                    DO_EXTENDED_CONNECTION_AND,
                                    &partner_data_key0.g_code);
        do_extended_set_filter_term(extended, 1,
                                    DO_EXTENDED_TYPE_INTEGER,
                                    offsetof(partner_data_struct_t, code),
                                    DO_EXTENDED_COMP_EQUAL,
                                    DO_EXTENDED_CONNECTION_NO,
                                    &partner_data_key0.code);

        do_extended_set_fields_full_record(extended, sizeof(partner_data_struct_t));
        list = do_extended_get(extended, -1);

        if (!list)
            err = 1;
        else {
            do_list_to_data(out, list);
            do_ret_list_free(list);
        }
    }
    if (extended)
        do_extended_free(extended);

    return !err;
}
int replic_get_document_order(do_alias_t *alias, const char *dtype, const char *sklad, const char *document, int line, do_data_t *out, do_extended_break_func break_func)
{
    document_order_key0_t document_order_key0;
    document_order_rec_t  document_order;
    do_text_set(alias, document_order_key0.dtype, dtype);
    do_text_set(alias, document_order_key0.sklad, sklad);
    do_text_set(alias, document_order_key0.document, document);
    document_order_key0.line = line;

    switch (do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_EQUAL)) {
        case DO_KEY_NOT_FOUND:
            return TRUE;
        case DO_ERROR:
            return FALSE;
        default: {
            do_size_t size;
            size = document_order.size;
            do_data_add(out, &size, sizeof(size));
            do_data_add(out, &document_order.data, size);
        }
    }
    return TRUE;
}
int replic_get_stock(do_alias_t *alias, const char *store, const char *code, do_data_t *out, do_extended_break_func break_func)
{
    stock_key0_t stock_key0;
    stock_rec_t stock;

    do_text_set(alias, stock_key0.sklad, store);
    do_text_set(alias, stock_key0.code, code);

    switch (do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL)) {
        case DO_ERROR:
            return FALSE;
        case DO_KEY_NOT_FOUND:
            stock.size = sizeof(stock.data);
            do_cpy(stock.data.sklad,stock_key0.sklad);
            do_cpy(stock.data.code,stock_key0.code);
            memset(stock.data.quant, 0, sizeof(stock.data.quant));
            memset(stock.data.nquant, 0, sizeof(stock.data.nquant));
        default: {
            do_size_t size;
            size = stock.size;
            do_data_add(out, &size, sizeof(size));
            do_data_add(out, &stock.data, size);
        }
    }
    return TRUE;
}
int replic_get_stocks(do_alias_t *alias, do_list_t *list, do_data_t *out, do_extended_break_func break_func)
{
    stock_key0_t *stock_key0;
    stock_rec_t stock;
    int i;

    for ( i = 0; i < list->count; i++ ) {
        stock_key0 = list->list[i];
        switch (do_stock_get0(alias, &stock, stock_key0, DO_GET_EQUAL)) {
            case DO_ERROR:
                return FALSE;
            case DO_KEY_NOT_FOUND:
                stock.size = sizeof(stock.data);
                do_cpy(stock.data.sklad,stock_key0->sklad);
                do_cpy(stock.data.code,stock_key0->code);
                memset(stock.data.quant, 0, sizeof(stock.data.quant));
                memset(stock.data.nquant, 0, sizeof(stock.data.nquant));
            default: {
                do_size_t size;
                size = stock.size;
                do_data_add(out, &size, sizeof(size));
                do_data_add(out, &stock.data, size);
#ifdef HAVY_DEBUG
                char *store,*code;
                store=do_text(alias, stock.data.sklad);
                code=do_text(alias, stock.data.code);
                do_log(LOG_INFO, "get stock %s %s %.3f", store, code, do_stock_quant(alias, &stock, 1));
                do_free(store);
                do_free(code);
#endif
            }
        }
    }
    return TRUE;
}
#ifndef DOMINO78
do_ret_list_t *replic_select_realization(do_alias_t *alias, const char *sklad, int date_start, int date_end, do_extended_break_func break_func)
{
    do_extended_t *extended = NULL;
    do_ret_list_t *list;

    realization_key1_t realization_key1;
    do_text_set(alias, realization_key1.sklad, sklad);
    do_text_set_empty(realization_key1.code);

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_REALIZATION);
    do_extended_set_key(extended, 1, &realization_key1);
    do_extended_set_filter(extended, TRUE, 2,
                           sizeof(realization_key1.sklad),
                           sizeof(realization_key1.month));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(realization_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &realization_key1.sklad);
    realization_key1.month = date_end;
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(realization_struct_t, month),
                                DO_EXTENDED_COMP_LE,
                                DO_EXTENDED_CONNECTION_NO,
                                &realization_key1.month);

    realization_key1.month = date_start;

    do_extended_set_fields_full_record(extended, sizeof(realization_struct_t));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);
    return list;
}
#endif
#ifndef DOMINO78
int replic_realization(do_alias_t *alias,
                     char *sklad, struct tm *date_start, struct tm *date_end,
                     void *data, size_t size,
                     do_extended_break_func break_func)

{

    int        count;
    void      *crnt = NULL;
    do_size_t *sz;
    BTI_WORD   len;
    int i;

    if ( !util_clean_realization(alias, sklad, do_datetosql(*date_start), do_datetosql(*date_end), break_func) )
        return FALSE;

    if (size) {
        crnt = data;
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz != sizeof(realization_struct_t));

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(realization_struct_t));
            len = *sz;
            if ( do_alias_btr(alias, B_INSERT, DO_DB_REALIZATION,
                                crnt, &len, NULL, -1) != DO_OK )
                return FALSE;
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }
    return TRUE;
}
#endif
static int protocol_write_change_stock(do_alias_t *alias, stock_key0_t *key)
{
    char *code,*store;
    int res;
    code = do_text(alias, key->code);
    store = do_text(alias, key->sklad);
    res = replic_protocol_write_change_stock(alias, code, store);
    do_free(code);
    do_free(store);
    return res;
}
int replic_protocol_write_change_stock(do_alias_t *alias, const char *code, const char *store)
{
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
    char *buff;
    int res;
    BTI_LONG date,time;
    int date_int;

    buff = (char*)do_malloc(64);//
    do_date_set(&date, tm);
    do_time_set(&time, tm);
    int status,i;

    /*switch (do_alias_tran_begin(alias)) {
        case DO_ERROR:
             do_log(LOG_ERR, "Error start transaction");
             return FALSE;
        case DO_INUSE:
             return FALSE;
        default:
             break;
    }*/
    status = DO_ERROR;
    product_data_rec_t product_data;
    product_data_key0_t product_data_key0;
    do_text_set(alias, product_data_key0.code, "00001");
    product_data_key0.type = 77;
    product_data_key0.number = INT_MAX;

    do_cpy(product_data.data.code,product_data_key0.code);
    product_data.data.type = product_data_key0.type;

    if ( do_product_data_key0(alias, &product_data_key0, DO_GET_LT) != DO_OK ||
         do_cmp(product_data.data.code,product_data_key0.code) ||
         product_data.data.type != product_data_key0.type )
         product_data.data.number = 1;
    else
         product_data.data.number = product_data_key0.number + 1;


    do_product_data_params_clear(alias, &product_data);
    do_product_data_param_set(alias, &product_data, "1", "5"); // stock or price
    do_product_data_param_set(alias, &product_data, "2", code);

    date_int=date;
    sprintf(buff,"%d",date_int);
    do_product_data_param_set(alias, &product_data, "3", buff);
    date_int=time;
    sprintf(buff,"%d",date_int);
    do_product_data_param_set(alias, &product_data, "4", buff);
    do_product_data_param_set(alias, &product_data, "5", store);
    //do_log(LOG_INFO, "write protocol %d", product_data.data.number);
    do_free(buff);
    do_alias_set_dublicate_allow(alias, TRUE);
    for ( i = 0; i < 200; i++ ) {
         status = do_product_data_insert(alias, &product_data);
         if ( status != DO_DUPLICATE_KEY_VALUE )
              break;
         product_data.data.number += 1;
    }
    do_alias_set_dublicate_allow(alias, FALSE);
    res = status == DO_OK;
    if ( !res )
        do_log(LOG_ERR, "Error write protocol");
    /*if ( res ) {
        if (do_alias_tran_end(alias) != DO_OK) {
	    do_log(LOG_ERR, "Error end transaction");
    	res = FALSE;
	}
    }
    else
	do_alias_tran_abort(alias);*/
    return res;
}

static int protocol_write_change_document(do_alias_t *alias, document_struct_t *document_st, int changed)
{
    int i;
    int res = TRUE;
    char *dtype,*store,*document;
    int handbook = FALSE;

    dtype = do_text(alias, document_st->dtype);
    store = do_text(alias, document_st->sklad);
    document = do_text(alias, document_st->document);
    for ( i = 0; i < DO_ARRAY_ELEMS(handbook_document); i++ ) {
        if ( handbook_document[i][0] == document_st->dtype[0] && handbook_document[i][1] == document_st->dtype[1] ) {
            handbook = TRUE;
            break;
        }
    }
    if ( !handbook ) {
        document_type_key0_t document_type_key0;
        do_text_set(alias, document_type_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
        if ( document_type_key0.dtype[0] == document_st->dtype[0] && document_type_key0.dtype[1] == document_st->dtype[1] )
            handbook = TRUE;
        if ( !handbook ) {
            do_text_set(alias, document_type_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
            if ( document_type_key0.dtype[0] == document_st->dtype[0] && document_type_key0.dtype[1] == document_st->dtype[1] )
                handbook = TRUE;
        }
    }
    switch ( changed ) {
        case 1:
               res = replic_protocol_write_change_document(alias, dtype, store, document, "Создан", handbook);
        case 2:
               res = replic_protocol_write_change_document(alias, dtype, store, document, "Изменен", handbook);
               if ( res && document_st->accepted )
                   res = replic_protocol_write_change_document(alias, dtype, store, document, "Акцептован", handbook);
               break;
        case 3:
               res = replic_protocol_write_change_document(alias, dtype, store, document, "Удален", handbook);
               break;
        default:
               res = replic_protocol_write_change_document(alias, dtype, store, document, NULL, handbook);

     }
     do_free(dtype);
     do_free(document);
     do_free(store);
     return res;
}
int replic_protocol_write_change_document(do_alias_t *alias, const char *dtype, const char *store, const char *document, const char *action, int handbook)
{
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
    char *buff;
    int res = TRUE;
    BTI_LONG date,time;
    int date_int;

    do_date_set(&date, tm);
    do_time_set(&time, tm);
    int status, i;

    if ( action ) {
        protocol_rec_t protocol;
        char buff[100];
        sprintf(buff, "%s.%s.%s.%s", do_param(DO_PARAM_PROTOCOL_OBJECT_DOCUMENT), dtype, store, document);
        do_text_set(alias, protocol.data.objectName, buff);
        do_text_set(alias, protocol.data.userName, REPLIC_NAME);
        do_protocol_date_set(&protocol.data, &tm);
        do_protocol_time_set(&protocol.data, &tm);
        protocol.size = sizeof(protocol.data);
        do_text_set(alias, protocol.data.action, action);
        if ( do_protocol_insert(alias, &protocol) != DO_OK )
            return FALSE;
    }
    if ( handbook ) {
        int k;
        for ( k = 0; k < 2; k ++ ) {
        buff = (char*)do_malloc(64);//
        status = DO_ERROR;
        product_data_rec_t product_data;
        product_data_key0_t product_data_key0;
        do_text_set(alias, product_data_key0.code, "00001");
        product_data_key0.type = (k == 0) ? 77 : 11000;
        product_data_key0.number = INT_MAX;

        do_cpy(product_data.data.code,product_data_key0.code);
        product_data.data.type = product_data_key0.type;

        if ( do_product_data_key0(alias, &product_data_key0, DO_GET_LT) != DO_OK ||
             do_cmp(product_data.data.code,product_data_key0.code) ||
             product_data.data.type != product_data_key0.type )
             product_data.data.number = 1;
        else
             product_data.data.number = product_data_key0.number + 1;


        do_product_data_params_clear(alias, &product_data);
        do_product_data_param_set(alias, &product_data, "1", "2"); // document
        sprintf(buff,"%s.%s.%s",dtype,store,document);
        do_product_data_param_set(alias, &product_data, "2", buff);

        date_int=date;
        sprintf(buff,"%d",date_int);
        do_product_data_param_set(alias, &product_data, "3", buff);
        date_int=time;
        sprintf(buff,"%d",date_int);
        do_product_data_param_set(alias, &product_data, "4", buff);
        do_product_data_param_set(alias, &product_data, "5", store);
        //do_log(LOG_INFO, "write protocol %d", product_data.data.number);
        do_free(buff);
        do_alias_set_dublicate_allow(alias, TRUE);
        for ( i = 0; i < 200; i++ ) {
              status = do_product_data_insert(alias, &product_data);
              if ( status != DO_DUPLICATE_KEY_VALUE )
                   break;
              product_data.data.number += 1;
        }
        do_alias_set_dublicate_allow(alias, FALSE);
        res = status == DO_OK;
        if ( !res )
            do_log(LOG_ERR, "Error write protocol");
        }
    }
    return res;
}
int replic_get_last_index(do_alias_t *alias)
{
    enum_key0_t enum_key0;
    enum_rec_t enum_;
    int status;
    do_text_set(alias, enum_key0.name, "R.STOCK");
    status = do_enum_get0(alias, &enum_, &enum_key0, DO_GET_EQUAL);
    switch ( status )  {
        case DO_ERROR:
            return -1;
        case DO_OK:
            return enum_.data.lastvalue;
        case DO_KEY_NOT_FOUND:
            return 0;
    }
    return -1;
}
int replic_set_last_index(do_alias_t *alias, int index)
{
    enum_key0_t enum_key0;
    enum_rec_t enum_;
    int status;
    do_text_set(alias, enum_key0.name, "R.STOCK");
    status = do_enum_get0(alias, &enum_, &enum_key0, DO_GET_EQUAL);
    switch ( status )  {
        case DO_ERROR:
            return FALSE;
        case DO_OK:
            enum_.data.lastvalue = index;
            return do_enum_update(alias, &enum_) == DO_OK;
        case DO_KEY_NOT_FOUND:
            do_text_set(alias, enum_.data.name, "R.STOCK");
            enum_.data.lastvalue = index;
            enum_.data.step = 1;
            enum_.data.LowBound = 0;
            enum_.data.hiBound =  INT_MAX;
            enum_.data.cycleFl = 0;
            do_text_set(alias, enum_.data.format, "N11");
            return do_enum_insert(alias, &enum_) == DO_OK;
    }
    return -1;
}
static int do_stock_st_quant_is_empty(do_alias_t *alias, stock_struct_t *stock)
{
    int i;
    for (i = 0; i < 6; i++)
        if (stock->nquant[i])
            if ( do_pdecimal(stock->quant[i], 8, 3) )
                return FALSE;
    return TRUE;
}
int replic_updated_stocks(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int        count, i;
    do_list_t *list_in;
    do_list_t *list_insert, *list_change, *list_delete;
    void      *crnt;
    do_size_t *sz;
    int        status;
    int        last_index;
    BTI_LONG   *index;
    int        empty;

    list_in = do_list_new(FALSE);

    stock_key0_t    stock_key0;
    stock_rec_t     stock;
    stock_rec_t     stock1;
    stock_struct_t *stock_st;

    crnt = data;

    if ( size ) {
        index = crnt;
        crnt += sizeof(*index);
        last_index = *index;
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(stock_struct_t));
            do_list_add(list_in, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }
    if ( !list_in->count ) {
        do_list_free(list_in);
        replic_set_last_index(alias, last_index);
        return TRUE;
    }
    list_insert = do_list_new(FALSE);
    list_delete = do_list_new(FALSE);
    list_change = do_list_new(FALSE);

    status = DO_OK;

	do_log(LOG_INFO, "check for %d stock", list_in->count);//!!
    for ( i = 0; i < list_in->count; i++ ) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        stock_st = list_in->list[i];
        do_cpy(stock_key0.sklad, stock_st->sklad);
        do_cpy(stock_key0.code, stock_st->code);
        empty = stock_st->code[5] == '.' && do_stock_st_quant_is_empty(alias, stock_st);

        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);

        switch ( status ) {
            case DO_KEY_NOT_FOUND:
                if ( !empty )
                    do_list_add(list_insert, stock_st);
                break;
            case DO_OK:
                if ( empty )
                    do_list_add(list_change, stock_st);
                else
                if ( memcmp(&stock.data, stock_st, sizeof(stock.data)) )
                    do_list_add(list_change, stock_st);
                break;
        }
    }

	do_log(LOG_INFO, "check for insert");//!!
    for (i = 0; i < list_insert->count && status == DO_OK; i++) {
        char *code;
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        stock.size = sizeof(stock.data);
        memcpy(&stock.data, list_insert->list[i], stock.size);
        //usleep(10000);//!!
        code = do_text(alias, stock.data.code);
        do_log(LOG_INFO, "insert stock \"%s\"", code);
        do_free(code);
        do_cpy(stock_key0.sklad,stock.data.sklad);
        do_cpy(stock_key0.code,stock.data.code);
        if (do_stock_get0(alias,&stock1,&stock_key0,DO_GET_EQUAL) == DO_KEY_NOT_FOUND )
	        status = do_stock_insert(alias, &stock);
	    else
	        status = do_stock_update(alias, &stock);
        /*if ( !protocol_write_change_stock(alias, &stock_key0) )
            status = DO_ERROR;*/

    }
	do_log(LOG_INFO, "check for update");
    for (i = 0; i < list_change->count && status == DO_OK; i++) {
        char *code;
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        stock_st = list_change->list[i];
        memcpy(&stock_key0.sklad,  &stock_st->sklad,  sizeof(stock_key0.sklad));
        memcpy(&stock_key0.code,  &stock_st->code,  sizeof(stock_key0.code));
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);

        //code = do_text(alias, stock_key0.code);
        //do_log(LOG_INFO, "update stock \"%s\"", code);
        //do_free(code);
        //usleep(10000);//!!

        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            stock.size = sizeof(stock.data);
            if (memcmp(&stock.data, stock_st, stock.size)) {
                memcpy(&stock.data, stock_st, stock.size);
                code = do_text(alias, stock_key0.code);
                do_log(LOG_INFO, "update stock \"%s\"", code);
                do_free(code);
                status = do_stock_update(alias, &stock);
            }
        }
        /*if ( !protocol_write_change_stock(alias, &stock_key0) )
            status = DO_ERROR;*/
    }
	do_log(LOG_INFO, "check for update");
    for (i = 0; i < list_delete->count && status == DO_OK; i++) {
        char *code;
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        stock_st = list_delete->list[i];
        memcpy(&stock_key0.sklad,  &stock_st->sklad,  sizeof(stock_key0.sklad));
        memcpy(&stock_key0.code,  &stock_st->code,  sizeof(stock_key0.code));

        code = do_text(alias, stock_key0.code);
        do_log(LOG_INFO, "delete stock 16 \"%s\"", code);
#ifdef DEB_EXIT1
        exit(16);
#endif
        do_free(code);
        //usleep(10000);//!!

        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            status = do_stock_delete(alias);
            /*if ( !protocol_write_change_stock(alias, &stock_key0) )
                status = DO_ERROR;*/
        }
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
	do_log(LOG_INFO, "end");

    do_list_free(list_in);
    do_list_free(list_insert);
    do_list_free(list_delete);
    do_list_free(list_change);
	do_log(LOG_INFO, "return");

    return replic_set_last_index(alias, last_index);
}
/*!!
int replic_updated_stocks(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int        count, i, j, del;
    do_list_t *list;
    do_list_t *list_in;
    do_list_t *list_insert, *list_change, *list_delete;
    void      *crnt;
    int        cmp;
    do_size_t *sz;
    int        status;
    int        last_index;
    BTI_LONG   *index;

    list_in = do_list_new(FALSE);

    stock_key0_t    stock_key0,crnt_stock_key;
    stock_rec_t     stock;
    stock_rec_t     stock1;
    stock_struct_t *stock_st;
    stock_struct_t *stock_new_st;

    crnt = data;

    if ( size ) {
        index = crnt;
        crnt += sizeof(*index);
        last_index = *index;
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz == sizeof(stock_struct_t));
            do_list_add(list_in, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }
    if ( !list_in->count ) {
        do_list_free(list_in);
        replic_set_last_index(alias, last_index);
        return TRUE;
    }
    list_insert = do_list_new(FALSE);
    list_delete = do_list_new(FALSE);
    list_change = do_list_new(FALSE);

    stock_new_st = list_in->list[0];

    do_set_empty(crnt_stock_key.code);

    strncpy(crnt_stock_key.code, stock_new_st->code, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE));
    do_cpy(crnt_stock_key.sklad, stock_new_st->sklad);

    status = DO_OK;
    list = do_list_new(TRUE);

    for ( j = 0; j < list_in->count; j++ ) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        do_list_clear(list);
        s
        if ( !replic_select_stock(alias, &crnt_stock_key, list, NULL) ) {
            status = DO_ERROR;
            break;
        }

        for (i = 0; i < list->count; i++) {
            if (!RUNNING_) {
                status = DO_ERROR;
                break;
            }
            stock_st = (stock_struct_t*)(list->list[i]);
            del = TRUE;
            cmp = 1;
            for (; cmp > 0 &&  j < list_in->count; j++) {
                stock_new_st = list_in->list[j];
                if ( do_cmp(stock_st->sklad, stock_new_st) )
                    break;

                cmp = do_cmp(stock_st->code, stock_new_st->code);
                //do_log(LOG_INFO, "\"%s\" \"%s\" %d %d %d",do_text(alias,stock_new_st->code),do_text(alias,stock_st->code), cmp,i,j);
                if ( cmp > 0 )
                    do_list_add(list_insert, stock_new_st);
                else
                if ( cmp == 0 ) {
                    del = FALSE;
                    if ( do_cmp(stock_st, stock_new_st) )
                        do_list_add(list_change, stock_new_st);
                }
            }
            if ( del )
                do_list_add(list_delete, stock_st);
        }
        for ( ;j < list_in->count; j++) {
            stock_new_st = list_in->list[j];
            if ( do_cmp(crnt_stock_key.sklad, stock_new_st->sklad) ||
                 strncmp(crnt_stock_key.code, stock_new_st->code, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE)) )
                break;
            do_list_add(list_insert, stock_new_st);
        }
        strncpy(crnt_stock_key.code, stock_new_st->code, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE));
        do_cpy(crnt_stock_key.sklad, stock_new_st->sklad);
    }

	do_log(LOG_INFO, "check for insert");//!!
    for (i = 0; i < list_insert->count && status == DO_OK; i++) {
        char *code;
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        stock.size = sizeof(stock.data);
        memcpy(&stock.data, list_insert->list[i], stock.size);
        //usleep(10000);//!!
        code = do_text(alias, stock.data.code);
        do_log(LOG_INFO, "insert stock \"%s\"", code);
        do_free(code);
        do_cpy(stock_key0.sklad,stock.data.sklad);
        do_cpy(stock_key0.code,stock.data.code);
        if (do_stock_get0(alias,&stock1,&stock_key0,DO_GET_EQUAL) == DO_KEY_NOT_FOUND )
	        status = do_stock_insert(alias, &stock);
	    else
	        status = do_stock_update(alias, &stock);
        if ( !protocol_write_change_stock(alias, &stock_key0) )
            status = DO_ERROR;

    }
	do_log(LOG_INFO, "check for update");
    for (i = 0; i < list_change->count && status == DO_OK; i++) {
        char *code;
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        stock_st = list_change->list[i];
        memcpy(&stock_key0.sklad,  &stock_st->sklad,  sizeof(stock_key0.sklad));
        memcpy(&stock_key0.code,  &stock_st->code,  sizeof(stock_key0.code));
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);

        code = do_text(alias, stock_key0.code);
        do_log(LOG_INFO, "update stock \"%s\"", code);
        do_free(code);
        //usleep(10000);//!!

        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            stock.size = sizeof(stock.data);
            if (memcmp(&stock.data, stock_st, stock.size)) {
                memcpy(&stock.data, stock_st, stock.size);
                status = do_stock_update(alias, &stock);
            }
        }
        if ( !protocol_write_change_stock(alias, &stock_key0) )
            status = DO_ERROR;
    }
	do_log(LOG_INFO, "check for update");
    for (i = 0; i < list_delete->count && status == DO_OK; i++) {
        char *code;
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        stock_st = list_delete->list[i];
        memcpy(&stock_key0.sklad,  &stock_st->sklad,  sizeof(stock_key0.sklad));
        memcpy(&stock_key0.code,  &stock_st->code,  sizeof(stock_key0.code));

        code = do_text(alias, stock_key0.code);
        do_log(LOG_INFO, "delete stock \"%s\"", code);
        do_free(code);
        //usleep(10000);//!!

        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            status = do_stock_delete(alias);
            if ( !protocol_write_change_stock(alias, &stock_key0) )
                status = DO_ERROR;
        }
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
	do_log(LOG_INFO, "end");

    do_list_free(list_in);
    do_list_free(list_insert);
    do_list_free(list_delete);
    do_list_free(list_change);
	do_log(LOG_INFO, "return");

    return replic_set_last_index(alias, last_index);
}
*/
int replic_stock(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int empty;
    int res;
    stock_key0_t stock_key0;
    stock_rec_t stock;
    stock_struct_t *stock_st;
    char *store, *code;

    void       *crnt;
    do_size_t  *sz;

    crnt = data;
    sz = crnt;
    crnt += sizeof(*sz);
    debug_assert(crnt <= data + size);

    debug_assert(*sz == sizeof(*stock_st));

    stock_st = crnt;
    store = do_text(alias, stock_st->sklad);
    if ( !strcmp(store, domino_unit()) ) {
        do_free(store);
        return TRUE;
    }
    code = do_text(alias, stock_st->code);

    empty = stock_st->code[5] == '.' && do_stock_st_quant_is_empty(alias, stock_st);
    do_cpy(stock_key0.sklad, stock_st->sklad);
    do_cpy(stock_key0.code, stock_st->code);

    switch ( do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL) ) {
        case DO_ERROR:
            res = FALSE;
            break;
        case DO_KEY_NOT_FOUND:
            if ( empty )
                return TRUE;
            do_cpy(stock.data, *stock_st);

            do_log(LOG_INFO, "insert stock \"%s %s\"", store, code);
            res = do_stock_insert(alias, &stock) == DO_OK;
            break;
        case DO_OK:
            if ( empty ) {
                do_log(LOG_INFO, "delete stock 17 \"%s %s\"", store, code);
#ifdef DEB_EXIT1
                exit(17);
#endif
                res = do_stock_delete(alias) == DO_OK;
            }
            else
            if ( do_cmp(stock.data, *stock_st) ) {
                do_cpy(stock.data, *stock_st);
                do_log(LOG_INFO, "update stock \"%s %s\"", store, code);
                res = do_stock_update(alias, &stock) == DO_OK;
            }
            break;
        default:
            do_log(LOG_ERR, "logical error");
            res = FALSE;
    }
    return res;
}
int replic_stock_list(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func)
{
    int empty;
    int res;
    stock_key0_t stock_key0;
    stock_rec_t stock;
    stock_struct_t *stock_st;
    char *store, *code;

    void       *crnt;
    do_size_t  *sz;

    crnt = data;
    res = TRUE;
#ifdef HAVY_DEBUG
        do_log(LOG_INFO, "start replic stock");
#endif
    while ( crnt < data + size && res ) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz == sizeof(*stock_st));

        stock_st = crnt;
        crnt += *sz;

        store = do_text(alias, stock_st->sklad);
        if ( !strcmp(store, domino_unit()) ) {
            do_log(LOG_WARNING, "replic own stock store \"%s\"", store);
            do_free(store);
            return TRUE;
        }
        code = do_text(alias, stock_st->code);

        empty = stock_st->code[5] == '.' && do_stock_st_quant_is_empty(alias, stock_st);
        do_cpy(stock_key0.sklad, stock_st->sklad);
        do_cpy(stock_key0.code, stock_st->code);
#ifdef HAVY_DEBUG
        do_log(LOG_INFO, "set stock %s %s %.3f", store, code,
        do_stock_struct_quant(alias, stock_st, DO_CONST_QUANT_REST) +
                do_stock_struct_quant(alias, stock_st, DO_CONST_QUANT_CRNTSALE));
#endif

        switch ( do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL) ) {
            case DO_ERROR:
                res = FALSE;
                break;
            case DO_KEY_NOT_FOUND:
                if ( !empty ) {
                    do_cpy(stock.data, *stock_st);
                    do_log(LOG_INFO, "insert stock \"%s %s\"", store, code);
                    res = do_stock_insert(alias, &stock) == DO_OK;
                }
                else
                    res = TRUE;
                break;
            case DO_OK:
                if ( empty ) {
                    do_log(LOG_INFO, "delete stock 18 \"%s %s\"", store, code);
#ifdef DEB_EXIT1
                    exit(18);
#endif
                    res = do_stock_delete(alias) == DO_OK;
                }
                else
                if ( do_cmp(stock.data, *stock_st) ) {
                    do_cpy(stock.data, *stock_st);
                    do_log(LOG_INFO, "update stock \"%s %s\"", store, code);
                    res = do_stock_update(alias, &stock) == DO_OK;
                }
                break;
            default:
                do_log(LOG_ERR, "logical error");
                res = FALSE;
        }
    }
#ifdef HAVY_DEBUG
        do_log(LOG_INFO, "end replic stock");
#endif
    return res;
}
/*
int get_balance_41(do_alias_t *alias, const char *store, struct tm date, double *balance, do_extended_break_func break_func)
{
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;

    do_text_set(alias, sklad_key0.code, store);
    switch ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            do_log(LOG_ERR, "store \"%s\" not found", store);
            return FALSE;
        default:
            return FALSE;
    }

    prowod_key2_t prowod_key2,key2;
    prowod_key3_t prowod_key3,key3;
    prowod_rec_t prowod;
    int status;
    double sum;
    const char *subaccount[] = {"02-00","02-10","02-20"};
    int i;
    sum = 0;

    for ( i = 0; i < DO_N_ELEMENTS(subaccount); i++ ) {
        do_text_set_empty(prowod_key2.division);
        do_text_set(alias, prowod_key2.debit_account, "41");
        do_text_set(alias, prowod_key2.debit_subaccount, subaccount[i]);
        do_cpy(prowod_key2.debit_p_g_code, sklad.data.p_g_code);
        prowod_key2.debit_p_code = sklad.data.p_code;
        do_text_set_empty(prowod_key2.debit_other);
        prowod_key2.date = 0;
        do_cpy(key2, prowod_key2);
        do_date_set(&key2.date, date);

        status = do_prowod_get2(alias, &prowod, &prowod_key2, DO_GET_GE);
        while ( status == DO_OK ) {
            if ( !RUNNING_ ) return FALSE;
            if ( memcmp(&key2, &prowod_key2, sizeof(key2.division)+sizeof(key2.debit_account)+
                                             sizeof(key2.debit_subaccount)+sizeof(key2.debit_p_g_code)+
                                             sizeof(key2.debit_p_code)+sizeof(key2.debit_other)) ||
                 key2.date < prowod_key2.date ) break;

            sum += do_prowod_sum(alias, &prowod, 0);
            status = do_prowod_get2(alias, &prowod, &prowod_key2, DO_GET_NEXT);
        }
        if ( status == DO_ERROR )
            return FALSE;

    }
    for ( i = 0; i < DO_N_ELEMENTS(subaccount); i++ ) {
        do_text_set_empty(prowod_key3.division);
        do_text_set(alias, prowod_key3.credit_account, "41");
        do_text_set(alias, prowod_key3.credit_subaccount, subaccount[i]);
        do_cpy(prowod_key3.credit_p_g_code, sklad.data.p_g_code);
        prowod_key3.credit_p_code = sklad.data.p_code;
        do_text_set_empty(prowod_key3.credit_other);
        prowod_key3.date = 0;
        do_cpy(key3, prowod_key3);
        do_date_set(&key3.date, date);

        status = do_prowod_get3(alias, &prowod, &prowod_key3, DO_GET_GE);
        while ( status == DO_OK ) {
            if ( !RUNNING_ ) return FALSE;
            if ( memcmp(&key3, &prowod_key3, sizeof(key3.division)+sizeof(key3.credit_account)+
                                             sizeof(key3.credit_subaccount)+sizeof(key3.credit_p_g_code)+
                                             sizeof(key3.credit_p_code)+sizeof(key3.credit_other)) ||
                 key3.date < prowod_key3.date ) break;

            sum -= do_prowod_sum(alias, &prowod, 0);
            status = do_prowod_get3(alias, &prowod, &prowod_key3, DO_GET_NEXT);
        }
        if ( status == DO_ERROR )
            return FALSE;
    }
    *balance = sum;
    return TRUE;
}*/
int get_balance_41(do_alias_t *alias, const char *store, struct tm date, double *balance, int *count,  do_extended_break_func break_func)
{
    document_key3_t document_key3,key3;
    document_rec_t document;
    int status;
    double sum_in,sum_out;
    sum_in = 0;
    *count = 0;
    do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
    do_text_set(alias, document_key3.sklad, store);
    document_key3.date = 0;
    document_key3.time = 0;
    do_cpy(key3, document_key3);
    do_document_key3_date_set(&key3, &date);
    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key3.dtype, document_key3.dtype) ||
             do_cmp(key3.sklad, document_key3.sklad) ||
             key3.date < document_key3.date ) break;
        if ( document.data.accepted ) {
            *count=*count + 1;
            sum_in += do_document_sum(alias, &document, 11);
        }
        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;
    sum_out = 0;
    do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    do_text_set(alias, document_key3.sklad, store);
    document_key3.date = 0;
    document_key3.time = 0;
    do_cpy(key3, document_key3);
    do_document_key3_date_set(&key3, &date);
    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key3.dtype, document_key3.dtype) ||
             do_cmp(key3.sklad, document_key3.sklad) ||
             key3.date < document_key3.date ) break;
        if ( document.data.accepted ) {
            *count=*count + 1;
            sum_out += do_document_sum(alias, &document, 11);
        }
        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;
    *balance = sum_in - sum_out;
    return TRUE;
}
#define ISLEAP(year)	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#define	SECS_PER_HOUR (60 * 60)
int do_protocol_key1_date_set(protocol_key1_t *key, struct tm *tm)
{
    int y = 1801;
    key->date = tm->tm_yday + 4;
    for (;y < tm->tm_year + 1900; y++) {
        key->date += ISLEAP(y) ? 366 : 365;
    }
    return key->date;
}
int do_protocol_date_set(protocol_struct_t *key, struct tm *tm)
{
    int y = 1801;
    key->date = tm->tm_yday + 4;
    for (;y < tm->tm_year + 1900; y++) {
        key->date += ISLEAP(y) ? 366 : 365;
    }
    return key->date;
}
int do_protocol_time_set(protocol_struct_t *key, struct tm *tm)
{
    key->time = (tm->tm_hour*SECS_PER_HOUR + tm->tm_min*60 + tm->tm_sec)*100;
    return key->time;
}
int do_document_key3_date_set(document_key3_t *key, struct tm *tm)
{
    int y = 1801;
    key->date = tm->tm_yday + 4;
    for (;y < tm->tm_year + 1900; y++) {
        key->date += ISLEAP(y) ? 366 : 365;
    }
    return key->date;
}

do_ret_list_t *replic_select_product_data_base_parcel(do_alias_t *alias, const char *sklad, int base_parcel, product_key4_t *key1, product_key4_t *key2, do_extended_break_func break_func)
{
#ifndef DOMINO78
    char *prefix = NULL;
    int   code_size = 0;
#endif // DOMINO78
    int   n = 0;
    char  parcel_separator = do_param(DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR)[0];
    int   base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE);

    do_ret_list_t       *retval;
    product_data_key0_t  product_data_key0;//, key_tmp;
    do_extended_t       *extended;
#ifndef DOMINO78
    if ( base_parcel ) {
        prefix = domino_get_unit_prefix(alias, sklad);
        if (!prefix || *prefix == '\0') {
            do_log(LOG_ERR, "local product's prefix are empty");
            if (prefix)
                do_free(prefix);
            return NULL;
        }
        code_size = strlen(prefix);
    }
#endif
    n++;
    if ( do_text_is_empty(key1->code) )
        do_text_set(alias, product_data_key0.code, PRODUCT_FIRST_CODE);// skip local data
        //do_text_set_empty(product_data_key0.code);
    else
        do_cpy(product_data_key0.code,key1->code);
    if ( !do_text_is_empty(key2->code) )
        n++;
    //do_text_set_empty(product_data_key0.code);
#ifndef DOMINO78
    product_data_key0.type = PRODUCT_DATA_FIRST_TYPE;
#else
    product_data_key0.type = 0;
#endif // DOMINO78
    product_data_key0.number = 0;
    //key_tmp.type = PRODUCT_DATA_RESERVE_TYPE;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PRODUCT_DATA);
    do_extended_set_key(extended, 0, &product_data_key0);
#ifndef DOMINO78
    do_extended_set_filter(extended, FALSE, n, (base_parcel) ? code_size : sizeof(parcel_separator),sizeof(product_data_key0.code));
#else
    do_extended_set_filter(extended, FALSE, n, sizeof(parcel_separator),sizeof(product_data_key0.code));
#endif
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(product_data_struct_t, code) + base_len,
                                (base_parcel) ?
                                  DO_EXTENDED_COMP_EQUAL :
                                  DO_EXTENDED_COMP_NOT_EQUAL,
                                ( n < 2) ? DO_EXTENDED_CONNECTION_NO : DO_EXTENDED_CONNECTION_AND,
#ifndef DOMINO78
                                (base_parcel) ?
                                  prefix :
#endif
                                  &parcel_separator);
    if ( n > 1 ) {
        do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(product_data_struct_t, code),
                                DO_EXTENDED_COMP_LT,
                                DO_EXTENDED_CONNECTION_NO,
                                key2->code);
    }
    do_extended_set_fields_full_record(extended, sizeof(product_data_struct_t));
    retval = do_extended_get(extended, -1);

#ifndef DOMINO78
    if (prefix)
        do_free(prefix);
#endif
    do_extended_free(extended);
    return retval;
}

int replic_products_data(do_alias_t *alias, void *data, size_t size, const char *sklad, int base_parcel, int clone, product_key4_t *key1, product_key4_t *key2, do_extended_break_func break_func)
{
    int            count, i, j, del;
    do_ret_list_t *list;
    do_list_t        *do_list_ins, *do_list_ch, *do_list_del;
    size_t         size_new;
    char          *code;
    void          *crnt;
    do_size_t   *sz;
    int            status;

    do_list_ins = do_list_new(0);
    do_list_del = do_list_new(0);
    do_list_ch  = do_list_new(0);

    product_data_key0_t    product_data_key0;
    product_data_rec_t     product_data;
    product_data_struct_t *product_data_st;
    product_data_struct_t *product_data_new_st;

    crnt = data;

    if (size) {
        sz = crnt;
        crnt += sizeof(*sz);
        debug_assert(crnt <= data + size);
        debug_assert(*sz >= 0);

        count = *sz;
        for (i = 0; i < count; i++) {
            sz = crnt;
            crnt += sizeof(*sz);
            debug_assert(crnt <= data + size);
            debug_assert(*sz <= sizeof(product_data_struct_t) && *sz >= sizeof(product_data_struct_t) - sizeof(product_data.data.params));
            do_list_add(do_list_ins, crnt);
            crnt += *sz;
            debug_assert(crnt <= data + size);
        }
    }
    list = replic_select_product_data_base_parcel(alias, sklad, base_parcel, key1, key2, break_func);

    status = (list != NULL) ? DO_OK : DO_ERROR;

    for (i = 0; status == DO_OK && i < do_ret_list_count(list); i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        product_data_st = do_ret_list_item_data(list, i);
        del = TRUE;
        for (j =0; j < do_list_ins->count; j++) {
            product_data_new_st = do_list_ins->list[j];
            size_new = *((do_size_t*)(do_list_ins->list[j] - sizeof(do_size_t)));

            if (product_data_st->type == product_data_new_st->type &&
                product_data_st->number == product_data_new_st->number &&
                !memcmp(&product_data_st->code, &product_data_new_st->code, sizeof(product_data_st->code))) {
                del = FALSE;
                do_list_delete(do_list_ins, j);
                if (size_new != do_ret_list_item_len(list, i) ||
                    memcmp(product_data_st, product_data_new_st, size_new)) {
                    do_list_add(do_list_ch, product_data_new_st);
                }
                break;
            }
        }
        if (del)
            do_list_add(do_list_del, product_data_st);
    }

    for (i = 0; i < do_list_ins->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }
        product_data.size = *((do_size_t*)(do_list_ins->list[i] - sizeof(do_size_t)));;
        memcpy(&product_data.data, do_list_ins->list[i], product_data.size);

        //if ( product_data.data.type == PRODUCT_DATA_RESERVE_TYPE )
        //    continue;

        code = do_text(alias, product_data.data.code);
        do_log(LOG_INFO, "insert product_data \"%s\"", code);
        status = do_product_data_insert(alias, &product_data);
        if ( status == DO_OK ) {
            if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                status = DO_ERROR;
        }
        do_free(code);
    }
    for (i = 0; i < do_list_ch->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }

        product_data_st = do_list_ch->list[i];
        //if ( product_data_st->type == PRODUCT_DATA_RESERVE_TYPE )
        //    continue;


        product_data_key0.type = product_data_st->type;
        product_data_key0.number = product_data_st->number;
        memcpy(&product_data_key0.code,  &product_data_st->code,  sizeof(product_data_key0.code));
        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_EQUAL);

        code = do_text(alias, product_data_key0.code);
        do_log(LOG_INFO, "update product_data \"%s\"", code);

        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            product_data.size = *((do_size_t*)(do_list_ch->list[i] - sizeof(do_size_t)));
            memcpy(&product_data.data, do_list_ch->list[i], product_data.size);
            status = do_product_data_update(alias, &product_data);
            if ( status == DO_OK ) {
                if ( !do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE) )
                    status = DO_ERROR;
            }

        }
        do_free(code);
    }
    for (i = 0; i < do_list_del->count && status == DO_OK; i++) {
        if (!RUNNING_) {
            status = DO_ERROR;
            break;
        }


        product_data_st = do_list_del->list[i];
        //if ( product_data_st->type == PRODUCT_DATA_RESERVE_TYPE )
        //    continue;

        product_data_key0.type = product_data_st->type;
        product_data_key0.number = product_data_st->number;
        memcpy(&product_data_key0.code,  &product_data_st->code,  sizeof(product_data_key0.code));

        code = do_text(alias, product_data_key0.code);
        do_log(LOG_INFO, "delete product_data \"%s\" %d %d", code, product_data_st->type, product_data_st->number);
        //exit(1);//!!


        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_EQUAL);
        if (status == DO_OK) {
            status = do_product_data_delete(alias);
            if ( status == DO_OK ) {
                if (!do_protocol_product_add(alias, code, DO_PROTOCOL_ACTION_CHANGE))
                    status = DO_ERROR;
            }
        }
        do_free(code);
        if (status == DO_KEY_NOT_FOUND)
            status = DO_OK;
    }
    if ( status != DO_OK && status != DO_ERROR ) {
        do_log(LOG_ERR, "the operation is completed with the status %d", status);
        status = DO_ERROR;
    }
    if (list)
        do_ret_list_free(list);
    do_list_free(do_list_ins);
    do_list_free(do_list_del);
    do_list_free(do_list_ch);

    return status == DO_OK;
}


//!!#endif
