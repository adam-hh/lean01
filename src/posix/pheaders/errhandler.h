#ifndef ERRHANDLER_H
#define ERRHANDLER_H
#include <signal.h>
#ifdef __cpluscplus
extern "C" {
#endif

void err_ret(const char *, ...);            /* Appendix B */
void err_sys(const char *, ...);
void err_msg(const char *, ...); 
void err_quit(const char *, ...);
void err_dump(const char *, ...);
void err_cont(int, const char *, ...);
void err_exit(int, const char *, ...); 

extern sig_atomic_t log_to_stderr;          /* Appendix B */
void log_open(const char *, int, int);
void log_ret(const char *, ...);
void log_sys(const char *, ...);
void log_msg(const char *, ...);
void log_quit(const char *, ...);
void log_exit(int error, const char *fmt, ...);

void *Malloc(size_t sz);
void *Calloc(size_t cnt, size_t sz);
void *Realloc(void *ptr, size_t sz);


#ifdef __cpluscplus
}
#endif

#endif    /* ERRHANDLER_H */