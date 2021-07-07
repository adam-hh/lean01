/**
 * ADAM H
 */

#ifndef BIGNUMBER_H
#define BIGNUMBER_H

#include <sys/types.h>                                      /*canonical int types, e.g uint32_t, uint64_t, etc*/
#include <stdint.h>                                         /*incase sys/types.h is not enough*/
#include <signal.h>                                         /*SIGFPE for arithmetic exception*/

#ifdef __cpluscplus
extern "C" {
#endif

/*basic integer size*/
#define NUMBERSIZE 128

/**
 * Big integer type identifier, mainly used 1024, 2048, 4096 bits Integer
 * Word: 128 bytes, 1024 bits
 * DWord: 256 bytes, 2048 bits
 * QWrod: 512 bytes, 4096 bits
 * Note: they woking on little edian machine only
 */
typedef struct {
    uint8_t val[NUMBERSIZE];
} N128;
typedef struct {
    uint8_t val[NUMBERSIZE * 2];
} N256;
typedef struct {
    uint8_t val[NUMBERSIZE * 4];
} N512;

/*extra wider and narrower integer, for middle operation usage*/
typedef struct {
    uint8_t val[NUMBERSIZE * 8];
} N1024;
typedef struct {
    uint8_t val[NUMBERSIZE / 2];
} N64;

/*test and def NULL pointer, for convinence*/
#ifndef NULL
#define NULL (void *)0
#endif

/**
 * Addition a+b->a
 * input: a, b
 * output: a
 * return: void
 */
static inline void addw(N128 *a, const N128 *b)
{
    uint32_t *pa, *pb;
    uint64_t sum, of;

    pa = (uint32_t *)(a->val);
    pb = (uint32_t *)(b->val);
    of = 0;

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        sum = *pa;
        sum += *pb;
        sum += of;
        of = (sum >> 32);
        *pa = sum;

        pa++;
        pb++;
    }
}
static inline void addd(N256 *a, const N256 *b)
{
    uint32_t *pa, *pb;
    uint64_t sum, of;

    pa = (uint32_t *)(a->val);
    pb = (uint32_t *)(b->val);
    of = 0;

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        sum = *pa;
        sum += *pb;
        sum += of;
        of = (sum >> 32);
        *pa = sum;

        pa++;
        pb++;
    }
}
static inline void addq(N512 *a, const N512 *b)
{
    uint32_t *pa, *pb;
    uint64_t sum, of;

    pa = (uint32_t *)(a->val);
    pb = (uint32_t *)(b->val);
    of = 0;

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        sum = *pa;
        sum += *pb;
        sum += of;
        of = (sum >> 32);
        *pa = sum;

        pa++;
        pb++;
    }
}
/**
 * Comparatiom
 * input: a, b
 * output: none
 * return: 1 if a>b, 0 if a==b, -1 if a<b
 */
static inline int cmpw(const N128 *a, const N128 *b)
{
    uint32_t *pa, *pb;

    pa = (uint32_t *)(a->val) + (sizeof(a->val) >> 2) - 1;
    pb = (uint32_t *)(b->val) + (sizeof(b->val) >> 2) - 1;

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        if(*pa == *pb) {
            pa--;
            pb--;
        } else {
            return (*pa > *pb) ? 1 : -1;
        }
    }
    return 0;
}
static inline int cmpd(const N256 *a, const N256 *b)
{
    uint32_t *pa, *pb;

    pa = (uint32_t *)(a->val) + (sizeof(a->val) >> 2) - 1;
    pb = (uint32_t *)(b->val) + (sizeof(b->val) >> 2) - 1;

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        if(*pa == *pb) {
            pa--;
            pb--;
        } else {
            return (*pa > *pb) ? 1 : -1;
        }
    }
    return 0;
}
static inline int cmpq(const N512 *a, const N512 *b)
{
    uint32_t *pa, *pb;

    pa = (uint32_t *)(a->val) + (sizeof(a->val) >> 2) - 1;
    pb = (uint32_t *)(b->val) + (sizeof(b->val) >> 2) - 1;

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        if(*pa == *pb) {
            pa--;
            pb--;
        } else {
            return (*pa > *pb) ? 1 : -1;
        }
    }
    return 0;
}

