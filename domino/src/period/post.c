#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <domino.h>
#include "../misc/forms.h"
#include "../misc/print.h"

#include <time.h>
#include <locale.h>
#include <stddef.h>

#define FIRM_OUT "03.219"
#define OUT_DTYPE "55"
#define HOST "operator-5"

typedef struct {
    char code[20];
    const char *manufactor;
    const char *certificate;
    const char *reg;

} medical_item_t;

static int item_cmp(medical_item_t *i1, medical_item_t *i2)
{
    return strcmp(i1->code, i2->code);
}
static int print_registry(do_alias_t *alias, do_sort_list_t *medical);
static int print_sklad(do_alias_t *alias, const char *sklad, do_sort_list_t *medical);
static int print_doc(do_alias_t *alias, document_key0_t *document_key0, do_sort_list_t *medical);
int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();
    do_alias_t *alias;
    do_xls_t *xls;
    do_sort_list_t *medical;


    if (!domino_init(NULL, FALSE, DO_PARAM_END))
        exit(1);

    if ((alias = domino_alias_new("local.10")) == NULL)
        exit(1);
    if ( !do_alias_open(alias, TRUE))
        exit(1);

    xls = do_xls_new("/home/mirror/Docs/Список.xls", TRUE);
    if ( !xls )
        exit(1);
    int i;
    if ( do_xls_worksheet_count(xls) < 1 )
        exit(1);
    medical = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)item_cmp, NULL);
    for ( i = 0; i < do_xls_row_count(xls, 0); i++ ) {
        int code;
        medical_item_t item;
        code = atoi(do_xls_cell_str(xls, 0, i, 1));
        if ( !code ) {
            do_log(LOG_ERR, "Ошибка в строке %d %s", i+1, do_xls_cell_str(xls, 0, i, 1));
            exit(1);
        }
        sprintf(item.code, "%5.5d", code);
        item.reg = do_xls_cell_str(xls, 0, i, 5);
        item.manufactor = do_xls_cell_str(xls, 0, i, 7);
        item.certificate = do_xls_cell_str(xls, 0, i, 6);
        do_sort_list_add_alloc(medical, &item, sizeof(item));
    }
    if ( !print_registry(alias, medical) )
        exit(1);

    do_alias_free(alias);
    return 0;
}
static int print_registry(do_alias_t *alias, do_sort_list_t *medical)
{
    int i;
    for ( i = 1; i <= 12; i++ ) {
        char buf[10];
        sprintf(buf, "%d", i);
        if ( !print_sklad(alias, buf, medical) )
            return FALSE;
    }
    return TRUE;
}
static int print_sklad(do_alias_t *alias, const char *sklad, do_sort_list_t *medical)
{
    document_key3_t document_key3, key;
    document_rec_t document;
    partner_key0_t partner_key0;
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    int status;
    int res;
    do_list_t *list;

    list = do_list_new(TRUE);

    char *firm = do_strdup(FIRM_OUT);
    char *ch;
    do_log(LOG_INFO, "Sklad %s", sklad);
    if ( (ch = strchr(firm, '.')) == NULL ) {
        do_log(LOG_ERR, "Undefine partner out");
        do_free(firm);
        return FALSE;
    }
    else {
        *ch = '\0';
        do_text_set(alias, partner_key0.g_code, firm);
        partner_key0.code = atoi(ch + 1);
    }
    do_free(firm);


    do_text_set(alias, key.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    do_text_set(alias, key.sklad, sklad);
    do_cpy(document_key3.dtype, key.dtype);
    do_cpy(document_key3.sklad, key.sklad);
    document_key3.date = 76655;
    document_key3.time = 0;
    document_key0_t document_key0;

    res = TRUE;
    int found;

    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(document_key3.dtype, key.dtype) ||
             do_cmp(document_key3.sklad, key.sklad) ||
             document_key3.date > 76655 ) break;
        if ( !strncmp(document.data.dtype1, OUT_DTYPE, strlen(OUT_DTYPE)) &&
             !do_cmp(document.data.p_g_code, partner_key0.g_code) &&
             !do_cmp(document.data.p_code, partner_key0.code)
            )
        {
            do_cpy(document_key0.dtype, document.data.dtype);
            do_cpy(document_key0.sklad, document.data.sklad);
            do_cpy(document_key0.document, document.data.document);


            do_cpy(document_order_key0.dtype, document_key0.dtype);
            do_cpy(document_order_key0.sklad, document_key0.sklad);
            do_cpy(document_order_key0.document, document_key0.document);
            document_order_key0.line = 0;

            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
            found = FALSE;
            while ( status == DO_OK ) {
                if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                     do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                     do_cmp(document_order_key0.document, document_key0.document) ) break;

                medical_item_t item;
                int indx;
                strcpy(item.code, do_text(alias, document_order.data.code));
                item.code[5] = '\0';

                if ( do_sort_list_find(medical, &indx, &item) ) {
                    found = TRUE;
                    break;
                }
                status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
            }
            if ( status == DO_ERROR ) break;

            if ( found ) {
                do_list_add_alloc(list, &document_key0, sizeof(document_key0));
            }
        }
        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        res = FALSE;
    int i;
    do_log(LOG_INFO, "count %d", list->count);
    for ( i = 0; i < list->count; i++ )
        if ( !print_doc(alias, list->list[i], medical) )
            exit(1);

    return res;
}
#define VALUE_SET_STR(a,b) { value = b; \
                             do_template_set(templ, a, value, NULL );\
                             do_free(value);}
