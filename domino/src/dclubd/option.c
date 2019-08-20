#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "option.h"
#include "../config/config.h"
#include "../misc/csv_util.h"
#include <dolib.h>

#define MASKIN "s%1s%6s.%3s tt check cash"
#define MASKOUT "r%1s%6s.%3s tt check cash"

#define PATH_IN "../mail/in"
#define PATH_OUT "../mail/out"
#define PATH_ERR "../mail/err"


option_t *option_new()
{
    option_t *result = (option_t *) do_malloc ( sizeof(option_t));
    result->config_file = DOMINO_CONFIG(DCLUB_CONFIG_FILE);
    //result->passwd_file = do_strdup(DOMINO_PASSWD_FILE);
    result->user = do_strdup(DOMINO_USERNAME);
    result->debug_level = DEFAULT_DEBUG_LEVEL;
    result->log_type = DEFAULT_LOG_TYPE;
    result->domino_config_file = NULL;
    result->log_file = DOMINO_CONFIG(DCLUB_LOG_FILE);
    result->show_version = 0;
    result->pid_file = NULL;
    result->run_foreground = 0;
    result->only_check = 0;
    result->create_db = 0;
    result->alias_name = NULL;
    result->path_input = do_strdup(PATH_IN);
    result->path_output = do_strdup(PATH_OUT);
    result->path_error = do_strdup(PATH_ERR);
    result->mask_input = do_strdup(MASKIN);
    result->mask_output = do_strdup(MASKOUT);
    result->ext_program = do_strdup(DOMINO_PATH "/" DCLUB_SCRIPT);
#ifdef _LINUX
    result->win_filename = FALSE;
#endif
    result->tcp_port = DCLUB_DEFAULT_TCP_PORT;
    result->telnet_tcp_port = DCLUB_DEFAULT_TELNET_TCP_PORT;
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
    if (opt->path_input)
        do_free(opt->path_input);
    if (opt->path_output)
        do_free(opt->path_output);
    if (opt->user)
        do_free(opt->user);
    if (opt->log_file)
        do_free(opt->log_file);
    if (opt->mask_input)
        do_free(opt->mask_input);
    if (opt->mask_output)
        do_free(opt->mask_output);
    if (opt->path_error)
        do_free(opt->path_error);
    if ( opt->alias_name )
        do_free(opt->alias_name);
    if ( opt->ext_program )
         do_free(opt->ext_program);
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
   {"config-file",         required_argument, NULL, OPT_CFGFILE},
   {"debug",               required_argument, NULL, OPT_DEBUG},
   {"check",               no_argument      , NULL, OPT_CHECK},
   {"createdb",            no_argument      , NULL, OPT_CREATE},
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
        if (!strcmp(element,"path-input")){
            if (opt->path_input) do_free(opt->path_input);
            opt->path_input = do_strdup(value);
        }
        else
        if (!strcmp(element,"path-output")){
            if (opt->path_output) do_free(opt->path_output);
            opt->path_output = do_strdup(value);
        }
        else
        if (!strcmp(element,"path-error")) {
            if (opt->path_error) do_free(opt->path_error);
            opt->path_error = do_strdup(value);
        }
        else
        if (!strcmp(element,"mask-input")){
            if (opt->mask_input) do_free(opt->mask_input);
            opt->mask_input = do_strdup(value);
        }
        else
        if (!strcmp(element,"alias-name")){
            if (opt->alias_name) do_free(opt->alias_name);
            opt->alias_name = do_strdup(value);
        }
        else
        if (!strcmp(element,"mask-output")){
            if (opt->mask_output) do_free(opt->mask_output);
            opt->mask_output = do_strdup(value);
        }
        else
        if (!strcmp(element,"user")){
            if (opt->user) do_free(opt->user);
            opt->user = do_strdup(value);
        }
        else
        if (!strcmp(element,"ext-command")){
            if (opt->ext_program) do_free(opt->ext_program);
            opt->ext_program = do_strdup(value);
        }
        else
#ifdef _LINUX
        if (!strcmp(element,"win-filename"))
            opt->win_filename = strcmp(value, "yes") == 0;
        else
#endif
        if (!strcmp(element,"tcp-port"))
            opt->tcp_port = atoi(value);
#ifdef PSEUDO_TELNET
        if (!strcmp(element,"telnet-port"))
            opt->telnet_tcp_port = atoi(value);
#endif
    }
    do_free(line);
    fclose(fp);
    int res;
    res = (opt->path_input != NULL) && (opt->path_output != NULL) && (opt->path_error != NULL) &&
              (opt->mask_input[0] != '\0') && (opt->mask_output[0] != '\0')
		;
    if (!res) {
        if (opt->path_input == NULL)
            do_log(LOG_ERR, "undefine \"path-input\" - directory to find input files\n");
        if (opt->path_output == NULL)
            do_log(LOG_ERR, "undefine \"path-output\" - directory to write output files\n");
        if (opt->path_error == NULL)
            do_log(LOG_ERR, "undefine \"path-error\" - directory to save error files\n");
        if (opt->mask_input[0] == '\0')
            do_log(LOG_ERR, "undefine \"mask-input\" - input file mask\n");
        if (opt->mask_output[0] == '\0')
            do_log(LOG_ERR, "undefine \"mask-out\" - output file mask\n");
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
      case OPT_CHECK:
         opt->run_foreground = 1;
         opt->only_check = 1;
         break;
      case OPT_CREATE:
         opt->run_foreground = 1;
         opt->create_db = 1;
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

