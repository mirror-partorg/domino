#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolib.h>
#include <domino.h>
#include <errno.h>
#include <locale.h>

#include "pfizer-const.h"

#define HEADER(a) "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\
<DAILYTRANSACTIONS>\n\
<VERSION>0.1</VERSION>\n\
<PHARMACYCHAINCODE>"a"</PHARMACYCHAINCODE>\n"

#define HEADER1(a) "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\
<MONTHLYTRANSACTIONS>\n\
<VERSION>0.1</VERSION>\n\
<PHARMACYCHAINCODE>"a"</PHARMACYCHAINCODE>\n"


#define FOOTER "</DAILYTRANSACTIONS>\n"
#define FOOTER1 "</MONTHLYTRANSACTIONS>\n"

static int read_sk(const char *alias_name, struct tm *date1, struct tm *date2, do_string_list_t *text);
static char *get_card_no(int number);

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();
    char *filename;

    if ( argc < 5 ) {
        fprintf(stderr, "usage %s <date_start> <date_end> <alias> [<alias>..] outfile \n", argv[0]);
        exit(1);
    }
    filename = argv[argc-1];
    if ( !domino_init(NULL, FALSE, DO_PARAM_END) )
        exit(1);
    struct tm date1, date2;
    if ( !do_atodate(argv[1], &date1) || !do_atodate(argv[2], &date2) ) {
        fprintf(stderr, "usage %s <date_start> <date_end> <alias> [<alias>..] outfile \n", argv[0]);
        exit(1);
    }

    int i;
    int long_report;

    do_string_list_t *text;

    long_report = mktime(&date2) > mktime(&date1);
    text = do_string_list_new();
    if ( long_report )
        do_string_list_add_alloc(text, HEADER1(ORG_CODE));
    else
        do_string_list_add_alloc(text, HEADER(ORG_CODE));

    for ( i = 3; i < argc -1; i++ ) {
        if ( !read_sk(argv[i], &date1, &date2, text) )
            exit(1);
    }
    if ( long_report )
        do_string_list_add_alloc(text, FOOTER1);
    else
        do_string_list_add_alloc(text, FOOTER);
    char *s;
    s= do_string_list_to_string(text);
    do_save_to_file(filename, s, strlen(s));
    return 0;
}
static char *get_barcode(do_alias_t *alias, sklad_key0_t *sklad_key0, int cardno, product_rec_t *product, const char *in_barcode);
static int read_sk_sklad(do_alias_t *alias, sklad_key0_t *sklad_key0, struct tm *date1, struct tm *date2, const char *inn, do_string_list_t *text, int nds);
static int read_sk(const char *alias_name, struct tm *date1, struct tm *date2, do_string_list_t *text)
{
    do_alias_t *alias;
    alias = domino_alias_new(alias_name);
    if ( !alias )
        return FALSE;
    if ( !do_alias_open(alias, TRUE) ) {
        do_alias_free(alias);
        return FALSE;
    }
    otdel_key0_t otdel_key0;
    otdel_rec_t otdel;

    do_text_set(alias, otdel_key0.code, domino_division());
    if ( do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL) != DO_OK ) {
        do_alias_free(alias);
        return FALSE;
    }
    int nds;
    char *val, *ch, *ss;
    val = do_otdel_param(alias, &otdel, "07");
    ch = strchr (val, '.');
    if ( !ch ) {
        do_alias_free(alias);
        do_log(LOG_ERR, "Can't find firm");
        return FALSE;
    }
    ss = do_otdel_param(alias, &otdel, "06");
    nds = !strcmp(ss, "НДС");

    partner_key0_t partner_key0;
    partner_rec_t partner;
    *ch = '\0';
    ch++;
    do_text_set(alias, partner_key0.g_code, val);
    partner_key0.code = atoi(ch);
    if ( !do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        do_alias_free(alias);
        do_log(LOG_ERR, "Can't find firm");
        return FALSE;
    }

    val = do_partner_param(alias, &partner, "ИНН1");

    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;
    int status;
    do_text_set_empty(sklad_key0.code);
    status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        ch = do_text(alias, sklad.data.type_);
        if ( !strcmp(ch, "ТО") ) { //!!&& (atoi(sklad.data.code) == 12) ) { //!!
            do_log(LOG_INFO, "read for %s", do_text(alias, sklad.data.code));
            if ( !read_sk_sklad(alias, &sklad_key0, date1, date2, val, text, nds) ) {
                do_alias_free(alias);
                do_log(LOG_ERR, "read sklad fail");
                return FALSE;
            }
        }
        status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_NEXT);
    }
    do_alias_free(alias);
    return TRUE;

}
static int check_ctl(do_alias_t *alias, int cash, int shift, int check, product_rec_t *product, double quant, double price);
static int get_dist_inn(do_alias_t *alias, sklad_key0_t *sklad_key0, struct tm *time, int cash, int shift, product_rec_t *product, double price, char **inn, int *nds);
static int read_sk_sklad(do_alias_t *alias, sklad_key0_t *sklad_key0, struct tm *date1, struct tm *date2, const char *inn, do_string_list_t *text, int nds)
{
    document_key3_t document_key3, key;
    document_rec_t document;
    int status, i;
    int long_report=FALSE;


    struct {
        int cash;
        int cash_number;
        int shift;
        int check;
        
        struct tm time;
        do_list_t *lines;
        char *doc_num;
        int cardno;
        char *in_barcode;
    } *check_info;

    struct {
        double price4;
        double price9;
        double quant;
        product_rec_t product;
        char *barcode;
        char *dist_inn;
        char *in_barcode;
        int nds;
    } *line;
    do_list_t *list;
    list = do_list_new(TRUE);

    long_report = mktime(date2) > mktime(date1);


    do_text_set(alias, document_key3.dtype, "DK");
    do_cpy(document_key3.sklad, sklad_key0->code);
    do_date_set(&document_key3.date, *date1);
    document_key3.time = 0;

    do_cpy(key.dtype, document_key3.dtype);
    do_cpy(key.sklad, document_key3.sklad);
    do_date_set(&key.date, *date2);

    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.dtype, document_key3.dtype) ||
             do_cmp(key.sklad, document_key3.sklad) ||
             document_key3.date > key.date ) break;

        long long card;
        card = atoll(do_document_param(alias, &document, "CARD"));
        if ( (card > PFIZER_SKB) && (card < PFIZER_SKB_MAX) ) {
            char *val, *ch;
            val = do_text(alias, document.data.p_doc);
            ch = val;
            ch[4] = '\0';
            check_info = do_malloc0(sizeof(*check_info));
            check_info->cardno = card;
            check_info->cash = atoi(ch);
            check_info->cash_number = document.data.p_code;
            ch[9] = '\0';
            check_info->shift = atoi(ch+5);
            check_info->check = atoi(ch+10);
            do_date(document.data.date, &check_info->time);
            do_time(document.data.time, &check_info->time);
            check_info->lines = do_list_new(TRUE);
            check_info->doc_num = do_text(alias, document.data.p_doc);

            document_order_key0_t document_order_key0;
            document_order_rec_t document_order;
            do_cpy(document_order_key0.dtype, document.data.dtype);
            do_cpy(document_order_key0.sklad, document.data.sklad);
            do_cpy(document_order_key0.document, document.data.document);
            document_order_key0.line = 0;
            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
            while ( status == DO_OK ) {
                if ( do_cmp(document_order_key0.dtype, document.data.dtype) ||
                     do_cmp(document_order_key0.sklad, document.data.sklad) ||
                     do_cmp(document_order_key0.document, document.data.document)
                    ) break;
                if ( do_document_order_price(alias, &document_order, 9) <
                     do_document_order_price(alias, &document_order, 4) ) {
                    char *bar;
                    product_key0_t product_key0;
                    
                    line = do_malloc0(sizeof(*line));
                    line->price4 = do_document_order_price(alias, &document_order, 4);
                    line->price9 = do_document_order_price(alias, &document_order, 9);
                    line->quant = do_document_order_quant(alias, &document_order, 1);
                    bar = do_document_order_param(alias, &document_order, "BAR");
                    if ( (!bar[0] || bar[0] == '2') && document_order.data.code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)] == '.' ) {
                        do_text_set(alias, product_key0.code, document_order.data.code);
                        product_key0.base_parcel = 1;
                        if ( do_product_get0(alias, &line->product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
                            do_log(LOG_ERR,"error read product \"%s.%d\"",do_text(alias, product_key0.code),product_key0.base_parcel);
                            return FALSE;
                        }
                        do_free(bar);
                        bar = do_product_param(alias, &line->product, "BARCODE");
                    }
                    line->in_barcode = bar;
                    document_order.data.code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)] = '\0';
                    do_text_set(alias, product_key0.code, document_order.data.code);
                    //product_key0.base_parcel = 1;
                    product_key0.base_parcel = 0;
                    if ( do_product_get0(alias, &line->product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
                        do_log(LOG_ERR,"error read1 product \"%s.%d\" from document \"%s.%s.%s.%d\"",do_text(alias, product_key0.code),product_key0.base_parcel,
                        do_text(alias,document_order.data.dtype),do_text(alias,document_order.data.sklad),do_text(alias,document_order.data.document),document_order.data.line);
                        return FALSE;
                    }
                    do_list_add_alloc(check_info->lines, line, sizeof(*line));

                }
                status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
            }
            if ( status == DO_ERROR ) {
                return FALSE;
            }
            if ( check_info->lines->count > 0 )
                do_list_add_alloc(list, check_info, sizeof(*check_info));
        }
        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;

    int count,j,indx;

    for ( j = 0; j < list->count; j++ ) {
        check_info = list->list[j];
        count = 0;
        for ( i = 0; i < check_info->lines->count; i++ ) {
            line = check_info->lines->list[i];
            line->barcode = get_barcode(alias, sklad_key0, check_info->cardno, &line->product, line->in_barcode);
            if ( !line->barcode ) {
                do_log(LOG_ERR, "product %s not found in sk list. Check %s/%s", do_text(alias, line->product.data.code), check_info->doc_num, do_text(alias, sklad_key0->code));
                return FALSE;
            }
            if ( !check_ctl(alias, check_info->cash_number, check_info->shift, check_info->check, &line->product, line->quant, line->price9) ) {
                do_log(LOG_ERR, "product %s not found in check. Check: %s store: %s %s", do_text(alias, line->product.data.code), check_info->doc_num, do_text(alias, sklad_key0->code), do_datetoa(check_info->time));
                return FALSE;
            }
            if ( long_report && !get_dist_inn(alias, sklad_key0, &check_info->time, check_info->cash, check_info->shift, &line->product, line->price9, &line->dist_inn, &line->nds) ) {
                do_log(LOG_ERR, "product %s not found in realization document. Check %s/%s", do_text(alias, line->product.data.code), check_info->doc_num, do_text(alias, sklad_key0->code));
                return FALSE;
            }
            count++;
        }
        if ( count ) {
            do_string_list_add_alloc(text, "\t<DOCUMENT>\n");
            do_string_list_addf_alloc(text, "\t\t<DOCNUMBER>%s</DOCNUMBER>\n", check_info->doc_num);
            do_string_list_addf_alloc(text, "\t\t<COUNTLINES>%d</COUNTLINES>\n", count);
            do_string_list_add_alloc(text, "\t\t<LINES>\n");
            char *sad;
            sad = do_strdup(asctime(&check_info->time));

            do_log(LOG_INFO, "document %d/%d/%d %s card %d %s", check_info->cash, check_info->shift, check_info->check, sad, check_info->cardno, get_card_no((check_info->cardno)));
            indx = 0;
            for ( i = 0; i < check_info->lines->count; i++ ) {
                line = check_info->lines->list[i];
                line->barcode = get_barcode(alias, sklad_key0, check_info->cardno, &line->product, line->in_barcode);
                if ( line->barcode ) {
                    do_string_list_add_alloc(text, "\t\t<DOCLINE>\n");
                    do_string_list_addf_alloc(text, "\t\t\t<LINE>%d</LINE>\n", ++indx);
                    do_string_list_addf_alloc(text, "\t\t\t<PHARMACYINN>%s</PHARMACYINN>\n", inn);
                    do_string_list_addf_alloc(text, "\t\t\t<PHARMACYCODE>%s</PHARMACYCODE>\n", do_text(alias, sklad_key0->code));
                    if ( long_report )
                        do_string_list_addf_alloc(text, "\t\t\t<DISTRIBUTORINN>%s</DISTRIBUTORINN>\n", line->dist_inn);

                    do_string_list_addf_alloc(text, "\t\t\t<CARDNUMBER>%s</CARDNUMBER>\n", get_card_no(check_info->cardno));
                    do_string_list_addf_alloc(text, "\t\t\t<OPERDATE>%2.2d%2.2d%4.4d</OPERDATE>\n", check_info->time.tm_mday,
                                                                                      check_info->time.tm_mon+1,check_info->time.tm_year+1900);
                    if ( !long_report ) {
                        do_string_list_addf_alloc(text, "\t\t\t<CASHREGISTERNUMBER>%d</CASHREGISTERNUMBER>\n", check_info->cash);
                        do_string_list_addf_alloc(text, "\t\t\t<SHIFTNUMBER>%d</SHIFTNUMBER>\n", check_info->shift);
                    }
                    do_string_list_addf_alloc(text, "\t\t\t<PRODUCTCODE>%s</PRODUCTCODE>\n", line->barcode);
                    do_string_list_addf_alloc(text, "\t\t\t<QUANTITY>%.0f</QUANTITY>\n", line->quant/do_product_coef(alias, &line->product));
                    do_string_list_addf_alloc(text, "\t\t\t<AMOUNTTOTAL>%s</AMOUNTTOTAL>\n", do_ftoasum(line->quant*line->price9));
                    do_string_list_addf_alloc(text, "\t\t\t<DISCOUNT>%s</DISCOUNT>\n", do_ftoasum(line->quant*(line->price4-line->price9)));
                    do_string_list_addf_alloc(text, "\t\t\t<PRICE>%s</PRICE>\n", do_ftoasum(line->price4*do_product_coef(alias, &line->product)));
                    if ( long_report ) {
                        double price_nds, discount_nds;
                        if ( !nds ) line->nds = 0;
                        price_nds = line->price9*line->nds/100.;
                        discount_nds = (line->quant*(line->price4-line->price9))*line->nds/100.;
                        do_string_list_addf_alloc(text, "\t\t\t<PRICEMINUSNDS>%s</PRICEMINUSNDS>\n", do_ftoasum((line->price9-price_nds)*do_product_coef(alias, &line->product)));
                        do_string_list_addf_alloc(text, "\t\t\t<AMOUNTNDS>%s</AMOUNTNDS>\n", do_ftoasum(line->quant*price_nds));
                        do_string_list_addf_alloc(text, "\t\t\t<DISCOUNTNDS>%s</DISCOUNTNDS>\n", do_ftoasum(discount_nds));
                        do_string_list_addf_alloc(text, "\t\t\t<PERCENTNDS>%d</PERCENTNDS>\n", line->nds);
                        do_string_list_addf_alloc(text, "\t\t\t<PROGRAMFLAG>Y</PROGRAMFLAG>\n");
                    }
                    do_string_list_add_alloc(text, "\t\t</DOCLINE>\n");
                }
            }


            do_string_list_add_alloc(text, "\t\t</LINES>\n");
            do_string_list_add_alloc(text, "\t</DOCUMENT>\n");

        }
    }
    return TRUE;
}
static char *get_barcode(do_alias_t *alias, sklad_key0_t *sklad_key0, int cardno, product_rec_t *product, const char *in_barcode)
{
    document_key3_t document_key3, key;
    document_rec_t document;
    int status;

    do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_DISCOUNT));
    do_text_set(alias, document_key3.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    document_key3.date = PFIZER_SKB;
    document_key3.time = PFIZER_SKB;
    do_cpy(key.dtype, document_key3.dtype);
    do_cpy(key.sklad, document_key3.sklad);

    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
    char *res = NULL;
    while ( status == DO_OK ) {
        if ( do_cmp(document_key3.dtype, key.dtype) ||
             do_cmp(document_key3.sklad, key.sklad) ||
            document_key3.date > cardno ) break;
        if ( document_key3.date <= cardno && document_key3.time >= cardno ) {
            document_order_key2_t document_order_key2;
            document_order_rec_t document_order;
            do_cpy(document_order_key2.dtype, document.data.dtype);
            do_cpy(document_order_key2.sklad, document.data.sklad);
            do_cpy(document_order_key2.document, document.data.document);
            do_cpy(document_order_key2.code, product->data.code);
            status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL);
            while ( status  == DO_OK ) {
                if ( do_cmp(document_order_key2.dtype, document.data.dtype) ||
                     do_cmp(document_order_key2.sklad, document.data.sklad) ||
                     do_cmp(document_order_key2.document, document.data.document) ||
                     do_cmp(document_order_key2.code, product->data.code) ) break;
                if ( res ) do_free(res);
                res = do_document_order_param(alias, &document_order, "BAR");
                //fprintf(stderr, "barcode %s %s %s %s %s %d %s\n", do_text(alias,product->data.code),do_document_order_param(alias, &document_order, "BAR"),do_text(alias,document_order.data.document),do_text(alias,document_order.data.dtype),do_text(alias,document.data.sklad),document_order.data.line, in_barcode);
                if ( !in_barcode[0] ||  !strcmp(in_barcode,res) ) {
                    //fprintf(stderr, "ret barcode %s\n", res);
                    return res;
                }
                status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_NEXT);
            }
        }
        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }
    //fprintf(stderr, "ret barcode1 %s\n", res);
    return res;
}
char *get_card_no(int number)
{
    int64_t nr;
    nr =  number + CARD_MASK;
    char *result = do_malloc0(100);

    sprintf(result, "%ld", nr);
    int p = 0;
    int sum = 0;
    int i;
    int N = 19;
    for ( i = 1; i < N; i++ ) {
        p = result[N-1-i] - '0';
        if ( i%2 != 0 ) {
            p*=2;
            if ( p > 9 )
                p = p - 9;
        }
        sum +=p;
    }
    sum =( (sum%10)==0 ) ? 0: ( 10- (sum%10) );
    result[18]='0'+sum;
    return result;
}
static int check_ctl(do_alias_t *alias, int cash, int shift, int check_, product_rec_t *product, double quant, double price)
{
    check_rec_t check;
    check_key0_t check_key0, key;
    int status;

    do_set_empty(check_key0);
    check_key0.cash = cash;
    check_key0.shift = shift;
    check_key0.check = check_;
    do_cpy(key, check_key0);

    status = do_check_get0(alias, &check, &check_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( (check_key0.cash != cash) ||
             (check_key0.shift != shift) ||
             (check_key0.check != check_) ) break;
        do_log(LOG_INFO, "Check %d.%d.%d %s %s check quant %.3f sk %.3f, price %.2f %.2f",cash,shift,check_,do_text(alias,check.data.product_code), do_text(alias,product->data.code), check.data.quantity,quant,check.data.sale_price,price);
        if ( !strncmp(check.data.product_code, product->data.code, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)) ) {
            if ( ( check.data.quantity >= quant -0.01) && ( check.data.quantity <= quant +0.01)

                )
                if ( (( check.data.sale_price >= price -0.001) && ( check.data.sale_price <= price +0.001)) ) 
                    return TRUE;
        }


        status = do_check_get0(alias, &check, &check_key0, DO_GET_NEXT);
    }
    do_set_empty(check_key0);
    check_key0.cash = cash;
    check_key0.shift = shift+10000;
    check_key0.check = check_;
    do_cpy(key, check_key0);

    status = do_check_get0(alias, &check, &check_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( (check_key0.cash != cash) ||
             (check_key0.shift != shift+10000) ||
             (check_key0.check != check_) ) break;
        do_log(LOG_INFO, "Check %d.%d.%d %s %s check quant %.3f sk %.3f, price %.2f %.2f",cash,shift,check_,do_text(alias,check.data.product_code), do_text(alias,product->data.code), check.data.quantity,quant,check.data.sale_price,price);
        if ( !strncmp(check.data.product_code, product->data.code, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)) ) {
            if ( ( check.data.quantity >= quant -0.01) && ( check.data.quantity <= quant +0.01)

                )
                if ( (( check.data.sale_price >= price -0.001) && ( check.data.sale_price <= price +0.001)) ) 
                    return TRUE;
        }


        status = do_check_get0(alias, &check, &check_key0, DO_GET_NEXT);
    }
    return FALSE;
}
static int get_dist_inn(do_alias_t *alias, sklad_key0_t *sklad_key0, struct tm *time, int cash, int shift, product_rec_t *product, double price, char **inn, int *nds)
{
    document_key3_t document_key3, key;
    document_rec_t document;
    int status;
    char p_doc[20];

    do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    do_cpy(document_key3.sklad, sklad_key0->code);
    do_date_set(&document_key3.date, *time);
    document_key3.date -=1;
    document_key3.time = 0;
    do_cpy(key.dtype, document_key3.dtype);
    do_cpy(key.sklad, document_key3.sklad);
    key.date = document_key3.date + 10;

    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
    sprintf(p_doc, "%2.2d/%6.6d", cash, shift);

    while ( status == DO_OK ) {
        if ( do_cmp(key.dtype, document_key3.dtype) ||
             do_cmp(key.sklad, document_key3.sklad) ||
             document_key3.date > key.date
            ) break;
        if ( ((document.data.dtype1[0] == '0') && (document.data.dtype1[1] == '1' || document.data.dtype1[1] == '3') && (document.data.dtype1[2] == ' ')) &&
             /*(document.data.p_code == cash) &&*/
              !strcmp(p_doc, do_text(alias, document.data.p_doc))
            ) {
            document_order_key2_t document_order_key2;
            document_order_rec_t document_order;

            do_cpy(document_order_key2.dtype, document.data.dtype);
            do_cpy(document_order_key2.sklad, document.data.sklad);
            do_cpy(document_order_key2.document, document.data.document);
            do_cpy(document_order_key2.code, product->data.code);

            status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_GE);
            while ( status == DO_OK ) {
                if ( do_cmp(document_order_key2.dtype, document.data.dtype) ||
                     do_cmp(document_order_key2.sklad, document.data.sklad) ||
                     do_cmp(document_order_key2.document, document.data.document) ||
                    (
                      (do_product_is_parcel(product->data.code, sizeof(product->data.code)) &&
                       do_cmp(document_order_key2.code, product->data.code)) ||
                      (!do_product_is_parcel(product->data.code, sizeof(product->data.code)) &&
                       strncmp(document_order_key2.code, product->data.code, do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)))
                      ) ) break;
                double price_9;
                price_9 = do_document_order_price(alias, &document_order, 9);
                do_log(LOG_INFO,"%s.%s.%s.%d %s parcel %d, doc price: %.3f, check price: %.3f",do_text(alias, document_order.data.dtype),do_text(alias, document_order.data.sklad),do_text(alias, document_order.data.document),document_order.data.line,do_text(alias, document_order.data.code), do_product_is_parcel(document_order.data.code, sizeof(document_order.data.code)), price_9, price);
                if ( (price_9 >= price - 0.0001) &&  (price_9 <= price + 0.0001) &&
                    do_product_is_parcel(document_order.data.code, sizeof(document_order.data.code)) ) {
                    product_key0_t product_key0;
                    product_rec_t p;
                    product_key0.base_parcel = 1;
                    //do_cpy(product_key0.code, product->data.code);
                    do_cpy(product_key0.code, document_order.data.code);

                    if ( do_product_get0(alias, &p ,&product_key0, DO_GET_EQUAL) != DO_OK ) {
                        do_log(LOG_ERR, "product %s not found", do_text(alias, product_key0.code));
                        return FALSE;
                    }
                    if ( !strncmp(p.data.supplier_region, "03", 2) ) {
                        partner_key0_t partner_key0;
                        partner_rec_t partner;
                        do_cpy(partner_key0.g_code, p.data.supplier_region);
                        partner_key0.code = p.data.supplier_code;
                        if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) != DO_OK ) {
                            do_log(LOG_ERR, "partner %s.%d not found", do_text(alias, partner_key0.g_code), partner_key0.code);
                            return FALSE;
                        }
                        *inn = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_INN));
                        //*nds = do_product_param_int(alias, &p, "14");
                        *nds = do_document_order_param_int(alias, &document_order, "НДС");
                        if ( *inn[0] != '\0' )
                            return TRUE;
                        do_log(LOG_WARNING, "partner %s.%d is empty INN", do_text(alias, partner_key0.g_code), partner_key0.code);
                    }
                    else {
                       //do_log(LOG_WARNING, "product %s not correct partner", do_text(alias, p.data.code));
                       do_log(LOG_WARNING, "product %s not correct partner", do_text(alias, document_order.data.code));
                    }
                }
                else {
                     do_log(LOG_INFO,"skip");
                }
                status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_NEXT);
            }

            //return FALSE;

        }

        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }

    return FALSE;
}
