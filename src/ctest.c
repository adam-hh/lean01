#include "commonHeader.h"
#define A 122530
#define M 12473
#define C 42
#define PERFECTTEST -1

int hello()
{
    int64_t x, y, a, m, c, g;
    a = PERFECTTEST;
    m = M;
    c = C;
    g = gcd(a, m);
    printf("gcd(%llu, %llu) is %llu \n", a, m, g);
    linearEquitionM(a, m, c,&x, &y);
    if(0 == c % g)
        printf("a group of solution of %llux + %lluy = %llu is (x, y) = (%llu, %llu)\n", a, m, c, x, y);
    else
        printf("no solution of the equition because 0 != %llu mod %llu\n",c ,g);
    createpTable();
    for(u_int64_t i = 0; i < PTABLESIZE; i++)
    {
        if(0 == pTable[i]){
            if(PTABLESIZE - 1 == i)
                printf("available numbers in %llu is more than %llu, last prime is %llu\n", PRIMEUPL, i, pTable[i - 1]);
            else
                printf("available numbers in %llu is %llu, last prime is %llu\n", PRIMEUPL, i, pTable[i - 1]);
            break;
        }
        /* printf("%-7" PRId64 " ", pTable[i]);
        if(0 == (i + 1) % 10)
            printf("\n"); */
    }

    u_int64_t *p, *q;
    p = (q = NULL);
    //a = 9208981628670443520;
    switch(primeFact(a, &p))
    {
    case 1:
        q = p;
        if(NULL != p)
        {
            printf("%llu's fact list:", a);
            while(0 != *p)
            {
                printf("%6llu,", *(p++));
            }
            putchar('\n');
        }
        break;
    case 0:
        printf("%llu is a prime number.\n", a);
        break;
    default:
        printf("unknow error occured.\n");
        break;
    }
    free(q);

    printf("Euler fai(%llu) = %llu\n", a, funcFai(a));
    printf("Euler delta(%" PRIu64 ") = %llu\n", PERFECTTEST, funcDelta(PERFECTTEST));
    printf("Euler s(%llu) = %llu\n", PERFECTTEST, funcS(PERFECTTEST));
    //printf("china remainder(%ld, %ld, %ld, %ld) is x = %ld\n", 12, a, 54, m, chinaRemainder(12, a, 54, m));
    //extern u_int64_t funcSRec(u_int64_t x, u_int64_t n);
    //funcSRec(14316,28);
    //putchar('\n');
#define ORIGTEXT 8234212425253
#define ENCKEY 677
#define MOD -1
    u_int64_t originText = ORIGTEXT, enckey = ENCKEY, mod = MOD;
    u_int64_t cipherText = powerMode(originText, mod, enckey);
    u_int64_t decText = resolvePowerMode(enckey, cipherText, mod);
    printf("original number is %x, k value is %llu, mod value is %x, cihper number is %x\n", originText, enckey, mod, cipherText);
    printf("decoded number is %x\n", decText);
    return 0;
}