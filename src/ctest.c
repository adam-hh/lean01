#include "commonHeader.h"
#define A 122530
#define M 12473
#define C 42
#define PERFECTTEST 8128

int main()
{
    int64_t x, y, a, m, c, g;
    a = PERFECTTEST;
    m = M;
    c = C;
    g = gcd(a, m);
    printf("gcd(%" PRId64 ", %" PRId64 ") is %" PRId64 " \n", a, m, g);
    linearEquitionM(a, m, c,&x, &y);
    if(0 == c % g)
        printf("a group of solution of %" PRId64 "x + %" PRId64 "y = %" PRId64 " is (x, y) = (%" 
            PRId64 ", %" PRId64 ")\n", a, m, c, x, y);
    else
        printf("no solution of the equition because 0 != %" PRId64 " mod %" PRId64 "\n",c ,g);
    createpTable();
    for(u_int64_t i = 0; i < PTABLESIZE; i++)
    {
        if(0 == pTable[i]){
            if(PTABLESIZE - 1 == i)
                printf("available numbers in %" PRId64" is more than %" PRId64 "\n", PRIMEUPL, i);
            else
                printf("available numbers in %" PRId64" is %" PRId64 "\n", PRIMEUPL, i);
            break;
        }
        /* printf("%-7" PRId64 " ", pTable[i]);
        if(0 == (i + 1) % 10)
            printf("\n"); */
    }

    u_int64_t *p, *q;
    p = (q = NULL);
    switch(primeFact(a, &p))
    {
    case 1:
        q = p;
        if(NULL != p)
        {
            printf("%" PRId64 "'s fact list:", a);
            while(0 != *p)
            {
                printf("%6" PRId64 ",", *(p++));
            }
            printf("\n");
        }
        break;
    case 0:
        printf("%" PRId64 " is a prime number.\n", a);
        break;
    default:
        printf("unknow error occured.\n");
        break;
    }
    free(q);

    printf("Euler fai(%"PRId64 ") = %" PRId64 "\n", PERFECTTEST, funcFai(PERFECTTEST));
    printf("Euler delta(%"PRId64 ") = %" PRId64 "\n", PERFECTTEST, funcDelta(PERFECTTEST));
    printf("Euler s(%"PRId64 ") = %" PRId64 "\n", PERFECTTEST, funcS(PERFECTTEST));
    //printf("china remainder(%ld, %ld, %ld, %ld) is x = %ld\n", 12, a, 54, m, chinaRemainder(12, a, 54, m));
    extern u_int64_t funcSRec(u_int64_t x, u_int64_t n);
    funcSRec(14316,28);
    putchar('\n');

    return 0;
}