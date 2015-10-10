/*
%use str0_length;
%use str0_equalc;
%use str0_copyc;
*/

#include "critbit0.h"
#include "int64.h"
#define child critbit0_private_child
#define root critbit0_private_root
#define byte critbit0_private_byte
#define otherbits critbit0_private_otherbits

static int traverse(uint8 *top,limitmalloc_pool *pool,str0 *result,int (*handle)(void),int64 prefixlen)
{
  critbit0_node *q;
  int direction;

  if (1 & (int) top) {
    q = (void *) (top - 1);
    for (direction = 0;direction < 2;++direction)
      switch(traverse(q->child[direction],pool,result,handle,prefixlen)) {
        case 1: break;
        case 0: return 0;
        default: return -1;
      }
    return 1;
  }

  while (prefixlen > 0 && *top) { ++top; --prefixlen; }
  /* unnecessarily defensive; strings all begin with prefix */

  if (!str0_copyc(result,pool,(const char *) top)) return -1;
  if (!handle) return 0;
  return handle();
}

int critbit0_allprefixed(critbit0_tree *t,limitmalloc_pool *pool,str0 *result,str0 *prefix,int (*handle)(void))
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

  if (!p) return 1; /* S = {} */

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
      return 1;

  return traverse(top,pool,result,handle,ulen);
}
