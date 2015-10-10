/*
%use str0_length;
*/

#include "str0.h"

int str0_equalc(str0 *s,const char *y)
{
  int64 len = 0;
  int64 i;
  if (y) while (y[len]) ++len;
  if (len != str0_length(s)) return 0;
  for (i = 0;i < len;++i) if (y[i] != (*s)[i]) return 0;
  return 1;
}
