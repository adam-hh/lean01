#include "apeu.h"
#include "sioUtl.h"
#include "sighandlerUtl.h"

/* A map of signal value-name */
char *localSiglist[NSIG] = {NULL};

/* atomic flag for Sigsuspend usage, has a default value 0 as static data*/
static sig_atomic_t quitflag;

/**
 * init localSiglist
 */
void genLocalSiglist() 
{
    size_t n;

    if(localSiglist[0])                 /*already generated*/
        return;

    n = sizeof(localSiglist);           /*first time to generate*/
    while(n--) {
        localSiglist[n] = strsignal(n); /*i suppose strsignal return a address of static obj*/
    }
    localSiglist[0] = "Unkown signal: 0";
}

/**
 * Print current caller's signal mask
 * Almost signal safe func to be called in signal handler
 * input:   str(a hint, can be set to NULL)
 * output:  none
 * return:  void
 */
void pr_mask(const char *str)
{
    sigset_t sigset;
    int errno_save;
    int n;

    errno_save = errno;
    Sigprocmask(0, NULL, &sigset);

    genLocalSiglist();
    n = sizeof(localSiglist);

    if(str)
        Sio_printf("%s: ",str);
    while(n--) {
        if(sigismember(&sigset, n))
            Sio_printf("(%l)%s ",n, localSiglist[n]);
    }
    Sio_puts("\n");

    errno = errno_save;
}
/**
 * The signal handler to set Sigsuspend flag
 */
static void sig_quitsuspend(int signo)
{
    quitflag = 1;
}

/**
 * The original suspend/sigsuspend sometimes intterupted by non signal handler events
 * Sigsuspend hava the opposite mask behavior with original sigsuspend function
 * And sigsuspend will never interrupted by non signal handler events
 * opset is the opposite mask, only signals in opset can intterupt Sigsuspend
 * input:   opset
 * output:  none
 * return:  -1
 * side effect:
 * capture the specified signals in opset before Sigsuspend return
 * restore the them before Sigsuspend return
 */
int Sigsuspend(const sigset_t *opset)
{
    sigset_t mask;
    Sigfunc *oHandler[NSIG] = {NULL};
    int ret, n;

    memcpy(&mask, opset, sizeof(sigset_t));
    sigopposite(&mask);

    n = sizeof(oHandler);
    quitflag = 0;                                       /*init quitflag*/
    while(--n) {
        if(sigismember(opset, n)) 
            oHandler[n] = Signal(n, sig_quitsuspend);   /*capture signals in opset*/
    }
    while(!quitflag)
        ret = sigsuspend(&mask);

    n = sizeof(oHandler);
    quitflag = 0;                                       /*reset quitflag*/
    while(--n) {
        if(sigismember(opset, n))
            Signal(n, oHandler[n]);                     /*restore signals in opset*/
    }

    return ret;
}

/**
 * SIGCHLD handler "correct edition" from stevens' book
 */
void sigChld(int signo)
{
    pid_t   pid;
    int     stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        Sio_printf("child %l terminated\n", pid);
    return;
}