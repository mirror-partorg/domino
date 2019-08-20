#ifndef OPTION_H
#define OPTION_H

#include <dolib.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    char *config_file;
    int   debug_level;
    do_log_type_t log_type;
    char *log_file;
    int  show_version;
    int  clear_only;
    int  run_foreground;
    char *passwd_file;
    char *pid_file;
    int   time_wait_piece;
    int   time_sleep;
    char *send_error_to;

    char *storage_url;
    char *conninfo;
    int  disable_send_error;
    char *ext_program;

    size_t storage_minimum_free;
    size_t storage_reserve_free;

    int  tcp_port;
    int  telnet_tcp_port;
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
