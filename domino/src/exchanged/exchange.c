
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include "exchange.h"
#include <domino.h>
#include <dolib.h>
#include <artix.h>
#include "../misc/iconv_.h"
#include "../misc/mproc_state.h"
#include "option.h"
#include <time.h>
#include <unistd.h>
#define RUNNING (do_mproc_state_get_state() == DO_STATE_RUN && do_alias_conected(exchange->alias))
#define WAIT_FOR_READY_SEC 20*60
#define MAX_RECORDS_WITHOUT_SLEEP 100
#define MAX_UPDATE_RECORD 200

typedef struct PACK_STRUCT {
    BTI_CHAR barcode[DOMINO_FIELD_BCODE_SIZE];
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
} barcodes_ret;


static int start_send_handbook(exchange_t *exchange);
static int end_send_handbook(exchange_t *exchange, int flag);

static int send_products(exchange_t *exchange);
static int send_updated_products(exchange_t *exchange, do_list_t **list_updated);
static int send_cashiers(exchange_t *exchange);
static int send_keyboard(exchange_t *exchange);
static int send_barcode(do_alias_t *alias, artix_alias_t *artix, artix_session_t *session_product, artix_session_t *session_price, product_rec_t *product, barcode_key0_t *barcode_key0, int multi_price, int nretail_price, int debug_level, do_list_t **list_updated);
//static int delete_barcode(do_alias_t *alias, artix_alias_t *artix, barcode_key0_t *barcode_key0);
static int read_protocol(exchange_t *exchange, int *update_all);
static int code_is_equal(const char *code1, const char *code2, size_t len1, size_t len2);
static int check_send(artix_t *artix, do_list_t *list, int debug_level);
//static void error_message(artix_info_t *info, const char *error);

static int error_send = 0;

exchange_t *exchange_new(option_t *opt, exchange_info_t *exchange_info, do_alias_t* alias)
{
    exchange_t *exchange = (exchange_t *) do_malloc(sizeof(exchange_t));
    exchange->opt = opt;
    exchange->changed = do_string_list_new(1);
    exchange->last_product_update = 0;
    exchange->crnt_state = 0;
    exchange->last_full_update = 0;
    exchange->alias = alias;
    exchange->artix = NULL;
    exchange_info_set(exchange_info, exchange, &exchange->last_full_update, &exchange->last_product_update, &exchange->crnt_state);
    return exchange;
}

void exchange_free(exchange_t *exchange)
{
    do_string_list_free(exchange->changed);
    if (exchange->artix)
        artix_free(exchange->artix);
    do_free(exchange);
}

