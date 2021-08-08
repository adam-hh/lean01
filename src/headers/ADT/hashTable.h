#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "singleList.h"

#ifdef __cpluscplus
extern "C" {
#endif

typedef uint32_t Index;

#define MinTableSize 15

typedef struct chainList CListNode;
typedef struct chainList *CList;
typedef struct chainList *CListPosition;
typedef struct hashTable *HashTable;

int nextPrime(int n);
#define strCmp(a, b)  ((strlen(a) == strlen(b) && strcmp(a, b) == 0) ? 1 : 0)

HashTable htInitializeTable_p(int tableSize);
void htDestroyTable_p(HashTable H);
CListPosition htFind_p(char *Key, HashTable H);
void htInsert_p(char *Key, HashTable H);
char *htRestrive_p(CListPosition P);
HashTable htMakeEmpty_p(HashTable H);
void htDeleteNode_p(char *Key, HashTable H);

HashTable oahtInitializaTable_p(int tableSize);
void oahtDestroyTable_p(HashTable H);
Index oahtFind_p(char *key, HashTable H);
void oahtInsert_p(char *key, HashTable H);
char *oahtRetrieve(Index P, HashTable H);
HashTable oahtRehash(HashTable H);
HashTable oahtMakeEmpty_p(HashTable H);
void oahtDeleteNode_p(char *Key, HashTable H);


#ifdef __cpluscplus
}
#endif

#endif    /* HASHTABLE_H */