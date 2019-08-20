#include <domino.h>
#include "../misc/define.h"

#ifdef EXPAT

static do_list_t *do_report_get_parcel(do_alias_t *alias, const char *code, int all_parcel, const char *sklad)
{
    product_key4_t product_key4, key;
    int len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);
    char *pref;
    do_list_t *retval;
    int status;
    retval = do_list_new(TRUE);

    if ( !all_parcel ) {
        do_text_set(alias, product_key4.code, code);
        do_list_add_alloc(retval, &product_key4, sizeof(product_key4));
    }
    else {

        do_text_set(alias, product_key4.code, code);
        do_list_add_alloc(retval, &product_key4, sizeof(product_key4));
        pref = domino_get_unit_prefix(alias, sklad);
        do_alias_text_set_(alias, product_key4.code + len, pref, sizeof(product_key4.code) - len);
        len += strlen(pref);
        do_free(pref);

        memcpy(&key.code, &product_key4.code, len);
        status = do_product_key4(alias, &product_key4, DO_GET_GE);

        while ( status == DO_OK ) {
            if ( memcmp(&key.code, &product_key4.code, len) ) break;
            do_list_add_alloc(retval, &product_key4, sizeof(product_key4));
            status = do_product_key4(alias, &product_key4, DO_GET_NEXT);
        }
        if ( status == DO_ERROR ) {
            do_list_free(retval);
            return NULL;
        }
    }
    return retval;
}
static int do_report_dtype1_cmp(do_list_t *s1, void *s2)
{
    int i;
    for ( i = 0; i < s1->count; i++ )
        if ( !memcmp(s2, s1->list[i], do_sizeof(document_struct_t, dtype1)) )
            return FALSE;
    return TRUE;
}
DO_EXPORT int do_report_product_traffic(do_alias_t *alias,
                                            const char *code,
                                            int all_parcel,
                                            const char *dtype,
                                            do_string_list_t *dtype1,
                                            const char *sklad,
                                            struct tm *date_b,
                                            struct tm *date_e,
                                            int quant_n,
                                            int price_n,
                                            double *quant,
                                            double *sum)
{
    document_order_key3_t document_order_key3;
    document_key0_t document_key0;
    do_extended_t *extended;
    do_ret_list_t *list;
    int i, j, indx;
    double p, q;
    do_list_t *parcels, *dtypes1;
    int retval;
    struct PACK_STRUCT {
        BTI_CHAR dtype[2];
        BTI_CHAR dtype1[10];
        BTI_CHAR sklad[10];
        BTI_BYTE quant[4][7];
        BTI_SINT nquant[4];
        BTI_REAL price[8];
        BTI_SINT nprice[8];
        BTI_BYTE accepted;
    } *ret;

    parcels = do_report_get_parcel(alias, code, all_parcel, sklad);
    if ( !parcels )
        return FALSE;
    dtypes1 = do_list_new(TRUE);

    for ( i = 0; i < dtype1->count; i++ ) {
        BTI_CHAR dtype1a[10];
        do_text_set(alias, dtype1a, (char*)dtype1->list[i]);
        do_list_add_alloc(dtypes1, &dtype1a, sizeof(dtype1a));
    }

    do_text_set(alias, document_key0.dtype, dtype);
    do_text_set(alias, document_key0.sklad, sklad);
    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);

    do_extended_set_filter(extended, TRUE, 2, sizeof(document_order_key3.code),
                                              sizeof(document_order_key3.date));

    do_extended_set_filter_term(extended, 0,
                                      DO_EXTENDED_TYPE_STRING,
                                      offsetof(document_order_struct_t, code),
                                      DO_EXTENDED_COMP_EQUAL,
                                      DO_EXTENDED_CONNECTION_AND,
                                      &document_order_key3.code);
    do_extended_set_filter_term(extended, 1,
                                      DO_EXTENDED_TYPE_INTEGER,
                                      offsetof(document_order_struct_t, date),
                                      DO_EXTENDED_COMP_LE,
                                      DO_EXTENDED_CONNECTION_NO,
                                      &document_order_key3.date);
    do_extended_set_fields(extended, 8,
                do_size_and_offset(document_order_struct_t, dtype),
                do_size_and_offset(document_order_struct_t, dtype1),
                do_size_and_offset(document_order_struct_t, sklad),
                do_size_and_offset(document_order_struct_t, quant),
                do_size_and_offset(document_order_struct_t, nquant),
                do_size_and_offset(document_order_struct_t, price),
                do_size_and_offset(document_order_struct_t, nprice),
                do_size_and_offset(document_order_struct_t, accepted));

    retval = TRUE;
    *sum = 0;
    *quant = 0;
    for ( indx = 0; indx < parcels->count; indx++ ) {
        product_key4_t *product_key4;
        product_key4 = parcels->list[indx];

        do_cpy(document_order_key3.code, product_key4->code);
        do_date_set(&document_order_key3.date, *date_e);
        do_extended_set_filter_term_set_value(extended, 0, &document_order_key3.code);
        do_extended_set_filter_term_set_value(extended, 1, &document_order_key3.date);

        do_date_set(&document_order_key3.date, *date_b);
        document_order_key3.time = 0;
        do_extended_set_key(extended, 3, &document_order_key3);


        list = do_extended_get(extended, -1);
        if ( !list ) {
            retval = FALSE;
            break;
        }

        for ( i = 0; i < do_ret_list_count(list); i++ ) {
            int ignore;
            ret = do_ret_list_item_data(list, i);
            if ( !ret->accepted ||
                 do_cmp(ret->dtype, document_key0.dtype) ||
                 do_cmp(ret->sklad, document_key0.sklad) ||
                 do_report_dtype1_cmp(dtypes1, ret->dtype1 )
                ) continue;

            q = 0;
            p = 0;
            for ( j = 0; j < 8; j++ )
                if ( ret->nprice[j] == price_n ) {
                    p = ret->price[j];
                    break;
                }
            ignore = FALSE;
            for ( j = 0; j < 4; j++ ) {
                if ( ret->nquant[j] == DO_CONST_QUANT_IGNORE ) {
                    ignore = TRUE;
                    break;
                }
                if ( ret->nquant[j] == quant_n ) {
                    q = do_pdecimal(ret->quant[j], 7, 3);
                }
            }
            if ( !ignore ) {
                (*quant) += q;
                (*sum) += p*q;
            }
        }
        do_ret_list_free(list);
    }
    do_extended_free(extended);
    do_list_free(parcels);
    do_list_free(dtypes1);
    return retval;
}
DO_EXPORT int do_report_product_traffic_partner(do_alias_t *alias,
                                            const char *code,
                                            int all_parcel,
                                            const char *dtype,
                                            do_string_list_t *dtype1,
                                            const char *sklad,
                                            struct tm *date_b,
                                            struct tm *date_e,
                                            int quant_n,
                                            int price_n,
                                            double *quant,
                                            double *sum,
                                            partner_key0_t *partner_key0)
{
    document_order_key3_t document_order_key3;
    document_key0_t document_key0;
    document_rec_t document;
    do_extended_t *extended;
    do_ret_list_t *list;
    int i, j, indx;
    double p, q;
    do_list_t *parcels, *dtypes1;
    int retval;
    struct PACK_STRUCT {
        BTI_CHAR dtype[2];
        BTI_CHAR dtype1[10];
        BTI_CHAR sklad[10];
        BTI_CHAR document[10];
        BTI_BYTE quant[4][7];
        BTI_SINT nquant[4];
        BTI_REAL price[8];
        BTI_SINT nprice[8];
        BTI_BYTE accepted;
    } *ret;

    parcels = do_report_get_parcel(alias, code, all_parcel, sklad);
    if ( !parcels )
        return FALSE;
    dtypes1 = do_list_new(TRUE);

    for ( i = 0; i < dtype1->count; i++ ) {
        BTI_CHAR dtype1a[10];
        do_text_set(alias, dtype1a, (char*)dtype1->list[i]);
        do_list_add_alloc(dtypes1, &dtype1a, sizeof(dtype1a));
    }

    do_text_set(alias, document_key0.dtype, dtype);
    do_text_set(alias, document_key0.sklad, sklad);
    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);

    do_extended_set_filter(extended, TRUE, 2, sizeof(document_order_key3.code),
                                              sizeof(document_order_key3.date));

    do_extended_set_filter_term(extended, 0,
                                      DO_EXTENDED_TYPE_STRING,
                                      offsetof(document_order_struct_t, code),
                                      DO_EXTENDED_COMP_EQUAL,
                                      DO_EXTENDED_CONNECTION_AND,
                                      &document_order_key3.code);
    do_extended_set_filter_term(extended, 1,
                                      DO_EXTENDED_TYPE_INTEGER,
                                      offsetof(document_order_struct_t, date),
                                      DO_EXTENDED_COMP_LE,
                                      DO_EXTENDED_CONNECTION_NO,
                                      &document_order_key3.date);
    do_extended_set_fields(extended, 8,
                do_size_and_offset(document_order_struct_t, dtype),
                do_size_and_offset(document_order_struct_t, dtype1),
                do_size_and_offset(document_order_struct_t, sklad),
                do_size_and_offset(document_order_struct_t, document),
                do_size_and_offset(document_order_struct_t, quant),
                do_size_and_offset(document_order_struct_t, nquant),
                do_size_and_offset(document_order_struct_t, price),
                do_size_and_offset(document_order_struct_t, nprice),
                do_size_and_offset(document_order_struct_t, accepted));

    retval = TRUE;
    *sum = 0;
    *quant = 0;
    for ( indx = 0; indx < parcels->count; indx++ ) {
        product_key4_t *product_key4;
        product_key4 = parcels->list[indx];

        do_cpy(document_order_key3.code, product_key4->code);
        do_date_set(&document_order_key3.date, *date_e);
        do_extended_set_filter_term_set_value(extended, 0, &document_order_key3.code);
        do_extended_set_filter_term_set_value(extended, 1, &document_order_key3.date);

        do_date_set(&document_order_key3.date, *date_b);
        document_order_key3.time = 0;
        do_extended_set_key(extended, 3, &document_order_key3);


        list = do_extended_get(extended, -1);
        if ( !list ) {
            retval = FALSE;
            break;
        }

        for ( i = 0; i < do_ret_list_count(list); i++ ) {
            int ignore;
            ret = do_ret_list_item_data(list, i);
            if ( !ret->accepted ||
                 do_cmp(ret->dtype, document_key0.dtype) ||
                 do_cmp(ret->sklad, document_key0.sklad) ||
                 do_report_dtype1_cmp(dtypes1, ret->dtype1 )
                ) continue;
            do_cpy(document_key0.document, ret->document);
            if ( do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL) != DO_OK ) {
                retval = FALSE;
                break;
            }
            if ( do_cmp(document.data.p_g_code, partner_key0->g_code) ||
                 document.data.p_code != partner_key0->code ) continue;

            q = 0;
            p = 0;
            for ( j = 0; j < 8; j++ )
                if ( ret->nprice[j] == price_n ) {
                    p = ret->price[j];
                    break;
                }
            ignore = FALSE;
            for ( j = 0; j < 4; j++ ) {
                if ( ret->nquant[j] == DO_CONST_QUANT_IGNORE ) {
                    ignore = TRUE;
                    break;
                }
                if ( ret->nquant[j] == quant_n ) {
                    q = do_pdecimal(ret->quant[j], 7, 3);
                }
            }
            if ( !ignore ) {
                (*quant) += q;
                (*sum) += p*q;
            }
        }
        do_ret_list_free(list);
    }
    do_extended_free(extended);
    do_list_free(parcels);
    do_list_free(dtypes1);
    return retval;
}

