/**
 * An Iimplementation of AES alogrithm
 * Copyright (c) 2020-2021, Adam hh <adamhh.wk@gmail.com>
 * ADVANCED ENCRYPTION STANDARD(AES)
 * Reference : https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
 */
#ifndef AES_H
#define AES_H

#include <stdint.h>           /* integer types */
#include <stddef.h>           /* NULL pointer */
#include <string.h>           /* memcpy operation */

#ifdef __cpluscplus
extern "C" {
#endif

#define AES_BLOCK_SIZE  16    /* fixed block size */
#define AES_MAXNR       14    /* max rounds */

/* Error codes */
enum {
  AESSuccess = 0,
  AESNullPointer,             /* null pointer */
  AESStateError,              /* work flow error, eg. uninitialized keyset */
  AESBadParam                 /* uer's key length wrong */
};

/* specification key bit length */
enum {
  AESKey128 = 128,
  AESKey192 = 192,
  AESKey256 = 256
};

/* AES CFB Submodes */
enum {
  AESCFB_1,
  AESCFB_8,
  AESCFB_64,
  AESCFB_128
};

/**
 * AES key set 
 * user's key need to be transformed to this fixed spec
 */
typedef struct AESKeySet {
  uint32_t  key[4 * (AES_MAXNR + 1)];
  int       rounds;
} AESKeySet;

typedef void (*block128_f) (const unsigned char in[16],
                            unsigned char out[16], const void *key);

typedef void (*cbc128_f) (const unsigned char *in, unsigned char *out,
                          size_t len, const void *key,
                          unsigned char ivec[16], int enc);

typedef void (*ecb128_f) (const unsigned char *in, unsigned char *out,
                          size_t len, const void *key,
                          int enc);

typedef void (*ctr128_f) (const unsigned char *in, unsigned char *out,
                          size_t blocks, const void *key,
                          const unsigned char ivec[16]);

typedef void (*ccm128_f) (const unsigned char *in, unsigned char *out,
                          size_t blocks, const void *key,
                          const unsigned char ivec[16],
                          unsigned char cmac[16]);

extern int AESSetKey(const void *userKey, size_t bits, AESKeySet *kset);

extern int AESEncrpto(const void *in, void *out, const AESKeySet *kset);
extern int AESDecrpto(const void *in, void *out, const AESKeySet *kset);


#ifdef __cpluscplus
}
#endif 

#endif      /* AES_H */