#ifndef PRIMITIVESTACK_H
#define PRIMITIVESTACK_H

#include "derivedStack.h"

#ifdef __cpluscplus
extern "C" {
#endif

/**
 * Standard primative type stack definition
 * it's a arrary structure stack, with simple stack ops
 * method: createStack, releaseStack, pushStack, topStack, popStack, isEmptyStack
 */
#define DEFINE_PRIMSTACK_OF(type) \
STACK_OF(type) { \
  int    capacity;  \
  int    topOfStack;  \
  type   *data; \
};  \
static inline int primstTopOfStack_##type(STACK_OF(type) *st) \
{ \
  return st->topOfStack;  \
} \
static inline STACK_OF(type) *primstNew_##type(int capacity)  \
{ \
  STACK_OF(type)  *st;  \
  if (capacity < min_nodes || capacity > max_nodes) { \
    err_msg("illegal capacity");  \
    return NULL;  \
  }  \
  if ((st = malloc(sizeof(STACK_OF(type)))) == NULL){ \
    err_msg("malloc error");  \
    return NULL;  \
  } \
  if ((st->data = malloc(sizeof(type) * capacity)) == NULL) { \
    err_msg("malloc error");  \
    free(st); \
    return NULL;  \
  } \
  st->capacity = capacity;  \
  st->topOfStack = EmptyTOS;  \
  return st;  \
} \
static inline void primstFree_##type(STACK_OF(type) *st)  \
{ \
  free(st->data); \
  free(st); \
} \
static inline void primstZero_##type(STACK_OF(type) *st)  \
{ \
  st->topOfStack = EmptyTOS;  \
} \
static inline int primstPush_##type(STACK_OF(type) *st, type data) \
{ \
  int newCap; \
  newCap = st->topOfStack + 1;  \
  if (newCap > st->capacity) { \
    newCap = compute_growth(newCap, st->capacity);  \
    if (newCap == 0) { \
      err_msg("stack growth error");  \
      return 0; \
    } \
    st->data = realloc(st->data, sizeof(type) * newCap);  \
    if (st->data == NULL) { \
      err_msg("realloc error"); \
      return 0; \
    } \
    st->capacity = newCap;  \
  } \
  st->data[st->topOfStack++] = data; \
  return st->topOfStack;  \
} \
static inline type primstPop_##type(STACK_OF(type) *st) \
{ \
  if (st->topOfStack != EmptyTOS) \
    return st->data[--st->topOfStack];  \
  err_msg("empty stack"); \
  return (type)0; \
} \
static inline type primstTop_##type(STACK_OF(type) *st) \
{ \
  if (st->topOfStack != EmptyTOS) \
    return st->data[st->topOfStack - 1];  \
  err_msg("empty stack"); \
  return (type)0; \
}

void reverPoland(FILE *in, FILE *out);
void postToinfix(FILE *in, FILE *out);

#ifdef __cpluscplus
}
#endif

#endif    /* PRIMITIVESTACK_H */