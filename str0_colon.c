/*
%use str0_length;
%use str0_copycl;
*/

#include "str0.h"

int str0_colon(str0 *field,limitmalloc_pool *pool,str0 *line,int (*handle)(void))
{
  int64 len = str0_length(line);
  int64 i;
  int64 j;

  j = 0;
  for (i = 0;i < len;++i) {
    if ((*line)[i] == ':') {
      if (!str0_copycl(field,pool,(*line) + j,i - j)) return -1;
      switch(handle()) {
        case 1: break;
        case 0: return 0;
	default: return -1;
      }
      j = i + 1;
    }
  }
  if (!str0_copycl(field,pool,(*line) + j,i - j)) return -1;
  switch(handle()) {
    case 1: return 1;
    case 0: return 0;
    default: return -1;
  }
}
