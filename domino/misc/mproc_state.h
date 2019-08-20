#ifndef MPROC_STATE_H_INCLUDED
#define MPROC_STATE_H_INCLUDED

#include <dolib.h>

#ifdef __cplusplus
extern "C"
{
#endif

void do_mproc_state_new();
do_proc_state_t *do_mproc_state_get();
do_state_t do_mproc_state_get_state();
void do_mproc_state_set_state(do_state_t state);
void do_mproc_state_free();

#ifdef __cplusplus
}
#endif


#endif // MPROC_STATE_H_INCLUDED
