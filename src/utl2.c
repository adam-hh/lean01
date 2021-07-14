#include "bigNumber.h"
#include "RSAkey.h"

/**
 * GCD func
 * input: a, b
 * output: g
 * return: 1 for success, -1 for fail
 */
int gcdw(const N128 *a, const N128 *b, N128 *g)
{
    N128 aval, bval, rem;                                       /*middle results*/

    if((a == NULL) || (b == NULL)) {                            /*illegal para*/
        raise(SIGFPE);                                          /*raise SIGFPE, should terminal the process by default*/
        return -1;
    }
    copyw(&aval, a);                                            /*init vars*/
    copyw(&bval, b);

    while(skiplzw(&bval, NULL) != (sizeof(bval.val) >> 2)) {     /*loop condition: bval not equals 0*/
        modew(&aval, &bval, &rem, NULL, NULL);                        /*aval mod bval -> rem*/
        copyw(&aval, &bval);                                    /*bval -> aval*/
        copyw(&bval, &rem);                                     /*rem -> bval*/
    }

    if(g) {                                                     /*g is not NULL*/
        copyw(g, &aval);                                        /*aval -> g*/
        return 1;
    }
    raise(SIGFPE);                                              /*g is NULL, exception*/
    return -1;
}
int gcdd(const N256 *a, const N256 *b, N256 *g)
{
    N256 aval, bval, rem;

    if((a == NULL) || (b == NULL)) {
        raise(SIGFPE);
        return -1;
    }
    copyd(&aval, a);
    copyd(&bval, b);

    while(skiplzd(&bval, NULL) != (sizeof(bval.val) >> 2)) {
        moded(&aval, &bval, &rem, NULL, NULL);
        copyd(&aval, &bval);
        copyd(&bval, &rem);
    }

    if(g) {
        copyd(g, &aval);
        return 1;
    }
    raise(SIGFPE);
    return -1;
}
int gcdq(const N512 *a, const N512 *b, N512 *g)
{
    N512 aval, bval, rem;

    if((a == NULL) || (b == NULL)) {
        raise(SIGFPE);
        return -1;
    }
    copyq(&aval, a);
    copyq(&bval, b);

    while(skiplzq(&bval, NULL) != (sizeof(bval.val) >> 2)) {
        modeq(&aval, &bval, &rem, NULL, NULL);
        copyq(&aval, &bval);
        copyq(&bval, &rem);
    }

    if(g) {
        copyq(g, &aval);
        return 1;
    }
    raise(SIGFPE);
    return -1;
}

/**
 * LinearEquition ax+by=gcd(a,b)
 * input: a, b
 * output: g, x, y
 * return: 1 for success, -1 for fail
 */
