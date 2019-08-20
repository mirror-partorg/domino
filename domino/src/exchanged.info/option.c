#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "option.h"
#include "../../config/config.h"
#include <domino.h>
#include <dolib.h>
#include "../misc/csv_util.h"
//#include "../misc/artix_send.h"


#define HANDBOOK_FULL_UPDATE_TIMEOUT 4*3600

option_t *option_new()
{
    option_t *result = (option_t *) do_malloc ( sizeof(option_t));
    result->config_file = DOMINO_CONFIG(EXCHANGE_CONFIG_FILE);
    result->user = do_strdup(DOMINO_USERNAME);
    result->debug_level = DEFAULT_DEBUG_LEVEL;
    result->log_type = DEFAULT_LOG_TYPE;
    result->domino_config_file = NULL;
    result->log_file = DOMINO_CONFIG(EXCHANGE_LOG_FILE);
    result->show_version = 0;
    result->run_foreground = 0;
  //  result->load_only = FALSE;
//    result->load_objects = 0;
    result->handbook_full_update_timeout = HANDBOOK_FULL_UPDATE_TIMEOUT;
    result->sleep_msec=0;
    result->pid_file = NULL;
    result->artix_host = do_strdup("localhost");
    result->artix_port = ARTIX_DEFAULT_INFO_SERVICE_PORT;
    result->artix_cs = do_strdup(ARTIX_DEFAULT_CONNECTION_STRING);
    result->artix_user = do_strdup(ARTIX_DEFAULT_USER);
    result->artix_password = do_strdup(ARTIX_DEFAULT_PASSWORD);

    result->tcp_port = EXCHANGE_DEFAULT_TCP_PORT;
    result->telnet_tcp_port = EXCHANGE_DEFAULT_TELNET_TCP_PORT;
#ifdef _WIN32
    result->service_install = 0;
#endif

    result->nretail_price = do_param_int(DO_PARAM_PRICE_RETAIL_DEFAULT);
    return result;
}
void option_free(option_t *opt)
{
    if (opt->config_file)
        do_free(opt->config_file);
    if (opt->domino_config_file)
        do_free(opt->domino_config_file);
    if (opt->user)
        do_free(opt->user);
    if (opt->log_file)
        do_free(opt->log_file);
    if (opt->artix_host)
        do_free(opt->artix_host);
    if (opt->artix_password)
        do_free(opt->artix_password);
    if (opt->artix_user)
        do_free(opt->artix_user);
    if (opt->artix_cs)
        do_free(opt->artix_cs);
    do_free(opt);
}
static char *const shortoptions = "f:d:l:?VbFIU";
enum {
   OPT_NOARG,
   OPT_HELP,
   OPT_VERSION,
 //  OPT_LOAD_ONLY,
   OPT_CFGFILE,
   OPT_DEBUG,
   OPT_CHECK,
   OPT_CREATE,
   OPT_PIDFILE,
#ifdef _WIN32
   OPT_INSTALL,
   OPT_UNINSTALL,
#endif
   OPT_RUNFOREGROUND,
};

static const struct option longoptions[] = {
   {"help",                no_argument,       NULL, OPT_HELP},
   {"version",             no_argument,       NULL, OPT_VERSION},
 //  {"load",           required_argument, NULL, OPT_LOAD_ONLY},
   {"config-file",         required_argument, NULL, OPT_CFGFILE},
   {"debug",               required_argument, NULL, OPT_DEBUG},
   {"writepid",            required_argument, NULL, OPT_PIDFILE},
#ifdef _WIN32
   {"install",             no_argument      , NULL, OPT_INSTALL},
   {"uninstall",           no_argument      , NULL, OPT_UNINSTALL},
#endif
   {"run-foreground",      no_argument,       NULL, OPT_RUNFOREGROUND},
   {0,                     no_argument,       NULL, OPT_NOARG}
};
void print_usage()
{
   printf("usage: exchanged [options]\n"
         "  Options are as follows:\n"
         "  -b, -F, --run-foreground  don't go into background(foreground)\n"
         "  -d, --debug <level>       set debug level (>0)\n"
         "  -f, --config-file <file>  load specified config file\n"
         "  -V, --version             display version info\n"
//         "  -l, --load [[keyboard|product|cashier...]|abort] \n"
         "  -?, --help                display this help\n"
         "  --writepid <pid_file>     write pid file\n"
#ifdef _WIN32
         "  -I, --install             install service\n"
         "  -U, --uninstall           uninstall service\n"
#endif
         "\n"
         "Copyright (C) 2008-2009 Youra Richesse\n"
         "exchanged is free software and comes with ABSOLUTELY NO WARRANTY\n"
         "under the terms of the GNU General Public License\n"
         "\n"
         "Report bugs to exchanged Support Center:\n"
         "  mir@glekar.ru\n");
}
#define DELSPACE while (cp[0] == ' ') ++cp
#define ISPACE(c) (c == ' ')

