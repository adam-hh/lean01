#include "aes.h"
#include "bitArray.h"     /* static bit arrays for convinens */

/* Alias of AES Block, a 4 by 4 bytes arrary */
typedef uint32_t AES_BLOCK[4];

/* static S_Box for substitution step */
static const uint8_t AES_S_Box[16][16] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

/* static INV_S_Box for inverse substitution step */
static const uint8_t AES_INV_S_Box[16][16] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

/* static left-mult matrix for mix columns step */
static const uint8_t AES_MixArray[4][4] = {
  0x02, 0x03, 0x01, 0x01,
  0x01, 0x02, 0x03, 0x01,
  0x01, 0x01, 0x02, 0x03,
  0x03, 0x01, 0x01, 0x02
};

/* static left-mult matrix for inverse mix columns step */
static const uint8_t AES_INV_MixArray[4][4] = {
  0x0e, 0x0b, 0x0d, 0x09,
  0x09, 0x0e, 0x0b, 0x0d,
  0x0d, 0x09, 0x0e, 0x0b,
  0x0b, 0x0d, 0x09, 0x0e
};

/* static rcon array for key expansion step */
static const uint8_t AES_Rcon[] = {
  0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a
};

/**
 * AddRoundKey operation
 * param type : uint32_t[4]
 * Input:   matrixa, matrixb
 * Output:  matrixa
 * Return:  none
 */
#define AddRoundKey(blocka, blockb) \
        blocka[0] ^= blockb[0]; \
        blocka[1] ^= blockb[1]; \
        blocka[2] ^= blockb[2]; \
        blocka[3] ^= blockb[3]

/* Cast uint32_t *ptr to uint8_t *ptr */
#define CastWToB(blocka) ((uint8_t *)blocka)

/**
 * Substitution operation
 * param type : uint8_t, uint8_t[16][16]
 * Input:   in, matrix(s-box or inv-s-box)
 * Output:  none
 * Return:  matrix[in >> 4][in &0x0f]
 */
#define SubByte(in, sbox) \
        sbox[in >> 4][in &0x0f]

/* S-Box substitution */
#define SSubstitution(blocka) \
        for (int i = 0; i < 16; i++) \
          *(CastWToB(blocka) + i) = SubByte(*(CastWToB(blocka) + i), AES_S_Box)

/* INV-S-Box substitution */
#define InvSSubstitution(blocka) \
        for (int i = 0; i < 16; i++) \
          *(CastWToB(blocka) + i) = SubByte(*(CastWToB(blocka) + i), AES_INV_S_Box)

/**
 * Shift rows operation, it is byte order relavant operation.
 * There is method to irrelevant byte order with performance lost.
 * We use static macro to detect byte order
 * param type : uint32_t[4]
 */
#ifndef BIGEDIAN_MACHINE
#define ShiftRows(blocka) \
        blocka[0] = blocka[0]; \
        blocka[1] = (blocka[1] >> 8)  | (blocka[1] << 24); \
        blocka[2] = (blocka[2] >> 16) | (blocka[2] << 16); \
        blocka[3] = (blocka[3] >> 24) | (blocka[3] << 8)
#else
#define ShiftRows(blocka) \
        blocka[0] = blocka[0]; \
        blocka[1] = (blocka[1] << 8)  | (blocka[1] >> 24); \
        blocka[2] = (blocka[2] << 16) | (blocka[2] >> 16); \
        blocka[3] = (blocka[3] << 24) | (blocka[3] >> 8)
#endif

/* Inverse shift rows operation */
#ifndef BIGEDIAN_MACHINE
#define InvShiftRows(blocka) \
        blocka[0] = blocka[0]; \
        blocka[1] = (blocka[1] << 8)  | (blocka[1] >> 24); \
        blocka[2] = (blocka[2] << 16) | (blocka[2] >> 16); \
        blocka[3] = (blocka[3] << 24) | (blocka[3] >> 8)
#else
#define InvShiftRows(blocka) \
        blocka[0] = blocka[0]; \
        blocka[1] = (blocka[1] >> 8)  | (blocka[1] << 24); \
        blocka[2] = (blocka[2] >> 16) | (blocka[2] << 16); \
        blocka[3] = (blocka[3] >> 24) | (blocka[3] << 8)
#endif

/**
 * xtime operation
 * param type : uint8_t
 */
