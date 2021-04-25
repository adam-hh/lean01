#ifndef COMMONHEADER
#define COMMONHEADER
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#define PRIMEUPL 1000000u//up limits number to be test
#define PTABLESIZE 500000u//prime table size
#define PFACTLISTSIZE 1000u//prime fact table size
extern int tadd_ok(int64_t, int64_t);
extern int uadd_ok(u_int64_t, u_int64_t);
extern int tmult_ok(int64_t, int64_t);
extern int umult_ok(u_int64_t, u_int64_t);
extern u_int64_t powu64(u_int64_t, u_int64_t);

extern int64_t mode(int64_t, int64_t, int64_t*);
extern int64_t gcd(int64_t, int64_t);
extern int64_t linearEquition(int64_t, int64_t, int64_t*, int64_t*);
extern int64_t linearEquitionM(int64_t, int64_t, int64_t, int64_t*, int64_t*);

extern u_int64_t pTable[PTABLESIZE];//prime number table, from 2 to PRIEUPL
extern void createpTable();
extern int primeFact(u_int64_t, u_int64_t**);
extern u_int64_t funcFai(u_int64_t);
extern u_int64_t funcDelta(u_int64_t);
extern u_int64_t funcS(u_int64_t);
extern int64_t chinaRemainder(int64_t, int64_t, int64_t, int64_t);

extern u_int64_t powerMode(u_int64_t a, u_int64_t m, u_int64_t k);
extern u_int64_t resolvePowerMode(u_int64_t k, u_int64_t b, u_int64_t m);

#endif