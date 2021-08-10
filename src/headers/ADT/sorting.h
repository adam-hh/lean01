#ifndef PRIMITIVESORTING_H
#define PRIMITIVESORTING_H
#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include "errhandler.h"

#ifdef __cpluscplus
extern "C" {
#endif

#ifndef PrimitieType
#define PrimitiveType char
#endif

void insertionSort(PrimitiveType *a, size_t len);
void shellSort(PrimitiveType *a, size_t len);

void heapSort(PrimitiveType *A, size_t lenOfA);
PrimitiveType heapSelect(PrimitiveType *A, size_t lenOfA, size_t k);

void qSort_p(PrimitiveType *a, size_t left, size_t right);
void qSelect_p(PrimitiveType *a, size_t k, size_t left, size_t right);

#ifdef __cpluscplus
}
#endif

#endif    /* PRIMITIVESORTING_H */