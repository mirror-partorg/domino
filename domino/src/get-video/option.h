#ifndef OPTION_H
#define OPTION_H

#include <dolib.h>

#define NULL_FORMAT "null"
#define NULL_DEVICE "/dev/null"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    char  *config_file;
    char  *passwd_file;

    char  *server_host;
    int    server_port;
    char  *server_user;
    char  *server_password;
    char  *cam_name;

    /*char  *user;
    char  *group;*/

    int   debug_level;
    do_log_type_t log_type;
    char *log_file;
    int  show_version;
    int  run_foreground;
    int  only_check;

    char *pid_file;
    char *storage_url;
#ifdef USE_POSTGRESQL
    char *conninfo;
#endif
    char *video_format;
    char *video_device;
    char *video_rate;
    char *video_bitrate;
    char *video_size;
    char *video_pixel_format;
    char *video_ratio;
    char *video_codec;

    char *audio_format;
    char *audio_device;
    char *audio_bitrate;
    char *audio_codec;

    int  filesize;
    int  disable_send_error;

    char *filters;

    int  tcp_port;
#ifdef _WIN32
    int  service_install;
#endif
} option_t;

option_t *option_new();
void option_free(option_t *opt);
char *option_show_config(option_t *opt);
int  option_read_config(option_t *opt);
int  option_parse_options(option_t *opt, int argc, char *argv[]);
#ifdef __cplusplus
}
#endif
#endif
