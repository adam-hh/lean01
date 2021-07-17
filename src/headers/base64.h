/**
 * Base64 encoding/decoding tools personal implementation
 * (Referenced from RFC4648)
 * Copyright (c) 2020-2021, Adam hh <adamhh.wk@gmail.com>
 * Standard WorkFlow:
 * 1. Init
 * 2. Encode/Decode
 * 3. Reset
 */
#ifndef BASH64_H
#define BASH64_H


#ifdef __cpluscplus
extern "C" {
#endif

#ifndef _B64_enum_
#define _B64_enum_
/**
 * Error codes of b64 functions
 */
enum {
  b64Success = 0,
  b64NullPointer,
  b64MemAllocError,
  b64StateError,
  b64BadParam
};

/**
 * Opitions of when encoding an obj
 */
enum B64Pad {
  B64PAD,
  B64UNPAD
};
#endif  /* _B64_enum_ */

#ifndef size_t
typedef unsigned long size_t;
#endif

/**
 * Holder of encoded obj(char arrary)
 */
typedef struct B64Encoder {
  char          *arry;  /* start address */
  size_t        len;    /* offset */
  enum B64Pad   pad;    /* PAD tag */
} B64Encoder;

/**
 * Holder of decoded obj(void* arrary)
 */
typedef struct B64Decoder {
  void          *arry;  /* start address */
  size_t        len;    /* offset */
} B64Decoder;

/**
 * Reference RFC 4648 $4: base64(standard)
 * https://datatracker.ietf.org/doc/html/rfc4648#section-4
 */
static const char b64StandardEncTable[64] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};
static const char b64StandardEncPadChar = '=';
static const int b64StandardDecTable[256] = {
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, 62, -1, -1, -1, 63,
  52, 53, 54, 55, 56, 57, 58, 59,
  60, 61, -1, -1, -1, -1, -1, -1,
  -1,  0,  1,  2,  3,  4,  5,  6,
   7,  8,  9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, -1, -1, -1, -1, -1,
  -1, 26, 27, 28, 29, 30, 31, 32,
  33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48,
  49, 50, 51, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1
};

/**
 * Reference RFC 4648 $5: base64url(URL-and filename-safe standard)
 * https://datatracker.ietf.org/doc/html/rfc4648#section-5
 */
static const char b64URLEncTable[64] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '-', '_'
};
static const char b64URLEncPadChar = '=';
static const int b64URLDecTable[256] = {
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, 62, -1, -1,
  52, 53, 54, 55, 56, 57, 58, 59,
  60, 61, -1, -1, -1, -1, -1, -1,
  -1,  0,  1,  2,  3,  4,  5,  6,
   7,  8,  9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, -1, -1, -1, -1, 63,
  -1, 26, 27, 28, 29, 30, 31, 32,
  33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48,
  49, 50, 51, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
};

extern int B64EncInit(B64Encoder *enc);
extern int B64EncReset(B64Encoder *enc);
extern int B64StandardEncode(B64Encoder *enc, void *in, size_t len);
extern int B64URLEncode(B64Encoder *enc, void *in, size_t len);

extern int B64DecInit(B64Decoder *dec);
extern int B64DecReset(B64Decoder *dec);
extern int B64StandardDecode(B64Encoder *enc, B64Decoder *dec);
extern int B64URLDecode(B64Encoder *enc, B64Decoder *dec);

extern int B64StandardEncodeFile(B64Encoder *enc, const char *filein, const char *fileout);
extern int B64URLEncodeFile(B64Encoder *enc, const char *filein, const char *fileout);

extern int B64StandardDecodeFile(B64Decoder *dec, const char *filein, const char *fileout);
extern int B64URLDecodeFile(B64Decoder *dec, const char *filein, const char *fileout);

#ifdef __cpluscplus
}
#endif

#endif          /* BASH64_H */