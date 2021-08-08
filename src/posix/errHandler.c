#include "apeu.h"
#include <errno.h>
#include <stdarg.h>
#include <syslog.h>

static void err_doit(int, int, const char *, va_list);
static void log_doit(int, int, int, const char *, va_list);

/**
 * Caller must define and set this: 
 * Zero if daemon
 * Nonzero if interactive
 */
sig_atomic_t log_to_stderr;

/**
 * Nonfatal error related to a system call.
 * print a message and return.
 */
void err_ret(const char *fmt, ...)
{
    va_list ap;
    
    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
}

/**
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void err_sys(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
    exit(1);
}

/**
 * Nonfatal error unrelated to a system call.
 * Error code passed as explict parameter.
 * Print a message and return
 */
void err_cont(int error, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    err_doit(1, error, fmt, ap);
    va_end(ap);
}

/**
 * Fatal error unrelated system cal.
 * Error code passed as explict parameter.
 * Print a message and terminate.
 */
void err_exit(int error, const char *fmt, ...)
{
    va_list ap;
    
    va_start(ap, fmt);
    err_doit(1, error, fmt ,ap);
    va_end(ap);
    exit(1);
}

/**
 * Fatal error related to  a system cal.
 * Print a message, dump core(by abort()), and terminate.
 */
void err_dump(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
    abort();                        /*may dump core*/
    exit(1);                        /*should not get here*/
}

/**
 * Nonfata error unrelated to a system call.
 * Print a message and return.
 */
void err_msg(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
}

/**
 * Fatal error unrelated to a system cal.
 * Print a message and terminate
 */
void err_quit(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
    exit(1);
}

/**
 * Routine to print error message to stderr
 * input:   errnoflag, error, fmt, ap
 * output:  none
 * return:  void
 */
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
    char buf[MAXLINE];

    vsnprintf(buf, MAXLINE - 1, fmt , ap);
    if(errnoflag)
        snprintf(buf + strlen(buf), MAXLINE - strlen(buf) - 1, ": %s", strerror(error));
    strcat(buf, "\n");
    fflush(stdout);
    fputs(buf, stderr);
}

/**
 * Print process exit status to stdout,
 * status parameter from waitpid func or system func
 */
void pr_exit(int status)
{
    if(WIFEXITED(status))
        printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
    else if(WIFSIGNALED(status))
        printf("abnormal termination, signal number = %d%s\n", WTERMSIG(status),
        #ifdef WCOREDUMP
        WCOREDUMP(status) ? " (core file generated)" : " (no core dump)");
        #else
            "");
        #endif
    else if(WIFSTOPPED(status))
        printf("child stopped, signal number = %d\n", WSTOPSIG(status));
        #ifdef WIFCONTINUED
    else if(WIFCONTINUED(status))
        printf("child continued, status = %d\n", status);
    else
        printf("unknown status %d", status);
        #endif
}

/**
 * Log process exit status to syslog
 * status parameter from waitpid func or system func
 */
void syslog_exit(int status, int priority)
{
    if(WIFEXITED(status))
        syslog(priority, "normal termination, exit status = %d\n", WEXITSTATUS(status));
    else if(WIFSIGNALED(status))
        syslog(priority, "abnormal termination, signal number = %d%s\n", WTERMSIG(status),
        #ifdef WCOREDUMP
        WCOREDUMP(status) ? " (core file generated)" : " (no core dump)");
        #else
            "");
        #endif
    else if(WIFSTOPPED(status))
        syslog(priority, "child stopped, signal number = %d\n", WSTOPSIG(status));
        #ifdef WIFCONTINUED
    else if(WIFCONTINUED(status))
        syslog(priority, "child continued, status = %d\n", status);
    else
        syslog(priority, "unknown status %d", status);
        #endif
}

/**
 * Initialize syslog(), if running as daemon.
 */
void log_open(const char *ident, int option, int facility)
{
    if(log_to_stderr == 0)
        openlog(ident, option, facility);
}

/**
 * Nofatal error related to a system call.
 * Print a message with the system's errno value and return
 */
void log_ret(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_doit(1, errno, LOG_ERR, fmt, ap);
    va_end(ap);
}

/**
 * Fatal error related to a system call
 * Print a message and terminate.
 */
void log_sys(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_doit(1, errno, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(2);
}

/**
 * Nonfatal error unrelated to a system call
 * Print a message and return.
 */
void log_msg(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_doit(0, 0, LOG_ERR, fmt, ap);
    va_end(ap);
}

/**
 * Fatal error unrelated to a system call
 * Print a message and terminate.
 */
void log_quit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_doit(0, 0, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(2);
}

/**
 * Fatal error related to a system call
 * Error number passed as an explicti parameter.
 * Print a message and terminate
 */
void log_exit(int error, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_doit(1, error, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(2);
}

/**
 * Routine to print error message to stderr or syslog(by global flag log_to_stderr)
 * global input: int log_to_stderr
 * input:   errnoflag, error, fmt, ap
 * output:  none
 * return:  void
 */
static void log_doit(int errorflag, int error, int priority, const char *fmt, va_list ap)
{
    char buf[MAXLINE];

    vsnprintf(buf, MAXLINE - 1, fmt, ap);
    if(errorflag)
        snprintf(buf + strlen(buf), MAXLINE - strlen(buf) - 1, ": %s", strerror(error));
    strcat(buf, "\n");

    if (log_to_stderr) {
        fflush(stdout);
        fputs(buf, stderr);
        fflush(stderr);
    } else {
        syslog(priority, "%s", buf);
    }
}

/* Wrapper of malloc with err exit */
void *Malloc(size_t sz)
{
    void *ptr;

    if ((ptr = malloc(sz)) == NULL)
        err_sys("fatal error: malloc error");
    return ptr;
}
/* Wrapper of calloc with err exit */
void *Calloc(size_t cnt, size_t sz)
{
    void *ptr;

    if ((ptr = calloc(cnt, sz)) == NULL)
        err_sys("fatal error: calloc error");
    return ptr;
}
/* Wrapper of realloc with err exit */
void *Realloc(void *ptr, size_t sz)
{
    void *p;

    if ((p = realloc(ptr, sz)) == NULL)
        err_sys("fatal error: realloc error");
    return p;
}