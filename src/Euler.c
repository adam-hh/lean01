#include "littlePrime.h"
#include <stdlib.h>
/**
 * Euler fai function, fai(m) = fai(p)*fai(q),p and q are prime. fai(p^k) = p^k - p^(k-1), p is prime.
 * input: x, ptable
 * output: none
 * return: positive result for success, -1 if x factorization failed
 */
u64 funcFai(u64 x, PTable *ptable)
{
    PFtable pf;
    u64 result;

    pf = primeFact(x, ptable);
    result = 1;

    if (pf.size == 0)
        return 0xffffffffffffffff;

    for(size_t i = 0; i < pf.size; i++) {
        if (pf.pftable[i].exp == 1)
            result *= (pf.pftable[i].fact - 1);
        else {
            result *= expu64(pf.pftable[i].fact, pf.pftable[i].exp - 1);
            result *= (pf.pftable[i].fact - 1);
        }
    }
    
    free(pf.pftable);
    return result;
}

/**
 * euler delta function, delta(m) = delta(p)*delta(q),p and q are prime. delta(p^k) = (p^(k+1) -1)/(p-1), p is prime.
 * delta(p) = P+1
 * input: x, ptable
 * output: none
 * return: positive result for success, -1 if x factorization failed
 */
u64 funcDelta(u64 x, PTable *ptable)
{
    PFtable pf;
    u64 result, mid, k;

    pf = primeFact(x, ptable);
    result = 1;
    mid = 0;

    if (pf.size == 0)
        return -1;
    
    for(size_t i = 0; i < pf.size; i++) {
        if (pf.pftable[i].exp == 1)
            result *= (pf.pftable[i].fact + 1);
        else {
            k = pf.pftable[i].exp;
            mid = 0;
            while(k) {
                mid += expu64(pf.pftable[i].fact, k);
                k--;
            }
            mid++;
            result *= mid;
        }
    }
    
    free(pf.pftable);
    return result;
}
/**
 * Real facts summation
 * math:Euler delta function deformation to test perfect number,delta(m)-m is the summation of m's real facts.
 */
u64 funcS(u64 x, PTable *ptable)
{
    return funcDelta(x, ptable) - x;
}