#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED


#if !defined(_WIN32) && !defined(_LINUX)
#error You must define one of the following: _WIN32, _LINUX
#endif

#ifdef _WIN32
#include <windows.h>
#include <process.h>
typedef HANDLE thread_handle_t;
#elif defined(_LINUX)
#include <pthread.h>
typedef pthread_t thread_handle_t;
#endif
typedef void (thread_proc_t)(void *);

typedef struct {
    thread_handle_t handle;
    thread_proc_t *run_proc;
    void* run_arg;
    int running;
}thread_t;

#ifdef __cplusplus
extern "C"
{
#endif

int thread_new(thread_proc_t *run_proc, void *run_arg, thread_t **thread);
void  thread_free(thread_t *thread);
void  thread_exit();
int thread_is_running(thread_t *thread);

#ifdef __cplusplus
}
#endif

#endif // THREAD_H_INCLUDED
