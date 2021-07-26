/**
 * Block cipher mode of operation
 * Reference : https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Padding
 */
#include "aes.h"

/* Alias of AES Block, a 4 by 4 bytes arrary */
typedef uint32_t AES_BLOCK[4];

/**
 * AES ECB mode encryption, math formula(index start with 1):
 * Ci = Ek(Pi), Pi = Dk(Ci)
 * input:   plainc, pkgCnt, kset
 * output:  cipherc
 * return:  Error code
 */
int AESECBEncrypto(const void *in, void *out, size_t pkgCnt, AESKeySet *kset)
{
  if ((in == NULL) || (out == NULL) || (kset == NULL))
    return AESNullPointer;
  if (pkgCnt & 0x0f)
    return AESBadParam;

  for (size_t i = 0; i < pkgCnt; i++) {
    AESEncrpto(in, out, kset);
    in  = (uint8_t *)in + 16;
    out = (uint8_t *)out + 16;
  }
  return AESSuccess;
}

/* AES ECB mode decryption */
int AESECBDecrypto(const void *in, void *out, size_t pkgCnt, AESKeySet *kset)
{
  if ((in == NULL) || (out == NULL) || (kset == NULL))
    return AESNullPointer;
  if ((pkgCnt & 0x0f) != 0)
    return AESBadParam;

  for (size_t i = 0; i < pkgCnt; i++) {
    AESDecrpto(in, out, kset);
    in  = (uint8_t *)in + 16;
    out = (uint8_t *)out + 16;
  }
  return AESSuccess;
}

#define AddRound(blocka, blockb) \
        blocka[0] ^= blockb[0]; \
        blocka[1] ^= blockb[1]; \
        blocka[2] ^= blockb[2]; \
        blocka[3] ^= blockb[3]
/**
 * AES CBC mode encryption, math formula(index start with 1):
 * Ci = Ek(Pi ^ Ci-1), C0 = IV; Pi = Dk(Ci) ^ Ci-1, C0 = IV
 * input:   IV, plainc, pkgCnt, kset
 * output:  cipherc
 * return:  Error code
 */
int AESCBCEncrypto(const uint8_t IV[16], const void *in, void *out, size_t pkgCnt, AESKeySet *kset)
{
  AES_BLOCK         Obj;
  const uint32_t    *ptrVector, *ptrin;
  if ((IV == NULL) || (in == NULL) || (out == NULL) || (kset == NULL))
    return AESNullPointer;
  if (pkgCnt & 0x0f)
    return AESBadParam;
  
  ptrVector = (uint32_t *)IV;
  for (size_t i = 0; i < pkgCnt; i++) {
    ptrin = (uint32_t *)in;
    Obj[0] = ptrin[0];
    Obj[1] = ptrin[1];
    Obj[2] = ptrin[2];
    Obj[3] = ptrin[3];
    AddRound(Obj, ptrVector);
    AESEncrpto(Obj, out, kset);

    ptrVector = (uint32_t *)out;
    in  = (uint8_t *)in + 16;
    out = (uint8_t *)out + 16;
  }
  return AESSuccess;
}

/* AES CBC decryption */
int AESCBCDecrypto(const uint8_t IV[16], const void *in, void *out, size_t pkgCnt, AESKeySet *kset)
{
  const uint32_t  *ptrVector;
  uint32_t        *ptrout;
  if ((IV == NULL) || (in == NULL) || (out == NULL) || (kset == NULL))
    return AESNullPointer;
  if (pkgCnt & 0x0f)
    return AESBadParam;

  ptrVector = (uint32_t *)IV;
  for (size_t i = 0; i < pkgCnt; i++) {
    ptrout = (uint32_t *)out;
    AESDecrpto(in, out, kset);
    AddRound(ptrout, ptrVector);

    ptrVector = (uint32_t *)in;
    in  = (uint8_t *)in + 16;
    out = (uint8_t *)out + 16;
  }
  return AESSuccess;
}

