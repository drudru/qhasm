/*
%use str0_length;
%use str0_equalc;
*/

#include "critbit0.h"
#define child critbit0_private_child
#define root critbit0_private_root
#define byte critbit0_private_byte
#define otherbits critbit0_private_otherbits

int critbit0_contains(critbit0_tree *t,str0 *u)
{
  const uint8 *ubytes = (void *) *u;
  int64 ulen = str0_length(u);
  uint8 *p = t->root;
  critbit0_node *q;
  uint32 byte;
  uint32 otherbits;
  uint8 c;
  int direction;

  if (!p) return 0; /* S = {} */

  while (1 & (int) p) {
    q = (void *) (p - 1);
    byte = q->byte;
    otherbits = q->otherbits;
    c = 0;
    if (byte < ulen) c = ubytes[byte];
    direction = (1 + (otherbits | c)) >> 8;
    p = q->child[direction];
  }

  return str0_equalc(u,(const char *) p);
}
