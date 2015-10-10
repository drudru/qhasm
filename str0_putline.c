#include <stdio.h>
#include "str0.h"

void str0_putline(str0 *s)
{
  char *x = *s;
  if (x)
    while (*x) {
      if (*x == '\n') putchar(0);
      else putchar(*x);
      ++x;
    }
  putchar('\n');
}
