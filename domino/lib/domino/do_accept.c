#include <domino.h>
#include <dolib.h>
#include <limits.h>
#include "../misc/define.h"

#define RUNNING_ (!break_func || !break_func(NULL))


static int do_accept_calc_stock(do_alias_t *alias, document_order_rec_t *document_order, do_doctype_t *doctype, int accept, do_extended_break_func break_func)
{
    int i, flag = 0;
    for ( i = 0; i < 4; i++ ) {
        if ( !RUNNING_ ) return FALSE;
        if ( !doctype->rest_action[i].number ) break;
        if ( !doctype->rest_action[i].action != DO_REST_ACTION_NONE ) {
            flag = 1;
            break;
        }
    }
    if ( !flag )
        return TRUE;

    double quant, rest;
    stock_rec_t stock, old;
    stock_key0_t stock_key0;
    int status;

    do_cpy(stock_key0.sklad, document_order->data.sklad);
    do_cpy(stock_key0.code, document_order->data.code);
    status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return FALSE;

    if ( status == DO_KEY_NOT_FOUND ) {
        do_cpy(stock.data.sklad, document_order->data.sklad);
        do_cpy(stock.data.code, document_order->data.code);
        memset(&stock.data.quant, 0, sizeof(stock.data.quant) + sizeof(stock.data.nquant));
        stock.size = sizeof(stock.data);
    }
    memcpy(&old.data, &stock.data, stock.size);

    for ( i = 0; i < 4; i++ ) {
        if ( !RUNNING_ ) return FALSE;
        if ( !doctype->rest_action[i].number ) break;
        if ( !doctype->rest_action[i].action == DO_REST_ACTION_NONE ) continue;
        quant = do_document_order_quant(alias, document_order, doctype->rest_action[i].number);
        if ( quant == 0 ) continue;
        if ( accept ) {
            if ( doctype->rest_action[i].action == DO_REST_ACTION_OUT )
                quant = -quant;
        }
        else {
            if ( doctype->rest_action[i].action == DO_REST_ACTION_IN )
                quant = -quant;
        }
        rest = do_stock_quant(alias, &stock, doctype->rest_action[i].number);
        if ( !do_stock_quant_set(alias, &stock, doctype->rest_action[i].number, rest+quant) )
            return FALSE;
    }
    if (memcmp(&old.data, &stock.data, stock.size)) {
        if ( status == DO_KEY_NOT_FOUND )
            status = do_stock_insert(alias, &stock);
        else
            status = do_stock_update(alias, &stock);
        return status == DO_OK;
    }
    return TRUE;
}

static int accept_document_order(do_alias_t *alias, document_order_rec_t *document_order, do_accept_t accept, do_doctype_t *doctype, int calc)
{
    if (calc && !do_accept_calc_stock(alias, document_order, doctype, 1, NULL))
        return 0;

    document_order->data.accepted = 1;
    int flag;
    if (accept == DO_ACCEPT_AND_INSERT)
        flag = do_document_order_insert(alias, document_order) == DO_OK;
    else
        flag = do_document_order_update(alias, document_order) == DO_OK;
    if (flag && accept == DO_ACCEPT_AND_INSERT)
        return do_protocol_document_order_add_rec(alias, document_order, DO_PROTOCOL_ACTION_INSERT, NULL);
    return flag;
}
static double calc_sum_from_dorder(do_alias_t *alias, document_order_rec_t *document_order, int number)
{
    int nds;
    int n;
    int str_nds=0;
    double price;

    if ( number < 1000 ) {
        nds = -1;
        n = number;
    }
    else {
        n = number / 1000;
        nds = number % 1000;
#ifdef NOCORRECTNDS
        if ( nds == 20 )
            nds = 18;
#endif

        if ( n == DO_CONST_PRICE_NDS_PURCHASE )
            str_nds = do_document_order_param_int(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_NDS_SUPPLIER));
        else
            str_nds = do_document_order_param_int(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_NDS));
#ifdef NOCORRECTNDS
        if ( str_nds == 20 )
            str_nds = 18;
#endif
    }

    if ( n == DO_CONST_PRICE_NDS_FACT )
        price = (do_document_order_price(alias, document_order, DO_CONST_PRICE_FACT) / (100. + nds)) * nds;
    else
    if ( n == DO_CONST_PRICE_NDS_PURCHASE )
        price = do_document_order_price(alias, document_order, DO_CONST_PRICE_NDS_PURCHASE_ );
    else
        price = do_document_order_price(alias, document_order, n);

    if ( nds == -1 || nds == str_nds )
        return do_real_to_money(do_document_order_quant(alias, document_order, DO_CONST_QUANT_REST) *
                           price)/100.;
    else
        return 0;
}
static void calc_sums_from_dorder(do_alias_t *alias, document_order_rec_t *document_order, do_doctype_t *doctype, double *sums)
{
    int i, *n;
    for ( i = 0; i < doctype->sums->count; i++ ) {
        n = doctype->sums->list[i];
        sums[i] += calc_sum_from_dorder(alias, document_order, *n);
    }
}


