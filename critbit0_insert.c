/*
%use limitmalloc_open_if2;
%use limitmalloc_close;
%use str0_length;
*/

#include <errno.h>
#include "critbit0.h"
#define child critbit0_private_child
#define byte critbit0_private_byte
#define otherbits critbit0_private_otherbits
#define root critbit0_private_root

int critbit0_insert(critbit0_tree *t,limitmalloc_pool *pool,str0 *u)
{
  const uint8 *ubytes = (void *) *u;
  int64 ulen = str0_length(u);
  uint8 *p = t->root;
  critbit0_node *q;
  uint32 byte;
  uint32 otherbits;
  uint8 c;
  int direction;
  char *x;
  int64 i;
  uint32 newbyte;
  uint32 newotherbits;
  int newdirection;
  critbit0_node *newnode;
  void **wherep;

  if (ulen > 1073741824) { errno = ENOMEM; return 0; }

  if (!p) { /* S = {} */
    x = limitmalloc_open_if2(pool,ulen + 1);
    if (!x) return 0;
    for (i = 0;i < ulen;++i) x[i] = ubytes[i];
    x[ulen] = 0;
    t->root = x;
    return 2;
  }

  while (1 & (int) p) {
    q = (void *) (p - 1);
    byte = q->byte;
    otherbits = q->otherbits;
    c = 0;
    if (byte < ulen) c = ubytes[byte];
    direction = (1 + (otherbits | c)) >> 8;
    p = q->child[direction];
  }

  for (newbyte = 0;newbyte < ulen;++newbyte)
    if (p[newbyte] != ubytes[newbyte]) {
      newotherbits = p[newbyte] ^ ubytes[newbyte];
      goto differentbyte;
    }
  if (p[newbyte] != 0) {
    newotherbits = p[newbyte];
    goto differentbyte;
  }
  return 1;

  differentbyte:
  newotherbits &= 255;
  while (newotherbits & (newotherbits - 1)) newotherbits &= newotherbits - 1;
  newotherbits ^= 255;
  c = p[newbyte];
  newdirection = (1 + (newotherbits | c)) >> 8;

  newnode = limitmalloc_open_if2(pool,sizeof(critbit0_node));
  if (!newnode) return 0;

  x = limitmalloc_open_if2(pool,ulen + 1);
  if (!x) {
    limitmalloc_close(pool,sizeof(critbit0_node),newnode);
    return 0;
  }
  for (i = 0;i < ulen;++i) x[i] = ubytes[i];
  x[ulen] = 0;

  newnode->byte = newbyte;
  newnode->otherbits = newotherbits;
  newnode->child[1 - newdirection] = x;

  wherep = &t->root;
  for (;;) {
    p = *wherep;
    if (!(1 & (int) p)) break;
    q = (void *) (p - 1);
    byte = q->byte;
    otherbits = q->otherbits;
    if (byte > newbyte) break;
    if (byte == newbyte && otherbits > newotherbits) break;
    c = 0;
    if (byte < ulen) c = ubytes[byte];
    direction = (1 + (otherbits | c)) >> 8;
    wherep = q->child + direction;
  }

  newnode->child[newdirection] = *wherep;
  *wherep = (void *) (1 + (char *) newnode);
  return 2;
}
