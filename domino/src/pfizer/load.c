#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolib.h>
#include <domino.h>
#include <errno.h>
#include <locale.h>

#include "read_files.h"
#include "pfizer-const.h"


static int load_rules(const char *alias_name, pfizer_rules_t *rules);
static int load_stop_list(const char *alias_name, pfizer_rules_t *rules);

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();
    pfizer_rules_t *rules;
    char *filename;

    if ( argc < 3 ) {
        fprintf(stderr, "usage %s <filename> <alias> [<alias>..]\n", argv[0]);
        exit(1);
    }
    filename = argv[1];

    rules =  pfizer_rules_new_from_file(filename);

    if ( !rules )
        exit(1);
    if ( !domino_init(NULL, FALSE, DO_PARAM_END) )
        exit(1);
    if ( rules->type == PFIZER_TYPE_BUSINESS_RULES ) {
        char *date_str=do_strdup(asctime(&rules->date));
        do_log(LOG_INFO, "load business rules tc %d ver %s date %s\n", rules->territory_code, rules->version, date_str);
        do_free(date_str);
        int i;
        for ( i = 2; i < argc; i++ )
            if ( !load_rules(argv[i], rules) )
                exit(1);
    }
    if ( rules->type == PFIZER_TYPE_STOP_LIST ) {
        char *date_str=do_strdup(asctime(&rules->date));
        do_log(LOG_INFO, "load stop list ver %s date %s\n", rules->version, date_str);
        do_free(date_str);
        int i;
        for ( i = 2; i < argc; i++ ) {
            if ( !load_stop_list(argv[i], rules) )
                exit(1);
            break;
        }
    }
    return 0;
}
static int clear_rules(do_alias_t *alias);
//static int clear_stop_list(do_alias_t *alias);
static int clear_stop_list(do_alias_t *alias, do_list_t *list);
static int insert_sk(do_alias_t *alias, pfizer_rule_t *rule);
static int insert_sk_line(do_alias_t *alias, pfizer_rule_t *rule, pfizer_line_t *line);
static int load_rules(const char *alias_name, pfizer_rules_t *rules)
{
    do_alias_t *alias;

    alias = domino_alias_new(alias_name);
    if ( !alias )
        return FALSE;

    if ( !do_alias_open(alias, TRUE) ) {
        do_alias_free(alias);
        return FALSE;
    }
    if ( !do_alias_tran_begin(alias) )  {
        do_alias_free(alias);
        return FALSE;
    }

    int retval;
    retval = TRUE;
    int i,j;

    if ( retval )
        retval = clear_rules(alias);

    for ( i = 0; retval && i < rules->rules->count; i++ ) {
        pfizer_rule_t *rule;
        rule = rules->rules->list[i];

        do_log(LOG_INFO, "add rule %d %s %d %d", rule->id, rule->desc, rule->start, rule->end);
        retval = insert_sk(alias, rule);
        for ( j = 0; retval && j < rule->lines->count; j++ ) {
            pfizer_line_t *line;
            line = rule->lines->list[j];
            //do_log(LOG_INFO,"\t add product %d %s %s %s %d", line->id, asctime(&line->time1), line->barcode, line->desc, line->discount);
            retval = insert_sk_line(alias, rule, line);
        }
    }
    if ( retval )
        retval = do_alias_tran_end(alias);
    else
        do_alias_tran_abort(alias);

    do_alias_free(alias);

    return retval;
}
static int clear_rules(do_alias_t *alias)
{
    document_key0_t document_key0, key;
    document_order_key0_t document_order_key0;
    document_rec_t document;
    document_order_rec_t document_order;
    int status;
    char buf[256];

    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_DISCOUNT));
    do_text_set(alias, document_key0.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    sprintf(buf, "%d", PFIZER_SK);
    do_text_set(alias, document_key0.document, buf);
    do_cpy(key.dtype, document_key0.dtype);
    do_cpy(key.sklad, document_key0.sklad);
    sprintf(buf,"%d", PFIZER_SK_MAX);
    do_text_set(alias, key.document, buf);

    status = do_document_get0(alias, &document, &document_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.dtype, document_key0.dtype) ||
             do_cmp(key.sklad, document_key0.sklad) ||
             do_cmp(key.document, document_key0.document) < 0 ) break;
        do_cpy(document_order_key0.dtype, document_key0.dtype);
        do_cpy(document_order_key0.sklad, document_key0.sklad);
        do_cpy(document_order_key0.document, document_key0.document);
        document_order_key0.line = 0;
        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
        while ( status == DO_OK ) {
            if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                 do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                 do_cmp(document_order_key0.document, document_key0.document) ) break;
            if ( do_document_order_delete(alias) != DO_OK )
                return FALSE;
            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
        }
        if ( status == DO_ERROR )
            return FALSE;

        if ( do_document_delete(alias) != DO_OK )
            return FALSE;

        if ( !do_protocol_document_add_key(alias, &document_key0, DO_PROTOCOL_ACTION_DELETE) )
            return FALSE;

        status = do_document_get0(alias, &document, &document_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;
    return TRUE;
}
static int insert_sk(do_alias_t *alias, pfizer_rule_t *rule)
{
    document_rec_t document;
    char buf[1256];

    sprintf(buf,"%d", PFIZER_SK+rule->id);

    do_text_set(alias, document.data.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_DISCOUNT));
    do_text_set(alias, document.data.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    do_text_set(alias, document.data.document, buf);

    do_text_set(alias, document.data.dtype1, "1");
    do_text_set_empty(document.data.name);
    document.data.p_date=0;
    document.data.p_time=0;
    do_text_set_empty(document.data.party);
    document.data.accepted=0;
    do_document_params_clear(alias, &document);

    document.data.date = rule->start;
    document.data.time = rule->end;
    do_text_set_empty(document.data.p_g_code);
    document.data.p_code = 0;
    do_text_set_empty(document.data.p_doc);
    do_document_param_int_set(alias, &document, "ОГРАНИЧ_ПЕРИОД",1);
    do_document_param_int_set(alias, &document, "ВИД",0);
    do_document_param_int_set(alias, &document, "СК_НАК",0);
    do_document_param_float_set(alias, &document, "ПРОЦЕНТ",0);
    do_document_param_set(alias, &document, "37",rule->desc);
    do_document_param_int_set(alias, &document, "ПЕРИОД",0);
    do_document_param_int_set(alias, &document, "ОГРАНИЧ_СУММ",0);
    do_document_param_int_set(alias, &document, "СУММА_ОТ",0);
    do_document_param_int_set(alias, &document, "СУММА_ДО",0);
    do_document_param_set(alias, &document, "409", rule->desc);
    do_document_param_int_set(alias, &document, "LOSS",1);


    if ( do_document_insert(alias, &document) != DO_OK )
        return FALSE;

    if ( !do_protocol_document_add_rec(alias, &document, DO_PROTOCOL_ACTION_INSERT) )
        return FALSE;

    return TRUE;

}
static int insert_sk_line(do_alias_t *alias, pfizer_rule_t *rule, pfizer_line_t *line)
{

    barcode_key0_t barcode_key0;
    barcode_rec_t barcode;
    product_rec_t  product;
    product_key4_t product_key4;
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    int status;
    double coef;

    do_text_set(alias, barcode_key0.barcode, line->barcode);
    status = do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return FALSE;
    if ( status == DO_KEY_NOT_FOUND ) {
        do_log (LOG_WARNING, "barcode '%s' не найден для товара '%s'", line->barcode, line->desc);
        return TRUE;
    }
    do_cpy(product_key4.code, barcode.data.code);
    status = do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return FALSE;
    if ( status == DO_KEY_NOT_FOUND ) {
        char *code;
        code = do_text(alias,product_key4.code);
        do_log (LOG_WARNING, "product '%s' не найден для товара '%s'", code, line->desc);
        do_free(code);
        return TRUE;
    }
    coef = do_product_coef(alias, &product);


    char buf[256], *v;

    sprintf(buf,"%d", PFIZER_SK+rule->id);

    do_text_set(alias, document_order.data.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_DISCOUNT));
    do_text_set(alias, document_order.data.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    do_text_set(alias, document_order.data.document, buf);
    do_text_set(alias, document_order.data.dtype1, "1");

    document_order.data.sort=0;
    document_order.data.accepted = 0;
    do_document_order_params_clear(alias, &document_order);
    do_document_order_quant_clear(alias, &document_order);
    do_document_order_price_clear(alias, &document_order);

    document_order.data.line=line->id;
    if ( line->time1.tm_year < 200 && line->time1.tm_year > 100 )
        do_date_set(&document_order.data.date, line->time1);
    else
        document_order.data.date=0;
    if ( line->time2.tm_year < 200 && line->time2.tm_year > 100 )
        do_date_set(&document_order.data.date, line->time2);
    else
        document_order.data.time=0;
    do_cpy(document_order.data.code, barcode.data.code);
    if ( line->discount > 0 ) {
        do_document_order_price_set(alias, &document_order, 2, line->discount);
        do_document_order_param_float_set(alias, &document_order, "MAX_DISCOUNT",line->maxdiscount);
        do_document_order_param_int_set(alias, &document_order, "PROC_SUM",0);
    }
    else {
        do_document_order_price_set(alias, &document_order, 4, line->maxdiscount);
        //do_document_order_param_float_set(alias, &document_order, "MAX_DISCOUNT",line->maxdiscount);
        do_document_order_param_int_set(alias, &document_order, "PROC_SUM",1);
    }
    if ( line->maxquant > 0 )
        do_document_order_quant_set(alias, &document_order, 2, line->maxquant*coef);
    do_document_order_param_set(alias, &document_order, "BAR", line->barcode);
    do_document_order_param_set(alias, &document_order, "DESC", line->desc);
    do_document_order_param_int_set(alias, &document_order, "PEREOPRED",1);
    do_document_order_param_int_set(alias, &document_order, "NO_UNPACK",1);
    do_document_order_param_int_set(alias, &document_order, "PEREOPRED",1);
    do_document_order_param_int_set(alias, &document_order, "NO_UNPACK",1);
    
    if ( line->time1.tm_year < 200 && line->time1.tm_year > 100 ) {
        v = do_datetoa(line->time1);
        do_document_order_param_set(alias, &document_order, "DATE1", v);
        do_free(v);
    }
    if ( line->time2.tm_year < 200 && line->time2.tm_year > 100 ) {
        v = do_datetoa(line->time2);
        do_document_order_param_set(alias, &document_order, "DATE2", v);
        do_free(v);
    }
    status = do_document_order_insert(alias, &document_order);
    if ( status == DO_ERROR )
            return FALSE;

    return TRUE;
}

