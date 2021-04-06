#include <stdio.h>
#include <inttypes.h>
/*
mode caculation:
a mode b == *r mode b, 0 =< *r < b
return q == a / b
*/
int mode(int64_t a, int64_t b, int64_t *r)
{
    *r = a % b;
    return a / b;
}
/*
ax + by = gcd(a, b)
*/
int64_t linearEquition(int64_t a, int64_t b, int64_t *x, int64_t *y)
{
    int64_t g, v, w, s, q, t;
    *x = 1;
    g = a;
    v = 0;
    w = b;
    while(w != 0)
    {
       q = mode(g, w, &t);
       s = (*x) - (q * v);
       *x = v;
       g = w;
       v = s;
       w = t;
    }
    while( *x < 0 )
    {
        *x = *x + (b / g);
    }
    *y = (g - a * (*x)) / b;
    return g;
}