static int copy_product_param_to_order_and_cal_sum(do_alias_t *alias, document_order_rec_t *document_order, do_doctype_t *doctype, double *sums)
{
    int i, *n;
    product_key4_t product_key4;
    product_rec_t product;
    char *code;
    do_cpy(product_key4.code, document_order->data.code);
    switch ( do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            code = do_text(alias, document_order->data.code);
            do_log(LOG_ERR, "product \"%s\" not found", code);
            do_free(code);
            return FALSE;
        default:
            return FALSE;
    }
    for ( i = 0; i < doctype->price_accept_copy->count; i++ ) {
        n = doctype->price_accept_copy->list[i];
        do_document_order_price_set(alias, document_order, *n,
                                    do_product_price(alias, &product, *n));
    }
    do_document_order_param_set(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_NDS),
                                 do_product_param(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_NDS)));
    calc_sums_from_dorder(alias, document_order, doctype, sums);
    return TRUE;
}

static int accept_document_order_out_step(do_alias_t *alias, document_order_rec_t *document_order, do_accept_t accept, do_doctype_t *doctype, stock_key0_t *stock_key0, double *sums)
{
    stock_rec_t stock;
    barcode_key1_t barcode_key1;
    stock_key0_t key0;
    int stat;
    if (!stock_key0) {
        stock_key0 = (stock_key0_t*)do_malloc(sizeof(stock_key0_t));
        do_cpy(stock_key0->sklad, document_order->data.sklad);
        do_cpy(stock_key0->code, document_order->data.code);
        do_cpy(key0, *stock_key0);
        stat = do_stock_get0(alias, &stock, stock_key0, DO_GET_GT);
    }
    else {
        do_cpy(key0, *stock_key0);
        stat = do_stock_get0(alias, &stock, stock_key0, DO_GET_NEXT);
    }
    double rest, quant = do_document_order_quant(alias, document_order, doctype->rest_action[0].number);
    int i;
    while (stat == DO_OK) {
        if (do_cmp(key0.sklad, stock_key0->sklad) ||
            !do_product_is_base_equal(key0.code, stock_key0->code, sizeof(key0.code)) )
            break;

        rest = do_stock_quant(alias, &stock, doctype->rest_action[0].number);
        if (rest > 0) {
            do_cpy(barcode_key1.code, stock.data.code);
            if (do_barcode_key1(alias, &barcode_key1, DO_GET_EQUAL) == DO_KEY_NOT_FOUND) {
                if (quant > rest) {
                    do_document_order_quant_set(alias, document_order, doctype->rest_action[0].number, rest);
                    quant = rest;
                }
                if (!do_stock_quant_set(alias, &stock, doctype->rest_action[0].number, rest - quant))
                    return FALSE;

                for ( i = 1; i < 4; i++ ) { // all quant
                    if (!doctype->rest_action[i].number) break;
                    if (!doctype->rest_action[i].action == DO_REST_ACTION_NONE) continue;
                    quant = do_document_order_quant(alias, document_order, doctype->rest_action[i].number);
                    if (quant == 0) continue;
                    rest = do_stock_quant(alias, &stock, doctype->rest_action[i].number);
                    if (!do_stock_quant_set(alias, &stock, doctype->rest_action[i].number, rest+quant))
                        return FALSE;
                }
                if ( do_stock_update(alias, &stock) != DO_OK )
                    return FALSE;
                do_cpy(document_order->data.code, stock.data.code);
                return copy_product_param_to_order_and_cal_sum(alias, document_order, doctype, sums) &&
                       accept_document_order(alias, document_order, accept, doctype, 0);
            }
        }
        stat = do_stock_get0(alias, &stock, stock_key0, DO_GET_NEXT);
    }
    return copy_product_param_to_order_and_cal_sum(alias, document_order, doctype, sums) &&
           accept_document_order(alias, document_order, accept, doctype, 1);
}
DO_EXPORT int do_accept_make_sums_prowod(do_alias_t *alias, document_rec_t *document, do_doctype_t *doctype, do_extended_break_func break_func)
{
    int i, debit = 0, *n;
    subaccount_key0_t subaccount_key0;
    prowod_rec_t prowod;
    sklad_rec_t sklad;
    sklad_key0_t sklad_key0;
    char buf[50];
    char *value;
    double sum;
    char *dtype;
    char *sklad_;
    char *prowodkey;
    do_list_t *accounts;
    int res;

    accounts = do_list_new(TRUE);

    switch ( doctype->rest_action[0].action ) {
        case DO_REST_ACTION_NONE:
            return TRUE;
        case DO_REST_ACTION_IN:
            debit = 1;
            break;
        case DO_REST_ACTION_OUT:
            debit = 0;
            break;
    }
    do_cpy(sklad_key0.code, document->data.sklad);
    switch ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, document->data.sklad);
            do_log(LOG_ERR, "sklad \"%s\" not found", value);
            do_free(value);
            return FALSE;
        default:
            return FALSE;
    }

    dtype = do_text(alias, document->data.dtype);
    sklad_ = do_text(alias, document->data.sklad);
    value = do_text(alias, document->data.document);
    prowodkey = (char*)do_malloc(strlen(dtype) + strlen(sklad_) + strlen(value) + 3 + 1);
    sprintf(prowodkey, "@%s.%s/%s", value, dtype, sklad_);
    do_free(dtype);do_free(sklad_);do_free(value);
    res = TRUE;

    for ( i = 0; i < doctype->sums->count && res; i++ ) {
        if ( !RUNNING_ ) {
            res = FALSE;
            break;
        }
        n = doctype->sums->list[i];
        sprintf(buf, "%d", *n);
        do_text_set(alias, subaccount_key0.account, do_param(DO_PARAM_ACCOUNT_SUM));
        do_text_set(alias, subaccount_key0.subaccount, buf);
        subaccount_key0.itog = 0;
        switch ( do_subaccount_key0(alias, &subaccount_key0, DO_GET_EQUAL) ) {
            case DO_OK:
                break;
            case DO_KEY_NOT_FOUND:
                continue;
            default:
                res = FALSE;
                continue;
        }
        sum = do_document_sum(alias, document, *n);
        do_text_set(alias, prowod.data.division, domino_division());
        prowod.data.date = document->data.date;
        if ( debit ) {
            do_cpy(prowod.data.debit_account, subaccount_key0.account);
            do_cpy(prowod.data.debit_subaccount, subaccount_key0.subaccount);
            do_text_set_empty(prowod.data.debit_other);
            do_cpy(prowod.data.debit_p_g_code, sklad.data.p_g_code);
            prowod.data.debit_p_code = sklad.data.p_code;
            do_text_set(alias, prowod.data.credit_account, do_param(DO_PARAM_ACCOUNT_SFF));
            do_text_set_empty(prowod.data.credit_subaccount);
            do_text_set_empty(prowod.data.credit_other);
            do_text_set_empty(prowod.data.credit_p_g_code);
            prowod.data.credit_p_code = 0;
        }
        else  {
            do_cpy(prowod.data.credit_account, subaccount_key0.account);
            do_cpy(prowod.data.credit_subaccount, subaccount_key0.subaccount);
            do_text_set_empty(prowod.data.credit_other);
            do_cpy(prowod.data.credit_p_g_code, sklad.data.p_g_code);
            prowod.data.credit_p_code = sklad.data.p_code;
            do_text_set(alias, prowod.data.debit_account, do_param(DO_PARAM_ACCOUNT_SFF));
            do_text_set_empty(prowod.data.debit_subaccount);
            do_text_set_empty(prowod.data.debit_other);
            do_text_set_empty(prowod.data.debit_p_g_code);
            prowod.data.debit_p_code = 0;
        }

        do_text_set_empty(prowod.data.valuta);
        memset(&prowod.data.sums, 0, sizeof(prowod.data.sums));
        do_prowod_sum_set(alias, &prowod, 0, sum);
        do_prowod_sum_set(alias, &prowod, 1, 0);
        do_prowod_sum_set(alias, &prowod, 2, 0);
        do_text_set_empty(prowod.data.type_doc);
        prowod.data.type_oper = 0;
        do_text_set(alias, prowod.data.document, prowodkey);
        do_text_set(alias, prowod.data.pach, prowodkey + 1);
        do_prowod_params_clear(alias, &prowod);
        do_prowod_param_set(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_DOCUMENT), prowodkey);

        res = do_prowod_insert(alias, &prowod) == DO_OK;
        do_balance_key_t key;
        int err = 0;
        if (res && do_balance_get_key_from_prowod(alias, &prowod.data, &key, DO_BALANCE_SIDE_DEBIT, &err))
            do_list_add_alloc(accounts, &key, sizeof(key));
        if ( err )
            res = FALSE;
        if (res && do_balance_get_key_from_prowod(alias, &prowod.data, &key, DO_BALANCE_SIDE_CREDIT, &err))
            do_list_add_alloc(accounts, &key, sizeof(key));
        if ( err )
            res = FALSE;

    }
    do_free(prowodkey);
    if ( res ) {
        for (i = 0; i < accounts->count; i++)
            if (!do_balance_add(alias, accounts->list[i], 1, break_func)) {
                res = FALSE;
                break;
            }
    }
    return res;
}
typedef struct {
    do_alias_t *alias;
    document_rec_t *document;
} value_func_param_t;

