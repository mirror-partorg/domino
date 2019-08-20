#ifndef REPLIC_H_INCLUDED
#define REPLIC_H_INCLUDED

#include <domino.h>
#include "../misc/mproc_state.h"
#define DOCUMENT_CONTEXT_COUNT 8
#define DOCUMENT_DB DO_DB_DOCUMENT, DO_DB_DOCUMENT_VIEW, DO_DB_DOCUMENT_DATA, \
                                           DO_DB_DOCUMENT_ORDER, DO_DB_DOCUMENT_LINK, \
                                           DO_DB_DOCUMENT_TYPE, DO_DB_PROTOCOL, \
                                           DO_DB_ACCOUNT,DO_DB_SUBACCOUNT, \
                                           DO_DB_SALDO, \
                                           DO_DB_PRODUCT, \
                                           DO_DB_PRODUCT_DATA, \
                                           DO_DB_PROWOD, DO_DB_PROWOD2, DO_DB_END

#ifdef __cplusplus
extern "C"
{
#endif

int replic_document(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);
int replic_document_period(do_alias_t *alias,
                     char *dtype, char *sklad, struct tm *date_start, struct tm *date_end, int all,
                     void *data, size_t size,
                     do_extended_break_func break_func);
int replic_checks(do_alias_t *alias,
                     char *sklad, struct tm *date_start, struct tm *date_end,
                     void *data, size_t size,
                     do_extended_break_func break_func);
int replic_realization(do_alias_t *alias,
                     char *sklad, struct tm *date_start, struct tm *date_end,
                     void *data, size_t size,
                     do_extended_break_func break_func);
int add_protocol(do_alias_t *alias,
                     struct tm *date_start, struct tm *date_end,
                     void *data, size_t size,
                     do_extended_break_func break_func);

int replic_product(do_alias_t *alias, const char *code, void *data, size_t size, do_extended_break_func break_func);
int replic_products(do_alias_t *alias, void *data, size_t size, const char *sklad, int base_parcel, do_extended_break_func break_func);
int replic_products_view(do_alias_t *alias, void *data, size_t size, const char *sklad, int base_parcel, do_extended_break_func break_func);
int replic_products_data(do_alias_t *alias, void *data, size_t size, const char *sklad, int base_parcel, do_extended_break_func break_func);
int replic_barcodes(do_alias_t *alias, void *data, size_t size, const char *sklad, int base_parcel, do_extended_break_func break_func);
int replic_partner(do_alias_t *alias, const char *g_code, int code, void *data, size_t size, do_extended_break_func break_func);
int replic_document_order(do_alias_t *alias, const char *dtype, const char *sklad, const char *document, int line, void *data, size_t size, int *changed, do_extended_break_func break_func);
int replic_regions(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);
int replic_partners(do_alias_t *alias, void *data, size_t size, const char *g_code, do_extended_break_func break_func);
int replic_partners_view(do_alias_t *alias, void *data, size_t size, const char *g_code, do_extended_break_func break_func);
int replic_partners_data(do_alias_t *alias, void *data, size_t size, const char *g_code, do_extended_break_func break_func);
int replic_class(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);
int replic_groups(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);
int replic_subgroups(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);
int replic_operations(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);
int replic_users(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);
int replic_sklads(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);
int replic_stocks(do_alias_t *alias, const char *sklad, void *data, size_t size, do_extended_break_func break_func);
int replic_stock(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);
int replic_stock_list(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);
int replic_updated_stocks(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);
int replic_otdels(do_alias_t *alias, void *data, size_t size, do_extended_break_func break_func);

int replic_get_product(do_alias_t *alias, const char *code, do_data_t *out, do_extended_break_func break_func);
int replic_get_partner(do_alias_t *alias, const char *g_code, int code, do_data_t *out, do_extended_break_func break_func);
int replic_get_document_order(do_alias_t *alias, const char *dtype, const char *sklad, const char *document, int line, do_data_t *out, do_extended_break_func break_func);
int replic_get_stock(do_alias_t *alias, const char *store, const char *code, do_data_t *out, do_extended_break_func break_func);
int replic_get_stocks(do_alias_t *alias, do_list_t *list, do_data_t *out, do_extended_break_func break_func);

do_ret_list_t *replic_select_product_base_parcel(do_alias_t *alias,  const char *sklad, int base_parcel, do_extended_break_func break_func);
do_ret_list_t *replic_select_product_view_base_parcel(do_alias_t *alias, const char *sklad, int base_parcel, do_extended_break_func break_func);
do_ret_list_t *replic_select_product_data_base_parcel(do_alias_t *alias, const char *sklad, int base_parcel, do_extended_break_func break_func);
do_ret_list_t *replic_select_barcode_base_parcel(do_alias_t *alias, const char *sklad, int base_parcel, do_extended_break_func break_func);
do_ret_list_t *replic_select_shifts(do_alias_t *alias, const char *sklad, int date_start, int date_end, do_extended_break_func break_func);
do_ret_list_t *replic_select_shift_check(do_alias_t *alias, shift_struct_t *shift_st, do_extended_break_func break_func);
do_ret_list_t *replic_select_shift_checksum(do_alias_t *alias, shift_struct_t *shift_st, do_extended_break_func break_func);

do_ret_list_t *replic_select_document_order(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func);
do_ret_list_t *replic_select_document_parcel(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func);
do_ret_list_t *replic_select_document_data(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func);
do_ret_list_t *replic_select_document_view(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func);
do_ret_list_t *replic_select_document_link(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func);
do_ret_list_t *replic_select_document_prowod(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func);
do_ret_list_t *replic_select_document_prowod2(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func);
do_ret_list_t *replic_select_stocks(do_alias_t *alias, const char *sklad, do_extended_break_func break_func);
do_ret_list_t *replic_select_document_protocol(do_alias_t *alias, document_key0_t *document_key0, do_extended_break_func break_func);

//!!int replic_select_stock(do_alias_t *alias, stock_key0_t *key, do_list_t *retval, do_extended_break_func break_func);


do_ret_list_t *replic_select_realization(do_alias_t *alias, const char *sklad, int date_start, int date_end, do_extended_break_func break_func);

int replic_protocol_write_change_stock(do_alias_t *alias, const char *code, const char *store);
int replic_protocol_write_change_document(do_alias_t *alias, const char *dtype, const char *store, const char *document, const char *action, int handbook);

int replic_get_last_index(do_alias_t *alias);
int replic_set_last_index(do_alias_t *alias, int index);

int get_balance_41(do_alias_t *alias, const char *store, struct tm date, double *balance, int *count, do_extended_break_func break_func);

#ifdef __cplusplus
}
#endif


#endif // REPLIC_H_INCLUDED
