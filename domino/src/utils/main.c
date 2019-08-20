#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <domino.h>
#include <../config/config.h>

typedef struct {
    product_key4_t product_key4;
    int rest[3];
} rest_t;

static int cmp(rest_t *s1, rest_t *s2)
{
    return do_cmp(s1->product_key4.code, s2->product_key4.code);
}
static int get_order(do_alias_t *alias, do_sort_list_t *list)
{
    const char *dtypes[3];
    document_order_key0_t document_order_key0;
    struct PACK_STRUCT {
        BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
        BTI_BYTE quant[4][7];
        BTI_SINT nquant[4];
        BTI_BYTE accepted;
    } *rec;
    do_extended_t *extended;
    int indx;
    int retval;

    dtypes[2] = do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN);
    dtypes[1] = do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT);
    dtypes[0] = do_param(DO_PARAM_DOCUMENT_TYPE_CRNTSALE);

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);
    do_extended_set_key(extended, 0, &document_order_key0);
    do_extended_set_filter(extended, TRUE, 2,
                           sizeof(document_order_key0.dtype),
                           sizeof(document_order_key0.sklad));
    do_extended_set_filter_term(extended,
                                0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, dtype),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_order_key0.dtype);
    do_extended_set_filter_term(extended,
                                1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &document_order_key0.sklad);
    do_extended_set_fields(extended, 4,
                           do_size_and_offset(document_order_struct_t, code),
                           do_size_and_offset(document_order_struct_t, quant),
                           do_size_and_offset(document_order_struct_t, nquant),
                           do_size_and_offset(document_order_struct_t, accepted)
                           ); // size,offset
    retval = TRUE;
    for ( indx = 0; indx < 3; indx++ ) {

        int nrest;
        int sign;
        int i, j;
        do_ret_list_t *ret;
        rest_t rest;

        sign = ( indx != 2 ) ? -1 : 1;
        nrest = ( indx ) ? 1 : 2;

        do_text_set(alias, document_order_key0.dtype, dtypes[indx]);
        do_text_set(alias, document_order_key0.sklad, domino_unit());
        do_text_set_empty(document_order_key0.document);
        document_order_key0.line = 0;
        do_extended_set_key(extended, 0, &document_order_key0);

        do_extended_set_filter_term_set_value(extended, 0, &document_order_key0.dtype);
        do_extended_set_filter_term_set_value(extended, 1, &document_order_key0.sklad);
        do_log(LOG_INFO, "Read \"%s.\%s\"", do_text(alias, document_order_key0.dtype),
               do_text(alias, document_order_key0.sklad));
        ret = do_extended_get(extended, -1);
        if ( !ret ) {
            retval = FALSE;
            break;
        }
        for ( i = 0; i < do_ret_list_count(ret); i++ ) {
            rec = do_ret_list_item_data(ret, i);
            if ( !rec->accepted ) continue;

            do_cpy(rest.product_key4.code, rec->code);
            memset(&rest.rest, 0, sizeof(rest.rest));
            for ( j = 0; j < 4; j ++ )
                if ( rec->nquant[j] == nrest ) {
                    rest.rest[nrest-1] =
                        do_pdecimal(rec->quant[j], 7, 3) * 1000 * sign;
                    break;
                }
            if ( do_sort_list_find(list, &j, &rest) ) {
                rest_t *stock;
                stock = list->list[j];
                stock->rest[0] += rest.rest[0];
                stock->rest[1] += rest.rest[1];
            }
            else
                do_sort_list_insert_new(list, j, &rest, sizeof(rest));
        }
        do_ret_list_free(ret);
    }
    do_extended_free(extended);
    return retval;

}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();
    do_alias_t *alias;
    do_sort_list_t *list;
    int res;
    //printf("size %d\n", sizeof(partner_struct_t));
