#ifndef _DLCUBINFO_H_
#define _DLCUBINFO_H_

#include <dolib.h>
#include "option.h"

typedef struct {
    option_t *opt;
    do_string_list_t *lasttrans;
    time_t *time_trans;
} dclub_info_t;

dclub_info_t *dclub_info_new(option_t *opt);
void dclub_info_free(dclub_info_t *dclub_info);
void dclub_info_set(dclub_info_t *dclub_info, do_string_list_t *list, time_t *time);

void dclub_exec_command(int client_id, void *owner, int command, do_list_t *params, do_data_t *data, do_data_t *ctrl_data_out);
do_rpc_command_t *dclub_info_get_cmd();
int dclub_info_get_cmd_count();

#endif // _DLCUBINFO_H_
