#include <domino.h>
#include <time.h>
#include <locale.h>
#include <math.h>

BTI_LONG start_date, end_date;

void set_date()
{
    struct tm tm;
    do_atodate("01/11/2010", &tm);
    do_date_set(&start_date, tm);
    do_atodate("31/06/2011", &tm);
    do_date_set(&end_date, tm);
}
typedef struct {
    int year;
    int month;
    int class_;
    int unit;
    int64_t sum_sale;
    int64_t sum_purchase;
} item_t;
do_sort_list_t *report;
static int report_cmp_func(item_t *i1, item_t *i2)
{
    int res;
    res = i1->year - i2->year;
    if ( res ) return res;
    res = i1->month - i2->month;
    if ( res ) return res;
    res = i1->unit - i2->unit;
    if ( res ) return res;
    res = i1->class_ - i2->class_;
    return res;
}

static int get_sk(do_alias_t *alias, int sklad, int unit);
static char *al[] = {"local.10", "network", "local.tta", "local.chel"};
int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();
    do_alias_t *alias;
    int i;
    report = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)report_cmp_func, NULL);

    if ( !domino_init(NULL, FALSE, DO_PARAM_END) )
        exit(1);
    int indx;
    for ( indx=0; indx < 1; indx++ ) {
    if ( (alias = domino_alias_new(al[indx])) == NULL )
        exit(1);
    if ( !do_alias_open(alias, TRUE) )
        exit(1);
    set_date();

    for ( i = 1; i < 16; i++ ) {
        printf("Склад %d\n", i);
        if ( !get_sk(alias, i, indx) )
            exit(1);
    }
    }
    int m;
    int u;
    item_t *it,item;

    for ( m = 0; m < 18; m++ ) {
        item.year = 2010 + m / 12;
        item.month = 1+ m % 12;
        fprintf(stderr,"%d-%d;", item.year, item.month);
        for ( u = 2; u < 5; u++) {
            item.unit = u;
            if ( u == 14 && item.year == 2010 && item.month < 4) continue;
            if ( u == 15 && item.year == 2010 && item.month < 7) continue;
            fprintf(stderr,";");
        }
    }
    fprintf(stderr,"\n");
    for ( m = 0; m < 18; m++ ) {
        item.year = 2010 + m / 12;
        item.month = 1+ m % 12;
        for ( u = 1; u < 5; u++) {
            item.unit = u;
            if ( u == 14 && item.year == 2010 && item.month < 4) continue;
            if ( u == 15 && item.year == 2010 && item.month < 7) continue;
            fprintf(stderr,"Аптека %d;", u);
        }
    }
    fprintf(stderr,"\n");
    for ( i = 0; i < 4; i++ ) {
        for ( m = 0; m < 18; m++ ) {
            item.year = 2010 + m / 12;
            item.month = 1+ m % 12;
            for ( u = 1; u < 5; u++) {
                item.unit = u;
                item.class_ = (i % 2) ? 2 : 1;
                if ( u == 14 && item.year == 2010 && item.month < 4) continue;
                if ( u == 15 && item.year == 2010 && item.month < 7) continue;
                if ( do_sort_list_find(report, &indx, &item) ) {
                    it = report->list[indx];
                    double sum;
                    if ( i > 1 )
                        sum = it->sum_sale - it->sum_purchase;
                    else
                        sum = it->sum_sale;
                    fprintf(stderr,"%f;",sum/100.);
                }
                else
                    fprintf(stderr,";");

            }
        }
        fprintf(stderr,"\n");
    }

    do_alias_free(alias);
    return 0;
}
static int get_doc(do_alias_t *alias, document_rec_t *doc, int reverse, int sklad);
static int get_sk(do_alias_t *alias, int sklad, int unit)
{
    document_key3_t document_key3,key;
    document_rec_t document;
    int indx;
    int status;
    char sk[10];
    sprintf(sk, "%d", sklad);

    for ( indx = 0; indx < 2; indx++ ) {
        do_text_set(alias, document_key3.dtype, ( indx ) ? "ПТ" : "РТ");
        do_text_set(alias, document_key3.sklad, sk);
        document_key3.date = start_date;
        document_key3.time = 0;
        do_cpy(key.dtype,document_key3.dtype);
        do_cpy(key.sklad,document_key3.sklad);

        status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
        while ( status == DO_OK ) {
            if ( do_cmp(key.dtype,document_key3.dtype) ||
                 do_cmp(key.sklad,document_key3.sklad) ||
                 document_key3.date > end_date ) break;
            if ( (indx && !strncmp(document.data.dtype1, "20", 2)) ||
                 (!indx && (document.data.dtype1[0] == '0') && (document.data.dtype1[1] < '5') ) ) {
                if ( !get_doc(alias, &document, indx, unit) )
                    return FALSE;
            }
            status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
        }

    }
    return TRUE;

}
typedef struct PACK_STRUCT
{
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    BTI_BYTE quant[4][7];
    BTI_SINT nquant[4];
    BTI_REAL price[8];
    BTI_SINT nprice[8];
    BTI_BYTE accepted;
} ret_t;