static do_r3_var_t *value_func(const char *name, value_func_param_t *param)
{
    char *cp;
    char buf[1024];
    do_alias_t *alias = param->alias;
    sklad_rec_t sklad;
    sklad_key0_t sklad_key0;
    document_rec_t *document = param->document;
    do_r3_var_t *res;

    if ( !strncmp(name, do_param(DO_PARAM_VAR_PARAMETR), strlen(do_param(DO_PARAM_VAR_PARAMETR))) ) {
        cp = (char*)name + strlen(do_param(DO_PARAM_VAR_PARAMETR));
        cp = do_document_param(alias, document, cp);
        res = do_r3_var_string(cp);
        return res;
    }
    if ( !strncmp(name, do_param(DO_PARAM_VAR_SUM), strlen(do_param(DO_PARAM_VAR_SUM))) ) {
        cp = (char*)name + strlen(do_param(DO_PARAM_VAR_SUM));
        return do_r3_var_double(do_document_sum(alias, document, atoi(cp)));
    }
    if ( !strcmp(name, do_param(DO_PARAM_VAR_DOCUMENT_PARTNER)) ) {
        cp = do_text(alias, document->data.p_g_code);
        int v;
        v = document->data.p_code;
        sprintf(buf, "%s.%d", cp, v);
        do_free(cp);
        return do_r3_var_string(buf);
    }
    if ( !strcmp(name, do_param(DO_PARAM_VAR_SKLAD_PARTNER)) ) {
        do_cpy(sklad_key0.code, document->data.sklad);
        if ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) != DO_OK )
            return NULL;
        int v;
        v = sklad.data.p_code;
        cp = do_text(alias, sklad.data.p_g_code);
        sprintf(buf, "%s.%d", cp, v);
        do_free(cp);
        return do_r3_var_string(buf);
    }
    if ( !strcmp(name, do_param(DO_PARAM_VAR_DOCUMENT)) ) {
        char *dtype, *sklad_;
        dtype = do_text(alias, document->data.dtype);
        sklad_ = do_text(alias, document->data.sklad);
        cp = do_text(alias, document->data.document);
        sprintf(buf, "%s.%s.%s", dtype, sklad_, cp);
        do_free(cp);do_free(sklad_);do_free(dtype);
        return do_r3_var_string(buf);
    }
    if ( !strcmp(name, do_param(DO_PARAM_VAR_COMMENT)) ) {
        cp = do_document_param(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_COMMENT));
        res = do_r3_var_string(cp);
        do_free(cp);
        return res;
    }
    if ( !strcmp(name, do_param(DO_PARAM_VAR_P_DOCUMENT)) ) {
        cp = do_text(alias, document->data.p_doc);
        res = do_r3_var_string(cp);
        do_free(cp);
        return res;
    }
    if ( !strcmp(name, do_param(DO_PARAM_VAR_P_DOCUMENT_DATE_INT)) ) {
        res = do_r3_var_int(document->data.p_date);
        return res;
    }
    if ( !strcmp(name, do_param(DO_PARAM_VAR_SF_ANALIT)) ) {
        struct tm time;
        BTI_LONG date;
        int v;
        cp = do_document_param(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_INVOICE_DATE));
        do_atodate(cp, &time);
        do_free(cp);
        cp = do_document_param(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_INVOICE));
        do_date_set(&date, time);
        v = date;
        sprintf(buf,"%s;%d", cp, v);
        res = do_r3_var_string(buf);
        do_free(cp);
        return res;
    }
    if ( !strcmp(name, do_param(DO_PARAM_VAR_DOCUMENT_DATE)) ) {
        struct tm time;
        do_date(document->data.date, &time);
        cp = do_datetoa(time);
        res = do_r3_var_string(cp);
        do_free(cp);
        return res;
    }
    if ( !strcmp(name, do_param(DO_PARAM_VAR_SF_NUMBER)) ) {
        cp = do_document_param(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_INVOICE));
        res = do_r3_var_string(cp);
        do_free(cp);
        return res;
    }
    if ( !strcmp(name, do_param(DO_PARAM_VAR_SF_DATE)) ) {
        cp = do_document_param(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_INVOICE_DATE));
        res = do_r3_var_string(cp);
        do_free(cp);
        return res;
    }
    return NULL;
}
#define VAL_STR(a,b) {value = do_document_prow_param(alias, &document_prow, do_param(b)); \
                      var = do_r3_expr_calc(value, (do_r3_var_get_value_func)value_func, &param);\
                      if ( !var ) { res = FALSE; break; } \
                      do_free(value);\
                      value = do_r3_var_value_string(var);\
                      do_text_set(alias, a, value);\
                      do_r3_var_free(var);\
                      do_free(value);}