DO_EXPORT int do_report_product_meta_traffic(do_meta_alias_t *meta,
                                            const char *code,
                                            int all_parcel,
                                            const char *dtype,
                                            do_string_list_t *dtype1,
                                            const char *sklad,
                                            struct tm *date_b,
                                            struct tm *date_e,
                                            int quant_n,
                                            int price_n,
                                            double *quant,
                                            double *sum)
{
    do_sort_list_t *aliases;
    double sum_res, quant_res;
    int res;
    int i;
    *sum = 0; *quant = 0;
    aliases = domino_meta_alias_get_list_from_sklad(meta, sklad, date_b, date_e);
    if ( !aliases )
        return FALSE;
    res = TRUE;
    for ( i = 0; i < aliases->count; i++ ) {
        do_alias_period_t *sk;
        sk = aliases->list[i];
        if ( !do_alias_conected(sk->alias) &&
            !do_alias_open(sk->alias, FALSE, DO_DB_SKLAD, DO_DB_DOCUMENT_ORDER, DO_DB_PRODUCT, DO_DB_END) ) {
            res = FALSE;
            break;
        }
        if ( !do_report_product_traffic(sk->alias,
                                            code,
                                            all_parcel,
                                            dtype,
                                            dtype1,
                                            sklad,
                                            &sk->date_b,
                                            &sk->date_e,
                                            quant_n,
                                            price_n,
                                            &quant_res,
                                            &sum_res) ) {
            res = FALSE;
            break;
        }
        (*sum) += sum_res;
        (*quant) += quant_res;
    }
    do_list_free(aliases);
    return res;
}
DO_EXPORT int do_report_product_meta_traffic_partner(do_meta_alias_t *meta,
                                            const char *code,
                                            int all_parcel,
                                            const char *dtype,
                                            do_string_list_t *dtype1,
                                            const char *sklad,
                                            struct tm *date_b,
                                            struct tm *date_e,
                                            int quant_n,
                                            int price_n,
                                            double *quant,
                                            double *sum,
                                            partner_key0_t *partner_key0)
{
    do_sort_list_t *aliases;
    double sum_res, quant_res;
    int res;
    int i;
    *sum = 0; *quant = 0;
    aliases = domino_meta_alias_get_list_from_sklad(meta, sklad, date_b, date_e);
    if ( !aliases )
        return FALSE;
    res = TRUE;
    for ( i = 0; i < aliases->count; i++ ) {
        do_alias_period_t *sk;
        sk = aliases->list[i];
        if ( !do_alias_conected(sk->alias) &&
            !do_alias_open(sk->alias, FALSE, DO_DB_SKLAD, DO_DB_DOCUMENT_ORDER, DO_DB_PRODUCT, DO_DB_DOCUMENT, DO_DB_END) ) {
            res = FALSE;
            break;
        }
        if ( !do_report_product_traffic_partner(sk->alias,
                                            code,
                                            all_parcel,
                                            dtype,
                                            dtype1,
                                            sklad,
                                            &sk->date_b,
                                            &sk->date_e,
                                            quant_n,
                                            price_n,
                                            &quant_res,
                                            &sum_res,
                                            partner_key0) ) {
            res = FALSE;
            break;
        }
        (*sum) += sum_res;
        (*quant) += quant_res;
    }
    do_list_free(aliases);
    return res;
}
struct _do_report_product_traffic_detail_t {
    do_list_t *document;
    do_list_t *partner;
};

