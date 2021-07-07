#include "bigNumber.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*max evidence number*/
#define MAXEVNUM 100

struct evidStackw {
    N128 stack[MAXEVNUM + 1];
    uint32_t rsp;
};
struct evidStackd {
    N256 stack[MAXEVNUM + 1];
    uint32_t rsp;
};
struct evidStackq {
    N512 stack[MAXEVNUM + 1];
    uint32_t rsp;
};

#define pushEvdw(sta, val) \
        sta->rsp++; \
        copyw(sta->stack + sta->rsp, val)

#define pushEvdd(sta, val) \
        sta->rsp++; \
        copyd(sta->stack + sta->rsp, val)

#define pushEvdq(sta, val) \
        sta->rsp++; \
        copyq(sta->stack + sta->rsp, val)

#define popEvd(sta) \
        (sta->rsp--, sta->stack + sta->rsp + 1)

//static N64 one64 = {{1}};
static N128 one128 = {{1}};
static N256 one256 = {{1}};

/**
 * Based on Miller-Rabin primality test algorithm
 * n is the number being tested, t is the number of evidence expected, fdUrandom is the random device fd
 * math eqution: n-1=(2^k)*q
 * input: n, t, fdUrandom
 * output: none
 * return: 1 for success, -1 for fail
 */