/**
 * Increase 1
 * input: a
 * output: a
 * return: void
 */
static inline void incw(N128 *a)
{
    uint32_t *pa;

    pa = (uint32_t *)(a->val);

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        if(++(*pa) == 0) {
            pa++;
        } else
            break;
    }
}
static inline void incd(N256 *a)
{
    uint32_t *pa;

    pa = (uint32_t *)(a->val);

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        if(++(*pa) == 0) {
            pa++;
        } else
            break;
    }
}
static inline void incq(N512 *a)
{
    uint32_t *pa;

    pa = (uint32_t *)(a->val);

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        if(++(*pa) == 0) {
            pa++;
        } else
            break;
    }
}

/**
 * Decrease 1
 * input: a
 * output: a
 * return: void
 */
static inline void decw(N128 *a)
{
    uint32_t *pa;
    uint64_t sum, of;

    pa = (uint32_t *)(a->val);
    of = 0;

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        sum = *pa;
        sum += 0xffffffff;
        sum += of;
        of = (sum >> 32);
        *pa = sum;

        pa++;
    }
}
static inline void decd(N256 *a)
{
    uint32_t *pa;
    uint64_t sum, of;

    pa = (uint32_t *)(a->val);
    of = 0;

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        sum = *pa;
        sum += 0xffffffff;
        sum += of;
        of = (sum >> 32);
        *pa = sum;

        pa++;
    }
}
static inline void decq(N512 *a)
{
    uint32_t *pa;
    uint64_t sum, of;

    pa = (uint32_t *)(a->val);
    of = 0;

    for(int i = 0; i < (sizeof(a->val) >> 2); i++) {
        sum = *pa;
        sum += 0xffffffff;
        sum += of;
        of = (sum >> 32);
        *pa = sum;

        pa++;
    }
}

/**
 * Copy src to dst
 * input: src
 * output: dst
 * return: void
 */
static inline void copyw(N128 *dst, const N128 *src)
{
    uint64_t *pa, *pb;

    pa = (uint64_t *)(dst->val);
    pb = (uint64_t *)(src->val);

    for(int i = 0; i < (sizeof(dst->val) >> 3); i++) {
        pa[i] = pb[i];
    }
}
static inline void copyd(N256 *dst, const N256 *src)
{
    uint64_t *pa, *pb;

    pa = (uint64_t *)(dst->val);
    pb = (uint64_t *)(src->val);

    for(int i = 0; i < (sizeof(dst->val) >> 3); i++) {
        pa[i] = pb[i];
    }
}
static inline void copyq(N512 *dst, const N512 *src)
{
    uint64_t *pa, *pb;

    pa = (uint64_t *)(dst->val);
    pb = (uint64_t *)(src->val);

    for(int i = 0; i < (sizeof(dst->val) >> 3); i++) {
        pa[i] = pb[i];
    }
}
static inline void copyhw(N64 *dst, const N64 *src) {
    uint64_t *pa, *pb;

    pa = (uint64_t *)(dst->val);
    pb = (uint64_t *)(src->val);

    for(int i = 0; i < (sizeof(dst->val) >> 3); i++) {
        pa[i] = pb[i];
    }
}

/**
 * Set zero
 * input: a
 * output: a
 * return: void
 */
static inline void wzero(N128 *a)
{
    uint64_t *pa;

    pa = (uint64_t *)(a->val);

    for(int i = 0; i < (sizeof(a->val) >> 3); i++)
        pa[i] = 0;
}
static inline void dzero(N256 *a)
{
    uint64_t *pa;

    pa = (uint64_t *)(a->val);

    for(int i = 0; i < (sizeof(a->val) >> 3); i++)
        pa[i] = 0;
}
static inline void qzero(N512 *a)
{
    uint64_t *pa;

    pa = (uint64_t *)(a->val);

    for(int i = 0; i < (sizeof(a->val) >> 3); i++)
        pa[i] = 0;
}
static inline void zero1024(N1024 *a)
{
    uint64_t *pa;

    pa = (uint64_t *)(a->val);

    for(int i = 0; i < (sizeof(a->val) >> 3); i++)
        pa[i] = 0;
}
static inline void hwzero(N64 *a)
{
    uint64_t *pa;

    pa = (uint64_t *)(a->val);

    for(int i = 0; i < (sizeof(a->val) >> 3); i++)
        pa[i] = 0;
}