int linearEquitionw(const N128 *restrict a, const N128 *restrict b, N128 *restrict g, N128 *restrict x, N128 *restrict y)
{
    N256 x256, s, v, y256;                                          /*middle results for solutions*/
    N256 inv, q256, a256, b256, r256;                               /*other middle results*/

    if((a == NULL) || (b == NULL) || (g == NULL) || (x == NULL)) {  /*para check*/
        raise(SIGFPE);                                              /*raise SIGFPE, should terminal the process by default*/
        return -1;
    }
    dzero(&x256);                                                   /*init vars*/
    x256.val[0] = 1;                                                /*give x 1*/
    dzero(&v);                                                      /*give v 0*/
    dzero(&s);
    a256 = castwtd(a);
    b256 = castwtd(b);

    while(skiplzd(&b256, NULL) != (sizeof(b256.val) >> 2)) {         /*loop condition: b not equals 0*/
        moded(&a256, &b256, &r256, &q256, NULL);                    /*a mod b, remainder to rem, quotient to quo*/

        invd(&v, &inv);                                             /*v = -v*/
        vmultd(&q256, &inv);                                        /*q=q*(-v)*/
        //copyd(&s, &x256);
        //addd(&s, &q256);
        addd(&x256, &q256);                                         /*x=x-q*v*/
        //addd(&q256, &x256);
        //copyd(&s, &q256);
        copyd(&s, &x256);                                           /* s = x - q*v */

        copyd(&x256, &v);                                           /*v->x*/
        copyd(&v, &s);                                              /*s->v*/

        copyd(&a256, &b256);                                        /*b->a*/
        copyd(&b256, &r256);                                        /*r->b*/
    }

    b256 = castwtd(b);                                              /*original b*/
    while(x256.val[255] >= 128) {                                   /*condition: x < 0*/
        addd(&x256, &b256);                                         /*x+b/g*/
    }
    *x = castdtw(&x256);                                            /*now x is positive, cast x to N128*/

    if(y) {                                                         /*resolve y if y is not NULL, y=(g-a*x)/b*/
        copyd(&y256, &a256);                                        /*got gcd*/
        a256 = castwtd(a);                                          /*original a*/
        invd(&a256, &inv);                                          /*a=-a*/
        vmultd(&x256, &inv);                                        /*x=x*(-a))*/
        addd(&y256, &x256);                                         /*y=g-a*x*/
        moded(&y256, &b256, NULL, &y256, NULL);                           /*y=y/b, quotient*/

        invd(&y256, &inv);                                          /*y256 is nagative, so reverse it*/
        *y = castdtw(&inv);                                         /*now y is abs of y256*/
    }

    if(g) {                                                         /*g is not NULL*/
        *g = castdtw(&a256);                                        /*set g then return*/
        return 1;
    }
    raise(SIGFPE);                                                  /*g is NULL, exception*/
    return -1;
}
int linearEquitiond(const N256 *restrict a, const N256 *restrict b, N256 *restrict g, N256 *restrict x, N256 *restrict y)
{
    N512 x512, s, v, y512;                                          /*middle results for solutions*/
    N512 inv, q512, a512, b512, r512;                               /*other middle results*/

    if((a == NULL) || (b == NULL) || (g == NULL) || (x == NULL)) {  /*para check*/
        raise(SIGFPE);                                              /*raise SIGFPE, should terminal the process by default*/
        return -1;
    }
    qzero(&x512);                                                   /*init vars*/
    x512.val[0] = 1;                                                /*give x 1*/
    qzero(&v);                                                      /*give v 0*/
    a512 = castdtq(a);
    b512 = castdtq(b);

    while(skiplzq(&b512, NULL) != (sizeof(b512.val) >> 2)) {         /*loop condition: b not equals 0*/
        modeq(&a512, &b512, &r512, &q512, NULL);                          /*a mod b, remainder to rem, quotient to quo*/

        invq(&v, &inv);                                             /*v = -v*/
        vmultq(&q512, &inv);                                        /*q=q*(-v)*/
        addq(&x512, &q512);                                         /*x=x-q*v*/
        copyq(&s, &x512);                                           /* s = x - q*v */

        copyq(&x512, &v);                                           /*v->x*/
        copyq(&v, &s);                                              /*s->v*/

        copyq(&a512, &b512);                                        /*b->a*/
        copyq(&b512, &r512);                                        /*r->b*/
    }

    b512 = castdtq(b);                                              /*original b*/
    while(x512.val[511] >= 128) {                                   /*condition: x < 0*/
        addq(&x512, &b512);                                         /*x+b/g*/
    }
    *x = castqtd(&x512);                                            /*now x is positive, cast x to N256*/

    if(y) {                                                         /*resolve y if y is not NULL, y=(g-a*x)/b*/
        copyq(&y512, &a512);                                        /*got gcd*/
        a512 = castdtq(a);                                          /*original a*/
        invq(&a512, &inv);                                          /*a=-a*/
        vmultq(&x512, &inv);                                         /*x=x*(-a))*/
        addq(&y512, &x512);                                         /*y=g-a*x*/
        modeq(&y512, &b512, NULL, &y512, NULL);                           /*y=y/b, quotient*/

        invq(&y512, &inv);                                          /*y512 is nagative, so reverse it*/
        *y = castqtd(&inv);                                         /*now y is abs of y512*/
    }

    if(g) {                                                         /*g is not NULL*/
        *g = castqtd(&a512);                                        /*set g then return*/
        return 1;
    }
    raise(SIGFPE);                                                  /*g is NULL, exception*/
    return -1;
}

