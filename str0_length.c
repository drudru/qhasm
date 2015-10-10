#include "str0.h"

int64 str0_length(str0 *s)
{
  int64 result = 0;
  char *x = *s;
  if (x) while (*x) { ++x; ++result; }
  return result;
}
