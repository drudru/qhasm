/*
%use str0_length;
%use str0_copycl;
*/

#include "str0.h"

int str0_word(str0 *field,limitmalloc_pool *pool,str0 *line,int (*handle)(int))
{
  int64 len = str0_length(line);
  int64 i;
  int64 j;
  char c;

  j = 0;
  for (i = 0;i <= len;++i) {
    if (i == len) c = ' '; else c = (*line)[i];
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')
      continue;
    if (j < i) {
      if (!str0_copycl(field,pool,(*line) + j,i - j)) return -1;
      switch(handle(1)) {
        case 1: break;
        case 0: return 0;
	default: return -1;
      }
    }
    if (i == len) break;
    if (!str0_copycl(field,pool,&c,1)) return -1;
    switch(handle(-(c == ' ' || c == '\t' || c == '\r' || c == '\n'))) {
      case 1: break;
      case 0: return 0;
      default: return -1;
    }
    j = i + 1;
  }

  return 1;
}
