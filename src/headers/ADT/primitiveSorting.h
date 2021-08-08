#ifndef PRIMITIVESORTING_H
#define PRIMITIVESORTING_H

#ifdef __cpluscplus
extern "C" {
#endif

/* Internal maco */
#define swap(ptra, ptrb)  \
        swapper   = *(ptra);  \
        *(ptra)   = *(ptrb);  \
        *(ptrb)   = swapper

#define DEFINE_PRIMSORTING_OF(type) \
static inline void insertionSort_##type(type *a, size_t len)  \
{ \
  size_t j, p;  \
  type tmp; \
  for (p = 1; p < len; p++) { \
    tmp = a[p]; \
    for (j = p; j > 0 && a[j - 1] > tmp; j--) \
      a[j] = a[j - 1];  \
    a[j] = tmp; \
  } \
} \
static inline void shellSort_##type(type *a, size_t len) \
{ \
  size_t i, j, increment; \
  type tmp; \
  for (increment = (len >> 1); increment > 0; increment >>= 1)  \
    for (i = increment; i < len; i++) { \
      tmp = a[i]; \
      for (j = i; j >= increment; j -= increment) \
        if (tmp < a[j - increment]) \
          a[j] = a[j - increment];  \
        else  \
          break;  \
      a[j] = tmp; \
    } \
} \
static inline type median3_##type(type *a, int left, int right)  \
{ \
  type swapper; \
  int center = (left + right) >> 1; \
  if (a[left] > a[center]) {  \
    swap(&a[left], &a[center]); \
  } \
  if (a[left] > a[right]) { \
    swap(&a[left], &a[right]);  \
  } \
  if (a[center] > a[right]) { \
    swap(&a[center], &a[right]); \
  } \
  swap(&a[center], &a[right - 1]);  \
  return a[right - 1];  \
} \
static inline void qsort_##type(type *a, int left, int right) \
{ \
  int i, j; \
  type swapper, pivot; \
  if (left + 3 <= right) {  \
    pivot = median3_##type(a, left, right); \
    i = left; \
    j = right - 1;  \
    for (; ;) { \
      while (a[++i] < pivot)  \
        ; \
      while (a[--j] > pivot)  \
        ; \
      if (i < j) {  \
        swap(&a[i], &a[j]); \
      } else  \
        break;  \
    } \
    swap(&a[i], &a[right - 1]); \
    qsort_##type(a, left, i - 1); \
    qsort_##type(a, i + 1, right);  \
  } else  \
    insertionSort_##type(a + left, right - left + 1); \
}











#ifdef __cpluscplus
}
#endif

#endif    /* PRIMITIVESORTING_H */