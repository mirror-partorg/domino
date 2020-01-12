#include <domino.h>
#include <stdarg.h>
#include <math.h>
#include "../misc/define.h"

typedef enum {
   MARKUP_KIND_ALL=0,
   MARKUP_KIND_SUPPLIER,
   MARKUP_KIND_GROUP,
   MARKUP_KIND_SUBGROUP,
   MARKUP_KIND_PRODUCT,
} markup_kind_t;


static markup_info_t *get_from_document(do_alias_t *alias, document_rec_t *document);
static int get_base_product(do_alias_t *alias, const char *code, product_rec_t *product);
DO_EXPORT markup_info_t *do_markup_get(do_alias_t *alias, const char *unit, const char *code, const char *partner_region, int partner_code)
{
    document_key0_t       document_key0, key;
    document_rec_t        document;
    document_order_key2_t document_order_key2;
    document_order_rec_t  document_order; 
    product_rec_t  product;
    int status, class_, product_class, product_nc;
    char *value;
    char *value1;
    char *value2;

    if ( !get_base_product(alias, code, &product) )
        return NULL;

    product_class = do_product_param_int(alias, &product , do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS));
    product_nc = do_product_param_int(alias, &product, "NC");

    do_text_set(alias, document_key0.dtype, "GP");
    do_text_set(alias, document_key0.sklad, unit);
    do_text_set(alias, document_key0.document, "\255");

    do_cpy(key.dtype, document_key0.dtype);
    do_cpy(key.sklad, document_key0.sklad);

    status = do_document_get0(alias, &document, &document_key0, DO_GET_LE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.dtype, document_key0.dtype) ||
             do_cmp(key.sklad, document_key0.sklad) ) break;

        class_ = do_document_param_int(alias, &document, "406");

        if ( !class_ || class_ == product_class || (class_ == 15 && product_nc == 1)) {
            switch ( do_document_param_int(alias, &document, "405") ) {
                case MARKUP_KIND_ALL:
                    return get_from_document(alias, &document);
                case MARKUP_KIND_GROUP:
                    value = do_document_param(alias, &document, do_param(DO_PARAM_NAME_GROUP));
                    value1 = do_text(alias, product.data.group_id);
                    status = !strcmp(value, value1);
                    do_free(value);do_free(value1);
                    if ( status )
                        return get_from_document(alias, &document);
                    break;
                case MARKUP_KIND_SUBGROUP:
                    value1 = do_text(alias, product.data.group_id);
                    value2 = do_text(alias, product.data.subgroup_id);
                    value = do_strdup_printf("%s.%s", value1, value2);
                    do_free(value2);do_free(value1);
                    value1 = do_document_param(alias, &document, do_param(DO_PARAM_NAME_SUBGROUP));
                    status = !strcmp(value, value1);
                    do_free(value);do_free(value1);
                    if ( status )
                        return get_from_document(alias, &document);
                    break;
                case MARKUP_KIND_PRODUCT:
                    do_cpy(document_order_key2.dtype, document.data.dtype);
                    do_cpy(document_order_key2.sklad, document.data.sklad);
                    do_cpy(document_order_key2.document, document.data.document);
                    do_text_set(alias, document_order_key2.code, code);
                    if ( do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL) == DO_OK ) {
                        markup_info_t *res;
                        res = get_from_document(alias, &document);
                        if ( do_document_order_param_int(alias, &document_order, "PEREOPRED") == 1 ) {
                            if ( do_document_order_param_int(alias, &document_order, "PROC_SUM") == 1 ) {
                                res->price_define = TRUE;
                                res->price = do_document_order_price(alias, &document_order, 4);
                            }
                            else {
                                double fl = 0;
                                do_list_clear(res->scale);
                                do_list_add_alloc(res->scale, &fl, sizeof(fl));
                                fl = do_document_order_price(alias, &document_order, 4);
                                do_list_add_alloc(res->scale, &fl, sizeof(fl));
                            }
                        }
                        return res;
                    }
                    break;
                case MARKUP_KIND_SUPPLIER:
                    if ( !partner_region )
                        break;
                    value = do_strdup_printf("%s.%d", partner_region, partner_code);
                    value1 = do_document_param(alias, &document, do_param(DO_PARAM_NAME_SUPPLIER));
                    status = !strcmp(value, value1);
                    do_free(value);do_free(value1);
                    if ( status )
                        return get_from_document(alias, &document);
                    break;
             }
        }
        status = do_document_get0(alias, &document, &document_key0, DO_GET_PREVIOUS);
    }

    return NULL;
}