//    exit(1);

    if ( !domino_init(NULL, FALSE, DO_PARAM_END) )
        exit(1);

    if ( (alias = domino_alias_new(NULL)) == NULL )
        exit(1);

    if ( !do_alias_open(alias, TRUE) )
        exit(1);
    if ( !do_alias_tran_begin(alias) ) {
        do_alias_free(alias);
        exit(1);
    }

    list = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)cmp, NULL);

    res = get_order(alias, list);
    if ( res ) {
        stock_rec_t stock;
        stock_key0_t stock_key0, key;
        int status;

        do_text_set(alias, stock_key0.sklad, domino_unit());
        do_text_set_empty(stock_key0.code);
        do_cpy(key.sklad, stock_key0.sklad);
        do_log(LOG_INFO, "Update stock");

        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_GE);
        while ( status == DO_OK ) {
            rest_t *rest, r;
            int indx;

            if ( do_cmp(key.sklad, stock_key0.sklad) ) break;

            do_cpy(r.product_key4.code, stock.data.code);
            if ( !do_sort_list_find(list, &indx, &r) ) {
                if ( do_stock_quant(alias, &stock, 1) != 0 ||
                     do_stock_quant(alias, &stock, 2) != 0 ) {
                    do_log(LOG_ERR, "%s %f %f 0 0",
                           do_text(alias, stock.data.code),
                           do_stock_quant(alias, &stock, 1),
                           do_stock_quant(alias, &stock, 2));

                    do_stock_quant_set(alias, &stock, 1, 0);
                    do_stock_quant_set(alias, &stock, 2, 0);
                    if ( do_stock_update(alias, &stock) != DO_OK ) {
                        res = FALSE;
                        break;
                    }
                }
            }
            else {
                rest = list->list[indx];
                rest->product_key4.code[0] = '\0';
                if ( do_stock_quant(alias, &stock, 1) != rest->rest[0]/1000. ||
                     do_stock_quant(alias, &stock, 2) != rest->rest[1]/1000. ) {
                    do_log(LOG_ERR, "%s %f %f %f %f",
                           do_text(alias, stock.data.code),
                           do_stock_quant(alias, &stock, 1),
                           do_stock_quant(alias, &stock, 2),
                           rest->rest[0]/1000.,
                           rest->rest[1]/1000.
                           );

                    do_stock_quant_set(alias, &stock, 1, rest->rest[0]/1000.);
                    do_stock_quant_set(alias, &stock, 2, rest->rest[1]/1000.);
                    if ( do_stock_update(alias, &stock) != DO_OK ) {
                        res = FALSE;
                        break;
                    }
                }
            }

            status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_NEXT);
        }
        if ( status == DO_ERROR )
            res = FALSE;
        if ( res ) {
            int i;
            do_text_set(alias, stock.data.sklad, domino_unit());
            for ( i = 0; i < list->count; i++ ) {
                rest_t *rest;
                rest = list->list[i];
                if ( rest->product_key4.code[0] != '\0' ) {
                    do_log(LOG_ERR, "%s %f %f ",
                           do_text(alias, rest->product_key4.code),
                           rest->rest[0]/1000.,
                           rest->rest[1]/1000.
                           );
                    do_cpy(stock.data.code, rest->product_key4.code);
                    stock.data.nquant[0] = 0;
                    stock.data.nquant[1] = 0;
                    stock.data.nquant[2] = 0;
                    stock.data.nquant[3] = 0;
                    stock.data.nquant[4] = 0;
                    stock.data.nquant[5] = 0;
                    do_stock_quant_set(alias, &stock, 1, rest->rest[0]/1000.);
                    do_stock_quant_set(alias, &stock, 2, rest->rest[1]/1000.);
                    if ( do_stock_insert(alias, &stock) != DO_OK ) {
                        res = FALSE;
                        break;
                    }
                }
            }
        }
    }
    if ( res )
        do_alias_tran_end(alias);
    else
        do_alias_tran_abort(alias);

    do_alias_free(alias);
    return !res;
}
