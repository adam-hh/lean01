/**
 * stevens'book unix program 3rd header
 */
#ifndef _APUE_H     
#define _APUE_H     

#define Sigfunc oldSigfunc          /* signal prototype */
#include <sys/types.h>              /* some systems still require this */
#include <sys/stat.h>     
#include <sys/termios.h>            /* for winsize */ 

#if defined(MACOS) || !defined(TIOCGWINSZ)
#include <sys/ioctl.h>     
#endif     

#include <stdio.h>                  /* for convenience */
#include <stdlib.h>                 /* for convenience */
#include <stddef.h>                 /* for offsetof */
#include <string.h>                 /* for convenience */
#include <unistd.h>                 /* for convenience */
#include <fcntl.h>                  /* for convenience */
#include <errno.h>                  /* for convenience */
#include <signal.h>                 /* for SIG_ERR */

#ifdef __cpluscplus
extern "C" {
#endif

#define _POSIX_C_SOURCE 200809L

#if defined(SOLARIS)                /* Solaris 10*/
#define _XOPEN_SOURCE   600       
#else
#define _XOPEN_SOURCE   700         /* Single UNIX Specification, Version 3 */
#endif

#ifdef __APPLE_CC__                 /* Apple and BSD style sigaction param */
#define _sigaction_sa_handler(sa) sa.__sigaction_u.__sa_handler
#define _sigaction_sa_sigaction(sa) sa.__sigaction_u.__sa_sigaction
#else
#define _sigaction_sa_handler(sa) sa.sa_handler
#define _sigaction_sa_sigaction(sa) sa.sa_sigaction
#endif

#define MAXLINE 4096                /* max line length */

/**
 * Default file access permissions for new files. 
 */  
#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)     
    
/**
 * Default permissions for new directories.  
 */    
#define DIR_MODE    (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)     

/*make alias Sigfunc of signal function prototype*/
#undef Sigfunc
typedef void Sigfunc(int);   
    
#if defined(SIG_IGN) && !defined(SIG_ERR)     
#define SIG_ERR ((Sigfunc *)-1)     
#endif     
    
#define min(a,b)     ((a) < (b) ? (a) : (b))     
#define max(a,b)     ((a) > (b) ? (a) : (b))     
    
/**
 * Prototypes for our own functions.  
 */
   
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

char *path_alloc(size_t *);                 /* Figure 2.15 */    
long open_max(void);                        /* Figure 2.16 */

int set_cloexec(int);                       /* Fiture 13.9*/
int clr_cloexec(int fd);                    /* Extra */
void clr_fl(int, int);                      /* Figure 3.11 */
void set_fl(int, int);                      /* Figure 3.11 */

void pr_exit(int);                          /* Figure 8.5 */
void syslog_exit(int status, int priority); /* Extra */

void pr_mask(const char *);                 /* Figure 10.14 */
Sigfunc *Signal_intr(int, Sigfunc *);       /* Figure 10.19 */
Sigfunc *Signal(int, Sigfunc *);            /* Extra origian signal wrapper */

pid_t Fork(void);                           /* for wrapper */
int Sigprocmask(int how, const sigset_t *restrict set, sigset_t *restrict oset);
                                            /* sigprocmask wrapper */
pid_t WaitErrRet(int *status);              /* wait wrapper */
pid_t WaitErrExit(int *status);             /* wait wrapper */
void WaitUntilECHILD(int *status);          /* waitpid wrapper */

time_t Sleep(time_t sec);                   /* sleep wrapper */
int Nanosleep(time_t tv_sec, long tv_nsec); /* nanosleep wrapper */
int prLongTime();                           /* Extra time util */
void busyWaitS(time_t sec);                 /* Extra time util */

void daemonize(const char *);               /* Figure 13.1 */

ssize_t readn(int, void *, size_t);         /* Figure 14.29 */
ssize_t writen(int, const void *, size_t);  /* Figure 14.29 */

int lock_reg(int, int, int, off_t, 
                int, off_t);                /* Figure 14.5 */
#define read_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))    
#define readw_lock(fd, offset, whence, len) lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))    
#define write_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))    
#define writew_lock(fd, offset, whence, len) lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))    
#define un_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))    
    
pid_t lock_test(int, int, off_t, 
                int, off_t);                /* Figure 14.6 */
#define is_read_lockable(fd, offset, whence, len) (lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define is_write_lockable(fd, offset, whence, len) (lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)
    
int fd_pipe(int *);                         /* Figures 17.6 and 17.13 */    
int recv_fd(int, ssize_t (*func)(int,    
            const void *, size_t));         /* Figures 17.21 and 17.23 */    
int send_fd(int, int);                      /* Figures 17.20 and 17.22 */    
int send_err(int, int,    
                const char *);              /* Figure 17.19 */    
int serv_listen(const char *);              /* Figures 17.10 and 17.15 */    
int serv_accept(int, uid_t *);              /* Figures 17.11 and 17.16 */    
    
int cli_conn(const char *);                 /* Figures 17.12 and 17.17 */    
int buf_args(char *, int (*func)(int,    
             char **));                     /* Figure 17.32 */    
int tty_cbreak(int);
int tty_raw(int);
int tty_reset(int);
void tty_atexit(void);
struct termios *tty_termios(void);

int tty_cbreak(int);                        /* Figure 18.20 */
int tty_raw(int);                           /* Figure 18.20 */
int tty_reset(int);                         /* Figure 18.20 */
void tty_atexit(void);                      /* Figure 18.20 */
#ifdef  ECHO                                /* only if <termios.h>  has been included */
struct termios  *tty_termios(void);         /* Figure 18.20 */
#endif

int ptym_open(char *, int);                 /* Figures 19.8, 19.9, and 19.10 */    
int ptys_open(char *);                      /* Figures 19.8, 19.9, and 19.10 */    
#ifdef  TIOCGWINSZ     
pid_t pty_fork(int *, char *, int, const struct termios *,    
                const struct winsize *);    /* Figure 19.11 */    
#endif        
    
#define is_read_lockable(fd, offset, whence, len) (lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)    
#define is_write_lockable(fd, offset, whence, len) (lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)    
    
void TELL_WAIT(void);                       /* parent/child from Section 8.9 */    
void TELL_PARENT(pid_t);    
void TELL_CHILD(pid_t);    
void WAIT_PARENT(void);    
void WAIT_CHILD(void);    

#ifdef __cpluscplus
{
#endif

#endif  /* _APUE_H */   