#!/bin/sh -ex

for i in uint8.h uint32.h uint64.h int64.h
do
  (
    for x in $i:*
    do
      rm -f $i.test
      cp $x $i.impl || continue
      ./compile -c $i.test.c || continue
      ./compile -o $i.test $i.test.o || continue
      ./$i.test || continue
      exit 0
    done
  ) || (
    rm -f $i.impl
    rm -f $i.test
    rm -f $i.test.o
    exit 111
  ) || exit 111
  
  cat $i.impl > $i
  rm -f $i.impl
  rm -f $i.test
  rm -f $i.test.o
done

./compile -c critbit0_allprefixed.c
./compile -c critbit0_clear.c
./compile -c critbit0_contains.c
./compile -c critbit0_delete.c
./compile -c critbit0_firstprefixed.c
./compile -c critbit0_insert.c
./compile -c critbit0-test1.c
./compile -c limitmalloc_close.c
./compile -c limitmalloc_open.c
./compile -c limitmalloc_open_if2.c
./compile -c limitmalloc-test1.c
./compile -c qhasm-ops.c
./compile -c qhasm-regs.c
./compile -c str0_colon.c
./compile -c str0_concat.c
./compile -c str0_concatc.c
./compile -c str0_copy.c
./compile -c str0_copyc.c
./compile -c str0_copycl.c
./compile -c str0_equal.c
./compile -c str0_equalc.c
./compile -c str0_free.c
./compile -c str0_getline.c
./compile -c str0_increment.c
./compile -c str0_length.c
./compile -c str0_putline.c
./compile -c str0-test1.c
./compile -c str0-test2.c
./compile -c str0-test3.c
./compile -c str0-test4.c
./compile -c str0-test5.c
./compile -c str0_uint64.c
./compile -c str0_word.c

./load -o critbit0-test1 \
critbit0_contains.o \
critbit0_delete.o \
critbit0_insert.o \
critbit0-test1.o \
limitmalloc_close.o \
limitmalloc_open_if2.o \
limitmalloc_open.o \
str0_copyc.o \
str0_equalc.o \
str0_equal.o \
str0_free.o \
str0_length.o

./load -o limitmalloc-test1 \
limitmalloc_close.o \
limitmalloc_open.o \
limitmalloc-test1.o

./load -o str0-test1 \
limitmalloc_close.o \
limitmalloc_open.o \
str0_concatc.o \
str0_copyc.o \
str0_copy.o \
str0_free.o \
str0_getline.o \
str0_length.o \
str0_putline.o \
str0-test1.o

./load -o str0-test2 \
limitmalloc_close.o \
limitmalloc_open.o \
str0_colon.o \
str0_concatc.o \
str0_copycl.o \
str0_copyc.o \
str0_free.o \
str0_getline.o \
str0_length.o \
str0_putline.o \
str0-test2.o

./load -o str0-test3 \
limitmalloc_close.o \
limitmalloc_open.o \
str0_concatc.o \
str0_copycl.o \
str0_copyc.o \
str0_free.o \
str0_getline.o \
str0_length.o \
str0_putline.o \
str0-test3.o \
str0_word.o

./load -o str0-test4 \
limitmalloc_close.o \
limitmalloc_open.o \
str0_free.o \
str0_length.o \
str0_putline.o \
str0-test4.o \
str0_uint64.o

./load -o str0-test5 \
limitmalloc_close.o \
limitmalloc_open.o \
str0_concatc.o \
str0_copyc.o \
str0_free.o \
str0_getline.o \
str0_increment.o \
str0_length.o \
str0_putline.o \
str0-test5.o

./load -o qhasm-ops \
critbit0_allprefixed.o \
critbit0_clear.o \
critbit0_contains.o \
critbit0_delete.o \
critbit0_firstprefixed.o \
critbit0_insert.o \
limitmalloc_close.o \
limitmalloc_open_if2.o \
limitmalloc_open.o \
qhasm-ops.o \
str0_colon.o \
str0_concatc.o \
str0_concat.o \
str0_copycl.o \
str0_copyc.o \
str0_copy.o \
str0_equalc.o \
str0_equal.o \
str0_free.o \
str0_getline.o \
str0_increment.o \
str0_length.o \
str0_putline.o \
str0_uint64.o \
str0_word.o

