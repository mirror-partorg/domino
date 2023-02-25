
#include <domino.h>
#include <dolib.h>
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "../misc/define.h"
#include <do-macros.h>
#include <math.h>

DO_EXPORT int  do_util_ch_params(BTI_CHAR *params, BTI_WORD *size, size_t offset)
{
#define eof (cp - params >= len)

    BTI_CHAR name[1024], *n;
    BTI_CHAR value[1024], *v;
    BTI_CHAR buffer[10024], *buf;
    size_t len = *size - offset;
    char *cp = params;
    buf = buffer;
    while (!eof) {
        n = name;
        v = value;
        while (!eof && *cp != '=' && *cp !='\n') {
            if (*cp != '\0' && *cp != '\a') {
                *n = *cp;
                 n++;
            }
            cp++;
        }
        if (eof) break;

        if (*cp == '\n' || (n - name) > 100) {
            cp++;
            continue;
        }

        cp++;
        while (!eof && *cp !='\n') {
            if (*cp != '\0') {
                *v = *cp;
                 v++;
            }
            cp++;
        }
        if (n > name) {
            strncpy(buf, name, n - name);
            buf += n - name;
            *buf = '=';
            buf++;
            strncpy(buf, value, v - value);
            buf += v - value;
            *buf = '\n';
            buf++;
        }
        if (!eof)
            cp++;
    }

    if (len != buf - buffer ||
        strncmp(params, buffer, len)) {
        strncpy(params, buffer, buf - buffer);
        *size = offset + (buf - buffer);
        return FALSE;
    }
    return TRUE;
}

