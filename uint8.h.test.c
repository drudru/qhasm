#include <unistd.h>
#include "uint8.h.impl"

int main(int argc,char **argv)
{
  uint8 i;
  i = 1;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  if (!i) _exit(1);
  if (i < 1) _exit(1);
  i += i;
  if (i) _exit(1);
  _exit(0);
}
