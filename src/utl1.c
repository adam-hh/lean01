#include "bigNumber.h"
#include <ctype.h>                                          /*isdigit*/
#include <string.h>                                         /*strlen*/

static char strings[NUMBERSIZE * 24 + 1];                   /*static strings to store digits, estimate 3 digits each byte*/

/**
 * Number to string
 * input: a
 * output: none
 * return: address of static obj strings
 */
char *wToString(const N128 *a)
{
    N128 divided;                                           /*middle result*/
    uint32_t *pwk, *position;                               /*pwk for travel, position for remembering the first non-zero posision*/
    size_t zcnta;                                           /*leading zero counter*/
    char *str;                                              /*travel pointer for strings*/
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        }v32;
    } div;                                                  /*union div work for binary-decimal conversion*/

    copyw(&divided, a);                                     /*init vars*/
    strings[NUMBERSIZE * 24] = 0;
    zcnta = skiplzw(&divided, &position);
    if(zcnta == (sizeof(a->val) / 4)) {                     /*a equals 0*/
        strings[NUMBERSIZE * 24 - 1] = '0';
        return &strings[NUMBERSIZE * 24 - 1];
    }

    str = &strings[NUMBERSIZE * 24 - 1];                    /*continue init vars*/
    while(*position) {
        pwk = position;                                     /*set pwk to position*/
        div.v64 = 0;                                        /*clear div*/
        while(pwk != (uint32_t *)(divided.val) - 1) {
            div.v32.l = *pwk;                               /*read pwk*/
            *pwk = (div.v64 / 10);                          /*write back pwk*/
            div.v32.r = (div.v64 % 0xa);                    /*store the remainder to div.v32.r*/
            pwk--;
        }
        *str-- = div.v32.r + 0x30;                          /*digit to ASCII char*/
        if(str == strings) {                                /*out of band of strings*/
            raise(SIGSEGV);                                 /*raise SIGSEGV, to terminal the process by default*/
            return ++str;
        }

        while(*position == 0) {
            position--;                                     /*moving position to match the first non-zero postion*/
            if(position == (uint32_t *)(divided.val) - 1) {
                position++;                                 /*now a equals 0, set *position 0 to end the outter loop*/
                break;
            }
        }
    }
    
    return ++str;                                           /*str should to be inc*/
}
char *dToString(const N256 *a)
{
    N256 divided;
    uint32_t *pwk, *position;
    size_t zcnta;
    char *str;
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        }v32;
    } div;

    copyd(&divided, a);
    strings[NUMBERSIZE * 24] = 0;
    zcnta = skiplzd(&divided, &position);
    if(zcnta == (sizeof(a->val) / 4)) {
        strings[NUMBERSIZE * 24 - 1] = '0';
        return &strings[NUMBERSIZE * 24 - 1];
    }

    str = &strings[NUMBERSIZE * 24 - 1];
    while(*position) {
        pwk = position;
        div.v64 = 0;
        while(pwk != (uint32_t *)(divided.val) - 1) {
            div.v32.l = *pwk;
            *pwk = (div.v64 / 10);
            div.v32.r = (div.v64 % 0xa);
            pwk--;
        }
        *str-- = div.v32.r + 0x30;
        if(str == strings) {
            raise(SIGSEGV);
            return ++str;
        }

        while(*position == 0) {
            position--;
            if(position == (uint32_t *)(divided.val) - 1) {
                position++;
                break;
            }
        }
    }
    
    return ++str;
}
char *qToString(const N512 *a)
{
    N512 divided;
    uint32_t *pwk, *position;
    size_t zcnta;
    char *str;
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        }v32;
    } div;

    copyq(&divided, a);
    strings[NUMBERSIZE * 24] = 0;
    zcnta = skiplzq(&divided, &position);
    if(zcnta == (sizeof(a->val) / 4)) {
        strings[NUMBERSIZE * 24 - 1] = '0';
        return &strings[NUMBERSIZE * 24 - 1];
    }

    str = &strings[NUMBERSIZE * 24 - 1];
    while(*position) {
        pwk = position;
        div.v64 = 0;
        while(pwk != (uint32_t *)(divided.val) - 1) {
            div.v32.l = *pwk;
            *pwk = (div.v64 / 10);
            div.v32.r = (div.v64 % 0xa);
            pwk--;
        }
        *str-- = div.v32.r + 0x30;
        if(str == strings) {
            raise(SIGSEGV);
            return ++str;
        }

        while(*position == 0) {
            position--;
            if(position == (uint32_t *)(divided.val) - 1) {
                position++;
                break;
            }
        }
    }
    
    return ++str;
}

