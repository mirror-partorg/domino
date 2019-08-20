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
    char *user;
    char *domino_config_file;
    char *alias_name;
    int  tcp_port;
    char *client_host;
    char *client_name;
    int   client_tcp_port;
    char *client_key_file;
    char *pid_file;

    int   handbook_replic1;
    int   document_replic1;
    int   stock_replic1;

    int   document_replic_all;
    char *document_replic_config_file;
    int   document_replic_aging_days;
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
