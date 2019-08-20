

#include "do_rz_ch.h"
#include "domino.h"
#include "time.h"

#define USD 28
#define EURO 38
#define DTYPE1 "05"
#define PRICE4 4
#define PRICE9 9

static gdouble trunc10(gdouble value)
{
    int i;
    i = value*100;
    if (i > value*100)
        i--;
    return i/100.0;

}
static gboolean do_rz_ch_mk_doc(do_alias_t *alias, document_key0_t *key)
{
    document_rec_t document;

    do_text_set(alias, key->dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRICE));
    do_text_set(alias, key->sklad, domino_unit());
    if (!do_util_get_empty_document_number(alias, key))
        return FALSE;

    memset(&document.data, 0, sizeof(document.data));
    document.size = sizeof(document.data) - sizeof(document.data.params);
    do_cpy(document.data.dtype, key->dtype);
    do_text_set(alias, document.data.dtype1, DTYPE1);
    do_cpy(document.data.sklad, key->sklad);
    do_cpy(document.data.document, key->document);
    do_text_set_empty(document.data.name);
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
    do_date_set(&document.data.date, tm);
    do_time_set(&document.data.time, tm);
    do_text_set(alias, document.data.p_g_code, "00");
    document.data.p_code = 1;
    do_text_set_empty(document.data.p_doc);
    do_text_set_empty(document.data.party);
    return do_document_insert(alias, &document) == DO_OK &&
           do_protocol_document_add_key(alias, key, DO_PROTOCOL_ACTION_INSERT);

}
static gboolean do_rz_ch_mk_line(do_alias_t *alias, document_order_key0_t *key, const gchar *code, double price, double quant)
{
    document_order_rec_t document_order;

    struct tm tm;
    time_t time1 = time(NULL);

    memset(&document_order.data, 0, sizeof(document_order.data));
    document_order.size = sizeof(document_order.data) - sizeof(document_order.data.params);

    do_cpy(document_order.data.dtype, key->dtype);
    do_cpy(document_order.data.sklad, key->sklad);
    do_cpy(document_order.data.document, key->document);
    key->line++;
    document_order.data.line = key->line;
    document_order.data.sort = 0;

    do_text_set(alias, document_order.data.dtype1, DTYPE1);
    tm = *localtime(&time1);
    do_date_set(&document_order.data.date, tm);
    do_time_set(&document_order.data.time, tm);

    do_text_set(alias, document_order.data.code, code);
    do_document_order_price_set(alias, &document_order, PRICE4, price);
    do_document_order_price_set(alias, &document_order, PRICE9, price);
    do_document_order_quant_set(alias, &document_order, DO_CONST_QUANT_REST, quant);

    return (do_document_order_insert(alias, &document_order) == DO_OK);
}
typedef struct PACK_STRUCT {
    BTI_CHAR dtype[2];
    BTI_CHAR sklad[10];
    BTI_CHAR document[10];
} DoRzChRet;

static gboolean do_rz_ch_get_line(do_alias_t *alias, do_ret_list_t *doc_list, const gchar *code, gdouble price)
{
    document_order_key2_t document_order_key2;
    document_order_rec_t document_order;
    DoRzChRet *ret;
    double price9;
    int i;

    for (i = do_ret_list_count(doc_list); i > 0; i--) {
        ret = do_ret_list_item_data(doc_list, i - 1);
        do_cpy(document_order_key2.dtype, ret->dtype);
        do_cpy(document_order_key2.sklad, ret->sklad);
        do_cpy(document_order_key2.document, ret->document);
        do_text_set(alias, document_order_key2.code, code);
        if (do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL) == DO_OK) {
            price9 = do_document_order_price(alias, &document_order, 9);
            if (price9 >= price - 0.00001 && price9 <= price + 0.00001)
                return TRUE;
        }
    }
    return FALSE;
}
static int do_rz_ch_reject_func(DoRzChRet *ret, BTI_SINT recLen, BTI_LONG recPos, document_key0_t *key)
{
    return do_cmp(ret->dtype, key->dtype) || do_cmp(ret->sklad, key->sklad);
}
static do_ret_list_t *do_rz_ch_get_doc_list(do_alias_t *alias)
{
    do_extended_t *extended;
    document_key0_t document_key0, key;

    do_ret_list_t *list;
    BTI_CHAR dtype1[10];

    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRICE));
    do_text_set(alias, document_key0.sklad, domino_unit());
    do_text_set_empty(document_key0.document);
    do_text_set(alias, dtype1, DTYPE1);
    do_cpy(key, document_key0);

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT);
    do_extended_set_key(extended, 0, &document_key0);
    do_extended_set_filter(extended, FALSE, 1, sizeof(dtype1));
    do_extended_set_filter_term(extended, 0, DO_EXTENDED_TYPE_STRING,
                                             offsetof(document_struct_t, dtype1),
                                             DO_EXTENDED_COMP_EQUAL,
                                             DO_EXTENDED_CONNECTION_NO,
                                             dtype1);

    do_extended_set_reject_func(extended, (do_extended_reject_func)do_rz_ch_reject_func, &key);
    do_extended_set_fields(extended, 3,
                                        do_size_and_offset(document_struct_t, dtype),
                                        do_size_and_offset(document_struct_t, sklad),
                                        do_size_and_offset(document_struct_t, document));

    list = do_extended_get(extended, -1);
    do_extended_free(extended);
    return list;
}