void exchange_cache_clear(exchange_t *exchange)
{
    exchange->last_full_update = 0;
}
void exchange_send_error(exchange_t *exchange, char *error)
{
    exchange->artix = artix_new(
                                exchange->opt->artix_host,
                                exchange->opt->artix_cs,
                                exchange->opt->artix_port,
                                exchange->opt->artix_user,
                                exchange->opt->artix_password,
                                FALSE);
    error_message(exchange->artix->info, error);
    if (exchange->artix)
        artix_free(exchange->artix);
    exchange->artix = NULL;
}
void exchange_run(exchange_t *exchange, int full_update)
{
    int res = TRUE;
    int update_all =
            (full_update || 
             (time(NULL) - exchange->last_full_update > exchange->opt->handbook_full_update_timeout));
    if ( !update_all ) {
	if ( !read_protocol(exchange, &update_all) )
            res = FALSE;
        if ( update_all ) 
    	    DO_SEC_SLEEP(180);
    }
    if ( res && (update_all || exchange->changed->count) ) {
        exchange->artix = artix_new(
                                    exchange->opt->artix_host,
                                    exchange->opt->artix_cs,
                                    exchange->opt->artix_port,
                                    exchange->opt->artix_user,
                                    exchange->opt->artix_password,
                                    FALSE);
        if ( exchange->artix ) {
#ifndef OLD_ARTIX_INFO	
    	    artix_info_clear_alert(exchange->artix->info, ARTIX_ALERT_EXCHANGE_NOT_RUN);
#endif	    
	    do_list_t *list = NULL;
            res = start_send_handbook(exchange);
            if ( res ) {
        	if ( update_all )
        	    res = send_products(exchange);
        	else
        	    res = send_updated_products(exchange, &list);
            }
            if ( res && update_all )
                res = send_cashiers(exchange);
            if ( res && update_all )
                res = send_keyboard(exchange);
            
            res = end_send_handbook(exchange, res);
#ifndef OLD_ARTIX_INFO	
	    if ( !res ) {
    		artix_info_set_alert(exchange->artix->info, ARTIX_ALERT_EXCHANGE_ERROR, NULL);
    		if (error_send < 3)
        		send_error_message_with_host_info("Error load handbook");
    		error_send++;
    	    }
	    else {
    		artix_info_clear_alert(exchange->artix->info, ARTIX_ALERT_EXCHANGE_ERROR);
    		error_send = 0;
    	    }
#endif
            if ( res && list ) {
                //DO_SEC_SLEEP(5);
        	if ( !check_send(exchange->artix, list, exchange->opt->debug_level) ) 
        	   exchange->last_full_update = 0;
    	    }
    	    if ( list )
    		do_list_free(list);

            artix_free(exchange->artix);
            exchange->artix = NULL;
            if ( res ) {
                if ( update_all )
                    exchange->last_full_update = time(NULL);
                else
                    exchange->last_product_update = time(NULL);
            }
        }
        else {
            res = FALSE;
            do_log(LOG_ERR, "Can't open artix \"%s:%d %s\"", exchange->opt->artix_host,
                                                                   exchange->opt->artix_port,
                                                                   exchange->opt->artix_cs);
        }
    }
    exchange->crnt_state = 0;
}

void error_message(artix_info_t *info, const char *error)
{
    //do_log(LOG_ERR, error);
    char **messages = NULL;
    int count_mes = 0;
    artix_info_get_upload_error_description(info, &messages, &count_mes);
    char *last = NULL;
    char *mess = NULL;
    char *str = UTF_CHARSET;
    iconv_t conv_t = iconv_open(ARTIX_CHARSET, UTF_CHARSET);
    if ( count_mes )
	last = do_strdup(messages[count_mes - 1]);
    if ( conv_t != (iconv_t) -1 )
	mess = iconv_(conv_t, error);
    else
	mess = do_strdup(error);
    if ( last && strcmp(last, mess) ) {
        artix_info_clear_upload_error_description(info);
        artix_info_add_upload_error_description(info, mess);
	artix_info_set_upload_state(info, upsError);
    }
    if ( messages )
        artix_info_list_string_free(messages, count_mes);
    if ( last ) do_free(last);
    if ( mess ) do_free(mess);
    if ( conv_t != (iconv_t) -1 ) 
	iconv_close(conv_t);
    //error_message_xmpp_with_host_info(error);
    send_error_message_with_host_info(error);
    //do_log(LOG_ERR, error);
}
inline static int is_base_and_only_parcel_send(do_alias_t *alias, product_rec_t *product)
{

    return !product->data.base_parcel &&
            (do_product_param_int(alias, product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) ==
              DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE );
           //  && (do_product_param_int(alias, product, "ADS") != 1);
}
static do_ret_list_t *get_barcodes(do_alias_t *alias)
{
    barcode_key1_t barcode_key1;
    do_extended_t *extended;
    do_ret_list_t *retval = NULL;

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_BARCODE);
    do_extended_set_fields(extended, 2,
                           do_size_and_offset(barcode_struct_t, barcode),
                           do_size_and_offset(barcode_struct_t, code));
    do_text_set_empty(barcode_key1.code);
    do_extended_set_key(extended, 1, &barcode_key1);
    retval = do_extended_get(extended, -1);
    return retval;
}

