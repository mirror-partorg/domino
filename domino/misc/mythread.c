#include <dolib.h>
#include <errno.h>
#include "mythread.h"

#ifdef _WIN32
unsigned int __stdcall thread_run(void* arg);
static CRITICAL_SECTION *alloc_cs = NULL;
#elif defined(_LINUX)
void* thread_run(void* arg);
//static pthread_mutex_t  alloc_mutex;
#endif

struct _mymutex_t {
#ifdef _WIN32
    CRITICAL_SECTION *alloc_cs;
#elif defined(_LINUX)
    pthread_mutex_t  alloc_mutex;
#endif
};

mythread_t *mythread_new(mythread_proc_t *run_proc, void *run_arg)
{
    mythread_t *thread = (mythread_t*)do_malloc(sizeof(mythread_t));
    thread->running = 1;
    thread->run_proc = run_proc;
    thread->run_arg = run_arg;
#ifdef _WIN32
    if (!alloc_cs) {
        alloc_cs = malloc(sizeof(CRITICAL_SECTION));
        InitializeCriticalSection(alloc_cs);
    }
    EnterCriticalSection(alloc_cs);
    if (NULL == (thread->handle = (thread_handle_t) _beginthreadex(NULL, 0, thread_run, thread, CREATE_SUSPENDED, NULL))) {
        do_log(LOG_ERR, "Failed to create thread");
        thread->running = 0;
        do_free(thread);
        thread = NULL;
        return NULL;
    }
    if (!SetThreadPriority(thread->handle, GetThreadPriority(GetCurrentThread()))) {
        do_log(LOG_ERR, "Failed to set thread priority");
    }
    if (ResumeThread(thread->handle) == (DWORD) -1) {
        do_log(LOG_ERR, "Failed to resume new thread");
        do_free(thread);
        thread = NULL;
        return thread;
    }
    LeaveCriticalSection(alloc_cs);
#elif defined(_LINUX)
    pthread_attr_t attr;
    pthread_attr_init(&attr);
//    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
    int code =
        pthread_create(&thread->handle, &attr, thread_run, thread);
    if (code == EPERM) {
        pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
        code =
            pthread_create(&thread->handle, &attr, thread_run, thread);
    }
    pthread_attr_destroy(&attr);
    if (code) {
        do_log(LOG_ERR, "Thread creation error: %s", strerror(errno));
        thread->running = 0;
        do_free(thread);
        thread = NULL;
        return NULL;
    }
#endif
    return thread;
}
#ifdef _WIN32
unsigned int __stdcall thread_run(void* arg)
#elif defined(_LINUX)
void* thread_run(void* arg)
#endif
{
    mythread_t *thread  = arg;
    (*thread->run_proc)(thread->run_arg);
    thread->running = 0;
    return 0;
}

void  mythread_free(mythread_t *thread)
{
    do_free(thread);
//    if (thread->running) {
#ifdef _WIN32
//        TerminateThread(thread->handle, 0);
#elif defined(_LINUX)
//if (pthread_cancel(thread->handle))
//            do_log(LOG_ERR, "Thread termination error: %s", strerror(errno));
#endif
//    }
}
void  mythread_exit()
{
#ifdef _WIN32
#elif defined(_LINUX)
    pthread_exit(NULL);
#endif
}
int mythread_is_running(mythread_t *thread)
{
    return thread->running;
}
mymutex_t *mymutex_new()
{
    mymutex_t *retval;
    retval = do_malloc0(sizeof(mymutex_t));
#ifdef _WIN32
    retval->alloc_cs = malloc(sizeof(CRITICAL_SECTION));
    InitializeCriticalSection(retval->alloc_cs);
#else
    pthread_mutex_init(&retval->alloc_mutex, NULL);

#endif
    return retval;
}

void mymutex_free(mymutex_t *mutex)
{
#ifdef _WIN32
    DeleteCriticalSection(mutex->alloc_cs);
#else
    pthread_mutex_destroy(&mutex->alloc_mutex);
#endif
    do_free(mutex);
}
void mymutex_lock(mymutex_t *mutex)
{
#ifdef _WIN32
    EnterCriticalSection(mutex->alloc_cs);
#else
    pthread_mutex_lock(&mutex->alloc_mutex);
#endif
}

void mymutex_unlock(mymutex_t *mutex)
{
#ifdef _WIN32
    LeaveCriticalSection(mutex->alloc_cs);
#else
    pthread_mutex_unlock(&mutex->alloc_mutex);
#endif
}