/**
 * Cast
 * input: a
 * output: none
 * return: a struct obj
 */
static inline N256 castwtd(const N128 *a)
{
    uint64_t *pa, *pb;
    N256 b = {0};

    pa = (uint64_t *)(a->val);
    pb = (uint64_t *)(b.val);

    for(int i = 0; i < (sizeof(a->val) >> 3); i++)
        pb[i] = pa[i];
    return b;
}
static inline N512 castdtq(const N256 *a)
{
    uint64_t *pa, *pb;
    N512 b = {0};

    pa = (uint64_t *)(a->val);
    pb = (uint64_t *)(b.val);

    for(int i = 0; i < (sizeof(a->val) >> 3); i++)
        pb[i] = pa[i];
    return b;
}
static inline N128 castdtw(const N256 *a)
{
    uint64_t *pa, *pb;
    N128 b = {0};

    pa = (uint64_t *)(a->val);
    pb = (uint64_t *)(b.val);

    for(int i = 0; i < (sizeof(b.val) >> 3); i++)
        pb[i] = pa[i];
    return b;
}
static inline N256 castqtd(const N512 *a)
{
    uint64_t *pa, *pb;
    N256 b = {0};

    pa = (uint64_t *)(a->val);
    pb = (uint64_t *)(b.val);

    for(int i = 0; i < (sizeof(b.val) >> 3); i++)
        pb[i] = pa[i];
    return b;
}
static inline N512 cast1024tq(const N1024 *a)
{
    uint64_t *pa, *pb;
    N512 b = {0};

    pa = (uint64_t *)(a->val);
    pb = (uint64_t *)(b.val);

    for(int i = 0; i < (sizeof(b.val) >> 3); i++)
        pb[i] = pa[i];
    return b;
}
static inline N128 casthwtw(const N64 *a)
{
    uint64_t *pa, *pb;
    N128 b = {0};

    pa = (uint64_t *)(a->val);
    pb = (uint64_t *)(b.val);

    for(int i = 0; i < (sizeof(a->val) >> 3); i++)
        pb[i] = pa[i];
    return b;
}

/**
 * Skip leading zeros and count
 * input: a, p
 * output: p
 * return: leading zeros count of a, by uint32_t
 */
static inline size_t skiplzw(const N128 *a, uint32_t **p)
{
    uint32_t *pa;
    size_t n;

    pa = (uint32_t *)(a->val) + (sizeof(a->val) >> 2) - 1;
    for(n = 0; n < (sizeof(a->val) >> 2); n++) {
        if(*pa == 0)
            pa--;
        else
            break;
    }
    if(p)
        *p = pa;
    return n;
}
static inline size_t skiplzd(const N256 *a, uint32_t **p)
{
    uint32_t *pa;
    size_t n;

    pa = (uint32_t *)(a->val) + (sizeof(a->val) >> 2) - 1;
    for(n = 0; n < (sizeof(a->val) >> 2); n++) {
        if(*pa == 0)
            pa--;
        else
            break;
    }
    if(p)
        *p = pa;
    return n;
}
static inline size_t skiplzq(const N512 *a, uint32_t **p)
{
    uint32_t *pa;
    size_t n;

    pa = (uint32_t *)(a->val) + (sizeof(a->val) >> 2) - 1;
    for(n = 0; n < (sizeof(a->val) >> 2); n++) {
        if(*pa == 0)
            pa--;
        else
            break;
    }
    if(p)
        *p = pa;
    return n;
}

/**
 * Count leading zeros, accurate to byte
 * input: a
 * output: none
 * return: leading zeros count of a, by uint8_t
 */
