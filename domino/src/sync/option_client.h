#ifndef OPTION_H
#define OPTION_H

#include <domino.h>

typedef struct {
    char  dtype[1024];
    char  sklad[1024];
    char  document[1024];
} option_document_t;

typedef struct{
    enum {
        OBJ_PRODUCT,
        OBJ_PRODUCTS,
        OBJ_PARCELS,
        OBJ_PARTNER,
        OBJ_PARTNERS,
        OBJ_GROUPS,
        OBJ_OPERATIONS,
        OBJ_SKLADS,
        OBJ_OTDELS,
        OBJ_USERS,
        OBJ_DOCUMENT,
        OBJ_DOCUMENT_PERIOD,
        OBJ_DOCUMENT_MODIFIED,
        OBJ_STOCKS1,
        OBJ_UPDATED_STOCKS,
        OBJ_CHECKS,
        OBJ_REALIZATION,
        OBJ_PROTOCOL,

        OBJ_MAKE_REALIZATION,
        OBJ_CHECK_BALANCE_41,
        OBJ_CHECK_PARCELS,
    } obj;
    union {
        struct {
            char  sklad[1024];
        } parcels;
        struct {
            char  sklad[1024];
        } stocks;
        struct {
            char code[1024];
        } product;
        struct{
            char  g_code[1024];
            int   code;
        } partner;
        struct {
            do_list_t *documents;
            struct tm date_beg;
            struct tm date_end;
            int   all;
        } document;
        struct {
            char  sklad[1024];
            struct tm date_beg;
            struct tm date_end;
        } sklad_period;
        struct {
            struct tm date_beg;
            struct tm date_end;
        } protocol;
        struct {
            char  sklad[1024];
            struct tm date_end;
        } check_balance_41;
    };
}obj_arg_t;

typedef struct {
    int   debug_level;
    do_log_type_t log_type;
    char *log_file;
    int  show_version;
    char *domino_config_file;
    char *host;
    char *user;
    char *password;
    char *pid_file;
    int   port;
    char *alias_name;
    char *key_file;
    do_list_t *objs;
} option_client_t;

#ifdef __cplusplus
extern "C"
{
#endif

option_client_t *option_client_new();
void option_client_free(option_client_t *opt);
int  option_client_parse_options(option_client_t *opt, int argc, char *argv[]);
#ifdef __cplusplus
}
#endif
#endif
