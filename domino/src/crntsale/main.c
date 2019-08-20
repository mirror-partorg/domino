#include <dolib.h>
#include <domino.h>
#include "option.h"
#include "../../config/config.h"
#include "version.h"
#include "crntsale.h"
#include <string.h>
#include <locale.h>

static option_t *opt = NULL;
const char *crntsale_name = "crntsale";

static int init()
{
    setlocale(LC_ALL, "");
    if (!option_read_config(opt))
        exit(1);
    do_log_set_type(crntsale_name, opt->log_type, opt->log_file);

    if  (!domino_init(opt->domino_config_file, FALSE, DO_PARAM_END))
        exit(1);
    return 1;
}

int main(int argc, char *argv[])
{
    opt = option_new();
    if (option_parse_options(opt, argc, argv))
       exit(1);
    if (opt->show_version) {
        printf("crntsale " CRNTSALE_TEXT_RELEASE " (" ADATE ") \n");
        exit(0);
    }
    init();
    do_log_set_stdout();

    if (!mk_crntsale(opt))
        exit(1);
    return 0;
}