DO_EXPORT int do_util_str_to_document_order_key0(do_alias_t *alias, const char *str, document_order_key0_t *key0)
{
    char *value = do_strdup(str);
    char *head, *ch;

    head = value;
    ch = strchr (head, '.');
    if (!ch) {
        do_free(value);
        return 0;
    }
    *ch = '\0';
    do_text_set(alias, key0->dtype, head);
    ch++;
    head = ch;
    ch = strchr (head, '.');
    if (!ch) {
        do_free(value);
        return 0;
    }
    *ch = '\0';
    do_text_set(alias, key0->sklad, head);
    ch++;
    head = ch;
    ch = strchr (head, '.');
    if (!ch) {
        do_free(value);
        return 0;
    }
    *ch = '\0';
    do_text_set(alias, key0->document, head);
    ch++;
    head = ch;
    key0->line = atoi(head);
    do_free(value);
    return key0->line != 0;
}
DO_EXPORT int do_util_str_to_document_key0(do_alias_t *alias, const char *str, document_key0_t *key0)
{
    char *value = do_strdup(str);
    char *head, *ch;

    head = value;
    ch = strchr (head, '.');
    if (!ch) {
        do_free(value);
        return 0;
    }
    *ch = '\0';
    do_text_set(alias, key0->dtype, head);
    ch++;
    head = ch;
    ch = strchr (head, '.');
    if (!ch) {
        do_free(value);
        return 0;
    }
    *ch = '\0';
    do_text_set(alias, key0->sklad, head);
    ch++;
    do_text_set(alias, key0->document, ch);
    return *ch != '\0';
}
DO_EXPORT char *do_util_document_order_key0_to_str(do_alias_t *alias, document_order_key0_t *key0)
{
    char *dtype;
    char *sklad;
    char *document;
    char *retval;
    int   line;

    dtype = do_text(alias, key0->dtype);
    sklad = do_text(alias, key0->sklad);
    document = do_text(alias, key0->document);
    line = key0->line;

    retval = do_malloc(strlen(dtype) + strlen(sklad) + strlen(document) + 3 + 20);
    sprintf(retval, "%s.%s.%s.%d", dtype, sklad, document, line);
    do_free(dtype);do_free(sklad);do_free(document);
    return retval;
}
DO_EXPORT char *do_util_document_order_to_str(do_alias_t *alias, document_order_rec_t *rec)
{
    char *dtype;
    char *sklad;
    char *document;
    char *retval;
    int   line;

    dtype = do_text(alias, rec->data.dtype);
    sklad = do_text(alias, rec->data.sklad);
    document = do_text(alias, rec->data.document);
    line = rec->data.line;

    retval = do_malloc(strlen(dtype) + strlen(sklad) + strlen(document) + 3 + 20);
    sprintf(retval, "%s.%s.%s.%d", dtype, sklad, document, line);
    do_free(dtype);do_free(sklad);do_free(document);
    return retval;
}
DO_EXPORT char *do_util_document_key0_to_str(do_alias_t *alias, document_key0_t *key0)
{
    char *dtype;
    char *sklad;
    char *document;
    char *retval;

    dtype = do_text(alias, key0->dtype);
    sklad = do_text(alias, key0->sklad);
    document = do_text(alias, key0->document);

    retval = do_malloc(strlen(dtype) + strlen(sklad) + strlen(document) + 3 + 20);
    sprintf(retval, "%s.%s.%s", dtype, sklad, document);
    do_free(dtype);do_free(sklad);do_free(document);
    return retval;
}
DO_EXPORT char *do_util_document_to_str(do_alias_t *alias, document_rec_t *rec)
{
    char *dtype;
    char *sklad;
    char *document;
    char *retval;

    dtype = do_text(alias, rec->data.dtype);
    sklad = do_text(alias, rec->data.sklad);
    document = do_text(alias, rec->data.document);

    retval = do_malloc(strlen(dtype) + strlen(sklad) + strlen(document) + 3 + 20);
    sprintf(retval, "%s.%s.%s", dtype, sklad, document);
    do_free(dtype);do_free(sklad);do_free(document);
    return retval;
}
#define default_format "%6.6d"
DO_EXPORT int do_util_get_empty_document_number(do_alias_t *alias, document_key0_t *key)
{
    char *format;
    enum_rec_t enum_;
    enum_key0_t enum_key0;
    char name[1024];
    char *dtype;
    char  value[50];
    char *sklad;
    int number;
    otdel_key0_t otdel_key0;
    otdel_rec_t otdel;

    do_text_set(alias, otdel_key0.code, domino_division());

    switch ( do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            format = do_otdel_param(alias, &otdel, do_param(DO_PARAM_OTDEL_PARAMETR_NUMBER_FORMAT));
            if ( *format == '\0' ) {
                do_free(format);
                format = do_strdup(default_format);
            }
            break;
        case DO_KEY_NOT_FOUND:
            do_log(LOG_ERR, "otdel not found");
            return FALSE;
        default:
            return FALSE;
    }

    dtype = do_text(alias, key->dtype);
    sklad = do_text(alias, key->sklad);

    sprintf(name, "%s.%s|%s|", do_param(DO_PARAM_PROTOCOL_OBJECT_DOCUMENT), dtype, sklad);
    do_free(dtype);do_free(sklad);

    do_text_set(alias, enum_key0.name, name);

    switch (do_enum_get0(alias, &enum_, &enum_key0, DO_GET_EQUAL)) {
        case DO_ERROR:
            do_free(format);
            return FALSE;
        case DO_KEY_NOT_FOUND:
            do_cpy(enum_.data.name, enum_key0.name);
            enum_.data.step = 1;
            enum_.data.hiBound = 999999;
            enum_.data.cycleFl = 1;
            do_text_set(alias, enum_.data.format, "N06");
            enum_.data.LowBound = 1;
            enum_.data.lastvalue = 1;
            if ( do_enum_insert(alias, &enum_) != DO_OK )
        	return FALSE;
    	    if ( do_enum_get0(alias, &enum_, &enum_key0, DO_GET_EQUAL) != DO_OK )
    		return FALSE;
            number = 1;
            break;
        default:
            number = enum_.data.lastvalue;
    }

    while (number < 999999) {
        sprintf(value, format, number);
        do_text_set(alias, key->document, value);
        switch (do_document_key0(alias, key, DO_GET_EQUAL)) {
            case DO_ERROR:
                do_free(format);
                return FALSE;
            case DO_KEY_NOT_FOUND:
                do_free(format);
                enum_.data.lastvalue = number + 1;
                return do_enum_update(alias, &enum_) == DO_OK;
        }
        number++;
    }
    do_free(format);
    return FALSE;
}
DO_EXPORT time_t do_sqltotime(int time, struct tm *tm)
{
    tm->tm_hour = (time & 0xFF000000) >> 24;
    tm->tm_min = (time &  0xFF0000) >> 16;
    tm->tm_sec =  (time & 0xFF00) >> 8;
    return mktime(tm);
}
DO_EXPORT int64_t do_real_to_money(double value)
{
    int i;
    double intvalue = (value >= 0) ? value : -value;
    int64_t ivalue;
    for (i = 0; i < 2; i++,intvalue*=10);
    ivalue = intvalue;
    if (intvalue - ivalue > 0.5)
        ivalue++;
    else
    if (intvalue - ivalue < -0.5)
        ivalue--;
    return (value >= 0) ? ivalue : -ivalue;
}
DO_EXPORT int do_util_str_to_partner_key0(do_alias_t *alias, const char *str, partner_key0_t *partner_key0)
{
    char *value = do_strdup(str);
    char *ch;

    ch = strchr (value, '.');
    if (!ch) {
        do_free(value);
        return 0;
    }
    *ch = '\0';
    ch++;
    do_text_set(alias, partner_key0->g_code, value);
    partner_key0->code = atoi(ch);
    do_free(value);
    return partner_key0->code != 0;
}
DO_EXPORT char *do_util_partner_key0_to_str(do_alias_t *alias, partner_key0_t *partner_key0)
{
    char *value;
    int code;
    char *buf = do_malloc(100);
    value = do_text(alias, partner_key0->g_code);
    code = partner_key0->code;
    sprintf(buf, "%s.%d", value, code);
    do_free(value);
    return buf;
}