#define AES_XTime(c)  (c & 0x80) ? (c << 1) ^ 0x1b : (c << 1)

/**
 * GF(2^8) multiplication operation, a x b mod 0x011b
 * 0x011b is a given prime number
 * input:   a, b
 * output:  none
 * return:  product of a * b
 */
static inline uint8_t AESMult(uint8_t a, uint8_t b)
{
  uint8_t product;

  product = 0;
  while (b) {
    product ^= (b & 0x01) ? a : 0;
    a = AES_XTime(a);
    b >>= 1;
  }
  return product;
}

/**
 * Mix columns operation
 * AES_MixArray left mult blocka
 * input:   blocka
 * output:  blocka
 * return:  void
 */
static inline void MixColums(AES_BLOCK blocka)
{
  AES_BLOCK blockabak;
  uint8_t   *ptrOfA[4];
  uint8_t   *ptrOfAbak[4];

  ptrOfA[0] = CastWToB(blocka);
  ptrOfA[1] = CastWToB(blocka) + 4;
  ptrOfA[2] = CastWToB(blocka) + 8;
  ptrOfA[3] = CastWToB(blocka) + 12;

  blockabak[0] = blocka[0];
  blockabak[1] = blocka[1];
  blockabak[2] = blocka[2];
  blockabak[3] = blocka[3];
  ptrOfAbak[0] = CastWToB(blockabak);
  ptrOfAbak[1] = CastWToB(blockabak) + 4;
  ptrOfAbak[2] = CastWToB(blockabak) + 8;
  ptrOfAbak[3] = CastWToB(blockabak) + 12;

  for (int i = 0; i < 4; i++) {
    ptrOfA[i][0] = 
    AESMult(ptrOfAbak[0][0], AES_MixArray[i][0]) ^
    AESMult(ptrOfAbak[1][0], AES_MixArray[i][1]) ^
    AESMult(ptrOfAbak[2][0], AES_MixArray[i][2]) ^
    AESMult(ptrOfAbak[3][0], AES_MixArray[i][3]);
    ptrOfA[i][1] = 
    AESMult(ptrOfAbak[0][1], AES_MixArray[i][0]) ^
    AESMult(ptrOfAbak[1][1], AES_MixArray[i][1]) ^
    AESMult(ptrOfAbak[2][1], AES_MixArray[i][2]) ^
    AESMult(ptrOfAbak[3][1], AES_MixArray[i][3]);
    ptrOfA[i][2] = 
    AESMult(ptrOfAbak[0][2], AES_MixArray[i][0]) ^
    AESMult(ptrOfAbak[1][2], AES_MixArray[i][1]) ^
    AESMult(ptrOfAbak[2][2], AES_MixArray[i][2]) ^
    AESMult(ptrOfAbak[3][2], AES_MixArray[i][3]);
    ptrOfA[i][3] = 
    AESMult(ptrOfAbak[0][3], AES_MixArray[i][0]) ^
    AESMult(ptrOfAbak[1][3], AES_MixArray[i][1]) ^
    AESMult(ptrOfAbak[2][3], AES_MixArray[i][2]) ^
    AESMult(ptrOfAbak[3][3], AES_MixArray[i][3]);
  }
}

/**
 * Inverse mix columns operation
 * AES_INV_MixArray left mult blocka
 * input:   blocka
 * output:  blocka
 * return:  void
 */
