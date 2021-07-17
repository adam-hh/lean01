/**
 * Copyright (c) 2020-2021, Adam hh <adamhh.wk@gmail.com>
 */

#ifndef LITTLEPRIME_H
#define LITTLEPRIME_H

#include <signal.h>

#ifdef __cpluscplus
exten "C" {
#endif

#ifndef NULL
#define NULL (void *)0
#endif

#ifndef size_t
typedef unsigned long size_t;
#endif

/*alias of buildin integer for convenience*/
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

/*128 bits width integer*/
typedef struct {
    u8 val[16];
} N16;
/*256 bits width integer*/
typedef struct {
    u8 val[32];
} N32;

/*prime table*/
typedef struct {
    u64 *ptable;
    size_t size;
} PTable;

/*prime fact, including fact and exponent*/
typedef struct {
    u64 fact;
    u64 exp;
} PFact;

/*prime fact table*/
typedef struct {
    PFact *pftable;
    size_t size;
} PFtable;

/*masks to split a 64 bits integer to half half*/
#define MASKL32 0x00000000ffffffff
#define MASKR32 0xffffffff00000000

/**
 * a+b
 * input: a, b
 * output: a
 * return: void
 */
static inline void addN16(N16 *a, const N16 *b)
{
    u32 *pa, *pb;
    u64 sum, addof;

    pa = (u32 *)(a->val);
    pb = (u32 *)(b->val);
    addof = 0;
    for(int i = 0; i < 4; i++) {
        sum = *pa;
        sum += *pb;
        sum += addof;
        addof = sum >> 32;
        *pa = sum;

        pa++;
        pb++;
    }
}

/**
 * a++
 * input: a
 * output: a
 * return: void
 */
static inline void incN16(N16 *a)
{
    u32 *pa;

    pa = (u32 *)(a->val);
    for(int i = 0; i < 4; i++) {
        (*pa)++;
        if (*pa == 0)
            pa++;
        else 
            break;
    }
}

/**
 * -a
 * input: inv
 * output: inv
 * return: void
 */
static inline void invN16(N16 *inv)
{
    u64 *pinv;

    pinv = (u64 *)(inv->val);
    pinv[0] = ~pinv[0];
    pinv[1] = ~pinv[1];
    incN16(inv);
}

/**
 * clear obj
 * input: a
 * output: a
 * return: void
 */
static inline void N16zero(N16 *a)
{
    u64 *pa;

    pa = (u64*)(a->val);
    pa[0] = 0;
    pa[1] = 0;
}
static inline void N32zero(N32 *a)
{
    u64 *pa;

    pa = (u64*)(a->val);
    pa[0] = 0;
    pa[1] = 0;
    pa[2] = 0;
    pa[3] = 0;
}

/**
 * copy src to dst
 * input: src
 * output: dst
 * return: void
 */
static inline void copyN16(N16 *dst, N16 *src)
{
    u64 *psrc, *pdst;

    pdst = (u64*)(dst->val);
    psrc = (u64*)(src->val);
    pdst[0] = psrc[0];
    pdst[1] = psrc[1];
}
static inline void copyN32(N32 *dst, N32 *src)
{
    u64 *psrc, *pdst;

    pdst = (u64*)(dst->val);
    psrc = (u64*)(src->val);
    pdst[0] = psrc[0];
    pdst[1] = psrc[1];
    pdst[2] = psrc[2];
    pdst[3] = psrc[3];
}

/**
 * cast fro N32 to N16
 * input: src
 * output: dst
 * return: void
 */
static inline void cast32t16(N16 *dst, const N32 *src)
{
    u64 *psrc, *pdst;

    pdst = (u64*)(dst->val);
    psrc = (u64*)(src->val);
    pdst[0] = psrc[0];
    pdst[1] = psrc[1];
}

/**
 * multplication a * b
 * input: a, b
 * output: a
 * return: void
 */
static inline void multN16(N16 *a, const N16 *b)
{
    N32 rlt;
    u64 aval, bval;
    u32 *pa, *pb, *prlt;
    u64 sum, addof, mltof;

    N32zero(&rlt);
    pa = (u32 *)(a->val);
    pb = (u32 *)(b->val);
    prlt = (u32 *)(rlt.val);

    for(int i = 0; i < (sizeof(b->val) >> 2); i++) {
        bval = *pb++;
        prlt = (u32 *)(rlt.val) + i;
        pa = (u32 *)(a->val);

        addof = 0;
        mltof = 0;
        for(int j = 0; j < (sizeof(a->val) >> 2); j++) {
            aval = *pa++;
            aval *= bval;
            aval += mltof;

            mltof = (aval & MASKR32) >> 32;
            aval &= MASKL32;

            sum = *prlt;
            sum += aval;
            sum += addof;
            *prlt = sum;

            prlt++;
            addof = (sum >> 32);
        }

        *prlt = addof + mltof;
    }

    cast32t16(a, &rlt);
}

/**
 * a mod b
 * input: a, b
 * output: a
 * retur: void
 */
static inline void modN8(N16 *a, u64 b)
{
    N16 rlt, binv;
    u64 *pa, *prlt, *pbinv;

    pa = (u64 *)(a->val);
    prlt = (u64*)(rlt.val);
    pbinv = (u64 *)(binv.val);
    N16zero(&rlt);
    pbinv[0] = b;
    pbinv[1] = 0;
    invN16(&binv);

    if (b == 0) {
        raise(SIGFPE);
        return;
    }

    pa[0] %= b;
    pa[1] %= b;
    for(int i = 64; i != 0; i++) {
        pa[1] <<= 1;
        pa[1] %= b;
    }
    pa[0] += pa[1];

    if (pa[0] < pa[1]) {
        prlt[0] = pa[0];
        prlt[1] = 0x1;
        addN16(&rlt, &binv);
        pa[0] = prlt[0];
    } else {
        pa[0] %= b;
    }
    pa[1] = 0;
}

/**
 * a^exp
 * input: a, exp
 * output: none
 * return: a^exp
 */
static inline u64 expu64(u64 a, u64 exp)
{
    u64 rlt;

    rlt = 1;
    if (exp == 0)
        return rlt;
    else {
        while(exp--) {
            rlt *= a;
        }
    }

    return rlt;
}

/**
 * 
 */
extern PTable createpTable(size_t ptablesize);
extern PFtable primeFact(u64 x, PTable *ptable);

extern void prPTable(PTable *ptable);
extern void prpfTable(PFtable *pftable);

extern u64 funcFai(u64 x, PTable *ptable);
extern u64 funcDelta(u64 x, PTable *ptable);

extern u64 gcd(u64 a, u64 b);
extern u64 linearEquition(u64 a, u64 m, u64 *x, u64 *y);

extern u64 powerModeN8(u64 a, u64 m, u64 k);












#ifdef __cpluscplus
}
#endif

#endif              /*LITTLEPRIME_H*/