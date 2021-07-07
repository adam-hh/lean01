#include "littlePrime.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int     fdrandom;
    u64     randomu64;
    PTable  pt;
    PFtable pf;

    fdrandom = open("/dev/urandom", O_RDONLY);
    read(fdrandom, &randomu64, sizeof(u64) - 2);
    
    pt = createpTable(100000000);
    /*
    if (pt.size) {
        printf("Full table of %lu prime numbers(from 2 to %llu):\n", pt.size, pt.ptable[pt.size - 1]);
        //prPTable(&pt);
    }
    */

    pf = primeFact(randomu64, &pt);
    printf("%llu factorization:\n", randomu64);
    prpfTable(&pf);

    free(pf.pftable);
    free(pt.ptable);
    exit(0);
}
