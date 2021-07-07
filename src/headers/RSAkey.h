#ifndef RSAKEY_H
#define RSAKEY_H
#include "bigNumber.h"

#ifdef __cpluscplus
extern "C" {
#endif

struct rsakey1024 {
    N128 mode;
    N128 fai;
    N128 p;
    N128 q;
    N128 privateKey;
    N128 publicKey;
};
struct rsakey2048 {
    N256 mode;
    N256 fai;
    N256 p;
    N256 q;
    N256 privateKey;
    N256 publicKey;
};

extern int genPrivatekey1024(struct rsakey1024 *key);
extern int genPrivateKey2048(struct rsakey2048 *key);

extern int cryptw(const N128 *plain, struct rsakey1024 *key, N128 *cypher);
extern int cryptd(const N256 *plain, struct rsakey2048 *key, N256 *cypher);

extern int descryptw(const N128 *cypher, struct rsakey1024 *key, N128 *plain);
extern int descryptd(const N256 *cypher, struct rsakey2048 *key, N256 *plain);

#ifdef __cpluscplus
{
#endif

#endif  /*RSAKEY_H*/