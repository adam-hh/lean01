#include <inttypes.h>
/*
Determine whether arguments can be added without overflow
*/
int tadd_ok(int64_t x, int64_t y){
    int64_t sum = x + y;
    //return(!(((x > 0) && (y > 0) && (sum <= 0)) || ((x < 0) && (y < 0)&& (sum >= 0))));
    return(!((x < 0 == y < 0) && (sum < 0 != x < 0)));
}
int uadd_ok(uint64_t x, uint64_t y){
    uint64_t sum = x + y;
    //return(!((sum < x) && (sum < y)));
    return(!(sum < x));
}

/*
Determine whether arguments can be multiplied without overflow
*/
int tmult_ok(int64_t x, int64_t y){
    int64_t p = x * y;
    // Either x is zero, or dividing p by x gives y
    return !x || p / x == y;
}
int umult_ok(u_int64_t x, u_int64_t y){
    u_int64_t p = x * y;
    return !x || p / x == y;
}

/*
power
*/
u_int64_t powu64(u_int64_t x, u_int64_t y)
{
    u_int64_t val = x;
    if(0 == y)
        return 1;
    if(1 == y)
        return val;
    while(1 != y--)
        val *= x;
    return val;
}