DO_EXPORT void do_markup_free(markup_info_t *markup)
{
    do_list_free(markup->scale);
    do_list_free(markup->scale_max);
}
double do_markup_get_rate(markup_info_t *markup, double price)
{
    int i;
    for ( i = 0; i < markup->scale->count; i++ ) {
        if ( *((double*)markup->scale->list[i]) == 0 ||
             *((double*)markup->scale->list[i]) >= price )
             return *((double*)markup->scale->list[i+1]);
    }
    do_log(LOG_ERR, "Markup not correct");
    return 0;
}
double do_markup_get_max_rate(markup_info_t *markup, double price)
{
    int i;
    for ( i = 0; i < markup->scale_max->count; i++ ) {
        if ( *((double*)markup->scale_max->list[i]) == 0 ||
             *((double*)markup->scale_max->list[i]) >= price )
             return *((double*)markup->scale_max->list[i+1]);
    }
    do_log(LOG_ERR, "Max markup not correct");
    return 0;
}
static markup_info_t *get_from_document(do_alias_t *alias, document_rec_t *document)
{
    markup_info_t *res;
    res = do_malloc0(sizeof(markup_info_t));
    res->price_reg = do_document_param_int(alias, document, do_param(DO_PARAM_NAME_PRICE)) == 40;
    res->aver_off = do_document_param_int(alias, document, "AVER_OFF");
    res->round_up = do_document_param_int(alias, document, "ROUND_UP");
    res->scale = do_list_new(TRUE);
    res->scale_max = do_list_new(TRUE);
    int i;
    char *value;
    double fl;
    do_string_list_t *vals;
    if ( do_document_param_int(alias, document, do_param(DO_PARAM_NAME_KIND)) == 1 ) {
        fl = 0;
        do_list_add_alloc(res->scale, &fl, sizeof(fl));
        fl = do_document_param_float(alias, document, do_param(DO_PARAM_NAME_RATE));
        do_list_add_alloc(res->scale, &fl, sizeof(fl));
    }
    else {
        value = do_document_param(alias, document, do_param(DO_PARAM_NAME_SCALE));
        vals = do_string_list_new_from_csv(value);
        do_free(value);
        for ( i = 0; i < vals->count; i++ ) {
            fl = atof(vals->list[i]);
            do_list_add_alloc(res->scale, &fl, sizeof(fl));
        }
        do_string_list_free(vals);
    }
    if ( do_document_param_int(alias, document, do_param(DO_PARAM_NAME_KIND_MAX)) == 1 ) {
        
        fl = do_document_param_float(alias, document, do_param(DO_PARAM_NAME_MAX_RATE));
        if ( fl > 0 ) {
            fl = 0;
            do_list_add_alloc(res->scale_max, &fl, sizeof(fl));
            fl = do_document_param_float(alias, document, do_param(DO_PARAM_NAME_MAX_RATE));
            do_list_add_alloc(res->scale_max, &fl, sizeof(fl));
        }
    }
    else {
        value = do_document_param(alias, document, do_param(DO_PARAM_NAME_MAX_SCALE));
        vals = do_string_list_new_from_csv(value);
        do_free(value);
        for ( i = 0; i < vals->count; i++ ) {
            fl = atof(vals->list[i]);
            do_list_add_alloc(res->scale_max, &fl, sizeof(fl));
        }
        do_string_list_free(vals);
    }

    return res;
}

