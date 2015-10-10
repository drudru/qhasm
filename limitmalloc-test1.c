/*
%use limitmalloc_open;
%use limitmalloc_close;
*/

#include <stdio.h>
#include "limitmalloc.h"

limitmalloc_pool t[7] = {
  { LIMITMALLOC_MAX }
, { 100 }
, { 50, &t[1] }
, { 50, &t[1] }
, { 50, &t[1] }
, { 100, &t[2] }
, { 30, &t[5] }
} ;

int main(int argc,char **argv)
{
  void *p;
  void *q;
  int64 size;
  int64 pool;

  for (size = 1;size <= 256;size += size) {
    p = limitmalloc_open(0,size);
    printf("open nopool %ld: %s\n",(long) size,p ? "ok" : "nomem");
    limitmalloc_close(0,size,p);
    for (pool = 0;pool < 7;++pool) {
      p = limitmalloc_open(&t[pool],size);
      printf("open t%ld %ld: %s\n",(long) pool,(long) size,p ? "ok" : "nomem");
      limitmalloc_close(&t[pool],size,p);
    }
  }

  q = limitmalloc_open(&t[6],25);
  printf("keeping open t6 25: %s\n",q ? "ok" : "nomem");

  for (size = 1;size <= 256;size += size) {
    p = limitmalloc_open(0,size);
    printf("open nopool %ld: %s\n",(long) size,p ? "ok" : "nomem");
    limitmalloc_close(0,size,p);
    for (pool = 0;pool < 7;++pool) {
      p = limitmalloc_open(&t[pool],size);
      printf("open t%ld %ld: %s\n",(long) pool,(long) size,p ? "ok" : "nomem");
      limitmalloc_close(&t[pool],size,p);
    }
  }

  return 0;
}