static int send_barcode_from_list(exchange_t *exchange, do_ret_list_t *list,
                                 artix_session_t *session_product, artix_session_t *session_price,
                                 do_list_t **list_updated)
{
    int result;
    //int multi_price;
    int indx;//, i;
    int status;
    int percent;
    product_rec_t product;
    product_key4_t product_key4;
    barcodes_ret *bar;
    int sleep = FALSE;
    //int base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);

    //multi_price = FALSE;
    do_text_set_empty(product_key4.code);
    do_text_set_empty(product.data.code);
    percent = 1;
    result = TRUE;
    
    sleep = (do_ret_list_count(list) > MAX_RECORDS_WITHOUT_SLEEP) && (exchange->opt->sleep_msec > 0);
    for ( indx = 0; result && indx < do_ret_list_count(list); indx++ ) {
        if (!RUNNING) { result = FALSE; break;}
        bar = do_ret_list_item_data(list, indx);

        /*if (strncmp(product_key4.code, bar->code, base_len)) {
            multi_price = FALSE;
            do_cpy(product_key4.code, bar->code);

            for (i = indx; i < do_ret_list_count(list); i++) {
                if (!RUNNING) break;
                bar = do_ret_list_item_data(list, i);
                if (strncmp(product_key4.code, bar->code, base_len)) break;
                if (do_product_is_parcel(bar->code, sizeof(bar->code))) {
                    multi_price = TRUE;
                    break;
                }
            }
            bar = do_ret_list_item_data(list, indx);
        }
        else*/
            do_cpy(product_key4.code, bar->code);

        if (!RUNNING) { result = FALSE; break;}
        if (do_cmp(product.data.code, product_key4.code)) {
            status = do_product_get4(exchange->alias, &product, &product_key4, DO_GET_EQUAL);
            if (status == DO_ERROR) {
                error_message(exchange->artix->info, "База данных недоступна");
                result = 0;
                break;
            };
        }

        if ( (status == DO_OK) &&
             (do_product_price(exchange->alias, &product, exchange->opt->nretail_price) > 0 ||
              !strncmp(product.data.code,"00001",5) )
              &&
             !is_base_and_only_parcel_send(exchange->alias, &product) ) 
            result = send_barcode(exchange->alias,
                                  exchange->artix->alias,
                                  session_product,
                                  session_price,
                                  &product,
                                  (barcode_key0_t*)&bar->barcode,
                                  FALSE,
                                  //multi_price,
                                  exchange->opt->nretail_price,
                                  exchange->opt->debug_level,
                                  list_updated);

        if (percent + 1 < (indx * 100.0 / do_ret_list_count(list))) {
            percent =  indx * 100.0 / do_ret_list_count(list) + 1;
            artix_info_set_upload_progress(exchange->artix->info, percent);
        }
        if ( sleep )
    	    DO_SEC_SLEEP((exchange->opt->sleep_msec)/1000.);
        //if ( sleep ) //&& indx % SLEEP_AFTER_RECORD == 0 )
        //	usleep(1);
    }
    return result;
}

