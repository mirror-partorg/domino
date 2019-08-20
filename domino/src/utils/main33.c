#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <domino.h>
#include <../config/config.h>
#include <glib.h>


int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();

    if ( !domino_init(NULL, FALSE, DO_PARAM_END) )
        exit(1);

    do_alias_t *alias;
    alias = domino_alias_new("local.tta");
    do_alias_open(alias, TRUE);

    partner_rec_t partner;
    partner_key0_t partner_key0;

    do_text_set(alias, partner_key0.g_code, "03");
    partner_key0.code = 74;

    do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);

    FILE *f;

    f = fopen("sad", "w+");

    fwrite(partner.data.params, 1, partner.size - (sizeof(partner.data) - sizeof(partner.data.params)), f);
    fclose(f);



}
