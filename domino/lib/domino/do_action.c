#include <domino.h>
#include <errno.h>
#include <string.h>
#include "../../misc/define.h"
#include "../../misc/csv_util.h"
#include "../../misc/iconv_.h"

#define DATE_DIF 30

#ifndef DOMINO78
DO_EXPORT int do_action_delete_document(do_alias_t *alias, document_key0_t *document_key0)
{
    return do_deaccept(alias, document_key0, DO_DEACCEPT_AND_DELETE, NULL);
}
DO_EXPORT int do_action_delete_document_order(do_alias_t *alias, document_order_key0_t *document_order_key0)
{
    if (!do_alias_tran_begin(alias))
        return 0;
    int res;
    document_order_rec_t document_order;
    res = do_document_order_get0(alias, &document_order, document_order_key0, DO_GET_EQUAL) == DO_OK;
    if (res)
        res = do_deaccept_order(alias, &document_order, DO_DEACCEPT_AND_DELETE, NULL);
    if (res)
        do_alias_tran_end(alias);
    else
        do_alias_tran_abort(alias);
    return res;
}
DO_EXPORT int do_action_insert_document_order(do_alias_t *alias, document_order_rec_t *document_order)
{
    if (!do_alias_tran_begin(alias))
        return 0;
    int res;
    res = do_document_order_insert(alias, document_order) == DO_OK;
    if (res)
        res = do_protocol_document_order_add_rec(alias, document_order, DO_PROTOCOL_ACTION_CHANGE, NULL);
    if (res)
        do_alias_tran_end(alias);
    else
        do_alias_tran_abort(alias);
    return res;
}

DO_EXPORT int do_action_insert_document(do_alias_t *alias, document_rec_t *document)
{
    if (!do_alias_tran_begin(alias))
        return 0;
    int res;
    res = do_document_insert(alias, document) == DO_OK;
    if (res)
        res = do_protocol_document_add_rec(alias, document, DO_PROTOCOL_ACTION_INSERT);
    if (res)
        do_alias_tran_end(alias);
    else
        do_alias_tran_abort(alias);
    return res;
}

typedef struct {
    int check;
    int pos;
    struct tm time;
    double price;
    double quantity;
    char *code;
    char *barcode;
    int cashier;
    int valuta;
    char *operation;
} check_pos_t;
typedef struct {
    int check;
    struct tm time;
    int operation;
    int valuta;
    double sum;
    char *info;
} money_t;
typedef struct {
    int cash;
    int shift;
    struct tm time_b;
    struct tm time;
    char *cashier;
    double sum;
    int total;
    char *sklad;
    do_list_t *check_pos;
    do_list_t *money;
} shift_t;
static void shift_free(shift_t *shift)
{
    do_free(shift->cashier);
    do_free(shift->sklad);
    if ( shift->check_pos ) {
        int i;
        for ( i = 0; i < shift->check_pos->count; i++ ) {
            check_pos_t *pos;
            pos = shift->check_pos->list[i];
            do_free(pos->code);
            do_free(pos->barcode);
            do_free(pos->operation);
        }
        do_list_free(shift->check_pos);
        for ( i = 0; i < shift->money->count; i++ ) {
            money_t *pos;
            pos = shift->money->list[i];
            do_free(pos->info);
        }
        do_list_free(shift->money);
    }
    do_free(shift);
}
static shift_t *shift_new()
{
    shift_t *retval;
    retval = do_malloc0(sizeof(shift_t));
    retval->check_pos = do_list_new(TRUE);
    retval->money = do_list_new(TRUE);
    return retval;
}
#endif
#define RUNNING_ (!break_func || !break_func(NULL))

