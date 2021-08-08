#include <stdlib.h>
#include <string.h>
#include "derivedStack.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

const void *OBJ_bsearch_ex_(const void *key, const void *base_, int num,
                            int size,
                            int (*cmp) (const void *, const void *),
                            int flags);

/**
 * a general derived type stack.
 * refs from openssl source
 */
struct stackSt {
  int     capacity;
  int     topOfStack;
  int     sorted;
  const void    **data;
  stackEleCmpFunc   cmp;
};

stackEleCmpFunc stSetCmpFunc(derivedStack *st, stackEleCmpFunc func)
{
  stackEleCmpFunc old;

  old = st->cmp;
  if (st->cmp != func)
    st->sorted = 0;
  st->cmp = func;
  return old;
}

derivedStack *stDup(const derivedStack *st)
{
    derivedStack *ret;

    if ((ret = malloc(sizeof(*ret))) == NULL) {
        err_msg("malloc error");
        return NULL;
    }

    /* direct structure assignment */
    *ret = *st;

    if (st->topOfStack == EmptyTOS) {
        /* postpone |ret->data| allocation */
        ret->data = NULL;
        ret->capacity = 0;
        return ret;
    }
    /* duplicate |sk->data| content */
    if ((ret->data = malloc(sizeof(*ret->data) * st->capacity)) == NULL)
        goto err;
    memcpy(ret->data, st->data, sizeof(void *) * st->topOfStack);
    return ret;
 err:
    free(ret);
    return NULL;
}

derivedStack *stDeepCopy(const derivedStack *st,
                             stackEleCopyFunc copy_func,
                             stackEleFreeFunc free_func)
{
    derivedStack *ret;
    int i;

    if ((ret = malloc(sizeof(*ret))) == NULL) {
        err_msg("malloc error");
        return NULL;
    }

    /* direct structure assignment */
    *ret = *st;

    if (st->topOfStack == 0) {
        /* postpone |ret| data allocation */
        ret->data = NULL;
        ret->capacity = 0;
        return ret;
    }

    ret->capacity = st->topOfStack > min_nodes ? st->topOfStack : min_nodes;
    ret->data = calloc(1, sizeof(*ret->data) * ret->capacity);
    if (ret->data == NULL) {
        free(ret);
        return NULL;
    }

    for (i = 0; i < ret->topOfStack; ++i) {
        if (st->data[i] == NULL)
            continue;
        if ((ret->data[i] = copy_func(st->data[i])) == NULL) {
            while (--i >= 0)
                if (ret->data[i] != NULL)
                    free_func((void *)ret->data[i]);
            free(ret);
            return NULL;
        }
    }
    return ret;
}

derivedStack *stNewNULL(void)
{
    return stNewReserve(NULL, 0);
}

derivedStack *stNewNULLC(stackEleCmpFunc c)
{
    return stNewReserve(c, 0);
}

/**
 * Allocate or rellocate the memory of stack to st->topOfStack + n
 * n is the expected new capacity, exact is a flag to apply compute_growth
 * if exact euals 0 won't reduce mem size, otherwise may reduce mem size
 * input:   st, n, exact
 * output:  none
 * return:  Errorcodes
 */
static int reserve(derivedStack *st, int n, int exact)
{
    const void **tmpdata;
    int  newCap;

    /* Check to see the reservation isn't exceeding the hard limit */
    if (n > max_nodes - st->topOfStack)
        return 0;

    /* Figure out the new size */
    newCap = st->topOfStack + n;
    if (newCap < min_nodes)
        newCap = min_nodes;

    /* If |st->data| allocation was postponed */
    if (st->data == NULL) {
        /*
         * At this point, |st->num_alloc| and |st->num| are 0;
         * so |num_alloc| value is |n| or |min_nodes| if greater than |n|.
         */
        if ((st->data = malloc(sizeof(*st->data) * newCap)) == NULL) {
            err_msg("maloc error");
            return 0;
        }
        st->capacity = newCap;
        return 1;
    }

    if (!exact) {
        if (newCap <= st->capacity)
            return 1;   /* won't reduce mem size */
        newCap = compute_growth(newCap, st->capacity);
        if (newCap == 0)
            return 0;
    } else if (newCap == st->capacity) {
        return 1;       /* may reduce mem size */
    }

    tmpdata = realloc((void *)st->data, sizeof(*st->data) * newCap);
    if (tmpdata == NULL)
        return 0;

    st->data = tmpdata;
    st->capacity = newCap;
    return 1;
}

/* change stack size to st->topOfStack + n */
int stReserve(derivedStack *st, int n)
{
  if (st == NULL)
    return 0;
  if (n < 0)
    return 1;
  return reserve(st, n, 1);
}

void stFree(derivedStack *st)
{
  if (st == NULL)
    return;
  free(st->data);
  free(st);
}