/**
 * AES PCBC encryption, math formula(index start with 1):
 * Ci = Ek(Pi ^ Pi-1 ^ Ci-1), P0 ^ C0 = IV; Pi = Dk(Ci) ^ Pi-1 ^ Ci-1, P0 ^ C0 = IV
 * 
 */
int AESPCBCEncrypto(const uint8_t IV[16], const void *in, void *out, size_t pkgCnt, AESKeySet *kset)
{
  AES_BLOCK       incur, inbak;
  AES_BLOCK       zero = {0};
  const uint32_t  *ptrin, *ptrVector;
  if ((IV == NULL) || (in == NULL) || (out == NULL) || (kset == NULL))
    return AESNullPointer;
  if (pkgCnt & 0x0f0)
    return AESBadParam;
  
  ptrVector = (uint32_t *)IV;
  for (size_t i = 0; i < pkgCnt; i++) {
    ptrin = (uint32_t *)in;
    incur[0] = ptrin[0];
    incur[1] = ptrin[1];
    incur[2] = ptrin[2];
    incur[3] = ptrin[3];
    AddRound(incur, (i ? inbak : zero));
    AddRound(incur, ptrVector);
    inbak[0] = ptrin[0];
    inbak[1] = ptrin[1];
    inbak[2] = ptrin[2];
    inbak[3] = ptrin[3];
    AESEncrpto(incur, out, kset);

    ptrVector = (uint32_t *)out;
    in  = (uint8_t *)in + 16;
    out = (uint8_t *)out + 16;
  }
  return AESSuccess;
}

/* AES PCBC decryption */
int AESPCBCDecrypto(const uint8_t IV[16], const void *in, void *out, size_t pkgCnt, AESKeySet *kset)
{
  AES_BLOCK       incur, inbak;
  AES_BLOCK       zero = {0};
  const uint32_t  *ptrin, *ptrVector;
  uint32_t        *ptrout;
  if ((IV == NULL) || (in == NULL) || (out == NULL) || (kset == NULL))
    return AESNullPointer;
  if (pkgCnt & 0x0f)
    return AESBadParam;

  ptrVector = (uint32_t *)IV;
  for (size_t i = 0; i < pkgCnt; i++) {
    ptrin  = (uint32_t *)in;
    ptrout = (uint32_t *)out;
    incur[0] = ptrin[0];
    incur[1] = ptrin[1];
    incur[2] = ptrin[2];
    incur[3] = ptrin[3];
    AESDecrpto(in, out, kset);
    AddRound(ptrout, ptrVector);
    AddRound(ptrout, (i ? inbak : zero));
    inbak[0] = incur[0];
    inbak[1] = incur[1];
    inbak[2] = incur[2];
    inbak[3] = incur[3];

    ptrVector = (uint32_t *)out;
    in  = (uint8_t *)in + 16;
    out = (uint8_t *)out + 16;
  }
  return AESSuccess;
}

static AES_BLOCK ROTAESBLOCKex; 
#define WORDROTAESBLOCK(blocka, wdLen)  \
        ROTAESBLOCKex[0] = blocka[0];   \
        ROTAESBLOCKex[1] = blocka[1];   \
        ROTAESBLOCKex[2] = blocka[2];   \
        ROTAESBLOCKex[3] = blocka[3];   \
        blocka[0] = blocka[(wdLen < 4) ? wdLen : 0];                \
        blocka[1] = blocka[(wdLen < 3) ? wdLen + 1 : 1];            \
        blocka[2] = blocka[(wdLen < 2) ? wdLen + 2 : 2];            \
        blocka[3] = ROTAESBLOCKex[wdLen > 0 ? wdLen - 1 : 3];       \
        blocka[2] = ROTAESBLOCKex[wdLen > 1 ? wdLen - 2 : 2];       \
        blocka[1] = ROTAESBLOCKex[wdLen > 2 ? wdLen - 3 : 1]

