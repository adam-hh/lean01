#include "commonHeader.h"

typedef struct {
    u_int64_t a1;
    u_int64_t a2;
} u128, *ptru128;
u_int64_t multU64TU128(ptru128 a, ptru128 b)//a * b -> a
{
    u_int64_t lval = (a->a2 < b->a2) ? a->a2 : b->a2;
    u_int64_t gval = (a->a2 != lval) ? a->a2 : b->a2;
    u_int64_t hval = 0;
    u_int64_t rval = 0;
    u_int64_t tmp = 0;
    u_int8_t cnt = 0;
    while (lval >= 1)
    {
        if(1 == (lval & 0x1)) {
            if(cnt)//avoid right move 64 bit
                hval += ((gval >> (64 - cnt)));
            tmp = (gval << cnt);
            rval += tmp;
            if(rval < tmp)//addtional over flow
                hval += 1;
        }
        lval >>= 1;
        cnt++;
    }
    a->a1 = hval;
    a->a2 = rval;
    return a->a2;
}
u_int64_t adddU64TU128(ptru128 a, ptru128 b)
{
    a->a2 += b->a2;
    if(a->a2 < b->a2)
        a->a1 += b->a1 + 1;
    else
        a->a1 += b->a1;
    return a->a2;
}
/*
(a*b) % m
*/
u_int64_t modU64(u_int64_t a, u_int64_t b, u_int64_t m)
{
    u128 a128 = {0}, b128 = {0}, rval = {0};
    a128.a2 = a;
    b128.a2 = b;
    b128.a2 = multU64TU128(&a128, &b128);
    if(!a128.a1)//multipcation no overflow
        return a128.a2 % m;
    a128.a2 = a128.a1;
    a128.a1 = 0;
    do
    {
        rval.a1 = 0;
        rval.a2 = (1 % m) + ((-1) % m);
        a128.a2 %= m;
        b128.a2 %= m;
        multU64TU128(&rval, &a128);
        adddU64TU128(&rval, &b128);
        if(rval.a1)
        {
            a128.a2 = rval.a1;
            b128.a2 = rval.a2;
        }
    } while (rval.a1);
    return rval.a2 % m;
}
/*
math formulas: a^k mod m, remainder r=a^k(mod m), 0<r<m.
binary expansion of k, set 1->r as the initial value, loop from the lowest bit increasing to the highest bit.
when get a 1 bit set a(mod m) * r -> r, then set a^2(mod m)->a without condition.
C logic: input a, m, k. output b(==remainder)
*/
u_int64_t powerMode(u_int64_t a, u_int64_t m, u_int64_t k)
{
    u_int64_t r = 1;//set a as initial value of r.
    if(0 == (a % m))
        return 0;//when divisible, remainder is 0
    while(k >= 1)//math:binary expansion k,from the lowest bit to higest bit.
    {
        if(1 == (k & 0x1))//math:get a 1 bit
            //r = (a * r % m);//math:set a*b(mod m)->b
            r = modU64(a, r, m);
        //a = (a * a % m);//math:set a^2(mod m)->a without condition
        a = modU64(a, a, m);
        k >>= 1;//math:move to a higer bit then loop
    }
    return r;
}
/*
math formulas: x^k=b(mod m), gcd(k, fai(m))=1, u is the solution of equition ku-fai(m)v=1, then x=b^u(mod m).
C logics: input k, b, m. output x.
*/
u_int64_t resolvePowerMode(u_int64_t k, u_int64_t b, u_int64_t m)
{
    u_int64_t u, fm = funcFai(m);
    if((0 == k) || (0 == fm))
        return -1;
    if(1 != gcd(k, fm))
        return -1;
    u = powerMode(k, fm, funcFai(fm) - 1);
    return powerMode(b, m, u);
}