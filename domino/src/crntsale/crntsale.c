#include "crntsale.h"
#include <domino.h>
#include <artix.h>
#include <limits.h>
#include <dolib.h>

#define DO_CRNTSALE_DB DO_DB_DOCUMENT, DO_DB_DOCUMENT_ORDER, DO_DB_STOCK, DO_DB_BARCODE, DO_DB_PRODUCT, DO_DB_END

inline static void error_info(artix_info_t *info, const char *msg)
{
    artix_info_add_upload_error_description(info, msg);
}
static int get_last_position(do_alias_t *alias, document_key0_t *key, int cash, int shift)
{
    document_order_key0_t document_order_key0;
    int status;

    status = do_document_key0(alias, key, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return -1;
    if (status == DO_KEY_NOT_FOUND) {
        document_rec_t document;
        time_t now = time(NULL);
        struct tm tm = *localtime(&now);
        char buf[20];

        do_cpy(document.data.dtype, key->dtype);
        do_text_set(alias, document.data.dtype1, do_param(DO_PARAM_DOCUMENT_OPERATION_CRNTSALE_DEFAULT));
        do_text_set_empty(document.data.name);
        do_cpy(document.data.sklad, key->sklad);
        do_cpy(document.data.document, key->document);
        do_date_set(&document.data.date, tm);
        do_time_set(&document.data.time, tm);
        do_text_set(alias, document.data.p_g_code, do_param(DO_PARAM_PARTNER_REGION_CASH));
        document.data.p_code = cash;
        sprintf(buf, "%2.2d/%6.6d", cash, shift);
        do_text_set(alias, document.data.p_doc, buf);
        do_date_set(&document.data.p_date, tm);
        do_time_set(&document.data.p_time, tm);
        do_text_set_empty(document.data.party);
        document.data.accepted = 0;
        document.size = sizeof(document.data) - sizeof(document.data.params);

        switch (do_document_insert(alias, &document)) {
            case DO_INUSE:
                return -2;
            case DO_OK:
                return 0;
            default:
                return -1;
        }
    }
    do_cpy(document_order_key0.dtype, key->dtype);
    do_cpy(document_order_key0.sklad, key->sklad);
    do_cpy(document_order_key0.document, key->document);
    document_order_key0.line = INT_MAX;

    switch (do_document_order_key0(alias, &document_order_key0, DO_GET_LE)) {
        case DO_ERROR:
            return -1;
        case DO_KEY_NOT_FOUND:
            return 0;
        default:
            if (!do_cmp(document_order_key0.dtype, key->dtype) &&
                !do_cmp(document_order_key0.sklad, key->sklad) &&
                !do_cmp(document_order_key0.document, key->document))
                return document_order_key0.line;
            return 0;
    }
}
static int get_last_position_(do_alias_t *alias, document_key0_t *key, int cash, int shift, struct tm *tm)
{
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    int status;

    status = do_document_key0(alias, key, DO_GET_EQUAL);
    if (status == DO_ERROR)
        return -1;
    if (status == DO_KEY_NOT_FOUND) {
        document_rec_t document;
        time_t now = time(NULL);
        struct tm tm = *localtime(&now);
        char buf[20];

        do_cpy(document.data.dtype, key->dtype);
        do_text_set(alias, document.data.dtype1, do_param(DO_PARAM_DOCUMENT_OPERATION_CRNTSALE_DEFAULT));
        do_text_set_empty(document.data.name);
        do_cpy(document.data.sklad, key->sklad);
        do_cpy(document.data.document, key->document);
        do_date_set(&document.data.date, tm);
        do_time_set(&document.data.time, tm);
        do_text_set(alias, document.data.p_g_code, do_param(DO_PARAM_PARTNER_REGION_CASH));
        document.data.p_code = cash;
        sprintf(buf, "%2.2d/%6.6d", cash, shift);
        do_text_set(alias, document.data.p_doc, buf);
        do_date_set(&document.data.p_date, tm);
        do_time_set(&document.data.p_time, tm);
        do_text_set_empty(document.data.party);
        document.data.accepted = 0;
        document.size = sizeof(document.data) - sizeof(document.data.params);

        switch (do_document_insert(alias, &document)) {
            case DO_INUSE:
                return -2;
            case DO_OK:
                return 0;
            default:
                return -1;
        }
    }
    do_cpy(document_order_key0.dtype, key->dtype);
    do_cpy(document_order_key0.sklad, key->sklad);
    do_cpy(document_order_key0.document, key->document);
    document_order_key0.line = INT_MAX;

    switch (do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_LE)) {
        case DO_ERROR:
            return -1;
        case DO_KEY_NOT_FOUND:
            return 0;
        default:
            if (!do_cmp(document_order_key0.dtype, key->dtype) &&
                !do_cmp(document_order_key0.sklad, key->sklad) &&
                !do_cmp(document_order_key0.document, key->document)) {
                do_date(document_order.data.date, tm);
                do_time(document_order.data.time, tm);

                return document_order_key0.line;
            }
            return 0;
    }
}
typedef struct {
    product_key4_t key;
    double quant;
} quant_t;

