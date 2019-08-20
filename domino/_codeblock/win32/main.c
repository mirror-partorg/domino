
#include <artix.h>
#include <domino.h>
#include "../../config/config.h"
#include <locale.h>

int main ()
{
    do_log_set_stdout();
    setlocale(LC_ALL, "");

    artix_alias_t *artix;
    artix_select_t *select;
    do_meta_alias_t *meta;
    int i, status;


    meta = domino_meta_alias_new();

    if (!domino_init(NULL, FALSE, DO_PARAM_END))
        exit(1);

    struct tm date_b;
    struct tm date_e;
    double sum, quant;
    do_atodate("11/06/2010", &date_b);
    do_atodate("30/11/2010", &date_e);
    do_report_product_meta_traffic(meta, "07141",
                              do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT),
                              do_string_list_new_from_csv("01,02,03,04,55"),
                              //do_string_list_new_from_csv("01"),
                              "5",
                              &date_b, &date_e,
                              1, 9,
                              &quant, &sum);

    domino_meta_alias_free(meta);
    return 0;
}
