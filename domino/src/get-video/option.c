#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "option.h"
#include "../config/config.h"
#include "../misc/csv_util.h"
#include "version.h"
#include <dolib.h>
#include <ctype.h>


option_t *option_new()
{
    option_t *result = (option_t *) do_malloc ( sizeof(option_t));

    result->config_file = DOMINO_CONFIG(FFMPEG_CONFIG_FILE);

    result->passwd_file = do_strdup(DOMINO_PASSWD_FILE);
    result->server_port = VIDEO_SERVER_TCP_PORT;
    result->server_host = NULL;
    result->server_user = do_strdup(VIDEO_USERNAME);
    result->server_password = do_strdup(VIDEO_PASSWORD);

    char hostname[1024];
    if ( gethostname(hostname, 1023) )
        strcpy(hostname,   "unknow");
    result->cam_name = do_strdup(hostname);


    //result->user = do_strdup(DOMINO_USERNAME);
    //result->group = do_strdup(DOMINO_USERNAME);

    result->debug_level = DEFAULT_DEBUG_LEVEL;
    result->log_type = DEFAULT_LOG_TYPE;
    result->log_file = DOMINO_CONFIG(FFMPEG_LOG_FILE);
    result->show_version = 0;
    result->run_foreground = 0;
    result->only_check = 0;

    //char *pid_file;
    //result->storage_url;

    result->video_format = do_strdup(FFMPEG_VIDEO_FORMAT);
    result->video_device = do_strdup(FFMPEG_VIDEO_DEVICE);
    result->video_rate = do_strdup(FFMPEG_VIDEO_RATE);
    result->video_bitrate = do_strdup(FFMPEG_VIDEO_BITRATE);
    result->video_size = do_strdup(FFMPEG_VIDEO_SIZE);
    result->video_pixel_format = do_strdup(FFMPEG_VIDEO_PIXEL_FORMAT);
    result->video_ratio = NULL;//do_strdup(FFMPEG_VIDEO_RATION);
    result->video_codec = do_strdup(FFMPEG_VIDEO_CODEC);

    result->audio_format = do_strdup(FFMPEG_AUDIO_FORMAT);
    result->audio_device = do_strdup(FFMPEG_AUDIO_DEVICE);
    result->audio_bitrate = do_strdup(FFMPEG_AUDIO_BITRATE);
    result->audio_codec = do_strdup(FFMPEG_AUDIO_CODEC);

    result->filesize = FFMPEG_FILESIZE;

    result->tcp_port = FFMPEG_DEFAULT_TCP_PORT;
#ifdef _WIN32
    result->service_install = 0;
#endif
    return result;
}
#define FREE_STR(a) { if (a) do_free(a); }
void option_free(option_t *opt)
{
    FREE_STR(opt->config_file);

    if (opt->log_file)
        do_free(opt->log_file);
//    FREE_STR(opt->user);
    FREE_STR(opt->pid_file);
    FREE_STR(opt->storage_url);
    FREE_STR(opt->video_format);
    FREE_STR(opt->video_device);
    FREE_STR(opt->video_rate);
    FREE_STR(opt->video_bitrate);
    FREE_STR(opt->video_size);
    FREE_STR(opt->video_pixel_format);
    FREE_STR(opt->video_ratio);
    FREE_STR(opt->video_codec);
    FREE_STR(opt->passwd_file);
    FREE_STR(opt->server_host);
    FREE_STR(opt->server_user);
    FREE_STR(opt->server_password);
    FREE_STR(opt->cam_name);
#ifdef USE_POSTGRESQL
    FREE_STR(opt->conninfo);
#endif
    FREE_STR(opt->audio_format);
    FREE_STR(opt->audio_device);
    FREE_STR(opt->audio_bitrate);
    FREE_STR(opt->audio_codec);

    do_free(opt);
}
static char *const shortoptions = "f:d:?VbFIU";
enum {
   OPT_NOARG,
   OPT_HELP,
   OPT_VERSION,
   OPT_DEBUG,
   OPT_CHECK,
   OPT_CFGFILE,
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
   {"debug",               required_argument, NULL, OPT_DEBUG},
   {"check",               no_argument      , NULL, OPT_CHECK},
   {"writepid",            required_argument, NULL, OPT_PIDFILE},
   {"config-file",         required_argument, NULL, OPT_CFGFILE},
#ifdef _WIN32
   {"install",             no_argument      , NULL, OPT_INSTALL},
   {"uninstall",           no_argument      , NULL, OPT_UNINSTALL},
#endif
   {"run-foreground",      no_argument,       NULL, OPT_RUNFOREGROUND},
   {0,                     no_argument,       NULL, OPT_NOARG}
};
void print_usage()
{
   printf("usage: %s [options]\n"
         "  Options are as follows:\n"
         "  -b, -F, --run-foreground  don't go into background(foreground)\n"
         "  -d, --debug <level>       set debug level (>0)\n"
         "  -f, --config-file <file>  load specified config file\n"
         "  -V, --version             display version info\n"
         "  -?, --help                display this help\n"
         "  --check                   check config and devices\n"
         "  --createdb                create new data file"
#ifdef _WIN32
         "  -I, --install             install service\n"
         "  -U, --uninstall           uninstall service\n"
#endif
         "\n"
         "Copyright (C) 2012 Youra Richesse\n"
         "dclubd is free software and comes with ABSOLUTELY NO WARRANTY\n"
         "under the terms of the GNU General Public License\n"
         "\n"
         "Report bugs to dclubd Support Center:\n"
         "  mir@glekar.ru\n", program_name);
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
//        READ_OPT_STR("user", user)
//        else
        READ_OPT_STR("passwd-file", passwd_file)
        else
        READ_OPT_STR("server-host", server_host)
        else
        READ_OPT_INT("server-port", server_port)
        else
        READ_OPT_STR("server-user", server_user)
        else
        READ_OPT_STR("server-password", server_password)
        else
        READ_OPT_STR("cam-name", cam_name)
        else
//        READ_OPT_STR("group", group)
//        else
        READ_OPT_STR("storage-url",storage_url)
        else
        READ_OPT_STR("video-format", video_format)
        else
        READ_OPT_STR("video-device", video_device)
        else
        READ_OPT_STR("video-rate", video_rate)
        else
        READ_OPT_STR("video-bitrate", video_bitrate)
        else
        READ_OPT_STR("video-size", video_size)
        else
        READ_OPT_STR("video-pixel-format", video_pixel_format)
        else
        READ_OPT_STR("video-ration", video_ratio)
        else
        READ_OPT_STR("video-codec", video_codec)
        else
        READ_OPT_STR("audio-format", audio_format)
        else
        READ_OPT_STR("audio-device", audio_device)
#ifdef USE_POSTGRESQL
        else
        READ_OPT_STR("storage-conninfo", conninfo)
#endif
        else
        READ_OPT_STR("audio-bitrate", audio_bitrate)
        else
        READ_OPT_STR("filters", filters)
        else
        READ_OPT_STR("audio-codec", audio_codec)
        else
        if (!strcmp(element,"filesize")) {
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
            }
            opt->filesize = (int64_t)fsize;
        }
        else
        if ( !strcmp(element,"disable-send-error") )
            opt->disable_send_error = !strcmp(value, "yes") || strcmp(value, "1");
        else
        if (!strcmp(element,"debug-level"))
            opt->debug_level = atoi(value);
        else
        if (!strcmp(element,"tcp-port"))
            opt->tcp_port = atoi(value);
    }
    do_free(line);
    fclose(fp);
    int res;
    res = (opt->storage_url != NULL);
		;
    if (!res) {
        if (opt->storage_url == NULL)
            do_log(LOG_ERR, "undefine \"storage-url\" - url storage\n");
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
      print_usage(argv);
   return errflag;
}