static inline size_t cntlzw(const N128 *a)
{
    const uint8_t *pa;
    size_t n;

    pa = a->val + sizeof(a->val) - 1;
    for(n = 0; n < sizeof(a->val); n++) {
        if(*pa == 0)
            pa--;
        else
            break;
    }
    return n;
}
static inline size_t cntlzd(const N256 *a)
{
    const uint8_t *pa;
    size_t n;

    pa = a->val + sizeof(a->val) - 1;
    for(n = 0; n < sizeof(a->val); n++) {
        if(*pa == 0)
            pa--;
        else
            break;
    }
    return n;
}
static inline size_t cntlzq(const N512 *a)
{
    const uint8_t *pa;
    size_t n;

    pa = a->val + sizeof(a->val) - 1;
    for(n = 0; n < sizeof(a->val); n++) {
        if(*pa == 0)
            pa--;
        else
            break;
    }
    return n;
}

/**
 * bit length
 * input: a
 * output: none
 * return: bit length of a
 */
static inline size_t bitLenw(const N128 *a)
{
    uint32_t *pa, val;
    size_t len;

    len = (sizeof(a->val) >> 2) -skiplzw(a, &pa);
    if (len) {
        len--;
        len <<= 5;     
    } else {
        return 0;
    }
    val = *pa;
    while(val) {
        val >>= 1;
        len++;
    }

    return len;
}
static inline size_t bitLend(const N256 *a)
{
    uint32_t *pa, val;
    size_t len;

    len = (sizeof(a->val) >> 2) -skiplzd(a, &pa);
    if (len) {
        len--;
        len <<= 5;     
    } else {
        return 0;
    }
    val = *pa;
    while(val) {
        val >>= 1;
        len++;
    }

    return len;
}
static inline size_t bitLenq(const N512 *a)
{
    uint32_t *pa, val;
    size_t len;

    len = (sizeof(a->val) >> 2) -skiplzq(a, &pa);
    if (len) {
        len--;
        len <<= 5;     
    } else {
        return 0;
    }
    val = *pa;
    while(val) {
        val >>= 1;
        len++;
    }

    return len;
}

/**
 * Additive inverse
 * input: a, inv
 * output: inv
 * return: void
 */
static inline void invw(const N128 *restrict a, N128 *restrict inv)
{
    uint64_t *pa, *pi;

    pa = (uint64_t *)(a->val);
    pi = (uint64_t *)(inv->val);
    for(int i = 0; i < (sizeof(a->val) >> 3); i++) {
        pi[i] = ~pa[i];
    }
    incw(inv);
}
static inline void invd(const N256 *restrict a, N256 *restrict inv)
{
    uint64_t *pa, *pi;

    pa = (uint64_t *)(a->val);
    pi = (uint64_t *)(inv->val);
    for(int i = 0; i < (sizeof(a->val) >> 3); i++) {
        pi[i] = ~pa[i];
    }
    incd(inv);
}
static inline void invq(const N512 *restrict a, N512 *restrict inv)
{
    uint64_t *pa, *pi;

    pa = (uint64_t *)(a->val);
    pi = (uint64_t *)(inv->val);
    for(int i = 0; i < (sizeof(a->val) >> 3); i++) {
        pi[i] = ~pa[i];
    }
    incq(inv);
}

/**
 * Arithmetic shift left n bits, work on little endian machine only
 * input: a, n
 * output: a
 * return: void
 */
static inline void salw(N128 *a, size_t n)
{
    size_t q, r;
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        } v32;
    } val;
    uint32_t tail, *pa;

    q = n >> 3;
    r = n & 0x7;

    if(q) {
        for(int i = (sizeof(a->val) - 1); i >= 0; i--) {
            if((i + q) < sizeof(a->val))
                a->val[i + q] = a->val[i];
            a->val[i] = 0;
        }
    }

    pa = (uint32_t *)(a->val);
    if(r) {
        tail = 0;
        for(int j = 0; j < (sizeof(a->val) >> 2); j++) {
            val.v64 = *pa;
            val.v64 <<= r;
            *pa = (val.v32.l | tail);
            tail = val.v32.r;
            pa++;
        }
    }
}
static inline void sald(N256 *a, size_t n)
{
    uint32_t q, r;
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        } v32;
    } val;
    uint32_t tail, *pa;

    q = n >> 3;
    r = n & 0x7;

    if(q) {
        for(int i = (sizeof(a->val) - 1); i >= 0; i--) {
            if((i + q) < sizeof(a->val))
                a->val[i + q] = a->val[i];
            a->val[i] = 0;
        }
    }

    pa = (uint32_t *)(a->val);
    if(r) {
        tail = 0;
        for(int j = 0; j < (sizeof(a->val) / 4); j++) {
            val.v64 = *pa;
            val.v64 <<= r;
            *pa = (val.v32.l | tail);
            tail = val.v32.r;
            pa++;
        }
    }
}
static inline void salq(N512 *a, size_t n)
{
    uint32_t q, r;
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        } v32;
    } val;
    uint32_t tail, *pa;

    q = n >> 3;
    r = n & 0x7;

    if(q) {
        for(int i = (sizeof(a->val) - 1); i >= 0; i--) {
            if((i + q) < sizeof(a->val))
                a->val[i + q] = a->val[i];
            a->val[i] = 0;
        }
    }

    pa = (uint32_t *)(a->val);
    if(r) {
        tail = 0;
        for(int j = 0; j < (sizeof(a->val) / 4); j++) {
            val.v64 = *pa;
            val.v64 <<= r;
            *pa = (val.v32.l | tail);
            tail = val.v32.r;
            pa++;
        }
    }
}