DO_EXPORT do_ret_list_t *do_util_product_list_get(do_alias_t *alias, partner_key0_t *partner_key0)
{
    do_extended_t *extended;
    document_order_key0_t document_order_key0;
    char buf[50];
    int value;

    do_ret_list_t *list;

    do_text_set(alias, document_order_key0.dtype, "AG");
    do_text_set(alias, document_order_key0.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    value = partner_key0->code;

    sprintf(buf, "%d", value);
    do_text_set(alias, document_order_key0.document, buf);
    document_order_key0.line = 0;


    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);
    do_extended_set_key(extended, 0, &document_order_key0);
    do_extended_set_filter(extended, FALSE, 3, sizeof(document_order_key0.dtype),
                                               sizeof(document_order_key0.sklad),
                                               sizeof(document_order_key0.document));

    do_extended_set_filter_term(extended, 0, DO_EXTENDED_TYPE_STRING,
                                             offsetof(document_order_struct_t, dtype),
                                             DO_EXTENDED_COMP_EQUAL,
                                             DO_EXTENDED_CONNECTION_AND,
                                             &document_order_key0.dtype);
    do_extended_set_filter_term(extended, 1, DO_EXTENDED_TYPE_STRING,
                                             offsetof(document_order_struct_t, sklad),
                                             DO_EXTENDED_COMP_EQUAL,
                                             DO_EXTENDED_CONNECTION_AND,
                                             &document_order_key0.sklad);
    do_extended_set_filter_term(extended, 2, DO_EXTENDED_TYPE_STRING,
                                             offsetof(document_order_struct_t, document),
                                             DO_EXTENDED_COMP_EQUAL,
                                             DO_EXTENDED_CONNECTION_NO,
                                             &document_order_key0.document);

    do_extended_set_fields(extended, 1, do_size_and_offset(document_order_struct_t, code));

    list = do_extended_get(extended, -1);

    return list;
}
DO_EXPORT int do_util_product_list_delete_product(do_alias_t *alias, partner_key0_t *partner_key0, product_key4_t *product_key4)
{
    document_order_key2_t document_order_key2;
    document_order_rec_t document_order;
    char buf[50];
    int value;
    int status;

    do_text_set(alias, document_order_key2.dtype, "AG");
    do_text_set(alias, document_order_key2.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    value = partner_key0->code;
    sprintf(buf, "%d", value);
    do_text_set(alias, document_order_key2.document, buf);
    do_cpy(document_order_key2.code, product_key4->code);

    status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL);
    while ( status == DO_OK ) {
        if ( do_document_order_delete(alias) != DO_OK ) {
            status = DO_ERROR;
            break;
        }
        status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL);
    }
    if ( status == DO_ERROR )
        return FALSE;
    return TRUE;
}
DO_EXPORT int do_util_product_list_insert_product(do_alias_t *alias, partner_key0_t *partner_key0, product_key4_t *product_key4)
{
    document_order_key2_t document_order_key2;
    document_order_rec_t document_order;
    document_key0_t document_key0;
    char buf[50];
    int value;

    do_text_set(alias, document_order_key2.dtype, "AG");
    do_text_set(alias, document_order_key2.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    value = partner_key0->code;
    sprintf(buf, "%d", value);
    do_text_set(alias, document_order_key2.document, buf);
    do_cpy(document_order_key2.code, product_key4->code);

    switch ( do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL) ) {
        case DO_OK:
            return TRUE;
        case DO_KEY_NOT_FOUND: {
            memset(&document_order.data, 0, sizeof(document_order.data));
            do_cpy(document_key0.dtype, document_order_key2.dtype);
            do_cpy(document_key0.sklad, document_order_key2.sklad);
            do_cpy(document_key0.document, document_order_key2.document);
            do_cpy(document_order.data.dtype, document_order_key2.dtype);
            do_cpy(document_order.data.sklad, document_order_key2.sklad);
            do_cpy(document_order.data.document, document_order_key2.document);
            document_order.data.line =
                do_document_get_last_line(alias, &document_key0) + 1;

            do_text_set(alias, document_order.data.dtype1, "1");
            document_order.data.sort = 0;
            do_cpy(document_order.data.code, document_order_key2.code);
            time_t now = time(NULL);
            struct tm tm = *localtime(&now);
            BTI_LONG date_,time_;
            document_order.data.date = do_date_set(&date_, tm);
            document_order.data.time = do_time_set(&time_, tm);
            do_document_order_params_clear(alias, &document_order);
            return do_document_order_insert(alias, &document_order) == DO_OK;
        }
        default:
            return FALSE;
    }
}

