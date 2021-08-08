#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "binTree.h"
#include "hashTable.h"
#include "pqueue.h"

SearchTree treeDeleteNode_p2(PrimitiveType X, SearchTree T);
SearchTree treeDeleteNode_p3(PrimitiveType X, SearchTree T);
AvlTree avlInsert_p2(PrimitiveType X, AvlTree T);

/* Midorder traversal */
static void midTravelTree(SearchTree t, int depth)
{
  if (t) {
    for (int i = 0; i < depth; i++)
      putchar(' ');
    printf("|%4d|\n", t->element);
    midTravelTree(t->left, depth + 6);
    midTravelTree(t->right, depth + 6);
  }
}

#define LGAP 10
/* Postorder traversal */
static void postTravelTree(SearchTree t, int depth)
{
  if (t) {
    depth += LGAP;
    postTravelTree(t->right, depth);
    for (int i = LGAP; i < depth; i++) {
      if (i < depth - 3)
        putchar(' ');
      else if (i == depth - 3)
        putchar('|');
      else
        putchar('-');
    }
    printf("$%4d|\n", t->element);
    postTravelTree(t->left, depth);
  } else
    return;
}

static void postTravelAvl(AvlTree t, int depth)
{
  if (t) {
    depth += LGAP;
    postTravelAvl(t->right, depth);
    for (int i = LGAP; i < depth; i++) {
      if (i < depth - 3)
        putchar(' ');
      else if (i == depth - 3)
        putchar('|');
      else
        putchar('-');
    }
    printf("$%4d:%d|\n", t->element, t->height);
    postTravelAvl(t->left, depth);
  } else
    return;
}

int binTreeDemo(int argc, char *argv[])
{
  int fdrandom;
  char usrIn[16];
  SearchTree t;
  AvlTree avlt;
  PrimitiveType c;

  if ((fdrandom = open("/dev/urandom", O_RDONLY)) == -1)
    err_sys("fatal error: open /dev/urandom fail");
  //t = treeInsert_p(0, NULL);
  avlt = avlInsert_p2(0, NULL);
  for (int i = 0; i < 20; i++) {
    read(fdrandom, &c, sizeof(c));
    //treeInsert_p(c, t);
    avlt = avlInsert_p2(c, avlt);
  }
  //postTravelTree(t, 0);
  postTravelAvl(avlt, 0);
  while (1) {
    fgets(usrIn, sizeof(usrIn), stdin);
    c = atoi(usrIn);
    printf("expect %d\n", c);
    printf("--------------------------------------------------------------------------\n");
    //t = treeDeleteNode_p2(c, t);
    avlt = avlDeleteNode_p(c, avlt);
    postTravelAvl(avlt, 0);
  }
}

int main(int argc, char *argv[])
{
  int fdrandom;
  char usr[200];

  if ((fdrandom = open("/dev/urandom", O_RDONLY)) == -1)
    err_sys("fatal error: open /dev/urandom fail");
  read(fdrandom, usr, sizeof(usr));
  printf("origin list:\n");
  for (int i = 0; i < sizeof(usr); i++)
    printf("%4d%c", usr[i], (i + 1) % 20 ? ',' : '\n');
  heapSort(usr, sizeof(usr));
  printf("sorted list:\n");
  for (int i = 0; i < sizeof(usr); i++)
    printf("%4d%c", usr[i], (i + 1) % 20 ? ',' : '\n');

  exit(0);
}