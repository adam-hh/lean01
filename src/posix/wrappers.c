#include "apeu.h"
#include <errno.h>

/**
 * fork wrapper
 */
pid_t Fork(void)
{
    pid_t pid;
    if((pid = fork()) < 0)
        err_sys("Fork error");
    return pid;
}

/**
 * A signal implement with sigaction
 * Try to restart all interrupted syscal except by SIGALRM
 */
Sigfunc *Signal(int signo, Sigfunc *func)
{
    struct sigaction act, oact;

    _sigaction_sa_handler(act) = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(signo == SIGALRM) {
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
        act.sa_flags |= SA_RESTART;
    }

    if(sigaction(signo, &act, &oact) < 0)
        return SIG_ERR;
    return _sigaction_sa_handler(oact);
}

/**
 * A signal implement with sigaction
 * Try to stop restart all interrupted syscal
 */
Sigfunc *Signal_intr(int signo, Sigfunc *func)
{
    struct sigaction act, oact;

    _sigaction_sa_handler(act) = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
#endif

    if(sigaction(signo, &act, &oact) < 0)
        return SIG_ERR;
    return _sigaction_sa_handler(oact);
}

/**
 * sigprocmast wrapper
 */
int Sigprocmask(int how, const sigset_t *restrict set, sigset_t *restrict oset)
{
    int n;
    if((n = sigprocmask(how, set, oset)) < 0)
        err_sys("sigprocmask error");
    return n;
}

/**
 * wait wrapper, print status to stdout if status not NULL
 * error return
 */
pid_t WaitErrRet(int *status)
{
    pid_t pid;

    pid = wait(status);
    if (pid >= 0) {
        if(status)
            pr_exit(*status);
        return pid;
    } else
        err_cont(errno, "wait err");

    return pid;
}

/**
 * wait wrapper, print status to stdout if status not NULL
 * error exit
 */
pid_t WaitErrExit(int *status)
{
    pid_t pid;

    pid = wait(status);
    if (pid >= 0) {
        if(status)
            pr_exit(*status);
        return pid;
    } else
        err_exit(errno, "wait err");

    return pid;
}

/**
 * Loop waitpid until all child process recycled
 * input:   none
 * output:  status
 * return:  void
 */
void WaitUntilECHILD(int *status)
{
    pid_t pid;

    while(1) {
        if ((pid = waitpid(-1, status, WUNTRACED | WCONTINUED)) > 0) {
            if (status) {
                printf("(pid:%d)", pid);
                pr_exit(*status);
                continue;
            }
        } else if (EINTR == errno) {
            err_cont(errno, "wait interruped, restarting...");
            continue;
        } else if (ECHILD == errno) {
            err_cont(errno, "wait finished");
            break;
        } else {
            err_cont(errno, "wait got unknown error, stop wait");
            break;
        }
    }
}

/**
 * This is not only a wrapper of system func
 * It is a an implement from stevens book which apply POSIX standar
 */
int System(const char *cmdstring)
{
    pid_t pid;
    int status;
    struct sigaction ignore, saveintr, savequit;
    sigset_t chldmask, savemask;

    if(!cmdstring)
        return 1;
#ifdef __APPLE_CC__
    ignore.__sigaction_u.__sa_handler = SIG_IGN;
#else
    ignore.sa_handler = SIG_IGN;
#endif
    sigemptyset(&ignore.sa_mask);
    ignore.sa_flags = 0;
    if(sigaction(SIGINT, &ignore, &saveintr) < 0)
        return -1;
    if(sigaction(SIGQUIT, &ignore, &saveintr) < 0)
        return -1;
    sigemptyset(&chldmask);
    sigaddset(&chldmask, SIGCHLD);
    if(sigprocmask(SIG_BLOCK, &chldmask, &savemask) < 0)
        return -1;

    if((pid = fork()) < 0)
        status = -1;                        /*probably out of processes*/
    else if(pid == 0) {                     /*child*/
        /*restore previous signal actions & reset signal mask */
        sigaction(SIGINT, &saveintr, NULL); /*should not work after execl call sucess*/
        sigaction(SIGQUIT, &savequit, NULL);
        Sigprocmask(SIG_SETMASK, &savemask, NULL);/*should work after execl call*/

        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127);                         /*exec error*/
    } else {                                /*parent*/
        while(waitpid(pid, &status, 0) < 0)
            if(errno != EINTR) {
                status = -1;                /*error other than EINTR from waitpid()*/
                break;
            }
    }

    /*restore previous signal actions & reset signal mask*/
    if(sigaction(SIGINT, &saveintr, NULL) < 0)
        return -1;
    if(sigaction(SIGQUIT, &savequit, NULL) < 0)
        return -1;
    if(sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
        return -1;
    return status;
}