static inline int quant_cmp_func(quant_t *q1, quant_t *q2)
{
    return do_cmp(q1->key.code, q2->key.code);
}
#define MAX_OPCODE 6
static const char *do_opcode[MAX_OPCODE] =
{
    "02","01","","","04","03"
};

int mk_crntsale(option_t *opt)
{
    do_alias_t *alias;
    artix_t *artix;
    artix_cash_t cash;
    artix_shift_t shift;
    char *buf;
    char document_number[20];
    int cash_code;
    int line_position;
    int result = 1;
    int count;
    int last_check;
    int last_position;
    artix_sale_t *sales = NULL;
    document_key0_t document_key0;

    artix = artix_new(opt->artix_host, opt->artix_cs, opt->artix_port, opt->artix_user, opt->artix_password, FALSE);
    if (!artix)
        return 0;

    alias = domino_alias_new(NULL);
    if (!alias) {
        error_info(artix->info, "Can't create domino alias");
        result = 0;
        goto end;
    }
    if (!do_alias_open(alias, 0, DO_CRNTSALE_DB)) {
        error_info(artix->info, "Can't connect to domino db");
        result = 0;
        goto end;
    }

    if (!artix_alias_get_cash(artix->alias, &cash)) {
        error_info(artix->info, "Can't get cash info");
        result = 0;
        goto end;
    }
    if (cash.close) {
        result = 1;
        goto end;
    }
    buf = artix_text(artix->alias, cash.cash);
    cash_code = atoi(buf);
    if (!cash_code) {
        result = 0;
        do_log(LOG_ERR, "Undefine cash info \"%s\"", buf);
        do_free(buf);
        goto end;
    }
    do_free(buf);

// crntsale

    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_CRNTSALE));
    do_text_set(alias, document_key0.sklad, domino_unit());
    sprintf(document_number, "%2.2d%4.4d", cash_code, cash.shift);
    do_text_set(alias, document_key0.document, document_number);

    line_position = get_last_position(alias, &document_key0, cash_code, cash.shift);
    

    if ( line_position < 0 ) {
        if (line_position != -2)
            error_info(artix->info, "Error get last position");
        result = line_position != -2;
        goto end;
    }


    last_position = line_position % 1000;
    last_check = (line_position - last_position) / 1000;
    memcpy(&shift.shop, &cash.shop, sizeof(cash.shop));
    memcpy(&shift.shift, &cash.shift, sizeof(cash.shift));
    memcpy(&shift.cash, &cash.cash, sizeof(cash.cash));
    count = 0;
    do_log(LOG_INFO, "%s.%s.%d %d %d",artix_text(artix->alias,cash.shop),artix_text(artix->alias,cash.cash),cash.shift,last_check, last_position);
    result = artix_alias_get_sale(artix->alias, &shift, last_check, last_position, &count, &sales);
    do_log(LOG_INFO, "res %d count %d", result, count);
    if ( !result || !count )
         goto end;

    do_sort_list_t *quants;
    quant_t quant;
    stock_key0_t stock_key0;
    stock_rec_t stock;
    barcode_key0_t barcode_key0;
    barcode_rec_t barcode;
    product_key0_t product_key0;
    char *str;
    int i, indx, status;
    document_order_rec_t document_order;

    do_cpy(document_order.data.dtype, document_key0.dtype);
    do_text_set(alias, document_order.data.dtype1, do_param(DO_PARAM_DOCUMENT_OPERATION_CRNTSALE_DEFAULT));
    do_cpy(document_order.data.sklad, document_key0.sklad);
    do_cpy(document_order.data.document, document_key0.document);
    document_order.data.sort = 0;
    document_order.data.accepted = 1;
    do_set_empty(document_order.data.nprice);
    do_set_empty(document_order.data.price);
    do_set_empty(document_order.data.quant);
    do_set_empty(document_order.data.nquant);
    document_order.data.nquant[0] = DO_CONST_QUANT_CRNTSALE;
    document_order.data.nprice[0] = DO_CONST_PRICE_FACT;

    switch (do_alias_tran_begin(alias)) {
        case DO_ERROR:
             error_info(artix->info, "Error start transaction");
             result = 0;
        case DO_INUSE:
             goto end;
        default:
             break;
    }
    quants = do_sort_list_new(1, 1, (do_list_cmp_func)quant_cmp_func, NULL);
    for (i = 0; i < count && result; i++) {
        str = artix_text(artix->alias, sales[i].bcode);
        do_log(LOG_INFO,"inser bar %s", str);
        do_text_set(alias, barcode_key0.barcode, str);
        do_free(str);
        switch (do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL) ) {
    	    case DO_ERROR:
                error_info(artix->info, "Error get barcode");
            case DO_KEY_NOT_FOUND:
        	product_key0.base_parcel=1;
        	str=do_text(alias, barcode_key0.barcode);
        	do_text_set(alias, product_key0.code, str);
        	do_free(str);
        	if ( do_product_key0(alias, &product_key0, DO_GET_EQUAL) ==  DO_OK ) {
        	    str=do_text(alias, product_key0.code);
        	    do_text_set(alias, barcode.data.code, str);
        	    do_free(str);
        	    break;
        	}
        	str= do_strdup_printf("Barcode \"%s\" not found", do_text(alias, barcode_key0.barcode));
                result = 0;
                goto end;
	    default:
    	     break;
        }
        str = do_text(alias, barcode.data.code);
        do_text_set(alias, quant.key.code, str);
        do_free(str);

         switch ( sales[i].action ) {
               case ARTIX_ALIAS_SALE_ACTION_RETURN:
                     sales[i].quant = -sales[i].quant;
                     sales[i].sum = -sales[i].sum;
                     break;
               case ARTIX_ALIAS_SALE_ACTION_SALE:
                     break;
         }
         quant.quant = sales[i].quant;
         if (!do_sort_list_find(quants, &indx, &quant))
               do_sort_list_insert_new(quants, indx, &quant, sizeof(quant));
         else
              ((quant_t*)quants->list[indx])->quant += quant.quant;
        document_order.size = sizeof(document_order.data) - sizeof(document_order.data.params);
        document_order.data.line = sales[i].check*1000 + sales[i].pos;
        do_cpy(document_order.data.code, quant.key.code);
        do_date_set(&document_order.data.date, sales[i].time);
        do_time_set(&document_order.data.time, sales[i].time);
        do_pdecimal_set(document_order.data.quant[0], 7, 3, sales[i].quant);
        document_order.data.price[0] = sales[i].sum / sales[i].quant;

        do_document_order_param_int_set(alias, &document_order, DO_CONST_ACCEPT_ORDER_PARAM, 1);
        do_document_order_param_int_set(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_CHECK), sales[i].check);
        do_document_order_param_int_set(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_CASHIER), sales[i].cashier);
        str = artix_text(artix->alias, sales[i].bcode);
        do_document_order_param_set(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_BARCODE), str);
        do_free(str);
        do_document_order_param_int_set(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_POSITION), sales[i].pos);
        if (sales[i].operation < MAX_OPCODE)
           do_document_order_param_set(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_OPERATION), do_opcode[sales[i].operation]);
        else
           do_document_order_param_int_set(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_OPERATION), sales[i].operation);
        if ( do_document_order_insert(alias, &document_order) != DO_OK ) {
             error_info(artix->info, "Error insert check position");
             result = 0;
             break;
        }
        do_log_debug(ZONE, "Insert check %d.%d", sales[i].check, sales[i].pos);
    }
    do_cpy(stock_key0.sklad, document_key0.sklad);
    for (i = 0; i < quants->count && result; i++) {
          do_cpy(stock_key0.code, ((quant_t*)quants->list[i])->key.code);
          status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
          if (status == DO_ERROR) {
               result = 0;
               break;
          }
        if (status == DO_KEY_NOT_FOUND) {
              do_cpy(stock.data.sklad, document_key0.sklad);
              do_cpy(stock.data.code, ((quant_t*)quants->list[i])->key.code);
              do_set_empty(stock.data.nquant);
              do_set_empty(stock.data.quant);
              stock.data.nquant[0] = DO_CONST_QUANT_CRNTSALE;
              do_pdecimal_set(stock.data.quant[0], 8, 3, -((quant_t*)quants->list[i])->quant);
              if (do_stock_insert(alias, &stock) != DO_OK) {
                     result = 0;
                     break;
              }
        }
        else {
              do_stock_quant_set(alias, &stock, DO_CONST_QUANT_CRNTSALE,
                           do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE)
                           -((quant_t*)quants->list[i])->quant);
              if (do_stock_update(alias, &stock) != DO_OK) {
                   result = 0;
                   break;
             }
        }
    }
    if (result) {
        if (do_alias_tran_end(alias) != DO_OK) {
            error_info(artix->info, "Error end transaction");
            result = 0;
        }
    }
    else
        do_alias_tran_abort(alias);

    do_sort_list_free(quants);
