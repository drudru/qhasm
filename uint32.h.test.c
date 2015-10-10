#include <unistd.h>
#include "uint32.h.impl"

int main(int argc,char **argv)
{
  uint32 i;
  i = 1;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  if (!i) _exit(1);
  if (i < 1) _exit(1);
  i += i;
  if (i) _exit(1);
  _exit(0);
}