/**
 * String to number
 * input: src, len
 * output: none
 * return: address of static obj rlt
 */
N128 *stringTow(const char *src,size_t len)
{
    static N128 rlt;                                /*final result for return*/
    N128 decNumber, decTen;                         /*middle results*/
    const char *psrc;                               /*travel pointer of src*/

    wzero(&rlt);                                    /*init vars*/
    wzero(&decNumber);
    wzero(&decTen);
    decTen.val[0] = 10;

    if(len == 0)                                    /*len is not given*/
        len = strlen(src);                          /*consider src a c style string*/
    psrc = src + len - 1;

    if((len == 0) || (len > sizeof(rlt.val) * 3)) { /*illegal src length*/
        //raise(SIGFPE);                              /*raise SIGFPE, should terminal process by default*/
        return &rlt;
    }
    
    for(int i = 0; i < len; i++) {
        if(!isdigit(*psrc)) {                       /*illegal non-digit char*/
            raise(SIGFPE);
            return &rlt;
        }
        wzero(&decNumber);
        decNumber.val[0] = *psrc - 0x30;            /*set decNumber during loop*/
        for(int j = 0; j < i; j++) {
            vmultw(&decNumber, &decTen);             /*mult 10*/
        }
        addw(&rlt, &decNumber);                     /*add middle result to final result*/
        psrc--;
    }
    return &rlt;
}
N256 *stringTod(const char *src, size_t len)
{
    static N256 rlt;                                /*final result for return*/
    N256 decNumber, decTen;                         /*middle results*/
    const char *psrc;                               /*travel pointer of src*/

    dzero(&rlt);                                    /*init vars*/
    dzero(&decNumber);
    dzero(&decTen);
    decTen.val[0] = 10;

    if(len == 0)
        len = strlen(src);
    psrc = src + len - 1;

    if((len == 0) || (len > sizeof(rlt.val) * 3)) { /*illegal src length*/
        raise(SIGFPE);                              /*raise SIGFPE, should terminal process by default*/
        return &rlt;
    }
    
    for(int i = 0; i < len; i++) {
        if(!isdigit(*psrc)) {                       /*illegal non-digit char*/
            raise(SIGFPE);
            return &rlt;
        }
        dzero(&decNumber);
        decNumber.val[0] = *psrc - 0x30;            /*set decNumber during loop*/
        for(int j = 0; j < i; j++) {
            vmultd(&decNumber, &decTen);             /*mult 10*/
        }
        addd(&rlt, &decNumber);                     /*add middle result to final result*/
        psrc--;
    }
    return &rlt;
}
N512 *stringToq(const char *src, size_t len)
{
    static N512 rlt;                                /*final result for return*/
    N512 decNumber, decTen;                         /*middle results*/
    const char *psrc;                               /*travel pointer of src*/

    qzero(&rlt);                                    /*init vars*/
    qzero(&decNumber);
    qzero(&decTen);
    decTen.val[0] = 10;

    if(len == 0)
        len = strlen(src);
    psrc = src + len - 1;

    if((len == 0) || (len > sizeof(rlt.val) * 3)) { /*illegal src length*/
        raise(SIGFPE);                              /*raise SIGFPE, should terminal process by default*/
        return &rlt;
    }
    
    for(int i = 0; i < len; i++) {
        if(!isdigit(*psrc)) {                       /*illegal non-digit char*/
            raise(SIGFPE);
            return &rlt;
        }
        qzero(&decNumber);
        decNumber.val[0] = *psrc - 0x30;            /*set decNumber during loop*/
        for(int j = 0; j < i; j++) {
            vmultq(&decNumber, &decTen);             /*mult 10*/
        }
        addq(&rlt, &decNumber);                     /*add middle result to final result*/
        psrc--;
    }
    return &rlt;
}