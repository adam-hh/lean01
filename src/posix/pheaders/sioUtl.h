/*************************************************************
 * The Sio (Signal-safe I/O) package - simple reentrant output
 * functions that are safe for signal handlers.
 *************************************************************/
#ifndef SIOUTIL_H
#define SIOUTIL_H

#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>

#ifdef __cpluscplus
extern "C" {
#endif

extern int Sio_wDevice(int newfd);
extern ssize_t Sio_putchar(char c);
extern ssize_t Sio_puts(const char *s);
extern ssize_t Sio_putl(long v);
extern void Sio_err_exit(const char *s);

extern ssize_t Sio_printf(const char *fmt, ...);
extern ssize_t Sio_snprintf(char *buf, size_t size, const char *fmt, ...);


#ifdef __cpluscplus
}
#endif

#endif  /*SIOUTIL_H*/