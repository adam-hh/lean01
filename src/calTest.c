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

int main()
{
    u8 x[BLOCKSIZE * 2] = {0}, y[BLOCKSIZE * 2] = {0}, cnt = (BLOCKSIZE / 4), remainder[BLOCKSIZE * 2] = {0};
    u8 z[BLOCKSIZE * 2] = {0}, q[BLOCKSIZE * 2] = {0};
    u32 *xptr = (u32*)x, *yptr = (u32*)y, *zptr = (u32*)z;
    u8 p1[BLOCKSIZE * 2] = {0};
    *((u32*)p1 + (BLOCKSIZE * 2) / 4 - 1) = 65537;
    //rand();
    //u_int8_t idx = 128;
    do
    {
        *(xptr + cnt) = random() + 1;
        *(yptr + cnt) = rand() + 1;
        *(zptr + cnt) = rand();
    } while (cnt++ != (((BLOCKSIZE * 2) / 4) - 1));
    /* printf("origin x:\n");
    for(cnt = 0; cnt != ((BLOCKSIZE * 2) / 4); cnt++)
        printf("%10u,", *(xptr + cnt));
    putchar('\n');
    printf("origin y:\n");
    for(cnt = 0; cnt != ((BLOCKSIZE * 2) / 4); cnt++)
        printf("%10u,", *(yptr + cnt));
    putchar('\n'); */
    u8 *str = NULL;
    int n;
    n = decDisp(x, &str);
    printf("origin x is\n%s\n", str + n);
    free(str);
    n = decDisp(y, &str);
    printf("origin y is\n%s\n", str + n);
    free(str);
    n = decDisp(z, &str);
    printf("origin z is\n%s\n", str + n);
    free(str);
    //int i = multBigNum(x, y);
    //memcpy(z, x, BLOCKSIZE * 2);
    //modBigNumam(z, x, z);
    //modBigNum(x, y, z, remainder);
    //modBigNumanWithQuo(y, x, remainder, q);
    //gcdBigNum(x, y, remainder);
    modMROneEquition(x, y, remainder);
    n = decDisp(remainder, &str);
    printf("xk = 1 (mod y) is\n%s\n", str + n);
    free(str);
    //n = decDisp(q, &str);
    //printf("y%%x quotient is\n%s\n", str + n);
    //free(str);
    //printf("sign val is %d\n", i);
    //printf("new    y is\n%s\n", decDisp(y));
    /* printf("new    x:\n");
    for(cnt = 0; cnt != ((BLOCKSIZE * 2) / 4); cnt++)
        printf("%10u,", *(xptr + cnt));
    putchar('\n'); */
    //printf("dec display x is\n%s\n", decDisp(x));
}