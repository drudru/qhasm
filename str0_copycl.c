/*
%use str0_free;
%use limitmalloc_open;
*/

#include "str0.h"

int str0_copycl(str0 *s,limitmalloc_pool *pool,const char *y,int64 limit)
{
  char *x;
  int64 length;

  length = 0;
  if (y) while ((length < limit) && y[length]) ++length;

  x = limitmalloc_open(pool,length + 1);
  if (!x) return 0;

  if (*s) str0_free(s,pool);
  *s = x;
  while (length > 0) { *x++ = *y++; --length; }
  *x = 0;

  return 1;
}
