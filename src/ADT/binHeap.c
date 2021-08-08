#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pqueue.h"

struct binHeap {
  int           capacity;
  int           size;
  PrimitiveType *elements;   
};

static inline void percolateDown(binHeapPQueue H, int i)
{
  int child;
  PrimitiveType swapper;
  swapper = H->elements[i];
  if (H->size & 0x01) {   /* Odd size */
    while ((child = i << 1) < H->size) {
      child += (H->elements[child + 1] < H->elements[child] ? 1 : 0);
      if (H->elements[i] < H->elements[child])
        H->elements[i] = H->elements[child];
      else
        break;
      i = child;    /* Direction towards down */
    }
  } else {    /* Even size */
    while ((child = i << 1) <= H->size) {
      if (child != H->size)
        child += (H->elements[child + 1] < H->elements[child] ? 1 : 0);
      if (H->elements[i] < H->elements[child])
        H->elements[i] = H->elements[child];
      else
        break;
      i = child;
    }
  }
  
  H->elements[i] = swapper;
}

/* PercolateUp is the insert operation to a bin heap */
static inline void percolateUp(binHeapPQueue H)
{
  int i, parent;
  PrimitiveType swapper;
  i = H->size;
  swapper = H->elements[i];
  while ((parent = (i >> 1)) >= 1) {
    if (H->elements[parent] > swapper) {
      H->elements[i] = H->elements[parent];
      i = parent;  /* Direction towards up */
    } else
      break;
  }
  H->elements[i] = swapper;
}

void bhBuildHeap(binHeapPQueue H)
{
  for (int i = H->size << 1; i > 0; i--)
    percolateDown(H, i);
}

binHeapPQueue bhInitialize(int capacity)
{
  binHeapPQueue H;

  if (capacity > MinPQSize) {
    H = Malloc(sizeof(struct binHeap));
    H->elements = Malloc((capacity + 1) * sizeof(PrimitiveType));
    H->capacity = capacity;
    H->size = 0;
    H->elements[0] = MinData;
  } else 
    err_quit("error: PQueue size is too small");
  return H;
}

int bhIsEmpty(binHeapPQueue H)
{
  if (H)
    return H->size == 0;
  else
    err_quit("fatal error: Null Pointer");
  return 0;
}

int bhIsFull(binHeapPQueue H)
{
  if (H)
    return H->size >= H->capacity;
  else
    err_quit("fatal error: Null Pointer");
  return 0;
}

PrimitiveType bhFindMin(binHeapPQueue H)
{
  if (H && H->size > 0)
    return H->elements[1];
  else
    err_quit("fatal error: empty PQueue");
  return 0;
}

void bhInsert(PrimitiveType X, binHeapPQueue H)
{
  int i;
  if (!bhIsFull(H)) {
    for (i = ++H->size; H->elements[i >> 1] > X; i >>= 1)
      H->elements[i] = H->elements[i >> 1];
    H->elements[i] = X;   /* X inserted to the last then percolate up it */
  } else
    err_msg("error: PQueue is full");
}

PrimitiveType bhDeleteMin(binHeapPQueue H)
{
  int i, child;
  PrimitiveType minElement, lastElement;

  i = 1;
  if (H->size > 0) {
    minElement = H->elements[1];
    lastElement = H->elements[H->size--];

    if (H->size & 0x01) {
      while ((child = i << 1) < H->size) {
        child += (H->elements[child + 1] < H->elements[child] ? 1 : 0);
        if (lastElement > H->elements[child])
          H->elements[i] = H->elements[child];
        else
          break;
        i = child;
      }
    } else {
      while ((child = i << 1) <= H->size) {
        if (child != H->size)
          child += (H->elements[child + 1] < H->elements[child] ? 1 : 0); 
        if (lastElement > H->elements[child])
          H->elements[i] = H->elements[child];
        else
          break;
        i = child;
      }
    }
    H->elements[i] = lastElement;
    return minElement;
  } else {
    err_msg("error: PQueue is empty");
    return H->elements[0];
  }
}