static inline void InvMixColums(AES_BLOCK blocka)
{
  AES_BLOCK blockabak;
  uint8_t   *ptrOfA[4];
  uint8_t   *ptrOfAbak[4];

  ptrOfA[0] = CastWToB(blocka);
  ptrOfA[1] = CastWToB(blocka) + 4;
  ptrOfA[2] = CastWToB(blocka) + 8;
  ptrOfA[3] = CastWToB(blocka) + 12;

  blockabak[0] = blocka[0];
  blockabak[1] = blocka[1];
  blockabak[2] = blocka[2];
  blockabak[3] = blocka[3];
  ptrOfAbak[0] = CastWToB(blockabak);
  ptrOfAbak[1] = CastWToB(blockabak) + 4;
  ptrOfAbak[2] = CastWToB(blockabak) + 8;
  ptrOfAbak[3] = CastWToB(blockabak) + 12;

  for (int i = 0; i < 4; i++) {
    ptrOfA[i][0] = 
    AESMult(ptrOfAbak[0][0], AES_INV_MixArray[i][0]) ^
    AESMult(ptrOfAbak[1][0], AES_INV_MixArray[i][1]) ^
    AESMult(ptrOfAbak[2][0], AES_INV_MixArray[i][2]) ^
    AESMult(ptrOfAbak[3][0], AES_INV_MixArray[i][3]);
    ptrOfA[i][1] = 
    AESMult(ptrOfAbak[0][1], AES_INV_MixArray[i][0]) ^
    AESMult(ptrOfAbak[1][1], AES_INV_MixArray[i][1]) ^
    AESMult(ptrOfAbak[2][1], AES_INV_MixArray[i][2]) ^
    AESMult(ptrOfAbak[3][1], AES_INV_MixArray[i][3]);
    ptrOfA[i][2] = 
    AESMult(ptrOfAbak[0][2], AES_INV_MixArray[i][0]) ^
    AESMult(ptrOfAbak[1][2], AES_INV_MixArray[i][1]) ^
    AESMult(ptrOfAbak[2][2], AES_INV_MixArray[i][2]) ^
    AESMult(ptrOfAbak[3][2], AES_INV_MixArray[i][3]);
    ptrOfA[i][3] = 
    AESMult(ptrOfAbak[0][3], AES_INV_MixArray[i][0]) ^
    AESMult(ptrOfAbak[1][3], AES_INV_MixArray[i][1]) ^
    AESMult(ptrOfAbak[2][3], AES_INV_MixArray[i][2]) ^
    AESMult(ptrOfAbak[3][3], AES_INV_MixArray[i][3]);
  }
}

/* 4 x 4 matrix permutation */
#define MatrixPermutation(matrix, ex) \
        ex = matrix[0][1]; \
        matrix[0][1] = matrix[1][0]; \
        matrix[1][0] = ex; \
        ex = matrix[0][2]; \
        matrix[0][2] = matrix[2][0]; \
        matrix[2][0] = ex; \
        ex = matrix[0][3]; \
        matrix[0][3] = matrix[3][0]; \
        matrix[3][0] = ex; \
        ex = matrix[1][2]; \
        matrix[1][2] = matrix[2][1]; \
        matrix[2][1] = ex; \
        ex = matrix[1][3]; \
        matrix[1][3] = matrix[3][1]; \
        matrix[3][1] = ex; \
        ex = matrix[2][3]; \
        matrix[2][3] = matrix[3][2]; \
        matrix[3][2] = ex

/**
 * Key transpose operation, the original expandent key blocks need to be permutated.
 * So the AddRoundKey operation can work on an entire word not byte.
 * input:   kset
 * output:  kset
 * return:  void
 */
static void KeyTranspose(AESKeySet *kset)
{
  int       i, i4;
  uint8_t   ex;
  uint8_t   *ptr[4];

  for (i = 0; i <= kset->rounds; i++) {
    i4 = i << 4;
    ptr[0] = CastWToB(kset->key) + i4;
    ptr[1] = CastWToB(kset->key) + i4 + 4;
    ptr[2] = CastWToB(kset->key) + i4 + 8;
    ptr[3] = CastWToB(kset->key) + i4 + 12;
    MatrixPermutation(ptr, ex);
  }
}

#ifndef BIGEDIAN_MACHINE
#define RotWord(in) ((in >> 8) | (in << 24))
#else
#define RotWord(in) ((in << 8) | (in >> 24))
#endif

#define SubWord(in) \
        in[0] = SubByte(in[0], AES_S_Box); \
        in[1] = SubByte(in[1], AES_S_Box); \
        in[2] = SubByte(in[2], AES_S_Box); \
        in[3] = SubByte(in[3], AES_S_Box)

static void KeyExpansion(const void *userKey, size_t len, int rounds, AESKeySet *kset)
{
  int       i;
  uint32_t  temp;
  uint8_t   *ptr;

  memcpy(kset->key, userKey, len << 2);
  kset->rounds = rounds;
  for (i = len; i < ((rounds + 1) << 2); i++) {
    temp = kset->key[i - 1];
    ptr = (uint8_t *)(&temp);
    if (i % len == 0) {
      temp = RotWord(temp);
      SubWord(ptr);
      temp ^= AES_Rcon[i / len];
    } else if (len > 6 && (i % len == 4)) {
      SubWord(ptr);
    }
    kset->key[i] = kset->key[i - len] ^ temp;
  }
  KeyTranspose(kset);
}

