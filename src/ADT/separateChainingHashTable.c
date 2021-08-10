#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashTable.h"

struct chainList {
  char              *key;
  struct chainList  *next;
};

struct hashTable {
  int tableSize;
  CList *lists;
};

static inline Index Hash(const char *key, int tableSize)
{
  Index hashVal = 0;

  while (*key)
    hashVal = (hashVal << 5) + *key++;
  return hashVal % tableSize;
}

/* Generate a next prime greater then n */
int nextPrime(int n)
{
  int i, sqroot;
  if ((n & 0x01) == 0)
    n++;
  
  while (1) {
    sqroot = (int)sqrt(n);
    for (i = 3; i < sqroot; i += 2) {
      if (n % i == 0)
        break;
    }
    if (i >= sqroot)
      return n;
    else
      n += 2;
  }
}

HashTable htInitializeTable_p(int tableSize)
{
  HashTable H;

  if (tableSize > MinTableSize) {
    H = Malloc(sizeof(*H));
    H->tableSize = nextPrime(tableSize);
    H->lists = Malloc(H->tableSize * sizeof(CList));
    memset(H->lists, 0, H->tableSize * sizeof(CList));
  } else {
    err_msg("error: Table size too small");
    return NULL;
  }

  return H;
}

CListPosition htFind_p(char *key, HashTable H)
{
  CListPosition p;

  p = H->lists[Hash(key, H->tableSize)];
  while (p && strcmp(p->key, key) != 0)
    p = p->next;
  return p;
}

void htInsert_p(char *key, HashTable H)
{
  CListPosition np;
  Index idx;

  if (htFind_p(key, H) == NULL) {
    idx = Hash(key, H->tableSize);
    np = Malloc(sizeof(CListNode));
    np->key = key;
    np->next = H->lists[idx];
    H->lists[idx] = np;
  } else
    err_msg("error: %s already exist", key);
}

void htDestroyTable_p(HashTable H)
{
  Index i;
  CList lcur, lnxt;

  if (H) {
    for (i = 0; i < H->tableSize; i++) {
      lcur = H->lists[i];
      while (lcur) {
        lnxt = lcur->next;
        free(lcur);
        lcur = lnxt;
      }
    }
    free(H);
  }
  else
    err_msg("error: Null pointer");
}

char *htRestrive_p(CListPosition P)
{
  if (P)
    return P->key;
  return NULL;
}

HashTable htMakeEmpty_p(HashTable H)
{
  Index i;
  CList lcur, lnxt;

  if (H)
    for (i = 0; i < H->tableSize; i++) {
      lcur = H->lists[i];
      while (lcur) {
        lnxt = lcur->next;
        free(lcur);
        lcur = lnxt;
      }
    }
  else
    err_msg("error: Null pointer");
  return H;
}

void htDeleteNode_p(char *Key, HashTable H)
{
  CListPosition lcur, lpre;
  Index idx;

  idx = Hash(Key, H->tableSize);
  lcur = lpre = H->lists[idx];
  if (lcur) {
    while (lcur && strcmp(Key, lcur->key) != 0) {
      lpre = lcur;
      lcur = lcur->next;
    }
    if (lcur == lpre) {
      free(lcur);
      H->lists[idx] = NULL;
    } else if (lcur != NULL) {
      lpre->next = lcur->next;
      free(lcur);
    } else
      goto end;
  } else {
    end:
    err_msg("error: %s not exist", Key);
  }
}