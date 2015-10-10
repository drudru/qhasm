/*
%use limitmalloc_close;
*/

#include "critbit0.h"
#define child critbit0_private_child
#define root critbit0_private_root
#define byte critbit0_private_byte
#define otherbits critbit0_private_otherbits

static void traverse(void **top,limitmalloc_pool *pool)
{
  uint8 *p;
  critbit0_node *q;
  int64 i;

  p = *top;
  if (1 & (int) p) {
    q = (void *) (p - 1);
    traverse(q->child,pool);
    traverse(q->child + 1,pool);
    limitmalloc_close(pool,sizeof(critbit0_node),q);
  } else {
    i = 0; while (p[i]) ++i;
    limitmalloc_close(pool,i + 1,p);
  }
  *top = 0;
}

void critbit0_clear(critbit0_tree *t,limitmalloc_pool *pool)
{
  if (t->root) traverse(&t->root,pool);
}