./load -o qhasm-regs \
critbit0_allprefixed.o \
critbit0_clear.o \
critbit0_contains.o \
critbit0_delete.o \
critbit0_firstprefixed.o \
critbit0_insert.o \
limitmalloc_close.o \
limitmalloc_open_if2.o \
limitmalloc_open.o \
qhasm-regs.o \
str0_colon.o \
str0_concatc.o \
str0_concat.o \
str0_copycl.o \
str0_copyc.o \
str0_copy.o \
str0_equalc.o \
str0_equal.o \
str0_free.o \
str0_getline.o \
str0_increment.o \
str0_length.o \
str0_putline.o \
str0_uint64.o \
str0_word.o

(
  echo '--- limitmalloc library works'
  ./limitmalloc-test1
  echo $?

  echo '--- str0_getline handles empty input'
  : | ./str0-test1
  echo $?
  
  echo '--- str0_getline handles normal lines'
  ( echo hello; echo there ) | ./str0-test1
  echo $?
  
  echo '--- str0_getline handles partial final lines'
  ( echo hello; echo there | tr -d '\012' ) | ./str0-test1
  echo $?
  
  echo '--- str0_getline handles long lines'
  ( echo 0123456789abcdefghijklmnopqrstuvwxyz
    echo 0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz
    echo 0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz
    echo 0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz
    echo 0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz
    echo 0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz
    echo 0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz
  ) | ./str0-test1
  echo $?
  
  echo '--- str0_getline and str0_putline handle nulls properly'
  echo hello | tr l '\0' | ./str0-test1 | tr '\0' x
  
  echo '--- str0_colon splits into fields'
  echo hello:world | ./str0-test2
  echo $?
  
  echo '--- str0_colon handles empty final field'
  echo hello:world: | ./str0-test2
  echo $?
  
  echo '--- str0_colon handles many fields'
  echo one:two:three:four:five:six:seven | ./str0-test2
  echo $?
  
  echo '--- str0_colon handles 1 field'
  echo hello world | ./str0-test2
  echo $?
  
  echo '--- str0_colon handles empty line'
  echo '' | ./str0-test2
  echo $?
  
  echo '--- str0_word splits into words'
  echo 'hello, world. "this is a test---really," he said.' | ./str0-test3
  echo $?
  
  echo '--- str0_word handles empty line'
  echo '' | ./str0-test3
  echo $?
  
  echo '--- str0_uint64 works'
  ./str0-test4
  echo $?
  
  echo '--- str0_increment works'
  ( echo 0
    echo 1
    echo 2
    echo 9
    echo 10
    echo 11
    echo 19
    echo 89
    echo 99
    echo 999999999999999999999999999999999999999999999
    echo ''
  ) | ./str0-test5
  echo $?

  echo '--- critbit0 library works'
  ./critbit0-test1
  echo $?

  echo '--- qhasm-ops prioritizes variable name over text'
  (
    echo ':name:int32:%eax:%ecx:%edx:%ebx:%esi:%edi:%ebp:'
    echo 'int32 r:var/r=int32:'
    echo 'zero r:>r=int32:asm/set >r to 0:'
    echo ':'
    echo 'int32 x'
    echo 'zero x'
    echo 'int32 zero'
    echo 'zero zero'
  ) | ./qhasm-ops
  echo $?

  echo '--- qhasm-regs handles register limits'
  (
    echo ':name:int32:%eax:%ecx:%edx:%ebx:%esi:%edi:%ebp:'
    echo 'int32 r:var/r=int32:'
    echo 'zero r:>r=int32#3,4,5:asm/set >r to 0:'
    echo 'read r:<r=int32:asm/inspect <r:'
    echo 'enter f:enter/f:asm/enter #f:'
    echo 'leave:nofallthrough:leave:asm/leave:'
    echo ':'
    echo 'enter f'
    echo 'int32 x'
    echo 'int32 y'
    echo 'int32 z'
    echo 'zero x'
    echo 'zero y'
    echo 'zero z'
    echo 'read x'
    echo 'read y'
    echo 'read z'
    echo 'zero x'
    echo 'zero y'
    echo 'read x'
    echo 'zero z'
    echo 'read y'
    echo 'read z'
    echo 'leave'
  ) | ./qhasm-ops | ./qhasm-regs
  echo $?

) > tests.out

diff -u tests.exp tests.out >&2
