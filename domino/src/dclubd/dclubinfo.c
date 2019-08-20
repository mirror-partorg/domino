#include "dclubinfo.h"
#include <string.h>

#include "dclub.h"
#include "option.h"
#include <domino.h>
#include <dolib.h>

typedef  enum {
    DC_CMD_TRANS = 0,
    DC_CMD_LAST,
    DC_CMD_UNDEFINE,
} dclub_command_t;

static const do_rpc_command_t dc_commands[DC_CMD_UNDEFINE] = {
    {"trans", "view last transaction", "view last transaction\r\n\tparams: [onlycard] [onlysums]", 0, 0},
    {"last", "view time of last transaction", "view time of last transaction", 0, 0},
};

do_rpc_command_t *dclub_info_get_cmd()
{
    return (do_rpc_command_t*)&dc_commands;
}

int dclub_info_get_cmd_count()
{
    return DC_CMD_UNDEFINE;
}

dclub_info_t  *dclub_info_new(option_t *opt)
{
    dclub_info_t *result = (dclub_info_t*) do_malloc (sizeof(dclub_info_t));
    result->opt = opt;
    result->lasttrans = NULL;
    return result;
}

void dclub_info_set(dclub_info_t *dclub_info, do_string_list_t *list, time_t *time)
{
    dclub_info->lasttrans = list;
    dclub_info->time_trans = time;
}

void dclub_info_free(dclub_info_t *dclub_info)
{
    do_free(dclub_info);
}

static void get_last_trans_info(dclub_info_t *dclub_info, do_data_t *out)
{
    do_data_set_str(out, "");
    if (!dclub_info->lasttrans || !dclub_info->lasttrans->count)
        return;
    int i;
    for (i = 0; i < dclub_info->lasttrans->count; i++)
        do_data_add_strf(out, "%s\n", dclub_info->lasttrans->list[i]);
}

static void get_last(dclub_info_t *dclub_info, do_data_t *out)
{
    do_data_set_str(out, "");
    if (!dclub_info->time_trans || !(*dclub_info->time_trans))
        return;
    char ch[101];
    size_t len = strftime(ch, 100, "%d/%m/%Y %H:%M:%S", localtime(dclub_info->time_trans));
    ch[len] = '\0';
    do_data_set_str(out, ch);
}

void dclub_exec_command(int client_id, void *owner, int command, do_list_t *params, do_data_t *data, do_data_t *out)
{
    dclub_info_t *dclub_info = owner;
    switch (command) {
        case DC_CMD_TRANS:
            return get_last_trans_info(dclub_info, out);
        case DC_CMD_LAST:
            return get_last(dclub_info, out);
        default:
            do_data_set_errf(out, "undefine command %d", command);
    }
}
