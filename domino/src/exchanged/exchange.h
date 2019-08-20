#ifndef _EXCHANGE_H
#define _EXCHANGE_H

#include <domino.h>
#include <artix.h>
#include <dolib.h>
#include "exchangeinfo.h"
#include "option.h"

#define NAME "exchanged"

#define DB_FOR_EXCHANGE DO_DB_PARTNER, DO_DB_BARCODE, DO_DB_PRODUCT, DO_DB_SKLAD, DO_DB_PROTOCOL, DO_DB_DOCUMENT, DO_DB_DOCUMENT_ORDER, DO_DB_END

typedef struct {
    option_t        *opt;
    do_string_list_t   *changed;
    exchange_info_t *exchange_info;
    time_t           last_full_update;
    time_t           last_product_update;
    int              crnt_state;
    do_alias_t      *alias;
    artix_t         *artix;
} exchange_t;


#ifdef __cplusplus
extern "C"
{
#endif

exchange_t *exchange_new(option_t *opt, exchange_info_t *exchange_info, do_alias_t* alias);
void exchange_free(exchange_t *exchange);
void exchange_run(exchange_t *exchange, int full_update);
void exchange_send_error(exchange_t *exchange, char *error);
void exchange_cache_clear(exchange_t *exchange);
void exchange_set_proc_state(do_proc_state_t *p);
void error_message(artix_info_t *info, const char *error);


#endif
