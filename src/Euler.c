#include "commonHeader.h"

/*
math:Euler fai function, fai(m) = fai(p)*fai(q),p and q are prime. fai(p^k) = p^k - p^(k-1), p is prime.
C logic: x is the number to be decomposited, fai(x) to be returned.
*/
u_int64_t funcFai(u_int64_t x)
{
    u_int64_t *q, *p;//store fact list
    u_int64_t tmp[2][PFACTLISTSIZE] = {0};//if arrary size set to 500000u, segment fault when runing
    u_int64_t result = 1;//def result
    int tag = primeFact(x, &p);//def tag is decomposite result flag
    q = p;//store the original value of p to avoid change
    u_int64_t i,j;
    if(1 == tag)//x decomposited and x is not a prime
    {
        i = 0;//arrary index counter
        j = 1;//power counter
        while(0 != *p)
        {
            if(0 == tmp[0][i])
                tmp[0][i] = *p;//fact
            tmp[1][i] = j;//power
            if(*(++p) == tmp[0][i])//comp *(p+1) and *p
                tmp[1][i] = ++j;//power + 1
            else{
                i++;//arrary index + 1
                j = 1;//power reset to 1
            }
        }
        free(q);
        i = 0;//reset i
        while(0 != tmp[0][i])
        {
            if(1 == tmp[1][i])//power == 1
                result *= (tmp[0][i] - 1);//math:fai(p) = p-1 when p is a prime
            else//power > 1
            {
                result *= (tmp[0][i] - 1);//math:fai(p^k) = p^k - p^(k-1) p is prime
                while(1 != tmp[1][i]--)
                    result *= tmp[0][i];
            }
            i++;
        }
        return result;
    }
    else if(0 == tag)//x is a prime
        return x - 1;
    return 0;//unknow error 
}
/*
math:china remainder theorem, x = b mod m, x = c mod n, gcd(m,n) = 1, then solution of x is iff.
*/
int64_t chinaRemainder(int64_t b, int64_t m, int64_t c, int64_t n)
{
    int64_t x, y;
    if(1 != linearEquitionM(m, n, c-b, &x, &y))
        return 0;
    return (b + m * x) % (m * n);
}
/*
mmath:Euler delta function, delta(m) = delta(p)*delta(q),p and q are prime. delta(p^k) = (p^(k+1) -1)/(p-1), p is prime.
C logic: x is the number to be decomposited, delta(x) to be returned.
*/
u_int64_t funcDelta(u_int64_t x)
{
    u_int64_t *p, *q;//store fact list
    u_int64_t tmp[2][PFACTLISTSIZE] = {0};
    u_int64_t i, j, result = 1;//def result
    int tag = primeFact(x, &p);//def tag is decomposite result flag
    q = p;//store the original value of p to avoid change
    if(1 == tag)//x decomposited and x is not a prime
    {
        i = 0;//arrary index counter
        j = 1;//power counter
        while(0 != *p)
        {
            tmp[0][i] = *(p++);//fact
            tmp[1][i] = j;//power
            if(*p == tmp[0][i]){//comp *(p+1) and *p
                tmp[1][i] = ++j;//power + 1
            }
            else{
                i++;//arrary index + 1
                j = 1;//power reset to 1
            }
        }
        free(q);
        i = 0;//reset i
        while(0 != tmp[0][i])
        {
            if(1 == tmp[1][i])//power == 1
                result *= (tmp[0][i] + 1);//math: delta(p) = p + 1 when p is a prime
            else//power > 1
                result *= ((powu64(tmp[0][i], tmp[1][i] + 1) - 1) / (tmp[0][i] - 1));//math:delta(p^k) = (p^(k+1)-1)/(p-1) when p is prime
            i++;
        }
        return result;
    }
    else if(0 == tag)//x is a prime
    {
        return x + 1;
    } 
    return 0;
}
/*
math:Euler delta function deformation to test perfect number,delta(m)-m is the summation of m's real facts.
*/
u_int64_t funcS(u_int64_t x)
{
    return funcDelta(x) - x;//real facts summation
}
/*
math:test continus series
*/
u_int64_t funcSRec(u_int64_t x, u_int64_t n)
{
    if(0 != n--)
    {
        printf("s(%" PRIu64 ") = %" PRIu64 ", ", x, funcS(x));
        funcSRec(funcS(x), n);//recursion
    }
    else
        return x;
}