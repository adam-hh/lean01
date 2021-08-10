#include "sorting.h"

#define swap(ptra, ptrb)  \
        swapper   = *(ptra);  \
        *(ptra)   = *(ptrb);  \
        *(ptrb)   = swapper

#define cutoff 10     /* a limit value, use insertion sort when arrary size less then it */

/**
 * Pre sorting a[left] a[center] and a[right]
 * so that a[left](a[right]) be the reason to stop cursor i(j) overband.
 */
static inline PrimitiveType median3(PrimitiveType *a, size_t left, size_t right)
{
  PrimitiveType swapper; 
  size_t center = (left + right) >> 1;

  if (a[left] > a[center]) {
    swap(&a[left], &a[center]);
  }
  if (a[left] > a[right]) {
    swap(&a[left], &a[right]); 
  }
  if (a[center] > a[right]) {
    swap(&a[center], &a[right]);
  }
  /* partitial sorting result a[left] <= a[center] <= a[right] */

  swap(&a[center], &a[right - 1]);  /* save pivot to right - 1*/
  return a[right - 1];  /* return pivot */
}

/**
 * A Quick sort implementation
 * input:   a, left, right (left and right are index not size)
 * output:  a
 * return:  void
 */
void qSort_p(PrimitiveType *a, size_t left, size_t right)
{
  size_t i, j;
  PrimitiveType swapper, pivot;

  if (left + cutoff <= right) {
    pivot = median3(a, left, right);
    i = left;
    j = right - 1;
    for (; ;) {
      while (a[++i] < pivot)  /* strictly less, equal stop */
        ;
      while (a[--j] > pivot)  /* strictly greater, equal stop */
        ;
      if (i < j) {
        swap(&a[i], &a[j]);
      } else
        break;
    }
    swap(&a[i], &a[right - 1]); /* restore pivot */

    qSort_p(a, left, i - 1);
    qSort_p(a, i + 1, right); 
  } else
    insertionSort(a + left, right - left + 1);
}

/**
 * Quick select the k(th) greatest element from a
 * ascending sequnce, a[k - 1] is the answer
 * Note, a is an output parameter
 */
void qSelect_p(PrimitiveType *a, size_t k, size_t left, size_t right)
{
  size_t i, j;
  PrimitiveType swapper, pivot;

  if (left + cutoff <= right) {
    pivot = median3(a, left, right);
    i = left;
    j = right - 1;
    for (; ;) {
      while (a[++i] < pivot)  /* strictly less, equal stop */
        ;
      while (a[--j] > pivot)  /* strictly greater, equal stop */
        ;
      if (i < j) {
        swap(&a[i], &a[j]);
      } else
        break;
    }
    swap(&a[i], &a[right - 1]); /* restore pivot */

    if (k <= i)
      qSelect_p(a, k, left, i - 1);
    else if (k > i + 1)
      qSelect_p(a, k, i + 1, right);
  } else
    insertionSort(a + left, right - left + 1);
}