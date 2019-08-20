#include "mproc_state.h"


static do_proc_state_t *proc_state = NULL;

inline void do_mproc_state_new()
{
    proc_state = do_proc_state_new();
}
inline do_proc_state_t *do_mproc_state_get()
{
    return proc_state;
}
inline void do_mproc_state_free()
{
    do_proc_state_free(proc_state);
}
inline do_state_t do_mproc_state_get_state()
{
    return do_proc_state_get_state(proc_state);
}
inline void do_mproc_state_set_state(do_state_t state)
{
    do_proc_state_set_state(proc_state, state);
}

