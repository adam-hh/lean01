#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "singleList.h"

/**
 * Single chain list
 */
struct listSt {
  void            *element; /* as a derived type list, it can be cast to void pointer */
  listEleCmpFunc  cmp;      /* list elements comparation funct */
  struct listSt   *next;
};

/* Create a new list, actually make a header */
derivedList listCreateList(listEleCmpFunc cmp)
{
  derivedList l;

  if ((l = malloc(sizeof(derivedNode))) != NULL) {
    l->element = NULL;
    l->next = NULL;
    l->cmp = cmp;
    return l;
  }
  err_msg("error: malloc error");
  return NULL;
}

/* Clear all entries (exclude header) */
derivedList listMakeEmpty(derivedList L)
{
  derivedPosition pcur, pnxt;

  pcur = L->next;
  while (pcur) {
    pnxt = pcur->next;
    free(pcur->element);
    free(pcur);
    pcur = pnxt;
  }
  L->next = NULL;
  return L;
}

/* Clear then free the whole list */
void listDeleteList(derivedList L)
{
  derivedPosition pcur, pnxt;

  pcur = L->next;
  while (pcur) {
    pnxt = pcur->next;
    free(pcur->element);
    free(pcur);
    pcur = pnxt;
  }
  L->next = NULL;
  free(L);
}

/* test empty */
int listIsEmpty(derivedList L)
{
  return L->next == NULL;
}
int listIsLast(derivedPosition P, derivedList L)
{
  return P->next == NULL;
}

/* search x return loc of x */
derivedPosition listFind(elementType X, derivedList L)
{
  derivedPosition p;

  p = L->next;
  while (p && L->cmp(p->element, X) != 0)
    p = p->next;
  return p;
}
/* search x return loc of x pre */
derivedPosition listFindPre(elementType X, derivedList L)
{
  derivedPosition p;

  p = L;
  while (p->next && L->cmp(p->next->element, X) != 0)
    p = p->next;
  return p;
}

/* seach the first entry matchs X then delete it */
void listDeleteEntry(elementType X, derivedList L)
{
  derivedList pcur, ppre;

  ppre = listFindPre(X, L);
  if (ppre) {
    pcur = ppre->next;
    ppre->next = pcur->next;
    free(pcur->element);
    free(pcur);
  }
}

/* Insert a new node after position P */
void listInsert(elementType X, derivedList L, derivedPosition P)
{
  derivedNode *nnode;

  if ((nnode = malloc(sizeof(derivedNode))) == NULL)
    err_sys("error: malloc error");
  nnode->element = X;   /* Note, x should be allocated with malloc before insert call */
  nnode->next = P->next;
  P->next = nnode;
}

derivedPosition listFirst(derivedList L)
{
  return L->next;
}
elementType listRetrive(derivedPosition P)
{
  return P->element;
}