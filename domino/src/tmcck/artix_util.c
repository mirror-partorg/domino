#include "artix_util.h"
#include <artix.h>
#include <domino.h>

typedef struct PACK_STRUCT {
    BTI_CHAR barcode[DOMINO_FIELD_BCODE_SIZE];
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
} barcodes_ret;

static int check_product(do_alias_t *alias, artix_alias_t *artix_alias, artix_product_t *product, product_key4_t *product_key4, product_rec_t *record, int nprice);
static int check_no_price(do_alias_t *alias, product_key4_t *product_key4, product_rec_t *product, int nprice);
static char *product_fullname(do_alias_t *alias, const char* barcode, product_rec_t *product, int nprice);
static char *artix_product_fullname(artix_alias_t *alias, artix_product_t *product);

static do_ret_list_t *get_barcodes(do_alias_t *alias)
{
    barcode_key0_t barcode_key0;
    do_extended_t *extended;
    do_ret_list_t *retval = NULL;

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_BARCODE);
    do_extended_set_fields(extended, 2,
                           do_size_and_offset(barcode_struct_t, barcode),
                           do_size_and_offset(barcode_struct_t, code));
    do_text_set_empty(barcode_key0.barcode);
    do_extended_set_key(extended, 0, &barcode_key0);
    retval = do_extended_get(extended, -1);
    return retval;
}

