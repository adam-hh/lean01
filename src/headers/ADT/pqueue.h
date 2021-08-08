#ifndef PQUEUE_H
#define PQUEUE_H
#include "singleList.h"

#ifdef __cpluscplus
extern "C" {
#endif

#ifndef PrimitieType
#define PrimitiveType char
#endif

#define MinPQSize 15
#define MinData 0
typedef struct binHeap *binHeapPQueue;

binHeapPQueue bhInitialize(int capacity);
void bhDestroy(binHeapPQueue H);
void bhMakeEmpty(binHeapPQueue H);
void bhInsert(PrimitiveType X, binHeapPQueue H);
PrimitiveType bhDeleteMin(binHeapPQueue H);
PrimitiveType bhFindMin(binHeapPQueue H);
int bhIsEmpty(binHeapPQueue H);
int bhIsFull(binHeapPQueue H);

void heapSort(PrimitiveType *A, int lenOfA);


#ifdef __cpluscplus
}
#endif

#endif  /* PQUEUE_H */