typedef struct {
    char     *sklad;
    char     *dtype;
    char     *document;
    char     *dtype1;
    struct tm time;
    char     *p_document;
    struct tm p_time;
    char     *p_g_code;
    int       p_code;
    do_list_t *line;
} ret_document_t;

typedef struct {
    char *g_code;
    int   code;
    char *name;
} ret_partner_t;

typedef struct {
    double quant;
    double price;
} ret_value_t;

DO_EXPORT void do_report_product_traffic_detail_free(do_report_product_traffic_detail_t *report)
{
    ret_partner_t  *par;
    ret_document_t *doc;
    int i;
    do_list_t *list;

    list = report->partner;
    for ( i=0; i < list->count; i++ ) {
        par = list->list[i];
        do_free(par->name);
        do_free(par->g_code);
    }
    do_list_free(list);
    list = report->document;
    for ( i=0; i < list->count; i++ ) {
        doc = list->list[i];
        do_free(doc->sklad);
        do_free(doc->dtype);
        do_free(doc->document);
        do_free(doc->dtype1);
        do_free(doc->p_document);
        do_free(doc->p_g_code);
        do_free(doc->line);
    }
    do_list_free(list);
    do_free(report);
}
DO_EXPORT int do_report_product_traffic_document_count(do_report_product_traffic_detail_t *report)
{
    return report->document->count;
}
DO_EXPORT int do_report_product_traffic_partner_count(do_report_product_traffic_detail_t *report)
{
    return report->partner->count;
}
DO_EXPORT int do_report_product_traffic_document_for_partner_count(do_report_product_traffic_detail_t *report, int par_indx)
{
    ret_partner_t *par;
    int i, res;

    par = report->partner->list[par_indx];
    res = 0;
    for ( i = 0; i < report->document->count; i++ ) {
        ret_document_t *doc;
        doc = report->document->list[i];
        if ( !strcmp(doc->p_g_code, par->g_code) &&
             doc->p_code == par->code ) res++;
    }
    return res;
}
DO_EXPORT int do_report_product_traffic_document_for_partner_document(do_report_product_traffic_detail_t *report, int par_indx, int indx)
{
    ret_partner_t *par;
    int i, res;

    par = report->partner->list[par_indx];
    res = 0;
    for ( i = 0; i < report->document->count; i++ ) {
        ret_document_t *doc;
        doc = report->document->list[i];
        if ( !strcmp(doc->p_g_code, par->g_code) &&
             doc->p_code == par->code ) {
             if ( res == indx )
                return i;
             res++;
        }
    }
    return 0;

}

