/*
%use str0_length;
%use str0_equalc;
%use limitmalloc_close;
*/

#include "critbit0.h"
#define child critbit0_private_child
#define root critbit0_private_root
#define byte critbit0_private_byte
#define otherbits critbit0_private_otherbits

int critbit0_delete(critbit0_tree *t,limitmalloc_pool *pool,str0 *u)
{
  const uint8 *ubytes = (void *) *u;
  int64 ulen = str0_length(u);
  uint8 *p = t->root;
  void **wherep = &t->root;
  void **whereq = 0;
  critbit0_node *q = 0;
  uint32 byte;
  uint32 otherbits;
  uint8 c;
  int direction;

  if (!p) return 0; /* S = {} */

  while (1 & (int) p) {
    whereq = wherep;
    q = (void *) (p - 1);
    byte = q->byte;
    otherbits = q->otherbits;
    c = 0;
    if (byte < ulen) c = ubytes[byte];
    direction = (1 + (otherbits | c)) >> 8;
    wherep = q->child + direction;
    p = *wherep;
  }

  if (!str0_equalc(u,(const char *) p)) return 0;
  limitmalloc_close(pool,ulen + 1,p);

  if (!whereq) { /* #S = 1 */
    t->root = 0;
    return 1;
  }

  *whereq = q->child[1 - direction];
  limitmalloc_close(pool,sizeof(critbit0_node),q);
  return 1;
}
