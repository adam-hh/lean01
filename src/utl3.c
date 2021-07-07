#include "bigNumber.h"

/**
 * a^k mod b
 * input: a, b, k
 * output: rem
 * return: 1 for success, -1 for fail
 */
int powerModew(const N128 *restrict a, const N128 *restrict b, const N128 *restrict k, N128 *restrict rem)
{
    N256 a256, b256, r256, b256mask;                                    /*middle results*/
    uint32_t wk, *pwk;                                                  /*used for binary explore k*/
    int lenOfk, i, j;                                                   /*len of k and loop counters*/

    if((a == NULL) || (b == NULL) || (k == NULL) || (rem == NULL)) {    /*illegal para*/
        raise(SIGFPE);                                                  /*raise SIGFPE, should terminal the process by default*/
        return -1;
    }

    a256 = castwtd(a);                                                  /*init vars, cast a and b so that mult overflow won't happen*/
    b256 = castwtd(b);
    dToMask(&b256, &b256mask);
    lenOfk = (sizeof(k->val) >> 2) - skiplzw(k, NULL);
    pwk = (uint32_t *)(k->val);

    moded(&a256, &b256, &r256, NULL, &b256mask);
    if(skiplzd(&r256, NULL) == (sizeof(r256.val) >> 2)) {               /*check if a % b = 0*/
        wzero(rem);                                                     /*set rem=0*/
        return 1;
    }

    dzero(&r256);
    r256.val[0] = 1;                                                    /*init r=1*/
    for(i = 0; i < lenOfk; i++) {                                       /*binary expansion k*/
        wk = pwk[i];
        if (i == (lenOfk - 1)) {
            for(; wk >= 1;) {                                           /*the highest part of k, only expand the significant bits*/
                if(1 == (wk & 0x01)) {
                    vmultd(&r256, &a256);
                    modd(&r256, &b256, &b256mask);                      /*(r*a) mod b*/
                }
                vmultd(&a256, &a256);
                modd(&a256, &b256, &b256mask);                          /*(a*a) mod b*/
                wk >>= 1;
            }
        } else {
            for(j = 0; j < 32; j++) {                                   /*the reset parts of k, expand all 32 bits*/
                if(1 == (wk & 0x01)) {
                    vmultd(&r256, &a256);
                    modd(&r256, &b256, &b256mask);
                }
                vmultd(&a256, &a256);
                modd(&a256, &b256, &b256mask);
                wk >>= 1;
            }
        }
    }

    *rem = castdtw(&r256);                                              /*set rem*/
    return 1;
}
int powerModed(const N256 *restrict a, const N256 *restrict b, const N256 *restrict k, N256 *restrict rem)
{
    N512 a512, b512, r512, b512mask;                                    /*middle results*/
    uint32_t wk, *pwk;                                                  /*used for binary explore k*/
    int lenOfk, i, j;                                                   /*len of k and loop counters*/

    if((a == NULL) || (b == NULL) || (k == NULL) || (rem == NULL)) {    /*illegal para*/
        raise(SIGFPE);                                                  /*raise SIGFPE, should terminal the process by default*/
        return -1;
    }

    a512 = castdtq(a);                                                  /*init vars, cast a and b so that mult overflow won't happen*/
    b512 = castdtq(b);
    qToMask(&b512, &b512mask);
    lenOfk = (sizeof(k->val) >> 2) - skiplzd(k, NULL);
    pwk = (uint32_t *)(k->val);

    modeq(&a512, &b512, &r512, NULL, &b512mask);
    if(skiplzq(&r512, NULL) == (sizeof(r512.val) >> 2)) {               /*check if a % b = 0*/
        dzero(rem);                                                     /*set rem=0*/
        return 1;
    }

    qzero(&r512);
    r512.val[0] = 1;                                                    /*init r=1*/
    for(i = 0; i < lenOfk; i++) {                                       /*binary expansion k*/
        wk = pwk[i];
        if (i == (lenOfk - 1)) {
            for(; wk >= 1;) {                                           /*the highest part of k, only expand the significant bits*/
                if(1 == (wk & 0x01)) {
                    vmultq(&r512, &a512);
                    modq(&r512, &b512, &b512mask);
                }
                vmultq(&a512, &a512);
                modq(&a512, &b512, &b512mask);
                wk >>= 1;
            }
        } else {
            for(j = 0; j < 32; j++) {                                   /*the reset parts of k, expand all 32 bits*/
                if(1 == (wk & 0x01)) {
                    vmultq(&r512, &a512);
                    modq(&r512, &b512, &b512mask);
                }
                vmultq(&a512, &a512);
                modq(&a512, &b512, &b512mask);
                wk >>= 1;
            }
        }
    }

    *rem = castqtd(&r512);                                              /*set rem*/
    return 1;
}
int powerModeq(const N512 *a, const N512 *b, const N512 *k, N512 *rem)
{
    return 0;
}