/**
 * User key input
 * input:   userKey, bits
 * output:  kset
 * return:  Error codes
 */
int AESSetKey(const void *userKey, size_t bits, AESKeySet *kset)
{
  int rounds;

  if (userKey == NULL || kset == NULL)
    return AESNullPointer;

  switch(bits) {
  case AESKey128 :
    rounds = 10;
    break;
  case AESKey192 :
    rounds = 12;
    break;
  case AESKey256 :
    rounds = 14;
    break;
  default :
    return AESBadParam;
  }

  KeyExpansion(userKey, bits >> 5, rounds, kset);
  return AESSuccess;
}

/**
 * Encrpto function
 * input:   plain, kset
 * output:  cipher
 */
int AESEncrpto(const void *in, void *out, const AESKeySet *kset)
{
  const uint32_t  *blockIn, *kblock;
  uint32_t        *blockOut;
  
  kblock = kset->key;
  blockIn = (uint32_t *)in;
  blockOut = (uint32_t *)out;
  blockOut[0] = blockIn[0];
  blockOut[1] = blockIn[1];
  blockOut[2] = blockIn[2];
  blockOut[3] = blockIn[3];
  AddRoundKey(blockOut, kblock);

  for (int r = 1; r < kset->rounds; r++) {
    kblock += 4;
    SSubstitution(blockOut);
    ShiftRows(blockOut);
    MixColums(blockOut);
    AddRoundKey(blockOut, kblock);
  }

  kblock += 4;
  SSubstitution(blockOut);
  ShiftRows(blockOut);
  AddRoundKey(blockOut, kblock);

  return AESSuccess;
}

/**
 * Decrypto function
 * input:   cipher, kset
 * output:  plain
 * return:  Error codes
 */
int AESDecrpto(const void *in, void *out, const AESKeySet *kset)
{
  const uint32_t  *blockIn, *kblock;
  uint32_t        *blockOut;
  
  kblock = kset->key + (kset->rounds << 2);
  blockIn = (uint32_t *)in;
  blockOut = (uint32_t *)out;
  blockOut[0] = blockIn[0];
  blockOut[1] = blockIn[1];
  blockOut[2] = blockIn[2];
  blockOut[3] = blockIn[3];
  AddRoundKey(blockOut, kblock);
  InvShiftRows(blockOut);
  InvSSubstitution(blockOut);

  for (int r = kset->rounds - 1; r > 0; r--) {
    kblock = kset->key + (r << 2);
    AddRoundKey(blockOut, kblock);
    InvMixColums(blockOut);
    InvShiftRows(blockOut);
    InvSSubstitution(blockOut);
  }

  kblock = kset->key;
  AddRoundKey(blockOut, kblock);

  return AESSuccess;
}

/**
 * Polynomial multiplication.
 * in GF(2^8) this is the original multiplication without modulo ox011b
 * input:   a, b
 * output:  none
 * return:  product of a * b
 */
static uint16_t polynomialMult(uint8_t a, uint8_t b)
{
  uint16_t product;
  uint16_t aval, bval;
  uint16_t mid;

  aval = a;
  bval = b;
  if (aval < bval) {
    mid  = aval;
    aval = bval;
    bval = mid;
  }

  product = 0;
  while (bval) {
    //product ^= (bval & 0x01) * aval;
    product ^= (bval & 0x01) ? aval : 0;
    aval <<= 1;
    bval >>= 1;
  }

  return product;
}

/**
 * Polynomial division.
 * input:   a, b
 * output:  rem
 * return:  quotient of a / b
 */
static uint16_t polynomialDivision(uint16_t a, uint16_t b, uint16_t *rem)
{
  uint16_t quo;
  int q;
  int bitWidthOfA, bitWidthOfB;

  quo = 0;
  bitWidthOfA = (a >> 8) == 0 ? octedBitWidth[a] : octedBitWidth[a >> 8] + 8;
  bitWidthOfB = (b >> 8) == 0 ? octedBitWidth[b] : octedBitWidth[b >> 8] + 8;
  q = bitWidthOfA - bitWidthOfB;
  while (q >= 0) {
    quo ^= (0x01 << q);
    a ^= (b << q);
    bitWidthOfA = (a >> 8) == 0 ? octedBitWidth[a] : octedBitWidth[a >> 8] + 8;
    q = bitWidthOfA - bitWidthOfB;
  }
  *rem = a;
  return quo;
}

