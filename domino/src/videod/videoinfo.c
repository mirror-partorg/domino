#include "videoinfo.h"
#include <string.h>

#include "video.h"
#include "option.h"
#include <domino.h>
#include <dolib.h>

struct _video_info_t
{
    video_t *video;
};

typedef enum {
    DC_CMD_CHANGE_STATUS = 0,
    DC_CMD_UNDEFINE
} video_command_t;

static const do_rpc_command_t dc_commands[DC_CMD_UNDEFINE] = {
    {"change_status", "cam change status", "cam change status\r\n\tparams: <cam> <status>", 0, 0},
};

do_rpc_command_t *video_info_get_cmd()
{
    return (do_rpc_command_t*)&dc_commands;
}

int video_info_get_cmd_count()
{
    return DC_CMD_UNDEFINE;
}

video_info_t  *video_info_new()
{
    video_info_t *result = (video_info_t*) do_malloc (sizeof(video_info_t));
    return result;
}

void video_info_set(video_info_t *video_info, video_t *video)
{
    video_info->video = video;
}

void video_info_free(video_info_t *video_info)
{
    do_free(video_info);
}
#define param_type(a,i)  ((do_rpc_command_param_t*)a->list[i])->type
#define param_value(a,i) ((do_rpc_command_param_t*)a->list[i])->value
#define param_count(a)   a->count

static void change_status(video_info_t *info, do_list_t *params, do_data_t *out)
{
    do_data_set_str(out, "");
    if ( !info->video ) {
        do_data_set_err(out, "server is down");
        return;
    }
    if ( param_count(params) != 2 ||
         param_type(params, 0) != do_lx_cstring ||
         param_type(params, 1) != do_lx_cstring
        ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    int cam_up;
    cam_up = !strcmp(param_value(params, 1).string, "start");
    video_set_cam_up(info->video, param_value(params, 0).string, cam_up);
}


void video_exec_command(int client_id, void *owner, int command, do_list_t *params, do_data_t *data, do_data_t *out)
{
    video_info_t *video_info = owner;
    switch (command) {
        case DC_CMD_CHANGE_STATUS:
            return change_status(video_info, params, out);
        default:
            do_data_set_errf(out, "undefine command %d", command);
    }
}