static int MRtesthw(const N64 *n, int t, int fdUrandom)
{
    N128 nval, qval, rem, nsub1, nmask;                     /*n, q, remainder, n-1*/
    N128 evidence;                                          /*RM evidence, which value between 1 and n-1*/
    struct evidStackw *candidate, *exchange;                /*Evidence stacks for sorting order*/
    size_t k;                                               /*k*/
    int i, j;                                               /*loop counters*/
    int cmp;                                                /*comparation result*/
    uint32_t wk, *pwk;                                      /*used to do binary expansion of n-1*/
    size_t lenOfn;                                          /*len of n*/

    nval = casthwtw(n);                                     /*init vars*/
    nsub1 = casthwtw(n);
    decw(&nsub1);
    copyw(&qval, &nsub1);
    pwk = (uint32_t *)(nsub1.val);
    lenOfn = sizeof(nval.val) - cntlzw(&nval);
    if((nval.val[0] & 0x01) == 0)                           /*n is even*/
        return -1;
    if(lenOfn <= 8) {                                       /*little number*/
        /*fact it directly*/
        return -1;
    }

    for(k = 0; k < (lenOfn << 3);) {                       /*binary expand n-1*/
        wk = *pwk++;
        for(i = 0; i < 32; i++) {
            if((wk & 0x01) == 0)
                k++;
            else
                goto kpoint;                                /*binary expansion finished, break loop*/
            wk >>= 1;
        }
    }

    kpoint:
    sarw(&qval, k);                                         /*got q*/
    if(k > 1)
        wToMask(&nval, &nmask);
    
    if((t == 0) || (t > MAXEVNUM))                          /*check value of t, then reset it*/
        t = MAXEVNUM;

    candidate = (struct evidStackw *)malloc(sizeof(struct evidStackw));
    exchange = (struct evidStackw *)malloc(sizeof(struct evidStackw));
    if((candidate == NULL) || (exchange == NULL))
        return -1;
    memset(candidate, 0, sizeof(struct evidStackw));
    memset(exchange, 0, sizeof(struct evidStackw));

    for(i = 0; i < t; i++) {
        cont:
        read(fdUrandom, evidence.val, lenOfn - 1);
        evidence.val[0] |= 0x02;

        /*sorting order of evidences to avoid duplicating*/
        while(1) {
            cmp = cmpw(&evidence, &(candidate->stack[candidate->rsp]));
            if (cmp > 0) {
                pushEvdw(candidate, &evidence);
                while(exchange->rsp) {
                    pushEvdw(candidate, popEvd(exchange));
                }
                break;
            } else if (cmp < 0) {
                pushEvdw(exchange, popEvd(candidate));
                continue;
            } else {
                i--;
                while(exchange->rsp) {
                    pushEvdw(candidate, popEvd(exchange));
                }
                goto cont;
            }
        }

        powerModew(&evidence, &nval, &qval, &rem);          /*a^q mod n remaind rem*/
        if(cmpw(&rem, &one128) == 0)                        /*a^q mod n remaind 1*/
            continue;
        if(cmpw(&rem, &nsub1) == 0)                         /*a^q mod n remaind -1*/
            continue;

        for(j = 1; j < k; j++) {                            /*loop count k-1*/
            vmultw(&rem, &rem);                             /*(a^q)^2*/
            modw(&rem, &nval, &nmask);                      /*(a^q)^2 mod n*/
            if(cmpw(&rem, &nsub1) == 0)                     /*compare (a^q)^2 mod n remaind and -1*/
                goto ppoint;
        }
        ppoint:
        if (j == k) {
            free(candidate);
            free(exchange);
            return -1;                                      /*passed one evidence's power mode calculation*/
        }
    }

    free(candidate);
    free(exchange);
    return 1;                                               /*failed all the power mode calculation*/
}
static int MRtestw(const N128 *n, int t, int fdUrandom)
{
    N128 nval, qval, rem, nsub1;                            /*n, q, remainder, n-1*/
    N256 n256, rem256, nsub1256, n256mask;                  /*double width edition*/
    N128 evidence;                                          /*RM evidence, which value between 1 and n-1*/
    struct evidStackw *candidate, *exchange;                /*Evidence stacks for sorting order*/
    size_t k;                                               /*k*/
    int i, j;                                               /*loop counters*/
    int cmp;                                                /*comparation result*/
    uint32_t wk, *pwk;                                      /*used to do binary expansion of n-1*/
    size_t lenOfn;                                          /*len of n*/

    copyw(&nval, n);                                        /*init vars*/
    copyw(&nsub1, n);
    decw(&nsub1);
    copyw(&qval, &nsub1);
    n256 = castwtd(n);
    nsub1256 = castwtd(&nsub1);

    pwk = (uint32_t *)(nsub1.val);
    lenOfn = sizeof(nval.val) - cntlzw(&nval);
    if((nval.val[0] & 0x01) == 0)                           /*n is even*/
        return -1;
    if(lenOfn <= 8) {                                       /*little number*/
        /*fact it directly*/
        write(STDOUT_FILENO, "small n", 7);
        return -1;
    }

    for(k = 0; k < (lenOfn << 3);) {                        /*binary expand n-1*/
        wk = *pwk++;
        for(i = 0; i < 32; i++) {
            if((wk & 0x01) == 0)
                k++;
            else
                goto kpoint;                                /*binary expansion finished, break loop*/
            wk >>= 1;
        }
    }

    kpoint:
    sarw(&qval, k);                                         /*got k and q*/
    if (k > 1)
        dToMask(&n256, &n256mask);
    
    if((t == 0) || (t > MAXEVNUM))                          /*check value of t, then reset it*/
        t = MAXEVNUM;

    candidate = (struct evidStackw *)malloc(sizeof(struct evidStackw));
    exchange = (struct evidStackw *)malloc(sizeof(struct evidStackw));
    if((candidate == NULL) || (exchange == NULL))
        return -1;
    memset(candidate, 0, sizeof(struct evidStackw));
    memset(exchange, 0, sizeof(struct evidStackw));

    for(i = 0; i < t; i++) {
        cont:
        read(fdUrandom, evidence.val, lenOfn - 1);          /*make sure evidence < n*/
        evidence.val[0] |= 0x02;                            /*make sure evidence > 1*/

        /*sorting order of evidences to avoid duplicating*/
        while(1) {
            cmp = cmpw(&evidence, &(candidate->stack[candidate->rsp]));
            if (cmp > 0) {
                pushEvdw(candidate, &evidence);
                while(exchange->rsp) {
                    pushEvdw(candidate, popEvd(exchange));
                }
                break;
            } else if (cmp < 0) {
                pushEvdw(exchange, popEvd(candidate));
                continue;
            } else {
                while(exchange->rsp) {
                    pushEvdw(candidate, popEvd(exchange));
                }
                goto cont;
            }
        }

        powerModew(&evidence, &nval, &qval, &rem);          /*a^q mod n remaind rem*/
        if(cmpw(&rem, &one128) == 0)                        /*a^q mod n remaind 1*/
            continue;
        if(cmpw(&rem, &nsub1) == 0)                         /*a^q mod n remaind -1*/
            continue;

        rem256 = castwtd(&rem);
        for(j = 1; j < k; j++) {                            /*loop count k-1*/
            vmultd(&rem256, &rem256);                       /*(a^q)^2*/
            modd(&rem256, &n256, &n256mask);                /*(a^q)^2 mod n*/
            if(cmpd(&rem256, &nsub1256) == 0)               /*compare (a^q)^2 mod n remaind and -1*/
                goto ppoint;
        }
        ppoint:
        if (j == k) {
            free(candidate);
            free(exchange);
            return -1;                                      /*passed one evidence's power mode calculation*/
        }
    }

    free(candidate);
    free(exchange);
    return 1;                                               /*failed all the power mode calculation*/
}
static int MRtestd(const N256 *n, int t, int fdUrandom)
{
    N256 nval, qval, rem, nsub1;                            /*n, q, remainder, n-1*/
    N512 n512, rem512, nsub1512, n512mask;                  /*double width edition*/
    N256 evidence;                                          /*RM evidence, which value between 1 and n-1*/
    struct evidStackd *candidate, *exchange;                /*Evidence stacks for sorting order*/
    size_t k;                                               /*k*/
    int i, j;                                               /*loop counters*/
    int cmp;                                                /*comparation result*/
    uint32_t wk, *pwk;                                      /*used to do binary expansion of n-1*/
    size_t lenOfn;                                          /*len of n*/

    copyd(&nval, n);                                        /*init vars*/
    copyd(&nsub1, n);
    decd(&nsub1);
    copyd(&qval, &nsub1);
    n512 = castdtq(&nval);
    nsub1512 = castdtq(&nsub1);

    pwk = (uint32_t *)(nsub1.val);
    lenOfn = sizeof(nval.val) - cntlzd(&nval);
    if((nval.val[0] & 0x01) == 0)                           /*n is even*/
        return -1;
    if(lenOfn <= 8) {                                       /*little number*/
        /*fact it directly*/
        write(STDOUT_FILENO, "small n", 7);
        return -1;
    }

    for(k = 0; k < (lenOfn << 3);) {                        /*binary expand n-1*/
        wk = *pwk++;
        for(i = 0; i < 32; i++) {
            if((wk & 0x01) == 0)
                k++;
            else
                goto kpoint;                                /*binary expansion finished, break loop*/
            wk >>= 1;
        }
    }

    kpoint:
    sard(&qval, k);                                         /*got k and q*/
    if (k > 1)
        qToMask(&n512, &n512mask);
    
    if((t == 0) || (t > MAXEVNUM))                          /*check value of t, then reset it*/
        t = MAXEVNUM;

    candidate = (struct evidStackd *)malloc(sizeof(struct evidStackd));
    exchange = (struct evidStackd *)malloc(sizeof(struct evidStackd));
    if((candidate == NULL) || (exchange == NULL)) {
        //write(STDOUT_FILENO, "malloc fail", strlen("malloc fail"));
        return -1;
    }
    memset(candidate, 0, sizeof(struct evidStackd));
    memset(exchange, 0, sizeof(struct evidStackd));

    for(i = 0; i < t; i++) {
        cont:
        read(fdUrandom, evidence.val, lenOfn - 1);          /*make sure evidence < n*/
        evidence.val[0] |= 0x02;                            /*make sure evidence > 1*/

        /*sorting order of evidences to avoid duplicating*/
        while(1) {
            cmp = cmpd(&evidence, &(candidate->stack[candidate->rsp]));
            if (cmp > 0) {
                pushEvdd(candidate, &evidence);
                while(exchange->rsp) {
                    pushEvdd(candidate, popEvd(exchange));
                }
                break;
            } else if (cmp < 0) {
                pushEvdd(exchange, popEvd(candidate));
                continue;
            } else {
                while(exchange->rsp) {
                    pushEvdd(candidate, popEvd(exchange));
                }
                goto cont;
            }
        }
        //printf("evidence %d k %lu\n", i, k);
        powerModed(&evidence, &nval, &qval, &rem);          /*a^q mod n remaind rem*/
        if(cmpd(&rem, &one256) == 0)                        /*a^q mod n remaind 1*/
            continue;
        if(cmpd(&rem, &nsub1) == 0)                         /*a^q mod n remaind -1*/
            continue;

        rem512 = castdtq(&rem);
        for(j = 1; j < k; j++) {                            /*loop count k-1*/
            vmultq(&rem512, &rem512);                       /*(a^q)^2*/
            modq(&rem512, &n512, &n512mask);                /*(a^q)^2 mod n*/
            if(cmpq(&rem512, &nsub1512) == 0)               /*compare (a^q)^2 mod n remaind and -1*/
                goto ppoint;
        }
        ppoint:
        if (j == k) {
            free(candidate);
            free(exchange);
            return -1;                                      /*passed one evidence's power mode calculation*/
        }
    }

    free(candidate);
    free(exchange);
    return 1;                                               /*failed all the power mode calculation*/
}

