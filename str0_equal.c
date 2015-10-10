/*
%use str0_length;
%use str0_equalc;
*/

#include "str0.h"

int str0_equal(str0 *s,str0 *t)
{
  return str0_equalc(s,*t);
}
