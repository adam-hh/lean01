#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorting.h"

#define swap(ptra, ptrb)  \
        swapper   = *(ptra);  \
        *(ptra)   = *(ptrb);  \
        *(ptrb)   = swapper

/* Max heap percdown */
static inline void maxPercDown(PrimitiveType *A, size_t i, size_t lenOfA)
{
  size_t child;
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

/* Max heap percdown */
static inline void minPercDown(PrimitiveType *A, size_t i, size_t lenOfA)
{
  size_t child;
  PrimitiveType tmp;

  tmp = A[i];
  if (lenOfA & 0x01) {
    while ((child = (i << 1) + 1) < lenOfA) {
      child += (A[child + 1] < A[child] ? 1 : 0);
      if (A[child] < tmp)
        A[i] = A[child];
      else
        break;
      i = child;
    }
  } else {
    while ((child = (i << 1) + 1) < lenOfA) {
      if (child != lenOfA - 1)
        child += (A[child + 1] < A[child] ? 1 : 0);
      if (A[child] < tmp)
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
void heapSort(PrimitiveType *A, size_t lenOfA)
{
  size_t i;
  PrimitiveType swapper;

  for (i = (lenOfA >> 1); i >= 0; i--)  /* Build a max heap */
    maxPercDown(A, i, lenOfA);
  for (i = lenOfA - 1; i > 0; i--) {
    swap(&A[0], &A[i]);   /* Delete max*/
    maxPercDown(A, 0, i);
  }
}

/**
 * Select the k(th) greatest element from A
 * descending sequnce
 */
PrimitiveType heapSelect(PrimitiveType *A, size_t lenOfA, size_t k)
{
  int i;
  PrimitiveType *aHeap, ret;

  ret = -1;
  if (k <= lenOfA) {
    aHeap = Malloc(k * sizeof(PrimitiveType));
    memcpy(aHeap, A, k * sizeof(PrimitiveType));
    for (i = (k >> 1); i >= 0; i--)   /* Build heap, assume this is the greatest elements in A */
      minPercDown(aHeap, i, k);
    for (i = k; i < lenOfA; i++) {
      if (A[i] > aHeap[0]) {
        aHeap[0] = A[i];
        minPercDown(aHeap, 0, k);
      }
    }
    ret = aHeap[0];
    free(aHeap);
  } else
    err_quit("error: k exceed range of A");
  return ret;
}