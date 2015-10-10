/*
%use str0_uint64;
%use str0_putline;
*/

#include <stdio.h>
#include "str0.h"

str0 n;

int main(int argc,char **argv)
{
  uint64 u;
  int result;

  for (u = 0;u < 1000;++u) {
    if (!str0_uint64(&n,0,u)) break;
    str0_putline(&n);
  }
  return 0;
}
