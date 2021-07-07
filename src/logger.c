#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>

static struct tms tmsstart, tmsend;
static clock_t start, end;
#define MAXTIMESIZE 64

void loggerStart(FILE *fp)
{
    if((start = times(&tmsstart)) == -1) {
        fprintf(stderr, "times error\n");
        exit(1);
    }
    char buf[MAXTIMESIZE];
    time_t now;
    struct tm *nowtime;
    size_t n;

    time(&now);
    nowtime = localtime(&now);

    if((n = strftime(buf, MAXTIMESIZE, "task: %Y %b %d %a, %T %Z", nowtime)) == 0) {
        fprintf(stderr, "strftime error\n");
        exit(1);
    } else
        buf[n] = 0;

    fprintf(fp, "********************************\n");
    fprintf(fp, "%s\n", buf);
    fflush(fp);
}

void loggerEnd(FILE *fp)
{
    static long clktck = 0;
    if(clktck == 0)
        if((clktck = sysconf(_SC_CLK_TCK)) < 0) {
            fprintf(stderr, "sysconf error\n");
            exit(1);
        }
    
    if((end = times(&tmsend)) == -1) {
        fprintf(stderr, "times error\n");
        exit(1);
    }
    fprintf(fp, "  real:       %7.2f\n", (end - start) / (double)clktck);
    fprintf(fp, "  user:       %7.2f\n", (tmsend.tms_utime - tmsstart.tms_utime) / (double)clktck);
    fprintf(fp, "  sys:        %7.2f\n", (tmsend.tms_stime - tmsstart.tms_stime) / (double)clktck);
    fprintf(fp, "  child user: %7.2f\n", (tmsend.tms_cutime - tmsstart.tms_cutime) / (double)clktck);
    fprintf(fp, "  child sys:  %7.2f\n", (tmsend.tms_cstime - tmsstart.tms_cstime) / (double)clktck);
    fprintf(fp, "********************************\n\n");
    fflush(fp);
}