/**
 * Extended Euclidean algorithm to resolve equition: ax = 1 mod b
 * note, require a and b are relatively prime, st, gcd(a,b) == 1
 * input:   a, b
 * output:  none
 * return:  solution x which is the multiplicative inverse of a
 * special value 0 returned if a or b equals 0, or gcd(a,b) != 1
 */
static uint16_t RESEEA(uint16_t a, uint16_t b)
{
  uint16_t rem, quo;
  uint16_t s, x, v, orgb;

  orgb = b;
  x = 0x01;
  v = 0;

  while (b != 0) {
    quo = polynomialDivision(a, b, &rem);

    s = x ^ polynomialMult(quo, v);
    x = v;
    v = s;

    a = b;
    b = rem;
  }
  if (a != 1)
    return 0;
  
  polynomialDivision(x, orgb, &rem);
  return rem;
}

/**
 * S_Box and S_INV_Box map generation rule matrix
 */
static const uint8_t S_Matrix[8] = {0x8f, 0xc7, 0xe3, 0xf1, 0xf8, 0x7c, 0x3e, 0x1f};
static const uint8_t S_INV_Matrix[8] = {0x52, 0x29, 0x94, 0x4a, 0x25, 0x92, 0x49, 0xa4};

/* Reverse bit sequence operation of an octed x */
#define ROTOCTED(x) \
        ((x & 0x80) >> 7) | ((x & 0x40) >> 5) | ((x & 0x20) >> 3) | ((x & 0x10) >> 1) | ((x & 0x08) << 1) | \
        ((x & 0x04) << 3) | ((x & 0x02) << 5) | ((x & 0x01) << 7)

/**
 * S_Box generation function
 * input:   in
 * output:  none
 * return:  mapped value of in
 */
uint8_t SMatrixMap(uint8_t in)
{
  uint8_t tmpOcted[8];
  uint8_t ret;

  in = RESEEA(in, 0x011b);
  in = ROTOCTED(in);
  tmpOcted[0] = octedBitCount[S_Matrix[0] & in] + 1;
  tmpOcted[1] = octedBitCount[S_Matrix[1] & in] + 1;
  tmpOcted[2] = octedBitCount[S_Matrix[2] & in] + 0;
  tmpOcted[3] = octedBitCount[S_Matrix[3] & in] + 0;
  tmpOcted[4] = octedBitCount[S_Matrix[4] & in] + 0;
  tmpOcted[5] = octedBitCount[S_Matrix[5] & in] + 1;
  tmpOcted[6] = octedBitCount[S_Matrix[6] & in] + 1;
  tmpOcted[7] = octedBitCount[S_Matrix[7] & in] + 0;

  ret = 0;
  for (int i = 0; i < sizeof(tmpOcted); i++) {
    tmpOcted[i] = tmpOcted[i] & 0x01;
    if (tmpOcted[i])
      ret |= (tmpOcted[i] << i);
  }

  return ret;
}

/**
 * S_INV_Box generation function
 * input:   in
 * output:  none
 * return:  mapped value of in
 */
uint8_t SInvMatrixMap(uint8_t in)
{
  uint8_t tmpOcted[8];
  uint8_t ret;

  tmpOcted[0] = octedBitCount[S_INV_Matrix[0] & in] + 0;
  tmpOcted[1] = octedBitCount[S_INV_Matrix[1] & in] + 0;
  tmpOcted[2] = octedBitCount[S_INV_Matrix[2] & in] + 0;
  tmpOcted[3] = octedBitCount[S_INV_Matrix[3] & in] + 0;
  tmpOcted[4] = octedBitCount[S_INV_Matrix[4] & in] + 0;
  tmpOcted[5] = octedBitCount[S_INV_Matrix[5] & in] + 1;
  tmpOcted[6] = octedBitCount[S_INV_Matrix[6] & in] + 0;
  tmpOcted[7] = octedBitCount[S_INV_Matrix[7] & in] + 1;

  ret = 0;
  for (int i = 0; i < sizeof(tmpOcted); i++) {
    tmpOcted[i] = tmpOcted[i] & 0x01;
    if (tmpOcted[i])
      ret |= (tmpOcted[i] << i);
  }
  ret = ROTOCTED(ret);
  ret = RESEEA(ret, 0x011b);

  return ret;
}