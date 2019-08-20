#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "grlspl.h"
#include <domino.h>
#include <locale.h>
#include "version.h"

char *alias_name;
char *xls_file;
char *config_file;
char *log_file = NULL;
int   no_send_message = FALSE;

void print_usage(int argc, char *argv[])
{
   printf("usage: %s [options] <alias_name> <config_file> <xls_file> \n"
         "  Options are as follows:\n"
         "  -o, --log-file            log file. (default) syslog\n"
         "  -V, --version             display version info\n"
         "  -?, --help                display this help\n"
         "  --no-send-message         no send message\n"
         "\n"
         "Copyright (C) 2008-2010 Youra Richesse\n"
         "grlspl is free software and comes with ABSOLUTELY NO WARRANTY\n"
         "under the terms of the GNU General Public License\n"
         "\n"
         "Report bugs to Support Center:\n"
         "  mir@glekar.ru\n",
         argv[0]);
    exit(1);
}

static char *const shortoptions = "o:?V";
enum {
   OPT_NOARG,
   OPT_HELP,
   OPT_VERSION,
   OPT_LOG_FILE,
   OPT_NO_SEND_MESSAGE,
};

static const struct option longoptions[] = {
   {"help",                no_argument,       NULL, OPT_HELP},
   {"version",             no_argument,       NULL, OPT_VERSION},
   {"log-file",            required_argument, NULL, OPT_LOG_FILE},
   {"no-send-message",     no_argument      , NULL, OPT_NO_SEND_MESSAGE},
   {0,                     no_argument,       NULL, OPT_NOARG}
};

void option_parse_options(int argc, char *argv[])
{
    int options = 0;
    int option_index;
    int errflag = 0;
    int c;

    while ( !errflag &&
           ( c = getopt_long(argc, argv, shortoptions,
             longoptions, &option_index) ) != -1) {
        options++;
        switch ( c ) {
            case OPT_NO_SEND_MESSAGE:
                no_send_message = TRUE;
                break;
            case 'o':
            case OPT_LOG_FILE:
                if ( optarg[0] == '-' ) {
                    errflag++;
                    break;
                }
                options++;
                log_file = do_strdup(optarg);
                break;
            case 'V':
            case OPT_VERSION:
                printf("grlspl " GRLSPL_RELEASE " (" ADATE ") \n");
                exit(0);
            case '?':
            case OPT_HELP:
            default:
                print_usage(argc, argv);
                exit(1);
        }
    }
    if ( options + 4 != argc )
        print_usage(argc, argv);
    alias_name = do_strdup(argv[options+1]);
    config_file = do_strdup(argv[options+2]);
    xls_file = do_strdup(argv[options+3]);
}

int main(int argc, char *argv[])
{
    option_parse_options(argc, argv);
    int show_info = FALSE;

    if ( log_file ) {
        if ( !strcmp(log_file, "stdout") ) {
            do_log_set_stdout();
            show_info = TRUE;
        }
        else
            do_log_set_type("grlspl", DO_LOG_TYPE_FILE, log_file);
    }
    setlocale(LC_ALL, "");

    if ( !domino_init(NULL, TRUE, DO_PARAM_END) )
        exit(1);

    if ( !grlspl_load_from_xls(alias_name, xls_file, config_file,
                         TRUE, TRUE, show_info, !no_send_message) )
        exit(1);

    return 0;
}
