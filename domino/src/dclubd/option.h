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
    int  run_foreground;
    int  only_check;
    int  create_db;
    char *user;
    char *path_input;
    char *path_output;
    char *path_error;
    char *mask_input;
    char *mask_output;
    char *domino_config_file;
    char *pid_file;
    char *alias_name;
    char *ext_program;

#ifdef _LINUX
    int  win_filename;
#endif
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