static int get_base_product(do_alias_t *alias, const char *code, product_rec_t *product)
{
    product_key0_t product_key0;
    product_key0.base_parcel = 0;
    char *ch;
    int empty = FALSE;
    do_text_set(alias, product_key0.code, code);
    for ( ch = product_key0.code; ch < product_key0.code + sizeof(product_key0.code); ch++ ) {
          if ( *ch == '.' ) empty = TRUE;
          if ( empty ) *ch = ' ';
    }
    return  do_product_get0(alias, product, &product_key0, DO_GET_EQUAL) == DO_OK;
}
static double do_markup_get_max_grls_price__(do_alias_t *alias, const char *unit, int vat_rate, double price_for_scale, double price_for_markup)
{
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;
    partner_key0_t partner_key0;
    partner_rec_t partner;
    double retval, markup1=0,markup2=0;
    do_string_list_t *l;
    int i;

    do_text_set(alias, sklad_key0.code, unit);
    if ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) != DO_OK )
        return 0;
    do_cpy(partner_key0.g_code, sklad.data.p_g_code);
    partner_key0.code = sklad.data.p_code;

    if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) != DO_OK )
        return 0;
    char *value, *ch;
    value = do_partner_param(alias, &partner, "CITY");
    ch = strchr(value, '.');
    if ( !ch ) {
        do_free(value);
        return 0;
    }
    do_text_set(alias, partner_key0.g_code, value);
    ch++;
    partner_key0.code = atoi(ch);
    do_free(value);
    if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) != DO_OK )
        return 0;

//OptPrice = Округлить (m2*(1+markup/100.)*(1+СтавкаНДС/100.),10);
//MaxPrice = Округлить (OptPrice + m2*(markupretail/100.)*(1+СтавкаНДС/100.),10);

    value = do_partner_param(alias, &partner, "SCALE");
    l = do_string_list_new_from_csv(value);
    do_free(value);
    for ( i = 0; i < l->count; i+=2 ) {
        double top;
        if ( i + 1 == l->count )
            return 0;
        top = atof(l->list[i]);
        if ( top >= price_for_scale || !top ) {
            markup1 = atof(l->list[i+1]);
            break;
        }
    }
    do_string_list_free(l);
    if ( !markup1 )
        return 0;
    value = do_partner_param(alias, &partner, "SCALERETAIL");
    l = do_string_list_new_from_csv(value);
    do_free(value);
    for ( i = 0; i < l->count; i+=2 ) {
        double top;
        if ( i + 1 == l->count )
            return 0;
        top = atof(l->list[i]);
        if ( top >= price_for_scale || !top ) {
            markup2 = atof(l->list[i+1]);
            break;
        }
    }
    if ( !markup2 )
        return 0;
    do_string_list_free(l);
    //retval = floor(price_for_markup*(1+markup1/100.+markup2/100.)*(1+vat_rate/100.)*100)/100;
    retval = floor(price_for_markup*(1+markup1/100.)*(1+vat_rate/100.)*100)/100.;
    retval = floor((retval + price_for_markup*(markup2/100.))*100)/100.;
    if ( !retval )
        return 0;
    return retval;

}
static void do_markup_get_max_grls_pct__(do_alias_t *alias, const char *unit, double *pct1, double *pct2, double price_for_scale)
{
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;
    partner_key0_t partner_key0;
    partner_rec_t partner;
    do_string_list_t *l;
    int i;
    *pct1 = 0;
    *pct2 = 0;

    do_text_set(alias, sklad_key0.code, unit);
    if ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) != DO_OK )
        return;
    do_cpy(partner_key0.g_code, sklad.data.p_g_code);
    partner_key0.code = sklad.data.p_code;

    if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) != DO_OK )
        return;
    char *value, *ch;
    value = do_partner_param(alias, &partner, "CITY");
    ch = strchr(value, '.');
    if ( !ch ) {
        do_free(value);
        return;
    }
    do_text_set(alias, partner_key0.g_code, value);
    ch++;
    partner_key0.code = atoi(ch);
    do_free(value);
    if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) != DO_OK )
        return;