int option_read_config(option_t *opt)
{
    FILE *fp;
    if ((fp = fopen(opt->config_file, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening configuration file (%s): %s\n", opt->config_file, strerror(errno));
        return 0;
    }
    size_t len = BUFSIZ;
    char* line = (char*) do_malloc(len + 1);
    while (!feof(fp))  {
        char* cp,*element,*value;
        if (fgets(line, len+1, fp) == NULL) break;
        while ((strlen(line) == len) && !feof(fp)) {
            line = (char *)do_realloc(line, len * 2 + 1);
            if (!fgets(line+len, len+1, fp)) {
        	do_free(line);
	        do_log(LOG_ERR, "Error reading configuration file (%s): %s\n", opt->config_file, strerror(errno));
	        return 0;
            }
            len *= 2;
        }
        cp = line;
	CSV_DELSPACE(cp);
        if (CSV_COMMENT(cp))
            continue;

        element = cp;
        while (!CSV_SPACE(cp) && !CSV_EOS(cp))
           ++cp;
        if (CSV_EOS(cp)) continue;
	    *cp = '\0';
	cp++;
	CSV_DELSPACE(cp);
        value = cp;
        while (!CSV_EOS(cp)) ++cp;
        --cp;
        while (cp != value && CSV_SPACE(cp)) --cp;
        *(cp + 1) = '\0';

        if (!strcmp(element,"log-file")) {
            if (opt->log_file) do_free(opt->log_file);
            opt->log_file = do_strdup(value);
        }
        else
        if (!strcmp(element,"log")) {
            if (!strcmp(value, "syslog"))
                opt->log_type = DO_LOG_TYPE_SYSLOG;
            else
            if (!strcmp(value, "file"))
                opt->log_type = DO_LOG_TYPE_FILE;
            else
                opt->log_type = DO_LOG_TYPE_STDOUT;
        }
        if (!strcmp(element,"domino-config-file")){
            if (opt->domino_config_file) do_free(opt->domino_config_file);
            opt->domino_config_file = do_strdup(value);
        }
        if (!strcmp(element,"debug-level"))
            opt->debug_level = atoi(value);

        if (!strcmp(element,"sleep-msecond"))
            opt->sleep_msec = atoi(value);

        if (!strcmp(element,"artix-host")){
            if (opt->artix_host) do_free(opt->artix_host);
            opt->artix_host = do_strdup(value);
        }
        else
        if (!strcmp(element,"artix-port")){
            opt->artix_port = atoi(value);
        }
        else
        if (!strcmp(element,"artix-connection-string")) {
            if (opt->artix_cs) do_free(opt->artix_cs);
            opt->artix_cs = do_strdup(value);
        }
        else
        if (!strcmp(element,"artix-user")){
            if (opt->artix_user) do_free(opt->artix_user);
            opt->artix_user = do_strdup(value);
        }
        else
        if (!strcmp(element,"artix-password")){
            if (opt->artix_password) do_free(opt->artix_password);
            opt->artix_password = do_strdup(value);
        }
        else
        if (!strcmp(element,"handbook-full-update-timeout")){
            opt->handbook_full_update_timeout = atoi(value);
        }
        else
        if (!strcmp(element,"user")){
            if (opt->user) do_free(opt->user);
            opt->user = do_strdup(value);
        }
        else
        if (!strcmp(element,"tcp-port"))
            opt->tcp_port = atoi(value);
        if (!strcmp(element,"telnet-port"))
            opt->telnet_tcp_port = atoi(value);
    }
    do_free(line);
    fclose(fp);
    return 1;
}

int option_parse_options(option_t *opt, int argc, char *argv[])
{
   int options = 0;
   int oneshot = 0;
   int option_index;
   int errflag = 0;
   int c;

   while (!errflag &&
          (c = getopt_long(argc, argv, shortoptions,
            longoptions, &option_index)) != -1) {
      options++;
      switch (c) {
      case 'b':                   /* do not become daemon */
      case 'F':
      case OPT_RUNFOREGROUND:
         opt->run_foreground = 1;
         break;
      case OPT_PIDFILE:
         if (optarg[0] == '-') {
            errflag++;
            break;
         }
         options--;
        if (opt->pid_file) do_free(opt->pid_file);
        opt->pid_file = do_strdup(optarg);
        break;
#ifdef _WIN32
      case 'I':
      case OPT_INSTALL:
         opt->service_install = 1;
         break;
      case 'U':
      case OPT_UNINSTALL:
         opt->service_install = -1;
         break;
#endif
    /*  case 'l':
      case OPT_LOAD_ONLY:
         opt->run_foreground = 1;
         opt->load_only = TRUE;
         c = optind-1 ;
         while ( c < argc && argv[c][0] != '-' ) {
            if ( !strcmp(argv[c],"abort") ) {
               opt->load_objects=-1;
               break;
            }
            if ( !strcmp(argv[c],"product") )
               opt->load_objects|=ARTIX_SEND_PRODUCT;
            else if ( !strcmp(argv[c],"cashier") )
               opt->load_objects|=ARTIX_SEND_CASHIER;
            else if ( !strcmp(argv[c],"keyboard") )
               opt->load_objects|=ARTIX_SEND_KEYBOARD;
            else {
               errflag++;
               break;
            }
            c++;
            options--;
         }
         if ( !opt->load_objects ) {
            errflag++;
            break;
         }
         break;*/
      case 'd':
      case OPT_DEBUG:
         if (optarg[0] == '-') {
            errflag++;
            break;
         }
         options--;
         opt->debug_level = atoi(optarg);
         break;
      case 'f':
      case OPT_CFGFILE:
         if (optarg[0] == '-') {
            errflag++;
            break;
         }
         options--;
         if (opt->config_file) do_free(opt->config_file);
         opt->config_file = do_strdup(optarg);
         break;
      case 'V':
      case OPT_VERSION:
         opt->show_version = 1;
         oneshot = 1;
         break;
      case '?':
      case OPT_HELP:
      default:
         errflag++;
      }
   }
   if ((oneshot == 1) && options > 1) {
      do_log(LOG_ERR, "Error: too many arguments.");
      errflag++;
   }

   if (errflag)
      print_usage(argv);
   return errflag;
}

