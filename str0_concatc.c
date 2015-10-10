/*
%use str0_free;
%use str0_length;
%use limitmalloc_open;
*/

#include "str0.h"

int str0_concatc(str0 *s,limitmalloc_pool *pool,const char *y)
{
  int64 slen;
  int64 tlen;
  int64 i;
  char *x;

  slen = str0_length(s);
  tlen = 0;
  if (y) while (y[tlen]) ++tlen;
  x = limitmalloc_open(pool,slen + tlen + 1);

  if (!x) return 0;

  for (i = 0;i < slen;++i) x[i] = (*s)[i];
  for (i = 0;i < tlen;++i) x[slen + i] = y[i];
  x[slen + tlen] = 0;

  str0_free(s,pool);
  *s = x;
  return 1;
}
