/*
%use str0_concatc;
%use str0_length;
*/

#include "str0.h"

int str0_increment(str0 *s,limitmalloc_pool *pool)
{
  char *x = *s;
  int64 len = str0_length(s);
  int64 i;

  for (i = len - 1;i >= 0;--i) {
    if (x[i] >= '0' && x[i] <= '8') { ++x[i]; return 1; }
    x[i] = '0';
  }
  if (!str0_concatc(s,pool,"0")) return 0;
  s[0][0]++;
  return 1;
}
