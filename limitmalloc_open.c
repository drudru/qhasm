#include <stdlib.h>
#include <errno.h>
#include "limitmalloc.h"
#define limit limitmalloc_private_limit
#define via limitmalloc_private_via
#define allocated limitmalloc_private_allocated

void *limitmalloc_open(limitmalloc_pool *p,int64 len)
{
  void *result;
  limitmalloc_pool *q;

  if (len < 0) { errno = ENOMEM; return 0; }
  if (len == 0) len = 1; /* protect bad allocators */
  if (len > 1073741823) { errno = ENOMEM; return 0; } /* protect bad allocators */

  for (q = p;q;q = q->via)
    if (len > q->limit - q->allocated) { errno = ENOMEM; return 0; }

  result = calloc(1,len);
  if (!result) return 0;

  for (q = p;q;q = q->via)
    q->allocated += len;

  return result;
}
