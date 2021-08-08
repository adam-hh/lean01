#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binTree.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
static inline int Height(AvlPosition P)
{
  if (P == NULL)
    return -1;
  return P->height;
}

/* LL rotate */
static inline AvlPosition singleRotateWithLeft(AvlPosition K2)
{
  AvlPosition K1;

  K1 = K2->left;
  K2->left = K1->right;
  K1->right = K2;

  K2->height = MAX(Height(K2->left), Height(K2->right)) + 1;
  K1->height = MAX(Height(K1->left), K2->height) + 1;
  return K1;
}

/* RR rotate */
static inline AvlPosition singleRotateWithRight(AvlPosition K1)
{
  AvlPosition K2;

  K2 = K1->right;
  K1->right = K2->left;
  K2->left = K1;

  K1->height = MAX(Height(K1->left), Height(K1->right)) + 1;
  K2->height = MAX(K1->height, Height(K2->right)) + 1;
  return K2;
}

/* LR rotate */
static inline AvlPosition doubleRotateWithLeft(AvlPosition K3)
{
  K3->left = singleRotateWithRight(K3->left);
  return singleRotateWithLeft(K3);
}

/* RL rotate */
static inline AvlPosition doubleRotateWithRight(AvlPosition K1)
{
  K1->right = singleRotateWithLeft(K1->right);
  return singleRotateWithRight(K1);
}

static AvlTree avlInsertRebanlance_p(PrimitiveType X, AvlTree T)
{
  if (X == T->element)
    return T;
  if (X < T->element) {
    T->left = avlInsertRebanlance_p(X, T->left);
    if (Height(T->left) - Height(T->right) == 2) {
      if (X < T->left->element) 
        T = singleRotateWithLeft(T);
      else
        T = doubleRotateWithLeft(T);
    }
  } else {
    T->right = avlInsertRebanlance_p(X, T->right);
    if (Height(T->right) - Height(T->left) == 2) {
      if (X > T->right->element)
        T = singleRotateWithRight(T);
      else
        T = doubleRotateWithRight(T);
    }
  }

  T->height = MAX(Height(T->left), Height(T->right)) + 1;
  return T;
}

AvlTree avlMakeEmpty_p(AvlTree T)
{
  if (T) {
    avlMakeEmpty_p(T->left);
    avlMakeEmpty_p(T->right);
    free(T);
  }
  return NULL;
}

AvlPosition avlFind_p(PrimitiveType X, AvlTree T)
{
  while (T) {
    if (X == T->element)
      return T;
    if (X < T->element)
      T = T->left;
    else
      T = T->right;
  }
  return T;
}

AvlPosition avlFindMin_p(AvlTree T)
{
  if (T)
    while (T->left)
      T = T->left;
  return T;
}

AvlPosition avlFindMax_p(AvlTree T)
{
  if (T)
    while (T->right)
      T = T->right;
  return T;
}

PrimitiveType avlRetrive_p(AvlPosition P)
{
  if (P == NULL)
    err_quit("fatal error: Null pointer");
  return P->element;
}

/* Insertion full recrusive */
AvlTree avlInsert_p(PrimitiveType X, AvlTree T)
{
  if (T) {
    if (X < T->element) {
      T->left = avlInsert_p(X, T->left);
      if (Height(T->left) - Height(T->right) == 2) {
        if (X < T->left->element)
          T = singleRotateWithLeft(T);
        else
          T = doubleRotateWithLeft(T);
      }
    } else if (X > T->element) {
      T->right = avlInsert_p(X, T->right);
      if (Height(T->right) - Height(T->left) == 2) {
        if (X > T->right->element)
          T = singleRotateWithRight(T);
        else
          T = doubleRotateWithRight(T);
      }
    } else
      return T;
    T->height = MAX(Height(T->left), Height(T->right)) + 1;
  } else {
    T = Malloc(sizeof(AvlNode));
    T->element = X;
    T->height = 0;
    T->left = T->right = NULL;
  }
  return T;
}

/* Insertion partitial recrusive */
AvlTree avlInsert_p2(PrimitiveType X, AvlTree T)
{
  AvlPosition t, tparent;
  int tdirec;

  if (T) {
    t = T;
    while (t) {
      if (X == t->element)
        return T;
      tparent = t;  /* Init tparent */
      if (X < t->element) {
        t = t->left;
        tdirec = 0;
      }
      else {
        t = t->right;
        tdirec = 1;
      }
    }
    if (tdirec) {
      tparent->right = Malloc(sizeof(TreeNode));
      tparent->right->element = X;
      tparent->right->height = 0;
      tparent->right->left = tparent->right->right = NULL;
      if (tparent->left == NULL)
        T = avlInsertRebanlance_p(X, T);
    } else {
      tparent->left = Malloc(sizeof(TreeNode));
      tparent->left->element = X;
      tparent->left->height = 0;
      tparent->left->left = tparent->left->right = NULL;
      if (tparent->right == NULL)
        T = avlInsertRebanlance_p(X, T);
    }
  } else {
    T = Malloc(sizeof(TreeNode));
    T->element = X;
    T->height = 0;
    T->left = T->right = NULL;
  }

  return T;
}

/* Deletion full recrusive */
AvlTree avlDeleteNode_p(PrimitiveType X, AvlTree T)
{
  AvlPosition tmpCell;

  if (T) {
    if (X < T->element) {
      T->left = avlDeleteNode_p(X, T->left);
      if (T->left)
        T->left->height = MAX(Height(T->left->left), Height(T->left->right)) + 1;
      T->height = MAX(Height(T->left), Height(T->right)) + 1;
      if (Height(T->right) - Height(T->left) == 2) {
        if (Height(T->right->left) > Height(T->right->right))
          T = doubleRotateWithRight(T);
        else
          T = singleRotateWithRight(T);
      }
    } else if (X > T->element) {
      T->right = avlDeleteNode_p(X, T->right);
      if (T->right)
        T->right->height = MAX(Height(T->right->left), Height(T->right->right)) + 1;
      T->height = MAX(Height(T->left), Height(T->right)) + 1;
      if (Height(T->left) - Height(T->right) == 2) {
        if (Height(T->left->right) > Height(T->left->left))
          T = doubleRotateWithLeft(T);
        else
          T = singleRotateWithLeft(T);
      }
    } else if (T->left && T->right) {
      tmpCell = avlFindMin_p(T->right);
      T->element = tmpCell->element;
      T->right = avlDeleteNode_p(T->element, T->right);
      if (T->right)
        T->right->height = MAX(Height(T->right->left), Height(T->right->right)) + 1;
      T->height = MAX(Height(T->left), Height(T->right)) + 1;
      if (Height(T->left) - Height(T->right) == 2) {
        if (Height(T->left->right) > Height(T->left->left))
          T = doubleRotateWithLeft(T);
        else
          T = singleRotateWithLeft(T);
      }
    } else {
      tmpCell = T;
      if (T->left == NULL)
        T = T->right;
      else if (T->right == NULL)
        T = T->left;
      free(tmpCell);
    }
  } else
    err_msg("error: element not found");
  return T;
}