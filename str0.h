#ifndef STR0_H
#define STR0_H

#include "limitmalloc.h"
#include "uint64.h"

typedef char *str0;

extern int64 str0_length(str0 *);
extern int str0_equal(str0 *,str0 *);

extern int str0_equalc(str0 *,const char *);

extern void str0_free(str0 *,limitmalloc_pool *);
extern int str0_copy(str0 *,limitmalloc_pool *,str0 *);
extern int str0_concat(str0 *,limitmalloc_pool *,str0 *);
extern int str0_uint64(str0 *,limitmalloc_pool *,uint64);
extern int str0_increment(str0 *,limitmalloc_pool *);

extern int str0_copyc(str0 *,limitmalloc_pool *,const char *);
extern int str0_copycl(str0 *,limitmalloc_pool *,const char *,int64);
extern int str0_concatc(str0 *,limitmalloc_pool *,const char *);

extern int str0_getline(str0 *,limitmalloc_pool *);
extern void str0_putline(str0 *);

extern int str0_colon(str0 *,limitmalloc_pool *,str0 *,int (*)(void));
extern int str0_word(str0 *,limitmalloc_pool *,str0 *,int (*)(int));

#endif
