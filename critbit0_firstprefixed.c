/*
%use str0_length;
%use str0_copyc;
*/

#include "critbit0.h"
#define child critbit0_private_child
#define root critbit0_private_root
#define byte critbit0_private_byte
#define otherbits critbit0_private_otherbits

int critbit0_firstprefixed(critbit0_tree *t,limitmalloc_pool *pool,str0 *result,str0 *prefix)
{
  const uint8 *ubytes = (void *) *prefix;
  int64 ulen = str0_length(prefix);
  uint8 *p = t->root;
  uint8 *top = p;
  critbit0_node *q;
  uint32 byte;
  uint32 otherbits;
  uint8 c;
  int direction;
  int64 i;

  if (!p) return 0; /* S = {} */

  while (1 & (int) p) {
    q = (void *) (p - 1);
    byte = q->byte;
    otherbits = q->otherbits;
    c = 0;
    if (byte < ulen) c = ubytes[byte];
    direction = (1 + (otherbits | c)) >> 8;
    p = q->child[direction];
    if (byte < ulen) top = p;
  }

  for (i = 0;i < ulen;++i)
    if (p[i] != ubytes[i])
      return 0;

  return str0_copyc(result,pool,(const char *) (p + ulen));
}