static int load_stop_list(const char *alias_name, pfizer_rules_t *rules)
{
    do_alias_t *alias;

    alias = domino_alias_new(alias_name);
    if ( !alias )
        return FALSE;

    if ( !do_alias_open(alias, TRUE) ) {
        do_alias_free(alias);
        return FALSE;
    }
    if ( !do_alias_tran_begin(alias) )  {
        do_alias_free(alias);
        return FALSE;
    }

    int retval;
    retval = TRUE;
    int i;
    char *v;

    if ( retval ) {
        //retval = clear_stop_list(alias);
        retval = clear_stop_list(alias, rules->rules);
    }
    do_log(LOG_INFO,"count blocks %d", rules->rules->count);
    for ( i = 0; retval && i < rules->rules->count; i++ ) {
        pfizer_rule_t *rule;
        partner_rec_t partner;
        rule = rules->rules->list[i];
        if ( rule->card ) {
        do_log(LOG_INFO, "add card %d", rule->card);
        do_text_set(alias, partner.data.g_code, "SKB");
        partner.data.code = rule->card;
        partner.data.type = 7;
        do_text_set(alias, partner.data.name, "ЯЯЯ Pfizer card block");
        do_partner_params_clear(alias, &partner);

        if ( rule->time1.tm_year < 200 && rule->time1.tm_year > 100 ) {
            v = do_datetoa(rule->time1);
            do_partner_param_set(alias, &partner, "ST_DATE", v);
            do_free(v);
        }

        if ( rule->time2.tm_year < 200 && rule->time2.tm_year > 100 ) {
            v = do_datetoa(rule->time2);
            do_partner_param_set(alias, &partner, "RE_DATE", v);
            do_free(v);
        }
        if ( do_partner_insert(alias, &partner) != DO_OK )
            return FALSE;
        if ( !do_protocol_partner_add_rec(alias, &partner, DO_PROTOCOL_ACTION_INSERT) )
            return FALSE;
        }
    }
    if ( retval )
        retval = do_alias_tran_end(alias);
    else
        do_alias_tran_abort(alias);

    do_alias_free(alias);

    return retval;
}
/*
static int clear_stop_list(do_alias_t *alias)
{
    partner_key0_t partner_key0, key;
    partner_rec_t partner;
    int status;

    do_text_set(alias, partner_key0.g_code, "SKB");
    partner_key0.code = PFIZER_SKB;
    do_cpy(key.g_code, partner_key0.g_code);
    key.code = PFIZER_SKB_MAX;

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.g_code, partner_key0.g_code) ||
              key.code < partner_key0.code ) break;

        if ( do_partner_delete(alias) != DO_OK )
            return FALSE;

        if ( !do_protocol_partner_add_rec(alias, &partner, DO_PROTOCOL_ACTION_DELETE) )
            return FALSE;

        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;


    return TRUE;
}
*/
static int clear_stop_list(do_alias_t *alias, do_list_t *list)
{
    partner_key0_t partner_key0, key;
    partner_rec_t partner;
    int status,i,flag;
    char *value,*v;

    do_text_set(alias, partner_key0.g_code, "SKB");
    partner_key0.code = PFIZER_SKB;
    do_cpy(key.g_code, partner_key0.g_code);
    key.code = PFIZER_SKB_MAX;
    do_log(LOG_INFO, "start %d stop %d",partner_key0.code,key.code);

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.g_code, partner_key0.g_code) ||
              key.code < partner_key0.code ) break;
        flag = TRUE;
        for ( i = 0; i < list->count; i++ ) {
            pfizer_rule_t *rule;
            rule = list->list[i];
            if ( rule->card == partner.data.code ) {
               flag = FALSE;
               value = do_partner_param(alias, &partner, "ST_DATE");
               if ( rule->time1.tm_year < 200 && rule->time1.tm_year > 100 ) {
                   v = do_datetoa(rule->time1);
                   if ( strcmp(v, value) )
                       flag = TRUE;
                   do_free(v);
               }
               else
                   flag = value[0] != '\0';
               do_free(value);
               if ( !flag ) {
                   value = do_partner_param(alias, &partner, "RE_DATE");
                   if ( rule->time2.tm_year < 200 && rule->time2.tm_year > 100 ) {
                       v = do_datetoa(rule->time2);
                       if ( strcmp(v, value) )
                           flag = TRUE;
                       do_free(v);
                   }
                   else 
                       flag = value[0] != '\0';
               }
               if ( !flag )
                   rule->card = 0;
               break;
            }
        }
        if ( flag && !do_partner_param_int(alias, &partner, "MANUL") ) {
            do_log(LOG_INFO, "delete %d stop %d",partner_key0.code,key.code);
            if ( do_partner_delete(alias) != DO_OK )
                return FALSE;
            if ( !do_protocol_partner_add_rec(alias, &partner, DO_PROTOCOL_ACTION_DELETE) )
               return FALSE;
        }

        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;


    return TRUE;
}
