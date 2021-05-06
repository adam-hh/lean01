#include "commonHeader.h"
#include <math.h>
#include <string.h>
u_int64_t pTable[PTABLESIZE] = {0};
/*
prime number decomposition, x is the number to be decomposite, *p is the arrary to store facts
*/
int primeFact(u_int64_t x, u_int64_t **p)
{
    u_int64_t i,j;//counters
    u_int64_t tmp[PFACTLISTSIZE] = {0};//store facts to a temp arrary
    for(i = (j = 0); pTable[i] <= sqrt(x); i++)
    {
        if(0 == (x % pTable[i]))
        {
            x /= pTable[i];
            tmp[j++] = pTable[i];
            i--;
        }
    }
    tmp[j] = x;
    if(0 == j)
        return 0;//x is a prime number
    if(NULL == (*p = (u_int64_t *)malloc((j + 2) * sizeof(u_int64_t)))){//arrary size set to "j + 2"
        printf("malloc failed.\n");
        return -1;//unknow error
    }
    memset(*p, 0, (j + 2) * sizeof(u_int64_t));//arrary size set to "j+1" to leave a zero at the end of arrary to identify the end
    if(j > PFACTLISTSIZE - 2)
        return -1;//outof band
    do{
        *(*p + j) = tmp[j];//
    }while(j-- != 0);
    return 1;
}
/*
test numbers between 4 and PRIMEUPL, fill the prime number to PTable[],and leave pTable[PTABLESIZE - 1] == 0;
*/
void createpTable()
{
    u_int64_t i, j, p;
    int tag = 0;//identify prime or non-prime
    pTable[0] = 2;
    pTable[1] = 3;
    p = 2;//counter of pTable, start from 2
    for(i = 5; i <= PRIMEUPL; i++)//i is a number to be test
    {
        if(!(i & 0x01))
            continue;//skip even number
        tag = 0;
        for(j = 1; (pTable[j] != 0) && (pTable[j] <= sqrt(i)); j++)
        {
            if(0 == (i % pTable[j])){
                tag = 1;//i is not a prime number
                break;
            }
        }
        if(0 == tag)
        {
            pTable[p++] = i;//i is a prime number
        }
        if((PTABLESIZE - 1) == p)//leave at least one zero at the end of pTable
            break;
    }

}