static int send_products(exchange_t *exchange)
{
    artix_measure_t measure;
    artix_session_t *session_product = NULL;
    artix_session_t *session_price = NULL;
    artix_session_t *session_measure = NULL;
    protocol_rec_t protocol;
    do_ret_list_t *list = NULL;
    int result = 0;
    int indx;

    exchange->crnt_state = 1;

    if (do_protocol_step(exchange->alias, &protocol, DO_STEP_LAST) == DO_ERROR) {
        error_message(exchange->artix->info, "База данных недоступна");
        return 0;
    }

    list = get_barcodes(exchange->alias);
    if (!list) {
        error_message(exchange->artix->info, "База данных недоступна");
        return 0;
    }

    if ((session_product = artix_session_new(exchange->artix->alias, ARTIX_OBJECT_PRODUCT, ARTIX_ACTION_REPLACE)) == NULL)
        error_message(exchange->artix->info, "Невозможно открыть товарную сессию");
    else
    if ((session_price = artix_session_new(exchange->artix->alias, ARTIX_OBJECT_PRICE, ARTIX_ACTION_REPLACE)) == NULL)
        error_message(exchange->artix->info, "Невозможно открыть сессию для цен");
    else
    if ((session_measure = artix_session_new(exchange->artix->alias, ARTIX_OBJECT_MEASURE, ARTIX_ACTION_REPLACE)) == NULL)
        error_message(exchange->artix->info, "Невозможно открыть сессию для еи");
    else
        result = 1;

    for (indx = 0; result && indx < do_unit_measure_count(); indx++) {
        if (!RUNNING) { result = 0; break;}

        measure.code = indx;
        artix_text_set(exchange->artix->alias, measure.name, do_unit_measure_name(indx));
        measure.flag = 0;
        result = artix_session_add_measure(session_measure, &measure);
    }

    if (result)
        result = send_barcode_from_list(exchange, list, session_product, session_price, NULL);

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
    if (list)
        do_ret_list_free(list);
    return result;
}

static int send_updated_products(exchange_t *exchange, do_list_t **list_updated)
{
    int indx;
    int result;
    int status;
    artix_session_t *session_product = NULL;
    artix_session_t *session_price = NULL;
    barcode_rec_t barcode;
    barcode_key1_t barcode_key1, barcode_key;
    do_ret_list_t *list;
    int base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);
    barcodes_ret bar;

    result = 0;
    exchange->crnt_state = 2;
    list = do_ret_list_static_new();

    if ((session_product = artix_session_new(exchange->artix->alias, ARTIX_OBJECT_PRODUCT, ARTIX_ACTION_ADD)) == NULL)
        error_message(exchange->artix->info, "Невозможно открыть сессию для товаров");
    else
    if ((session_price = artix_session_new(exchange->artix->alias, ARTIX_OBJECT_PRICE, ARTIX_ACTION_ADD)) == NULL)
        error_message(exchange->artix->info, "Невозможно открыть сессию для цен");
    else
        result = 1;

    if (result) {
        for (indx = 0; indx < exchange->changed->count; indx++) {
            if (!RUNNING) { result = 0; break;}
            do_text_set(exchange->alias, barcode_key.code, exchange->changed->list[indx]);
            do_cpy(barcode_key1, barcode_key);

            status = do_barcode_get1(exchange->alias, &barcode, &barcode_key1, DO_GET_GE);
            while (status == DO_OK) {
                if (strncmp(barcode.data.code, barcode_key.code, base_len)) break;
                do_cpy(bar.barcode, barcode.data.barcode);
                do_cpy(bar.code, barcode.data.code);
                do_ret_list_static_add(list, &bar, sizeof(bar));

                status = do_barcode_get1(exchange->alias, &barcode, &barcode_key1, DO_GET_NEXT);
            }
            if (status == DO_ERROR) {
                error_message(exchange->artix->info, "База данных недоступна");
                result = 0;
                break;
            };
        }
    }
    if (result)
        result = send_barcode_from_list(exchange, list, session_product, session_price, list_updated);
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
    if (list)
        do_ret_list_free(list);
    return result;
}

