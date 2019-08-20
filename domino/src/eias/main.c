#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolib.h>
#include <domino.h>
#include <errno.h>
#include <locale.h>
#include "../misc/csv_util.h"

#define fileout "out.csv"
#define UNITS "13,14,15"
do_list_t *units;
document_key0_t pt_doc, rt_doc;

BTI_LONG date1, date2;


static do_string_list_t *get_list(const char *filename)
{
    do_string_list_t *retval;

    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening file (%s): %s\n", filename, strerror(errno));
        return NULL;
    }
    retval = do_string_list_new();

    size_t len = 1023;
    char* line = (char*) do_malloc(len + 1);
    char *cp;
    while (!feof(fp))  {
        if (fgets(line, len+1, fp) == NULL) break;
        while ((strlen(line) == len) && !feof(fp)) {
            line = (char *)do_realloc(line, len * 2 + 1);
            if (!fgets(line+len, len+1, fp)) {
                do_free(line);
                do_log(LOG_ERR, "Error reading file (%s): %s\n", filename, strerror(errno));
                do_string_list_free(retval);
                return NULL;
            }

            len *= 2;
        }
        cp = line;
        while (!CSV_EOS(cp))
           ++cp;
	    *cp = '\0';
        do_string_list_add_alloc(retval, line);
    }
    do_free(line);
    fclose(fp);
    return retval;
}
int mk_product(do_alias_t *alias, const char *bar, double *total, double *purchase, double *retail);

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();
    do_alias_t *alias;
    units = do_list_new(TRUE);


    FILE *fp;
    if ((fp = fopen(fileout, "w+")) == NULL)  {
        do_log(LOG_ERR, "Error write file (%s): %s\n", fileout, strerror(errno));
        exit(1);
    }

    if ( !domino_init(NULL, FALSE, DO_PARAM_END) )
        exit(1);

    if ( (alias = domino_alias_new("main.net")) == NULL )
        exit(1);
    if ( !do_alias_open(alias, TRUE) )
        exit(1);

    int i;
    do_string_list_t *list;

    list = do_string_list_new_from_csv(UNITS);
    for ( i = 0; i < list->count; i++ ) {
        sklad_key0_t sklad_key0;
        do_text_set(alias, sklad_key0.code, list->list[i]);
        do_list_add_alloc(units, &sklad_key0, sizeof(sklad_key0));
    }
    do_text_set(alias, pt_doc.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
    do_text_set(alias, rt_doc.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    struct tm tm;
    do_atodate("01/01/2011", &tm);
    do_date_set(&date1, tm);
    do_atodate("31/12/2011", &tm);
    do_date_set(&date2, tm);


    list = get_list("list.csv");

    if ( list ) {
        do_log(LOG_INFO, "list count %d", list->count);
        int count = 0;
        double total,purchase,retail;

        for ( i = 0; i < list->count; i++ ) {
            if ( strcmp(list->list[i], "4607003242749") ) continue;

            if ( !mk_product(alias, list->list[i], &total, &purchase, &retail) )
                exit(1);

            if ( total != 0 ) {
                count++;
            }
            fprintf(fp, "\"%s\";%f;%f;%f\n", (char*)list->list[i], total, purchase, retail);
            if ( i % 10 == 0 )
                do_log(LOG_INFO, "process %d", i);

        }
        do_log(LOG_INFO, "proccessed %d", count);
    }
    fclose(fp);

    do_alias_free(alias);
    return 0;
}
int is_bar(do_alias_t *alias, product_key0_t *product_key0, const char *bar);
int get_r(do_alias_t *alias, product_key0_t *product_key0, double *total, double *purchase, double *retail);
int mk_product(do_alias_t *alias, const char *bar, double *total, double *purchase, double *retail)
{

    barcode_key0_t barcode_key0;
    barcode_rec_t barcode;
    int status;
    *total = 0;
    *purchase = 0;
    *retail = 0;


    do_text_set(alias, barcode_key0.barcode, bar);
    status = do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return FALSE;

    if ( status == DO_KEY_NOT_FOUND )
        return TRUE;

    product_key0_t product_key0;
    char *code;

    code = do_text(alias, barcode.data.code);

    product_key0.base_parcel = 1;
    do_text_set(alias, product_key0.code, code);
    status = do_product_key0(alias, &product_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( strncmp(product_key0.code, code, 5) ||
             product_key0.base_parcel == 0 ) break;

        status = is_bar(alias, &product_key0, bar);
        if ( status == DO_ERROR )
            return FALSE;

        if (  status == DO_OK ) {
            if ( !get_r(alias, &product_key0, total, purchase, retail ))
                return FALSE;
        }
        status = do_product_key0(alias, &product_key0, DO_GET_NEXT);

    }
    if ( status == DO_ERROR )
        return FALSE;
    if ( *total != 0 ) {
        product_rec_t product;
        product_key0.base_parcel = 0;
        do_text_set(alias, product_key0.code, code);
        status = do_product_get0(alias, &product, &product_key0, DO_GET_GE);
        if ( status != DO_OK )
            return FALSE;
        *total = (*total)/do_product_coef(alias, &product);
    }

    return TRUE;
};
int is_bar(do_alias_t *alias, product_key0_t *product_key0, const char *bar)
{
    document_order_key3_t document_order_key3;

    document_order_rec_t document_order;
    int i;
    int status;

    do_cpy(document_order_key3.code, product_key0->code);
    document_order_key3.date = 0;
    document_order_key3.time = 0;

    status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(document_order_key3.code, product_key0->code) ) break;

        if ( !do_cmp(document_order.data.dtype, pt_doc.dtype) &&
            !strncmp(document_order.data.dtype1, "01", 2) ) {
            for ( i = 0; i < units->count; i++ ) {
                if ( !do_cmp(document_order.data.sklad, ((sklad_key0_t*)units->list[i])->code) ) {
                    char *s;
                    s = do_document_order_param(alias, &document_order, "BAR");
                    if ( !strcmp(s, bar) )
                        return DO_OK;
                    else
                        return DO_KEY_NOT_FOUND;
                }
            }
        }
        status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return status;

    return DO_KEY_NOT_FOUND;

}
int get_r(do_alias_t *alias, product_key0_t *product_key0, double *total, double *purchase, double *retail)
{
    document_order_key3_t document_order_key3;

    document_order_rec_t document_order;
    int i;
    int status;

    do_cpy(document_order_key3.code, product_key0->code);
    document_order_key3.date = date1;
    document_order_key3.time = 0;

    status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(document_order_key3.code, product_key0->code) ||
             document_order_key3.date > date2 ) break;

        if ( !do_cmp(document_order.data.dtype, rt_doc.dtype) &&
            (strncmp(document_order.data.dtype1, "05", 2) < 0) ) {
            for ( i = 0; i < units->count; i++ ) {
                if ( !do_cmp(document_order.data.sklad, ((sklad_key0_t*)units->list[i])->code) ) {
                    *total = *total + do_document_order_quant(alias, &document_order, 1);
                    *purchase = *purchase + do_document_order_quant(alias, &document_order, 1)*do_document_order_price(alias, &document_order, 2);
                    *retail = *retail + do_document_order_quant(alias, &document_order, 1)*do_document_order_price(alias, &document_order, 9);
                }
            }
        }
        status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;

    return TRUE;

}
