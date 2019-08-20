#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "option.h"
#include "../config/config.h"
#include "../misc/csv_util.h"
#include <dolib.h>

#define MASKIN "s%1s%6s.%3s tt check cash"
#define MASKOUT "r%1s%6s.%3s tt check cash"

#define PATH_IN "../mail/in"
#define PATH_OUT "../mail/out"
#define PATH_ERR "../mail/err"


#define FREE_STR(a) { if (a) do_free(a); }

option_t *option_new()
{
    option_t *result = (option_t *) do_malloc ( sizeof(option_t));
    result->config_file = DOMINO_CONFIG(VIDEO_CONFIG_FILE);
    result->passwd_file = do_strdup(DOMINO_PASSWD_FILE);
    result->debug_level = DEFAULT_DEBUG_LEVEL;
    result->log_type = DEFAULT_LOG_TYPE;
    result->log_file = DOMINO_CONFIG(VIDEO_LOG_FILE);
    result->show_version = 0;
    result->pid_file = NULL;
    result->run_foreground = 0;
    result->tcp_port = VIDEO_SERVER_TCP_PORT;
    result->telnet_tcp_port = VIDEO_SERVER_TELNET_TCP_PORT;
    result->time_wait_piece = 60*20;
    result->time_sleep = 60*2;
    result->storage_minimum_free = 500*1024*1024;
    result->storage_reserve_free = 50*1024*1024;
    result->storage_minimum_free *= 1024;
    result->storage_reserve_free *= 1024;


#ifdef _WIN32
    result->service_install = 0;
#endif
    return result;
}
void option_free(option_t *opt)
{
    FREE_STR(opt->config_file);
    FREE_STR(opt->passwd_file);
    FREE_STR(opt->log_file);
    FREE_STR(opt->pid_file);
    FREE_STR(opt->storage_url);
    FREE_STR(opt->conninfo);
    FREE_STR(opt->send_error_to);
    FREE_STR(opt->ext_program);
    do_free(opt);
}
static char *const shortoptions = "f:d:?VbFIU";
enum {
   OPT_NOARG,
   OPT_HELP,
   OPT_VERSION,
   OPT_CFGFILE,
   OPT_DEBUG,
   OPT_CLEAR,
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
   {"config-file",         required_argument, NULL, OPT_CFGFILE},
   {"debug",               required_argument, NULL, OPT_DEBUG},
   {"clear",               no_argument,       NULL, OPT_CLEAR},
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
   printf("usage: dclubd [options]\n"
         "  Options are as follows:\n"
         "  -b, -F, --run-foreground  don't go into background(foreground)\n"
         "  --clear                   clear storage only \n"
         "  -d, --debug <level>       set debug level (>0)\n"
         "  -f, --config-file <file>  load specified config file\n"
         "  -V, --version             display version info\n"
         "  -?, --help                display this help\n"
         "  --check                   check config\n"
         "  --createdb                create new data file"
         "  --writepid <pid_file>     write pid file\n"
#ifdef _WIN32
         "  -I, --install             install service\n"
         "  -U, --uninstall           uninstall service\n"
#endif
         "\n"
         "Copyright (C) 2008-2009 Youra Richesse\n"
         "dclubd is free software and comes with ABSOLUTELY NO WARRANTY\n"
         "under the terms of the GNU General Public License\n"
         "\n"
         "Report bugs to dclubd Support Center:\n"
         "  mir@glekar.ru\n");
}

int option_read_config(option_t *opt)
{
#define READ_OPT_STR(s,b)  if (!strcmp(element,s)) {if (opt->b) do_free(opt->b); opt->b = do_strdup(value); }
#define READ_OPT_INT(s,b)  if (!strcmp(element,s)) { opt->b = atoi(value); }

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
        READ_OPT_STR("passwd-file", passwd_file)
        else
        READ_OPT_INT("time-wait-piece", time_wait_piece)
        else
        READ_OPT_INT("time-sleep", time_sleep)
        else
        READ_OPT_STR("storage-url", storage_url)
        else
        READ_OPT_STR("ext-program", ext_program)
        else
        READ_OPT_STR("conninfo", conninfo)
        else
        READ_OPT_STR("send-error-to", send_error_to)
        else
        if ( !strcmp(element,"disable-send-error") )
            opt->disable_send_error = !strcmp(value, "yes") || strcmp(value, "1");
        else
        if (!strcmp(element,"debug-level"))
            opt->debug_level = atoi(value);
        else
        if (!strcmp(element,"tcp-port"))
            opt->tcp_port = atoi(value);
        else
        if ( !strcmp(element,"storage-minimum-free") ) {
            char *p1;
            double fsize;
            p1 = value;
            fsize = strtod(p1, &p1);
            switch(toupper(*p1)) {
            case 'K':
                fsize *= 1024;
                break;
            case 'M':
                fsize *= 1024*1024;
                break;
            case 'G':
                fsize *= 1024*1024*1024;
                break;
            case 'T':
                fsize *= 1024*1024*1024;
                fsize *= 1024;
                break;
            }
            opt->storage_minimum_free = (int64_t)fsize;
        }
        else
        if ( !strcmp(element,"storage-reserve-free") ) {
            char *p1;
            double fsize;
            p1 = value;
            fsize = strtod(p1, &p1);
            switch(toupper(*p1)) {
            case 'K':
                fsize *= 1024;
                break;
            case 'M':
                fsize *= 1024*1024;
                break;
            case 'G':
                fsize *= 1024*1024*1024;
                break;
            case 'T':
                fsize *= 1024*1024*1024;
                fsize *= 1024;
                break;
            }
            opt->storage_reserve_free = (int64_t)fsize;
        }
#ifdef PSEUDO_TELNET
        if (!strcmp(element,"telnet-port"))
            opt->telnet_tcp_port = atoi(value);
#endif
    }
    do_free(line);
    fclose(fp);
    int res;
    res = (opt->storage_url != NULL) && (opt->conninfo != NULL)
		;
    if (!res) {
        if (opt->storage_url == NULL)
            do_log(LOG_ERR, "undefine \"storage-url\"");
        if (opt->conninfo == NULL)
            do_log(LOG_ERR, "undefine \"conninfo\"");
    }
    return res;
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
      case OPT_PIDFILE:
         if (optarg[0] == '-') {
            errflag++;
            break;
         }
         options--;
        if (opt->pid_file) do_free(opt->pid_file);
        opt->pid_file = do_strdup(optarg);
        break;
      case 'b':                   /* do not become daemon */
      case 'F':
      case OPT_RUNFOREGROUND:
         opt->run_foreground = 1;
         break;
      case OPT_CLEAR:
         opt->clear_only = 1;
         opt->run_foreground = 1;
         opt->debug_level = 100;
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