static int send_barcode(do_alias_t *alias, artix_alias_t *artix, artix_session_t *session_product, artix_session_t *session_price, product_rec_t *product, 
                        barcode_key0_t *barcode_key0, int multi_price, int nretail_price, int debug_level, do_list_t **list_updated)
{
    artix_product_t artix_product;
    artix_price_t artix_price;

    artix_product.price = do_product_price(alias, product, nretail_price);
    
    if ( artix_product.price <= 0 && strncmp(product->data.code,"00001",5) )  return 1;

    int result = 1;
    char *value;
    int  measure;
    double coef;

    
    value = do_text(alias, product->data.code);
    artix_text_set(artix, artix_product.code, value);
    do_free(value);

    value = do_text(alias, product->data.name);
    artix_text_set(artix, artix_product.name, value);
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

    if ( multi_price &&
         do_product_param_int(alias, product, do_param(DO_PARAM_PRODUCT_PARAMETR_NO_QUANT_LIMIT)) != 1 )
        artix_product.quant_limit = 1;
    else
        artix_product.quant_limit = 0;

    /*if ( (   &&
         (product->data.code[5] != '.') )
         artix_product.price = 999999;*/

    if (coef > 1.001 &&
        code_is_equal(barcode_key0->barcode, product->data.code,
                      sizeof(barcode_key0->barcode), sizeof(product->data.code))
        )
        artix_product.coef = 1;
    else
        artix_product.coef = coef;

    value = do_text(alias, barcode_key0->barcode);
    artix_text_set(artix, artix_product.bcode, value);
    do_free(value);
    if (artix_product.coef > 1.001) {
        artix_product.measure = measure;
        memcpy(&artix_price.bcode, artix_product.bcode, sizeof(artix_price.bcode));
        artix_price.price = artix_product.price;
        artix_price.indx = ARTIX_UNPACK_PRICE_INDEX;
        result = artix_session_add_price(session_price, &artix_price);
        //do_log(LOG_INFO,"%s %f\n",artix_text(artix,artix_price.bcode), artix_product.price);//!!
        artix_product.price *= artix_product.coef;
    }
    else
        artix_product.measure = artix_product.unpack_measure;
    if ( debug_level > 100 ) {
        char *s;
        s = do_text(alias, barcode_key0->barcode);
        value = do_text(alias, product->data.code);
	do_log(LOG_DEBUG, "ADD_TMC_ITEM(\"%s\",\"%s\",%.2f)", value, s, artix_product.price);
	do_free(value);do_free(s);
    }        
    if ( list_updated ) {
	if ( *list_updated == NULL )
	    *list_updated = do_list_new(TRUE);
	do_list_add_alloc(*list_updated, &artix_product, sizeof(artix_product));
    }
    
    if ( !result )
	return result;
    if ( do_product_param_int(alias, product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) ==
              DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE) { 
       	value = do_text(alias, product->data.code);
        artix_text_set(artix, artix_product.code, value);
        do_free(value);
        value = do_text(alias, product->data.code);
        artix_text_set(artix, artix_product.bcode, value);
        do_free(value);
        artix_product.coef = 1;
        artix_product.price = do_product_price(alias, product, nretail_price);
        result = artix_session_add_product(session_product, &artix_product);
        value = do_text(alias, barcode_key0->barcode);
        artix_text_set(artix, artix_product.bcode, value);
        do_free(value);
        artix_product.coef = coef;
	artix_product.price *= coef;    
    }
    else {
        value = do_text(alias, product->data.code);
        value[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)]='\0';
        artix_text_set(artix, artix_product.code, value);
    }
    value = do_text(alias, barcode_key0->barcode);
    artix_text_set(artix, artix_product.bcode, value);
    do_free(value);
    result = artix_session_add_product(session_product, &artix_product);
    //}
    //else  //!! End ADS Fuck