/**
 * Arithmetic right shift n bits
 * input: a, n
 * output: a
 * return: void
 */
static inline void sarw(N128 *a, size_t n)
{
    size_t zcnta, q, r;
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        } v32;
    } val;
    uint32_t tail, *pa;

    zcnta = skiplzw(a, &pa);
    q = n >> 3;
    r = n & 0x7;

    if(n >= (sizeof(a->val) << 3) - (zcnta << 5)) {
        wzero(a);
        return;
    }

    if(q) {
        for(int i = 0; i < sizeof(a->val); i++) {
            if(i >= q)
                a->val[i - q] = a->val[i];
            a->val[i] = 0;
        }
    }

    if(r) {
        tail = 0;
        while(pa != (uint32_t *)(a->val) - 1) {
            val.v32.l = 0;
            val.v32.r = *pa;
            val.v64 >>= r;
            *pa = (tail | val.v32.r);
            tail = val.v32.l;
            pa--;
        }
    }
}
static inline void sard(N256 *a, size_t n)
{
    uint32_t zcnta, q, r;
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        } v32;
    } val;
    uint32_t tail, *pa;

    zcnta = skiplzd(a, &pa);
    q = n >> 3;
    r = n & 0x7;

    if(n >= (sizeof(a->val) << 3) - (zcnta << 5)) {
        dzero(a);
        return;
    }

    if(q) {
        for(int i = 0; i < sizeof(a->val); i++) {
            if(i >= q)
                a->val[i - q] = a->val[i];
            a->val[i] = 0;
        }
    }

    if(r) {
        tail = 0;
        while(pa != (uint32_t *)(a->val) - 1) {
            val.v32.l = 0;
            val.v32.r = *pa;
            val.v64 >>= r;
            *pa = (tail | val.v32.r);
            tail = val.v32.l;
            pa--;
        }
    }
}
static inline void sarq(N512 *a, size_t n)
{
    uint32_t zcnta, q, r;
    union {
        uint64_t v64;
        struct {
            uint32_t l;
            uint32_t r;
        } v32;
    } val;
    uint32_t tail, *pa;

    zcnta = skiplzq(a, &pa);
    q = n >> 3;
    r = n & 0x7;

    if(n >= (sizeof(a->val) << 3) - (zcnta << 5)) {
        qzero(a);
        return;
    }

    if(q) {
        for(int i = 0; i < sizeof(a->val); i++) {
            if(i >= q)
                a->val[i - q] = a->val[i];
            a->val[i] = 0;
        }
    }

    if(r) {
        tail = 0;
        while(pa != (uint32_t *)(a->val) - 1) {
            val.v32.l = 0;
            val.v32.r = *pa;
            val.v64 >>= r;
            *pa = (tail | val.v32.r);
            tail = val.v32.l;
            pa--;
        }
    }
}

/**
 * generate a mask
 * input: a
 * output: mask
 * return: void
 */
