#include "commonHeader.h"
#include <string.h>
#include <ctype.h>
int multBigNum(u8 *a, u8 *b);
int modBigNum(u8 *a, u8 *b, u8 *m, u8 *r);
/*
a[BLOCKSISE * 2] + b[BLOCKSIXE * 2] -> a
*/
static inline int addBigNum(u8 *a, u8 *b)
{
    register u32 *pa = (u32*)a, *pb = (u32*)b, cnt = 0, addOverflow = 0;
    pa += (((BLOCKSIZE * 2) / 4) - 1);
    pb += (((BLOCKSIZE * 2) / 4) - 1);
    while (cnt++ != ((BLOCKSIZE * 2) / 4) - 1)//when BLOCKSIZE is 128, this 64 loops
    {
        if((!*pa) && (!*pb) && (!addOverflow)){
            pa--;
            pb--;
            continue;
        } 
        *pa += (*pb + addOverflow);
        if(((*pa - addOverflow) < *pb) || (*pa < addOverflow))//pa+pb overflow or pa+pb+addOverflow overflow
            addOverflow = 1;
        else
            addOverflow = 0;
        pa--;
        pb--;
    }
    return *((u32*)a);
}
static inline int addReverse(u8 *a)
{
    u8 one[BLOCKSIZE * 2] ={0};
    *((u32*)one + ((BLOCKSIZE * 2) / 4) - 1) = 1;
    for(int i = BLOCKSIZE; i < BLOCKSIZE * 2; i++)
        *(a + i) = ~(*(a + i));//tmp = ~b
    addBigNum(a, one);//tmp = -b
    *((u32*)a + (BLOCKSIZE / 4) -1) = 0;//resolve -0 sign problem
}
static inline int subBigNum(u8 *a, u8 *b)
{
    u8 tmp[BLOCKSIZE * 2] = {0}, one[BLOCKSIZE * 2] = {0};
    *((u32*)one + ((BLOCKSIZE * 2) / 4) - 1) = 1;//set one to 1, aligin to u32, avoid little/big endian problem
    for(int i = BLOCKSIZE; i < BLOCKSIZE * 2; i++)
        tmp[i] = ~(*(b + i));//tmp = ~b
    addBigNum(tmp, one);//tmp = -b
    *((u32*)tmp + (BLOCKSIZE / 4) - 1) = 0;
    return addBigNum(a, tmp);//a=a-b
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
    pa += (BLOCKSIZE / 4);
    pb += (BLOCKSIZE / 4);
    ptrVal += (((BLOCKSIZE * 2) / 4) - 1);
    ptrTmp += (((BLOCKSIZE * 2) / 4) - 1);
    u32 cnta = 0, cntb = 0;//leading zeros counters
    while(0 == *pa){//skip leading zeros
        pa++;
        cnta++;
        if((BLOCKSIZE / 4) == cnta)
            break;
    }
    while(0 == *pb){
        pb++;
        cntb++;
        if((BLOCKSIZE / 4) == cntb)
            break;
    }
    if((BLOCKSIZE / 4 == cnta) || (BLOCKSIZE / 4 == cntb)){//a=0 or b=0, a*b=0, return
        memset(a, 0, BLOCKSIZE * 2);
        return 0;
    }
    cnt = cnta;//inner loop conter
    cnt2 = cntb;//outter loop conter
    pa = (u32*)a + (((BLOCKSIZE * 2) / 4) - 1);//when BLOCKSIZE is 128, this is 63. make pa to point to the last u32 data.
    pb = (u32*)b + (((BLOCKSIZE * 2) / 4) - 1);//make pb to point to the last u32 data

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
        } while (cnt++ != ((BLOCKSIZE / 4) - 1));//when BLOCKSISE is 128, BLOCKSIZE / 4 -cnta loops
        *ptrTmp = (a1 >> 32);//fill the last overflow u32 data from the high 32 bits of a1
        multOverflow = 0;//reset multOverflow
        pa = (u32*)a + ((BLOCKSIZE * 2) / 4) - 1;//reset pa
        ptrTmp += (BLOCKSIZE / 4) -cnta;//reset ptrTmp with relative move
        do
        {
            *ptrVal += (*ptrTmp + addOverflow);//get a final result data, addional overflow must be deal with
            if((*ptrVal - addOverflow < *ptrTmp) || (*ptrVal < addOverflow))
                addOverflow = 1;
            else
                addOverflow = 0;
            ptrVal--;
            ptrTmp--;
        } while (cnt-- != cnta);//1 more loops, when BLOCKSISE is 128, BLOCKSIZE / 4 -cnta +1 loops
        cnt++;//reset cnt,cnt=cnta
        ptrVal += (BLOCKSIZE / 4) - cnta;//reset ptrVal with relative move
        ptrTmp += (BLOCKSIZE / 4) - cnta;//reset ptrTmp
        pb--;//pb move forward
    } while(cnt2++ != ((BLOCKSIZE / 4) - 1));//when BLOCKSISE is 128, BLOCKSIZE / 4 -cnt2 loops
    memcpy(a, val, BLOCKSIZE * 2);
    return *(int*)a;//a int data returned.the sign bit is in it.
}
/*
division
*/
int sDivideBigNumWithQuo(u8 *a, u8 *b32, u8 *r32, u8 *q)//special division when b32 is a 32bits wide number
{
    u8 quo[BLOCKSIZE * 2] = {0};
    u32 *qptr = (u32*)quo + (BLOCKSIZE / 4), *b32ptr = (u32*)b32, b32val;
    u64 rem = 0, overFlow = 0;
    while(0 == *b32ptr){//skip leading zeros
        b32ptr++;
        if(b32ptr - (u32*)b32 == (BLOCKSIZE * 2) / 4 - 1)//test the non-zero bits
            break;
    }
    if(b32ptr - (u32*)b32 != (BLOCKSIZE * 2) / 4 - 1)//if b32 is not a 32bits data
        return -1;
    memcpy(quo, a, BLOCKSIZE * 2);//copy a to quo
    while(0 == *qptr){//skip leading zeros
        qptr++;
        if((qptr - (u32*)quo) == (BLOCKSIZE * 2) / 4)
            break;
    }
    if((qptr - (u32*)quo) == (BLOCKSIZE * 2) / 4)//divided number is 0
        return 0;
    rem = *qptr;//divided number
    b32val = *b32ptr;//avoid too many times of indirect reference to improve performance
    if(0 == b32val)//divide number is 0
        return -1;
    do
    {
        rem = *qptr + overFlow;
        *qptr++ = rem / b32val;
        if(qptr - (u32*)quo == (BLOCKSIZE * 2) / 4){
            rem %= b32val;
            break;
        }
        overFlow = (rem % b32val) << 32;
    } while (1);
    memcpy(q, quo, BLOCKSIZE * 2);
    *((u32*)r32 + (BLOCKSIZE * 2) / 4 -1) = rem;
    return 1;
}
int sDivideBigNum(u8 *a, u8 *b32, u8 *r32)//special division when b32 is a 32bits wide number, without quotient output
{
    u32 *aptr = (u32*)a + (BLOCKSIZE / 4), *b32ptr = (u32*)b32, b32val;
    u64 rem = 0, overFlow = 0;
    while(0 == *b32ptr){//skip leading zeros
        b32ptr++;
        if(b32ptr - (u32*)b32 == (BLOCKSIZE * 2) / 4 - 1)//test the non-zero bits
            break;
    }
    if(b32ptr - (u32*)b32 != (BLOCKSIZE * 2) / 4 - 1)//if b32 is not a 32bits data
        return -1;
    while(0 == *aptr){//skip leading zeros
        aptr++;
        if((aptr - (u32*)a) == (BLOCKSIZE * 2) / 4)
            break;
    }
    if((aptr - (u32*)a) == (BLOCKSIZE * 2) / 4)//divided number is 0
        return 0;
    rem = *aptr;//divided number
    b32val = *b32ptr;//avoid too many times of indirect reference to improve performance
    if(0 == b32val)//divide number is 0
        return -1;
    do
    {
        rem = *(aptr++) + overFlow;
        if(aptr - (u32*)a == (BLOCKSIZE * 2) / 4){
            rem %= b32val;
            break;
        }
        overFlow = (rem % b32val) << 32;
    } while (1);
    *((u32*)r32 + (BLOCKSIZE * 2) / 4 -1) = rem;
    return 1;
}
static inline int modBigNumam(u8 *a, u8 *m, u8 *r)//a % m, a and m are BLOCKSIZE wide
{
    u32 *rptr = (u32*)(r + BLOCKSIZE), *mptr = (u32*)(m + BLOCKSIZE);
    while(0 == *mptr){
        mptr++;
        if(mptr - (u32*)m == (BLOCKSIZE * 2) / 4 - 1)
            break;
    }
    if(0 == *mptr)
        return -1;
    if(mptr - (u32*)m == (BLOCKSIZE * 2) / 4 - 1){//m is u32 bits width and is not 0
        sDivideBigNum(a, m, r);//use special division
        return 1;
    }
    
    mptr = (u32*)(m + BLOCKSIZE);
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
int modBigNumanWithQuo(u8 *a, u8 *m, u8 *r, u8 *quo)
{
    u32 *rptr = (u32*)(r + BLOCKSIZE), *mptr = (u32*)(m + BLOCKSIZE);
    u8 one[BLOCKSIZE * 2] = {0};
    *((u32*)one + ((BLOCKSIZE * 2) / 4) - 1) = 1;//1
    while(0 == *mptr){//skip leading zeros
        mptr++;
        if(mptr - (u32*)m == (BLOCKSIZE * 2) / 4 - 1)
            break;
    }
    if(mptr - (u32*)m == (BLOCKSIZE * 2) / 4 - 1){//test if m is a small u32 data
        if(0 == *mptr)
            return -1;
        else{
            sDivideBigNumWithQuo(a, m, r, quo);//use special division
            return 1;
        }
    }
    memset(quo, 0, BLOCKSIZE * 2);
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
            addBigNum(quo, one);//quo++
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
gcd
*/
int gcdBigNum(u8 *a, u8 *b, u8 *gcd)
{
    u8 aar[BLOCKSIZE * 2] = {0}, bar[BLOCKSIZE * 2] = {0};
    memcpy(aar, a, BLOCKSIZE * 2);
    memcpy(bar, b, BLOCKSIZE * 2);
    u8 *bptr = bar, tag = 1;
    while(tag)
    {
        while(0 == *bptr){
            bptr++;
            if(bptr - bar == BLOCKSIZE * 2) {//b == 0
                tag = 0;
                break;
            }
            continue;
        }
        bptr = bar;
        if(tag)
        {
            modBigNumam(aar, bar, gcd);
            memcpy(aar, bar, BLOCKSIZE * 2);
            memcpy(bar, gcd, BLOCKSIZE * 2);
        }
    }
    memcpy(gcd, aar, BLOCKSIZE * 2);
}
int modMROneEquition(u8 *a, u8 *m, u8 *solution)
{
    u8 aar[BLOCKSIZE * 2] = {0}, mar[BLOCKSIZE * 2] = {0}, gcd[BLOCKSIZE * 2] = {0}, qar[BLOCKSIZE * 2] = {0};
    memcpy(aar, a, BLOCKSIZE * 2);
    memcpy(mar, m, BLOCKSIZE * 2);
    u8 *mptr = mar, tag = 1;
    u8 sar[BLOCKSIZE * 2] = {0}, var[BLOCKSIZE * 2] = {0};
    *((u32*)solution + ((BLOCKSIZE * 2) / 4) - 1) = 1;//x=1;
    while(tag)
    {
        while(0 == *mptr){
            if((++mptr) - mar == BLOCKSIZE * 2) {//b == 0
                tag = 0;
                break;
            }
            continue;
        }
        mptr = mar;
        if(tag)
        {
            modBigNumanWithQuo(aar, mar, gcd, qar);
            memcpy(sar, solution, BLOCKSIZE * 2);//s=x
            if(*((int*)var + ((BLOCKSIZE * 2) / 8)) >= 0)
            {
                multBigNum(qar, var);
                subBigNum(sar, qar);//s=x-q*v
                *((u32*)sar + ((BLOCKSIZE * 2) / 8) - 1) = 0;//remove overflow part if exists
                memcpy(solution, var, BLOCKSIZE * 2);//x=v
                memcpy(var, sar, BLOCKSIZE * 2);//v=s
            }
            else
            {
                addReverse(var);
                multBigNum(qar, var);
                addBigNum(sar, qar);
                *((u32*)sar + ((BLOCKSIZE * 2) / 8) - 1) = 0;//remove overflow part if exists
                memcpy(solution, var, BLOCKSIZE * 2);//x=v
                memcpy(var, sar, BLOCKSIZE * 2);//v=s
            }
            memcpy(aar, mar, BLOCKSIZE * 2);
            memcpy(mar, gcd, BLOCKSIZE * 2);
        }
    }
    if(*((u32*)aar + ((BLOCKSIZE * 2) / 4) - 1) != 1)//gcd != 1
        return -1;
    modBigNumam(solution, m, solution);
    return 1;
}
/*
a^k % m = r
*/
int powerModeBigNum(u8 *a, u8 *m, u8 *k, u8 *r)
{
    u8 rem[BLOCKSIZE * 2] = {0}, *remptr, key[BLOCKSIZE * 2] = {0};
    remptr = rem;
    modBigNumam(a, m, rem);
    while(0 == *remptr)
    {
        if(((++remptr) - rem) == BLOCKSIZE * 2)
            break;
    }
    if((remptr - rem) == BLOCKSIZE * 2)
        return 0;//a % m == 0
    memset(rem, 0, BLOCKSIZE * 2);
    *((u32*)rem + ((BLOCKSIZE * 2) / 4) -1) = 1;//rem = 1
    u32 *kptr = (u32*)key + BLOCKSIZE / 4, cnt = 0, cnt2 = BLOCKSIZE / 4;
    memcpy(key, k, BLOCKSIZE * 2);
    while(0 == *kptr)//skip leading zeros
    {
        kptr++;
        cnt2--;
        if(0 == cnt2)
            break;
    }//
    if(0 == cnt2)
        return -1;//k=0
    kptr = (u32*)key + (BLOCKSIZE * 2) / 4 - 1;
    do
    {
        if(1 != cnt2)
        {
            if(1 == *kptr & 0x01){
                modBigNum(a, rem, m, rem);//a*rem % m -> rem
            }
            modBigNum(a, a, m, a);//a*a % m -> a
            *kptr >>= 1;
            if(0 == (++cnt) % 32){
                kptr--;
                cnt2--;
            }
            continue;
        }
        else if(*kptr >= 1)
        {
            if(1 == *kptr & 0x01){
                modBigNum(a, rem, m, rem);//a*rem % m -> rem
            }
            modBigNum(a, a, m, a);//a*a % m -> a
            *kptr >>= 1;
        }
        else
        {
            break;
        }
    }while(1);

    memcpy(r, rem, BLOCKSIZE * 2);
    return 1;
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
        while(0 == *p){//skip leading zeros
            if((++p) - product == BLOCKSIZE)
                break;
        }
        if(p - product == BLOCKSIZE)
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

    memcpy(divided, src, BLOCKSIZE * 2);
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
/*
digits string to number
*/
int StringToNumber(u8 *src, u8 *des)
{
    u32 slen = strlen(src);
    //if(slen > BLOCKSIZE * 2)//conservative estimate
        //return -1;
    u32 rem = 0, rem2 = 0, *dptr;//rem and rem2 to store remainder value, dptr is a write pointer of des
    u8 sar[slen], *sptr;//sar to store digit exchanged from char of ascii code
    sptr = sar;
    memcpy(sar, src, slen);
    while(sptr != sar + slen){
        if(isdigit(*sptr))
            *sptr++ -= 0x30;//exchange char of ascii code to digit
        else
            return -1;//illegal src string contains non digit symbol
    }
    sptr = sar;//sptr point to the start position of sar 
    dptr = (u32*)des + (BLOCKSIZE * 2 / 4)- 1;//dptr point to the end position of des
    u32 cnt = 0;
    do
    {
        while(0 == *sptr){//skip leading zeros
            sptr++;
            if(sptr - sar == slen)//sar equals 0
                break;
        }
        if(sptr - sar == slen)//outer loop break condition is sar eqauls 0
            break;
        do
        {
            rem2 = (rem + *sptr) & 0x01;//divide 2 remainder
            *sptr = (rem + *sptr) >> 1;//divide 2 quotient
            rem = rem2;//update rem value for the next loop
            if((++sptr) - sar == slen)
                break;//inner loop break condition is sar traversal finished
            if(rem)
                rem = 10;//carrier value
        } while (1);
        if(rem)
            *dptr += (rem << cnt);//get one bit store to des
        rem = 0;//reset rem
        if(32 == ++cnt){
            dptr--;//move forward to higher position
            if(dptr == (u32*)des + (BLOCKSIZE / 4) - 1)//out of band
                return -1;
            cnt = 0;//reset cnt
        }
        sptr = sar;//reset sptr
    } while (1);
    return 0;
}
/*
number to digits string
*/
int NumberToString(u8 *src, u8 *des)
{
    u8 quo[BLOCKSIZE * 2] = {0}, divided[BLOCKSIZE * 2] = {0};//arrarys to store mid value of calculate process, quo is quotient, divided is divided numbers
    u32 *qptr = (u32*)quo, *dptr = (u32*)divided;//pointers for traversal
    u64 rem = 0;//rem is remainder, initialize to 0
    u8 ss[BLOCKSIZE * 6 + 1] = {0}, *ssptr;//a template to initialize des
    memset(ss, '0', BLOCKSIZE * 6);
    ssptr = ss + BLOCKSIZE * 6 - 1;//point to the last '0' position, the lowest digit position
    if(strlen(des) < 6 * BLOCKSIZE)
        return -1;//des size too small
    memcpy(des, ss, BLOCKSIZE * 6 + 1);//initialize des
    memcpy(divided, src, BLOCKSIZE * 2);//initialize diveded number
    do
    {
        while(0 == *dptr){//skip leading zeros
            dptr++;
            qptr++;
            if(dptr - (u32*)divided == BLOCKSIZE * 2 / 4)//divided number equals 0
                break;
            continue;
        }
        if(dptr - (u32*)divided == BLOCKSIZE * 2 / 4)//outer loop break condition is divided number equals 0
            break;
        do
        {
            *qptr++ = (rem + *dptr) / 10;//fill quo arrary
            rem = (rem + *dptr++) % 10;
            if(dptr - (u32*)divided == BLOCKSIZE * 2 / 4)//inner loop break condition is divided arrary traversal finished
                break;
            rem <<= 32;
        } while (1);
        *ssptr-- = rem + 0x30;//get a digit from 0 to 9
        rem = 0;//reset rem
        if(ssptr == ss)
            return -1;//out of band
        memcpy(divided, quo, BLOCKSIZE * 2);//copy quo arrary to dividec arrary
        memset(quo, 0, BLOCKSIZE * 2);//empty quo arrary
        dptr = (u32*)divided;//reset dptr
        qptr = (u32*)quo;//reset qptr
    } while (1);
    memcpy(des, ss, BLOCKSIZE * 6 + 1);
    return ssptr - ss + 1;//number of leading zeros
}
int sllBigNum(u8 *a, u32 k)//logical shift left operation
{
    if(k >= BLOCKSIZE * 8)
        return -1;
    u32 q = k / 32, r = k % 32, *aptr =  (u32*)a + BLOCKSIZE / 4 + 1;
    u64 tmp = 0;
    union
    {
        u64 t;
        struct
        {
            u32 x1;
            u32 x2;
        }x;
    }slr = {0};
    while(q--){
        while(aptr != (u32*)a + BLOCKSIZE * 2 / 4){
            *(aptr - 1) = *aptr;
            aptr++;
        }
        *(aptr - 1) = *aptr;
        *aptr = 0;
        aptr = (u32*)a + BLOCKSIZE / 4 + 1;
    }
    aptr = (u32*)a + BLOCKSIZE * 2 / 4 - k / 32;
    while(aptr != (u32*)a + BLOCKSIZE * 2 / 4){
        slr.x.x1 = *aptr;
        slr.t <<= r;
        *aptr = slr.x.x1 + tmp;
        tmp = slr.x.x2;
        slr.t = 0;
    }
}
int slrBigNum(u8 *a, u32 k)//logical shift right operation
{
    if(k >= BLOCKSIZE * 8)
        return -1;
    u32 q = k / 32, r = k % 32, *aptr =  (u32*)a + BLOCKSIZE * 2 / 4 - 2;
    u64 tmp = 0;
    union//little edian problly work
    {
        u64 t;
        struct
        {
            u32 x1;
            u32 x2;
        }x;
    }slr = {0};
    while(q--){
        while(aptr != (u32*)a + BLOCKSIZE / 4){
            *(aptr + 1) = *aptr;
            aptr--;
        }
        *(aptr + 1) = *aptr;
        *aptr = 0;
        aptr = (u32*)a + BLOCKSIZE * 2 / 4 - 2;
    }
    aptr = (u32*)a + BLOCKSIZE / 4 + k / 32;
    while(aptr != (u32*)a + BLOCKSIZE * 2 / 4){
        slr.x.x2 = *aptr;
        slr.t >>= r;
        *aptr++ = slr.x.x2 + tmp;
        tmp = slr.x.x1;
        slr.t = 0;
    }
}
int equalBigNum(u8 *a, u8 *b)
{
    u32 *aptr = (u32*)a, *bptr = (u32*)b;
    while(aptr != (u32*)a + BLOCKSIZE * 2 / 4)
    {
        if(*aptr++ == *bptr++)
            continue;
        else
            return 0;
    }
    if(*aptr == *bptr)
        return 1;
    else
        return 0;
}
/*
rabin-miller
*/
int rabinMiller(u8 *x, u8 *a)
{
    u8 q[BLOCKSIZE * 2] = {0}, xSubOne[BLOCKSIZE * 2] = {0}, one[BLOCKSIZE * 2] = {0}, nagativeOne[BLOCKSIZE * 2] = {0};
    u8 rem[BLOCKSIZE * 2] = {0};
    u32 *xptr = (u32*)xSubOne + (BLOCKSIZE * 2 / 4) -1, k = 0;
    memcpy(xSubOne, x, BLOCKSIZE * 2);
    *((u32*)one + BLOCKSIZE * 2 / 4 - 1) = 1;
    subBigNum(xSubOne, one);
    *(xptr + BLOCKSIZE / 4 - 1) = 0;//erase overflow if exist
    memcpy(nagativeOne, xSubOne, BLOCKSIZE * 2);//-1 = x-1 % x
    while(!(*xptr & 0x01)){
        if(!(*xptr)){
            k += 32;
            xptr--;
        }
        else
        {
            k++;
            *xptr >>= 1;//xSubOne is overriding here
        }
        if(xptr - (u32*)xSubOne == BLOCKSIZE / 4 - 1)
            return -1;//x is an illegal number
    }
    memcpy(q, nagativeOne, BLOCKSIZE * 2);
    slrBigNum(q, k);
    powerModeBigNum(a, x, q, rem);
    if(equalBigNum(rem, one))
        return 0;
    for(u32 i = 0; i < k; i++)
    {
        sllBigNum(q, i);
        powerModeBigNum(a, x, q, rem);
        if(equalBigNum(rem, nagativeOne))
            return 0;
    }
    return 1;
}