static uint8_t ROTAESBLOCKbak[16];
#define BYTEROTAESBLOCK(arry, byteLen)      \
        memcpy(ROTAESBLOCKbak, arry, sizeof(ROTAESBLOCKbak));       \
        for (int i = 0; i < 16; i++) {                              \
          arry[i] = arry[(byteLen < 16 - i) ? byteLen + i : i];     \
          arry[15 - i] = ROTAESBLOCKbak[byteLen > i ? byteLen - i - 1 : 15 - i];   \
        }


#ifndef BIGEDIAN_MACHINE
static uint32_t ROTAESWDMASK[] = {
  0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
  0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff, 0x000001ff, 0x000003ff, 0x00007fff,
  0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
  0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff
};
#define BITROTAESBLOCK(blocka, bitLen)                          \
        ROTAESBLOCKex[3] = blocka[3] & ROTAESWDMASK[bitLen];    \
        blocka[3] >>= bitLen;                                   \
        ROTAESBLOCKex[2] = blocka[2] & ROTAESWDMASK[bitLen];    \
        blocka[2] >>= bitLen;                                   \
        blocka[2] |= (ROTAESBLOCKex[3] << (32 - bitLen));       \
        ROTAESBLOCKex[1] = blocka[1] & ROTAESWDMASK[bitLen];    \
        blocka[1] >>= bitLen;                                   \
        blocka[1] |= (ROTAESBLOCKex[2] << (32 - bitLen));       \
        ROTAESBLOCKex[0] = blocka[0] & ROTAESWDMASK[bitLen];    \
        blocka[0] >>= bitLen;                                   \
        blocka[0] |= (ROTAESBLOCKex[1] << (32 - bitLen));       \
        blocka[3] |= (ROTAESBLOCKex[0] << (32 - bitLen))
#else
#define BITROTAESBLOCK(blocka)  \
        BITROTAESBLOCKtag3 = blocka[3] & 0x80000000;      \
        blocka[3] <<= 1;                                  \
        BITROTAESBLOCKtag2 = blocka[2] & 0x80000000;      \
        blocka[2] <<= 1;                                   \
        blocka[2] += BITROTAESBLOCKtag3;                  \
        BITROTAESBLOCKtag1 = blocka[1] & 0x80000000;      \
        blocka[1] <<= 1;                                  \
        blocka[1] += BITROTAESBLOCKtag2;                  \
        BITROTAESBLOCKtag0 = blocka[0] & 0x80000000;      \
        blocka[0] <<= 1;                                  \
        blocka[0] += BITROTAESBLOCKtag1;                  \
        blocka[3] += BITROTAESBLOCKtag0
#endif

/**
 * AES CFB encryption, math formula(index start with 1):
 * Ci = IV (when i = 0), Ci = Ek(Ci-1) ^ Pi (otherwise); Pi = Ek(Ci-1) ^ Ci
 * Note, only encryption function involved, decryption function not.
 */