DO_EXPORT int do_util_product_get4(do_alias_t *alias, product_rec_t *product, product_key4_t *product_key4)
{
    switch ( do_product_get4(alias, product, product_key4, DO_GET_EQUAL) ) {
        case DO_OK:
            return TRUE;
        case DO_KEY_NOT_FOUND: {
            char *code;
            code = do_text(alias, product_key4->code);
            do_log(LOG_ERR, "product \"%s\" not found", code);
            do_free(code);
        }
        default:
            return FALSE;
    }
}
#define CITY_REGION "CYT"
#define INSIDE_REGION "01"
#define FIRM_REGION "00"

DO_EXPORT char *do_city_get_name(do_alias_t *alias, int code)
{
    partner_key0_t partner_key0;
    partner_rec_t partner;

    do_text_set(alias, partner_key0.g_code, CITY_REGION);
    partner_key0.code = code;

    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_KEY_NOT_FOUND:
            do_log(LOG_ERR, "partner %s.%d not found", CITY_REGION, code);
        case DO_ERROR:
            return do_strdup("");
        case DO_OK:
            return do_text(alias, partner.data.name);
    }
    return do_strdup("");
}
static int get_city(do_alias_t *alias, sklad_rec_t *sklad);
static int unit_cmp(char *s1, char *s2)
{
    return atoi(s1) - atoi(s2);
}

