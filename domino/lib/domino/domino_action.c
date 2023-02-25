#include <domino_action.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "../../misc/define.h"

static const char *marked_name[] =
{ "unmark",
  "mark",
  "accept",
  "reject"
};

DO_EXPORT int domino_order_set_marked(do_alias_t *alias, const char *unit, const char *product_code,  struct tm *tm1_, struct tm *tm2_, domino_marked_t marked)
{
    return domino_order_set_marked_quant(alias, unit, product_code, tm1_, tm2_, marked, 0);
}
DO_EXPORT int domino_order_set_marked_full(do_alias_t *alias, const char *unit, const char *product_code, struct tm *tm1_, struct tm *tm2_, domino_marked_t marked, double quant, int partner, const char *desc, const char *life_time, const char *manufactor, double price, double price_fact)
{
    document_order_key2_t document_order_key2;
    //document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    document_key0_t document_key0;
    BTI_LONG date_, time_;


    char document_number[40];
    char buf[1024];
    char hostname[1024];

    struct tm tm_now;
    struct tm tm_;
    time_t now;
    int found;

    now = time(NULL);
    tm_now = *localtime(&now);
    //if (gethostname(hostname, 1023))
        strcpy(hostname, "unknow");

    tm_ = *tm2_;
    found = FALSE;

    do_text_set(alias, document_order_key2.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_ORDER1));
    do_text_set(alias, document_order_key2.sklad, unit);
    do_cpy(document_key0.dtype, document_order_key2.dtype);
    do_cpy(document_key0.sklad, document_order_key2.sklad);
    do_text_set(alias, document_order_key2.code, product_code);

    while ( do_date_cmp(&tm_, tm1_) >= 0 && !found) {

        sprintf(document_number, "%4.4d%2.2d%2.2d", tm_.tm_year + 1900, tm_.tm_mon + 1, tm_.tm_mday);
        do_text_set(alias, document_order_key2.document, document_number);
        do_cpy(document_key0.document, document_order_key2.document);

        switch ( do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL) ) {
            case DO_ERROR:
                return FALSE;
            case DO_OK:
                found = TRUE;
                break;
            case DO_KEY_NOT_FOUND:
                break;
        }
        if ( found )
            break;
        if ( marked == DOMINO_MARKED )
            break;
        tm_.tm_mday-=1;
        mktime(&tm_);
    }
    if ( !found ) {
        sprintf(document_number, "%4.4d%2.2d%2.2d", tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday);
        do_text_set(alias, document_order_key2.document, document_number);
        do_cpy(document_key0.document, document_order_key2.document);

        switch ( do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL) ) {
            case DO_ERROR:
                return FALSE;
            case DO_OK:
                found = TRUE;
                break;
            case DO_KEY_NOT_FOUND:
                break;
        }
    }

    sprintf(document_number, "%4.4d%2.2d%2.2d", tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday);
    switch ( marked ) {
        case DOMINO_MARKED_UNMARK: {
            if ( !found ||
                (document_order.data.accepted != DOMINO_MARKED && !domino_check_role(DOMINO_ROLE_MANAGER) )
                )
                return FALSE;
            break;

        }
        case DOMINO_MARKED: {
            if ( found ) {
                if ( document_order.data.accepted == DOMINO_MARKED_UNMARK )
                    break;
                /*if ( domino_check_role(DOMINO_ROLE_MANAGER) )
                    break;*/
                struct tm time_mark;
                /*do_date(do_document_order_param_int(alias, &document_order, "DATE_ORDER"), &time_mark);
                do_time(do_document_order_param_int(alias, &document_order, "TIME_ORDER"), &time_mark);*/
                do_date(document_order.data.date, &time_mark);
                do_time(document_order.data.time, &time_mark);
                /*if ( domino_order_after_traffic_in_(do_alias_get_name(alias), unit, product_code, &time_mark) ) {
                    BTI_LONG date_now;
                    do_date_set(&date_now, tm_now);
                    found = date_now == document_order.data.date;
                    break;
                }*/
            }
            break;
        }
        case DOMINO_MARKED_AND_ACCEPT:
        case DOMINO_MARKED_AND_REJECT:
            if ( marked == document_order.data.accepted && strncmp(document_order.data.document, document_number, 8) )
                found = FALSE;
            break;
    }
    sprintf(document_number, "%4.4d%2.2d%2.2d", tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday);

    if ( found && marked == document_order.data.accepted ) {
         return TRUE;
    }

    if ( !found ) {
        sprintf(document_number, "%4.4d%2.2d%2.2d", tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday);
        do_text_set(alias, document_order_key2.document, document_number);
        do_cpy(document_key0.document, document_order_key2.document);
    }

    if ( marked != DOMINO_MARKED_UNMARK ) {
        document_rec_t document;

        switch ( do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL)) {
            case DO_ERROR:
                return FALSE;
            case DO_OK:
                break;
            case DO_KEY_NOT_FOUND: {
                document_rec_t document;
                memset(&document.data, 0, sizeof(document.data));
                document.size = sizeof(document.data) - sizeof(document.data.params);
                do_cpy(document.data.dtype, document_key0.dtype);
                do_text_set(alias, document.data.dtype1, "1");
                do_cpy(document.data.sklad, document_key0.sklad);
                do_cpy(document.data.document, document_key0.document);
                do_text_set_empty(document.data.name);
                BTI_LONG date_,time_;
                if ( found ) { // just in case
                    document.data.date = do_date_set(&date_, tm_);
                    document.data.time = do_time_set(&time_, tm_);
                }
                else {
                    document.data.date = do_date_set(&date_, tm_now);
                    document.data.time = do_time_set(&time_, tm_now);
                }
                do_text_set(alias, document.data.p_g_code, "00");
                document.data.p_code = 1;
                do_text_set_empty(document.data.p_doc);
                do_text_set_empty(document.data.party);
                if (do_document_insert(alias, &document) == DO_ERROR)
                    return FALSE;
                if ( !do_protocol_document_add_rec(alias, &document, DO_PROTOCOL_ACTION_INSERT) )
                    return FALSE;
                break;
            }
        }
    }

    if ( !found ) {
        document_order_key0_t document_order_key0;

        do_cpy(document_order_key0.dtype, document_key0.dtype);
        do_cpy(document_order_key0.sklad, document_key0.sklad);
        do_cpy(document_order_key0.document, document_key0.document);
        document_order_key0.line = 2147483646;

        switch ( do_document_order_key0(alias, &document_order_key0, DO_GET_LT) ) {
            case DO_ERROR:
                return FALSE;
            case DO_KEY_NOT_FOUND:
                document_order_key0.line = 0;
                break;
            case DO_OK:
                if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                     do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                     do_cmp(document_order_key0.document, document_key0.document)
                    )
                    document_order_key0.line = 0;
                break;
        }
        do_cpy(document_order.data.dtype, document_key0.dtype);
        do_text_set(alias, document_order.data.dtype1, "1");
        do_cpy(document_order.data.sklad, document_key0.sklad);
        do_cpy(document_order.data.document, document_key0.document);
        document_order.data.line = document_order_key0.line + 1;
        document_order.data.sort = 0;
        do_text_set(alias, document_order.data.code, product_code);
        BTI_LONG date_,time_;
        document_order.data.date = do_date_set(&date_, tm_now);
        document_order.data.time = do_time_set(&time_, tm_now);
        document_order.data.accepted = 0;
        do_set_empty(document_order.data.nquant);
        do_set_empty(document_order.data.nprice);
        do_document_order_params_clear(alias, &document_order);
    }
    document_order.data.accepted = marked;
    do_date_set(&date_, tm_now);
    do_time_set(&time_, tm_now);
    do_document_order_param_int_set(alias, &document_order, "DATE_ORDER",  date_);
    do_document_order_param_int_set(alias, &document_order, "TIME_ORDER",  time_);
    if ( partner )
        do_document_order_param_int_set(alias, &document_order, "PARTNER",  partner);
    if ( desc && desc[0] )
        do_document_order_param_set(alias, &document_order, "DESC", desc);
    if ( life_time && life_time[0] )
        do_document_order_param_set(alias, &document_order, "LIFE_TIME", life_time);
    if ( manufactor && manufactor[0] )
        do_document_order_param_set(alias, &document_order, "MANUFACTOR", manufactor);
    if ( price )
        do_document_order_price_set(alias, &document_order, 2, price);
    if ( price_fact )
        do_document_order_price_set(alias, &document_order, 9, price_fact);
    if ( quant )
        do_document_order_quant_set(alias, &document_order, 1, quant);

    char *param;
    int i;
    for ( i = 0; i < 5; i++ ) {
        sprintf(buf, "PRO%3.3d", i);
        param = do_document_order_param(alias, &document_order, buf);
        if ( param[0] == '\0' ) {
            do_free(param);
            break;
        }
    }
    param = do_strdup_printf("%2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d - %s %s",
                            tm_now.tm_mday,  tm_now.tm_mon + 1, tm_now.tm_year + 1900, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec,
                            marked_name[marked], hostname);
    do_document_order_param_set(alias, &document_order, buf, param);
    do_free(param);
    if ( found ) {
        if ( do_document_order_update(alias, &document_order) != DO_OK )
            return FALSE;
        sprintf(buf, "%s %s %s", marked_name[marked], product_code, hostname);
    }
    else {
        if ( do_document_order_insert(alias, &document_order) != DO_OK )
            return FALSE;
        sprintf(buf, "ins %s %s", product_code, hostname);

    }
    do_protocol_document_order_add_rec(alias, &document_order, DO_PROTOCOL_ACTION_NONE, buf);
    do_protocol_document_add_key(alias, &document_key0, DO_PROTOCOL_ACTION_ACCEPT);

    return TRUE;
}
DO_EXPORT int domino_order_set_marked_quant(do_alias_t *alias, const char *unit, const char *product_code,  struct tm *tm1_, struct tm *tm2_, domino_marked_t marked, double quant)
{
    return domino_order_set_marked_full(alias, unit, product_code, tm1_, tm2_, marked, quant, 0, "", "", "", 0, 0);
}
DO_EXPORT int domino_order_set_marked_(const char *alias_name, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t marked)
{
    do_alias_t *alias;
    alias = domino_alias_new(alias_name);
    if ( !alias )
        return FALSE;

    int ret = TRUE;

    if ( do_alias_open(alias, FALSE, DOMINO_ORDER_DBS) )
        ret = domino_order_set_marked(alias, unit, product_code, tm1, tm2, marked);
    else
        ret = FALSE;
    do_alias_free(alias);
    return ret;
}

