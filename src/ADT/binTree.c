#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binTree.h"

SearchTree treeMakeEmpty_p(SearchTree T);
TreePosition treeFind_p(PrimitiveType X, SearchTree T);
TreePosition treeFindMin_p(SearchTree T);
TreePosition treeFindMax_p(SearchTree T);
SearchTree treeInsert_p(PrimitiveType X, SearchTree T);
SearchTree treeDeleteNode_p(PrimitiveType X, SearchTree T);
PrimitiveType treeRetrive_p(TreePosition P);

/**
 * Create or Clean a empty tree
 * input:   T
 * output:  T
 * return:  NULL
 */
SearchTree treeMakeEmpty_p(SearchTree T)
{
  if (T) {
    treeMakeEmpty_p(T->left);
    treeMakeEmpty_p(T->right);
    free(T);
  }
  return NULL;
}

/* Find X's position in the tree */
TreePosition treeFind_p(PrimitiveType X, SearchTree T)
{
  while (T) {
    if (X == T->element)
      break;
    if (X < T->element)
      T = T->left;
    else
      T = T->right;
  }
  return T;
}

/* Find the min element's position in the tree */
TreePosition treeFindMin_p(SearchTree T)
{
  if (T)
    while (T->left)
      T = T->left;
  return T;
}

/* Find the max element's position in the tree */
TreePosition treeFindMax_p(SearchTree T)
{
  if (T) 
    while (T->right)
      T = T->right;
  return T;
}

/* Find X's position in the tree, insert X if not found, ret root */
SearchTree treeInsert_p(PrimitiveType X, SearchTree T)
{
  TreePosition t, tparent;
  int tdirec;

  if (T == NULL) {
    T = Malloc(sizeof(TreeNode));
    T->element = X;
    T->left = T->right = NULL;
  }
  t = T;
  while (t) {
    if (X == t->element)
      return T;   /* return here so tparent initied pass here */
    tparent = t;
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
    tparent->right->left = tparent->right->right = NULL;
  } else {
    tparent->left = Malloc(sizeof(TreeNode));
    tparent->left->element = X;
    tparent->left->left = tparent->left->right = NULL;
  }
  return T;
}

SearchTree treeDeleteNode_p3(PrimitiveType X, SearchTree T)
{
  SearchTree ttmp, nt;

  nt = T;
  while (T) {
    if (X == T->element)
      break;
    if (X < T->element)
      T = T->left;
    else
      T = T->right;
  }

  if (T != NULL) {
    if (T->left && T->right) {
      if (T->right->left != NULL) {
        while (T->right->left->left)
          T->right = T->right->left;
        T->element = T->right->left->element;
        ttmp = T->right->left;
        T->right->left = T->right->left->right;
        free(ttmp);
      } else {
        ttmp = T->right;
        T->right = T->right->right;
        free(ttmp);
      }
    } else if (T->left == NULL) {
      if ((ttmp = T->right) != NULL) {
        memcpy(T, ttmp, sizeof(*T));
        T->right = NULL;
      } else {
        ttmp = T;
        T = NULL;
      }
      free(ttmp);
    } else if (T->right == NULL) {
      if ((ttmp = T->left) != NULL) {
        memcpy(T, ttmp, sizeof(*T));
        T->left = NULL;
      } else {
        ttmp = T;
        T = NULL;
      }
      free(ttmp);
    }
  } else
    err_msg("error: element not found");

  return nt;
}

SearchTree treeDeleteNode_p(PrimitiveType X, SearchTree T)
{
  TreePosition tmpCell;

  if (T) {
    if (X < T->element)
      T->left = treeDeleteNode_p(X, T->left);
    else if (X > T->element)
      T->right = treeDeleteNode_p(X, T->right);
    else if (T->left && T->right) {
      tmpCell = treeFindMin_p(T->right);
      T->element = tmpCell->element;
      T->right = treeDeleteNode_p(T->element, T->right);
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

/* Tree delete node rutine with light recrusive implement */
SearchTree treeDeleteNode_p2(PrimitiveType X, SearchTree T)
{
  TreePosition tmpCell, nt, tparent;
  int tdirec;

  nt = T;   /* Save original value of root */
  tparent = NULL; /* Tracking parent node */
  while (T) {
    if (X == T->element)
      break;
    tparent = T;
    if (X < T->element) {
      T = T->left;
      tdirec = 0;
    }
    else {
      T = T->right;
      tdirec = 1;
    }
  }
  if (T) {    /* X was found */
    if (T->left && T->right) {  /* have both left and right child */
      tmpCell = treeFindMin_p(T->right);
      T->element = tmpCell->element;
      T->right = treeDeleteNode_p2(T->element, T->right); /* crusive only executed once at most */
    } else if (tparent && tdirec == 0) {  /* have one or 0 child */
      tmpCell = T;
      if (T->left == NULL) {
        tparent->left = T->right;
      } else if (T->right == NULL) {
        tparent->left = T->left;
      }
      free(tmpCell);
    } else if (tparent && tdirec == 1) {
      tmpCell = T;
      if (T->left == NULL) {
        tparent->right = T->right;
      } else if (T->right == NULL) {
        tparent->right = T->left;
      }
      free(tmpCell);
    } else {    /* 0 child, so root should be changed */
      tmpCell = T;
      if (T->left == NULL)
        nt = T->right;    /* change root */
      else if (T->right == NULL)
        nt = T->left;
      free(tmpCell);
    }
  } else
    err_msg("error: element not found");
  return nt;  /* new root */
}

PrimitiveType treeRetrive_p(TreePosition P)
{
  if (P == NULL)
    err_quit("fatal error: Null pointer");
  return P->element;
}