#include "util.h"

#define RUNNING_ (!break_func || !break_func(NULL))

int util_clean_realization(do_alias_t *alias, const char *sklad, int date_start, int date_end, do_extended_break_func break_func)
{
    int status;
    int res;
    realization_rec_t realization;
    realization_key1_t realization_key1, key;
    do_text_set(alias, realization_key1.sklad, sklad);
    do_cpy(key.sklad, realization_key1.sklad);
    realization_key1.month = date_start;
    do_text_set_empty(realization_key1.code);
    res = TRUE;

    status = do_realization_get1(alias, &realization, &realization_key1, DO_GET_GE);

    while ( status == DO_OK ) {
        if ( !RUNNING_ ) { res = FALSE; break; }
        if ( do_cmp(key.sklad, realization_key1.sklad) ||
             realization_key1.month > date_end ) break;
        if ( do_realization_delete(alias) != DO_OK ) {
            res = FALSE;
            break;
        }
        status = do_realization_get1(alias, &realization, &realization_key1, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        res = FALSE;

    return res;
}
static int realization_cmp(realization_struct_t *rec1, realization_struct_t *rec2)
{
    int res;
    res = rec1->month - rec2->month;
    if ( res )
        return res;
    res = do_cmp(rec1->code, rec2->code);
    return res;
}
static int util_make_realization_l(do_alias_t *alias, const char *sklad, struct tm *date_start, struct tm *date_end, do_sort_list_t *list, do_extended_break_func break_func)
{
    realization_struct_t rec, *item;
    document_rec_t document;
    document_key3_t document_key3, key;
    document_order_key0_t document_order_key0;
    do_extended_t *extended;
    do_ret_list_t *orders;
    struct tm doc_date;
    int status;
    int res, i, j;
    int base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);
    struct PACK_STRUCT {
        BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
        BTI_BYTE quant[4][7];
        BTI_SINT nquant[4];
        BTI_REAL price[8];
        BTI_SINT nprice[8];
    } *order;

    do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    do_text_set(alias, document_key3.sklad, sklad);
    do_date_set(&document_key3.date, *date_start);
    document_key3.time = 0;
    do_cpy(key.dtype, document_key3.dtype);
    do_cpy(key.sklad, document_key3.sklad);
    do_date_set(&key.date, *date_end);

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);
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

    do_extended_set_fields(extended, 5,
                            do_size_and_offset(document_order_struct_t, code),
                            do_size_and_offset(document_order_struct_t, quant),
                            do_size_and_offset(document_order_struct_t, nquant),
                            do_size_and_offset(document_order_struct_t, price),
                            do_size_and_offset(document_order_struct_t, nprice)
                           );
    res = TRUE;
    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( !RUNNING_ ) { res = FALSE; break; };
        if ( do_cmp(key.dtype, document_key3.dtype) ||
             do_cmp(key.sklad, document_key3.sklad) ||
             key.date < document_key3.date
            ) break;

        if ( document.data.accepted &&
             (!strncmp(document.data.dtype1, "01", 2) ||
              !strncmp(document.data.dtype1, "02", 2) ||
              !strncmp(document.data.dtype1, "03", 2) ||
              !strncmp(document.data.dtype1, "04", 2))
             ) {
            do_cpy(rec.sklad, document.data.sklad);
            do_date(document.data.date, &doc_date);
            doc_date.tm_mday = 1;
            mktime(&doc_date);
            rec.month = do_datetosql(doc_date);
            do_cpy(document_order_key0.dtype, document.data.dtype);
            do_cpy(document_order_key0.sklad, document.data.sklad);
            do_cpy(document_order_key0.document, document.data.document);
            document_order_key0.line = 0;

            do_extended_set_key(extended, 0, &document_order_key0);
            do_extended_set_filter_term_set_value(extended, 0, document.data.dtype);
            do_extended_set_filter_term_set_value(extended, 1, document.data.sklad);
            do_extended_set_filter_term_set_value(extended, 2, document.data.document);

            orders = do_extended_get(extended, -1);
            if ( !orders ) { res = FALSE; break; }

            for ( i = 0; i < do_ret_list_count(orders); i++ ) {
                double price2, price9;
                if ( !RUNNING_ ) { res = FALSE; break; }
                order = do_ret_list_item_data(orders, i);
                price2 = 0; price9 = 0;
                rec.total = 0;
                for ( j = 0; j < 4; j++ )
                    if ( order->nquant[j] == DO_CONST_QUANT_REST ) {
                        rec.total = do_pdecimal(order->quant[j], 7, 3);
                        break;
                    }
                for ( j = 0; j < 8; j ++ ) {
                    if ( order->nprice[j] == DO_CONST_PRICE_PURCHASE )
                        price2 = order->price[j];
                    if ( order->nprice[j] == DO_CONST_PRICE_FACT )
                        price9 = order->price[j];
                }
                do_cpy(rec.code, order->code);
                memset(rec.code + base_len, ' ', sizeof(rec.code) - base_len);
                rec.sale_sum = (rec.total*price9)*BTI_CURRENCY_SHIFT;
                rec.profit = (rec.total*(price9-price2))*BTI_CURRENCY_SHIFT;
                if ( do_sort_list_find(list, &j, &rec) ) {
                    item = list->list[j];
                    item->total += rec.total;
                    item->sale_sum += rec.sale_sum;
                    item->profit += rec.profit;
                }
                else
                    do_sort_list_insert_new(list, j, &rec, sizeof(rec));
            }
            do_ret_list_free(orders);
        }
        if ( !res ) break;

        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }
    do_extended_free(extended);
    if ( status == DO_ERROR ) {
        res = FALSE;
    }
    return res;

}
int util_make_realization(do_alias_t *alias, const char *sklad, int date_start, int date_end, do_extended_break_func break_func)
{
    if ( !util_clean_realization(alias, sklad, date_start, date_end, break_func) )
        return FALSE;

    do_meta_alias_t *meta;
    struct tm date_b;
    struct tm date_e;
    do_list_t *aliases;
    do_sort_list_t *list;
    int res, i;

    do_sqltodate(date_start, &date_b);
    do_sqltodate(date_end, &date_e);

    meta = domino_meta_alias_new();
    if ( !meta )
        return FALSE;
    aliases = domino_meta_alias_get_list_from_sklad(meta, sklad, &date_b, &date_e);
    if ( !aliases ) {
        domino_meta_alias_free(meta);
        return FALSE;
    }
    list = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)realization_cmp, NULL);

    res = TRUE;

    for ( i = 0; i < aliases->count; i++ ) {
        if ( !RUNNING_ ) { res = FALSE; break; }
        do_alias_period_t *sk;
        sk = aliases->list[i];
        if ( !do_alias_conected(sk->alias) )
            if ( !do_alias_open(sk->alias, FALSE, DO_DB_DOCUMENT, DO_DB_SKLAD, DO_DB_DOCUMENT_ORDER, DO_DB_PRODUCT, DO_DB_PRODUCT_DATA, DO_DB_END) ) {
                res = FALSE;
                break;
            }
        if ( !util_make_realization_l(sk->alias,
                                            sklad,
                                            &sk->date_b,
                                            &sk->date_e,
                                            list,
                                            break_func) ) {
            res = FALSE;
            break;
        }
    }
    for ( i = 0; i < list->count; i++ ) {
        BTI_WORD len = sizeof(realization_struct_t);
        //realization_struct_t *rec;
        //rec = list->list[i];
        if ( do_alias_btr(alias, B_INSERT, DO_DB_REALIZATION, list->list[i], &len, NULL, -1) != DO_OK ) {
            res = FALSE;
            break;
        }
    }

    do_sort_list_free(list);
    do_list_free(aliases);
    domino_meta_alias_free(meta);
    return res;
}