DO_EXPORT int domino_order_get_marked(do_alias_t *alias, const char *unit, const char *product_code,  struct tm *tm1, struct tm *tm2, domino_marked_t *marked, struct tm *time_mark)
{
    return domino_order_get_marked_quant(alias,unit,product_code,tm1,tm2,marked,time_mark,NULL);
}
DO_EXPORT int domino_order_get_marked_quant(do_alias_t *alias, const char *unit, const char *product_code,  struct tm *tm1, struct tm *tm2, domino_marked_t *marked, struct tm *time_mark, double *quant)
{
	*marked = DOMINO_MARKED_UNMARK;
    document_order_key2_t document_order_key2;
    document_order_rec_t document_order;
    char document_number[40];
    struct tm tm;



    do_text_set(alias, document_order_key2.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_ORDER1));
    do_text_set(alias, document_order_key2.sklad, unit);
    do_text_set(alias, document_order_key2.code, product_code);

    tm = *tm2;
    while ( do_date_cmp(&tm, tm1) >= 0 ) {

        sprintf(document_number, "%4.4d%2.2d%2.2d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
        do_text_set(alias, document_order_key2.document, document_number);

        switch ( do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL) ) {
            case DO_ERROR:
                return FALSE;
            case DO_OK: {
                *marked = document_order.data.accepted;
                do_date(document_order.data.date, time_mark);
                do_time(document_order.data.time, time_mark);
                if ( quant )
                    *quant = do_document_order_quant(alias, &document_order, 1);
                /*switch ( document_order.data.accepted )  {
                    case DOMINO_MARKED:
                        do_date(document_order.data.date, time_mark);
                        do_time(document_order.data.time, time_mark);
                        break;
                    case DOMINO_MARKED_AND_ACCEPT:
                    case DOMINO_MARKED_AND_REJECT:
                        do_date(do_document_order_param_int(alias, &document_order, "DATE_ORDER"), time_mark);
                        do_time(do_document_order_param_int(alias, &document_order, "TIME_ORDER"), time_mark);
                        break;
                    default:
                        break;
                }*/
                return TRUE;
            }
            case DO_KEY_NOT_FOUND:
                break;
        }
        tm.tm_mday-=1;
        mktime(&tm);
    }

    return TRUE;
}
DO_EXPORT int domino_order_get_marked_(const char *alias_name, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t *marked, struct tm *time_mark)
{
    return domino_order_get_marked_quant_(alias_name, unit, product_code, tm1, tm2, marked, time_mark, NULL);
}
DO_EXPORT int domino_order_get_marked_quant_(const char *alias_name, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t *marked, struct tm *time_mark, double *quant)
{
    do_alias_t *alias;
    *marked = DOMINO_MARKED_UNMARK;
    alias = domino_alias_new(alias_name);
    if ( !alias )
        return FALSE;

    int ret = TRUE;

    if ( do_alias_open(alias, FALSE, DO_DB_DOCUMENT_ORDER, DO_DB_END) )
        ret = domino_order_get_marked_quant(alias, unit, product_code, tm1, tm2, marked, time_mark, quant);
    else
        ret = FALSE;
    do_alias_free(alias);
    return ret;
}