int artix_check_products(artix_t *artix, do_alias_t *alias, int nprice)
{
    do_ret_list_t *list = NULL;
    product_rec_t product;
    int key_count = 0;
    do_log(LOG_INFO, "Get domino products");
    if ((list = get_barcodes(alias)) == NULL) {
        return 0;
    }
//    do_log(LOG_ERR, "Barcodes count %d", do_ret_list_count(list));
    artix_product_t *products = NULL;
    char artix_bcode[ARTIX_FIELD_BCODE_SIZE];
    int product_count = 0;
    do_log(LOG_INFO, "Get cash products");
    if (!artix_alias_get_products(artix->alias, &products, &product_count, 1)) {
        if (list) do_ret_list_free(list);
        if (products) do_free(products);
        return 0;
    }
    int result = 1 , cmp;
    int pindx = 0,
#ifdef   SHOW_PROGRESS
        crntperc = 0,
#endif
        kindx = 0;
    key_count = do_ret_list_count(list);
    char *bcode, *mes, *mes1;
    for (; pindx < product_count && kindx < key_count; pindx++) {
        bcode = do_text(alias, ((barcodes_ret*)do_ret_list_item_data(list, kindx))->barcode);
        artix_text_set(artix->alias, artix_bcode, bcode);
        while ((cmp = memcmp(products[pindx].bcode, artix_bcode, ARTIX_FIELD_BCODE_SIZE)) > 0) {
            if (!check_no_price(alias, (product_key4_t*)((barcodes_ret*)do_ret_list_item_data(list, kindx))->code,
                         &product, nprice)) {
                mes = product_fullname(alias, bcode, &product, nprice);
                do_log(LOG_ERR, "Record not found %s", mes);
                do_free(mes);
                result = 0;
            }
            do_free(bcode);
            kindx++;
            bcode = do_text(alias, ((barcodes_ret*)do_ret_list_item_data(list, kindx))->barcode);
            artix_text_set(artix->alias, artix_bcode, bcode);
        }
        if (cmp < 0) {
#ifdef ARTIX_OLD_UPLOAD
            if (memcmp(products[pindx].code, products[pindx].bcode,
                (ARTIX_FIELD_BCODE_SIZE < ARTIX_FIELD_PRODUCT_CODE_SIZE) ? ARTIX_FIELD_BCODE_SIZE : ARTIX_FIELD_PRODUCT_CODE_SIZE)) {
#endif
                mes = artix_product_fullname(artix->alias, &products[pindx]);
                do_log(LOG_ERR, "Unnecessary record %s", mes);
                do_free(mes);
                result = 0;
#ifdef ARTIX_OLD_UPLOAD
            }
#endif
        }
        else {
            if (!check_product(alias, artix->alias, &products[pindx], (product_key4_t*)((barcodes_ret*)do_ret_list_item_data(list, kindx))->code,
                        &product, nprice)) {
                mes = artix_product_fullname(artix->alias, &products[pindx]);
                mes1 = product_fullname(alias, bcode, &product, nprice);
                do_log(LOG_ERR, "Record mismatch \n%s\n%s", mes, mes1);
                do_free(mes);
                result = 0;
            }
            kindx++;
        }
#ifdef SHOW_PROGRESS
        if (crntperc + 1 < (pindx * 100.0 / product_count) ) {
            crntperc = pindx * 100.0 / product_count;
            do_log(LOG_INFO, "%d percent has processed", crntperc);
        }
#endif
    }
    for (; pindx < product_count ; pindx++) {
#ifdef ARTIX_OLD_UPLOAD
        if (memcmp(products[pindx].code, products[pindx].bcode,
            (ARTIX_FIELD_BCODE_SIZE < ARTIX_FIELD_PRODUCT_CODE_SIZE) ? ARTIX_FIELD_BCODE_SIZE : ARTIX_FIELD_PRODUCT_CODE_SIZE)) {
#endif
            mes = artix_product_fullname(artix->alias, &products[pindx]);
            do_log(LOG_INFO, "Unnecessary record %s", mes);
            do_free(mes);
            result = 0;
#ifdef ARTIX_OLD_UPLOAD
        }
#endif
    }
    for (; kindx < key_count; kindx++) {
        bcode = do_text(alias, ((barcodes_ret*)do_ret_list_item_data(list, kindx))->barcode);
        if (!check_no_price(alias, (product_key4_t*)((barcodes_ret*)do_ret_list_item_data(list, kindx))->code,
                 &product, nprice)) {
            mes = product_fullname(alias, bcode, &product, nprice);
            do_log(LOG_INFO, "Record not found %s", mes);
            do_free(mes);
            result = 0;
        }
        do_free(bcode);
    }
    do_log(LOG_INFO, "%100 percent has processed");
    return result;
}
static int check_product(do_alias_t *alias, artix_alias_t *artix_alias, artix_product_t *product, product_key4_t *product_key4, product_rec_t *record, int nprice)
{
    int status = do_product_get4(alias, record, product_key4, DO_GET_EQUAL);
    if (status == DO_ERROR) return -1;
    if (status == DO_KEY_NOT_FOUND) {
        char *code;
        code = do_text(alias, product_key4->code);
        do_log(LOG_ERR, "Product with code \"%s\" not found", code);
        do_free(code);
        return -1;
    }

    artix_product_t domino_product;
    char *value = do_text(alias, record->data.code);
    artix_text_set(artix_alias, domino_product.code, value);
    do_free(value);
    value = do_text(alias, record->data.name);
    artix_text_set(artix_alias, domino_product.name, value);
    do_free(value);
    int result = 1;
#ifndef ARTIX_OLD_UPLOAD
        result = result &&     !memcmp(product->code, domino_product.code, ARTIX_FIELD_PRODUCT_CODE_SIZE) ;//&&
#endif
        result = result &&     !memcmp(product->name, domino_product.name, ARTIX_FIELD_PRODUCT_NAME_SIZE) ;//&&
        result = result &&     ((product->coef == 1) ? 1 :
               ((product->coef >= do_product_coef(alias, record) - 0.001) &&
                (product->coef <= do_product_coef(alias, record) + 0.001))) ;//&&
        result = result &&     ((do_product_price(alias, record, nprice)*product->coef >= product->price - 0.01) &&
              (do_product_price(alias, record, nprice)*product->coef <= product->price + 0.01));
    return result;
}
static int check_no_price(do_alias_t *alias, product_key4_t *product_key4, product_rec_t *product, int nprice)
{
    int status = do_product_get4(alias, product, product_key4, DO_GET_EQUAL);
    if (status == DO_ERROR) return -1;
    if (status == DO_KEY_NOT_FOUND) {
        char *code;
        code = do_text(alias, product_key4->code);
        do_log(LOG_ERR, "Product with code \"%s\" not found", code);
        do_free(code);
        return -1;
    }
    if (!product->data.base_parcel &&
            do_product_param_int(alias, product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) ==
              DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE)
	return 1;
    if (do_product_price(alias, product, nprice) != 0)
        return 0;
    return 1;
}
static char *product_fullname(do_alias_t *alias, const char* barcode, product_rec_t *product, int nprice)
{
    char *name = do_text(alias, product->data.name);
    char *code = do_text(alias, product->data.code);
    char *res = (char*) do_malloc(500);
    double coef = (!strcmp(barcode, code)) ? 1 : do_product_coef(alias, product);
    sprintf(res, "\"%s\" \"%s\" \"%s\" price %f coef %f", barcode, code,
                               name, (strcmp(code,barcode)) ?
                               coef * do_product_price(alias, product, nprice):
                               do_product_price(alias, product, nprice), coef);
    do_free(name);
    do_free(code);
    return res;
}
static char *artix_product_fullname(artix_alias_t *alias, artix_product_t *product)
{
    char *bcode = artix_text(alias, product->bcode);
    char *name = artix_text(alias, product->name);
    char *code = artix_text(alias, product->code);
    char *res = (char*) do_malloc(500);
    sprintf(res, "\"%s\" \"%s\" \"%s\" price %f coef %f", bcode, code, name, product->price, product->coef);
    do_free(name);
    do_free(code);
    do_free(bcode);
    return res;
}

