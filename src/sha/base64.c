#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "base64.h"

/* Max file size(byte) to read, it's 10 kb here */
#define MAXFILESIZE 10 << 10

#ifndef FILE_MODE
#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) 
#endif

/* prototypes */
static int B64Encode(B64Encoder *enc, void *in, size_t len, enum B64Pad pad, const char *encTable, char encPad);
static int B64Decode(B64Encoder *enc, B64Decoder *dec, const int *decTable, char encPad);
static int B64EncodeFile(B64Encoder *enc, const char *filein, const char *fileout, enum B64Pad pad, const char *encTable, char encPad);
static int B64DecodeFile(B64Decoder *dec, const char *filein, const char *fileout, const int *decTable, char encPad);

/**
 * Init enc
 * Standard work flow(3 steps):
 * 1. Init
 * 2. Encode/Decode
 * 3. Reset
 */
int B64EncInit(B64Encoder *enc)
{
  if (enc == NULL)
    return b64NullPointer;
  enc->arry = NULL;
  enc->len = 0;
  return b64Success;
}

/**
 * Init dec
 */
int B64DecInit(B64Decoder *dec)
{
  if (dec == NULL)
    return b64NullPointer;
  dec->arry = NULL;
  dec->len = 0;
  return b64Success;
}

/**
 * Reset enc and release mem
 * So it's reusable
 * input:   enc
 * output:  enc
 * return:  Error code
 */
int B64EncReset(B64Encoder *enc)
{
  if (enc == NULL)
    return b64NullPointer;
  if (enc->len) {
    free(enc->arry);
    enc->arry = NULL;
  }
  enc->len = 0;
  return b64Success;
}

/**
 * Reset dec and release mem
 * input:   dec
 * output:  dec
 * return:  Error code
 */
int B64DecReset(B64Decoder *dec)
{
  if (dec == NULL)
    return b64NullPointer;
  if (dec->len) {
    free(dec->arry);
    dec->arry = NULL;
  }
  dec->len = 0;
  return b64Success;
}

/**
 * base64(standard) encode
 * input:   in, len
 * output:  enc
 * return:  Error code
 */
int B64StandardEncode(B64Encoder *enc, void *in, size_t len)
{
  return B64Encode(enc, in, len, B64PAD, b64StandardEncTable, b64StandardEncPadChar);
}

/**
 * base64url(URL-and filename-safe standard) encode
 * input:   in, len
 * output:  enc
 * return:  Error code
 */
int B64URLEncode(B64Encoder *enc, void *in, size_t len)
{
  return B64Encode(enc, in, len, B64PAD, b64URLEncTable, b64URLEncPadChar);
}

/**
 * base64(standard) decode
 * input:   enc
 * output:  dec
 * return:  Error code
 */
int B64StandardDecode(B64Encoder *enc, B64Decoder *dec)
{
  return B64Decode(enc, dec, b64StandardDecTable, b64StandardEncPadChar);
}

/**
 * base64url(URL-and filename-safe standard) decode
 * input:   enc
 * output:  dec
 * return:  Error Code
 */
int B64URLDecode(B64Encoder *enc, B64Decoder *dec)
{
  return B64Decode(enc, dec, b64URLDecTable, b64URLEncPadChar);
}

/**
 * base64(standard) encode(source from a file)
 * input:   filein
 * output:  enc, fileout (one of them can be NULL, not both NULL)
 * return:  Error code
 */
int B64StandardEncodeFile(B64Encoder *enc, const char *filein, const char *fileout)
{
  return B64EncodeFile(enc, filein, fileout, B64PAD, b64StandardEncTable, b64StandardEncPadChar);
}

/**
 * base64url(URL-and filename-safe standard) encode(source from a file)
 * input:   filein
 * output:  enc, fileout (one of them can be NULL, not both NULL)
 * return:  Error code
 */
int B64URLEncodeFile(B64Encoder *enc, const char *filein, const char *fileout)
{
  return B64EncodeFile(enc, filein, fileout, B64PAD, b64URLEncTable, b64URLEncPadChar);
}

/**
 * base64(standard) decode(source from a file)
 * input:   filein
 * output:  dec, fileout (one of them can be NULL, not both NULL)
 * return:  Error code
 */
int B64StandardDecodeFile(B64Decoder *dec, const char *filein, const char *fileout)
{
  return B64DecodeFile(dec, filein, fileout, b64StandardDecTable, b64StandardEncPadChar);
}

/**
 * base64url(URL-and filename-safe standard) decode(source from a file)
 * input:   filein
 * output:  dec, fileout (one of them can be NULL, not both NULL)
 * return:  Error code
 */
