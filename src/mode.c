#include "bigNumber.h"

/**
 * Modular operation, reentrantable
 * input: a, b, bmask
 * output: rem, quo
 * return: void
 * Note: rem, quo and bmask can be set to NULL
 */
void modew(N128 *a, const N128 *b, N128 *rem, N128 *quo, const N128 *bmask)
{
    N128 remainder, origin, inverse, quotient;                  /*middle results*/
    uint32_t zcnta, zcntb;                                      /*leading zeros counter*/
    int extrashift;                                             /*extrashift flag*/

    copyw(&origin, b);                                          /*init vars*/
    invw(&origin, &inverse);                                    
    copyw(&remainder, a);                                       
    zcnta = cntlzw(a);                                          
    zcntb = cntlzw(b);

    if(zcntb == sizeof(b->val)) {                               /*b equals 0 cause an arithmetic exception*/
        raise(SIGFPE);
        return;
    }

    if(quo) {                                                   /*with quo*/
        wzero(quo);                                             /*init quo*/
        wzero(&quotient);                                       /*init quotient*/
        extrashift = 0;                                         /*init extrashift flag*/
        while(cmpw(&remainder, b) >= 0) {
            if(zcntb > zcnta) {
                /* check the value of first non-zero byte of remainder to do extra 4 bits shift or not */
                if (remainder.val[sizeof(remainder.val) - zcnta - 1] & 0xf0) {
                    extrashift = 1;
                    salw(&origin, (zcntb - zcnta - 1) * 8 + 4); /*left shift origin and inverse, key operation for division efficiency*/
                    salw(&inverse, (zcntb - zcnta - 1) * 8 + 4);
                } else {
                    salw(&origin, (zcntb - zcnta - 1) * 8);     /*left shift origin and inverse, key operation for division efficiency*/
                    salw(&inverse, (zcntb - zcnta - 1) * 8);
                }
            }
            while(cmpw(&remainder, &origin) >= 0) {
                addw(&remainder, &inverse);
                incw(&quotient);
            }
            if(zcntb > zcnta) {
                if (extrashift) {
                    extrashift = 0;
                    salw(&quotient, (zcntb - zcnta - 1) * 8 + 4);/*left shift quotient the same distance*/
                }
                else
                    salw(&quotient, (zcntb - zcnta - 1) * 8); 
            }
            addw(quo, &quotient);                               /*add middle result quotient to final result quo*/
            wzero(&quotient);                                   /*clear quotient*/

            copyw(&origin, b);                                  /*reset origin and inverse for next loop*/
            invw(&origin, &inverse);

            zcnta = cntlzw(&remainder);                         /*upgrate zcnta value, increasing at here*/
        }
    } else {                                                    /*without quo, only care remainder*/
        //wToMask(b, &bmask);
        modw(&remainder, b, bmask);
    }

    if(rem)                                                     /*rem is not NULL*/
        copyw(rem, &remainder);                                 /*copy remiander to rem*/
}
void moded(N256 *a, const N256 *b, N256 *rem, N256 *quo, const N256 *bmask)
{
    N256 remainder, origin, inverse, quotient;                  /*middle results*/
    uint32_t zcnta, zcntb;                                      /*leading zeros counter*/
    int extrashift;

    copyd(&origin, b);                                          /*init vars*/
    invd(&origin, &inverse);                                    
    copyd(&remainder, a);                                       
    zcnta = cntlzd(a);                                          
    zcntb = cntlzd(b);

    if(zcntb == sizeof(b->val)) {                               /*b equals 0 cause an arithmetic exception*/
        raise(SIGFPE);
        return;
    }

    if(quo) {                                                   /*with quo*/
        dzero(quo);                                             /*init quo*/
        dzero(&quotient);                                       /*init quotient*/
        extrashift = 0;
        while(cmpd(&remainder, b) >= 0) {
            if(zcntb > zcnta) {
                if (remainder.val[sizeof(remainder.val) - zcnta - 1] & 0xf0) {
                    extrashift = 1;
                    sald(&origin, (zcntb - zcnta - 1) * 8 + 4); /*left shift origin and inverse, key operation for division calculation efficiency*/
                    sald(&inverse, (zcntb - zcnta - 1) * 8 + 4);
                } else {
                    sald(&origin, (zcntb - zcnta - 1) * 8);     /*left shift origin and inverse, key operation for division calculation efficiency*/
                    sald(&inverse, (zcntb - zcnta - 1) * 8);
                }
            }
            while(cmpd(&remainder, &origin) >= 0) {
                addd(&remainder, &inverse);
                incd(&quotient);
            }
            if(zcntb > zcnta) {
                if (extrashift) {
                    extrashift = 0;
                    sald(&quotient, (zcntb - zcnta - 1) * 8 + 4);/*left shift quotient the same distance*/
                } else
                    sald(&quotient, (zcntb - zcnta - 1) * 8);
            }
            addd(quo, &quotient);                               /*add middle result quotient to final result quo*/
            dzero(&quotient);                                   /*clear quotient*/

            copyd(&origin, b);                                  /*reset origin and inverse for next loop*/
            invd(&origin, &inverse);

            zcnta = cntlzd(&remainder);                         /*upgrate zcnta value, increasing at here*/
        }
    } else {                                                    /*without quo, only care remainder*/
        modd(&remainder, b, bmask);
    }

    if(rem)                                                     /*rem is not NULL*/
        copyd(rem, &remainder);                                 /*copy remiander to rem*/
}
void modeq(N512 *a, const N512 *b, N512 *rem, N512 *quo, const N512 *bmask)
{
    N512 remainder, origin, inverse, quotient;                  /*middle results*/
    uint32_t zcnta, zcntb;                                      /*leading zeros counter*/
    int extrashift;

    copyq(&origin, b);                                          /*init vars*/
    invq(&origin, &inverse);                                    
    copyq(&remainder, a);                                       
    zcnta = cntlzq(a);                                          
    zcntb = cntlzq(b);

    if(zcntb == sizeof(b->val)) {                               /*b equals 0 cause an arithmetic exception*/
        raise(SIGFPE);
        return;
    }

    if(quo) {                                                   /*with quo*/
        qzero(quo);                                             /*init quo*/
        qzero(&quotient);                                       /*init quotient*/
        extrashift = 0;
        while(cmpq(&remainder, b) >= 0) {
            if(zcntb > zcnta) {
                if (remainder.val[sizeof(remainder.val) - zcnta - 1] & 0xf0) {
                    extrashift = 1;
                    salq(&origin, (zcntb - zcnta - 1) * 8 + 4); /*left shift origin and inverse, key operation for division calculation efficiency*/
                    salq(&inverse, (zcntb - zcnta - 1) * 8 + 4);
                } else {
                    salq(&origin, (zcntb - zcnta - 1) * 8);     /*left shift origin and inverse, key operation for division calculation efficiency*/
                    salq(&inverse, (zcntb - zcnta - 1) * 8);
                }
            }
            while(cmpq(&remainder, &origin) >= 0) {
                addq(&remainder, &inverse);
                incq(&quotient);
            }
            if(zcntb > zcnta) {
                if(extrashift) {
                    extrashift = 0;
                    salq(&quotient, (zcntb - zcnta - 1) * 8 + 4);/*left shift quotient the same distance*/
                } else
                    salq(&quotient, (zcntb - zcnta - 1) * 8);
            }
            addq(quo, &quotient);                               /*add middle result quotient to final result quo*/
            qzero(&quotient);                                   /*clear quotient*/

            copyq(&origin, b);                                  /*reset origin and inverse for next loop*/
            invq(&origin, &inverse);

            zcnta = cntlzq(&remainder);                         /*upgrate zcnta value, increasing at here*/
        }
    } else {                                                    /*without quo, only care remainder*/
        modq(&remainder, b, bmask);
    }

    if(rem)                                                     /*rem is not NULL*/
        copyq(rem, &remainder);                                 /*copy remiander to rem*/
}