/*
%use str0_getline;
%use str0_putline;
%use str0_copy;
*/

#include <stdio.h>
#include "str0.h"

limitmalloc_pool pool = { 4096 };

str0 s;
str0 t;

int main(int argc,char **argv)
{
  for (;;) {
    int result = str0_getline(&s,&pool);
    printf("getline %d\n",result);
    if (result <= 0) return 0;
    result = str0_copy(&t,&pool,&s);
    printf("copy %d\n",result);
    if (result <= 0) return 0;
    str0_putline(&t);
  }
}
