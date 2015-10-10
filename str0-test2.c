/*
%use str0_getline;
%use str0_colon;
%use str0_putline;
*/

#include <stdio.h>
#include "str0.h"

str0 line;
str0 field;

int handle(void)
{
  printf("field ");
  str0_putline(&field);
  if (str0_length(&field) == 1) return -1;
  return 1;
}

int main(int argc,char **argv)
{
  while (str0_getline(&line,0) > 0)
    printf("returns %d\n",str0_colon(&field,0,&line,handle));
  return 0;
}
