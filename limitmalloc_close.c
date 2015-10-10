#include <stdlib.h>
#include "limitmalloc.h"
#define limit limitmalloc_private_limit
#define via limitmalloc_private_via
#define allocated limitmalloc_private_allocated

void limitmalloc_close(limitmalloc_pool *p,int64 len,void *x)
{
  if (!x) return;
  free(x);

  if (len < 0) return;
  if (len == 0) len = 1;
  if (len > 1073741823) return;

  for (;p;p = p->via)
    if (p->allocated >= len) /* otherwise caller screwed up */
      p->allocated -= len;
}
