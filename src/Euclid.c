#include "commonHeader.h"
/*
math equition: a = r mod(b), 0 <= r < b; q = a/b, discards remainder
*/
u_int64_t mode(u_int64_t a, u_int64_t b, u_int64_t *r)
{
    if((0 == a) | (0 == b)) {
        *r = 0;
        return -1;
    }
    *r = a % b;
    return a / b;
}
/*
math equitions: x = gcd(a, b), a = r mod(b).
C logics: def r, if b != 0, loop (a%b)->r, b->a, r->b. if b == 0 then a is the final result of gcd.
*/
u_int64_t gcd(u_int64_t a, u_int64_t b){
    u_int64_t r; //remainder
    while (0 != b)
    {
        mode(a, b, &r);
        a = b;//math b->a
        b = r;//math r->b
    }   
    return a;
}
/*
mathematics equition: 
ax + my = g = gcd(a, m); r[n] = x[n]a - q[n]m; x[n] = x[n-2] - q[n]*x[n-1]; given x[1] = 1, x[0] = 0, x[-1] = 1.
C logics: 
def g,w,q,s,v,r, 1(==x[-1])->*x, 0(==x[0])->v, if m != 0, loop(on math logic value of n increasing from 1 with loop):
(a%m)->r, m->a, r->m, (x[n-2] - q[n]*x[n-1]) == x[n]->s, v(==x[n-1])->*x, s(==x[n])->v,
if m == 0 then 
a is the final result of gcd.
*x(==x[n-1]) is the final result of x
*/
u_int64_t linearEquition(u_int64_t a, u_int64_t m, int64_t *x, int64_t *y)
{
    int64_t s, v;
    u_int64_t g, w, r, q;
    *x = 1;//math *x = x[-1] = x[n-2]
    v = 0;//math v = x[0] = x[n-1]
    g = a;//store a to avoid changed in the loop
    w = m;//store m to avoid changed in the loop
    while(0 != w)
    {
       q = mode(g, w, &r);
       s = (*x) - (q * v);//math x[n]->s, mark:initial n = 1 at the first loop, n = 2 at the second loop...
       *x = v;//math x[n-1]->*x
       v = s;//math x[n]->v
       g = w;//math w->g, mark: value of g changes here
       w = r;//math r->w, mark: value of w changes here
    }
    while( *x <= 0 )//get a positive value of x
    {
        *x = *x + (m / g);//math: equition solution x = x[1] + k * (m / g), k belongs to Z
    }
    *y = (g - a * (*x)) / m;//get solution of y, need to use the original value of a and m
    return g;
}
/*
math equition: ax + my =c, g = gcd(a, m). if c % g = 0, then equition resolvable, else equition unresolvable.
number of resolves is g, one group of resolve will be stored in (x, y).
*/
u_int64_t linearEquitionM(u_int64_t a, u_int64_t m, u_int64_t c, int64_t *x, int64_t *y)
{
    u_int64_t g = linearEquition(a, m, x, y);//math: resolve equition ax + my = gcd(a, m)
    if(0 == c % g)//math: ax + my = c resolvable
    {
        *x = (*x) * (c / g) % m;
        //*y = (*y) * (c / g) % m;
        *y = (c - a * (*x)) / m;
    }
    else//math: ax + my = c unresolvable
    {
        printf("Eqution has no solution,because % "PRId64" %% % "PRId64" != 0\n", c, g);
        return 0;
    }
    return g;
}

/*
math: linear mod eqution ax = c mod m, g = gcd(a, m), iff c = 0 mod g, equition resolvable.
*/
u_int64_t linearModEqution(u_int64_t a, u_int64_t m, u_int64_t c, u_int64_t **x)
{

}