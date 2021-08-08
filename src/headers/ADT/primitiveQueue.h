#ifndef COMMONDS_H
#define COMMONDS_H

#include "primitiveStack.h"

#ifdef __cpluscplus
extern "C" {
#endif

#define QUEUE_OF(type) struct queueOf_##type   /* standard stack declaration */

/**
 * Standard primative type queue definition
 * it's a arrary structure implementation
 * method: 
 */
#define IsEmpty(Q)  ((Q)->size == 0)
#define IsFull(Q)   ((Q)->size >= (Q)->capacity)
#define MakeEmpty(Q)  \
        (Q)->size = 0;  \
        (Q)->front = 1; \
        (Q)->rear = 0
#define NextSlot(loc, Q)  (++(loc) == (Q)->capacity ? 0 : loc)
#define DEFINE_PRIMQUEUE_OF(type) \
QUEUE_OF(type) { \
  int    capacity;  \
  int    front;  \
  int    rear;  \
  int    size;  \
  type   *data; \
};  \
static inline QUEUE_OF(type) *primQueNew_##type(int capacity) \
{ \
  QUEUE_OF(type)  *que;  \
  if (capacity < min_nodes || capacity > max_nodes) { \
    err_msg("illegal capacity");  \
    return NULL;  \
  }  \
  if ((que = malloc(sizeof(QUEUE_OF(type)))) == NULL){ \
    err_msg("malloc error");  \
    return NULL;  \
  } \
  if ((que->data = malloc(sizeof(type) * capacity)) == NULL) { \
    err_msg("malloc error");  \
    free(que); \
    return NULL;  \
  } \
  que->capacity = capacity;  \
  MakeEmpty(que);  \
  return que;  \
} \
static inline void primQueFree_##type(QUEUE_OF(type) *que)  \
{ \
  free(que->data); \
  free(que); \
} \
static inline void primQueEnqueue_##type(type x, QUEUE_OF(type) *que) \
{ \
  if (IsFull(que))  \
    err_quit("Full queue"); \
  else {  \
    que->size++;  \
    que->rear = NextSlot(que->rear, que); \
    que->data[que->rear] = x; \
  } \
} \
static inline type primQueDequeue_##type(QUEUE_OF(type) *que) \
{ \
  type ret; \
  if (IsEmpty(que)) \
    err_quit("Empty queue");  \
  else {  \
    ret = que->data[que->front];  \
    que->size--;  \
    que->front = NextSlot(que->front, que); \
    return ret; \
  } \
}

#ifdef __cpluscplus
}
#endif    /* __cpluscplus */

#endif    /* COMONDS_H */