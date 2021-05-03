#include "commonHeader.h"
#include <string.h>
extern int addBigNum(u8 *a, u8 *b);
extern int multBigNum(u8 *a, u8 *b);
extern int subBigNum(u8 *a, u8 *b);
extern int decDisp(u8 *src, u8 **des);
extern int modBigNum(u8 *a, u8 *b, u8 *m, u8 *r);
extern int modBigNumanWithQuo(u8 *a, u8 *m, u8 *r, u8 *quo);
extern int gcdBigNum(u8 *a, u8 *b, u8 *gcd);
extern int modMROneEquition(u8 *a, u8 *m, u8 *solution);
extern int powerModeBigNum(u8 *a, u8 *m, u8 *k, u8 *r);
extern int StringToNumber(u8 *src, u8 *des);
extern int NumberToString(u8 *src, u8 *des);

int main()
{
    u8 x[BLOCKSIZE * 2] = {0}, y[BLOCKSIZE * 2] = {0}, cnt = (BLOCKSIZE / 4), remainder[BLOCKSIZE * 2] = {0};
    u8 z[BLOCKSIZE * 2] = {0}, q[BLOCKSIZE * 2] = {0};
    u32 *xptr = (u32*)x, *yptr = (u32*)y, *zptr = (u32*)z;
    u8 p1[BLOCKSIZE * 2] = {0};
    *((u32*)p1 + (BLOCKSIZE * 2) / 4 - 1) = 65537;
    do
    {
        *(xptr + cnt) = random() + 1;
        *(yptr + cnt) = rand() + 1;
        *(zptr + cnt) = rand();
    } while (cnt++ != (((BLOCKSIZE * 2) / 4) - 1));
    u8 str[BLOCKSIZE * 6 + 1] = {0};
    u8 in[] = "064343075650832924466657686176469695105543116940669731556838135140020422445172228926643085867380685746992944957039962473951270165081640803794163470954944298645581530550047885900856614299711529602283508608395310814545244160";
    u32 n, m;
    memset(str, '0', BLOCKSIZE * 6);
    n = NumberToString(x, str);
    if(-1 == n)
        return -1;
    printf("origin x is\n%s\n", str + n);
    m = StringToNumber(in, remainder);
    if(!m){
        printf("in to number sucess.\n");
        n = NumberToString(remainder, str);
        printf("in is\n%s\n", str + n);
    }
    return 1;
}