gboolean do_rz_ch(GtkWidget *parent)
{
    do_alias_t *alias;
    int status;
    int len;
    stock_rec_t stock;
    stock_key0_t stock_key0, key;
    product_rec_t product;
    product_key0_t product_key0;
    barcode_key1_t barcode_key1;
    do_list_t *list;
    double price_max, price;
    double quant, coef;
    rz_info_t *rz;
    char *base_code;
    char *parcel_code;
    gboolean parcel, mkdoc;
    document_key0_t document_key0;
    document_order_key0_t document_order_key0;
    do_ret_list_t *doc_list;
    int i, count;

    alias = domino_alias_new(NULL);
    if (!alias || !do_alias_open(alias, 1)) {
        if (alias)
            do_alias_free(alias);
        DOMINO_SHOW_ERROR("Невозможно открыть БД");
        return FALSE;
    }

    do_text_set(alias, stock_key0.sklad, domino_unit());
    do_text_set_empty(stock_key0.code);
    do_cpy(key.sklad, stock_key0.sklad);
    len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);
    count = 0;
    mkdoc = FALSE;

    doc_list = do_rz_ch_get_doc_list(alias);

    if (doc_list) {
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_GE);
        while (status == DO_OK) {
            if (do_cmp(key.sklad, stock_key0.sklad))
                break;
            quant = do_stock_quant(alias, &stock, DO_CONST_QUANT_REST) + do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE);
            if (quant > 0) {
                do_text_set_empty(product_key0.code);
                memcpy(product_key0.code, stock.data.code, len);
                product_key0.base_parcel = 0;
                status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
                if (status != DO_OK) {
                    DOMINO_SHOW_ERROR("Невозможно найти товар \"%s\"", do_text(alias, product_key0.code));
                    break;
                }
                if (do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS))
                     == DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE) {
                    base_code = do_text(alias, product.data.code);
                    coef = do_product_coef(alias, &product);
                    list = do_rz_info_list_from_code(alias, base_code);
                    price_max = 0;

                    for (i = 0; i < list->count; i++) {
                        rz = list->list[i];
                        switch (rz->valuta) {
                            case 2:
                                price = rz->price*USD;
                                break;
                            case 3:
                                price = rz->price*EURO;
                                break;
                            default:
                                price = rz->price;
                        }
                        if (price <= 50)
                            price = trunc10(trunc10(price*1.2*1.1) + price*0.30*1.1);
                        else
                        if (price <= 500)
                            price = trunc10(trunc10(price*1.17*1.1) + price*0.21*1.1);
                        else
                            price = trunc10(trunc10(price*1.15*1.1) + price*0.10*1.1);
                        if (!i || price_max > price)
                            price_max = price;
                    }
                    do_cpy(barcode_key1.code, stock.data.code);
                    parcel = (do_barcode_key1(alias, &barcode_key1, DO_GET_EQUAL) == DO_OK);

                    if (parcel) {
                        do_cpy(product_key0.code, stock.data.code);
                        product_key0.base_parcel = 1;
                        status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
                        if (status != DO_OK) {
                            DOMINO_SHOW_ERROR("Невозможно найти товар \"%s\"", do_text(alias, product_key0.code));
                            break;
                        }
                    }
                    price = do_product_price(alias, &product, 4)*do_product_coef(alias, &product);
                    if (price > price_max) {
                        if (!parcel) {
                            if (!do_rz_ch_get_line(alias, doc_list, base_code, price_max/coef)) {
                                if (!mkdoc) {
                                    if (!do_rz_ch_mk_doc(alias, &document_key0)) {
                                        status = DO_ERROR;
                                        break;
                                    }
                                    memcpy(&document_order_key0, &document_key0, sizeof(document_key0));
                                    document_order_key0.line = 0;
                                    mkdoc = TRUE;
                                }
                                if (!do_rz_ch_mk_line(alias, &document_order_key0, base_code, price_max/coef, 0)) {
                                    status = DO_ERROR;
                                    break;
                                }
                                count++;
                            }
                        }
                        parcel_code = do_text(alias, stock.data.code);
                        if (!do_rz_ch_get_line(alias, doc_list, parcel_code, price_max/coef)) {
                            if (!mkdoc) {
                                if (!do_rz_ch_mk_doc(alias, &document_key0)) {
                                    status = DO_ERROR;
                                    break;
                                }
                                memcpy(&document_order_key0, &document_key0, sizeof(document_key0));
                                document_order_key0.line = 0;
                                mkdoc = TRUE;
                            }
                            if (!do_rz_ch_mk_line(alias, &document_order_key0, parcel_code, price_max/coef, quant)) {
                                status = DO_ERROR;
                                break;
                            }
                            count++;
                        }
                        g_free(parcel_code);
                    }
                    do_rz_info_list_free(list);
                    g_free(base_code);
                }
            }
            status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_NEXT);
        }
        do_ret_list_free(doc_list);
    }
    else
        status = DO_ERROR;

    if (status == DO_ERROR)
        DOMINO_SHOW_ERROR("Ошибка БД")
    else
    if (mkdoc)
        DOMINO_SHOW_INFO("Создан документ \"%s\" %d строк", do_text(alias, document_key0.document), count)
    else
        DOMINO_SHOW_INFO("Товаров с неправильной ценой необнаружено");
    do_alias_free(alias);
    return TRUE;
}

