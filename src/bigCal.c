#include "commonHeader.h"
#include <string.h>
/*
a[BLOCKSISE * 2] + b[BLOCKSIXE * 2] -> a
*/
static inline int addBigNum(u8 *a, u8 *b)
{
    register u32 *pa = (u32*)a, *pb = (u32*)b, cnt = 0, addOverflow = 0;
    pa += (((BLOCKSIZE * 2) / 4) - 1);
    pb += (((BLOCKSIZE * 2) / 4) - 1);
    do
    {
        *pa += (*pb + addOverflow);
        if(((*pa - addOverflow) < *pb) || (*pa < addOverflow))//pa+pb overflow or pa+pb+addOverflow overflow
            addOverflow = 1;
        else
            addOverflow = 0;
        pa--;
        pb--;
    } while (cnt++ != ((BLOCKSIZE * 2) / 4) - 1);//when BLOCKSIZE is 128, this 33 loops
    return *((u32*)a);
}
static inline int subBigNum(u8 *a, u8 *b)
{
    u8 tmp[BLOCKSIZE * 2] = {0}, one[BLOCKSIZE * 2] = {0};
    *((u32*)one + ((BLOCKSIZE * 2) / 4) - 1) = 1;//set one to 1, aligin to u32, avoid little/big endian problem
    for(int i = BLOCKSIZE; i < BLOCKSIZE * 2; i++)
        tmp[i] = ~(*(b + i));//tmp = ~b
    addBigNum(tmp, one);//tmp = -b
    tmp[BLOCKSIZE -1] = 0;//resolve 0 sign problem
    return addBigNum(a, tmp);//a=a-b
}
static inline int modBigNumam(u8 *a, u8 *m, u8 *r)//a % m, a and m are BLOCKSIZE wide
{
    u32 *rptr = (u32*)(r + BLOCKSIZE), *mptr = (u32*)(m + BLOCKSIZE);
    if(a != r)
        memcpy(r, a, BLOCKSIZE * 2);
    while(*(rptr++) == *(mptr++))//skip equal part, may outof band
    {
        if((rptr - (u32*)(r)) == (BLOCKSIZE * 2) / 4)
            break;
    }
    if(*(--rptr) >= *(--mptr))
    {
        do
        {
            subBigNum(r, m);
            *((u32*)r + (BLOCKSIZE / 4) - 1) = 0;//set addtional over flow bits to 0, aligin to u32
            rptr = (u32*)(r + BLOCKSIZE);
            mptr = (u32*)(m + BLOCKSIZE);
            while(*(rptr++) == *(mptr++))
            {
                if((rptr - (u32*)(r)) == (BLOCKSIZE * 2) / 4)
                    break;
            }
        } while (*(--rptr) >= *(--mptr));     
    }
    return 1;
}
/*
a[BLOCKSIZE * 2] * b[BLOCKSIZE * 2] -> a
*/
int multBigNum(u8 *a, u8 *b)
{
    register u32 *pa = (u32*)a, *pb = (u32*)b, multOverflow = 0, cnt = 0, cnt2 = 0, addOverflow = 0;
    register u64 a1, b1;//a1,b1 to store the u32 data, so the multiplication can not overflow. 
    u8 val[BLOCKSIZE * 2] = {0}, tmp[BLOCKSIZE * 2] = {0};//val[] to store the final result, tmp[] to store the middle result
    register u32 *ptrVal = (u32*)val, *ptrTmp = (u32*)tmp;
    pa += (((BLOCKSIZE * 2) / 4) - 1);//when BLOCKSIZE is 128, this is 63. make pa to point to the last u32 data.
    pb += (((BLOCKSIZE * 2) / 4) - 1);//make pb to point to the last u32 data
    ptrVal += (((BLOCKSIZE * 2) / 4) - 1);
    ptrTmp += (((BLOCKSIZE * 2) / 4) - 1);
    do
    {
        do
        {
            a1 = *(pa--);//pa move forward
            b1 = *pb;
            a1 *= b1;//no overflow in math
            a1 += multOverflow;//no overflow in math   
            multOverflow = (a1 >> 32);//get the carry data from the high 32 bits
            *(ptrTmp--) = ((a1 << 32) >> 32);//get a middle result data from the low 32 bits
        } while (cnt++ != ((BLOCKSIZE / 4) - 1));//when BLOCKSISE is 128, this 32 loops
        *ptrTmp = (a1 >> 32);//fill the last overflow u32 data from the high 32 bits of a1
        multOverflow = 0;//reset multOverflow
        pa += cnt;//reset pa
        ptrTmp += cnt;//reset ptrTmp
        do
        {
            *ptrVal += (*ptrTmp + addOverflow);//get a final result data, addional overflow must be deal with
            if((*ptrVal - addOverflow < *ptrTmp) || (*ptrVal < addOverflow))
                addOverflow = 1;
            else
                addOverflow = 0;
            ptrVal--;
            ptrTmp--;
        } while (cnt-- != 0);//1 more loops, when BLOCKSISE is 128, this 33 loops
        cnt = 0;//reset cnt
        ptrVal += ((BLOCKSIZE / 4) + 1);//reset ptrVal
        ptrTmp += ((BLOCKSIZE / 4) + 1);//reset ptrTmp
        pb--;//pb move forward
        ptrVal--;
        ptrTmp--;
    } while(cnt2++ != ((BLOCKSIZE / 4) - 1));//when BLOCKSISE is 128, this 32 loops
    for(int i = 0; i < (BLOCKSIZE * 2); i++)
        *(a + i) = val[i];//fill back final result to a[]
    return *(int*)a;//a int data returned.the sign bit is in it.
}
/*
(a*b)%m, put the remainder to r
*/
int modBigNum(u8 *a, u8 *b, u8 *m, u8 *r)
{
    u8 product[BLOCKSIZE *2] = {0}, *p, rprod[BLOCKSIZE * 2] = {0}, one[BLOCKSIZE *2] = {0}, minusOne[BLOCKSIZE *2] = {0}, tmp[BLOCKSIZE * 2] = {0};
    *((u32*)one + ((BLOCKSIZE * 2) / 4) - 1) = 1;//1
    p = minusOne;
    for(p += BLOCKSIZE; (p - minusOne) < (BLOCKSIZE *2); p++)
        *p = 0xFF;//-1
    p = product;
    memcpy(product, a, BLOCKSIZE * 2);
    multBigNum(product, b);//a*b -> product
    do
    {
        while(0 == *(p++))//skip leading zeros
            ;
        if((p - product - 1) >= BLOCKSIZE)
        {
            modBigNumam(product, m, r);
            break;//loop break condition
        }
        else
        {
            memcpy(rprod + BLOCKSIZE, product + BLOCKSIZE, BLOCKSIZE);//copy right part of product to rprod
            modBigNumam(rprod, m, rprod);
            memcpy(product + BLOCKSIZE, product, BLOCKSIZE);//copy left part of product to it's right part
            memset(product, 0, BLOCKSIZE);
            modBigNumam(minusOne, m, tmp);
            addBigNum(tmp, one);
            multBigNum(product, tmp);
            addBigNum(product, rprod);//to be do
            p = product;
        }
    } while (1);
}
/*
decimal display, src is a arrary src[BLOCKSIZE * 2].
*/
int decDisp(u8 *src, u8 **des)
{
    u8 divided[BLOCKSIZE * 2] = {0}, q[BLOCKSIZE * 2] = {0}, *desptr;
    register u32 *qptr = (u32*)q, *dividedptr = (u32*)divided, *tmp, cnt, cnt2;
    register u64 rem;//remainder middle value
    (*des) = (u8*)malloc(BLOCKSIZE * 6 + 1);//destination to output
    if(NULL == *des)
        return -1;
    memset(*des, '0', BLOCKSIZE * 6);
    *(*des + (BLOCKSIZE * 6)) = 0;
    desptr = *des + BLOCKSIZE * 6 -1;

    for(int i = 0; i < BLOCKSIZE * 2; i++)
        divided[i] = *(src + i);   
    cnt = 0;
    do
    {
        if(!(rem = *dividedptr)) {//skip leading zeros
            qptr++;
            dividedptr++;
            if(cnt++ == ((BLOCKSIZE * 2) / 4) - 1)//outter loop break condition
                break;
            continue;
        }
        cnt2 = cnt;
        cnt = 0;//reset cnt
        do
        {
            *(qptr++) = rem / 10;
            rem = rem % 10;
            if(cnt2++ == ((BLOCKSIZE * 2) / 4) - 1)//inner loop break condition
                break;
            rem <<= 32;
            rem += *(++dividedptr);
        } while (1);
        *(desptr--) = rem + 0x30;//get a decimal digit
        dividedptr -= (((BLOCKSIZE * 2) / 4) - 1);//reset divideptr
        qptr -= ((BLOCKSIZE * 2) / 4);//reset qptr
        tmp = dividedptr;//exchange dividedptr and qptr value
        dividedptr = qptr;
        qptr = tmp;
        memset(qptr, 0, BLOCKSIZE * 2);//reset qptr arrary
    } while (1);

    return desptr - *des + 1;//return number of leading zeros
}