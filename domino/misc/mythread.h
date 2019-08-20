#ifndef MYTHREAD_H_INCLUDED
#define MYTHREAD_H_INCLUDED


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
typedef void (mythread_proc_t)(void *);

typedef struct {
    thread_handle_t handle;
    mythread_proc_t *run_proc;
    void* run_arg;
    int running;
} mythread_t;

typedef struct _mymutex_t mymutex_t;

#ifdef __cplusplus
extern "C"
{
#endif

mythread_t *mythread_new(mythread_proc_t *run_proc, void *run_arg);
void  mythread_free(mythread_t *thread);
void  mythread_exit();
int mythread_is_running(mythread_t *thread);


mymutex_t *mymutex_new();
void       mymutex_free(mymutex_t *);
void       mymutex_lock(mymutex_t *);
void       mymutex_unlock(mymutex_t *);

#ifdef __cplusplus
}
#endif

#endif // MYTHREAD_H_INCLUDED
