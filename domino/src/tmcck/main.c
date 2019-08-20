#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <artix.h>
#include <domino.h>
#include "artix_util.h"

#include <time.h>
#include <locale.h>

void print_usage()
{
    printf("usage: tmcch "
#ifdef LOCAL_CASH
	   "[<domino-config>]"
#else
	   "[<cash-number> [<domino-config>]]"
#endif
         "\n"
         "Copyright (C) 2008-2009 Youra Richesse\n"
         "dclubd is free software and comes with ABSOLUTELY NO WARRANTY\n"
         "under the terms of the GNU General Public License\n"
         "\n"
         "Report bugs to tmcch Support Center:\n"
         "  mir@glekar.ru\n");
    exit(0);
}

int main(int argc, char *argv[])
{

#ifdef LOCAL_CASH
    if (argc > 2)
#else
    if (argc > 3)
#endif
        print_usage();
    char *config = NULL;
    int cash_code = 0;
    if (argc > 1) {
	if (!strcmp(argv[1], "--help"))
	    print_usage();
	cash_code = atoi(argv[1]);
        if (argc > 2)
        config = argv[3];
    }
    do_log_set_stdout();
    if (!domino_init(config, FALSE, DO_PARAM_END))
        exit(1);
    setlocale(LC_ALL,"");
    do_log(LOG_INFO, "Open domino database");
    do_alias_t *alias;
    if ((alias = domino_alias_new(NULL)) == NULL)
        exit(1);
    if (!do_alias_open(alias, 0, DO_DB_BARCODE, DO_DB_PARTNER, DO_DB_PRODUCT, DO_DB_END))
        exit(1);
    char *address;
#ifndef LOCAL_CASH
    partner_rec_t partner;
    partner_key0_t partner_key0, partner_key;
    do_text_set(alias, partner_key0.g_code, do_param(DO_PARAM_PARTNER_REGION_CASH));
    memcpy(&partner_key, &partner_key0, sizeof(partner_key0.g_code));
    partner_key0.code = cash_code;
    int status;
    if (!cash_code)
         status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_GE);
    else {
         status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
         if (status == DO_KEY_NOT_FOUND) {
             do_log(LOG_ERR, "Cash %d not found", cash_code);
             exit(1);
         }
    }
    char *unit;
    while (status == DO_OK) {
        if (memcmp(&partner_key, &partner_key0, sizeof(partner_key0.g_code))) break;
        unit = do_text(alias, partner.data.sklad);
        if (!strcmp(unit, domino_unit())) {
            address = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_CASH_IP));
            if (*address != '\0') {
                do_log(LOG_INFO, "Open cash %d database", partner.data.code);
                artix_t *artix = artix_new_default(address, FALSE);
#else
                do_log(LOG_INFO, "Open cash database");
		address = do_strdup("localhost");
                artix_t *artix = artix_new_default(address, FALSE);
#endif
                if (!artix)
                    exit(1);
                if (!artix_check_products(artix, alias, 4))
                    printf("Error\n");
                else
                    printf("OK\n");
                artix_free(artix);
#ifndef LOCAL_CASH
            }
            do_free(address);
        }
        do_free(unit);
        if (cash_code) break;
        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_NEXT);
    }
#endif
    return 0;
}