DO_EXPORT const char *do_report_product_traffic_partner_name(do_report_product_traffic_detail_t *report, int par_indx)
{
    return ((ret_partner_t*)report->partner->list[par_indx])->name;
}
DO_EXPORT const char *do_report_product_traffic_document_p_doc(do_report_product_traffic_detail_t *report, int doc_indx)
{
    return ((ret_document_t*)report->document->list[doc_indx])->p_document;
}
DO_EXPORT struct tm *do_report_product_traffic_document_p_time(do_report_product_traffic_detail_t *report, int doc_indx)
{
    return &((ret_document_t*)report->document->list[doc_indx])->p_time;
}
DO_EXPORT const char *do_report_product_traffic_document_sklad(do_report_product_traffic_detail_t *report, int doc_indx)
{
    return ((ret_document_t*)report->document->list[doc_indx])->sklad;
}
DO_EXPORT int do_report_product_traffic_document_line_count(do_report_product_traffic_detail_t *report, int doc_indx)
{
    return ((ret_document_t*)report->document->list[doc_indx])->line->count;
}
DO_EXPORT double do_report_product_traffic_document_line_quant(do_report_product_traffic_detail_t *report, int doc_indx, int line_indx)
{
    return ((ret_value_t*)((ret_document_t*)report->document->list[doc_indx])->line->list[line_indx])->quant;
}