//OptPrice = Округлить (m2*(1+markup/100.)*(1+СтавкаНДС/100.),10);
//MaxPrice = Округлить (OptPrice + m2*(markupretail/100.)*(1+СтавкаНДС/100.),10);

    value = do_partner_param(alias, &partner, "SCALE");
    l = do_string_list_new_from_csv(value);
    do_free(value);
    for ( i = 0; i < l->count; i+=2 ) {
        double top;
        if ( i + 1 == l->count )
            return;
        top = atof(l->list[i]);
        if ( top >= price_for_scale || !top ) {
            *pct1 = atof(l->list[i+1]);
            break;
        }
    }
    do_string_list_free(l);
    value = do_partner_param(alias, &partner, "SCALERETAIL");
    l = do_string_list_new_from_csv(value);
    do_free(value);
    for ( i = 0; i < l->count; i+=2 ) {
        double top;
        if ( i + 1 == l->count )
            return;
        top = atof(l->list[i]);
        if ( top >= price_for_scale || !top ) {
            *pct2 = atof(l->list[i+1]);
            break;
        }
    }
    do_string_list_free(l);
}
DO_EXPORT double do_markup_get_max_grls_price(do_alias_t *alias, const char *unit, int vat_rate, double price, ...)
{
    va_list arg;
    double price_for_scale, price_for_markup,v;
    price_for_scale=price; price_for_markup=price;
    va_start(arg, price);
    while ( (v = va_arg(arg, double) )  >= 0 )  {
        if ( v > 0 ) {
            if ( price_for_scale < v )
                price_for_scale = v;
            if ( price_for_markup > v )
                price_for_markup = v;
        }
    }
    va_end(arg);
    return do_markup_get_max_grls_price__(alias, unit, vat_rate, price_for_scale, price_for_markup);
}
DO_EXPORT double do_markup_get_max_grls_price_(do_alias_t *alias, const char *unit, int vat_rate, int price_len, double *prices)
{
    int i;
    double price_for_scale, price_for_markup;
    price_for_scale=0; price_for_markup=0;
    for ( i = 0; i < price_len; i++ ) {
        if ( !price_for_scale || (price_for_scale < prices[i] && prices[i] > 0))
            price_for_scale = prices[i];
        if ( !price_for_markup || (price_for_markup > prices[i] && prices[i] > 0))
            price_for_markup = prices[i];
    }
    return do_markup_get_max_grls_price__(alias, unit, vat_rate, price_for_scale, price_for_markup);
}
DO_EXPORT void  do_markup_get_max_grls_pct(do_alias_t *alias, const char *unit, double *pct1, double *pct2, double price, ...)
{
    va_list arg;
    double price_for_scale,v;
    price_for_scale=price;
    va_start(arg, price);
    while ( (v = va_arg(arg, double) )  >= 0 )  {
        if ( v > 0 ) {
            if ( price_for_scale < v )
                price_for_scale = v;
        }
    }
    va_end(arg);
    do_markup_get_max_grls_pct__(alias, unit, pct1, pct2, price_for_scale);
}
DO_EXPORT void do_markup_get_max_grls_pct_(do_alias_t *alias, const char *unit, double *pct1, double *pct2, int price_len, double *prices)
{
    int i;
    double price_for_scale;
    price_for_scale=0;
    for ( i = 0; i < price_len; i++ ) {
        if ( !price_for_scale || price_for_scale < prices[i] )
            price_for_scale = prices[i];
    }
    do_markup_get_max_grls_pct__(alias, unit, pct1, pct2, price_for_scale);
}
