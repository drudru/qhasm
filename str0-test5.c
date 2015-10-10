/*
%use str0_getline;
%use str0_increment;
%use str0_putline;
*/

#include "str0.h"

str0 line;

int main(int argc,char **argv)
{
  while (str0_getline(&line,0) > 0) {
    if (!str0_increment(&line,0)) return 111;
    str0_putline(&line);
  }
  return 0;
}
