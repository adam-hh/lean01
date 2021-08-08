#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pqueue.h"

#define swap(ptra, ptrb)  \
        swapper   = *(ptra);  \
        *(ptra)   = *(ptrb);  \
        *(ptrb)   = swapper

static inline void percDown(PrimitiveType *A, int i, int lenOfA)
{
  int child;
  PrimitiveType tmp;

  tmp = A[i];
  if (lenOfA & 0x01) {
    while ((child = (i << 1) + 1) < lenOfA) {
      child += (A[child + 1] > A[child] ? 1 : 0);
      if (A[child] > tmp)
        A[i] = A[child];
      else
        break;
      i = child;
    }
  } else {
    while ((child = (i << 1) + 1) < lenOfA) {
      if (child != lenOfA - 1)
        child += (A[child + 1] > A[child] ? 1 : 0);
      if (A[child] > tmp)
        A[i] = A[child];
      else
        break;
      i = child;
    }
  }
  A[i] = tmp;
}

/**
 * A heap sorting rutine
 * input:   A, lenOfA(number of elements in arrary A)
 * output:  A
 * return:  void
 */
void heapSort(PrimitiveType *A, int lenOfA)
{
  int i;
  PrimitiveType swapper;

  for (i = (lenOfA >> 1); i >= 0; i--)  /* Build a max heap */
    percDown(A, i, lenOfA);
  for (i = lenOfA - 1; i > 0; i--) {
    swap(&A[0], &A[i]);   /* Delete max*/
    percDown(A, 0, i);
  }
}