DO_EXPORT do_string_list_t *do_city_get_units(do_alias_t *alias, int code)
{
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;

    int status;
    do_sort_list_t *retval;

    retval = do_sort_list_new(FALSE, TRUE, (do_list_cmp_func)unit_cmp, NULL);

    do_text_set_empty(sklad_key0.code);
    status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( get_city(alias, &sklad) == code ) {
            char *unit;
            unit = do_text(alias, sklad_key0.code);
            do_sort_list_add(retval, unit);
        }

        status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        do_sort_list_free(retval);
        return NULL;
    }
    int i;
    do_string_list_t *r;
    r = do_string_list_new();
    for ( i = 0; i < retval->count; i++ )
        do_string_list_add(r, retval->list[i]);
    do_sort_list_free(retval);
    return r;
}
DO_EXPORT do_string_list_t *do_get_units(do_alias_t *alias)
{
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;

    int status;
    do_sort_list_t *retval;

    retval = do_sort_list_new(FALSE, TRUE, (do_list_cmp_func)unit_cmp, NULL);

    do_text_set_empty(sklad_key0.code);
    status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( get_city(alias, &sklad) != 0 ) {
            char *unit;
            unit = do_text(alias, sklad_key0.code);
            do_sort_list_add(retval, unit);
        }

        status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        do_string_list_free(retval);
        return NULL;
    }
    int i;
    do_string_list_t *r;
    r = do_string_list_new();
    for ( i = 0; i < retval->count; i++ )
        do_string_list_add(r, retval->list[i]);
    do_sort_list_free(retval);
    return r;
}
static int get_city(do_alias_t *alias, sklad_rec_t *sklad)
{
    partner_key0_t partner_key0;
    partner_rec_t partner;
    char *value, *ch;
    int retval;

    do_text_set(alias, partner_key0.g_code, CITY_REGION);
    if ( !do_cmp(sklad->data.p_g_code, partner_key0.g_code) )
        return sklad->data.p_code;

    do_text_set(alias, partner_key0.g_code, INSIDE_REGION);
    if ( do_cmp(sklad->data.p_g_code, partner_key0.g_code) )
        return 0;
    partner_key0.code = sklad->data.p_code;
    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_KEY_NOT_FOUND:
            do_log(LOG_ERR, "partner \"%s.%d\" not found", INSIDE_REGION, partner_key0.code);
        case DO_ERROR:
            return 0;
    };

    value = do_partner_param(alias, &partner, "CITY");

    ch = strchr(value, '.');
    if ( !ch )
        retval = 0;
    else
        retval = atoi(ch+1);
    do_free(value);
    return retval;
}

