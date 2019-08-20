#ifndef EOPTION_H
#define EOPTION_H

#include <domino.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    char          *config_file;
    int            debug_level;
    do_log_type_t  log_type;
    char          *log_file;
    int            show_version;
    char          *artix_host;
    char          *artix_cs;
    char          *artix_user;
    char          *artix_password;
    char          *domino_config_file;
} option_t;

option_t *option_new();
void option_free(option_t *opt);
int  option_read_config(option_t *opt);
int  option_parse_options(option_t *opt, int argc, char *argv[]);
#ifdef __cplusplus
}
#endif
#endif