void stPopFree(derivedStack *st, stackEleFreeFunc func)
{
  if (st == NULL)
    return;
  for (int i = 0; i < st->topOfStack; i++)
    if (st->data[i] != NULL)
      func((char *)st->data[i]);  /* try cast to char* to handle compiler warning */
}

void stZero(derivedStack *st)
{
  if (st == NULL || st->capacity == 0)
    return;
  memset(st->data, 0, sizeof(*st->data) * st->capacity);
  st->capacity = 0;
}

int stTopOfStack(derivedStack *st)
{
  return st == NULL ? -1 : st->topOfStack;
}

void *stGetAt(derivedStack *st, int idx)
{
  if (st == NULL || idx < 0 || idx >= st->topOfStack)
    return NULL;
  return (void *)st->data[idx];
}

void *stSetAt(derivedStack *st, int idx, const void *data)
{
  if (st == NULL || idx < 0 || idx >= st->topOfStack)
    return NULL;
  st->data[idx] = data;
  st->sorted = 0;
  return (void *)st->data[idx];
}

void stSort(derivedStack *st)
{
  if (st != NULL && !st->sorted && st->cmp != NULL) {
    if (st->capacity > 1)
      qsort(st->data, st->capacity, sizeof(void *), st->cmp);
    st->sorted = 1;
  }
}

int stIsSorted(derivedStack *st)
{
  return st == NULL ? 1 : st->sorted;
}

derivedStack *stNewReserve(stackEleCmpFunc func, int cap)
{
  derivedStack *st;

  if ((st = calloc(1, sizeof(derivedStack))) == NULL)
    return NULL;
  st->topOfStack = EmptyTOS;
  st->cmp = func;

  if (cap <= 0)
    return st;
  
  if (!stReserve(st, cap)) {
    free(st);
    return NULL;
  }

  return st;
}

/* random loc insert is not a regular stack op, but this is the undlying func for push op */
int stInsert(derivedStack *st,const void *data, int loc)
{
  if (st == NULL || st->capacity == max_nodes)
    return 0;
  if (!reserve(st, 1, 0))   /* may increase the stack size dynamictly */
    return 0;
  
  if ((loc >= st->topOfStack) || (loc < 0)) {
    st->data[st->topOfStack] = data;
  } else {
    memmove(&st->data[loc + 1], &st->data[loc], sizeof(st->data[0]) * (st->topOfStack - loc));
    st->data[loc] = data;
  }
  st->topOfStack++;
  st->sorted = 0;
  return st->topOfStack;
}

/* be ware, random loc delete is not a regular stack op */
static inline void *internalDelete(derivedStack *st, int loc)
{
  const void *ret;

  ret = st->data[loc];
  if (loc != st->topOfStack - 1)
    memmove(&st->data[loc], &st->data[loc + 1], sizeof(st->data[0]) * (st->topOfStack - loc - 1));
  st->topOfStack--;

  return (void *)ret;
}

void *stDeletePtr(derivedStack *st, const void *p)
{
  for (int i = 0; i < st->topOfStack; i++)
    if (st->data[i] == p)
      return internalDelete(st, i);
  return NULL;
}

void *stDelete(derivedStack *st, int loc)
{
  if (st == NULL || loc < 0 || loc >= st->topOfStack)
    return NULL;
  return internalDelete(st, loc);
}

/* find is not a regular stack op */
static int internalFind(derivedStack *st, const void *data, int retOptions)
{
  const void *r;
  int i;

  if (st == NULL || st->topOfStack == 0)
    return -1;
  if (st->cmp == NULL) {
    for (i = 0; i < st->topOfStack; i++)
      if (st->data[i] == data)
        return i;
    return -1;
  }

  if (!st->sorted) {
    if (st->topOfStack > 1)
      qsort(st->data, st->topOfStack, sizeof(void *), st->cmp);
    st->sorted = 1;
  }
  if (data == NULL)
    return -1;
  r = OBJ_bsearch_ex_(&data, st->data, st->topOfStack, sizeof(void *), st->cmp,
                        retOptions);

  return r == NULL ? -1 : (int)((const void **)r - st->data);
}
int stFind(derivedStack *st, const void *data)
{
    return internalFind(st, data, 0x02);
}
int stFindex(derivedStack *st, const void *data)
{
    return internalFind(st, data, 0x01);
}

int stPush(derivedStack *st, const void *data)
{
  if (st == NULL)
    return -1;
  return stInsert(st, data, st->topOfStack);
}

/* pop return the top element value and move top loc */
void *stPop(derivedStack *st)
{
  if (st == NULL || st->topOfStack == 0)
    return NULL;
  return internalDelete(st, st->topOfStack - 1);
}

/* top return the top element value, but won't move top loc */
void *stTop(derivedStack *st)
{
  if (st == NULL || st->topOfStack == 0)
    return NULL;
  return (void *)st->data[st->topOfStack - 1];
}