end:
    if (sales)
        do_free(sales);

    if ( !result )
        goto end_;

// Storno

    do_text_set(alias, document_key0.dtype, "ST");

    do_text_set(alias, document_key0.sklad, domino_unit());
    sprintf(document_number, "%2.2d%4.4d", cash_code, cash.shift);
    do_text_set(alias, document_key0.document, document_number);
    struct tm tm;

    last_position = get_last_position_(alias, &document_key0, cash_code, cash.shift, &tm );
    if ( last_position < 0 ) {
        if (last_position != -2)
            error_info(artix->info, "Error get last position");
        result = last_position != -2;
        goto end_;
    }
    memcpy(&shift.shop, &cash.shop, sizeof(cash.shop));
    memcpy(&shift.shift, &cash.shift, sizeof(cash.shift));
    memcpy(&shift.cash, &cash.cash, sizeof(cash.cash));
    count = 0;
    artix_storno_t *stornos = NULL;

    result = artix_alias_get_storno(artix->alias, &shift, (last_position > 0) ? &tm : NULL, &count, &stornos);

    if ( !result || !count )
         goto end_;

    do_cpy(document_order.data.dtype, document_key0.dtype);
    do_text_set(alias, document_order.data.dtype1, do_param(DO_PARAM_DOCUMENT_OPERATION_CRNTSALE_DEFAULT));
    do_cpy(document_order.data.sklad, document_key0.sklad);
    do_cpy(document_order.data.document, document_key0.document);
    document_order.data.sort = 0;
    document_order.data.accepted = 1;
    do_set_empty(document_order.data.nprice);
    do_set_empty(document_order.data.price);
    do_set_empty(document_order.data.quant);
    do_set_empty(document_order.data.nquant);
    document_order.data.nquant[0] = DO_CONST_QUANT_CRNTSALE;
    document_order.data.nprice[0] = DO_CONST_PRICE_FACT;

    for (i = 0; i < count && result; i++) {
        str = artix_text(artix->alias, stornos[i].code);
        do_text_set(alias, quant.key.code, str);
        do_free(str);

        last_position++;

        document_order.size = sizeof(document_order.data) - sizeof(document_order.data.params);
        document_order.data.line = last_position;
        do_cpy(document_order.data.code, quant.key.code);
        do_date_set(&document_order.data.date, stornos[i].time_storno);
        do_time_set(&document_order.data.time, stornos[i].time_storno);
        do_pdecimal_set(document_order.data.quant[0], 7, 3, stornos[i].quant);
        document_order.data.price[0] = stornos[i].sum / stornos[i].quant;

        do_document_order_param_int_set(alias, &document_order, DO_CONST_ACCEPT_ORDER_PARAM, 1);
        do_document_order_param_int_set(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_CHECK), stornos[i].check);
        do_document_order_param_int_set(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_CASHIER), stornos[i].cashier);
        str = artix_text(artix->alias, stornos[i].bcode);
        do_document_order_param_set(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_BARCODE), str);
        do_free(str);
        str = do_datetoa(stornos[i].time);
        do_document_order_param_set(alias, &document_order, "DATE", str);
        do_free(str);
        str = do_timetoa(stornos[i].time);
        do_document_order_param_set(alias, &document_order, "TIME", str);
        do_free(str);
        do_document_order_param_int_set(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_POSITION), stornos[i].pos);
        if ( do_document_order_insert(alias, &document_order) != DO_OK ) {
             error_info(artix->info, "Error insert check position");
             result = 0;
             break;
        }
        do_log_debug(ZONE, "Insert check %d.%d", stornos[i].check, stornos[i].pos);
    }
end_:
    if (stornos)
        do_free(stornos);

    artix_free(artix);
    if (alias)
        do_alias_free(alias);
    return result;
}
