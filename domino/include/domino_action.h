#ifndef DOMINO_ACTION_H_INCLUDED
#define DOMINO_ACTION_H_INCLUDED

#include <domino.h>

#define DOMINO_ORDER_DBS DO_DB_DOCUMENT_ORDER, DO_DB_DOCUMENT, DO_DB_PROTOCOL, DO_DB_PRODUCT, DO_DB_END


typedef struct {
    char *code;
    domino_marked_t marked;
    struct tm tm;
    struct tm last_tm;
    double quant;
} domino_order_marked_list_item_t;



#ifdef __cplusplus
extern "C"
{
#endif

int domino_order_get_marked(do_alias_t *alias, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t *marked, struct tm *time_mark);
int domino_order_get_marked_quant(do_alias_t *alias, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t *marked, struct tm *time_mark, double *quant);
int domino_order_get_marked_(const char *alias_name, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t *marked, struct tm *time_mark);
int domino_order_get_marked_quant_(const char *alias_name, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t *marked, struct tm *time_mark, double *quant);
int domino_order_set_marked(do_alias_t *alias, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t marked);
int domino_order_set_marked_quant(do_alias_t *alias, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t marked, double quant);
int domino_order_set_marked_full(do_alias_t *alias, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t marked, double quant, int partner, const char *desc, const char *life_time, const char *manufactor, double price, double price_fact);
int domino_order_set_marked_(const char *alias_name, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t marked);
int domino_order_set_marked_quant_(const char *alias_name, const char *unit, const char *product_code, struct tm *tm1, struct tm *tm2, domino_marked_t marked, double quant);
int domino_order_after_traffic_in(do_alias_t *alias, const char *unit, const char *product_code, struct tm *tm1);
int domino_order_after_traffic_in_(const char *alias_name, const char *unit, const char *product_code, struct tm *tm1);
do_list_t *domino_order_get_marked_list(do_alias_t *alias, const char *unit, struct tm *tm1, struct tm *tm2, int uniq);
do_list_t *domino_order_get_marked_list_(const char *alias_name, const char *unit, struct tm *tm1, struct tm *tm2, int uniq);
void domino_order_marked_list_free(do_list_t *list);
int domino_set_recommend(do_alias_t *alias, const char *product_code);



#ifdef __cplusplus
}
#endif

#endif // DOMINO_ACTION_H_INCLUDED