int B64URLDecodeFile(B64Decoder *dec, const char *filein, const char *fileout)
{
  return B64DecodeFile(dec, filein, fileout, b64URLDecTable, b64URLEncPadChar);
}

static int B64Encode(B64Encoder *enc, void *in, size_t len, enum B64Pad pad, const char *encTable, char encPad)
{
  size_t    olen, rem;
  size_t    i, j;
  unsigned  v;
  uint8_t   *ptrin;

  if ((in == NULL) || (len == 0) || (enc == NULL))
    return b64NullPointer;

  B64EncReset(enc);
  enc->pad = pad;

  olen = (len << 3) / 6;
  rem = (len << 3) % 6;
  if (rem) {
    olen++;
    if (enc->pad == B64PAD) {
      rem = olen & 3;
      if (rem)
        olen += (4 - rem);
    }
  }

  enc->len = olen;
  enc->arry = (char *)malloc(olen + 1);
  if (enc->arry == NULL)
    return b64MemAllocError;
  enc->arry[olen] = '\0';

  ptrin = (uint8_t *)in;
  for (i = 0, j = 0; i < len; i += 3, j += 4) {
    v = ptrin[i];
    v = i + 1 < len ? (v << 8) | ptrin[i + 1] : v << 8;
    v = i + 2 < len ? (v << 8) | ptrin[i + 2] : v << 8;

    enc->arry[j] = encTable[(v >> 18) & 0x3f];
    enc->arry[j + 1] = encTable[(v >> 12) & 0x3f];
    if (i + 1 < len) {
      enc->arry[j + 2] = encTable[(v >> 6) & 0x3f];
    } else if (enc->pad == B64PAD) {
      enc->arry[j + 2] = encPad;
    } else {
      return b64Success;
    }
    if (i + 2 < len) {
      enc->arry[j + 3] = encTable[v & 0x3f];
    } else if (enc->pad == B64PAD) {
      enc->arry[j + 3] = encPad;
    } else {
      return b64Success;
    }
  }

  return b64Success;
}

static int B64Decode(B64Encoder *enc, B64Decoder *dec, const int *decTable, char encPad)
{
  size_t    ilen, olen, rem;
  size_t    i, j, i1, i2, i3;
  unsigned  v;
  uint8_t   *out;

  if ((enc == NULL) || (enc->len == 0) || (dec == NULL))
    return b64NullPointer;
  B64DecReset(dec);

  ilen = enc->len;
  if (enc->pad == B64PAD) {
    if (enc->arry[ilen - 1] == encPad)
      ilen--;
    if (enc->arry[ilen - 1] == encPad)
      ilen--;
  }
  olen = (ilen * 6) >> 3;

  dec->arry = malloc(olen);
  if (dec->arry == NULL)
    return b64MemAllocError;
  dec->len = olen;
  out = (uint8_t *)(dec->arry);

  for (i = 0, j = 0; i < ilen; i += 4, j += 3) {
    i1 = i + 1;
    i2 = i + 2;
    i3 = i + 3;
    if ((enc->arry[i] == -1) || (enc->arry[i1] == -1))
      return b64BadParam;

    v = decTable[enc->arry[i]];
    v = (v << 6) | decTable[enc->arry[i1]];
    v = (i2 < ilen) ? (v << 6) | decTable[enc->arry[i2]] : v << 6;
    v = (i3 < ilen) ? (v << 6) | decTable[enc->arry[i3]] : v << 6;

    out[j] = (v >> 16);
    if (i2 < ilen) {
      if (enc->arry[i2] == -1)
        return b64BadParam;
      out[j + 1] = (v >> 8);
    }
    else
      return b64Success;
    if (i3 < ilen) {
      if (enc->arry[i3] == -1)
        return b64BadParam;
      out[j + 2] = v;
    }
    else
      return b64Success;
  }

  return b64Success;
}

/**
 * 
 */
