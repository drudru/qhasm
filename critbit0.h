#ifndef CRITBIT0_H
#define CRITBIT0_H

#include "str0.h"
#include "uint32.h"
#include "uint8.h"
#include "limitmalloc.h"

typedef struct {
  void *critbit0_private_child[2];
  uint32 critbit0_private_byte;
  uint8 critbit0_private_otherbits;
} critbit0_node;

typedef struct {
  void *critbit0_private_root;
} critbit0_tree;

extern int critbit0_contains(critbit0_tree *,str0 *);
extern int critbit0_insert(critbit0_tree *,limitmalloc_pool *,str0 *);
extern int critbit0_delete(critbit0_tree *,limitmalloc_pool *,str0 *);
extern void critbit0_clear(critbit0_tree *,limitmalloc_pool *);
extern int critbit0_firstprefixed(critbit0_tree *,limitmalloc_pool *,str0 *,str0 *);
extern int critbit0_allprefixed(critbit0_tree *,limitmalloc_pool *,str0 *,str0 *,int (*)(void));

#endif
