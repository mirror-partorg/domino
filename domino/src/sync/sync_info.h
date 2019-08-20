#ifndef _SYNCINFO_H_
#define _SYNCINFO_H_

#include <domino.h>
#include "option.h"

typedef struct {
    option_t *opt;

} sync_info_t;

sync_info_t *sync_info_new(option_t *opt);
void sync_info_free(sync_info_t *sync_info);

void sync_exec_command(int client_id, void *owner, int command, do_list_t *params, do_data_t *data, do_data_t *out);
do_rpc_command_t *sync_info_get_cmd();
int sync_info_get_cmd_count();
void sync_get_document(do_alias_t *alias, document_key0_t *document_key0, do_data_t *out);
void sync_info_put_product(sync_info_t *sync_info, int clientId, const char *param, do_data_t *in, do_data_t *out);

#endif // _SYNCINFO_H_
