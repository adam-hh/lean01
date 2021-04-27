#include "commonHeader.h"

/*
a[BLOCKSISE * 2] + b[BLOCKSIXE * 2] -> a
*/
int addBigNum(u8 *a, u8 *b)
{
    register u64 *pa = (u64*)a, *pb = (u64*)b, cnt = 0, addOverflow = 0;
    pa += (((BLOCKSIZE * 2) / 8) - 1);
    pb += (((BLOCKSIZE * 2) / 8) - 1);
    do
    {
        *pa += (*pb + addOverflow);
        if(*(pa--) - addOverflow < *(pb--))
            addOverflow = 1;
        else
            addOverflow = 0;
    } while (cnt++ != (BLOCKSIZE / 8));//when BLOCKSIZE is 128, this 17 loops
    
    return *(pa + 1);
}
/*
a[BLOCKSIZE * 2] * b[BLOCKSIZE * 2] -> a
*/
int multBigNum(u8 *a, u8 *b)
{
    register u32 *pa = (u32*)a, *pb = (u32*)b, multOverflow = 0, cnt = 0, cnt2 = 0, addOverflow = 0;
    register u64 a1, b1;
    u8 val[BLOCKSIZE * 2] = {0}, tmp[BLOCKSIZE * 2] = {0};
    register u32 *ptrVal = (u32*)val, *ptrTmp = (u32*)tmp;
    pa += (((BLOCKSIZE * 2) / 4) - 1);//when BLOCKSIZE is 128, this is 63
    pb += (((BLOCKSIZE * 2) / 4) - 1);
    ptrVal += (((BLOCKSIZE * 2) / 4) - 1);
    ptrTmp += (((BLOCKSIZE * 2) / 4) - 1);
    do
    {
        do
        {
            a1 = *(pa--);
            b1 = *pb;
            a1 *= b1;//no multOverflow
            a1 += multOverflow;//no multOverflow in math   
            multOverflow = (a1 >> 32);
            *(ptrTmp--) = ((a1 << 32) >> 32);
        } while (cnt++ != ((BLOCKSIZE / 4) - 1));//when BLOCKSISE is 128, this 32 loops
        *ptrTmp = (a1 >> 32);//fill the last multOverflow dword
        multOverflow = 0;//reset multOverflow
        pa += cnt;//reset pa
        ptrTmp += cnt;//reset ptrTmp
        do
        {
            *ptrVal += (*ptrTmp + addOverflow);//multOverflow to be deal with
            if(*(ptrVal--) - addOverflow < *(ptrTmp--))
                addOverflow = 1;
            else
                addOverflow = 0;
        } while (cnt-- != 0);//1 more loops, when BLOCKSISE is 128, this 33 loops
        cnt = 0;//reset cnt
        ptrVal += ((BLOCKSIZE / 4) + 1);//reset ptrVal
        ptrTmp += ((BLOCKSIZE / 4) + 1);//reset ptrTmp
        pb --;
        ptrVal --;
        ptrTmp --;
    } while(cnt2++ != ((BLOCKSIZE / 4) - 1));//when BLOCKSISE is 128, this 32 loops
    for(int i = 0; i < (BLOCKSIZE * 2); i++)
        *(a + i) = val[i];//fill back mult result
    return 1;   
}