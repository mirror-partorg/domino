#include <stdio.h>
#include <stdlib.h>
#include <artix.h>
#include <dirent.h>
#include "../config/config.h"

int main()
{
    artix_t *artix;
    artix_sale_t *ss = NULL;
    artix_shift_t shift;
    int count;

    struct direct *direc nt;


    artix = artix_new("kassa-16", ARTIX_DEFAULT_CONNECTION_STRING,
                       ARTIX_DEFAULT_INFO_SERVICE_PORT,
                       ARTIX_DEFAULT_USER, ARTIX_DEFAULT_PASSWORD,
                       TRUE);
    sprintf(shift.cash, "%s", "16");
    sprintf(shift.shop, "%s" ,"5");
    shift.shift = 1092;
    count = 0;
    int result;
    result = artix_alias_get_storno(artix->alias,&shift,0,0,&count, &ss);
    if ( count && result )
        return 0;

    return -1;
}
