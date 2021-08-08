#ifndef DEVIDSTACK_H
#define DEVIDSTACK_H

#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include "errhandler.h"
#ifdef __cpluscplus
extern "C" {
#endif

/*
 * The initial number of nodes in the array.
 */
static const int min_nodes = 4;
static const int max_nodes = SIZE_MAX / sizeof(void *) < INT_MAX
                             ? (int)(SIZE_MAX / sizeof(void *))
                             : INT_MAX;

/* stack top location begin with 0 */
#define EmptyTOS      (0)

#define STACK_OF(type) struct stackOf_##type   /* standard stack declaration */

typedef struct stackSt derivedStack;
typedef int (*stackEleCmpFunc)(const void *, const void *);
typedef void (*stackEleFreeFunc)(void *);
typedef void *(*stackEleCopyFunc)(const void *);

/*
 * Calculate the array growth based on the target size.
 *
 * The growth fraction is a rational number and is defined by a numerator
 * and a denominator.  According to Andrew Koenig in his paper "Why Are
 * Vectors Efficient?" from JOOP 11(5) 1998, this factor should be less
 * than the golden ratio (1.618...).
 *
 * We use 3/2 = 1.5 for simplicity of calculation and overflow checking.
 * Another option 8/5 = 1.6 allows for slightly faster growth, although safe
 * computation is more difficult.
 *
 * The limit to avoid overflow is spot on.  The modulo three correction term
 * ensures that the limit is the largest number than can be expanded by the
 * growth factor without exceeding the hard limit.
 *
 * Do not call it with |current| lower than 2, or it will infinitely loop.
 */
static inline int compute_growth(int target, int current)
{
    const int limit = (max_nodes / 3) * 2 + (max_nodes % 3 ? 1 : 0);

    while (current < target) {
        /* Check to see if we're at the hard limit */
        if (current >= max_nodes)
            return 0;

        /* Expand the size by a factor of 3/2 if it is within range */
        current = current < limit ? current + current / 2 : max_nodes;
    }
    return current;
}


stackEleCmpFunc stSetCmpFunc(derivedStack *st, stackEleCmpFunc func);
derivedStack *stDup(const derivedStack *st);
derivedStack *OPENSSL_sk_deep_copy(const derivedStack *st,
                             stackEleCopyFunc copy_func,
                             stackEleFreeFunc free_func);

derivedStack *stNewNULL(void);
derivedStack *stNewNULLC(stackEleCmpFunc c);

int stReserve(derivedStack *st, int n);
void stFree(derivedStack *st);
void stPopFree(derivedStack *st, stackEleFreeFunc func);
void stZero(derivedStack *st);

int stTopOfStack(derivedStack *st);
void *stGetAt(derivedStack *st, int idx);
void *stSetAt(derivedStack *st, int idx,const void *data);

void stSort(derivedStack *st);
int stIsSorted(derivedStack *st);

derivedStack *stNewReserve(stackEleCmpFunc func, int cap);

int stInsert(derivedStack *st,const void *data, int loc);
void *stDeletePtr(derivedStack *st, const void *p);
void *stDelete(derivedStack *st, int loc);

int stPush(derivedStack *st, const void *data);
void *stPop(derivedStack *st);
void *stTop(derivedStack *st);


/**
 * Standard derived type stack definition
 * it's a arrary structure stack, with simple stack ops
 * method: createStack, releaseStack, pushStack, topStack, popStack, isEmptyStack
 */
#define DEFINE_DERIVSTACK_OF(type)  \
STACK_OF(type); \
typedef int (*type##CmpFunc)(const type *a, const type *b); \
typedef void (*type##FreeFunc)(type *a);  \
typedef type *(*type##CopyFunc)(const type *a); \
static inline int stTopOfStack_##type(STACK_OF(type) *st) \
{ \
  return stTopOfStack((derivedStack *)st);  \
} \
static inline STACK_OF(type) *stNewReserve_##type(type##CmpFunc cmp, int capacity)  \
{ \
  return (STACK_OF(type) *)stNewReserve((stackEleCmpFunc)cmp, capacity);  \
} \
static inline void stFree_##type(STACK_OF(type) *st) \
{ \
  stFree((derivedStack *)st); \
} \
static inline void stPopFree_##type(STACK_OF(type) *st, type##FreeFunc freefunc)  \
{ \
  return stPopFree((derivedStack *)st, (stackEleFreeFunc)freefunc); \
} \
static inline int stPush_##type(STACK_OF(type) *st, type *ptr)  \
{ \
  return stPush((derivedStack *)st, (const void *)ptr); \
} \
static inline type *stPop_##type(STACK_OF(type) *st)  \
{ \
  return (type *)stPop((derivedStack *)st); \
} \
static inline type *stTop_##type(STACK_OF(type) *st)  \
{ \
  return (type *)stTop((derivedStack *)st); \
}


#ifdef __cpluscplus
}
#endif

#endif    /* DEVIDESTACK_H */