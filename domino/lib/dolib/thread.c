#include <dolib.h>
#include <errno.h>
#include "thread.h"

#ifdef _WIN32
unsigned int __stdcall thread_run(void* arg);
static CRITICAL_SECTION *alloc_cs = NULL;
#elif defined(_LINUX)
void* thread_run(void* arg);
static pthread_mutex_t  alloc_mutex;
#endif

int thread_new(thread_proc_t *run_proc, void *run_arg, thread_t **th)
{
    thread_t *thread = (thread_t*)do_malloc(sizeof(thread_t));
    *th = thread;
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
        return 0;
    }
    if (!SetThreadPriority(thread->handle, GetThreadPriority(GetCurrentThread()))) {
        do_log(LOG_ERR, "Failed to set thread priority");
    }
    if (ResumeThread(thread->handle) == (DWORD) -1) {
        do_log(LOG_ERR, "Failed to resume new thread");
	do_free(thread);
	thread = NULL;
	return 0;
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
	return 0;
    }
#endif
    return 1;
}
#ifdef _WIN32
unsigned int __stdcall thread_run(void* arg)
#elif defined(_LINUX)
void* thread_run(void* arg)
#endif
{
    thread_t *thread  = arg;
    (*thread->run_proc)(thread->run_arg);
    thread->running = 0;
    return 0;
}

void  thread_free(thread_t *thread)
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
void  thread_exit()
{
#ifdef _WIN32
#elif defined(_LINUX)
    pthread_exit(NULL);
#endif
}
int thread_is_running(thread_t *thread)
{
    return thread->running;
}
