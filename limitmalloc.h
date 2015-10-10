#ifndef LIMITMALLOC_H
#define LIMITMALLOC_H

#include "int64.h"

#define LIMITMALLOC_MAX 9223372036854775807LL

typedef struct limitmalloc_pool {
  int64 limitmalloc_private_limit;
  struct limitmalloc_pool *limitmalloc_private_via;
  int64 limitmalloc_private_allocated;
} limitmalloc_pool;

extern void *limitmalloc_open(limitmalloc_pool *,int64);
extern void *limitmalloc_open_if2(limitmalloc_pool *,int64);

extern void limitmalloc_close(limitmalloc_pool *,int64,void *);

#endif
