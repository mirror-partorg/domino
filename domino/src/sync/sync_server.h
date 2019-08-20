#ifndef _SYNC_SERVER_H
#define _SYNC_SERVER_H

#include "sync_info.h"
#include "option.h"
#include <domino.h>

#define NAME "syncd"


typedef struct {
    option_t *opt;
    sync_info_t *sync_info;
    do_alias_t *alias;
} sync_server_t;


#ifdef __cplusplus
extern "C"
{
#endif

sync_server_t *sync_server_new(option_t *opt, sync_info_t *sync_info, do_alias_t* alias);
void sync_server_free(sync_server_t *sync);
void sync_server_run(sync_server_t *sync);
void sync_server_check_config(option_t *opt);

#endif
