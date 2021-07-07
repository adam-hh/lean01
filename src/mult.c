#include "bigNumber.h"

/**
 * Multiplication with static data return
 * Not reentrantable
 * input: a, b
 * output: a
 * return: address of the static obj midval
 */
N256 *multw(N128 *a, N128 *b)
{
    static N256 midval;                                                     /*middle and final result*/
    uint32_t *pa, *pb, *pmid;                                               /*travel pointers*/
    uint64_t sum, ofadd, ofmlt;                                             /*ofadd is addition overflow bits, ofmlt is mult overflow bits*/
    size_t zcnta, zcntb;                                                    /*leading zero counters*/
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        } v32;
    } va, vb;                                                               /*union va and vb used to do multipication*/

    pa = (uint32_t *)(a->val);                                              /*init vars*/
    pb = (uint32_t *)(b->val);
    pmid = (uint32_t *)(midval.val);
    zcnta = skiplzw(a, NULL);
    zcntb = skiplzw(b, NULL);
    dzero(&midval);
    if((zcnta == (sizeof(a->val) / 4)) || (zcntb == (sizeof(b->val) / 4))) {/*a or b equals 0*/
        wzero(a);
        return &midval;
    }

    for(size_t cnt1 = 0; cnt1 < (sizeof(b->val) / 4 - zcntb); cnt1++) {     /*outter loop depends on len of b*/
        vb.v64 = *pb++;                                                     /*set vb*/
        pa = (uint32_t *)(a->val);                                          /*set pa*/
        pmid = (uint32_t *)(midval.val) + cnt1;                             /*set pmid, pmid moving with outter loop*/

        ofadd = 0;                                                          /*init ofadd and ofmlt*/
        ofmlt = 0;
        for(size_t cnt2 = 0; cnt2 < (sizeof(a->val) / 4 -zcnta); cnt2++) {  /*inner loop depends on len of a*/
            va.v64 = *pa++;                                                 /*set va*/
            va.v64 *= vb.v64;
            va.v64 += ofmlt;
            ofmlt = va.v32.r;                                               /*reset ofmlt*/

            sum = *pmid;                                                    /*read pmid value*/
            sum += va.v32.l;
            sum += ofadd;
            ofadd = (sum >> 32);                                            /*reset ofadd*/
            *pmid++ = sum;                                                  /*write back pmid value*/
        }
        *pmid = ofmlt + ofadd;                                              /*write back last pmid*/
    }

    *a = castdtw(&midval);                                                  /*write final result to a, cast from N256 to N128*/
    return &midval;
}
N512 *multd(N256 *a, N256 *b)
{
    static N512 midval;
    uint32_t *pa, *pb, *pmid;
    uint64_t sum, ofadd, ofmlt;
    size_t zcnta, zcntb;
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        } v32;
    } va, vb;

    pa = (uint32_t *)(a->val);
    pb = (uint32_t *)(b->val);
    pmid = (uint32_t *)(midval.val);
    zcnta = skiplzd(a, NULL);
    zcntb = skiplzd(b, NULL);
    qzero(&midval);
    if((zcnta == (sizeof(a->val) / 4)) || (zcntb == (sizeof(b->val) / 4))) {
        dzero(a);
        return &midval;
    }

    for(size_t cnt1 = 0; cnt1 < (sizeof(b->val) / 4 - zcntb); cnt1++) {
        vb.v64 = *pb++;
        pa = (uint32_t *)(a->val);
        pmid = (uint32_t *)(midval.val) + cnt1;

        ofadd = 0;
        ofmlt = 0;
        for(size_t cnt2 = 0; cnt2 < (sizeof(a->val) / 4 - zcnta); cnt2++) {
            va.v64 = *pa++;
            va.v64 *= vb.v64;
            va.v64 += ofmlt;
            ofmlt = va.v32.r;

            sum = *pmid;
            sum += va.v32.l;
            sum += ofadd;
            ofadd = (sum >> 32);
            *pmid++ = sum;
        }
        *pmid = ofmlt + ofadd;
    }

    *a = castqtd(&midval);
    return &midval;
}
N1024 *multq(N512 *a, N512 *b)
{
    static N1024 midval;
    uint32_t *pa, *pb, *pmid;
    uint64_t sum, ofadd, ofmlt;
    size_t zcnta, zcntb;
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        } v32;
    } va, vb;

    pa = (uint32_t *)(a->val);
    pb = (uint32_t *)(b->val);
    pmid = (uint32_t *)(midval.val);
    zcnta = skiplzq(a, NULL);
    zcntb = skiplzq(b, NULL);
    zero1024(&midval);
    if((zcnta == (sizeof(a->val) / 4)) || (zcntb == (sizeof(b->val) / 4))) {
        qzero(a);
        return &midval;
    }

    for(size_t cnt1 = 0; cnt1 < (sizeof(b->val) / 4 - zcntb); cnt1++) {
        vb.v64 = *pb++;
        pa = (uint32_t *)(a->val);
        pmid = (uint32_t *)(midval.val) + cnt1;

        ofadd = 0;
        ofmlt = 0;
        for(size_t cnt2 = 0; cnt2 < (sizeof(a->val) / 4 -zcnta); cnt2++) {
            va.v64 = *pa++;
            va.v64 *= vb.v64;
            va.v64 += ofmlt;
            ofmlt = va.v32.r;

            sum = *pmid;
            sum += va.v32.l;
            sum += ofadd;
            ofadd = (sum >> 32);
            *pmid++ = sum;
        }
        *pmid = ofmlt + ofadd;
    }

    *a = cast1024tq(&midval);
    return &midval;
}