static int B64EncodeFile(B64Encoder *enc, const char *filein, const char *fileout, enum B64Pad pad, const char *encTable, char encPad)
{
  int         fdin, fdout;
  struct stat sta;
  size_t      fileSize;
  B64Encoder  b64enc;
  uint8_t     *fbuffer;
  int         rlt;

  if (filein == NULL)
    return b64NullPointer;
  if ((enc == NULL) && (fileout == NULL))
    return b64NullPointer;

  B64EncInit(&b64enc);
  if (enc == NULL)
    enc = &b64enc;
  B64EncReset(enc);

  if ((fdin = open(filein, O_RDONLY)) == -1) {
    fprintf(stderr, "open %s error: %s\n", filein, strerror(errno));
    return b64BadParam;
  }
  if (fstat(fdin, &sta) == -1) {
    fprintf(stderr, "stat %s error: %s\n", filein, strerror(errno));
    close(fdin);
    return b64BadParam;
  }
  if (!S_ISREG(sta.st_mode)) {
    fprintf(stderr, "%s is not a regular file.\n", filein);
    close(fdin);
    return b64BadParam;
  }
  if ((fileSize = sta.st_size) > MAXFILESIZE) {
    fprintf(stderr, "%s too big.\n", filein);
    close(fdin);
    return b64BadParam;
  }

  if ((fbuffer = (uint8_t *)malloc(fileSize)) == NULL) {
    close(fdin);
    return b64MemAllocError;
  }
  if (read(fdin, fbuffer, fileSize) != fileSize) {
    fprintf(stderr, "read %s error: %s\n", filein, strerror(errno));
    close(fdin);
    return b64BadParam;
  }

  if ((rlt = B64Encode(enc, fbuffer, fileSize, pad, encTable, encPad)) == b64Success) {
    if (fileout) {
      if ((fdout = open(fileout, O_CREAT | O_WRONLY | O_TRUNC, FILE_MODE)) == -1) {
        fprintf(stderr, "open %s error: %s\n", fileout, strerror(errno));
        free(fbuffer);
        B64EncReset(enc);
        close(fdin);
        return b64BadParam;
      }

      if (write(fdout, enc->arry, enc->len) != enc->len) {
        fprintf(stderr, "write %s error: %s\n", fileout, strerror(errno));
        free(fbuffer);
        B64EncReset(enc);
        close(fdin);
        close(fdout);
        return b64BadParam;
      }
      close(fdout);
    }
  } else {
    fprintf(stderr, "%s encode failed: %s\n", filein, rlt == b64MemAllocError ? "b64MemAllocError" : "b64NullPointer");
    free(fbuffer);
    B64EncReset(enc);
    close(fdin);
    return b64BadParam;
  }

  free(fbuffer);
  close(fdin);
  return b64Success;
}

static int B64DecodeFile(B64Decoder *dec, const char *filein, const char *fileout, const int *decTable, char encPad)
{
  int         fdin, fdout;
  struct stat sta;
  size_t      fileSize;
  B64Encoder  b64enc;
  B64Decoder  b64dec;
  int         rlt;

  if (filein == NULL)
    return b64NullPointer;
  if ((dec == NULL) && (fileout == NULL))
    return b64NullPointer;

  B64EncInit(&b64enc);
  B64DecInit(&b64dec);
  if (dec == NULL)
    dec = &b64dec;
  B64DecReset(dec);

  if ((fdin = open(filein, O_RDONLY)) == -1) {
    fprintf(stderr, "open %s error: %s\n", filein, strerror(errno));
    return b64BadParam;
  }
  if (fstat(fdin, &sta) == -1) {
    fprintf(stderr, "stat %s error: %s\n", filein, strerror(errno));
    close(fdin);
    return b64BadParam;
  }
  if (!S_ISREG(sta.st_mode)) {
    fprintf(stderr, "%s is not a regular file.\n", filein);
    close(fdin);
    return b64BadParam;
  }
  if ((fileSize = sta.st_size) > (MAXFILESIZE << 1)) {
    fprintf(stderr, "%s too big.\n", filein);
    close(fdin);
    return b64BadParam;
  }

  if ((b64enc.arry = (char *)malloc(fileSize)) == NULL) {
    close(fdin);
    return b64MemAllocError;
  }
  if (read(fdin, b64enc.arry, fileSize) != fileSize) {
    fprintf(stderr, "read %s error: %s\n", filein, strerror(errno));
    close(fdin);
    return b64BadParam;
  }
  b64enc.len = fileSize;

  if ((rlt = B64Decode(&b64enc, dec, decTable, encPad)) == b64Success) {
    if (fileout) {
      if ((fdout = open(fileout, O_CREAT | O_WRONLY | O_TRUNC, FILE_MODE)) == -1) {
        fprintf(stderr, "open %s error: %s\n", fileout, strerror(errno));
        B64EncReset(&b64enc);
        B64DecReset(dec);
        close(fdin);
        return b64BadParam;
      }

      if (write(fdout, dec->arry, dec->len) != dec->len) {
        fprintf(stderr, "write %s error: %s\n", fileout, strerror(errno));
        B64EncReset(&b64enc);
        B64DecReset(dec);
        close(fdin);
        close(fdout);
        return b64BadParam;
      }
      close(fdout);
    }
  } else {
    fprintf(stderr, "%s docode failed: %s\n", filein, rlt == b64MemAllocError ? "b64MemAllocError" : "b64BadParam");
    B64EncReset(&b64enc);
    B64DecReset(dec);
    close(fdin);
    return b64BadParam;
  }

  B64EncReset(&b64enc);
  close(fdin);
  return b64Success;
}