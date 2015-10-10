#include <unistd.h>
#include "int64.h.impl"

int main(int argc,char **argv)
{
  int64 i;
  i = 1;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  i += i; i += i; i += i; i += i; i += i; i += i; i += i;
  if (!i) _exit(1);
  if (i > 0) _exit(1);
  i += i;
  if (i) _exit(1);
  _exit(0);
}
