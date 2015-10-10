/*
%use limitmalloc_open;
%use str0_free;
*/

#include "str0.h"

int str0_uint64(str0 *s,limitmalloc_pool *pool,uint64 u)
{
  char *x;
  int64 len;
  uint64 tmp;

  if (*s) str0_free(s,pool);

  len = 0;
  tmp = u;
  do { ++len; tmp /= 10; } while (tmp > 0);

  x = limitmalloc_open(pool,len + 1);
  if (!x) return 0;

  *s = x;
  while (len > 0) {
    --len;
    x[len] = '0' + (u % 10);
    u /= 10;
  }
  return 1;
}