#ifndef DOMINO78
static shift_t *do_action_read_cl(do_alias_t *alias, const char *filename, do_extended_break_func break_func)
{
    FILE* fp;
    if ((fp = fopen(filename, "r")) == NULL )
    {
        do_log(LOG_ERR,"open file %s - %s", filename, strerror(errno));
        return NULL;
    }
    shift_t *shift;
    check_pos_t check;
    money_t money;
    size_t len;
    char* line;
    char* convline;
    int header;
    int line_indx;
    int err;
    barcode_key0_t barcode_key0;
    barcode_rec_t barcode;
    int crnt_pos;
    int crnt_check;
    int t_line, m_line;
    double sum;
    int init_time_b;

    shift = shift_new();
    len = BUFSIZ;
    line = (char*) do_malloc(len + 1);
    convline = NULL;
    header = 1;
    line_indx = 0;
    err = FALSE;
    crnt_pos = 0;
    crnt_check = 0;
    init_time_b = FALSE;

    while ( !feof(fp) && !err ) {
        char* cp;
        if ( !RUNNING_ ) {
            err = TRUE;
            break;
        }

        if ( fgets(line, len + 1, fp) == NULL ) {
            if ( !feof(fp) ) {
                do_log(LOG_ERR,"read file %s - %s", filename, strerror(errno));
                err = TRUE;
            }
            break;
        }
        while ( (strlen(line) == len) && !feof( fp ) )
        {
            line = (char*) do_realloc(line, len * 2 + 1);
            if (!fgets(line + len, len + 1, fp)) {
                do_log(LOG_ERR,"read file %s - %s", filename, strerror(errno));
                err = TRUE;
            }
            len *= 2;
        }
        if ( err )
            break;

        line_indx++;
        if ( convline ) do_free(convline);

        convline = do_alias_text_(alias, line, strlen(line));
        cp = convline;
        CSV_DELSPACE(cp);
        // A leading '#' or ';' signals a comment line.
        if ( cp[0] == '#' || cp[0] == ';' || cp[0] == '\0' || cp[0] == '\n' )
            continue;
        t_line = 0;
        m_line = 0;
        if ( header ) // header part
        {
            if ( !shift->time.tm_mday && csv_isname(&cp, do_param(DO_PARAM_CL_PARAMETR_DATE)) )
            {
                do_atodate(cp, &shift->time);
                continue;
            }
            if ( !shift->time_b.tm_mday && csv_isname(&cp, do_param(DO_PARAM_CL_PARAMETR_DATE_B)) )
            {
                do_atodate(cp, &shift->time_b);
                continue;
            }
            if ( !shift->time.tm_hour && csv_isname(&cp, do_param(DO_PARAM_CL_PARAMETR_TIME)) )
            {
                do_atotime(cp, &shift->time);
                continue;
            }
            if ( !shift->cash && csv_isname(&cp, do_param(DO_PARAM_CL_PARAMETR_CASH)) )
            {
                shift->cash = atoi(cp);
                continue;
            }
            if ( !shift->shift && csv_isname(&cp, do_param(DO_PARAM_CL_PARAMETR_SHIFT)) )
            {
                char *ch;
                ch = strchr(cp, '/');
                if ( !ch ) {
                    do_log(LOG_ERR, "Error shift format %d:\"%s\"", line_indx, convline);
                    err = TRUE;
                    break;
                }
                shift->shift = atoi(ch + 1);
                if ( !shift->shift ) {
                    do_log(LOG_ERR, "Error shift format %d:\"%s\"", line_indx, convline);
                    err = TRUE;
                    break;
                }
                continue;
            }
            if ( !shift->cashier && csv_isname(&cp, do_param(DO_PARAM_CL_PARAMETR_CASHIER)) )
            {
                shift->cashier = do_strdup(cp);
                continue;
            }
            if ( csv_isname(&cp, "T") )
            {
                t_line = 1;
                header = 0;
            }
            else
            if ( csv_isname(&cp, "M") )
            {
                m_line = 1;
                header = 0;
            }
            else
                continue;
        }
        else
        {
            if ( csv_isname(&cp, "T") )
            {
                t_line = 1;
                header = 0;
            }
            else
            if ( csv_isname(&cp, "M") )
            {
                m_line = 1;
                header = 0;
            }
            else
                continue;
        }

        if ( m_line ) {
            // line

            money.check = atoi(csv_getelem(&cp)); //1
            csv_getelem(&cp); //2
            csv_getelem(&cp); //3
            csv_getelem(&cp); //4
            csv_getelem(&cp); //5
            do_atodate(csv_getelem(&cp), &money.time); //6
            do_atotime(csv_getelem(&cp), &money.time); //7
            money.operation = atoi(csv_getelem(&cp)); //8
            money.valuta = atoi(csv_getelem(&cp)); //9
            csv_getelem(&cp); //10
            csv_getelem(&cp); //11
            csv_getelem(&cp); //12
            csv_getelem(&cp); //13
            money.sum = do_atof(csv_getelem(&cp)); // 14
            csv_getelem(&cp); //15
            csv_getelem(&cp); //16
            money.info = do_strdup(csv_getelem(&cp)); //27
            do_list_add_alloc(shift->money, &money, sizeof(money));
        }
        else
        if ( t_line ) {
            // money

            check.check = atoi(csv_getelem(&cp)); //1
            if ( check.check != crnt_check ) {
                crnt_check = check.check;
                crnt_pos = 0;
                shift->total++;
            }
            crnt_pos++;

            do_atodate(csv_getelem(&cp), &check.time); //2
            do_atotime(csv_getelem(&cp), &check.time); //3
            if ( !init_time_b ) {
                shift->time_b.tm_hour = check.time.tm_hour;
                shift->time_b.tm_min = check.time.tm_min;
                shift->time_b.tm_sec = check.time.tm_sec;
                init_time_b = TRUE;
            }

            check.operation = do_strdup(csv_getelem(&cp)); //4
            csv_getelem(&cp); //5
            csv_getelem(&cp); //6
            check.barcode = do_strdup(csv_getelem(&cp)); //7
            csv_getelem(&cp); //8
            csv_getelem(&cp); //9
            csv_getelem(&cp); //10
            check.price = do_atof(csv_getelem(&cp)); // 11
            csv_getelem(&cp); //12
            csv_getelem(&cp); //13
            csv_getelem(&cp); //14
            check.quantity = do_atof(csv_getelem(&cp)); // 15
            check.cashier = atoi(csv_getelem(&cp)); //16
            check.valuta = atoi(csv_getelem(&cp)); //17
            csv_getelem(&cp); //18
            csv_getelem(&cp); //19
            check.pos = atoi(csv_getelem(&cp)); //20
            if ( !check.pos )
                check.pos = crnt_pos;
            sum = check.price*check.quantity;
            sum = do_real_to_money(sum);
            shift->sum += sum;

            do_text_set(alias, barcode_key0.barcode, check.barcode);
            switch ( do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL) ) {
                case DO_OK:
                    check.code = do_text(alias, barcode.data.code);
                    break;
                case DO_KEY_NOT_FOUND:
                    do_log(LOG_ERR, "Product \"%s\" not found", check.barcode);
                    check.code = do_strdup("");
                    err = TRUE;
                    break;
                default:
                    check.code = do_strdup("");
                    err = TRUE;
                    break;
            }
            do_list_add_alloc(shift->check_pos, &check, sizeof(check));
        }

    }
    if (convline)
        do_free(convline);
    do_free(line);
    fclose(fp);

    partner_rec_t partner;
    partner_key0_t partner_key0;

    partner_key0.code = shift->cash;
    do_text_set(alias, partner_key0.g_code, do_param(DO_PARAM_PARTNER_REGION_CASH));

    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            shift->sklad = do_text(alias, partner.data.sklad);
            if ( *shift->sklad == '\0' ) {
                do_log(LOG_ERR, "undefine partner unit \"%s.%d\"", do_param(DO_PARAM_PARTNER_REGION_CASH), shift->cash);
                err = TRUE;
            }
            break;
        case DO_KEY_NOT_FOUND:
            do_log(LOG_ERR, "cash partner \"%s.%d\" not found", do_param(DO_PARAM_PARTNER_REGION_CASH), shift->cash);
            err = TRUE;
            break;
        default:
            err = TRUE;
    }

    if ( err ) {
        shift_free(shift);
        return NULL;
    }

    shift->sum /= 100;
    return shift;
};
static int do_action_shift_delete(do_alias_t *alias, shift_t *shift, do_extended_break_func break_func)
{
    int status;
    shift_key0_t shift_key0;
    shift_rec_t shift_;

    do_set_empty(shift_key0);
    shift_key0.cash = shift->cash;
    shift_key0.shift = shift->shift;

    status = do_shift_get0(alias, &shift_, &shift_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( !RUNNING_ )
            return FALSE;

        if ( shift_.data.cash != shift->cash ||
             shift_.data.shift != shift->shift ) break;

        if ( do_shift_delete(alias) != DO_OK )
            return FALSE;

        status = do_shift_get0(alias, &shift_, &shift_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;

    checksum_key0_t checksum_key0;
    checksum_rec_t checksum;
    do_set_empty(checksum_key0);
    checksum_key0.cash = shift->cash;
    checksum_key0.shift = shift->shift;
    status = do_checksum_get0(alias, &checksum, &checksum_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( !RUNNING_ )
            return FALSE;
        if ( checksum.data.cash != shift->cash ||
             checksum.data.shift != shift->shift ) break;

        if ( do_checksum_delete(alias) != DO_OK )
            return FALSE;
        status = do_checksum_get0(alias, &checksum, &checksum_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;

    check_key0_t check_key0;
    check_rec_t check;
    do_set_empty(check_key0);
    check_key0.cash = shift->cash;
    check_key0.shift = shift->shift;
    status = do_check_get0(alias, &check, &check_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( !RUNNING_ )
            return FALSE;
        if ( check.data.cash != shift->cash ||
             check.data.shift != shift->shift ) break;

        if ( do_check_delete(alias) != DO_OK )
            return FALSE;
        status = do_check_get0(alias, &check, &check_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;


    return TRUE;
}
static int do_action_shift_create(do_alias_t *alias, shift_t *shift, do_extended_break_func break_func)
{
    shift_rec_t shift_;
    checksum_rec_t checksum;
    check_rec_t check;
    int64_t real;
    int i, crnt_check;
    int64_t crnt_sum;

    do_set_empty(shift_);
	shift_.data.cash = shift->cash;
	shift_.data.shift = shift->shift;
	shift_.data.date = do_datetosql(shift->time);
	shift_.data.time = do_timetosql(shift->time);
	shift_.data.totalCheck = shift->total;
	real = shift->sum*10000;
	do_cpy(shift_.data.sale_sum, real);
    shift_.data.date_b = do_datetosql(shift->time_b);
    shift_.data.time_b = do_timetosql(shift->time_b);
    do_text_set(alias, shift_.data.sklad, shift->sklad);

	shift_.size = sizeof(shift_struct_t);

    if ( do_shift_insert(alias, &shift_) != DO_OK)
        return FALSE;

    crnt_check = 0;
    crnt_sum = 0;

    for ( i = 0; i < shift->check_pos->count; i++ ) {
        if ( !RUNNING_ )
            return FALSE;

        check_pos_t *pos;
        pos = shift->check_pos->list[i];
        if ( crnt_check != pos->check ) {
            if ( crnt_check ) {
                checksum.data.sale_sum = crnt_sum/100.;
                if ( do_checksum_insert(alias, &checksum) != DO_OK )
                    return FALSE;

            }
            do_set_empty(checksum);
            checksum.size = sizeof(checksum_struct_t);
            checksum.data.cash = shift->cash;
            checksum.data.shift = shift->shift;
            checksum.data.check = pos->check;
            checksum.data.date = do_datetosql(pos->time);
            checksum.data.time = do_timetosql(pos->time);
            checksum.data.cashier = pos->cashier;
            checksum.data.valuta = pos->valuta;
            do_text_set(alias, checksum.data.operation, pos->operation);
            checksum.data.sale_sum = 0;
            checksum.data.total = 0;
            crnt_sum = 0;
            crnt_check = pos->check;
        }
        checksum.data.total++;
        crnt_sum += do_real_to_money(pos->price*pos->quantity);
        do_set_empty(check);
        check.size = sizeof(check_struct_t);
        check.data.cash = shift->cash;
        check.data.shift = shift->shift;
        check.data.check = pos->check;
        check.data.position = pos->pos;
        check.data.date = do_datetosql(pos->time);
        check.data.time = do_timetosql(pos->time);
        check.data.sale_price = pos->price;
        check.data.quantity = pos->quantity;
        do_text_set(alias, check.data.product_code, pos->code);
        do_text_set(alias, check.data.bar_code, pos->barcode);
        check.data.cashier = pos->cashier;
        check.data.valuta = pos->valuta;
        do_text_set(alias, check.data.operation, pos->operation);
        if ( do_check_insert(alias, &check) != DO_OK)
            return FALSE;
    }
    if ( crnt_check ) {
        checksum.data.sale_sum = crnt_sum/100.;
        if ( do_checksum_insert(alias, &checksum) != DO_OK )
            return FALSE;

    }
    return TRUE;
}
static do_list_t *do_action_shift_document_get(do_alias_t *alias, shift_t *shift, do_extended_break_func break_func)
{
    do_list_t *retval;
    char buf[30];
    document_key0_t document_key0;
    document_key3_t document_key3;
    document_rec_t document;
    BTI_CHAR p_doc[20];
    BTI_CHAR p_g_code[3];

    int status;

    retval = do_list_new(TRUE);

    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_CRNTSALE));
    do_text_set(alias, document_key0.sklad, shift->sklad);
    sprintf(buf, "%2.2d%4.4d", shift->cash, shift->shift);
    do_text_set(alias, document_key0.document, buf);
    sprintf(buf, "%2.2d/%6.6d", shift->cash, shift->shift);
    do_text_set(alias, p_doc, buf);
    do_text_set(alias, p_g_code, do_param(DO_PARAM_PARTNER_REGION_CASH));

    switch ( do_document_key0(alias, &document_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            do_list_add_alloc(retval, &document_key0, sizeof(document_key0));
            break;
        case DO_KEY_NOT_FOUND:
            break;
        default:
            do_list_free(retval);
            return NULL;
    }
    do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    do_text_set(alias, document_key3.sklad, shift->sklad);
    do_cpy(document_key0.dtype, document_key3.dtype);
    do_cpy(document_key0.sklad, document_key3.sklad);
    BTI_LONG date_;
    document_key3.date = do_date_set(&date_, shift->time);
    document_key3.date -= DATE_DIF;
    document_key3.time = 0;

    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);

    while ( status == DO_OK ) {
        if ( !RUNNING_ ) {
            status = DO_ERROR;
            break;
        }
        if ( do_cmp(document_key0.dtype, document_key3.dtype) ||
             do_cmp(document_key0.sklad, document_key3.sklad) )
            break;
        if ( !do_cmp(document.data.p_g_code, p_g_code) &&
             document.data.p_code == shift->cash &&
             !do_cmp(document.data.p_doc, p_doc) ) {
            do_cpy(document_key0.document, document.data.document);
            do_list_add_alloc(retval, &document_key0, sizeof(document_key0));
        }
        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        do_list_free(retval);
        return NULL;
    }
    return retval;
}
#endif
typedef struct {
    char *code;
    double quantity;
    double price;
} product_sale_t;

typedef struct {
    char *info;
    double sum;
} info_sale_t;

typedef struct {
    char *operation;
    double sum;
    int total;
    do_sort_list_t *sale;
    do_list_t *info;
} sale_t;

static int product_sale_cmp_func (product_sale_t *p1, product_sale_t *p2)
{
    int i;
    i = strcmp(p1->code, p2->code);
    if ( !i ) {
        if (p1->price > p2->price)
            return 1;
        if (p1->price < p2->price)
            return -1;
        return 0;
    }
    return i;
}

#ifndef DOMINO78
static int do_action_doc_set_info(do_alias_t *alias, shift_t *shift, sale_t *sale, int check, do_list_t *info_l, do_extended_break_func break_func)
{
    int i, j;
    money_t *money;
    info_sale_t *info;
    int res;
    res = FALSE;

    for ( i = 0; i < shift->money->count; i++ ) {
        money = shift->money->list[i];
        if ( money->check > check )
            break;
        if ( money->check == check ) {
            if ( money->operation == 70 ) {
                res = TRUE;
                info = NULL;
                for ( j = 0; j < info_l->count; j++ ) {
                    if ( !strcmp(((info_sale_t*)info_l->list[j])->info, money->info) ) {
                        info = info_l->list[j];
                        info->sum += money->sum;
                        break;
                    }
                }
                if ( !info ) {
                    info = do_malloc0(sizeof(info_sale_t));
                    info->info = money->info;
                    info->sum  = money->sum;
                    do_list_add_alloc(info_l, info, sizeof(info_sale_t));
                }
            }
        }
    }
    return res;
}
static int do_action_doc_create_order(do_alias_t *alias, document_rec_t *document, sale_t *sale, do_accept_param_t *param, do_extended_break_func break_func)
{
    int i, res;
    document_order_rec_t document_order;
    product_sale_t *pr;

    do_cpy(document_order.data.dtype, document->data.dtype);
    do_cpy(document_order.data.dtype1, document->data.dtype1);
    do_cpy(document_order.data.sklad, document->data.sklad);
    do_cpy(document_order.data.document, document->data.document);
    document_order.data.line = 0;
    document_order.data.sort = 0;
    document_order.data.date = document->data.date;
    document_order.data.time = document->data.time;

    res = TRUE;
    for ( i = 0; i < sale->sale->count && res; i++ ) {
        pr = sale->sale->list[i];
        document_order.data.line++;
        do_text_set(alias, document_order.data.code, pr->code);
        do_document_order_quant_clear(alias, &document_order);
        do_document_order_quant_set(alias, &document_order, DO_CONST_QUANT_REST, pr->quantity);
        do_document_order_price_clear(alias, &document_order);
        do_document_order_price_set(alias, &document_order, DO_CONST_PRICE_FACT, pr->price);
        document_order.data.accepted = 0;
        do_document_order_params_clear(alias, &document_order);
        res = do_accept_order(alias, document, &document_order, DO_ACCEPT_AND_INSERT, param);
}

    return res;
}
static int do_action_doc_create(do_alias_t *alias, shift_t *shift, sale_t *sale, do_extended_break_func break_func)
{
    document_key0_t document_key0;
    document_rec_t document;
    do_doctype_t *doctype;

    char buf[512];
    char *value;
    int i;

    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    do_text_set(alias, document_key0.sklad, shift->sklad);

    if ( !do_util_get_empty_document_number(alias, &document_key0) )
        return FALSE;
    value = do_text(alias, document_key0.document);
    do_log(LOG_INFO, "create document \"%s.%s.%s\"", do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT), shift->sklad, value);
    do_free(value);

    do_cpy(document.data.dtype, document_key0.dtype);
    do_text_set(alias, document.data.dtype1, sale->operation);
    do_text_set_empty(document.data.name);
    do_cpy(document.data.sklad, document_key0.sklad);
    do_cpy(document.data.document, document_key0.document);
    DTI_LONG date_,time_;
    document.data.date = do_date_set(&date_, shift->time);
    document.data.time = do_time_set(&time_, shift->time);
    do_text_set(alias, document.data.p_g_code, do_param(DO_PARAM_PARTNER_REGION_CASH));
    document.data.p_code = shift->cash;
    sprintf(buf, "%2.2d/%6.6d", shift->cash, shift->shift);
    do_text_set(alias, document.data.p_doc, buf);
    document.data.p_date = do_date_set(&date_, shift->time);
    document.data.p_time = do_time_set(&time_, shift->time);
    do_text_set_empty(document.data.party);
    document.data.accepted = 1;
    do_document_params_clear(alias, &document);

    do_document_param_float_set(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_CHECK_SUM), sale->sum);
    do_document_param_int_set(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_CHECK_TOTAL), sale->total);
    do_document_param_set(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_CASHIER), shift->cashier);
    for ( i = 0; i < sale->info->count; i++ ) {
        info_sale_t *info;
        info = sale->info->list[i];
        sprintf(buf, "%s%d", do_param(DO_PARAM_DOCUMENT_PARAMETR_INFO_PAY), i+1);
        do_document_param_set(alias, &document, buf, info->info);
        sprintf(buf, "%ssum_%d", do_param(DO_PARAM_DOCUMENT_PARAMETR_INFO_PAY), i+1);
        do_document_param_float_set(alias, &document, buf, info->sum);
    };
    doctype = do_doctype_new_from_document(alias, &document);
    if ( !doctype )
        return FALSE;

    int res;
    double *sums;
    sums = do_malloc0(sizeof(double)*doctype->sums->count);
    do_accept_param_t param;
    param.dtype = doctype;
    param.sums = sums;
    param.protocol = FALSE;
    char *sk;
    char *prefix;
    sk = do_text(alias, document.data.sklad);
    prefix = domino_get_unit_prefix(alias, sk);
    do_free(sk);
    if (!prefix || *prefix == '\0') {
        do_log(LOG_ERR, "local product's prefix are empty");
        if (prefix)
            do_free(prefix);
        res = FALSE;

    }
    param.pref = do_strdup(prefix);
    do_free(prefix);

    if ( res )
        res = do_action_doc_create_order(alias, &document, sale, &param, break_func);
    if ( !RUNNING_ )
        res = FALSE;
    for ( i = 0; i < doctype->sums->count; i++ ) {
        int *n;
        n = doctype->sums->list[i];
        do_document_sum_set(alias, &document, *n, sums[i]);
    }
    if ( res )
        res = do_document_insert(alias, &document) == DO_OK;
    if ( !RUNNING_ )
        res = FALSE;
    if ( res )
        res = do_protocol_document_add_rec(alias, &document, DO_PROTOCOL_ACTION_INSERT);
    if ( res )
        res = do_accept_make_sums_prowod(alias, &document, doctype, break_func);
    if ( res )
        res = do_accept_make_prowod(alias, &document, doctype, break_func);
    do_doctype_free(doctype);
    do_free(sums);
    return res;
}
static int do_action_shift_document_create(do_alias_t *alias, shift_t *shift, do_extended_break_func break_func)
{
    sale_t *sale;
    do_list_t *sales;
    product_sale_t p, *pr;
    check_pos_t *pos;
    int i, indx, j, crnt_check;

    sales = do_list_new(TRUE);
    sale = NULL;
    crnt_check = 0;

    for ( i = 0; i < shift->check_pos->count; i++ ) {

        pos = shift->check_pos->list[i];
        sale = NULL;
        for ( j = 0; j < sales->count; j++ )
            if ( !strcmp(((sale_t*)sales->list[j])->operation, pos->operation) ) {
                sale = sales->list[j];
                break;
            }
        if ( !sale ) {
            sale = do_malloc0(sizeof(sale_t));
            sale->operation = pos->operation;
            sale->sale = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)product_sale_cmp_func, NULL);
            sale->info = do_list_new(TRUE);
            do_list_add(sales, sale);
        }
        if ( crnt_check != pos->check) {
            crnt_check = pos->check;
            sale->total++;
            do_action_doc_set_info(alias, shift, sale, crnt_check, sale->info, break_func);
        }
        sale->sum += pos->price*pos->quantity;
        p.code = pos->code;
        p.price = pos->price;
        p.quantity = pos->quantity;
        if ( do_sort_list_find(sale->sale, &indx, &p) ) {
            pr = sale->sale->list[indx];
            pr->quantity += p.quantity;
        }
        else
            do_sort_list_insert_new(sale->sale, indx, &p, sizeof(p));
    }
    int res;
    res = TRUE;
    for ( i = 0; i < sales->count && res; i++ ) {
        if ( !RUNNING_ ) {
            res = FALSE;
            break;
        }
        res = do_action_doc_create(alias, shift, sales->list[i], break_func);
    }

    for ( i = 0; i < sales->count; i++ ) {
        do_sort_list_free(((sale_t*)sales->list[i])->sale);
        do_list_free(((sale_t*)sales->list[i])->info);
    }
    do_list_free(sales);

    return res;
}
DO_EXPORT int do_action_receive_cl(do_alias_t *alias, const char *filename, do_extended_break_func break_func)
{
    shift_t *shift;

    do_log(LOG_INFO, "parse file \"%s\"", filename);
    shift = do_action_read_cl(alias, filename, break_func);
    if ( !shift )
        return FALSE;

    if ( !do_alias_tran_begin(alias) ) {
        shift_free(shift);
        return FALSE;
    }
    int res, i;
    do_list_t *docs;

    res = TRUE;
    docs = do_action_shift_document_get(alias, shift, break_func);
    if ( !docs )
        res = FALSE;

    for ( i = 0; i < docs->count && res; i++ ) {
        document_key0_t *document_key0;
        char *dtype, *sklad, *document;
        document_key0 = docs->list[i];
        dtype = do_text(alias, document_key0->dtype);
        sklad = do_text(alias, document_key0->sklad);
        document = do_text(alias, document_key0->document);
        do_log(LOG_INFO, "delete document \"%s.%s.%s\"", dtype, sklad, document);
        do_free(dtype);do_free(sklad);do_free(document);

        res = do_action_delete_document(alias, document_key0);
    }
    if ( res ) {
        do_log(LOG_INFO, "clear checks \"%d.%d\"", shift->cash, shift->shift);
        res = do_action_shift_delete(alias, shift, break_func);
    }
    if ( res ) {
        do_log(LOG_INFO, "create checks \"%d.%d\"", shift->cash, shift->shift);
        res = do_action_shift_create(alias, shift, break_func);
    }
    if ( res ) {
        do_log(LOG_INFO, "create documents \"%d.%d\"", shift->cash, shift->shift);
        res = do_action_shift_document_create(alias, shift, break_func);
    }

    if ( res )
        res = do_alias_tran_end(alias);
    else
        do_alias_tran_abort(alias);

    shift_free(shift);
    return res;
}
#endif