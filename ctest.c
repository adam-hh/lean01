#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#define A 22241739
#define B 19848039
int64_t linearEquition(int64_t a, int64_t b, int64_t *x, int64_t *y);
int main()
{
    //printf("hello world.\n");
    //return 0;
    int64_t x, y, a, b;
    a = A;
    b = B;
    printf("gcd(%" PRId64 ", %" PRId64 ") is %" PRId64 " \n", a, b, linearEquition(a, b, &x, &y));
    printf("solution of %" PRId64 "x + %" PRId64 "y = gcd(%" PRId64 ", %" PRId64 ") is\n(x, y) = (%" PRId64 ", %" PRId64 ")\n", a, b, a, b, x, y);
        
}