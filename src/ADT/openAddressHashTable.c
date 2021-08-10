#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashTable.h"

#define MinTableSize 15

enum KindOfEntry {EMPTY = 0, LEGITIMATE, DELETED};

struct hashEntry {
  char              *key;
  enum KindOfEntry  info;
};
typedef struct hashEntry Cell;
struct hashTable {
  int     tableSize;
  Cell    *cells;
};

static inline Index Hash(const char *key, int tableSize)
{
  Index hashVal = 0;

  while (*key)
    hashVal = (hashVal << 5) + *key++;
  return hashVal % tableSize;
}

HashTable oahtInitializaTable_p(int tableSize)
{
  HashTable H;

  if (tableSize > MinTableSize) {
    H = Malloc(sizeof(*H));
    H->tableSize = nextPrime(tableSize);
    H->cells = Malloc(H->tableSize * sizeof(Cell));
    memset(H->cells, 0, H->tableSize * sizeof(Cell));
  } else {
    err_msg("error: Table size too small");
    return NULL;
  }

  return H;
}

void oahtDestroyTable_p(HashTable H)
{
  if (H) {
    if (H->cells)
      free(H->cells);
    free(H);
  } else
    err_msg("error: Null pointer");
}

Index oahtFind_p(char *key, HashTable H)
{
  Index cur;
  int collisionNum;

  collisionNum = 0;
  cur = Hash(key, H->tableSize);
  while (H->cells[cur].info != EMPTY && strcmp(key, H->cells[cur].key) != 0) {
    cur += ((++collisionNum) << 1) - 1;   /* may overflow */
    if (cur >= H->tableSize)
      cur -= H->tableSize;
  }
  return cur;
}

void oahtInsert_p(char *key, HashTable H)
{
  Index idx;

  idx = oahtFind_p(key, H);
  if (H->cells[idx].info != LEGITIMATE) {
    H->cells[idx].info = LEGITIMATE;
    H->cells[idx].key = key;
  }
}

char *oahtRetrieve(Index P, HashTable H)
{
  if (H && P < H->tableSize) {
    if (H->cells[P].info == LEGITIMATE)
      return H->cells[P].key;
  }
  return NULL;
}

HashTable oahtMakeEmpty_p(HashTable H)
{
  if (H) {
    if (H->cells)
      free(H->cells);
  } else
    err_msg("error: Null pointer");
  return H;
}

void oahtDeleteNode_p(char *Key, HashTable H)
{
  Index idx;

  idx = oahtFind_p(Key, H);
  if (H->cells[idx].info == LEGITIMATE)
    H->cells[idx].info = DELETED;
  else
    err_msg("error: %s not exist", Key);
}

HashTable oahtRehash(HashTable H)
{
  int oldSize;
  Cell *oldCells;

  if (H) {
    oldCells = H->cells;
    oldSize = H->tableSize;

    H = oahtInitializaTable_p(oldSize << 1);
    for (int i = 0; i < oldSize; i++)
      if (oldCells[i].info == LEGITIMATE)
        oahtInsert_p(oldCells[i].key, H);
    free(oldCells);
  } else
    err_msg("error: Null pointer");
  return H;
}