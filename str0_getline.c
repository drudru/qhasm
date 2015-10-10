/*
%use str0_free;
%use str0_copyc;
%use str0_concatc;
*/

#include <stdio.h>
#include "str0.h"

/* XXX: quadratic time for long lines */

static char buf[129];

int str0_getline(str0 *s,limitmalloc_pool *pool)
{
  char c;
  int pos;

  if (!str0_copyc(s,pool,"")) return -1;

  pos = 0;
  for (;;) {
    if (scanf("%c",&c) < 1) {
      if (ferror(stdin)) return -2;
      buf[pos] = 0;
      if (!str0_concatc(s,pool,buf)) return -1;
      if (!str0_length(s)) return 0;
      return 1;
    }
    if (c == '\n') {
      buf[pos] = 0;
      if (!str0_concatc(s,pool,buf)) return -1;
      return 2;
    }
    if (c == 0) c = '\n';
    buf[pos++] = c;
    if (pos == 128) {
      buf[pos] = 0; /* redundant */
      if (!str0_concatc(s,pool,buf)) return -1;
      pos = 0;
    }
  }
}