gboolean do_rz_make(GtkWidget *parent, gboolean break_)
{
    do_alias_t *alias;
    int status;
    int len;
    stock_rec_t stock;
    stock_key0_t stock_key0, key;
    product_rec_t product;
    product_key0_t product_key0;
    barcode_key1_t barcode_key1;
    double quant;
    char *base_code;
    char *parcel_code;
    gboolean mkdoc;
    document_key0_t document_key0;
    document_order_key0_t document_order_key0;
    int count;

    alias = domino_alias_new(NULL);
    if (!alias || !do_alias_open(alias, 1)) {
        if (alias)
            do_alias_free(alias);
        DOMINO_SHOW_ERROR("Невозможно открыть БД");
        return FALSE;
    }

    do_text_set(alias, stock_key0.sklad, domino_unit());
    do_text_set_empty(stock_key0.code);
    do_cpy(key.sklad, stock_key0.sklad);
    len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);
    count = 0;
    mkdoc = FALSE;

    status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_GE);
    while (status == DO_OK) {
        if (do_cmp(key.sklad, stock_key0.sklad))
            break;
        quant = do_stock_quant(alias, &stock, DO_CONST_QUANT_REST) +
                do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE);
        if (quant > 0) {
            do_text_set_empty(product_key0.code);
            memcpy(product_key0.code, stock.data.code, len);
            product_key0.base_parcel = 0;
            status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
            if (status != DO_OK) {
                DOMINO_SHOW_ERROR("Невозможно найти товар \"%s\"", do_text(alias, product_key0.code));
                break;
            }
            if (do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS))
                 == DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE) {

                base_code = do_text(alias, product.data.code);
                do_cpy(barcode_key1.code, stock.data.code);
                if (do_barcode_key1(alias, &barcode_key1, DO_GET_EQUAL) == DO_KEY_NOT_FOUND) {
                    if (!mkdoc) {
                        if (!do_rz_ch_mk_doc(alias, &document_key0)) {
                            status = DO_ERROR;
                            break;
                        }
                        memcpy(&document_order_key0, &document_key0, sizeof(document_key0));
                        document_order_key0.line = 0;
                        mkdoc = TRUE;
                    }
                    parcel_code = do_text(alias, stock.data.code);
                    do_cpy(product_key0.code, stock.data.code);
                    product_key0.base_parcel = 1;
                    status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
                    if (status != DO_OK) {
                        DOMINO_SHOW_ERROR("Невозможно найти товар \"%s\"", do_text(alias, product_key0.code));
                        break;
                    }
                    if (!do_rz_ch_mk_line(alias, &document_order_key0, parcel_code, do_product_price(alias, &product, 4), quant)) {
                        status = DO_ERROR;
                        break;
                    };
                    count++;
                    g_free(parcel_code);
                }
                g_free(base_code);
            }
        }
        if (count >= 30 && break_)
            break;
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_NEXT);
    }
    if (status == DO_ERROR)
        DOMINO_SHOW_ERROR("Ошибка БД")
    else
    if (mkdoc)
        DOMINO_SHOW_INFO("Создан документ \"%s\" %d строк", do_text(alias, document_key0.document), count)
    else
        DOMINO_SHOW_INFO("Товары без штрихкодов необнаружены");
    do_alias_free(alias);
    return TRUE;
}

