#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <domino.h>
#include "../misc/iconv_.h"

void print_usage()
{
    fprintf(stderr, "usage: get-stock <alias> <sklad>\n"
         "\n"
         "Copyright (C) 2008-2011 Youra Richesse\n"
         "send_file is free software and comes with ABSOLUTELY NO WARRANTY\n"
         "under the terms of the GNU General Public License\n"
         "\n"
         "Report bugs to send_file Support Center:\n"
         "  mir@glekar.ru\n");
    exit(1);
}
do_list_t *get_stock(do_alias_t *alias, const char *sklad);
iconv_t conv=(iconv)-1;
#define OUT_CHARSET "CP1251"
//#define OUT_CHARSET "UTF-8"
char *text(const char *text)
{
    char *result;
    char *ch;
    result = iconv_(conv, text);

    for ( ch = result; *ch != '\0'; ch++ )
        if ( *ch == ';' )
            *ch = ' ';
    return result;

}
typedef struct {
    char *code;
    double price;
    int stock;
} stock_t;
int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    setlocale(LC_NUMERIC,"C");
    
    do_log_set_stdout();
    do_alias_t *alias;
    do_sort_list_t *stock;
    int i, status;
    char *sklad;

    conv = iconv_open(OUT_CHARSET, get_locale_lang());
    if ( argc != 3 )
        print_usage();

    if ( !domino_init(NULL, FALSE, DO_PARAM_END) )
        exit(1);

    if ( (alias = domino_alias_new(argv[1])) == NULL )
        exit(1);

    if ( !do_alias_open(alias, TRUE) )
        exit(1);

    if ( !do_alias_tran_begin(alias) ) {
        do_alias_free(alias);
        exit(1);
    }
    sklad = do_strdup(argv[2]);

    stock = get_stock(alias, sklad);
    if ( !stock ) {
        do_alias_free(alias);
        exit(1);
    }
    setlocale(LC_NUMERIC,"C");
    for ( i = 0; i < stock->count; i++ ) {
        stock_t *rec;
        product_key4_t product_key4;
        product_rec_t product;
        partner_rec_t partner;
        partner_key0_t partner_key0;

        rec = stock->list[i];
        do_text_set(alias, product_key4.code, rec->code);
        status = do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL);
        if ( status != DO_OK ) {
            do_log(LOG_ERR, "product \"%s\" not found", do_text(alias, rec->code));
            exit(1);
        }
        do_cpy(partner_key0.g_code, product.data.mfr_region);
        do_cpy(partner_key0.code, product.data.mfr_code);
        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
        if ( status != DO_OK ) {
            do_log(LOG_ERR, "partner \"%s.%d\" not found", do_text(alias, partner_key0.g_code), partner_key0.code);
            exit(1);
        }

        fprintf(stderr, "%s;%s;%s;%d;%d;%d;%.2f;1\n",
                text(do_text(alias, product.data.code)),
                text(do_text(alias, product.data.name)),
                text(do_text(alias, partner.data.name)),
                do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) ==
                    DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE,
                do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) ==
                    3,
                do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_NDS)),
                rec->price
                     );


    }
    do_alias_free(alias);
    return 0;
}
do_list_t *get_stock(do_alias_t *alias, const char *sklad)
{
    stock_key0_t stock_key0, key;
    stock_rec_t stock;
    barcode_key1_t barcode_key1;
    int status;
    stock_t crnt;
    do_list_t *retval;

    retval = do_list_new(TRUE);
    crnt.code = do_strdup("");
    crnt.stock = FALSE;
    crnt.price = 0;
    do_text_set(alias, stock_key0.sklad, sklad);
    do_text_set_empty(stock_key0.code);
    do_cpy(key.sklad, stock_key0.sklad);

    status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.sklad, stock_key0.sklad) ) break;
        if ( strncmp(stock.data.code, crnt.code, 5) ) {
            if ( crnt.stock ) {
                if ( crnt.price > 0 )
                    do_list_add_alloc(retval, &crnt, sizeof(crnt));
                else
                    do_log(LOG_ERR, "product \"%s\" no price", crnt.code);
            }
            crnt.code = do_text(alias, stock.data.code);
            crnt.code[5] = '\0';
            crnt.price = 0;
            crnt.stock = FALSE;
        }
        if ( stock.data.code[5] != '.' &&
             do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL) > 0
             ) {
            crnt.price = do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL);
        }
        if ( stock.data.code[5] == '.' &&
            do_stock_quant(alias, &stock, DO_CONST_QUANT_REST) +
            do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE) > 0
            ) {
            do_cpy(barcode_key1.code, stock.data.code);
            status = do_barcode_key1(alias, &barcode_key1, DO_GET_EQUAL);
            if ( status == DO_ERROR )
                break;
            if ( status == DO_OK ) {
                stock_t parcel;
                parcel.code = do_text(alias, stock.data.code);
                parcel.price = do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL);
                parcel.stock = TRUE;
                do_list_add_alloc(retval, &parcel, sizeof(parcel));
            }
            else {
                crnt.stock = TRUE;
            }
        }

        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_NEXT);
    }
    if ( crnt.stock ) {
        if ( crnt.price > 0 )
            do_list_add_alloc(retval, &crnt, sizeof(crnt));
        else
            do_log(LOG_ERR, "product \"%s\" no price", crnt.code);
    }

    if ( status == DO_ERROR )
        return NULL;
    return retval;
}