static int do_list_cmp_partner(ret_partner_t *s1, ret_partner_t *s2)
{
    int res;
    res = strcmp(s1->g_code,s2->g_code);
    if ( !res )
        res = s1->code - s2->code;
    return res;
}
static int do_list_cmp_document(ret_document_t *s1, ret_document_t *s2)
{
    int res;
    res = strcmp(s1->sklad,s2->sklad);
    if ( !res )
        res = strcmp(s1->dtype,s2->dtype);
    if ( !res )
        res = strcmp(s1->document,s2->document);
    return res;
}

static do_report_product_traffic_detail_t *do_report_product_traffic_detail_new()
{
    do_report_product_traffic_detail_t *report;
    report = do_malloc(sizeof(do_report_product_traffic_detail_t));
    report->partner = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)do_list_cmp_partner,NULL);
    report->document = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)do_list_cmp_document,NULL);
    return report;
}
static int add_document_traffic(do_alias_t *alias, do_report_product_traffic_detail_t *report, const char *sklad, const char *dtype, const char *doc, double quant, double price)
{
    ret_document_t doc_rec, *pdoc;
    int indx;

    doc_rec.sklad = (char*)sklad;
    doc_rec.dtype = (char*)dtype;
    doc_rec.document = (char*)doc;
    if ( !do_sort_list_find(report->document, &indx, &doc_rec) ) {
        document_rec_t document;
        document_key0_t document_key0;
        ret_partner_t par_rec;
        char *value;

        do_text_set(alias, document_key0.sklad, sklad);
        do_text_set(alias, document_key0.dtype, dtype);
        do_text_set(alias, document_key0.document, doc);

        switch (do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL)) {
            case DO_OK:
                break;
            case DO_KEY_NOT_FOUND:
                value = do_util_document_key0_to_str(alias, &document_key0);
                do_log(LOG_ERR, "document \"%s\" not found", value);
                do_free(value);
                return FALSE;
            default:
                return FALSE;
        }
        doc_rec.sklad = do_strdup(sklad);
        doc_rec.dtype = do_strdup(dtype);
        doc_rec.document = do_strdup(doc);
        doc_rec.dtype1 = do_text(alias, document.data.dtype1);
        do_date(document.data.date, &doc_rec.time);
        do_time(document.data.time, &doc_rec.time);
        doc_rec.p_document = do_text(alias, document.data.p_doc);
        do_date(document.data.p_date, &doc_rec.p_time);
        do_time(document.data.p_time, &doc_rec.p_time);
        doc_rec.p_g_code = do_text(alias, document.data.p_g_code);
        doc_rec.p_code = document.data.p_code;
        doc_rec.line = do_list_new(TRUE);
        do_sort_list_insert_new(report->document, indx, &doc_rec, sizeof(doc_rec));
        pdoc = report->document->list[indx];

        par_rec.g_code = doc_rec.p_g_code;
        par_rec.code = doc_rec.p_code;

        if ( !do_sort_list_find(report->partner, &indx, &par_rec) ) {
            partner_rec_t partner;
            partner_key0_t partner_key0;

            do_text_set(alias, partner_key0.g_code, par_rec.g_code);
            partner_key0.code = par_rec.code;
            switch (do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL)) {
                case DO_OK:
                    break;
                case DO_KEY_NOT_FOUND:
                    value = do_util_partner_key0_to_str(alias, &partner_key0);
                    do_log(LOG_ERR, "partner \"%s\" not found", value);
                    do_free(value);
                    return FALSE;
                default:
                    return FALSE;
            }
            par_rec.g_code = do_strdup(doc_rec.p_g_code);
            par_rec.name = do_text(alias, partner.data.name);
            do_sort_list_insert_new(report->partner, indx, &par_rec, sizeof(par_rec));
        }
    }
    else
        pdoc = report->document->list[indx];
    ret_value_t rec_value;
    rec_value.quant = quant;
    rec_value.price = price;
    do_list_add_alloc(pdoc->line, &rec_value, sizeof(rec_value));
    return TRUE;
}
DO_EXPORT do_report_product_traffic_detail_t *do_report_product_traffic_detail(do_alias_t *alias,
                                            const char *code,
                                            int all_parcel,
                                            const char *dtype,
                                            do_string_list_t *dtype1,
                                            const char *sklad,
                                            struct tm *date_b,
                                            struct tm *date_e,
                                            int quant_n,
                                            int price_n,
                                            do_report_product_traffic_detail_t *val)
{
    document_order_key3_t document_order_key3;
    document_key0_t document_key0;
    do_extended_t *extended;
    do_report_product_traffic_detail_t *report;
    do_ret_list_t *list;
    int i, j, indx;
    double p, q;
    do_list_t *parcels, *dtypes1;
    char *doc;
    //char *sklad, *dtype;
    struct PACK_STRUCT {
        BTI_CHAR dtype[2];
        BTI_CHAR dtype1[10];
        BTI_CHAR sklad[10];
        BTI_CHAR document[10];
        BTI_BYTE quant[4][7];
        BTI_SINT nquant[4];
        BTI_REAL price[8];
        BTI_SINT nprice[8];
        BTI_BYTE accepted;
    } *ret;

    parcels = do_report_get_parcel(alias, code, all_parcel, sklad);
    if ( !parcels )
        return NULL;
    dtypes1 = do_list_new(TRUE);
    if ( !val )
        report = do_report_product_traffic_detail_new();
    else
        report = val;
    for ( i = 0; i < dtype1->count; i++ ) {
        BTI_CHAR dtype1a[10];
        do_text_set(alias, dtype1a, (char*)dtype1->list[i]);
        do_list_add_alloc(dtypes1, &dtype1a, sizeof(dtype1a));
    }

    do_text_set(alias, document_key0.dtype, dtype);
    do_text_set(alias, document_key0.sklad, sklad);
    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);

    do_extended_set_filter(extended, TRUE, 2, sizeof(document_order_key3.code),
                                              sizeof(document_order_key3.date));

    do_extended_set_filter_term(extended, 0,
                                      DO_EXTENDED_TYPE_STRING,
                                      offsetof(document_order_struct_t, code),
                                      DO_EXTENDED_COMP_EQUAL,
                                      DO_EXTENDED_CONNECTION_AND,
                                      &document_order_key3.code);
    do_extended_set_filter_term(extended, 1,
                                      DO_EXTENDED_TYPE_INTEGER,
                                      offsetof(document_order_struct_t, date),
                                      DO_EXTENDED_COMP_LE,
                                      DO_EXTENDED_CONNECTION_NO,
                                      &document_order_key3.date);
    do_extended_set_fields(extended, 9,
                do_size_and_offset(document_order_struct_t, dtype),
                do_size_and_offset(document_order_struct_t, dtype1),
                do_size_and_offset(document_order_struct_t, sklad),
                do_size_and_offset(document_order_struct_t, document),
                do_size_and_offset(document_order_struct_t, quant),
                do_size_and_offset(document_order_struct_t, nquant),
                do_size_and_offset(document_order_struct_t, price),
                do_size_and_offset(document_order_struct_t, nprice),
                do_size_and_offset(document_order_struct_t, accepted));

    for ( indx = 0; indx < parcels->count && report; indx++ ) {
        product_key4_t *product_key4;
        product_key4 = parcels->list[indx];

        do_cpy(document_order_key3.code, product_key4->code);
        do_date_set(&document_order_key3.date, *date_e);
        do_extended_set_filter_term_set_value(extended, 0, &document_order_key3.code);
        do_extended_set_filter_term_set_value(extended, 1, &document_order_key3.date);

        do_date_set(&document_order_key3.date, *date_b);
        document_order_key3.time = 0;
        do_extended_set_key(extended, 3, &document_order_key3);


        list = do_extended_get(extended, -1);
        if ( !list ) {
            do_report_product_traffic_detail_free(report);
            report = NULL;
            break;
        }

        for ( i = 0; i < do_ret_list_count(list); i++ ) {
            int ignore;
            ret = do_ret_list_item_data(list, i);
            if ( !ret->accepted ||
                 do_cmp(ret->dtype, document_key0.dtype) ||
                 do_cmp(ret->sklad, document_key0.sklad) ||
                 do_report_dtype1_cmp(dtypes1, ret->dtype1 )
                ) continue;

            q = 0;
            p = 0;
            for ( j = 0; j < 8; j++ )
                if ( ret->nprice[j] == price_n ) {
                    p = ret->price[j];
                    break;
                }
            ignore = FALSE;
            for ( j = 0; j < 4; j++ ) {
                if ( ret->nquant[j] == DO_CONST_QUANT_IGNORE ) {
                    ignore = TRUE;
                    break;
                }
                if ( ret->nquant[j] == quant_n ) {
                    q = do_pdecimal(ret->quant[j], 7, 3);
                }
            }
            if ( !ignore ) {
                //sklad = do_text(alias, ret->sklad);
                //dtype = do_text(alias, ret->dtype);
                doc = do_text(alias, ret->document);
                if ( !add_document_traffic(alias, report, sklad, dtype, doc, q, p) ) {
                    do_report_product_traffic_detail_free(report);
                    report = NULL;
                    break;
                }
                do_free(doc);
            }
        }
        do_ret_list_free(list);
    }
    do_extended_free(extended);
    do_list_free(parcels);
    do_list_free(dtypes1);
    return report;
}
DO_EXPORT do_report_product_traffic_detail_t *do_report_product_meta_traffic_detail(do_meta_alias_t *meta,
                                            const char *code,
                                            int all_parcel,
                                            const char *dtype,
                                            do_string_list_t *dtype1,
                                            const char *sklad,
                                            struct tm *date_b,
                                            struct tm *date_e,
                                            int quant_n,
                                            int price_n,
                                            do_report_product_traffic_detail_t *val)
{
    do_sort_list_t *aliases;
    do_report_product_traffic_detail_t *report;
    int i;
    aliases = domino_meta_alias_get_list_from_sklad(meta, sklad, date_b, date_e);
    if ( !aliases )
        return NULL;
    report = val;
    for ( i = 0; i < aliases->count; i++ ) {
        do_alias_period_t *sk;
        sk = aliases->list[i];
        if ( !do_alias_conected(sk->alias) &&
            !do_alias_open(sk->alias, FALSE, DO_DB_SKLAD, DO_DB_DOCUMENT_ORDER, DO_DB_PARTNER, DO_DB_DOCUMENT, DO_DB_PRODUCT, DO_DB_END) ) {
            if ( report )
                do_report_product_traffic_detail_free(report);
            report = NULL;
            break;
        }
        if ( (report = do_report_product_traffic_detail(sk->alias,
                                            code,
                                            all_parcel,
                                            dtype,
                                            dtype1,
                                            sklad,
                                            &sk->date_b,
                                            &sk->date_e,
                                            quant_n,
                                            price_n,
                                            report)) == NULL) {
            report = NULL;
            break;
        }
    }
    do_list_free(aliases);
    return report;
}
#endif
DO_EXPORT void do_rest_free(do_rest_t *rest)
{
    do_free(rest->code);
    do_free(rest);
};
DO_EXPORT void do_rest_list_free(do_sort_list_t *list)
{
    do_list_foreach(list, (do_list_foreach_func)do_rest_free, NULL);
    do_list_free(list);
}
static int do_rest_cmp(do_rest_t *i1, do_rest_t *i2)
{
    return strcmp(i1->code,i2->code);
}
DO_EXPORT do_list_t *do_report_get_rest(do_alias_t *alias, const char *unit)
{
    stock_key0_t stock_key0;
    stock_key1_t stock_key1;
    do_extended_t *extended;
    do_ret_list_t *list;
    int i, j;
    do_sort_list_t *retval;
    struct PACK_STRUCT {
        BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
        BTI_BYTE quant[6][8];
        BTI_SINT nquant[6];
    } *ret;
    do_rest_t rest, *crnt;

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_STOCK);

    if ( unit ) {
        do_text_set(alias, stock_key0.sklad, unit);
        do_text_set_empty(stock_key0.code);


        do_extended_set_filter(extended, TRUE, 1, sizeof(stock_key0.sklad));

        do_extended_set_filter_term(extended, 0,
                                      DO_EXTENDED_TYPE_STRING,
                                      offsetof(stock_struct_t, sklad),
                                      DO_EXTENDED_COMP_EQUAL,
                                      DO_EXTENDED_CONNECTION_NO,
                                      &stock_key0.sklad);
        do_extended_set_key(extended, 0 , &stock_key0);
    }
    else {

        do_text_set_empty(stock_key1.code);

        do_extended_set_key(extended, 1 , &stock_key1);
    }
    do_extended_set_fields(extended, 3,
                do_size_and_offset(stock_struct_t, code),
                do_size_and_offset(stock_struct_t, quant),
                do_size_and_offset(stock_struct_t, nquant)
                );

    list = do_extended_get(extended, -1);
    if ( !list ) {
         do_extended_free(extended);
         return NULL;
    }
    retval = do_sort_list_new(FALSE, TRUE, (do_list_cmp_func)do_rest_cmp, NULL);
    rest.quant[0] = 0;
    rest.quant[1] = 0;

    for ( i = 0; i < do_ret_list_count(list); i++ ) {
         ret = do_ret_list_item_data(list, i);
         if ( i &&
              !strncmp(ret->code, ((do_rest_t*)retval->list[retval->count-1])->code, 5) ) {
            crnt = retval->list[retval->count-1];
         }
         else {
            int indx;
            rest.code = do_text(alias, ret->code);
            rest.code[5] = '\0';
            indx = do_sort_list_add_alloc(retval, &rest, sizeof(rest));
            crnt = retval->list[indx];
         }
         for ( j = 0; j < 6; j++ ) {
            if ( ret->nquant[j] == 1 )
                crnt->quant[0] += do_pdecimal(ret->quant[j], 8, 3);
            if ( ret->nquant[j] == 2 )
                crnt->quant[1] += do_pdecimal(ret->quant[j], 8, 3);
         }
    }
    do_extended_free(extended);
    do_ret_list_free(list);
    return retval;
}
