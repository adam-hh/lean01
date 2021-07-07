#include "littlePrime.h"
#include <signal.h>
/**
 * a mod b
 * input: a, b
 * output: r
 * return: void
 */
static u64 mode(u64 a, u64 b, u64 *r)
{
    if (b == 0) {
        raise(SIGFPE);
        *r = a;
        return 0xffffffffffffffff;
    }
    *r = a % b;
    return a / b;
}

/**
 * gcd(a,b)
 * input: a, b
 * output: none
 * return: value of gcd(a,b)
 */
u64 gcd(u64 a, u64 b)
{
    if ((a == 0) || (b == 0))
        return 0;
    u64 r;
    while (0 != b)
    {
        mode(a, b, &r);
        a = b;
        b = r;
    }   
    return a;
}
/**
 * Resolve linear equition ax+my=g
 * ax + my = g = gcd(a, m); r[n] = x[n]a - q[n]m; x[n] = x[n-2] - q[n]*x[n-1]; given x[1] = 1, x[0] = 0, x[-1] = 1.
 * input: a, m
 * output: x, y
 * return: g
 */
u64 linearEquition(u64 a, u64 m, u64 *x, u64 *y)
{
    N16 aval, mval;
    N16 s, v, xval, yval, qval;
    u64 *pa, *pm;
    u64 *ps, *pv, *px, *py, *pq;
    u64 g, w, r, q;

    if ((a == 0) || (m == 0) || (x == NULL)) {
        return 0;
    }

    ps = (u64 *)(s.val);
    pv = (u64*)(v.val);
    px = (u64 *)(xval.val);
    py = (u64*)(yval.val);
    pq = (u64*)(qval.val);
    pa = (u64 *)(aval.val);
    pm = (u64*)(mval.val);
    px[0] = 1;                              /*init x=1*/
    px[1] = 0;
    pv[0] = 0;                              /*init v=0*/
    pv[1] = 0;
    pa[0] = a;                              /*copy a*/
    pa[1] = 0;
    pm[0] = m;                              /*copy m*/
    pm[1] = 0;

    g = a;                                  /*store a to avoid be changed in the loop*/
    w = m;                                  /*store m to avoid be changed in the loop*/
    while(0 != w)
    {
       q = mode(g, w, &r);

       pq[0] = q;                           /*cast q*/
       pq[1] = 0;
       multN16(&qval, &v);
       invN16(&qval);
       addN16(&xval, &qval);
       ps[0] = px[0];                       /*s = x-(q*v)*/
       ps[1] = px[1];

       px[0] = pv[0];                       /*x = v*/
       px[1] = pv[1];

       pv[0] = ps[0];                       /*v = s*/
       pv[1] = ps[1];

       g = w;                               /*math w->g, mark: value of g changes here*/
       w = r;                               /*math r->w, mark: value of w changes here*/
    }
    while(px[1] & 0x8000000000000000)       /*get a positive value of x*/
    {
        addN16(&xval, &mval);               /*math: equition solution x = x[1] + k * (m / g), k belongs to Z*/
    }
    *x = px[0];
    if (y)
        *y = (g - a * (*x)) / m;            /*get solution of y, need to use the original value of a and m*/
    return g;
}

/**
 * Resolve linear equition: ax + my =c, g = gcd(a, m). if c % g = 0, then equition resolvable, else equition unresolvable.
 * number of resolves is g, one group of resolve will be stored in (x, y).
 * input: a, m, c
 * output: x, y
 * return: gcd(a,b)
 */
u64 linearEquitionM(u64 a, u64 m, u64 c, u64 *x, u64 *y)
{
    u64 g = linearEquition(a, m, x, y);     /*math: resolve equition ax + my = gcd(a, m)*/

    if (0 == c % g) {                       /*math: ax + my = c resolvable*/
        *x = (*x) * (c / g) % m;
        *y = (c - a * (*x)) / m;
    } else {                                /*math: ax + my = c unresolvable*/
        *x = 0;
        *y = 0;
    }
    return g;
}