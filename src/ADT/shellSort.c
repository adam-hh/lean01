/**
 * Sedgewick sequnce ref from
 * https://en.wikipedia.org/wiki/Shellsort#Gap_sequences
 * math formula : 4^{k}+3 * 2^{k-1}+1, prefixed with 1
 */
#include "sorting.h"

/**
 * Since the sedgewick sequnce increasing very fast.
 * maybe it is better to store them in a static arrary.
 */
static inline size_t sedGapSeq(size_t k)
{
  if (k == 0)
    return 1;
  return (1 << (k << 1)) + 3 * (1 << (k - 1)) + 1;
}

/* calculate the first sed seq value, on descending order */
static size_t firstSedGap(size_t N)
{
  size_t p;
  size_t uN;

  uN = N;   /* Store the original value of N */
  if (N < 100)
    return 0;

  p = 0;
  while(N >>= 1)
    p++;
  p >>= 1;

  if (uN > sedGapSeq(p + 1)) /* Use the original N to compare */
    return p + 1;
  else if (uN > sedGapSeq(p))
    return p;
  else
    return p - 1;
}

/* insertion sorting */
void insertionSort(PrimitiveType *a, size_t len)
{
  size_t j, p;
  PrimitiveType tmp;
  for (p = 1; p < len; p++) {
    tmp = a[p];
    for (j = p; j > 0 && a[j - 1] > tmp; j--)
      a[j] = a[j - 1];
    a[j] = tmp;
  }
}

/**
 * Shell sorting with Sedgewick gap sequnce
 */
void shellSort(PrimitiveType *a, size_t len)
{
  size_t i, j, increment;
  int sedIdx;
  PrimitiveType tmp;

  for (sedIdx = (int)firstSedGap(len); sedIdx >= 0; sedIdx--) {
    increment = sedGapSeq(sedIdx);
    for (i = increment; i < len; i++) {
      tmp = a[i];
      for (j = i; j >= increment; j -= increment)
        if (tmp < a[j - increment])
          a[j] = a[j - increment]; 
        else 
          break; 
      a[j] = tmp;
    }
  }
}