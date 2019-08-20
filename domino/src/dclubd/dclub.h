#ifndef _DCLUB_H
#define _DCLUB_H

#include <dolib.h>
#include "../misc/mask.h"
#include <domino.h>
#include "dclubinfo.h"
#include "option.h"
#include <iconv.h>

#define NAME "dclubd"

#define DB_FOR_DCLUB DO_DB_DOCUMENT, DO_DB_DOCUMENT_DATA, DO_DB_DOCUMENT_ORDER, DO_DB_SKLAD, DO_DB_PARTNER, DO_DB_DISCOUNT, DO_DB_BARCODE, DO_DB_PRODUCT, DO_DB_PROTOCOL, DO_DB_END

 typedef struct{
    int position;      //- check's position
    int code;          //- product's code
    char *barcode;  //- product's barcode
    char *name;     //- product's name
    double bquant;     //- quantity
    double price;      //- source product's price
    double price_i;    //- product's price
    double sum_i;      // -sum
    int price_type;    //- price type
    int price_mode;    //- price mode
    char *cdept;    //- depatment's code
    double price_new;  //
    double sum_new;    //
    double price_new_min; //
    double sum_new_min; //
    int coef; // for artix
    int action;        //- flag
    char *message;  //  end;
    char *message_min;  //  end;
    double price_retail;
    double price_purchase;
//    double price_after_discount;
    double quant_discount;
    product_key4_t product_key4;
    int unpack;
 } rec_t;

 typedef struct{
    int request;
    int cash;
    int shift;
    int scode;
    struct tm time;
    int check;
    int64_t cardno;
    int one_off;
    char *cardname;
    time_t time_block;
    time_t time_unblock;
    int error;
    double sum;
    do_list_t *rec_list;
//    char card_type[];
 } trans_t;

#define MAX_VARIABLE 10

typedef struct {
    document_key0_t key;
    char *description;
    int type_kind;
    double *scale;
    double *perc;
    int limit_product;
    int kind_limit;
    int loss;
    int one_off;
} discount_t;

typedef struct {
    option_t *opt;
    do_string_list_t *lasttrans;
    do_list_t *discount_list;
    mask_t   *mask_in;
    mask_t   *mask_out;
    dclub_info_t *dclub_info;
    trans_t *trans;
    time_t time_trans;
    do_alias_t *alias;
    iconv_t tdclub_conv;
    iconv_t msg_conv;
    iconv_t msg_conv_inv;
    iconv_t utf8_conv;
    iconv_t fdclub_conv;
} dclub_t;


#ifdef __cplusplus
extern "C"
{
#endif

dclub_t *dclub_new(option_t *opt, dclub_info_t *dclub_info, do_alias_t* alias);
void dclub_free(dclub_t *dclub);
void dclub_run(dclub_t *dclub);
void dclub_check_config(option_t *opt);

trans_t *trans_new();
void     trans_free(trans_t *trans);

int discount_calc(do_alias_t *alias, trans_t *trans, discount_t *discount, const char* unit, do_string_list_t *log, int inproc, char **error, iconv_t utf8_conv);
int discount_list(do_alias_t *alias, trans_t *trans, do_list_t *list, const char* unit, do_string_list_t *log, int inproc);
char* discount_get_card_name(do_alias_t *alias, const int cardno);
void discount_do_list_clear(do_list_t *list);

rec_t *rec_new();
void rec_free(rec_t *rec);

#endif