DO_EXPORT do_list_t *domino_order_get_marked_list(do_alias_t *alias, const char *unit, struct tm *tm1, struct tm *tm2, int uniq)
{
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;

    document_key0_t document_key0;
    char document_number[40];
    struct tm tm;
    do_list_t *retval;
    domino_order_marked_list_item_t item;


    tm = *tm1;
    retval = do_list_new(TRUE);


    while ( do_date_cmp(&tm, tm2) <= 0 ) {

        int status;

        sprintf(document_number, "%4.4d%2.2d%2.2d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
        do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_ORDER1));
        do_text_set(alias, document_key0.sklad, unit);
        do_text_set(alias, document_key0.document, document_number);

        do_cpy(document_order_key0.dtype, document_key0.dtype);
        do_cpy(document_order_key0.sklad, document_key0.sklad);
        do_cpy(document_order_key0.document, document_key0.document);

        document_order_key0.line = 0;


        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);

        while ( status == DO_OK ) {
            if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                 do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                 do_cmp(document_order_key0.document, document_key0.document) )
                 break;

            item.code = do_text(alias, document_order.data.code);
            item.marked = document_order.data.accepted;
            item.quant = do_document_order_quant(alias, &document_order, 1);

            do_date(document_order.data.date, &item.tm);
            do_time(document_order.data.time, &item.tm);
            do_date(do_document_order_param_int(alias, &document_order, "DATE_ORDER"), &item.last_tm);
            do_time(do_document_order_param_int(alias, &document_order, "TIME_ORDER"), &item.last_tm);
            /*switch ( document_order.data.accepted )  {
                case DOMINO_MARKED:
                    break;
                case DOMINO_MARKED_AND_ACCEPT:
                case DOMINO_MARKED_AND_REJECT:
                    do_date(do_document_order_param_int(alias, &document_order, "DATE_ORDER"), &item.tm);
                    do_time(do_document_order_param_int(alias, &document_order, "TIME_ORDER"), &item.tm);
                    break;
                default:
                    break;
            }*/
            if ( uniq ) {
                int i;
                for ( i = 0; i < retval->count; i++ ) {
                    domino_order_marked_list_item_t *rec;
                    rec = retval->list[i];
                    if ( !strcmp(item.code, rec->code) ) {
                        item.quant+=rec->quant;
                        do_list_delete(retval, i);
                        break;
                    }
                }
            }
            do_list_add_alloc(retval, &item, sizeof(item));

            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
        }
        if ( status == DO_ERROR )  {
            do_string_list_free(retval);
            return NULL;
        }
        tm.tm_mday+=1;
        mktime(&tm);
    }

    return retval;
}
DO_EXPORT do_list_t *domino_order_get_marked_list_(const char *alias_name, const char *unit, struct tm *tm1, struct tm *tm2, int uniq)
{
    do_alias_t *alias;
    alias = domino_alias_new(alias_name);
    if ( !alias )
        return NULL;
    do_string_list_t *ret = NULL;

    if ( do_alias_open(alias, FALSE, DO_DB_DOCUMENT_ORDER, DO_DB_END) )
        ret = domino_order_get_marked_list(alias, unit, tm1, tm2, uniq);

    do_alias_free(alias);

    return ret;
}
DO_EXPORT void domino_order_marked_list_free(do_list_t *list)
{
    int i;
    for ( i = 0; i < list->count; i++ ) {
        domino_order_marked_list_item_t *item;
        item = list->list[i];
        do_free(item->code);
    };
    do_list_free(list);
}
DO_EXPORT int domino_order_after_traffic_in_(const char *alias_name, const char *unit, const char *product_code, struct tm *tm1)
{
    do_alias_t *alias;
    alias = domino_alias_new(alias_name);
    if ( !alias )
        return FALSE;
    int ret = FALSE;

    if ( do_alias_open(alias, FALSE, DOMINO_ORDER_DBS) )
        ret = domino_order_after_traffic_in(alias, unit, product_code, tm1);

    do_alias_free(alias);

    return ret;
}
DO_EXPORT int domino_order_after_traffic_in(do_alias_t *alias, const char *unit, const char *product_code, struct tm *tm1)
{
    product_key0_t product_key0;
    product_rec_t product;
    BTI_LONG date;
    BTI_LONG time;
    int unit_int;
    char key[sizeof(product_key0.code)+1];
    int status;
    BTI_LONG m_date_accept=0;
    BTI_LONG m_time_accept=0;

    if ( !strcmp(product_code,"19139") ) {
        status = 0;
    }

    do_date_set(&date, *tm1);
    do_time_set(&time, *tm1);
    unit_int = atoi(unit);
    memset(product_key0.code, '9', sizeof(product_key0.code));
    sprintf(key, "%s.%2.2d", product_code, unit_int);
    strncpy(product_key0.code, key, strlen(key));
    product_key0.base_parcel = 1;


    status = do_product_get0(alias, &product, &product_key0, DO_GET_LE);
    while ( status == DO_OK ) {
        BTI_LONG date_accept;
        BTI_LONG time_accept;

        if ( strncmp(product_key0.code, key, strlen(key)) ||
            !product_key0.base_parcel ) break;
         date_accept = do_product_param_int(alias, &product , do_param(DO_PARAM_COMMON_DATE_ACCEPT));
         time_accept = do_product_param_int(alias, &product , do_param(DO_PARAM_COMMON_TIME_ACCEPT));

         if ( !date_accept || date_accept < 7000 ) {
            date_accept = product.data.date;
            time_accept = product.data.time;
         }
         if ( !time_accept )
            time_accept = product.data.time;

         if ( !m_date_accept || m_date_accept < date_accept )
            m_date_accept = date_accept;

         if ( m_date_accept == date_accept && ( !m_time_accept || m_time_accept < time_accept ))
            m_time_accept = time_accept;

         if ( (m_date_accept  && m_time_accept) && ( m_date_accept > date || ( m_date_accept == date && m_time_accept > time ) ) )
            return TRUE;

         status = do_product_get0(alias, &product, &product_key0, DO_GET_PREVIOUS);
    }

    return FALSE;
/*
        product.data.date >= date )  {

        if ( product.data.date > date + 7 )
            return TRUE;

        document_order_key3_t document_order_key3;
        document_type_key0_t in,out,crnt;
        document_order_rec_t document_order;
        do_cpy(document_order_key3.code, product_key0.code);
        document_order_key3.date = date;
        document_order_key3.time = 0;
        double quant_in,quant_out;

        do_text_set(alias, in.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
        do_text_set(alias, out.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
        do_text_set(alias, crnt.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_CRNTSALE));

        quant_in = quant_out = 0;

        status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_GE);
        while ( status == DO_OK ) {
            if ( do_cmp(document_order_key3.code, product_key0.code) || document_order_key3.date != date ) break;

            if ( !do_cmp(document_order.data.dtype, in.dtype) )
                quant_in += do_document_order_quant(alias, &document_order, 1);
            else
            if ( !do_cmp(document_order.data.dtype, out.dtype) )
                quant_out += do_document_order_quant(alias, &document_order, 1);
            else
            if ( !do_cmp(document_order.data.dtype, crnt.dtype) )
                quant_out += do_document_order_quant(alias, &document_order, 2);

            status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_NEXT);
        }

        if ( quant_out > quant_in*0.7 )
            return FALSE;
        return TRUE;
    }
    else
        return FALSE;*/
}
DO_EXPORT int domino_set_recommend(do_alias_t *alias, const char *product_code)
{
    return TRUE;
    /*
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    document_key0_t document_key0;
    document_rec_t document;
    //BTI_LONG date_, time_;

    char document_number[40];
    //char buf[1024];
    char hostname[1024];

    struct tm tm_now;
    time_t now;
    now = time(NULL);
    tm_now = *localtime(&now);
    if (gethostname(hostname, 1023))
        strcpy(hostname, "unknow");

    do_text_set(alias, document_key0.dtype, "MM");
    do_text_set(alias, document_key0.sklad, domino_unit());
    sprintf(document_number, "%4.4d%2.2d%2.2d", tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday);
    do_text_set(alias, document_key0.document, document_number);


    switch ( do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL)) {
        case DO_ERROR:
            return FALSE;
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND: {
            document_rec_t document;
            memset(&document.data, 0, sizeof(document.data));
            document.size = sizeof(document.data) - sizeof(document.data.params);
            do_cpy(document.data.dtype, document_key0.dtype);
            do_text_set(alias, document.data.dtype1, "1");
            do_cpy(document.data.sklad, document_key0.sklad);
            do_cpy(document.data.document, document_key0.document);
            do_text_set_empty(document.data.name);
            do_date_set(&document.data.date, tm_now);
            do_time_set(&document.data.time, tm_now);
            do_text_set(alias, document.data.p_g_code, "00");
            document.data.p_code = 1;
            do_text_set_empty(document.data.p_doc);
            do_text_set_empty(document.data.party);
            if (do_document_insert(alias, &document) == DO_ERROR)
                return FALSE;
            if ( !do_protocol_document_add_rec(alias, &document, DO_PROTOCOL_ACTION_INSERT) )
                return FALSE;
            break;
        }
    }

    do_cpy(document_order_key0.dtype, document_key0.dtype);
    do_cpy(document_order_key0.sklad, document_key0.sklad);
    do_cpy(document_order_key0.document, document_key0.document);
    document_order_key0.line = 2147483646;

    switch ( do_document_order_key0(alias, &document_order_key0, DO_GET_LT) ) {
        case DO_ERROR:
            return FALSE;
        case DO_KEY_NOT_FOUND:
            document_order_key0.line = 0;
            break;
        case DO_OK:
            if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                 do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                 do_cmp(document_order_key0.document, document_key0.document)
                )
                document_order_key0.line = 0;
            break;
    }
    do_cpy(document_order.data.dtype, document_key0.dtype);
    do_text_set(alias, document_order.data.dtype1, "1");
    do_cpy(document_order.data.sklad, document_key0.sklad);
    do_cpy(document_order.data.document, document_key0.document);
    document_order.data.line = document_order_key0.line + 1;
    document_order.data.sort = 0;
    do_text_set(alias, document_order.data.code, product_code);
    do_date_set(&document_order.data.date, tm_now);
    do_time_set(&document_order.data.time, tm_now);
    document_order.data.accepted = 0;
    do_set_empty(document_order.data.nquant);
    do_set_empty(document_order.data.nprice);
    do_document_order_params_clear(alias, &document_order);
    do_document_order_param_set(alias, &document_order, "host", hostname);
    if ( do_document_order_insert(alias, &document_order) != DO_OK )
        return FALSE;
    return TRUE;*/
}
