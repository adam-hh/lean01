#include "littlePrime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/*max prime table size*/
#define MAXPTABLESIZE 100000000u

/*general permition bits to create file*/
#ifndef FILE_MODE
#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#endif

/**
 * Create a prime number table
 * input: ptablesize
 * output: none
 * return: a PTable obj pTable, the caller must consider free pTable.ptable
 */
PTable createpTable(size_t ptablesize)
{
    int fd;                                                 /*the file to save prime numbers*/
    struct stat  sta;                                       /*stat for fd*/
    PTable pTable;                                          /*prime table*/
    u64 sqrti;                                              /*sqrt root of i*/
    size_t cur;                                             /*cursor of pTable*/
    int tag;                                                /*a flag to mark prime or compsite*/

    pTable.size = ptablesize;                               /*init vars*/
    if (ptablesize > MAXPTABLESIZE)
        pTable.size = MAXPTABLESIZE;
    if (ptablesize < 1000)
        pTable.size = 1000;                                 /*let pTable.size between [1000, MAXPTABLESIZE]*/

    pTable.ptable = (u64 *)malloc(pTable.size * sizeof(u64));
    if (pTable.ptable == NULL) {
        pTable.size = 0;
        return pTable;                                      /*malloc error*/
    }

    fd = open("primetable", O_RDWR | O_CREAT, FILE_MODE);
    if (fd == -1) {
        fprintf(stderr, "open file primetable error\n");
        free(pTable.ptable);
        pTable.ptable = NULL;
        pTable.size = 0;
        return pTable;
    }
    fstat(fd, &sta);
    if (fstat(fd, &sta) == -1) {
        fprintf(stderr, "stat file primetable error\n");
        free(pTable.ptable);
        pTable.ptable = NULL;
        pTable.size = 0;
        return pTable;
    }

    if (sta.st_size >= pTable.size * sizeof(u64)) {
        if (pTable.size * sizeof(u64) != pread(fd, pTable.ptable, pTable.size * sizeof(u64), 0)) {
            fprintf(stderr, "read primetable file error");
            free(pTable.ptable);
            pTable.ptable = NULL;
            pTable.size = 0;
            return pTable;
        }
        close(fd);
        return pTable;
    } else
        ftruncate(fd, 0);

    pTable.ptable[0] = 2;
    pTable.ptable[1] = 3;
    cur = 2;                                                /*counter of pTable, start from 2*/
    tag = 1;
    for(u64 i = 5; ; i++)                                   /*i is a number to be test*/
    {
        if ((i & 0x01) == 0)
            continue;                                       /*skip even number*/
        tag = 1;
        sqrti = (u64)sqrt(i);
        for(size_t j = 1; pTable.ptable[j] <= sqrti; j++)
        {
            if(0 == (i % pTable.ptable[j])){
                tag = 0;                                    /*i is not a prime number*/
                break;
            }
        }
        if(tag)
        {
            pTable.ptable[cur++] = i;                       /*i is a prime number*/
            //write(STDOUT_FILENO, ".", 1);                   /*heart beats*/
        }
        if(cur == pTable.size) {                            /*break condition*/
            //write(STDOUT_FILENO, "\n", 1);
            break;
        }
    }

    if(pwrite(fd, pTable.ptable, pTable.size * sizeof(u64), 0) != pTable.size * sizeof(u64)){
        fprintf(stderr, "write primetable file error\n");
    }

    close(fd);
    return pTable;
}

/**
 * Factorization
 * input: x, ptable
 * output: none
 * return: a PFtable obj pfTable, the caller must consider free pfTable.pftable
 */
PFtable primeFact(u64 x, PTable *ptable)
{
    PFtable pfTable;                                        /*prime fact table*/
    u64 sqrtx;                                              /*sqrt root of x*/
    size_t i, j;                                            /*loop counters*/
    size_t pfsize;                                          /*pre allocated pfTable size*/

    pfTable.pftable = NULL;                                 /*init vars*/
    pfTable.size = 0;
    pfsize = 64;
    sqrtx = (u64)sqrt(x);

    if (x == 0)
        return pfTable;

    /*pre allocate 64 slot for pfTable*/
    pfTable.pftable = (PFact *)calloc(pfsize, sizeof(PFact));
    if(pfTable.pftable == NULL)
        return pfTable;

    for(i = (j = 0); ptable->ptable[i] <= sqrtx; i++)
    {
        if (i > ptable->size - 1) {
            fprintf(stderr, "%llu factorization failed\n", x);
            free(pfTable.pftable);
            pfTable.pftable = NULL;
            pfTable.size = 0;
            return pfTable;                                 /*size of ptable too small*/
        }
        if(0 == (x % ptable->ptable[i]))
        {
            x /= ptable->ptable[i];
            sqrtx = (u64)sqrt(x);
            if (pfTable.pftable[j].fact != ptable->ptable[i]) {
                if (pfTable.pftable[j].fact == 0) {
                    pfTable.pftable[j].fact = ptable->ptable[i];
                    pfTable.pftable[j].exp++;
                } else {
                    j++;
                    if ((j % 64) == 0) {
                        pfsize += 64;
                        pfTable.pftable = (PFact *)realloc(pfTable.pftable, pfsize * sizeof(PFact));
                        if (pfTable.pftable == NULL) {
                            pfTable.size = 0;
                            return pfTable;
                        }
                        memset(pfTable.pftable + pfsize - 64, 0, 64);
                    }
                    pfTable.pftable[j].fact = ptable->ptable[i];
                    pfTable.pftable[j].exp++;
                }
            } else {
                pfTable.pftable[j].exp++;
            }
            i--;
        }
    }

    if  (pfTable.pftable[j].fact == x)
        pfTable.pftable[j].exp++;
    else {
        if (pfTable.pftable[j].fact == 0) {
            pfTable.pftable[j].fact = x;
            pfTable.pftable[j].exp++;
        } else {
            j++;
            if ((j % 64) == 0) {
                pfsize += 64;
                pfTable.pftable = (PFact *)realloc(pfTable.pftable, pfsize * sizeof(PFact));
                if (pfTable.pftable == NULL) {
                    pfTable.size = 0;
                    return pfTable;
                }
                memset(pfTable.pftable + pfsize - 64, 0, 64);
            }
            pfTable.pftable[j].fact = x;
            pfTable.pftable[j].exp++;
        }
    }

    pfTable.size = j + 1;
    return pfTable;
}

/**
 * Print a prime table to stdout
 * input: ptable
 * output: none
 * return: void
 */
void prPTable(PTable *ptable)
{
    if (ptable->size) {
        for(size_t i = 0; i < ptable->size; i++) {
            printf("%9llu ", ptable->ptable[i]);
            if ((i + 1) % 10 == 0)
                putchar('\n');
        }
        putchar('\n');
    }
}

/**
 * Print a prime fact table
 * input: pftable
 * output: none
 * return: void
 */
void prpfTable(PFtable *pftable)
{
    if (pftable->size) {
        for(size_t i = 0; i < pftable->size; i++) {
            if (pftable->pftable[i].exp != 1)
                printf("(%llu^%llu)", pftable->pftable[i].fact, pftable->pftable[i].exp);
            else
                printf("%llu", pftable->pftable[i].fact);
            if (i != pftable->size - 1)
                putchar('*');
        }
        putchar('\n');
        fflush(stdout);
    }
}