/**
 * Generage private key, need gcd(publicKey, fai) == 1
 * input: key(key->publicKey, key->p, key->q)
 * output: key(key->mode, key->privateKey, key->fai)
 * return: 1 for sucess, -1 for fail
 */
int genPrivatekey1024(struct rsakey1024 *key)
{
    N128 gcd, one128, privatekey, fai, mod, mid;                                    /*middle results*/

    wzero(&privatekey);                                                             /*init vars*/
    wzero(&one128);
    one128.val[0] = 1;

    copyw(&mod, &(key->p));
    vmultw(&mod, &(key->q));                                                        /* mod=p*q */

    copyw(&fai, &(key->p));
    decw(&fai);
    copyw(&mid, &(key->q));
    decw(&mid);
    vmultw(&fai, &mid);                                                             /* fai=(p-1)*(q-1) */

    if(-1 == linearEquitionw(&(key->publicKey), &fai, &gcd, &privatekey, NULL))     /*resolve linearequition*/
        return -1;
    if(cmpw(&gcd, &one128) != 0)                                                    /*check whether gcd(publicKey, fai) equals 1*/
        return -1;

    copyw(&(key->privateKey), &privatekey);                                         /*resolution x(key) is the privateKey we need*/
    copyw(&(key->mode), &mod);
    copyw(&(key->fai), &fai);
    return 1;
}
int genPrivateKey2048(struct rsakey2048 *key)
{
    N256 gcd, one256, privatekey, fai, mod, mid;                                    /*middle results*/

    dzero(&privatekey);                                                             /*init vars*/
    dzero(&one256);
    one256.val[0] = 1;

    copyd(&mod, &(key->p));
    vmultd(&mod, &(key->q));                                                        /* mod=p*q */

    copyd(&fai, &(key->p));
    decd(&fai);
    copyd(&mid, &(key->q));
    decd(&mid);
    vmultd(&fai, &mid);                                                             /* fai=(p-1)*(q-1) */

    if(-1 == linearEquitiond(&(key->publicKey), &fai, &gcd, &privatekey, NULL))     /*resolve linearequition*/
        return -1;
    if(cmpd(&gcd, &one256) != 0)                                                    /*check whether gcd(publicKey, fai) equals 1*/
        return -1;

    copyd(&(key->privateKey), &privatekey);                                         /*resolution x(key) is the privateKey we need*/
    copyd(&(key->mode), &mod);
    copyd(&(key->fai), &fai);
    return 1;
}

/**
 * Crypt with key->privateKey
 * input: plain, key
 * output: cypher
 * return: 1 for success, -1 for fail
 */
int cryptw(const N128 *plain, struct rsakey1024 *key, N128 *cypher)
{
    if((plain == NULL) || (key == NULL) || (cypher == NULL))
        return -1;
    return powerModew(plain, &(key->mode), &(key->privateKey), cypher);
}
int cryptd(const N256 *plain, struct rsakey2048 *key, N256 *cypher)
{
    if((plain == NULL) || (key == NULL) || (cypher == NULL))
        return -1;
    return powerModed(plain, &(key->mode), &(key->privateKey), cypher);
}

/**
 * Descrypt with key->publicKey
 * input: cypher, key
 * output: plain
 * return 1 for success, -1 for fail
 */
int descryptw(const N128 *cypher, struct rsakey1024 *key, N128 *plain)
{
    if((plain == NULL) || (key == NULL) || (cypher == NULL))
        return -1;
    return powerModew(cypher, &(key->mode), &(key->publicKey), plain);
}
int descryptd(const N256 *cypher, struct rsakey2048 *key, N256 *plain)
{
    if((plain == NULL) || (key == NULL) || (cypher == NULL))
        return -1;
    return powerModed(cypher, &(key->mode), &(key->publicKey), plain);
}