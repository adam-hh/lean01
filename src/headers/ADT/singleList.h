#ifndef PRIMITIVELIST_H
#define PRIMITIVELIST_H

#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include "errhandler.h"

#ifdef __cpluscplus
extern "C" {
#endif

typedef struct listSt derivedNode;        /* Entry of the lsit */
typedef struct listSt *derivedList;       /* start address of the list, usally header node address */
typedef struct listSt *derivedPosition;   /* address of any entry */
typedef void *elementType;                /* element type of a derived is actually a void pointer */
/* List element compare function prototype */
typedef int (*listEleCmpFunc)(const void *a, const void *b);

derivedList listCreateList(listEleCmpFunc cmp);
derivedList listMakeEmpty(derivedList L);
int listIsEmpty(derivedList L);
int listIsLast(derivedPosition P, derivedList L);
derivedPosition listFind(elementType X, derivedList L);
void listDeleteEntry(elementType X, derivedList L);
derivedPosition listFindPre(elementType X, derivedList L);
void listInsert(elementType X, derivedList L, derivedPosition P);
void listDeleteList(derivedList L);
derivedPosition listFirst(derivedList L);
elementType listRetrive(derivedPosition P);

/* standard list nameing schema */
#define LIST_OF(type) struct listOf_##type

/* standard single chain derived list declaration */
#define DEFINE_DERIVLIST_OF(type) \
LIST_OF(type);  \
typedef int (*listEleCmpFunc_##type)(const void *a, const void *b); \
typedef LIST_OF(type)  derivedNode_##type;  \
typedef LIST_OF(type)  *derivedList_##type; \
typedef LIST_OF(type)  *derivedPosition_##type; \
static derivedList_##type listCreateList_##type(listEleCmpFunc_##type cmp)  \
{ \
  return (derivedList_##type)listCerateList((listEleCmpFunc)cmp);  \
} \
static derivedList_##type listMakeEmpty_##type(derivedList_##type L) \
{ \
  return (derivedList_##type)listMakeEmpty((derivedList)L); \
} \
static int listIsEmpty_##type(derivedList_##type L)  \
{ \
  return listIsEmpty((derivedList)L); \
} \
static int listisLast_##type(derivedList_##type L) \
{ \
  return listIsLast((derivedList)L);  \
} \
static derivedPosition_##type listFind_##type(type X, derivedList_##type L)  \
{ \
  return (derivedPosition_##type)listFind((elementType)X, (derivedList)L);  \
} \
static derivedPosition_##type listFindPre_##type(type X, derivedList_##type L) \
{ \
  return (derivedPosition_##type)listFindPre((elementType)X, (derivedList)L); \
} \
static void listDeleteEntry_##type(type X, derivedList_##type L)  \
{ \
  listDeleteEntry((elementType)X, (derivedList)L);  \
} \
static void listInsert_##type(type X, derivedList_##type L) \
{ \
  listInsert((elementType)X, (derivedList)L); \
} \
static void listDeleteList_##type(derivedList_##type L) \
{ \
  listDeleteList((derivedList)L); \
} \
static derivedPosition_##type listFirst_##type(derivedlist_##type L)  \
{ \
  return (derivedPosition_##type)listFirst((derivedList)L); \
} \
static type listRetrive_##type(derivedPosition_##type P)  \
{ \
  return (type)listRetrive((derivedPosition)P); \
}



/* standard single chain primitive list declaration */
#define DEFINE_PRIMLIST_OF(type)  \
LIST_OF(type) { \
  type          element; \
  LIST_OF(type) *next;  \
};  \
typedef LIST_OF(type) primNode_##type;  \
typedef LIST_OF(type) *primList_##type; \
typedef LIST_OF(type) *primPosition_##type; \
static primList_##type listCreateList_##type() \
{ \
  primList_##type l;  \
  if ((l = malloc(sizeof(primNode_##type))) != NULL) {  \
    l->element = (type)0; \
    l->next    = NULL;  \
    return l; \
  } \
  err_msg("error: malloc error"); \
  return NULL; \
} \
static primList_##type listMakeEmpty_##type(primList_##type L) \
{ \
  primPosition_##type pcur, pnxt; \
  pcur = L->next; \
  while (pcur) {  \
    pnxt = pcur->next;  \
    free(pcur); \
    pcur = pnxt;  \
  } \
  L->next = NULL; \
  return L; \
} \
static void listDeleteList_##type(primList_##type L)  \
{ \
  primPosition_##type pcur, pnxt; \
  pcur = L->next; \
  while (pcur) {  \
    pnxt = pcur->next;  \
    free(pcur); \
    pcur = pnxt;  \
  } \
  L->next = NULL; \
  free(L);  \
} \
static int listIsEmpty_##type(primList_##type L)  \
{ \
  return L->next == NULL; \
} \
static int listIsLast_##type(primPosition_##type P, primList_##type L)  \
{ \
  return P->next == NULL; \
} \
static primPosition_##type listFind_##type(type X, primList_##type L) \
{ \
  primPosition_##type p;  \
  p = L->next;  \
  while (p && p->element != X) \
    p = p->next;  \
  return p; \
} \
static primPosition_##type listFindPre_##type(type X, primList_##type L)  \
{ \
  primPosition_##type p;  \
  p = L;  \
  while (p->next && p->next->element != X) \
    p = p->next;  \
  return p; \
} \
static void listDeleteEntry_##type(type X, primList_##type L) \
{ \
  primList_##type pcur, ppre; \
  ppre = listFindPre_##type(X, L); \
  if (ppre) { \
    pcur = ppre->next;  \
    ppre->next = pcur->next;  \
    free(pcur); \
  } \
} \
static void listInsert_##type(type X, primList_##type L, primPosition_##type P) \
{ \
  primNode_##type *nnode; \
  if ((nnode = malloc(sizeof(primNode_##type))) == NULL)  \
    err_sys("error: malloc error"); \
  nnode->element = X; \
  nnode->next = P->next;  \
  P->next = nnode;  \
} \
static primPosition_##type listFirst_##type(primList_##type L)  \
{ \
  return L->next; \
} \
static type listRetrive_##type(primPosition_##type P) \
{ \
  return P->element;  \
}





#ifdef __cpluscplus
}
#endif

#endif      /* PRIMITIVELIST_H */