static inline void wToMask(const N128 *a, N128 *mask)
{
    uint32_t *pm;
    size_t len, cnt;

    copyw(mask, a);
    pm = (uint32_t *)(mask->val);
    len = bitLenw(mask);
    for(cnt = 0; cnt < (len >> 5); cnt++) {
        pm[cnt] = 0xffffffff;
    }
    if(pm[cnt]) {
        pm[cnt] = 0xffffffff;
        pm[cnt] >>= (32 - (len & 0x1f));
    }
}
static inline void dToMask(const N256 *a, N256 *mask)
{
    uint32_t *pm;
    size_t len, cnt;

    copyd(mask, a);
    pm = (uint32_t *)(mask->val);
    len = bitLend(mask);
    for(cnt = 0; cnt < (len >> 5); cnt++) {
        pm[cnt] = 0xffffffff;
    }
    if(pm[cnt]) {
        pm[cnt] = 0xffffffff;
        pm[cnt] >>= (32 - (len & 0x1f));
    }
}
static inline void qToMask(const N512 *a, N512 *mask)
{
    uint32_t *pm;
    size_t len, cnt;

    copyq(mask, a);
    pm = (uint32_t *)(mask->val);
    len = bitLenq(mask);
    for(cnt = 0; cnt < (len >> 5); cnt++) {
        pm[cnt] = 0xffffffff;
    }
    if(pm[cnt]) {
        pm[cnt] = 0xffffffff;
        pm[cnt] >>= (32 - (len & 0x1f));
    }
}

/**
 * Mask operation
 * input: a, mask
 * output: a
 * return: void
 */
static inline void maskw(N128 *a, const N128 *mask)
{
    uint64_t *pa, *pm;

    pa = (uint64_t *)(a->val);
    pm = (uint64_t *)(mask->val);
    for(int i = 0; i < (sizeof(a->val) >> 3); i++) {
        pa[i] &= pm[i];
    }
}
static inline void maskd(N256 *a, N256 *mask)
{
    uint64_t *pa, *pm;

    pa = (uint64_t *)(a->val);
    pm = (uint64_t *)(mask->val);
    for(int i = 0; i < (sizeof(a->val) >> 3); i++) {
        pa[i] &= pm[i];
    }
}
static inline void maskq(N512 *a, N512 *mask)
{
    uint64_t *pa, *pm;

    pa = (uint64_t *)(a->val);
    pm = (uint64_t *)(mask->val);
    for(int i = 0; i < (sizeof(a->val) >> 3); i++) {
        pa[i] &= pm[i];
    }
}

/**
 * Basic mod operation, a mod b
 * input: a, b
 * output: a
 * return: void
 */
