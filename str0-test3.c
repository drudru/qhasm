/*
%use str0_getline;
%use str0_word;
%use str0_putline;
*/

#include <stdio.h>
#include "str0.h"

str0 line;
str0 word;

int handle(int alpha)
{
  printf("word %d ",alpha);
  str0_putline(&word);
  return 1;
}

int main(int argc,char **argv)
{
  while (str0_getline(&line,0) > 0)
    printf("returns %d\n",str0_word(&word,0,&line,handle));
  return 0;
}
