/*
%use str0_free;
%use str0_length;
%use limitmalloc_open;
*/

#include "str0.h"

int str0_copy(str0 *s,limitmalloc_pool *pool,str0 *t)
{
  char *x;
  int64 length;
  char *y;

  if (*s) str0_free(s,pool);
  length = str0_length(t);
  x = limitmalloc_open(pool,length + 1);
  if (!x) return 0;

  *s = x;
  y = *t;
  while (length > 0) { *x++ = *y++; --length; }
  *x = 0;

  return 1;
}
