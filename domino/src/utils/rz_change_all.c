#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <domino.h>


int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();
    do_alias_t *alias;

    if ( !domino_init(NULL, FALSE, DO_PARAM_END) )
        exit(1);

    if ( (alias = domino_alias_new(NULL)) == NULL )
        exit(1);

    if ( !do_alias_open(alias, TRUE) )
        exit(1);

    product_rec_t product;
    product_key0_t product_key0;
    int status;
    do_list_t *list;

    list = do_list_new(TRUE);

    do_text_set_empty(product_key0.code);
    product_key0.base_parcel = 0;

    status = do_product_get0(alias, &product, &product_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( product_key0.base_parcel ) break;
        product_key0.base_parcel =
            do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_TEMPERATURE));
        if ( product_key0.base_parcel == 4 ||
             product_key0.base_parcel == 10
            ){
            do_list_add_alloc(list, &product_key0, sizeof(product_key0));
        }
        product_key0.base_parcel = 0;
        status = do_product_get0(alias, &product, &product_key0, DO_GET_NEXT);
    }
    if ( status != DO_ERROR ) {
        int i;
        for ( i = 0; i < list->count; i++ ) {
            product_key0_t *key;
            key = list->list[i];
            do_cpy(product_key0.code, key->code);
            product_key0.base_parcel = 0;
            if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK) {
                do_log(LOG_ERR, "Fuck");
                exit(1);
            }
            if ( key->base_parcel == 4 )
                do_product_param_int_set(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS),
                                     DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE);
            else
                do_product_param_int_set(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS),
                                     DO_CONST_PRODUCT_CLASS_DEFAULT);
            if ( do_product_update(alias, &product) != DO_OK ) {
                do_log(LOG_ERR, "Fuck");
                exit(1);
            }
            do_log(LOG_INFO, "%d %.0f %s",
                   do_get_rest(alias, do_text(alias, product.data.code), domino_unit()),
                   do_rz_info_exist_by_code(alias, do_text(alias, product.data.code)),
                   do_text(alias, product.data.name));
            do_cpy(product_key0.code, key->code);
            product_key0.base_parcel = 1;
            status = do_product_get0(alias, &product, &product_key0, DO_GET_GE);
            while ( status == DO_OK ) {
                barcode_key1_t barcode_key1;
                barcode_rec_t barcode;
                if ( strncmp(product_key0.code, key->code, 5) ||
                     product_key0.base_parcel != 1 ) break;

                if ( key->base_parcel == 4 )
                    do_product_param_int_set(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS),
                                         DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE);
                else
                    do_product_param_int_set(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS),
                                         DO_CONST_PRODUCT_CLASS_DEFAULT);
                if ( do_product_update(alias, &product) != DO_OK ) {
                    do_log(LOG_ERR, "Fuck");
                    exit(1);
                }
                do_log(LOG_INFO, "  %s", do_text(alias, product.data.code));
                if ( key->base_parcel == 4 ) {
                    do_cpy(barcode_key1.code, product.data.code);
                    status = do_barcode_get1(alias, &barcode, &barcode_key1, DO_GET_EQUAL);
                    while ( status == DO_OK ) {
                        if ( do_cmp(barcode_key1.code, product.data.code) ) break;
                        do_log(LOG_INFO, "      %s", do_text(alias, barcode.data.barcode));
                        if ( do_barcode_delete(alias) != DO_OK )
                            exit(1);
                        status = do_barcode_get1(alias, &barcode, &barcode_key1, DO_GET_NEXT);
                    }
                    if ( status == DO_ERROR )
                        exit(1);
                }

                status = do_product_get0(alias, &product, &product_key0, DO_GET_NEXT);
            }
            if ( status == DO_ERROR )
                exit(1);

        }
        do_log(LOG_INFO, "count %d", list->count);
    }

    do_alias_free(alias);
    return 0;
}
