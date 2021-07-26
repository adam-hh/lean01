#include <stdint.h>
#include <stdio.h>
#include "bitArray.h"

typedef uint8_t octetBitSet[8];

octetBitSet octetArray[256];

/**
 * A static table of octed's bit width
 * when octed != 0, then octed's degree == octed's bit width - 1
 */
const uint8_t octedBitWidth[256] = {
  0, 1, 2, 2, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6,
  7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8
};

const uint8_t octedBitCount[256] = {
  0, 1, 1, 2, 1, 2, 2, 3,
  1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7,
  5, 6, 6, 7, 6, 7, 7, 8
};

void initOctetArray()
{
  uint8_t val;

  for (val = 0; val != 0xff; val++) {
    octetArray[val][0] = (val & 0x01) ? 0 : 0xff;
    octetArray[val][1] = (val & 0x02) ? 1 : 0xff;
    octetArray[val][2] = (val & 0x04) ? 2 : 0xff;
    octetArray[val][3] = (val & 0x08) ? 3 : 0xff;

    octetArray[val][4] = (val & 0x10) ? 4 : 0xff;
    octetArray[val][5] = (val & 0x20) ? 5 : 0xff;
    octetArray[val][6] = (val & 0x40) ? 6 : 0xff;
    octetArray[val][7] = (val & 0x80) ? 7 : 0xff;
  }
  octetArray[val][0] = 0;
  octetArray[val][1] = 1;
  octetArray[val][2] = 2;
  octetArray[val][3] = 3;
  octetArray[val][4] = 4;
  octetArray[val][5] = 5;
  octetArray[val][6] = 6;
  octetArray[val][7] = 7;
}

void prtOctetArray()
{
  int       i, j, k;
  uint64_t  *ptr;
  uint64_t ff, tmp1, tmp2;

  ff = 0xffffffffffffffff;
  for (i = 0; i < 256; i++) {
    ptr = (uint64_t *)&octetArray[i][0];
    printf("0x%02x: {", i);
    for (j = k = 0; j < 8; j++) {
      if (octetArray[i][j] != 0xff) {
        k++;
        tmp1 = (*ptr) >> ((j + 1) * 8);
        tmp2 = ff >> ((j + 1) * 8);
        printf("%d%c", octetArray[i][j], (tmp1 == tmp2) || (j == 7) ? 0x01 : ',');
      }
    }
    k = j-k;
    while (k--)
      printf("  ");
    printf("} %c", ((i + 1) & 0x03) == 0 ? '\n' : ' ');
  }
}

void initOctedBitWidth()
{
  uint32_t  val;
  uint32_t  cnt;
  uint32_t  mid;

  for (val = 0; val < 256; val++) {
    mid = val;
    cnt = 0;
    while (mid) {
      cnt++;
      mid >>= 1;
    }
    //octedBitWidth[val] = cnt;
  }
}
void prtOctedBitWidth()
{
  int i;
  for (i = 0; i < sizeof(octedBitWidth) - 1; i++) {
    printf("%d,%c", octedBitWidth[i], (i + 1) % 8 == 0 ? '\n' : ' ');
  }
  printf("%d\n", octedBitWidth[i]);
}

void initOctedBitCount()
{
  uint32_t val;
  uint32_t cnt;
  uint32_t mid;

  for (val = 0; val < 256; val++) {
    mid = val;
    cnt = 0;
    while (mid) {
      if (mid & 0x01)
        cnt++;
      mid >>= 1;
    }
    //octedBitCount[val] = cnt;
  }
}
void prtOctedBitCount()
{
  int i;
  for (i = 0; i < sizeof(octedBitCount) - 1; i++) {
    printf("%d,%c", octedBitCount[i], (i + 1) % 8 == 0 ? '\n' : ' ');
  }
  printf("%d\n", octedBitCount[i]);
}