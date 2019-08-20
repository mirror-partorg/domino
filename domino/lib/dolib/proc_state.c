#include <stdio.h>
#include <dolib.h>
#include "../misc/define.h"


struct _do_proc_state_t{
    do_state_t state;
    int percent;
    do_proc_state_callback_t *change_state;
    void *change_state_arg;
    do_proc_state_callback_t *change_percent;
    void *change_percent_arg;
} ;

static const char *proc_stat_str[DO_STATE_UNDEFINE] = {
    "stop", "run", "pause", "stoping", "starting", "pausing", "resuming", "restarting"};

DO_EXPORT do_proc_state_t *do_proc_state_new()
{
    do_proc_state_t *do_proc_state = (do_proc_state_t*)do_malloc(sizeof(do_proc_state_t));
    do_proc_state->percent = 0;
    do_proc_state->state = DO_STATE_UNDEFINE;
    do_proc_state->change_state = NULL;
    do_proc_state->change_percent = NULL;
    return do_proc_state;
}
DO_EXPORT do_state_t do_proc_state_get_state(do_proc_state_t *do_proc_state)
{
    return do_proc_state->state;
}
DO_EXPORT void  do_proc_state_set_state(do_proc_state_t *do_proc_state, do_state_t state)
{
    do_proc_state->state = state;
    if (do_proc_state->change_state)
        (*do_proc_state->change_state)(do_proc_state, do_proc_state->change_state_arg);
}
DO_EXPORT int do_proc_state_get_persent(do_proc_state_t *do_proc_state)
{
    return do_proc_state->percent;
}
DO_EXPORT void do_proc_state_set_persent(do_proc_state_t *do_proc_state, int percent)
{
    do_proc_state->percent = percent;
    if (do_proc_state->change_percent)
        (*do_proc_state->change_percent)(do_proc_state, do_proc_state->change_percent_arg);
}
DO_EXPORT void do_proc_state_free(do_proc_state_t *do_proc_state)
{
    do_free(do_proc_state);
}
DO_EXPORT const char *do_proc_state_get_name(do_state_t state)
{
    return proc_stat_str[(int)state];
}
DO_EXPORT void  do_proc_state_set_callbackdo_state(do_proc_state_t *do_proc_state, do_proc_state_callback_t *change_state, void *arg)
{
    do_proc_state->change_state = change_state;
    do_proc_state->change_state_arg = arg;
}
DO_EXPORT void  do_proc_state_set_callback_percent(do_proc_state_t *do_proc_state, do_proc_state_callback_t *change_percent, void *arg)
{
    do_proc_state->change_percent = change_percent;
    do_proc_state->change_percent_arg = arg;
}
