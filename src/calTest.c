#include "commonHeader.h"
extern int addBigNum(u8 *a, u8 *b);
extern int multBigNum(u8 *a, u8 *b);
int main()
{
    u8 x[BLOCKSIZE * 2] = {0}, y[BLOCKSIZE * 2] = {0}, cnt = (BLOCKSIZE / 4);
    u32 *xptr = (u32*)x, *yptr = (u32*)y;
    //rand();
    //u_int8_t idx = 128;
    do
    {
        *(xptr + cnt) = rand();
        *(yptr + cnt) = rand();
    } while (cnt++ != (((BLOCKSIZE * 2) / 4) - 1));
    printf("origin x:\n");
    for(cnt = 0; cnt != ((BLOCKSIZE * 2) / 4); cnt++)
        printf("%10u,", *(xptr + cnt));
    putchar('\n');
    printf("origin y:\n");
    for(cnt = 0; cnt != ((BLOCKSIZE * 2) / 4); cnt++)
        printf("%10u,", *(yptr + cnt));
    putchar('\n');

    multBigNum(x, y);
    printf("new    x:\n");
    for(cnt = 0; cnt != ((BLOCKSIZE * 2) / 4); cnt++)
        printf("%10u,", *(xptr + cnt));
    putchar('\n');
}