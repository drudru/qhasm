/*
%use critbit0_contains;
%use critbit0_insert;
%use critbit0_delete;
%use str0_copyc;
%use str0_equal;
*/

#include <stdio.h>
#include "critbit0.h"

critbit0_tree t;

char randstr[5];
str0 data[32];

limitmalloc_pool pool = { 1024 };

int main(int argc,char **argv)
{
  int64 i;
  int64 j;
  int64 loop;

  for (loop = 0;loop < 100;++loop) {
    for (i = 0;i < 32;++i) {
      for (;;) {
        randstr[0] = random();
        randstr[1] = random();
        randstr[2] = random();
        randstr[3] = 0;
	str0_copyc(&data[i],&pool,randstr);
        for (j = 0;j < i;++j) if (str0_equal(&data[j],&data[i])) break;
        if (j == i) break;
      }
    }
    for (j = 0;j <= 64;++j) {
      for (i = 0;i < 32;++i) printf("%d",critbit0_contains(&t,&data[i]));
      if (j < 32) printf(" %d",critbit0_insert(&t,&pool,&data[j]));
      else if (j < 64) printf(" %d",critbit0_delete(&t,&pool,&data[j - 32]));
      printf("\n");
    }
  }

  return 0;
}
