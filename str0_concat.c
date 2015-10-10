/*
%use str0_free;
%use str0_length;
%use limitmalloc_open;
*/

#include "str0.h"

int str0_concat(str0 *s,limitmalloc_pool *pool,str0 *t)
{
  int64 slen = str0_length(s);
  int64 tlen = str0_length(t);
  int64 i;
  char *x = limitmalloc_open(pool,slen + tlen + 1);

  if (!x) return 0;

  for (i = 0;i < slen;++i) x[i] = (*s)[i];
  for (i = 0;i < tlen;++i) x[slen + i] = (*t)[i];
  x[slen + tlen] = 0;

  str0_free(s,pool);
  *s = x;
  return 1;
}
