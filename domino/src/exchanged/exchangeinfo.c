#include "exchangeinfo.h"
#include <string.h>

#include "exchange.h"
#include "option.h"
#include <domino.h>
#include <dolib.h>
#include "../../config/version.h"
#include "../misc/mproc_state.h"

typedef  enum {
    DC_CMD_GET_TIME_FULL_UPDATE = 0,
    DC_CMD_GET_TIME_PRODUCT_UPDATE,
    DC_CMD_GET_SERVICE_STATE,
    DC_CMD_FULL_UPDATE,
    DC_CMD_GET_INFO,
    DC_CMD_UNDEFINE,
} exchange_command_t;

static const do_rpc_command_t dc_commands[DC_CMD_UNDEFINE] = {
    {"get_time_full_update", "show time full update", "show time full update", 0, 0},
    {"get_time_product_update", "show time product update", "show time product update", 0, 0},
    {"get_service_state", "show service state", "show service state ", 0, 0},
    {"full_update", "start full update", "start full update", 0, 0},
    {"get_info", "show info information", "show full info information", 0, 0}
};

do_rpc_command_t *exchange_info_get_cmd()
{
    return (do_rpc_command_t*)&dc_commands;
}

int exchange_info_get_cmd_count()
{
    return DC_CMD_UNDEFINE;
}

exchange_info_t  *exchange_info_new(option_t *opt)
{
    exchange_info_t *result = (exchange_info_t*) do_malloc (sizeof(exchange_info_t));
    result->opt = opt;
    result->last_full_update = NULL;
    result->last_update = NULL;
    result->state = NULL;
    return result;
}

void exchange_info_set(exchange_info_t *exchange_info, void *exchange, time_t *create, time_t *update, int *state)
{
    exchange_info->last_full_update = create;
    exchange_info->last_update = update;
    exchange_info->state = state;
    exchange_info->exchange = exchange;
}

void exchange_info_free(exchange_info_t *exchange_info)
{
    do_free(exchange_info);
}

static void get_time_full_update(exchange_info_t *exchange_info, do_data_t *out)
{
    do_data_set_str(out, "");
    if (exchange_info->last_full_update && *exchange_info->last_full_update) {
        char ch[101];
	size_t len = strftime(ch, 100, "%d/%m/%Y %H:%M:%S", localtime(exchange_info->last_full_update));
	ch[len] = '\0';
        do_data_add_str(out, ch);
    }
}

static void get_time_product_update(exchange_info_t *exchange_info, do_data_t *out)
{
    do_data_set_str(out, "");
    if (exchange_info->last_update && *exchange_info->last_update) {
        char ch[101];
	size_t len = strftime(ch, 100, "%d/%m/%Y %H:%M:%S", localtime(exchange_info->last_update));
	ch[len] = '\0';
        do_data_add_str(out, ch);
    }
}

static void get_service_state(exchange_info_t *exchange_info, do_data_t *out)
{
    do_data_set_str(out, "");
    switch (*exchange_info->state) {
	case 0:
            do_data_add_strf(out, "sleep");
	    break;
	case 1:
            do_data_add_strf(out, "product full update");
	    break;
	case 2:
            do_data_add_strf(out, "product update");
	    break;
	case 3:
            do_data_add_strf(out, "cashier full update");
	    break;
	case 4:
            do_data_add_strf(out, "keyboard full update");
	    break;
	case 5:
            do_data_add_strf(out, "wait for ready");
	    break;
	default :
    	    do_data_set_str(out, "");
    }
}

static void cache_clear(exchange_info_t *exchange_info)
{
    exchange_cache_clear(exchange_info->exchange);
}
static void get_info(exchange_info_t *exchange_info, do_data_t *out)
{
    do_data_set_str(out, "");
    do_data_t *tmp = do_data_new();
    do_data_set_str(out, VERSION " (" ADATE ")\n");
    do_data_add_str(out, do_proc_state_get_name(do_mproc_state_get_state()));
    do_data_add_do_data(out, tmp);
    do_data_add_str(out, "\n");
    get_service_state(exchange_info, tmp);
    do_data_add_do_data(out, tmp);
    do_data_add_str(out, "\n");
    get_time_full_update(exchange_info, tmp);
    do_data_add_do_data(out, tmp);
    do_data_add_str(out, "\n");
    get_time_product_update(exchange_info, tmp);
    do_data_add_do_data(out, tmp);
    do_data_add_str(out, "\n");
    do_data_free(tmp);
}
void exchange_exec_command(int id, void *owner, int command, do_list_t *param, do_data_t *in, do_data_t *out)
{
    exchange_info_t *exchange_info = owner;
    switch (command) {
        case DC_CMD_GET_TIME_FULL_UPDATE:
    	    get_time_full_update(exchange_info, out);
            break;
        case DC_CMD_GET_TIME_PRODUCT_UPDATE:
    	    get_time_product_update(exchange_info, out);
            break;
        case DC_CMD_GET_SERVICE_STATE:
    	    get_service_state(exchange_info, out);
            break;
        case DC_CMD_FULL_UPDATE:
            cache_clear(exchange_info);
            break;
        case DC_CMD_GET_INFO:
    	    get_info(exchange_info, out);
            break;
        default:
            do_data_set_errf(out, "undefine command %d", command);
    }
}
