#ifndef EOPTION_H
#define EOPTION_H

#include <dolib.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    char         *config_file;
    int           debug_level;
    do_log_type_t log_type;
    char         *log_file;
    int           show_version;
//    int           load_only;
//    int           load_objects;
    int           run_foreground;
    char         *user;
    int           sleep_msec;

    char *artix_host;
    int   artix_port;
    char *artix_cs;
    char *artix_user;
    char *artix_password;
    char *pid_file;

    int   handbook_full_update_timeout;

    char *domino_config_file;

    int  tcp_port;
    int  telnet_tcp_port;
#ifdef _WIN32
    int  service_install;
#endif
    int nretail_price;
} option_t;

option_t *option_new();
void option_free(option_t *opt);
int  option_read_config(option_t *opt);
int  option_parse_options(option_t *opt, int argc, char *argv[]);
#ifdef __cplusplus
}
#endif
#endif
