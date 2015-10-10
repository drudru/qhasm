/*
%use str0_length;
%use limitmalloc_close;
*/

#include "str0.h"

void str0_free(str0 *s,limitmalloc_pool *pool)
{
  if (*s) {
    limitmalloc_close(pool,str0_length(s) + 1,*s);
    *s = 0;
  }
}
