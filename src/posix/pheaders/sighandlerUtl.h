#ifndef SIGHANDLERUTIL_H
#define SIGHANDLERUTIL_H

#define Sigfunc oldSigfunc
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>


#ifdef __cpluscplus
extern "C" {
#endif

/*make alias Sigfunc of signal function prototype*/
#undef Sigfunc
typedef void  Sigfunc(int);

/*test and def NULL*/
#ifndef NULL
#define NULL (void *)0
#endif

/*test and def NSIG*/
#ifndef NSIG
#define NSIG 32
#endif

#define SIGBAD(signo) ((signal <= 0) ||(signal >= NSIG))


/**
 * Opposite the sigset
 * input:   set
 * output:  set
 * return:  0 for success, -1 for fail
 */
static int sigopposite(sigset_t *set)
{
    char *op;

    if(set == NULL)
        return -1;
    op = (char *)set;
    for(int i = 0; i < sizeof(sigset_t); i++) {
        *op = ~(*op);
        op++;
    }
    return 0;
}
/**
 * Since there is no proper POSIX rules to provide
 * a static value-name map for signals
 * I def a local static map here
 */

extern char *localSiglist[];
extern void genLocalSiglist();
extern void pr_mask(const char *str);

extern int Sigsuspend(const sigset_t *opset);
extern void sigChld(int signo);

#ifdef __cpluscplus
}
#endif

#endif  /*SIGHANDLERUTIL_H*/