DO_EXPORT int do_city_get_city_by_unit(do_alias_t *alias, const char *unit)
{
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;

    do_text_set(alias, sklad_key0.code, unit);
    switch ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) ) {
        case DO_KEY_NOT_FOUND:
            do_log(LOG_ERR, "sklad \"%s\" not found", unit);
        case DO_ERROR:
            return 0;
    };
    return get_city(alias, &sklad);
}
DO_EXPORT do_list_t *do_city_get_cities(do_alias_t *alias)
{
    partner_key0_t partner_key0,key;
    int status;
    do_list_t *retval;

    do_text_set(alias, partner_key0.g_code, CITY_REGION);
    do_cpy(key.g_code, partner_key0.g_code);
    partner_key0.code = 0;
    retval = do_list_new(TRUE);

    status = do_partner_key0(alias, &partner_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.g_code, partner_key0.g_code) ) break;
        do_list_add_alloc(retval, &partner_key0.code, sizeof(partner_key0.code));
        status = do_partner_key0(alias, &partner_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        do_list_free(retval);
        return NULL;
    }
    return retval;
}
DO_EXPORT char *do_firm_get_name(do_alias_t *alias, int code)
{
    partner_key0_t partner_key0;
    partner_rec_t partner;

    do_text_set(alias, partner_key0.g_code, FIRM_REGION);
    partner_key0.code = code;

    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_KEY_NOT_FOUND:
            do_log(LOG_ERR, "partner %s.%d not found", FIRM_REGION, code);
        case DO_ERROR:
            return do_strdup("");
        case DO_OK:
            return do_text(alias, partner.data.name);
    }
    return do_strdup("");
}
DO_EXPORT do_string_list_t *do_firm_get_units(do_alias_t *alias, int code)
{
    char *value_str;
    partner_key0_t partner_key0, key;
    partner_rec_t partner;

    value_str = do_strdup_printf("%s.%d", FIRM_REGION, code);

    int status;
    do_sort_list_t *retval;

    retval = do_sort_list_new(FALSE, TRUE, (do_list_cmp_func)unit_cmp, NULL);

    do_text_set(alias, partner_key0.g_code, INSIDE_REGION);
    do_cpy(key.g_code, partner_key0.g_code);
    partner_key0.code = 0;

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.g_code, partner_key0.g_code) ) break;
        char *value;
        value = do_partner_param(alias, &partner, "DIVIS");
        if ( !strcmp(value,value_str) ) {
            char *unit;
            unit = do_text(alias, partner.data.sklad);
            //do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_SKLAD));
            do_sort_list_add(retval, unit);
        }
        do_free(value);
        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_NEXT);
    }
    do_free(value_str);
    if ( status == DO_ERROR ) {
        do_sort_list_free(retval);
        return NULL;
    }
    int i;
    do_string_list_t *r;
    r = do_string_list_new();
    for ( i = 0; i < retval->count; i++ )
        do_string_list_add(r, retval->list[i]);
    do_sort_list_free(retval);
    return r;
}
DO_EXPORT int do_firm_get_firm_by_unit(do_alias_t *alias, const char *unit)
{
    partner_key0_t partner_key0, key;
    partner_rec_t partner;

    int status;
    int retval;

    do_text_set(alias, partner_key0.g_code, INSIDE_REGION);
    do_cpy(key.g_code, partner_key0.g_code);
    partner_key0.code = 0;

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.g_code, partner_key0.g_code) ) break;
        char *unit1;
        unit1 = do_text(alias, partner.data.sklad);
        if ( strlen (unit1) > 0 ) {
        //_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_SKLAD));
        if ( !strcmp(unit,unit1) ) {
            char *value, *ch;
            value = do_partner_param(alias, &partner, "DIVIS");
            ch = strchr(value, '.');
            if ( !ch )
                retval = 0;
            else
                retval = atoi(ch+1);
            do_free(unit1);
            do_free(value);
            return retval;
        }
        }
        do_free(unit1);
        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_NEXT);
    }
    return 0;
}
DO_EXPORT do_list_t *do_firm_get_firms(do_alias_t *alias)
{
    partner_key0_t partner_key0, key;
    partner_rec_t partner;

    int status, i;
    do_list_t *retval;

    do_text_set(alias, partner_key0.g_code, INSIDE_REGION);
    do_cpy(key.g_code, partner_key0.g_code);
    partner_key0.code = 0;

    retval = do_list_new(TRUE);

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.g_code, partner_key0.g_code) ) break;
        char *value, *ch;
        value = do_partner_param(alias, &partner, "DIVIS");
        ch = strchr(value, '.');
        if ( ch ) {
            int firm = atoi(ch+1), flag = FALSE;
	   

            for ( i = 0; i < retval->count; i++ )
                if ( firm == *((int*)(retval->list[i])) ) {
                    flag = TRUE;
                    break;
                }
            if ( !flag )
                do_list_add_alloc(retval, &firm, sizeof(firm));
        }
        do_free(value);
        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_NEXT);
    }
    return retval;
}
DO_EXPORT char *do_get_unit_desc(do_alias_t *alias, const char *units)
{
    do_alias_t *alias_;
    if ( !alias ) {
        alias_ = domino_alias_new(NULL);
        if ( !do_alias_open(alias_, TRUE) ) {
            do_alias_free(alias_);
            return do_strdup("");
        }
    }
    else
        alias_ = alias;

    int city = 0;
    int firm = 0;
    int all = 0;
    int i,v;
    do_string_list_t *lc;

    lc = do_string_list_new_from_csv(units);

    for ( i = 0; i < lc->count; i++ ) {
        if ( city != -1 ) {
            v = do_city_get_city_by_unit(alias_, lc->list[i]);
            if ( !city )
                city = v;
            else
            if ( city != v )
                city = -1;
        }
        if ( firm != -1 ) {
            v = do_firm_get_firm_by_unit(alias_, lc->list[i]);
            if ( !firm )
                firm = v;
            else
            if ( firm != v )
                firm = -1;
        }
    }
    if ( city <= 0 && firm <= 0 ) {
        do_string_list_t *all1;
        int j, flag;
        all1 = do_get_units(alias_);
        if ( lc->count == all1->count ) {
            all = TRUE;
            for ( i = 0; i < lc->count; i++ ) {
                flag = TRUE;
                for ( j = 0; j < all1->count; j++ )
                    if ( !strcmp(lc->list[i], all1->list[j]) ) {
                        flag = FALSE;
                        do_string_list_delete(all1, j);
                        break;
                    }
                if ( flag ) {
                    all = FALSE;
                    break;
                }

            }
            all = all && !all1->count;
        }
        do_string_list_free(all1);
    }
    do_string_list_free(lc);
    char *retval;
    if ( all )
        retval = do_strdup("Все");
    else
    if ( firm > 0 )
        retval = do_firm_get_name(alias_, firm);
    else
    if ( city > 0 )
        retval = do_city_get_name(alias_, city);
    else
        retval = do_strdup(units);

    if ( !alias )
        do_alias_free(alias_);
    return retval;
}
