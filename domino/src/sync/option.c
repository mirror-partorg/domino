#include <getopt.h>
#include <errno.h>

#include <domino.h>
#include "option.h"
#include "../config/config.h"
#include "../misc/csv_util.h"

option_t *option_new()
{
    option_t *result = (option_t *) do_malloc ( sizeof(option_t));
    result->config_file = DOMINO_CONFIG(SYNC_CONFIG_FILE);
    result->user = do_strdup(DOMINO_USERNAME);
    result->debug_level = DEFAULT_DEBUG_LEVEL;
    result->log_type = DEFAULT_LOG_TYPE;
    result->domino_config_file = NULL;
    result->log_file = DOMINO_CONFIG(SYNC_LOG_FILE);
    result->document_replic_config_file = DOMINO_CONFIG(SYNC_DOCUMENT_REPLIC_CONFIG_FILE);
    result->document_replic_aging_days = SYNC_DOCUMENT_REPLIC_AGING_DAYS;
    result->show_version = 0;
    result->run_foreground = 0;
    result->only_check = 0;
    result->tcp_port = SYNC_DEFAULT_TCP_PORT;
    result->alias_name = NULL;
    result->client_host = NULL;
    result->client_name = NULL;
    result->client_tcp_port = SYNC_DEFAULT_TCP_PORT;
    result->client_key_file = NULL;
    result->pid_file = NULL;
    result->handbook_replic1 = FALSE;
    result->stock_replic1 = FALSE;
    result->document_replic1 = FALSE;
    result->document_replic_all = FALSE;


    result->telnet_tcp_port = SYNC_DEFAULT_TELNET_TCP_PORT;
#ifdef _WIN32
    result->service_install = 0;
#endif
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
    if (opt->alias_name)
        do_free(opt->alias_name);
    if (opt->client_host)
        do_free(opt->client_host);
    if (opt->client_name)
        do_free(opt->client_name);
    if (opt->client_key_file)
        do_free(opt->client_key_file);
    if (opt->pid_file)
        do_free(opt->pid_file);
    if (opt->document_replic_config_file)
        do_free(opt->document_replic_config_file);
    do_free(opt);
}
static char *const shortoptions = "f:d:?VbFIU";
enum {
   OPT_NOARG,
   OPT_HELP,
   OPT_VERSION,
   OPT_CFGFILE,
   OPT_DEBUG,
   OPT_CHECK,
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
   {"config",              required_argument, NULL, OPT_CFGFILE},
   {"debug",               required_argument, NULL, OPT_DEBUG},
   {"check",               no_argument      , NULL, OPT_CHECK},
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
   printf("usage: syncd [options]\n"
         "  Options are as follows:\n"
         "  -b, -F, --run-foreground  don't go into background(foreground)\n"
         "  --writepid <pid_file>     write pid file\n"
         "  -d, --debug <level>       set debug level (>0)\n"
         "  -f, --config <file>  load specified config file\n"
         "  -V, --version             display version info\n"
         "  -?, --help                display this help\n"
         "  --check                   check config\n"
#ifdef _WIN32
         "  -I, --install             install service\n"
         "  -U, --uninstall           uninstall service\n"
#endif
         "\n"
         "Copyright (C) 2008-2009 Youra Richesse\n"
         "syncd is free software and comes with ABSOLUTELY NO WARRANTY\n"
         "under the terms of the GNU General Public License\n"
         "\n"
         "Report bugs to dclubd Support Center:\n"
         "  mir@glekar.ru\n");
}

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
        else
        if (!strcmp(element,"domino-config-file")){
            if (opt->domino_config_file) do_free(opt->domino_config_file);
            opt->domino_config_file = do_strdup(value);
        }
        else
        if (!strcmp(element,"debug-level"))
            opt->debug_level = atoi(value);
        else
        if (!strcmp(element,"user")){
            if (opt->user) do_free(opt->user);
            opt->user = do_strdup(value);
        }
        else
        if (!strcmp(element,"alias-name")){
            if (opt->alias_name) do_free(opt->alias_name);
            opt->alias_name = do_strdup(value);
        }
        else
        if (!strcmp(element,"client-host")){
            if (opt->client_host) do_free(opt->client_host);
            opt->client_host = do_strdup(value);
        }
        else
        if (!strcmp(element,"client-name")){
            if (opt->client_name) do_free(opt->client_name);
            opt->client_name = do_strdup(value);
        }
        else
        if (!strcmp(element,"client-tcp-port"))
            opt->client_tcp_port = atoi(value);
        else
        if (!strcmp(element,"client-key-file")){
            if (opt->client_key_file) do_free(opt->client_key_file);
            opt->client_key_file = do_strdup(value);
        }
        else
        if (!strcmp(element,"document-replic-config-file")){
            if (opt->document_replic_config_file) do_free(opt->document_replic_config_file);
            opt->document_replic_config_file = do_strdup(value);
        }
        else
        if (!strcmp(element,"handbook-replic")){
            opt->handbook_replic1 = !strcmp(value, "yes");
        }
        else
        if (!strcmp(element,"stock-replic")){
            opt->stock_replic1 = !strcmp(value, "yes");
        }
        else
        if (!strcmp(element,"document-replic")){
            opt->document_replic1 = !strcmp(value, "yes");
        }
        else
        if (!strcmp(element,"document-replic-all")){
            opt->document_replic_all = !strcmp(value, "yes");
        }
        else
        if (!strcmp(element,"tcp-port")) {
            opt->tcp_port = atoi(value);
        }
        else
        if (!strcmp(element,"telnet-port")) {
            opt->telnet_tcp_port = atoi(value);
        }
        else
        if (!strcmp(element,"document-replic-aging-days")) {
            opt->document_replic_aging_days = atoi(value);
        }
    }
    do_free(line);
    fclose(fp);

    if ( opt->document_replic1 || opt->handbook_replic1 || opt->stock_replic1  ) {
        if (!opt->client_host || !opt->client_key_file) {
            if (!opt->client_host)
                do_log(LOG_ERR, "undefine \"client-host\" - client host\n");
            if (!opt->client_key_file)
                do_log(LOG_ERR, "undefine \"client-key-file\" - client key file\n");
            return 0;
        }
        if ( opt->document_replic1 && !opt->document_replic_config_file ) {
            do_log(LOG_ERR, "undefine \"document-replic-config-file\" - document replic config file\n");
            return 0;
        }
    }
    if ( !opt->client_name )
        opt->client_name = do_strdup(opt->client_host ? opt->client_host : "LOCAL");
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
      case OPT_CHECK:
         opt->run_foreground = 1;
         opt->only_check = 1;
         break;
      case 'b':                   /* do not become daemon */
      case 'F':
      case OPT_RUNFOREGROUND:
         opt->run_foreground = 1;
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
      case OPT_PIDFILE:
         if (optarg[0] == '-') {
            errflag++;
            break;
         }
         options--;
         if (opt->pid_file) do_free(opt->pid_file);
         opt->pid_file = do_strdup(optarg);
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
      print_usage();
   return errflag;
}

