#ifndef BINTREE_H
#define BINTREE_H
#include "singleList.h"

#ifdef __cpluscplus
extern "C" {
#endif

#ifndef PrimitiveType
#define PrimitiveType char
#endif

struct primitiveBinTree {
  PrimitiveType             element;
  struct primitiveBinTree   *left;
  struct primitiveBinTree   *right;
};
typedef struct primitiveBinTree TreeNode;
typedef struct primitiveBinTree *SearchTree;
typedef struct primitiveBinTree *TreePosition;

struct primitiveAvlTree {
  PrimitiveType             element;
  int                       height;
  struct primitiveAvlTree   *left;
  struct primitiveAvlTree   *right;
};
typedef struct primitiveAvlTree AvlNode;
typedef struct primitiveAvlTree *AvlTree;
typedef struct primitiveAvlTree *AvlPosition;


SearchTree treeMakeEmpty_p(SearchTree T);
TreePosition treeFind_p(PrimitiveType X, SearchTree T);
TreePosition treeFindMin_p(SearchTree T);
TreePosition treeFindMax_p(SearchTree T);
SearchTree treeInsert_p(PrimitiveType X, SearchTree T);
SearchTree treeDeleteNode_p(PrimitiveType X, SearchTree T);
PrimitiveType treeRetrive_p(TreePosition P);

AvlTree avlMakeEmpty_p(AvlTree T);
AvlPosition avlFind_p(PrimitiveType X, AvlTree T);
AvlPosition avlFindMin_p(AvlTree T);
AvlPosition avlFindMax_p(AvlTree T);
AvlTree avlInsert_p(PrimitiveType X, AvlTree T);
AvlTree avlDeleteNode_p(PrimitiveType X, AvlTree T);
PrimitiveType avlRetrive_p(AvlPosition P);


#ifdef __cpluscplus
}
#endif

#endif    /* BINTREE_H */