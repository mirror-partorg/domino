#ifndef _EXCHANGEINFO_H_
#define _EXCHANGEINFO_H_

#include <dolib.h>
#include <domino.h>
#include "option.h"


typedef struct {
    option_t *opt;
    time_t *last_full_update;
    time_t *last_update;
    int    *state;
    void   *exchange;
    do_alias_t *alias;
} exchange_info_t;

#ifdef __cplusplus
extern "C"
{
#endif

exchange_info_t *exchange_info_new(option_t *opt);
void exchange_info_free(exchange_info_t *exchange_info);
void exchange_info_set(exchange_info_t *exchange_info, void *exchange, time_t *create, time_t *update, int *state);
void exchange_info_set_alias(exchange_info_t *exchange_info, do_alias_t *alias);

void exchange_exec_command(int client_id, void *owner, int command, do_list_t *params, do_data_t *in, do_data_t *out);
do_rpc_command_t *exchange_info_get_cmd();
int exchange_info_get_cmd_count();

#ifdef __cplusplus
}
#endif


#endif // _DLCUBINFO_H_