static int AESCFBEncrypto(const uint8_t *IV, const void *in, void *out, size_t len, AESKeySet *kset, int tag)
{
  size_t          segSize, segCnt, segRem;
  AES_BLOCK       Obj, SegXOR;
  uint8_t         *outptr, *Objptr, *ptrSeg;
  const uint8_t   *inptr;
  if ((IV == NULL) || (in == NULL) || (out == NULL) || (kset == NULL))
    return AESNullPointer;
  
  switch (tag) {
  case AESCFB_1 :
    segSize = 1;
    break;
  case AESCFB_8 :
    segSize = 8;
    break;
  case AESCFB_64 :
    segSize = 64;
    break;
  case AESCFB_128 :
    segSize = 128;
    break;
  default :
    return AESBadParam;
  }
  segCnt = (len << 3) / segSize;
  segRem = (len << 3) % segSize;

  memcpy(Obj, IV, 16);
  SegXOR[0] = 0;
  SegXOR[1] = 0;
  SegXOR[2] = 0;
  SegXOR[3] = 0;
  ptrSeg = (uint8_t *)SegXOR;
  inptr = in;
  outptr = out;
  Objptr = (uint8_t *)Obj;

  for (size_t i = 0, j = 0; i < segCnt; i++) {
    AESEncrpto(Obj, Obj, kset);
    switch (segSize) {
    case 1 :
      switch (i % 8) {
      case 0 :
        ptrSeg[0] = inptr[j] & 0x80;
        break;
      case 1 :
        ptrSeg[0] = (inptr[j] << 1) & 0x80;
        break;
      case 2 :
        ptrSeg[0] = (inptr[j] << 2) & 0x80;
        break;
      case 3 :
        ptrSeg[0] = (inptr[j] << 3) & 0x80;
        break;
      case 4 :
        ptrSeg[0] = (inptr[j] << 4) & 0x80;
        break;
      case 5 :
        ptrSeg[0] = (inptr[j] << 5) & 0x80;
        break;
      case 6 :
        ptrSeg[0] = (inptr[j] << 6) & 0x80;
        break;
      case 7 :
        ptrSeg[0] = (inptr[j] << 7) & 0x80;
        //j++;
        break;
      default :
        return AESStateError;
      }
      Obj[0] ^= SegXOR[0];
      switch (i % 8) {
      case 0 :
        outptr[j] = Objptr[0] & 0x80;
        break;
      case 1 :
        outptr[j] |= ((Objptr[0] >> 1) & 0x40);
        break;
      case 2 :
        outptr[j] |= ((Objptr[0] >> 2) & 0x20);
        break;
      case 3 :
        outptr[j] |= ((Objptr[0] >> 3) & 0x10);
        break;
      case 4 :
        outptr[j] |= ((Objptr[0] >> 4) & 0x08);
        break;
      case 5 :
        outptr[j] |= ((Objptr[0] >> 5) & 0x04);
        break;
      case 6 :
        outptr[j] |= ((Objptr[0] >> 6) & 0x02);
        break;
      case 7 :
        outptr[j] |= ((Objptr[0] >> 7) & 0x01);
        j++;
        break;
      default :
        return AESStateError;
      }
      
      BITROTAESBLOCK(Obj, 1);
      break;
    case 8 :
      ptrSeg[0] = inptr[i];
      Obj[0] ^= SegXOR[0];
      outptr[i] = Objptr[0];
      BYTEROTAESBLOCK(Objptr, 1);
      break;
    case 64 :
      memcpy(ptrSeg, inptr, 8);
      Obj[0] ^= SegXOR[0];
      Obj[1] ^= SegXOR[1];
      memcpy(outptr, Objptr, 8);
      inptr += 8;
      outptr += 8;
      WORDROTAESBLOCK(Obj, 2);
      break;
    case 128 :
      AddRound(Obj, ((uint32_t *)in));
      memcpy(outptr, Objptr, 16);
      inptr += 16;
      outptr += 16;
      break;
    default :
      return AESBadParam;
    }
  }

  AESEncrpto(Obj, Obj, kset);
  if ((segSize == 64) || (segSize == 128)) {
    for (size_t n = 0; n < segRem; n++) {
      outptr[n] = inptr[n] ^ Objptr[n];
    }
  }

  return AESSuccess;
}

