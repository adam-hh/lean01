#include "apeu.h"
#include "sioUtl.h"

/*************************************************************
 * The Sio (Signal-safe I/O) package - simple reentrant output
 * functions that are safe for signal handlers.
 *************************************************************/

/* Private sio functions */
/* $begin sioprivate */

/*Output device fd, init it to STDOUT_FILENO*/
static sig_atomic_t wDevice = STDOUT_FILENO;

/**
 * strlen sio edition
 */
static size_t sio_strlen(const char *s)
{
    int i = 0;

    while (s[i] != '\0')
        ++i;
    return i;
}

/**
 * strncpy sio edition
 */
static char *sio_strncpy(char *dst, const char *src, size_t len)
{
    size_t ss, smin;

    ss = sio_strlen(src);
    smin = min(len, ss);
    for(int i = 0; i < smin; i++) {
        dst[i] = src[i];
    }
    dst[smin] = 0;
    return dst;
}

/**
 * sio_reverse - Reverse a string (from K&R) 
 */
static void sio_reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = sio_strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/**
 * sio_ltoa - Convert long to base b string (from K&R) 
 */
static void sio_ltoa(long v, char s[], int b) 
{
    int c, i = 0;
    int neg = (v < 0);

    if (neg)
	v = -v;

    do {  
        s[i++] = ((c = (v % b)) < 10)  ?  c + '0' : c - 10 + 'a';
    } while ((v /= b) > 0);

    if (neg)
	    s[i++] = '-';
    s[i] = '\0';
    sio_reverse(s);
}
/* $end sioprivate */

/* Public Sio functions */
/* $begin siopublic */

/**
 * Put char c to wDevice
 */
ssize_t sio_putchar(char c)
{
    char cval;

    cval = c;
    return write(wDevice, &cval, 1);
}

/**
 * Put string s to wDevice
 */
ssize_t sio_puts(const char *s)
{
    return write(wDevice, s, sio_strlen(s));
}

/**
 * Put long v to wDevice
 */
ssize_t sio_putl(long v)
{
    char s[26];
    
    sio_ltoa(v, s, 10);
    return sio_puts(s);
}

/**
 * Put error message s to wDevice then exit
 */
void sio_err_exit(const char *s)
{
    sio_puts(s);
    _exit(1);   /*sio exit*/
}
/* $end siopublic */

/*******************************
 * Wrappers for the SIO routines
 ******************************/

/**
 * Modify wDevice, which is inited to STDOUT_FILENO
 * input:   newfd
 * output:  none
 * return:  oldfd
 */
int Sio_wDevice(int newfd)
{
    int oldfd;

    oldfd = wDevice;
    wDevice = newfd;
    return oldfd;
}

/**
 * Wrapper of sio_putchar
 */
ssize_t Sio_putchar(char c)
{
    ssize_t n;

    if((n = sio_putchar(c)) < 0)
        sio_err_exit("Sio_putchar error");
    return n;
}

/**
 * Wrapper of sio_puts
 */
ssize_t Sio_puts(const char *s)
{
    ssize_t n;
  
    if ((n = sio_puts(s)) < 0)
	    sio_err_exit("Sio_puts error");
    return n;
}

/**
 * Wrapper of sio_putl
 */
ssize_t Sio_putl(long v)
{
    ssize_t n;
  
    if ((n = sio_putl(v)) < 0)
	    sio_err_exit("Sio_putl error");
    return n;
}

/**
 * Wrapper of sio_err_exit
 * Not neccesary, just padding
 */
void Sio_err_exit(const char *s)
{
    sio_err_exit(s);
}

/**
 * Format print, support %l and %s format
 */
ssize_t Sio_printf(const char *fmt, ...)
{
    va_list ap;
    const char *pf;
    long lval;
    char *sval;
    ssize_t rlt;

    va_start(ap, fmt);
    rlt = 0;
    for(pf = fmt; *pf; pf++) {
        if(*pf != '%') {
            rlt += Sio_putchar(*pf);
            continue;
        }
        switch(*++pf) {
        case 'l' :
            lval = va_arg(ap, long);
            rlt += Sio_putl(lval);
            break;
        case 's' :
            sval = va_arg(ap, char *);
            rlt += Sio_puts(sval);
            break;
        default :
            rlt += Sio_putchar(*pf);
            break;
        }
    }
    va_end(ap);
    return rlt;
}

/**
 * Format string, support %l and %s format
 */
ssize_t Sio_snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list ap;
    const char *pf;
    long lval;
    char *sval;
    char ltoa[26];
    size_t szltoa;
    ssize_t rlt;

    va_start(ap, fmt);
    rlt = 0;
    pf = fmt;
    for(rlt = 0; (rlt < size - 1) && (*pf); ) {
        if(*pf != '%') {
            buf[rlt++] = *pf++;
            continue;
        }
        switch(*++pf) {
        case 'l' :
            lval = va_arg(ap, long);
            sio_ltoa(lval, ltoa, 10);
            szltoa = sio_strlen(ltoa);
            if(szltoa + rlt >= size) {
                buf[0] = 0;
                return -1;
            }
            sio_strncpy(buf + rlt, ltoa, szltoa);
            rlt += szltoa;
            pf++;
            break;
        case 's' :
            sval = va_arg(ap, char *);
            szltoa = sio_strlen(sval);
            if(szltoa + rlt >= size) {
                buf[0] = 0;
                return -1;
            }
            sio_strncpy(buf + rlt, sval, szltoa);
            rlt += szltoa;
            pf++;
            break;
        default :
            buf[rlt++] = *pf++;
            break;
        }
    }
    va_end(ap);

    buf[rlt] = 0;
    if(rlt)
        return rlt - 1;
    return 0;
}