#define VAL_PARTNER(a,c,b) {value = do_document_prow_param(alias, &document_prow, do_param(b)); \
                      var = do_r3_expr_calc(value, (do_r3_var_get_value_func)value_func, &param);\
                      if ( !var ) { res = FALSE; break; } \
                      do_free(value);\
                      char *ch;\
                      value = do_r3_var_value_string(var); \
                      if ( (ch = strchr(value, '.')) != NULL ) { \
                      *ch = '\0';\
                      do_text_set(alias, a, value);\
                      c = atoi(ch + 1);\
                      }else {\
                      do_text_set_empty(a);\
                      c = 0;\
                      }\
                      do_r3_var_free(var);\
                      do_free(value);}
#define VAL_REAL(a, b) {value = do_document_prow_param(alias, &document_prow, do_param(b)); \
                      var = do_r3_expr_calc(value, (do_r3_var_get_value_func)value_func, &param);\
                      if ( !var ) { res = FALSE; break; } \
                      do_free(value);\
                      a = do_r3_var_value_double(var);\
                      do_r3_var_free(var);\
                      }



DO_EXPORT int do_accept_make_prowod(do_alias_t *alias, document_rec_t *document, do_doctype_t *doctype, do_extended_break_func break_func)
{
    int res;
    int status;
    char *value, *dtype, *sklad, *prowodkey;
    document_prow_key0_t document_prow_key0;
    document_prow_rec_t document_prow;
    prowod_rec_t prowod;
    value_func_param_t param;
    do_r3_var_t *var;
    double sum1, sum2, sum3;
    do_list_t *accounts;

    accounts = do_list_new(TRUE);
    param.alias = alias;
    param.document = document;
    res = TRUE;
    do_cpy(document_prow_key0.dtype, document->data.dtype);
    do_cpy(document_prow_key0.dtype1, document->data.dtype1);
	document_prow_key0.number = 0;

    dtype = do_text(alias, document->data.dtype);
    sklad = do_text(alias, document->data.sklad);
    value = do_text(alias, document->data.document);
    prowodkey = (char*)do_malloc(strlen(dtype) + strlen(sklad) + strlen(value) + 3 + 1);
    sprintf(prowodkey, "@%s.%s/%s", value, dtype, sklad);
    do_free(dtype);do_free(sklad);do_free(value);


	status = do_document_prow_get0(alias, &document_prow, &document_prow_key0, DO_GET_GE);
	while ( status == DO_OK ) {
	    if ( !RUNNING_ ) {
	        res = FALSE;
	        break;
	    }
	    if ( do_cmp(document_prow_key0.dtype, document->data.dtype) ||
             do_cmp(document_prow_key0.dtype1, document->data.dtype1) ) break;

        do_text_set(alias, prowod.data.division, domino_division());
        prowod.data.date = document->data.date;

        VAL_STR(prowod.data.debit_account, DO_PARAM_DPROWOD_PARAMETR_DEBIT_ACCOUNT);
        VAL_STR(prowod.data.debit_subaccount, DO_PARAM_DPROWOD_PARAMETR_DEBIT_SUB_ACCOUNT);
        VAL_PARTNER(prowod.data.debit_p_g_code, prowod.data.debit_p_code, DO_PARAM_DPROWOD_PARAMETR_DEBIT_PARTNER);
        VAL_STR(prowod.data.debit_other, DO_PARAM_DPROWOD_PARAMETR_DEBIT_ANALIT);

        VAL_STR(prowod.data.credit_account, DO_PARAM_DPROWOD_PARAMETR_CREDIT_ACCOUNT);
        VAL_STR(prowod.data.credit_subaccount, DO_PARAM_DPROWOD_PARAMETR_CREDIT_SUB_ACCOUNT);
        VAL_PARTNER(prowod.data.credit_p_g_code, prowod.data.credit_p_code, DO_PARAM_DPROWOD_PARAMETR_CREDIT_PARTNER);
        VAL_STR(prowod.data.credit_other, DO_PARAM_DPROWOD_PARAMETR_CREDIT_ANALIT);

        do_text_set_empty(prowod.data.valuta);
        memset(&prowod.data.sums, 0, sizeof(prowod.data.sums));
        VAL_REAL(sum1, DO_PARAM_DPROWOD_PARAMETR_CREDIT_SUM_1);
        do_prowod_sum_set(alias, &prowod, 0, sum1);
        VAL_REAL(sum2, DO_PARAM_DPROWOD_PARAMETR_CREDIT_SUM_2);
        do_prowod_sum_set(alias, &prowod, 1, sum2);
        VAL_REAL(sum3, DO_PARAM_DPROWOD_PARAMETR_CREDIT_SUM_3);
        do_prowod_sum_set(alias, &prowod, 2, sum3);
        do_text_set_empty(prowod.data.type_doc);
        prowod.data.type_oper = 0;
        do_text_set(alias, prowod.data.document, prowodkey);
        do_text_set(alias, prowod.data.pach, prowodkey + 1);
        do_prowod_params_clear(alias, &prowod);
        do_prowod_param_set(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_DOCUMENT), prowodkey);

        value = do_document_prow_param(alias, &document_prow, do_param(DO_PARAM_DPROWOD_PARAMETR_CREDIT_COMMENT));
        var = do_r3_expr_calc(value, (do_r3_var_get_value_func)value_func, &param);
        if ( !var ) { res = FALSE; break; }
        do_free(value);
        value = do_r3_var_value_string(var);
        do_prowod_param_set(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_COMMENT), value);
        do_r3_var_free(var);
        do_free(value);

        if ( (sum1 >= 0.009 || sum1 <= -0.009) ||
             (sum2 >= 0.009 || sum2 <= -0.009) ||
             (sum3 >= 0.009 || sum3 <= -0.009) ) {
            res = do_prowod_insert(alias, &prowod) == DO_OK;
            do_balance_key_t key;
            int err = 0;
            if ( res && do_balance_get_key_from_prowod(alias, &prowod.data, &key, DO_BALANCE_SIDE_DEBIT, &err))
                do_list_add_alloc(accounts, &key, sizeof(key));
            if ( err )
                res = FALSE;
            if ( res && do_balance_get_key_from_prowod(alias, &prowod.data, &key, DO_BALANCE_SIDE_CREDIT, &err))
                do_list_add_alloc(accounts, &key, sizeof(key));
            if ( err )
                res = FALSE;
        }

        if ( res )
            status = do_document_prow_get0(alias, &document_prow, &document_prow_key0, DO_GET_NEXT);
        if ( !res )
            break;
	}
    if ( status == DO_ERROR )
        res = FALSE;

    do_free(prowodkey);
    if ( res ) {
        int i;
        for (i = 0; i < accounts->count; i++)
            if (!do_balance_add(alias, accounts->list[i], 1, break_func)) {
                res = FALSE;
                break;
            }
    }
    return res;
}
DO_EXPORT int do_accept_order(do_alias_t *alias, document_rec_t *document, document_order_rec_t *document_order, do_accept_t accept, do_accept_param_t *param)
{
    if ( document_order->data.accepted ) {
        calc_sums_from_dorder(alias, document_order, param->dtype, param->sums);
        return TRUE;
    }

    if (do_alias_tran_begin(alias) != DO_OK) {
        return FALSE;
    }

    int result = 1;
    if (!do_product_is_parcel(document_order->data.code, sizeof(document_order->data.code)) &&
        param->dtype->auto_parcel &&
        (param->dtype->rest_action[0].number &&
         param->dtype->rest_action[0].action != DO_REST_ACTION_NONE) // just in case
        ) {
        char *code = do_text(alias, document_order->data.code);
        if (param->dtype->rest_action[0].action == DO_REST_ACTION_IN) { // create parcial
            int indx;
            product_rec_t product;
            product_key4_t product_key4;
            char *new_code = do_product_create_parcel(alias, code, param->pref);
            if (new_code == NULL)
                result = FALSE;
            if ( result ) {
                do_text_set(alias, product_key4.code, new_code);
                if ( do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) != DO_OK ) {
                    result = FALSE;
                }
            }
            if ( result ) {
                for ( indx = 0; indx < 8; indx++ )
                    if ( document_order->data.nprice[indx] )
                        do_product_price_set(alias, &product, document_order->data.nprice[indx], document_order->data.price[indx]);

                do_product_param_set(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_NDS_SUPPLIER),
                                 do_document_order_param(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_NDS_SUPPLIER)));
                do_product_param_set(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_SERIA),
                                 do_document_order_param(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_SERIA)));
                do_product_param_set(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_LIFE_TIME),
                                 do_document_order_param(alias, document_order, do_param(DO_PARAM_DORDER_PARAMETR_LIFE_TIME)));
                do_cpy(product.data.supplier_region, document->data.p_g_code);
                do_cpy(product.data.supplier_code, document->data.p_code);
                product.data.date = document->data.date;
                product.data.time = document->data.time;
                result = do_product_update(alias, &product) == DO_OK;
            }

            if ( result ) {
                do_text_set(alias, document_order->data.code, new_code);
                do_free(new_code);
            }
            if ( result )
                result = copy_product_param_to_order_and_cal_sum(alias, document_order, param->dtype, param->sums);
        }
        else { // DO_REST_ACTION_OUT
            double quant = do_document_order_quant(alias, document_order, param->dtype->rest_action[0].number);
            stock_key0_t *stock_key0 = NULL;
            document_order_rec_t *new_document_order = document_order;
            do_accept_t crnt_accept = accept;
            int line = 0;
            while (quant > 0) {
                if (!accept_document_order_out_step(alias, new_document_order, crnt_accept, param->dtype, stock_key0, param->sums)) {
                    result = FALSE;
                    break;
                }
                quant -= do_document_order_quant(alias, new_document_order, param->dtype->rest_action[0].number);
                if (quant > 0) {
                    if (new_document_order == document_order)
                        new_document_order = (document_order_rec_t*) do_malloc(sizeof(document_order_rec_t));
                    if (!line) {
                        if (accept == DO_ACCEPT_AND_INSERT)
                            line = document_order->data.line;
                        else {
                            document_key0_t document_key0;
                            do_cpy(document_key0.dtype, document_order->data.dtype);
                            do_cpy(document_key0.sklad, document_order->data.sklad);
                            do_cpy(document_key0.document, document_order->data.document);
                            line = do_document_get_last_line(alias, &document_key0);
                        }
                    }
                    do_cpy(new_document_order, document_order);
                    do_text_set(alias, new_document_order->data.code, code);
                    new_document_order->data.line = ++line;
                    do_document_order_quant_clear(alias, new_document_order);
                    do_document_order_quant_set(alias, new_document_order, param->dtype->rest_action[0].number, quant);
                    crnt_accept = DO_ACCEPT_AND_INSERT;
                }
            }
            if (stock_key0) do_free(stock_key0);
            if (new_document_order != document_order) {
                do_free(new_document_order);
                if (accept != DO_ACCEPT_AND_INSERT) { // after insert need document_order
                    document_order_key0_t document_order_key0;
                    do_cpy(document_order_key0.dtype, document_order->data.dtype);
                    do_cpy(document_order_key0.sklad, document_order->data.sklad);
                    do_cpy(document_order_key0.document, document_order->data.document);
                    document_order_key0.line = document_order->data.line;
                    result = do_document_order_get0(alias, document_order, &document_order_key0, DO_GET_EQUAL) == DO_OK;
                }
            }
        }
        do_free(code);
    }
    else
        result = copy_product_param_to_order_and_cal_sum(alias, document_order, param->dtype, param->sums);
    if (result && !document_order->data.accepted)
        result = accept_document_order(alias, document_order, accept, param->dtype, 1);
    if ( result )
        if ( !param || param->protocol )
            result = do_protocol_document_order_add_rec(alias, document_order, DO_PROTOCOL_ACTION_ACCEPT, NULL);
    if ( result )
        result = do_alias_tran_end(alias) == DO_OK;
    else
        do_alias_tran_abort(alias);
    return result;
}
DO_EXPORT int do_accept(do_alias_t *alias, document_key0_t *document_key0, do_accept_t accept, do_extended_break_func break_func)
{
    do_accept_param_t param;
    document_rec_t document;
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    int status;
    char *value;
    int res;
    int i;

    switch ( do_document_get0(alias, &document, document_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_util_document_key0_to_str(alias, document_key0);
            do_log(LOG_ERR, "document \"%s\" not found", value);
            do_free(value);
        default:
            return FALSE;
    }
    if ( document.data.accepted )
        return TRUE;

    if (do_alias_tran_begin(alias) != DO_OK)
        return FALSE;

    res = TRUE;
    char *sk;
    char *prefix;
    sk = do_text(alias, document.data.sklad);
    prefix = domino_get_unit_prefix(alias, sk);
    do_free(sk);
    if (!prefix || *prefix == '\0') {
        do_log(LOG_ERR, "local product's prefix are empty");
        if (prefix)
            do_free(prefix);
        return FALSE;

    }

    param.dtype = do_doctype_new_from_document(alias, &document);
    param.pref = do_strdup(prefix);
    do_free(prefix);
    if ( !param.dtype )
        res = FALSE;
    if ( res ) {
        param.sums = do_malloc0(sizeof(double)*param.dtype->sums->count);
        param.protocol = TRUE;

        do_cpy(document_order_key0.dtype, document.data.dtype);
        do_cpy(document_order_key0.sklad, document.data.sklad);
        do_cpy(document_order_key0.document, document.data.document);
        document_order_key0.line = 0;

        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
        while ( status == DO_OK && res ) {
            if ( !RUNNING_ ) {
                res = FALSE;
                break;
            }
            if ( do_cmp(document_order_key0.dtype, document.data.dtype) ||
                 do_cmp(document_order_key0.sklad, document.data.sklad) ||
                 do_cmp(document_order_key0.document, document.data.document) ) break;

            res = do_accept_order(alias, &document, &document_order, DO_ACCEPT_DEFAULT, &param);

            if ( !res )
                break;

            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
        }
        if ( status == DO_ERROR )
            res = FALSE;
    }
    if ( res ) {
        for ( i = 0; i < param.dtype->sums->count; i++ ) {
            int *n;
            n = param.dtype->sums->list[i];
            do_document_sum_set(alias, &document, *n, param.sums[i]);
        }
        document.data.accepted = 1;
    }
    if ( res )
        res = do_document_update(alias, &document) == DO_OK;
    if ( res )
        res = do_protocol_document_add_rec(alias, &document, DO_PROTOCOL_ACTION_ACCEPT);
    if ( res )
        res = do_accept_make_sums_prowod(alias, &document, param.dtype, NULL);
    if ( res )
        res = do_accept_make_prowod(alias, &document, param.dtype, NULL);

    if ( param.dtype ) {
        do_doctype_free(param.dtype);
        do_free(param.pref);
        do_free(param.sums);
    }
    if ( res )
        res = do_alias_tran_end(alias) == DO_OK;
    else
        do_alias_tran_abort(alias);

    return res;
}
static int do_deaccept_delete_prowod(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func)
{
    prowod_key4_t prowod_key4, prowod_key;
    prowod_rec_t prowod;
    int status, err = 0;
    char *prowodkey, *param_doc;
    char *dtype, *sklad, *document;
    do_list_t *accounts;
    int i;

    accounts = do_list_new(TRUE);

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
                do_list_add_alloc(accounts, &key, sizeof(key));
            if (!err && do_balance_get_key_from_prowod(alias, &prowod.data, &key, DO_BALANCE_SIDE_CREDIT, &err))
                do_list_add_alloc(accounts, &key, sizeof(key));
            if ( err )
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
	if (status == DO_ERROR)
        err = TRUE;

    if (!err) {
        for (i = 0; i < accounts->count; i++)
            if (!do_balance_add(alias, accounts->list[i], 0, break_func)) {
                err = TRUE;
                break;
            }
    }
    do_sort_list_free(accounts);


	return !err;
}
static int do_deaccept_document_order(do_alias_t *alias, document_order_rec_t *document_order, do_deaccept_t accept, do_doctype_t **cache, do_extended_break_func break_func)
{
    switch ( accept ) {
        case DO_DEACCEPT_DEFAULT:
            if ( !document_order->data.accepted )
                return TRUE;
            if ( *cache == NULL )
                *cache = do_doctype_new_from_document_order(alias, document_order);
            if ( *cache == NULL )
                return FALSE;
            if ( (*cache)->accept_by_line )
                return TRUE;
            break;
        case DO_DEACCEPT_AND_DELETE:
            if ( *cache == NULL )
                *cache = do_doctype_new_from_document_order(alias, document_order);
            break;
        case DO_DEACCEPT_SHORT:
            return TRUE;
    }
    if ( *cache == NULL )
        return FALSE;

    int result ;

    result = do_accept_calc_stock(alias, document_order, *cache, 0, break_func);

    if (result) {
        switch ( accept ) {
            case DO_DEACCEPT_DEFAULT:
                document_order->data.accepted = 0;
                result = do_document_order_update(alias, document_order) == DO_OK;
                break;
            case DO_DEACCEPT_AND_DELETE:
                result = do_document_order_delete(alias) == DO_OK;
                break;
            case DO_DEACCEPT_SHORT:
                break;
        }
    }
    return result;
}
DO_EXPORT int do_deaccept_order(do_alias_t *alias, document_order_rec_t *document_order, do_deaccept_t accept, do_extended_break_func break_func)
{
    if (do_alias_tran_begin(alias) != DO_OK)
        return FALSE;

    do_doctype_t *dtype = NULL;
    int result;

    result = do_deaccept_document_order(alias, document_order, accept, &dtype, break_func);
    if (result)
        result = do_protocol_document_order_add_rec(alias, document_order, DO_PROTOCOL_ACTION_DEACCEPT, NULL);
    if (result)
        result = do_alias_tran_end(alias) == DO_OK;
    else
        do_alias_tran_abort(alias);
    if (dtype)
        do_doctype_free(dtype);
    return result;
}

static int do_deaccept_orders(do_alias_t *alias, document_key0_t *document_key0, do_deaccept_t accept, do_extended_break_func break_func)
{
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    do_doctype_t *dtype = NULL;
    int status;
    int res;

    memcpy(&document_order_key0, document_key0, sizeof(document_key0_t));
    document_order_key0.line = 0;

    res = TRUE;
    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( !RUNNING_ ) {
            res = FALSE;
	        break;
        }
        if (memcmp(&document_order_key0, document_key0, sizeof(document_key0_t))) break;

        res = do_deaccept_document_order(alias, &document_order, accept, &dtype, break_func);
        if ( !res )
            break;
        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        res = FALSE;
    if ( dtype )
        do_doctype_free(dtype);

    return res;
}
DO_EXPORT int do_deaccept(do_alias_t *alias, document_key0_t *document_key0, do_deaccept_t accept, do_extended_break_func break_func)
{
    if ( !do_alias_tran_begin(alias) )
        return FALSE;

    int res;
    document_rec_t document;

    res = do_deaccept_delete_prowod(alias, document_key0, break_func);
    if ( res )
        res = do_protocol_document_add_key(alias, document_key0, DO_PROTOCOL_ACTION_DEACCEPT);
    if ( res ) {
        switch ( do_document_get0(alias, &document, document_key0, DO_GET_EQUAL) ) {
            case DO_OK:
                break;
            case DO_KEY_NOT_FOUND: {
                char *value;
                value = do_util_document_key0_to_str(alias, document_key0);
                do_log(LOG_ERR, "document \"%s\" not found", value);
                do_free(value);
            }
            default:
                res = FALSE;
        }
    }
    if ( res )
        switch ( accept ) {
            case DO_DEACCEPT_SHORT:
            case DO_DEACCEPT_DEFAULT:
                do_document_sum_clear(alias, &document);
                document.data.accepted = 0;
                res = do_document_update(alias, &document) == DO_OK;
                break;
            case DO_DEACCEPT_AND_DELETE:
                res = do_document_delete(alias) == DO_OK;
                if ( res )
                    res = do_protocol_document_add_key(alias, document_key0, DO_PROTOCOL_ACTION_DELETE);
        }
    if ( res )
        switch ( accept ) {
            case DO_DEACCEPT_SHORT:
                break;
            case DO_DEACCEPT_DEFAULT:
            case DO_DEACCEPT_AND_DELETE:
                res = do_deaccept_orders(alias, document_key0, accept, break_func);
        }
    if ( res )
        res = do_alias_tran_end(alias);
    else
        do_alias_tran_abort(alias);
    return res;
}