static inline void modw(N128 *a, const N128 *b, const N128 *bmask)
{
    size_t lenOfA, lenOfB, weight, cnt;                 /*bits width, value weight and loop counter*/
    N128 ha, la, aval, maskb, invb;                     /*middle results*/
    
    if (bmask)                                          /*init vars*/
        copyw(&maskb, bmask);
    else
        wToMask(b, &maskb);
    
    invw(b, &invb);                                     /*-b*/
    lenOfB = bitLenw(b);
    lenOfA = bitLenw(a);

    if (lenOfB == 0) {                                  /*arithmetic error*/
        raise(SIGFPE);
        return;
    }

    wzero(&aval);                                       /*a=0*/
    weight = 0;                                         /*init weight = 0*/
    while (lenOfA) {
        copyw(&la, a);
        sarw(&la, weight * lenOfB);
        maskw(&la, &maskb);
        copyw(&ha, a);
        sarw(&ha, (weight + 1) * lenOfB);
        maskw(&ha, &maskb);

        if (cmpw(&la, b) >= 0) {
            addw(&la, &invb);
        }
        cnt = (weight * lenOfB);
        while (cnt) {
            salw(&la, 1);
            if (cmpw(&la, b) >= 0) {
                addw(&la, &invb);
            }
            cnt--;
        }
        addw(&aval, &la);

        if (cmpw(&aval, b) >= 0) {
            addw(&aval, &invb);
        }
        
        if (lenOfA > lenOfB) {                          /*if lenOfA > lenOfB, split a*/
            if (cmpw(&ha, b) >= 0) {
                addw(&ha, &invb);
            }
            cnt = (weight + 1) * lenOfB;
            while (cnt) {
                salw(&ha, 1);
                if (cmpw(&ha, b) >= 0) {
                    addw(&ha, &invb);
                }
                cnt--;
            }
            addw(&aval, &ha);

            if (cmpw(&aval, b) >= 0) {
                addw(&aval, &invb);
            }
        }

        weight += 2;
        if (lenOfA > (lenOfB << 1))
            lenOfA -= (lenOfB << 1);
        else 
            lenOfA = 0;
    }
    copyw(a, &aval);
}
static inline void modd(N256 *a, const N256 *b, const N256 *bmask)
{
    size_t lenOfA, lenOfB, weight, cnt;                 /*bits width, value weight and loop counter*/
    N256 ha, la, aval, maskb, invb;                     /*middle results*/
    
    if (bmask)                                          /*init vars*/
        copyd(&maskb, bmask);
    else
        dToMask(b, &maskb);
    
    invd(b, &invb);                                     /*-b*/
    lenOfB = bitLend(b);
    lenOfA = bitLend(a);

    if (lenOfB == 0) {                                  /*arithmetic error*/
        raise(SIGFPE);
        return;
    }

    dzero(&aval);                                       /*a=0*/
    weight = 0;                                         /*init weight = 0*/
    while (lenOfA) {
        copyd(&la, a);
        sard(&la, weight * lenOfB);
        maskd(&la, &maskb);
        copyd(&ha, a);
        sard(&ha, (weight + 1) * lenOfB);
        maskd(&ha, &maskb);

        if (cmpd(&la, b) >= 0) {
            addd(&la, &invb);
        }
        cnt = (weight * lenOfB);
        while (cnt) {
            sald(&la, 1);
            if (cmpd(&la, b) >= 0) {
                addd(&la, &invb);
            }
            cnt--;
        }
        addd(&aval, &la);

        if (cmpd(&aval, b) >= 0) {
            addd(&aval, &invb);
        }
        
        if (lenOfA > lenOfB) {                          /*if lenOfA > lenOfB, split a*/
            if (cmpd(&ha, b) >= 0) {
                addd(&ha, &invb);
            }
            cnt = (weight + 1) * lenOfB;
            while (cnt) {
                sald(&ha, 1);
                if (cmpd(&ha, b) >= 0) {
                    addd(&ha, &invb);
                }
                cnt--;
            }
            addd(&aval, &ha);

            if (cmpd(&aval, b) >= 0) {
                addd(&aval, &invb);
            }
        }

        weight += 2;
        if (lenOfA > (lenOfB << 1))
            lenOfA -= (lenOfB << 1);
        else 
            lenOfA = 0;
    }
    copyd(a, &aval);
}
static inline void modq(N512 *a, const N512 *b, const N512 *bmask)
{
    size_t lenOfA, lenOfB, weight, cnt;                 /*bits width, value weight and loop counter*/
    N512 ha, la, aval, maskb, invb;                     /*middle results*/
    
    if (bmask)                                          /*init vars*/
        copyq(&maskb, bmask);
    else
        qToMask(b, &maskb);
    
    invq(b, &invb);                                     /*-b*/
    lenOfB = bitLenq(b);
    lenOfA = bitLenq(a);

    if (lenOfB == 0) {                                  /*arithmetic error*/
        raise(SIGFPE);
        return;
    }

    qzero(&aval);                                       /*a=0*/
    weight = 0;                                         /*init weight = 0*/
    while (lenOfA) {
        copyq(&la, a);
        sarq(&la, weight * lenOfB);
        maskq(&la, &maskb);
        copyq(&ha, a);
        sarq(&ha, (weight + 1) * lenOfB);
        maskq(&ha, &maskb);

        if (cmpq(&la, b) >= 0) {
            addq(&la, &invb);
        }
        cnt = (weight * lenOfB);
        while (cnt) {
            salq(&la, 1);
            if (cmpq(&la, b) >= 0) {
                addq(&la, &invb);
            }
            cnt--;
        }
        addq(&aval, &la);

        if (cmpq(&aval, b) >= 0) {
            addq(&aval, &invb);
        }
        
        if (lenOfA > lenOfB) {                          /*if lenOfA > lenOfB, split a*/
            if (cmpq(&ha, b) >= 0) {
                addq(&ha, &invb);
            }
            cnt = (weight + 1) * lenOfB;
            while (cnt) {
                salq(&ha, 1);
                if (cmpq(&ha, b) >= 0) {
                    addq(&ha, &invb);
                }
                cnt--;
            }
            addq(&aval, &ha);

            if (cmpq(&aval, b) >= 0) {
                addq(&aval, &invb);
            }
        }

        weight += 2;
        if (lenOfA > (lenOfB << 1))
            lenOfA -= (lenOfB << 1);
        else 
            lenOfA = 0;
    }
    copyq(a, &aval);
}