gboolean do_rz_old(GtkWidget *parent)
{
    do_alias_t *alias;
    int status;
    int len;
    stock_rec_t stock;
    stock_key0_t stock_key0, key;
    product_rec_t product;
    product_key0_t product_key0;
    barcode_key1_t barcode_key1;
    barcode_rec_t barcode;
    double quant;
    char *base_code;
    char *parcel_code;
    gboolean mkdoc;
    document_key0_t document_key0;
    document_order_key0_t document_order_key0;
    int count;

    alias = domino_alias_new(NULL);
    if (!alias || !do_alias_open(alias, 1)) {
        if (alias)
            do_alias_free(alias);
        DOMINO_SHOW_ERROR("Невозможно открыть БД");
        return FALSE;
    }

    do_text_set(alias, stock_key0.sklad, domino_unit());
    do_text_set_empty(stock_key0.code);
    do_cpy(key.sklad, stock_key0.sklad);
    len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);
    count = 0;
    mkdoc = FALSE;

    status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_GE);
    while (status == DO_OK) {
        if (do_cmp(key.sklad, stock_key0.sklad))
            break;
        quant = do_stock_quant(alias, &stock, DO_CONST_QUANT_REST) +
                do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE);
        if (quant > 0) {
            do_text_set_empty(product_key0.code);
            memcpy(product_key0.code, stock.data.code, len);
            product_key0.base_parcel = 0;
            status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
            if (status != DO_OK) {
                DOMINO_SHOW_ERROR("Невозможно найти товар \"%s\"", do_text(alias, product_key0.code));
                break;
            }
            if (do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS))
                 == DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE) {

                base_code = do_text(alias, product.data.code);
                do_cpy(barcode_key1.code, stock.data.code);
                if (do_barcode_get1(alias, &barcode, &barcode_key1, DO_GET_EQUAL) == DO_OK &&
                    do_barcode_sk(alias, &barcode) < 0) {
                    if (!mkdoc) {
                        if (!do_rz_ch_mk_doc(alias, &document_key0)) {
                            status = DO_ERROR;
                            break;
                        }
                        memcpy(&document_order_key0, &document_key0, sizeof(document_key0));
                        document_order_key0.line = 0;
                        mkdoc = TRUE;
                    }
                    parcel_code = do_text(alias, stock.data.code);
                    do_cpy(product_key0.code, stock.data.code);
                    product_key0.base_parcel = 1;
                    status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
                    if (status != DO_OK) {
                        DOMINO_SHOW_ERROR("Невозможно найти товар \"%s\"", do_text(alias, product_key0.code));
                        break;
                    }
                    if (!do_rz_ch_mk_line(alias, &document_order_key0, parcel_code, do_product_price(alias, &product, 4), quant)) {
                        status = DO_ERROR;
                        break;
                    };
                    count++;
                    g_free(parcel_code);
                }
                g_free(base_code);
            }
        }
//        if (count >= 30 && break_)
  //          break;
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_NEXT);
    }
    if (status == DO_ERROR)
        DOMINO_SHOW_ERROR("Ошибка БД")
    else
    if (mkdoc)
        DOMINO_SHOW_INFO("Создан документ \"%s\" %d строк", do_text(alias, document_key0.document), count)
    else
        DOMINO_SHOW_INFO("Товары без штрихкодов необнаружены");
    do_alias_free(alias);
    return TRUE;
}