#define VALUE_SET_INT(a,b) { value = do_malloc(50); \
			     sprintf(value, "%d", b); \
                             do_template_set(templ, a, value, NULL );\
                             do_free(value);}
static int print_doc(do_alias_t *alias, document_key0_t *document_key0, do_sort_list_t *medical)
{
    do_template_t *templ;
    document_rec_t document;
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    struct tm time;
    int status;
    char *value;
    int line_indx;

    templ = do_template_new("register", TRUE);
    if ( !templ )
        return FALSE;

    do_template_cursor_t *line;
    do_template_cursor_t *cursor;

    cursor = do_template_cursor(templ);
    line = do_template_section(templ, "Line");

    switch ( do_document_get0(alias, &document, document_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_util_document_key0_to_str(alias, document_key0);
            do_log(LOG_ERR, "document \"%s\" not found", value);
            do_free(value);
        default:
            do_template_free(templ);
            return FALSE;
    }

    do_date(document.data.p_date, &time);
    VALUE_SET_STR("DocumentNumber", do_text(alias, document.data.p_doc));
    VALUE_SET_STR("DocumentDate", do_datetoa(time));
    line_indx = 0;

    do_template_cursor_run_step_for(cursor, "Line");

    do_cpy(document_order_key0.dtype, document_key0->dtype);
    do_cpy(document_order_key0.sklad, document_key0->sklad);
    do_cpy(document_order_key0.document, document_key0->document);
    document_order_key0.line = 0;

    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        medical_item_t item;
        int indx;

        if ( do_cmp(document_order_key0.dtype, document_key0->dtype) ||
             do_cmp(document_order_key0.sklad, document_key0->sklad) ||
             do_cmp(document_order_key0.document, document_key0->document) ) break;

        strcpy(item.code, do_text(alias, document_order.data.code));
        item.code[5] = '\0';
        if ( do_sort_list_find(medical, &indx, &item) ) {
            medical_item_t *item;
            product_key4_t product_key4;
            product_rec_t product;

            item = medical->list[indx];

            do_cpy(product_key4.code, document_order.data.code);
            switch ( do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) ) {
                case DO_OK:
                    break;
                case DO_KEY_NOT_FOUND:
                    value = do_text(alias, product_key4.code);
                    do_log(LOG_ERR, "product \"%s\" not found", value);
                    do_free(value);
                default:
                    do_template_free(templ);
                    return FALSE;
            }
            line_indx++;
            VALUE_SET_INT("Number", line_indx);
            VALUE_SET_STR("ProductName", do_text(alias, product.data.name));
            VALUE_SET_STR("ProductSeria",
                          do_product_param(alias, &product,
                                           do_param(DO_PARAM_PRODUCT_PARAMETR_SERIA)));
            VALUE_SET_STR("Manufactor", do_strdup(item->manufactor));
            VALUE_SET_STR("Certificate", do_strdup(item->certificate));
            VALUE_SET_STR("Date", do_strdup("item->date"));
            VALUE_SET_STR("Organization", do_strdup("item->organization"));
            VALUE_SET_STR("Registry", do_strdup(item->reg));

            do_template_cursor_first_run(line);
        }
        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
    }

    do_template_cursor_skip(cursor);
    do_template_cursor_run(cursor);

    if ( status == DO_ERROR ) {
        do_template_free(templ);
        return FALSE;
    }

    char *str;
    str = do_template_text(templ);
    if ( !str )
        return FALSE;
    do_log(LOG_INFO, "Печать документа %s", do_util_document_key0_to_str(alias, document_key0));
    if ( !print_html(HOST, str) )
        return FALSE;

    DO_SEC_SLEEP(10);

    return FALSE;
}
