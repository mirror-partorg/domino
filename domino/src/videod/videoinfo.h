#ifndef _DLCUBINFO_H_
#define _DLCUBINFO_H_

#include <dolib.h>
#include "option.h"
#include "video.h"

typedef struct _video_info_t video_info_t;

video_info_t *video_info_new();
void video_info_free(video_info_t *video_info);
void video_info_set(video_info_t *video_info, video_t *video);

void video_exec_command(int client_id, void *owner, int command, do_list_t *params, do_data_t *data, do_data_t *ctrl_data_out);
do_rpc_command_t *video_info_get_cmd();
int video_info_get_cmd_count();

#endif // _DLCUBINFO_H_
