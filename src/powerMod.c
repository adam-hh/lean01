#include "littlePrime.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>

/**
 * (a*b) mod m
 * input: a, b, m
 * output: a
 * return: void
 */
static inline void multModN8(u64 *a, u64 b, u64 m)
{
    N16 aval, bval;
    u64 *pa, *pb;

    pa = (u64 *)(aval.val);
    pb = (u64 *)(bval.val);
    pa[0] = *a;
    pa[1] = 0;
    pb[0] = b;
    pb[1] = 0;

    if(b == 0) {
        raise(SIGFPE);
        return;
    }

    multN16(&aval, &bval);
    modN8(&aval, b);
    *a = pa[0];
}

/**
 * a^k mod m
 * input: a, k, m
 * output: none
 * return: operation result
 */
u64 powerModeN8(u64 a, u64 m, u64 k)
{
    u64 aval, r;                    /*set 1 as initial value of r.*/

    aval = a;
    r = 1;
    if (m == 0) {
        raise(SIGFPE);
        return -1;
    }
    if (0 == (a % m))
        return 0;                   /*directly divisible*/
    while(k >= 1)                   /*math:binary expansion k,from the lowest bit to higest bit.*/
    {
        if(1 == (k & 0x1))          /*math:get the lowest bit*/
            multModN8(&r, aval, m);
        multModN8(&aval, aval, m);  /*squal of a, keep the remainder to a*/
        k >>= 1;                    /*math:right shit 1 bit each time, will finally went to the break point of while loop*/
    }
    return r;
}

/**
 * Resolve equition kx mod m = 1, gcd(k,m) = 1
 * solution is k^(fai(m) - 1) mod m
 * input: k, m, ptable
 * output: none
 * return: solution
 */
u64 rModEquition(u64 k, u64 m, PTable *ptable)
{
    u64 fm;

    fm = funcFai(m, ptable);
    if (fm == 0xffffffffffffffff)
        return fm;
    return powerModeN8(k, m, fm - 1);

}