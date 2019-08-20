
#include <dolib.h>
#include "../../misc/define.h"

#ifdef _WIN32
#include "win32/syslog.c"
#include <windows.h>
#else
#include <sys/syslog.h>
#ifndef HAVE_SYSLOG
void syslog(int level, const char *fmt, ...) {};
void openlog(const char *name, int flags, ...) {};
void closelog() {}
#endif
#endif

#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define _do_log_unknown "unknown"
#define MAX_LOG_LINE (1024)
#define MAX_LEVEL_HISTORY LOG_DEBUG + 1

typedef struct do_log_st{
    do_log_type_t type;
    FILE         *file;
    do_log_func   func;
    void         *user_data;
} do_log_t;

#define ZONE __FILE__,__LINE__
#define MAX_DEBUG 8192

static const char *_do_log_level[] =
{
    "emergency",
    "alert",
    "critical",
    "error",
    "warning",
    "notice",
    "info",
    "debug",
};
static struct do_log_st _log = {DO_LOG_TYPE_SYSLOG, NULL, NULL, NULL};

DO_EXPORT void do_log_set_log_func(do_log_func func, void *user_data)
{
    _log.func = func;
    _log.user_data = user_data;
}

DO_EXPORT void do_log_set_type(const char *ident, do_log_type_t type, const char* filename)
{
    _log.type = type;
    if (_log.file && _log.file != stdout) {
        fclose(_log.file);
        _log.file = NULL;
    }
    switch (type) {
        case DO_LOG_TYPE_SYSLOG:
            openlog(ident, LOG_PID, LOG_LOCAL0);
            break;
        case DO_LOG_TYPE_STDOUT:
            _log.file = stdout;
            break;
        case DO_LOG_TYPE_FILE:
            _log.file = fopen(filename, "a+");
            if(_log.file == NULL) {
                fprintf(stderr,
                    "ERROR: couldn't open logfile: %s\n"
                    "       logging will go to stdout instead\n", strerror(errno));
                _log.type = DO_LOG_TYPE_STDOUT;
                _log.file = stdout;
            }
            break;
    }
}
DO_EXPORT void do_log(int level, const char *msgfmt, ...)
{
    va_list ap;
    char *pos, message[MAX_LOG_LINE+1];
    int sz, len = 0;
    time_t t;
    if (_log.func) {
        va_start(ap, msgfmt);
        len = vsnprintf(message, MAX_LOG_LINE, msgfmt, ap);
        if (len > MAX_LOG_LINE)
            message[MAX_LOG_LINE] = '\0';
        else
            message[len] = '\0';
        _log.func(message, level, _log.user_data);
        va_end(ap);
    }
    if(_log.type == DO_LOG_TYPE_SYSLOG) {
        va_start(ap, msgfmt);
#ifdef HAVE_SYSLOG
        vsyslog(level, msgfmt, ap);
#else
        len = vsnprintf(message, MAX_LOG_LINE, msgfmt, ap);
        if (len > MAX_LOG_LINE)
            message[MAX_LOG_LINE] = '\0';
        else
            message[len] = '\0';
        syslog(level, "%s", message);
#endif
        va_end(ap);

#ifndef DEBUG
        return;
#endif
    }
    if (level == LOG_ERR) {
        message[len] = '\0';
    }
    /* timestamp */
    t = time(NULL);
    pos = ctime(&t);
    sz = strlen(pos);
    /* chop off the \n */
    pos[sz-1]=' ';

    /* insert the header */
    if (level >= MAX_LEVEL_HISTORY)
        len = snprintf(message, MAX_LOG_LINE, "%s[%s]", pos, _do_log_unknown);
    else
        len = snprintf(message, MAX_LOG_LINE, "%s[%s] ", pos, _do_log_level[level]);
    if (len > MAX_LOG_LINE)
        message[MAX_LOG_LINE] = '\0';
    else
        message[len] = '\0';

    /* find the end and attach the rest of the msg */
    for (pos = message; *pos != '\0'; pos++); /*empty statement */
    sz = pos - message;
    va_start(ap, msgfmt);
    vsnprintf(pos, MAX_LOG_LINE - sz, msgfmt, ap);
    va_end(ap);
    if (_log.file) {
        char *bufu = message;
        fprintf(_log.file, "%s", bufu);
        fprintf(_log.file, "\n");
        fflush(_log.file);
        if (bufu != message)
            free(bufu);
    }

#ifdef DEBUG
    if (_log.type != DO_LOG_TYPE_STDOUT) {
#ifdef _WIN32

        CharToOem(message, message);
#endif
        fprintf(stdout, "%s\n", message);
        fflush(stdout);
    }
#endif /*DEBUG*/
}

#ifdef DEBUG
/** debug logging */
DO_EXPORT void do_log_debug(const char *file, int line, const char *msgfmt, ...)
{
    va_list ap;
    char *pos, message[MAX_DEBUG];
    int sz;
    time_t t;

    /* timestamp */
    t = time(NULL);
    pos = ctime(&t);
    sz = strlen(pos);
    /* chop off the \n */
    pos[sz-1]=' ';

    /* insert the header */
    snprintf(message, MAX_DEBUG, "%s%s:%d ", pos, file, line);

    /* find the end and attach the rest of the msg */
    for (pos = message; *pos != '\0'; pos++); /*empty statement */
    sz = pos - message;
    va_start(ap, msgfmt);
    vsnprintf(pos, MAX_DEBUG - sz, msgfmt, ap);
    va_end(ap);
    do_log(LOG_DEBUG, message);
}

#else /* DEBUG */
void  debug_log(const char *file, int line, const char *msgfmt, ...)
{ }
#endif