/**
 * Multiplication with no return, discard overflow bits
 * Reentrantable
 * input:   a, b
 * output:  a
 * return:  void
 */
static inline void vmultw(N128 *a, N128 *b)
{
    N256 midval;                                                            /*middle and final result*/
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

    for(size_t cnt1 = 0; cnt1 < (sizeof(b->val) >> 2) - zcntb; cnt1++) {    /*outter loop depends on len of b*/
        vb.v64 = *pb++;                                                     /*set vb*/
        pa = (uint32_t *)(a->val);                                          /*set pa*/
        pmid = (uint32_t *)(midval.val) + cnt1;                             /*set pmid, pmid moving with outter loop*/

        ofadd = 0;                                                          /*init ofadd and ofmlt*/
        ofmlt = 0;
        for(size_t cnt2 = 0; cnt2 < (sizeof(a->val) >> 2) -zcnta; cnt2++) { /*inner loop depends on len of a*/
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
}
static inline void vmultd(N256 *a, N256 *b)
{
    N512 midval;
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

    for(size_t cnt1 = 0; cnt1 < (sizeof(b->val) >> 2) - zcntb; cnt1++) {
        vb.v64 = *pb++;
        pa = (uint32_t *)(a->val);
        pmid = (uint32_t *)(midval.val) + cnt1;

        ofadd = 0;
        ofmlt = 0;
        for(size_t cnt2 = 0; cnt2 < (sizeof(a->val) >> 2) - zcnta; cnt2++) {
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
}
static inline void vmultq(N512 *a, N512 *b)
{
    N1024 midval;
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

    for(size_t cnt1 = 0; cnt1 < (sizeof(b->val) >> 2) - zcntb; cnt1++) {
        vb.v64 = *pb++;
        pa = (uint32_t *)(a->val);
        pmid = (uint32_t *)(midval.val) + cnt1;

        ofadd = 0;
        ofmlt = 0;
        for(size_t cnt2 = 0; cnt2 < (sizeof(a->val) >> 2) -zcnta; cnt2++) {
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
}

/**
 * Other integer operation
 */

extern N256 *multw(N128 *a, N128 *b);
extern N512 *multd(N256 *a, N256 *b);
extern N1024 *multq(N512 *a, N512 *b);

extern void modew(N128 *a, const N128 *b, N128 *rem, N128 *quo, const N128 *bmask);
extern void moded(N256 *a, const N256 *b, N256 *rem, N256 *quo, const N256 *bmask);
extern void modeq(N512 *a, const N512 *b, N512 *rem, N512 *quo, const N512 *bmask);

extern int gcdw(const N128 *a, const N128 *b, N128 *g);
extern int gcdd(const N256 *a, const N256 *b, N256 *g);
extern int gcdq(const N512 *a, const N512 *b, N512 *g);

extern int linearEquitionw(const N128 *a, const N128 *b, N128 *g, N128 *x, N128 *y);
extern int linearEquitiond(const N256 *a, const N256 *b, N256 *g, N256 *x, N256 *y);

extern int powerModew(const N128 *a, const N128 *b, const N128 *k, N128 *rem);
extern int powerModed(const N256 *a, const N256 *b, const N256 *k, N256 *rem);

extern char *wToString(const N128 *a);
extern char *dToString(const N256 *a);
extern char *qToString(const N512 *a);

extern N128 *stringTow(const char *src,size_t len);
extern N256 *stringTod(const char *src, size_t len);
extern N512 *stringToq(const char *src, size_t len);

extern int primeGenhw(N64 *prime, int t);
extern int primeGenw(N128 *prime, int t, size_t *realLen);
extern int primeGend(N256 *prime, int t, size_t *realLen);

#ifdef __cpluscplus
}
#endif

#endif  /*BIGNUMBER_H*/