static do_ret_list_t *get_order(do_alias_t *alias, document_rec_t *doc);
static int get_class(do_alias_t *alias, const char *code);
static int get_doc(do_alias_t *alias, document_rec_t *doc, int reverse, int sklad1)
{
    do_ret_list_t *order;
    ret_t *r;
    char *code;
    double quant, price_purchase, price_sale;
    int j, indx;
    item_t item,item2, *it;
    struct tm tm;
    do_date(doc->data.date, &tm);

    item.year = tm.tm_year + 1900;
    item.month = tm.tm_mon + 1;
    item.unit = sklad1;
    item.sum_purchase = 0;
    item.sum_sale = 0;
    item2.year = tm.tm_year + 1900;
    item2.month = tm.tm_mon + 1;
    item2.unit = sklad1;
    item2.sum_purchase = 0;
    item2.sum_sale = 0;
    item2.class_ = 0;

    order = get_order(alias, doc);
    if ( !order )
        return FALSE;

    int i;
    int k;
    k = do_ret_list_count(order);
    double sum_purchase,sum_sale;
    for ( i = 0; i < k; i++ ) {
        r = do_ret_list_item_data(order, i);
        code = do_text(alias, r->code);
        item.class_ = get_class(alias, code);
        if ( !item.class_ )
            return FALSE;
        price_purchase = 0;
        price_sale = 0;
        for ( j = 0; j < 8; j++ ) {
            if ( r->nprice[j] == 9 )
                price_sale = r->price[j];
            if ( r->nprice[j] == 11 )
                price_purchase = r->price[j];
        }
        quant = 0;
        for ( j = 0; j < 4; j++ )
            if ( r->nquant[j] == 1 ) {
                quant = do_pdecimal(r->quant[j], 7, 3);
                break;
            }
        if ( reverse ) {
            sum_sale = -round(quant*price_sale*100);
            sum_purchase = -round(quant*price_purchase*100);
        }
        else {
            sum_sale = round(quant*price_sale*100);
            sum_purchase = round(quant*price_purchase*100);
        }

        if ( !do_sort_list_find(report, &indx, &item) )
            do_sort_list_insert_new(report, indx, &item, sizeof(item));
        it = report->list[indx];
        it->sum_sale += sum_sale;
        it->sum_purchase += sum_purchase;
        /*if ( !do_sort_list_find(report, &indx, &item2) )
            do_sort_list_insert_new(report, indx, &item2, sizeof(item));
        it = report->list[indx];
        it->sum_sale += sum_sale;
        it->sum_purchase += sum_purchase;*/
    }
    /*it->sum_sale = 0;
    it->sum_purchase = 0;*/
    do_ret_list_free(order);
    return TRUE;
}
static do_ret_list_t *get_order(do_alias_t *alias, document_rec_t *doc)
{
    document_order_key0_t document_order_key0;
    do_extended_t        *extended;
    do_ret_list_t        *retval;

    do_cpy(document_order_key0.dtype, doc->data.dtype);
    do_cpy(document_order_key0.sklad, doc->data.sklad);
    do_cpy(document_order_key0.document, doc->data.document);
    document_order_key0.line = 0;

    extended = do_extended_new(alias);
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

    do_extended_set_fields(extended, 6,
                do_size_and_offset(document_order_struct_t, code),
                do_size_and_offset(document_order_struct_t, quant),
                do_size_and_offset(document_order_struct_t, nquant),
                do_size_and_offset(document_order_struct_t, price),
                do_size_and_offset(document_order_struct_t, nprice),
                do_size_and_offset(document_order_struct_t, accepted));

    retval = do_extended_get(extended, -1);
    do_extended_free(extended);

    return retval;
}
typedef struct {
    char *code;
    int class_;
} cache_t;
do_sort_list_t *cache = NULL;
inline static int cmp_func(cache_t *s1, cache_t *s2)
{
    return strncmp(s1->code, s2->code, 5);
}
static int med(const char *g, const char *s)
{
    int i;
static char *gr[24] =
{
"47","66","43","51","50","53","65","64","49","67","45","55",
"46","54","60","71","62","57","61","58","63","70","48","68"
};
    for ( i = 0; i < 24; i++ )
        if ( !strcmp(g,gr[i]) ) return TRUE;
    if ( !strcmp(g,"41") && !strcmp(s, "01") )
        return TRUE;
    return FALSE;
}
static int get_class(do_alias_t *alias, const char *code)
{
    if ( !cache )
        cache = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)cmp_func, NULL);
    cache_t c;
    int indx;
    c.code = (char*)code;
    if ( do_sort_list_find(cache, &indx, &c) )
        return ((cache_t*)cache->list[indx])->class_;

    product_key0_t product_key0;
    product_rec_t product;

    c.code = do_strdup(code);
    c.code[5] = '\0';

    product_key0.base_parcel = 0;
    do_text_set(alias, product_key0.code, c.code);

    if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
        do_log(LOG_ERR, "Товар не найден \"%s\"", c.code);
        return 0;
    }
    switch ( do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS))) {
        case 1:
            c.class_ = 1;
            break;
        case 3:
            c.class_ = 2;//2;
            break;
        case 4:
            c.class_ = 2;//3;
            break;
        default:
            c.class_ = (med(do_text(alias, product.data.group_id), do_text(alias, product.data.subgroup_id))) ? 1 : 2;//4;
    }
    do_sort_list_insert_new(cache, indx, &c , sizeof(c));

    return c.class_;
}