/* AES CFB decryption */
static int AESCFBDecrypto(const uint8_t *IV, const void *in, void *out, size_t len, AESKeySet *kset, int tag)
{
  size_t      segSize, segCnt, segRem;
  AES_BLOCK   Obj, SegXOR;
  uint8_t     *outptr, *Objptr, *ptrSeg;
  const uint8_t *inptr;
  uint32_t    *outBlock, *inBlock;
  if ((IV == NULL) || (in == NULL) || (out == NULL) || (kset == NULL))
    return AESNullPointer;
  
  switch (tag) {
  case AESCFB_1 :
    segSize = 1;
    break;
  case AESCFB_8 :
    segSize = 8;
    break;
  case AESCFB_64 :
    segSize = 64;
    break;
  case AESCFB_128 :
    segSize = 128;
    break;
  default :
    return AESBadParam;
  }
  segCnt = (len << 3) / segSize;
  segRem = (len << 3) % segSize;

  memcpy(Obj, IV, 16);
  SegXOR[0] = 0;
  SegXOR[1] = 0;
  SegXOR[2] = 0;
  SegXOR[3] = 0;
  ptrSeg = (uint8_t *)SegXOR;
  inptr = in;
  outptr = out;
  Objptr = (uint8_t *)Obj;

  for (size_t i = 0, j = 0; i < segCnt; i++) {
    AESEncrpto(Obj, Obj, kset);
    switch (segSize) {
    case 1 :
      switch (i % 8) {
      case 0 :
        ptrSeg[0] = inptr[j] & 0x80;
        break;
      case 1 :
        ptrSeg[0] = (inptr[j] << 1) & 0x80;
        break;
      case 2 :
        ptrSeg[0] = (inptr[j] << 2) & 0x80;
        break;
      case 3 :
        ptrSeg[0] = (inptr[j] << 3) & 0x80;
        break;
      case 4 :
        ptrSeg[0] = (inptr[j] << 4) & 0x80;
        break;
      case 5 :
        ptrSeg[0] = (inptr[j] << 5) & 0x80;
        break;
      case 6 :
        ptrSeg[0] = (inptr[j] << 6) & 0x80;
        break;
      case 7 :
        ptrSeg[0] = (inptr[j] << 7) & 0x80;
        //j++;
        break;
      default :
        return AESStateError;
      }
      switch (i % 8) {
      case 0 :
        outptr[j] = (inptr[j] ^ Objptr[0]) & 0x80;
        break;
      case 1 :
        outptr[j] |= (inptr[j] ^ (Objptr[0] >> 1)) & 0x40;
        break;
      case 2 :
        outptr[j] |= (inptr[j] ^ (Objptr[0] >> 2)) & 0x20;
        break;
      case 3 :
        outptr[j] |= (inptr[j] ^ (Objptr[0] >> 3)) & 0x10;
        break;
      case 4 :
        outptr[j] |= (inptr[j] ^ (Objptr[0] >> 4)) & 0x08;
        break;
      case 5 :
        outptr[j] |= (inptr[j] ^ (Objptr[0] >> 5)) & 0x04;
        break;
      case 6 :
        outptr[j] |= (inptr[j] ^ (Objptr[0] >> 6)) & 0x02;
        break;
      case 7 :
        outptr[j] |= (inptr[j] ^ (Objptr[0] >> 7)) & 0x01;
        j++;
        break;
      default :
        return AESStateError;
      }
      if (ptrSeg[0])
        Objptr[0] |= 0x80;
      else
        Objptr[0] &= 0x7f;
      BITROTAESBLOCK(Obj, 1);
      break;
    case 8 :
      ptrSeg[0] = inptr[i];
      outptr[i] = inptr[i] ^ Objptr[0];
      Objptr[0] = ptrSeg[0];
      BYTEROTAESBLOCK(Objptr, 1);
      break;
    case 64 :
      outBlock = (uint32_t *)outptr;
      inBlock  = (uint32_t *)inptr;
      memcpy(ptrSeg, inptr, 8);
      outBlock[0] = inptr[0] ^ Obj[0];
      outBlock[1] = inptr[1] ^ Obj[1];
      memcpy(Obj, ptrSeg, 8);
      WORDROTAESBLOCK(Obj, 2);
      inptr += 8;
      outptr += 8;
      break;
    case 128 :
      outBlock = (uint32_t *)outptr;
      inBlock  = (uint32_t *)inptr;
      memcpy(ptrSeg, inptr, 16);
      outBlock[0] = inptr[0] ^ Obj[0];
      outBlock[1] = inptr[1] ^ Obj[1];
      outBlock[2] = inptr[2] ^ Obj[2];
      outBlock[3] = inptr[3] ^ Obj[3];
      memcpy(Obj, ptrSeg, 16);
      inptr += 16;
      outptr += 16;
      break;
    default :
      return AESBadParam;
    }
  }

  AESEncrpto(Obj, Obj, kset);
  if ((segSize == 64) || (segSize == 128)) {
    for (size_t n = 0; n < segRem; n++) {
      outptr[n] = inptr[n] ^ Objptr[n];
    }
  }

  return AESSuccess;
}