//	result = artix_session_add_product(session_product, &artix_product);

    return result;
}
/*
static int delete_barcode(do_alias_t *alias, artix_alias_t *artix, barcode_key0_t *barcode_key0)
{
#ifdef ARTIX_DELETE_PRODUCT_PROCEDURE
    char *value;
    int result;

    value = do_text(alias, barcode_key0->barcode);
    result = artix_alias_delete_product(artix, value);
    do_free(value);

    return result;
#else
    return 0;
#endif
}
*/
static int code_is_equal(const char *code1, const char *code2, size_t len1, size_t len2)
{
    if (memcmp(code1, code2, (len1 < len2) ? len1 : len2))
        return 0;
    return (len1 == len2) ||
         ((len1 < len2) ? (code2[len1] == ' ') : (code1[len2] == ' '));
}
static int send_cashiers(exchange_t *exchange)
{
    int status, result = 0;
    artix_session_t *session_cashier = NULL;
    do_alias_t *alias = exchange->alias;

    exchange->crnt_state = 3;

    if ((session_cashier = artix_session_new(exchange->artix->alias, ARTIX_OBJECT_CASHIER, ARTIX_ACTION_REPLACE)) == NULL)
        error_message(exchange->artix->info, "Невозможно открыть сессию для кассиров");
    else
        result = 1;

    if (result) {
        int count = 0;
        partner_rec_t partner;
        partner_key0_t partner_key0, partner_key;
        artix_cashier_t artix_cashier;
        do_text_set(exchange->alias, partner_key0.g_code, do_param(DO_PARAM_PARTNER_REGION_CASHIER));
        memcpy(&partner_key, &partner_key0, sizeof(partner_key0.g_code));
        partner_key0.code = 0;
        status = do_partner_get0(exchange->alias, &partner, &partner_key0, DO_GET_GE);
        char *value, *unit;
        while (status == DO_OK && result) {
            if (!RUNNING) {
                result = 0;
                break;
            }
            if ( do_cmp(partner_key.g_code, partner_key0.g_code) ) break;
            count++;
            unit = do_text(alias, partner.data.sklad);
            if ( !unit[0] || !strcmp(unit, domino_unit()) ) {
        	value = (char*)do_malloc(100);
        	sprintf(value, "%d", (int)partner.data.code);
        	artix_text_set(exchange->artix->alias, artix_cashier.code, value);
        	do_free(value);
        	value = do_text(exchange->alias, partner.data.name);
        	artix_text_set(exchange->artix->alias, artix_cashier.name, value);
        	do_free(value);
        	value = do_partner_param(exchange->alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_CASHIER_LOGIN));
        	artix_text_set(exchange->artix->alias, artix_cashier.login, value);
        	do_free(value);
        	value = do_partner_param(exchange->alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_CASHIER_PASSWORD));
        	artix_text_set(exchange->artix->alias, artix_cashier.passwd, value);
        	do_free(value);
        	artix_cashier.role = do_partner_param_int(exchange->alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_CASHIER_ROLE));
                result = artix_session_add_cashier(session_cashier, &artix_cashier);
	    }
	    do_free(unit);
	    if ( !result )
		break;
            status = do_partner_get0(exchange->alias, &partner, &partner_key0, DO_GET_NEXT);
        }
        if (status == DO_ERROR) {
            error_message(exchange->artix->info, "База данных недоступна");
            result = 0;
        }
        if (result && !count) {
            result = 0;
            error_message(exchange->artix->info, "Список кассиров пуст");
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
static int send_keyboard(exchange_t *exchange)
{
    int status, result = 0;
    //int i,j,f;
    artix_session_t *session_keyboard = NULL;
    artix_t *artix;
    artix=exchange->artix;
    do_alias_t *alias = exchange->alias;
    const char *sklad = domino_unit();

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


static int start_send_handbook(exchange_t *exchange)
{
    artix_info_upload_state_t state = upsUndefine;
    int i;
    for (i = 0; i < WAIT_FOR_READY_SEC && state != upsNone; i++) {
	exchange->crnt_state = 5;
        if (!RUNNING) break;
        if (!artix_info_get_upload_state(exchange->artix->info, &state)) {
            do_log(LOG_ERR, "Cannot get artix state");
            return 0;
        }
        if (state != upsNone)
            DO_SEC_SLEEP(1);
    }
    if (state != upsNone) {
        error_message(exchange->artix->info, "Время ожидания открытия сессии истекло");
        return 0;
    }
    if (!artix_info_start_upload(exchange->artix->info)) {
        do_log(LOG_ERR, "Cannot start artix upload");
        error_message(exchange->artix->info, "Невозможно начать загрузку");
        return 0;
    }

    if (!artix_multi_session_start(exchange->artix->alias)) {
        error_message(exchange->artix->info, "Невозможно начать транзакцию");
        return 0;
    }
    if ( exchange->opt->debug_level > 100 ) 
       do_log(LOG_DEBUG, "START UPLOAD SESSION");

    artix_info_set_upload_progress(exchange->artix->info, 1); // just in case
    return 1;
}
static int read_protocol(exchange_t *exchange, int *update_all)
{
    do_string_list_clear(exchange->changed);
    protocol_rec_t protocol;
    int indx;
    int status;
    const char *sep = do_param(DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR);
    status = do_protocol_step(exchange->alias, &protocol, DO_STEP_NEXT);
    char *object, *ch, *tail;
    const char *product_obj = do_param(DO_PARAM_PROTOCOL_OBJECT_PRODUCT);
    while (status == DO_OK) {
        object = do_text(exchange->alias, protocol.data.objectName);
        if (!strncmp(object, product_obj, strlen(product_obj))) {
            ch = object + strlen(product_obj);
            if (*ch == '.') {
                ch++;
                tail = ch;
                while (*tail != '\0' && *tail != sep[0]) tail++;
                *tail = '\0';
                if ( *ch != '\0' &&
                     !do_string_list_find(exchange->changed, &indx, ch) ) {
                    do_string_list_add_alloc(exchange->changed, ch);
                    if ( exchange->changed->count > MAX_UPDATE_RECORD ) {
                	*update_all = TRUE;
                	do_string_list_clear(exchange->changed);
                	break;
                    }
                    if ( exchange->opt->debug_level > 100 ) {
                	do_log(LOG_DEBUG, "need update \"%s\"", ch);
                    }
                }
            }
        }
        do_free(object);
        status = do_protocol_step(exchange->alias, &protocol, DO_STEP_NEXT);
    }
    if (status == DO_ERROR) {
        error_message(exchange->artix->info, "База данных недоступна");
        return 0;
    }
    return 1;
}

static int end_send_handbook(exchange_t *exchange, int flag)
{
    int result = flag;

    if (result) {
        result = artix_multi_session_commit(exchange->artix->alias);
        if ( exchange->opt->debug_level > 100 ) 
          do_log(LOG_DEBUG, "END UPLOAD SESSION");
    }
    else {
        artix_multi_session_abort(exchange->artix->alias);
        if ( exchange->opt->debug_level > 100 ) 
          do_log(LOG_DEBUG, "ABORT UPLOAD SESSION");
    }

    artix_info_set_upload_progress(exchange->artix->info, 0); // just in case
    if (!result)
        artix_info_set_upload_state(exchange->artix->info, upsNone);
    else {
        artix_info_success_upload(exchange->artix->info);
    }
    if ( exchange->opt->debug_level > 100 ) 
       do_log(LOG_DEBUG, "SEND END UPLOAD FLAG: %s", result ? "OK":"ERROR!!");
    return result;
}
static int check_send(artix_t *artix, do_list_t *list, int debug_level)
{
    artix_product_t *product;
    int i;
    for ( i = 0; i < list->count; i++ ) {
	product = list->list[i];
	if ( debug_level > 100 ) {
	    char *value;
    	    value = artix_text(artix->alias, product->code);
	    do_log(LOG_DEBUG, "check product in tmc \"%s\"", value);
	    do_free(value);
	}
	if ( !artix_check_product(artix->alias, product) ) {
    	    char errbuf[1024], *value, *bar;
    	    value = artix_text(artix->alias, product->code);
    	    bar = artix_text(artix->alias, product->bcode);
    	    snprintf(errbuf, 1023, "Товар \"%s\" \"%s\" не передан", value, bar);
            error_message(artix->info, errbuf);
            do_log(LOG_ERR, "Product %s %s not found in tmc", value, bar);
            do_free(value);do_free(bar);
	    return FALSE;
	}
    }
    return TRUE;
}