/**
 * Generate a prime number
 * input: t
 * output: prime
 * return: positive value for sucess, -1 for fail
 */
int primeGenhw(N64 *prime, int t)
{
    N64 pTest;                                              /*number to be test*/
    int fd, cnt;                                            /*fd is the random generation device, cnt is counter*/

    cnt = 0;                                                /*init cnt*/
    if (-1 == (fd = open("/dev/urandom", O_RDONLY))) {      /*open /dev/urandom device*/
        return -1;
    }
    for(;;) {
        if (sizeof(pTest.val) != read(fd, pTest.val, sizeof(pTest.val))) {
            printf("\n(pid:%u)random device error\n",getpid());
            close(fd);
            return -1;
        }

        if (pTest.val[0] & 0x01) {
            cnt++;
            if (MRtesthw(&pTest, t, fd) == 1) {             /*call Mill-Rabin tester*/
                copyhw(prime, &pTest);
                close(fd);
                printf("\n(pid:%u)total tested %d numbers\n",getpid(), cnt);
                return cnt;
            } else {
                write(STDOUT_FILENO, ".", 1);
            }
        }
    }
}
int primeGenw(N128 *prime, int t, size_t *realLen)
{
    N128 pTest;
    int fd, cnt;

    cnt = 0;
    if (-1 == (fd = open("/dev/urandom", O_RDONLY))) {
        return -1;
    }

    if(*realLen > sizeof(pTest.val))
        *realLen = sizeof(pTest.val);
    if(*realLen < 9)
        *realLen = 9;

    wzero(&pTest);
    for(;;) {
        if (*realLen != read(fd, pTest.val, *realLen)) {
            printf("\n(pid:%u)random device error\n",getpid());
            close(fd);
            return -1;
        }

        if (pTest.val[0] & 0x01) {
            cnt++;
            if (MRtestw(&pTest, t, fd) == 1) {
                copyw(prime, &pTest);
                close(fd);
                printf("\n(pid:%u)total tested %d numbers\n",getpid(), cnt);
                return cnt;
            } else {
                write(STDOUT_FILENO, ".", 1);
            }
        }
    }
}

/**
 * 
 * 
 * 
 * 
 */
int primeGend(N256 *prime, int t, size_t *realLen)
{
    N256 pTest;
    int fd, cnt;

    cnt = 0;
    if (-1 == (fd = open("/dev/urandom", O_RDONLY))) {
        return -1;
    }

    if(*realLen > sizeof(pTest.val))
        *realLen = sizeof(pTest.val);
    if(*realLen < 9)
        *realLen = 9;

    dzero(&pTest);
    for(;;) {
        if (*realLen != read(fd, pTest.val, *realLen)) {
            printf("\n(pid:%u)random device error\n",getpid());
            close(fd);
            return -1;
        }

        if (pTest.val[0] & 0x01) {
            cnt++;
            if (MRtestd(&pTest, t, fd) == 1) {
                copyd(prime, &pTest);
                close(fd);
                printf("\n(pid:%u)total tested %d numbers\n",getpid(), cnt);
                return cnt;
            } else {
                write(STDOUT_FILENO, ".", 1);
            }
        }
    }
}