/**
 * AES OFB encryption, math formula(index start with 1):
 * Cj = Pj ^ Oj, Pj = Cj ^ Oj, Oj = Ek(Ij), Ij = Oj-1, I0 = IV
 * Note, only encryption function involved, decryption function not.
 */
int AESOFBEncrypto(const uint8_t *IV, const void *in, void *out, size_t len, AESKeySet *kset)
{
  size_t            segCnt, segRem;
  AES_BLOCK         Obj;
  const uint32_t    *ptrin;
  uint32_t          *ptrout;
  const uint8_t     *inc;
  uint8_t           *outc, ptrObj;
  if ((IV == NULL) || (in == NULL) || (out == NULL) || (kset == NULL))
    return AESNullPointer;
  
  segCnt = (len >> 4);
  segRem = (len & 0x0f);
  memcpy(Obj, IV, 16);
  for (size_t i = 0; i < segCnt; i++) {
    ptrin = in;
    ptrout = out;
    AESEncrpto(Obj, Obj, kset);
    ptrout[0] = ptrin[0] ^ Obj[0];
    ptrout[1] = ptrin[1] ^ Obj[1];
    ptrout[2] = ptrin[2] ^ Obj[2];
    ptrout[3] = ptrin[3] ^ Obj[3];
    in = (uint8_t *)in + 16;
    out = (uint8_t *)out + 16;
  }

  AESEncrpto(Obj, Obj, kset);
  ptrObj = (uint8_t *)Obj;
  inc  = in;
  outc = out;
  for (size_t n = 0; n < segRem; n++) {
    outc[n] = inc[n] ^ ptrObj[n];
  }
  return AESSuccess;
}

/* AES OFB decryption */
int AESOFBDecrypto(const uint8_t *IV, const void *in, void *out, size_t len, AESKeySet *kset)
{
  AESOFBEncrypto(IV, in, out, len, kset);
}

#define INCCTR(obj, len)  \
        switch (len) {    \
        case 1 :          \
          obj[0]++;       \
          break;          \
        case 2 :          \
          obj[1] += (++obj[0] == 0 ? 1: 0);   \
          break;          \
        case 3 :          \
          obj[2] += ((obj[1] += (++obj[0] == 0 ? 1 : 0)) == 0 ? 1 : 0); \
          break;          \
        default :         \
          return AESBadParam;                 \
        }         

/**
 * AES CTR Encryption, a variants of OFB
 * IV splited to nonce combine counter part.
 * The increasing vector not the previous Ek output, 
 * is the only input of each Ek operation
 * So it's faster then OFB, ont safer
 */
int AESCTREncrypto(const uint32_t *Nonce, size_t nonceLen, const AES_BLOCK *plainc, AES_BLOCK *cipherc, size_t pkgCnt, AESKeySet *kset)
{
  AES_BLOCK     Obj;
  if ((Nonce == NULL) || (plainc == NULL) || (cipherc == NULL) || (kset == NULL))
    return AESNullPointer;
  
  for (size_t i = 0; i < pkgCnt; i++) {
    AESEncrpto(Obj, cipherc[i], kset);
    AddRound(cipherc[i], plainc[i]);
    INCCTR((Obj + nonceLen), sizeof(AES_BLOCK) - nonceLen);
  }
  return AESSuccess;
}

int AESCTRDecrypto(const uint32_t *Nonce, size_t nonceLen, const AES_BLOCK *cipherc, AES_BLOCK *plainc, size_t pkgCnt, AESKeySet *kset)
{
  AES_BLOCK     Obj;
  if ((Nonce == NULL) || (plainc == NULL) || (cipherc == NULL) || (kset == NULL))
    return AESNullPointer;
  
  for (size_t i = 0; i < pkgCnt; i++) {
    AESEncrpto(Obj, plainc[i], kset);
    AddRound(plainc[i], cipherc[i]);
    INCCTR((Obj + nonceLen), sizeof(AES_BLOCK) - nonceLen);
  }
  return AESSuccess;
}