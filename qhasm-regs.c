/*
%use critbit0_allprefixed;
%use critbit0_clear;
%use critbit0_contains;
%use critbit0_delete;
%use critbit0_firstprefixed;
%use critbit0_insert;
%use str0_colon;
%use str0_concat;
%use str0_concatc;
%use str0_copy;
%use str0_equal;
%use str0_getline;
%use str0_increment;
%use str0_putline;
%use str0_uint64;
%use str0_word;
*/

#include <unistd.h>
#include "critbit0.h"
#include "str0.h"

void nomem(void)
{
  _exit(111); /* XXX: louder? */
}


str0 empty;

critbit0_tree inputlines;
int64 inputlinesnum;

critbit0_tree labels;
critbit0_tree vtypes;
critbit0_tree alltypes;
critbit0_tree inputregs;
critbit0_tree inputregassignments;
critbit0_tree outputregs;
critbit0_tree outputregassignments;
critbit0_tree reglimitok;
critbit0_tree uses;
critbit0_tree entries;
critbit0_tree exits;
critbit0_tree nextinsn;

critbit0_tree assignments;
critbit0_tree reads;
critbit0_tree finalreads;
critbit0_tree values;
critbit0_tree conflicts;

critbit0_tree blockbegins;
critbit0_tree blockends;
critbit0_tree blockleaps;
critbit0_tree blockkilled;
critbit0_tree blockread;
critbit0_tree blockassigned;
critbit0_tree blockvisible;
critbit0_tree blocklive;
/* XXX: also handle renames */

critbit0_tree usegraph;
critbit0_tree assignusegraph;

critbit0_tree livevalues;
critbit0_tree livevaluetypes;
critbit0_tree liveregs;
critbit0_tree livefloat80regs;

critbit0_tree avoidregs;
critbit0_tree allocated;
critbit0_tree allocations;
critbit0_tree float80after;

str0 inputline;
str0 inputfield;
str0 inputword;

str0 n;
str0 next;
str0 key;
str0 type;
str0 reglimit;
str0 reglimitprefix;
str0 r;
str0 v;
str0 x;
str0 y;
str0 z;
str0 u;

str0 outputline;


int findvars2_state;

int findvars2(void)
{
  switch(findvars2_state) {
    case 0:
      if (!str0_copy(&n,0,&inputfield)) return -1;
      findvars2_state = 1; return 1;
    case 1:
      if (str0_equalc(&inputfield,"label")) { findvars2_state = 10; return 1; }
      if (str0_equalc(&inputfield,"type")) { findvars2_state = 20; return 1; }
      break;
    case 10:
      if (!str0_copy(&key,0,&inputfield)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&n)) return -1;
      if (!critbit0_insert(&labels,0,&key)) return -1;
      break;
    case 20:
      if (!str0_copy(&key,0,&inputfield)) return -1;
      findvars2_state = 21; return 1;
    case 21:
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&inputfield)) return -1;
      if (!critbit0_insert(&vtypes,0,&key)) return -1;
      if (!critbit0_insert(&alltypes,0,&inputfield)) return -1;
      break;
  }
  return 0;
}

int findvars(void)
{
  findvars2_state = 0;
  if (str0_colon(&inputfield,0,&inputline,findvars2) < 0) return -1;
  return 1;
}


int finduses_reglimit_state;

int finduses_reglimit(int alpha)
{
  switch(finduses_reglimit_state) {
    case 0:
      if (!str0_equalc(&inputword,"=")) return 0;
      finduses_reglimit_state = 1; return 1;
    case 1:
      if (!str0_copy(&type,0,&inputword)) return -1;
      finduses_reglimit_state = 2; return 1;
    case 2:
      if (!str0_equalc(&inputword,"#")) return 0;
      finduses_reglimit_state = 3; return 1;
    case 3:
      if (!str0_copy(&key,0,&reglimit)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&type)) return -1;
      if (!str0_concatc(&key,0,"#")) return -1;
      if (!str0_concat(&key,0,&inputword)) return -1;
      if (!critbit0_insert(&reglimitok,0,&key)) return -1;
      break;
  }
  return 0;
}

int finduses2_state;

int finduses2(void)
{
  switch(finduses2_state) {
    case 0:
      if (!str0_copy(&n,0,&inputfield)) return -1;
      finduses2_state = 1; return 1;
    case 1:
      if (str0_equalc(&inputfield,"inputreg")) { finduses2_state = 10; return 1; }
      if (str0_equalc(&inputfield,"outputreg")) { finduses2_state = 20; return 1; }
      if (str0_equalc(&inputfield,"kill")) { finduses2_state = 30; return 1; }
      if (str0_equalc(&inputfield,"reglimit")) { finduses2_state = 40; return 1; }
      if (str0_equalc(&inputfield,"op")) { finduses2_state = 50; return 1; }
      if (str0_equalc(&inputfield,"maybegoto")) { finduses2_state = 50; return 1; }
      break;
    case 10:
      if (!critbit0_insert(&inputregs,0,&inputfield)) return -1;
      if (!str0_copy(&key,0,&inputfield)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!critbit0_firstprefixed(&vtypes,0,&type,&key)) {
	if (!str0_copyc(&outputline,0,"error:unknown variable ")) return -1;
	if (!str0_concat(&outputline,0,&key)) return -1;
	if (!str0_concat(&outputline,0,&inputline)) return -1;
	str0_putline(&outputline);
	return 0;
      }
      finduses2_state = 11; return 1;
    case 11:
      if (!str0_copy(&x,0,&key)) return -1;
      if (!str0_concat(&x,0,&inputfield)) return -1;
      if (!critbit0_insert(&inputregassignments,0,&x)) return -1;
      break;
    case 20:
      if (!critbit0_insert(&outputregs,0,&inputfield)) return -1;
      if (!str0_copy(&key,0,&inputfield)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!critbit0_firstprefixed(&vtypes,0,&type,&key)) {
	if (!str0_copyc(&outputline,0,"error:unknown variable ")) return -1;
	if (!str0_concat(&outputline,0,&key)) return -1;
	if (!str0_concat(&outputline,0,&inputline)) return -1;
	str0_putline(&outputline);
	return 0;
      }
      finduses2_state = 21; return 1;
    case 21:
      if (!str0_copy(&x,0,&key)) return -1;
      if (!str0_concat(&x,0,&inputfield)) return -1;
      if (!critbit0_insert(&outputregassignments,0,&x)) return -1;
      break;
    case 30:
      if (!str0_copy(&key,0,&inputfield)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!critbit0_firstprefixed(&vtypes,0,&type,&key)) {
	if (!str0_copyc(&outputline,0,"error:unknown variable ")) return -1;
	if (!str0_concat(&outputline,0,&key)) return -1;
	if (!str0_concat(&outputline,0,&inputline)) return -1;
	str0_putline(&outputline);
      }
      break;
    case 40:
      if (!str0_copy(&key,0,&inputfield)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!critbit0_firstprefixed(&vtypes,0,&type,&key)) {
	if (!str0_copyc(&outputline,0,"error:unknown variable ")) return -1;
	if (!str0_concat(&outputline,0,&key)) return -1;
	if (!str0_concat(&outputline,0,&inputline)) return -1;
	str0_putline(&outputline);
	return 0;
      }
      if (!str0_copy(&reglimit,0,&n)) return -1;
      if (!str0_concatc(&reglimit,0,"<")) return -1;
      if (!str0_concat(&reglimit,0,&inputfield)) return -1;
      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,":<")) return -1;
      if (!str0_concat(&key,0,&inputfield)) return -1;
      if (!critbit0_insert(&uses,0,&key)) return -1;
      finduses2_state = 41; return 1;
    case 41:
      finduses_reglimit_state = 0;
      if (str0_word(&inputword,0,&inputfield,finduses_reglimit) < 0) return -1;
      return 1;
    case 50:
      finduses2_state = 51; return 1;
    case 51:
      if (str0_length(&inputfield) > 0) {
        if (!str0_copy(&key,0,&n)) return -1;
        if (!str0_concatc(&key,0,":")) return -1;
        if (!str0_concat(&key,0,&inputfield)) return -1;
        if (!critbit0_insert(&uses,0,&key)) return -1;
      }
      return 1;
  }
  return 0;
}

int finduses(void)
{
  finduses2_state = 0;
  if (str0_colon(&inputfield,0,&inputline,finduses2) < 0) return -1;
  return 1;
}


int findentries3(void)
{
  if (!str0_copy(&key,0,&n)) return -1;
  if (!str0_concatc(&key,0,":>")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!critbit0_insert(&uses,0,&key)) return -1;
  if (!str0_copy(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (critbit0_firstprefixed(&inputregassignments,0,&y,&key) < 1) return -1;
  if (!str0_copy(&key,0,&n)) return -1;
  if (!str0_concatc(&key,0,">")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&y)) return -1;
  if (!critbit0_insert(&reglimitok,0,&key)) return -1;
  return 1;
}

int findexits3(void)
{
  if (!str0_copy(&key,0,&n)) return -1;
  if (!str0_concatc(&key,0,":<")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!critbit0_insert(&uses,0,&key)) return -1;
  if (!str0_copy(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (critbit0_firstprefixed(&outputregassignments,0,&y,&key) < 1) return -1;
  if (!str0_copy(&key,0,&n)) return -1;
  if (!str0_concatc(&key,0,"<")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&y)) return -1;
  if (!critbit0_insert(&reglimitok,0,&key)) return -1;
  return 1;
}

int findentries2_state;

int findentries2(void)
{
  switch(findentries2_state) {
    case 0:
      if (!str0_copy(&n,0,&inputfield)) return -1;
      if (!str0_copy(&next,0,&inputfield)) return -1;
      if (!str0_increment(&next,0)) return -1;
      findentries2_state = 1; return 1;
    case 1:
      if (str0_equalc(&inputfield,"leave")) { findentries2_state = 20; return 1; }
      if (str0_equalc(&inputfield,"goto")) { findentries2_state = 30; return 1; }
      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&next)) return -1;
      if (!critbit0_insert(&nextinsn,0,&key)) return -1;
      if (str0_equalc(&inputfield,"maybegoto")) { findentries2_state = 30; return 1; }
      if (str0_equalc(&inputfield,"enter")) { findentries2_state = 10; return 1; }
      break;
    case 10:
      if (!critbit0_insert(&blockbegins,0,&n)) return -1;
      if (!critbit0_insert(&entries,0,&n)) return -1;
      if (critbit0_allprefixed(&inputregs,0,&v,&empty,findentries3) < 1) nomem();
      break;
    case 20:
      if (!critbit0_insert(&exits,0,&n)) return -1;
      if (critbit0_allprefixed(&outputregs,0,&v,&empty,findexits3) < 1) nomem();
      break;
    case 30:
      if (!critbit0_insert(&blockbegins,0,&next)) return -1;
      if (!str0_copy(&key,0,&inputfield)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      switch(critbit0_firstprefixed(&labels,0,&next,&key)) {
	case 1:
	  break;
	case 0:
	  if (!str0_copyc(&outputline,0,"error:unknown label ")) return -1;
	  if (!str0_concat(&outputline,0,&key)) return -1;
	  if (!str0_concat(&outputline,0,&inputline)) return -1;
	  str0_putline(&outputline);
	  return 0;
	default: 
	  return -1;
      }
      if (!critbit0_insert(&blockbegins,0,&next)) return -1;
      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&next)) return -1;
      if (!critbit0_insert(&nextinsn,0,&key)) return -1;
  }
  return 0;
}

int findentries(void)
{
  findentries2_state = 0;
  if (str0_colon(&inputfield,0,&inputline,findentries2) < 0) return -1;
  return 1;
}


int findblocks_read(void)
{
  if (!str0_copy(&key,0,&n)) return -1;
  if (!str0_concatc(&key,0,"<")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!critbit0_insert(&reads,0,&key)) return -1;

  if (!str0_copy(&key,0,&inputfield)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  switch(critbit0_firstprefixed(&blockread,0,&x,&key)) {
    case 1:
      if (!str0_concat(&key,0,&x)) return -1;
      critbit0_delete(&blockread,0,&key);
      if (!str0_copy(&key,0,&x)) return -1;
      if (!str0_concatc(&key,0,"<")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,"<")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!critbit0_insert(&usegraph,0,&key)) return -1;
      if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,"<")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&x)) return -1;
      if (!str0_concatc(&key,0,"<")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
      break;
    case 0:
      break;
    default:
      return -1;
  }
  if (!str0_copy(&key,0,&inputfield)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&n)) return -1;
  if (!critbit0_insert(&blockread,0,&key)) return -1;

  if (!str0_copy(&key,0,&inputfield)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  switch(critbit0_firstprefixed(&blockassigned,0,&x,&key)) {
    case 1:
      if (!str0_copy(&key,0,&x)) return -1;
      if (!str0_concatc(&key,0,">")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,"<")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!critbit0_insert(&usegraph,0,&key)) return -1;
      if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,"<")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&x)) return -1;
      if (!str0_concatc(&key,0,">")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
      return 1;
    case 0:
      break;
    default:
      return -1;
  }

  if (!str0_copy(&key,0,&inputfield)) return -1;
  if (!str0_concatc(&key,0,"=")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (critbit0_contains(&blockkilled,&key)) {
    if (!str0_copyc(&outputline,0,"error:reading dead variable ")) return -1;
    if (!str0_concat(&outputline,0,&v)) return -1;
    if (!str0_concatc(&outputline,0,":")) return -1;
    if (!str0_concat(&outputline,0,&inputline)) return -1;
    str0_putline(&outputline);
    return 1;
  }

  if (!str0_copy(&key,0,&inputfield)) return -1;
  if (!str0_concatc(&key,0,"=")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&n)) return -1;
  if (!str0_concatc(&key,0,"<")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!critbit0_insert(&usegraph,0,&key)) return -1;
  if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
  if (!str0_copy(&key,0,&n)) return -1;
  if (!str0_concatc(&key,0,"<")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&inputfield)) return -1;
  if (!str0_concatc(&key,0,"=")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
  return 1;
}

int findblocks_kill(void)
{
  if (!str0_copy(&key,0,&inputfield)) return -1;
  if (!str0_concatc(&key,0,"=")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!critbit0_insert(&blockkilled,0,&key)) return -1;

  if (!str0_copy(&key,0,&inputfield)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  switch(critbit0_firstprefixed(&blockassigned,0,&x,&key)) {
    case 1:
      if (!str0_concat(&key,0,&x)) return -1;
      critbit0_delete(&blockassigned,0,&key);
      break;
    case 0:
      break;
    default:
      return -1;
  }

  if (!str0_copy(&key,0,&inputfield)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  switch(critbit0_firstprefixed(&blockread,0,&x,&key)) {
    case 1:
      if (!str0_concat(&key,0,&x)) return -1;
      critbit0_delete(&blockread,0,&key);
      break;
    case 0:
      break;
    default:
      return -1;
  }
  return 1;
}

int findblocks_write(void)
{
  if (!str0_copy(&key,0,&n)) return -1;
  if (!str0_concatc(&key,0,">")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!critbit0_insert(&assignments,0,&key)) return -1;

  if (findblocks_kill() < 0) return -1;
  if (!str0_copy(&key,0,&inputfield)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&n)) return -1;
  if (!critbit0_insert(&blockassigned,0,&key)) return -1;

  return 1;
}

int findblocks_finallinks_writing;
int findblocks_finallinks2_state;

int findblocks_finallinks2(void)
{
  static str0 v;

  switch(findblocks_finallinks2_state) {
    case 0:
      if (!str0_copy(&v,0,&x)) return -1;
      findblocks_finallinks2_state = 1; return 1;
    case 1:
      if (!str0_copy(&key,0,&x)) return -1;
      if (!str0_concatc(&key,0,findblocks_finallinks_writing ? ">" : "<")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!critbit0_insert(&usegraph,0,&key)) return -1;
      if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&x)) return -1;
      if (!str0_concatc(&key,0,findblocks_finallinks_writing ? ">" : "<")) return -1;
      if (!str0_concat(&key,0,&v)) return -1;
      if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
  }
  return 0;
}

int findblocks_finallinks(void)
{
  findblocks_finallinks2_state = 0;
  if (str0_colon(&x,0,&v,findblocks_finallinks2) < 0) return -1;
  return 1;
}

int findblocks2(void)
{
  if (critbit0_contains(&blockbegins,&inputfield)) {
    if (!str0_copy(&n,0,&inputfield)) return -1;
    for (;;) {
      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,":<")) return -1;
      if (critbit0_allprefixed(&uses,0,&v,&key,findblocks_read) < 1) return -1;

      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,":kill>")) return -1;
      if (critbit0_allprefixed(&uses,0,&v,&key,findblocks_kill) < 1) return -1;

      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,":>")) return -1;
      if (critbit0_allprefixed(&uses,0,&v,&key,findblocks_write) < 1) return -1;

      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_copy(&next,0,&n)) return -1;
      if (!str0_increment(&next,0)) return -1;
      if (!str0_concat(&key,0,&next)) return -1;
      if (critbit0_contains(&blockbegins,&next)) break;
      if (!critbit0_contains(&nextinsn,&key)) break;
      if (!str0_copy(&n,0,&next)) return -1;
    }

    if (!critbit0_insert(&blockends,0,&n)) return -1;
    if (!str0_copy(&key,0,&inputfield)) return -1;
    if (!str0_concatc(&key,0,":")) return -1;
    if (!str0_concat(&key,0,&n)) return -1;
    if (!critbit0_insert(&blockleaps,0,&key)) return -1;

    findblocks_finallinks_writing = 1;
    if (!str0_copy(&key,0,&inputfield)) return -1;
    if (!str0_concatc(&key,0,":")) return -1;
    if (critbit0_allprefixed(&blockassigned,0,&v,&key,findblocks_finallinks) < 1) nomem();

    findblocks_finallinks_writing = 0;
    if (!str0_copy(&key,0,&inputfield)) return -1;
    if (!str0_concatc(&key,0,":")) return -1;
    if (critbit0_allprefixed(&blockread,0,&v,&key,findblocks_finallinks) < 1) nomem();

    return 0;
  }
  return 0;
}

int findblocks(void)
{
  if (str0_colon(&inputfield,0,&inputline,findblocks2) < 0) return -1;
  return 1;
}


int traceassignment_state;

critbit0_tree totrace;
critbit0_tree traced;
str0 traceinsn;
str0 tracestatus;
str0 tracevar;

int addtotrace(void)
{
  if (!critbit0_insert(&totrace,0,&y)) return -1;
  return 1;
}

int traceword_state;

int traceword(int alpha)
{
  switch(traceword_state) {
    case 0:
      if (!str0_copy(&traceinsn,0,&y)) return -1;
      traceword_state = 1; return 1;
    case 1:
      if (!str0_copy(&tracestatus,0,&y)) return -1;
      traceword_state = 2; return 1;
    case 2:
      if (!str0_copy(&tracevar,0,&y)) return -1;
      traceword_state = 3; return 1;
  }
  return 0;
}

int connectblocks(void)
{
  if (critbit0_contains(&blockbegins,&y)) {
    if (!str0_copy(&key,0,&y)) return -1;
    if (!str0_concatc(&key,0,"=")) return -1;
    if (!str0_concat(&key,0,&tracevar)) return -1;
    if (!critbit0_insert(&totrace,0,&key)) return -1;

    if (!str0_copy(&key,0,&traceinsn)) return -1;
    if (!str0_concatc(&key,0,"=")) return -1;
    if (!str0_concat(&key,0,&tracevar)) return -1;
    if (!str0_concatc(&key,0,":")) return -1;
    if (!str0_concat(&key,0,&y)) return -1;
    if (!str0_concatc(&key,0,"=")) return -1;
    if (!str0_concat(&key,0,&tracevar)) return -1;
    if (!critbit0_insert(&assignusegraph,0,&key)) return -1;

    if (!str0_copy(&key,0,&y)) return -1;
    if (!str0_concatc(&key,0,"=")) return -1;
    if (!str0_concat(&key,0,&tracevar)) return -1;
    if (!str0_concatc(&key,0,":")) return -1;
    if (!str0_concat(&key,0,&traceinsn)) return -1;
    if (!str0_concatc(&key,0,"=")) return -1;
    if (!str0_concat(&key,0,&tracevar)) return -1;
    if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
  }
  return 1;
}

int traceassignment(void)
{
  int flagstarting;

  critbit0_clear(&traced,0);
  critbit0_clear(&totrace,0);
  if (!critbit0_insert(&totrace,0,&v)) return -1;

  for (flagstarting = 1;;flagstarting = 0) {
    switch(critbit0_firstprefixed(&totrace,0,&x,&empty)) {
      case 1:
	break;
      case 0:
	if (traceassignment_state == 1) {
	  if (!critbit0_insert(&finalreads,0,&v)) return -1;
	}
        return 1;
      default:
	return -1;
    }
    critbit0_delete(&totrace,0,&x);


    if (traceassignment_state == 1)
      if (!flagstarting)
        if (str0_equal(&v,&x))
	  return 1;

    switch(critbit0_insert(&traced,0,&x)) {
      case 2: break;
      case 1: continue;
      default: return -1;
    }

    traceword_state = 0;
    switch(str0_word(&y,0,&x,traceword)) {
      case 1:
	if (str0_equalc(&tracestatus,"<")) {
	  if (traceassignment_state == 2) {
	    if (!critbit0_insert(&blocklive,0,&v)) return -1;
	    return 1;
	  }
	  if (traceassignment_state == 1) {
	    if (!flagstarting) {
	      return 1;
	    }
	    break;
	  }
	  if (!str0_copy(&key,0,&v)) return -1;
	  if (!str0_concatc(&key,0,":")) return -1;
	  if (!str0_concat(&key,0,&x)) return -1;
	  if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
	  if (!str0_copy(&key,0,&x)) return -1;
	  if (!str0_concatc(&key,0,":")) return -1;
	  if (!str0_concat(&key,0,&v)) return -1;
	  if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
	} else if (str0_equalc(&tracestatus,"=")) {
	  if (critbit0_contains(&blockends,&traceinsn)) {
	    if (!str0_copy(&key,0,&traceinsn)) return -1;
	    if (!str0_concatc(&key,0,":")) return -1;
            if (critbit0_allprefixed(&nextinsn,0,&y,&key,connectblocks) < 1) return -1;
	  }
	  if (critbit0_contains(&blockbegins,&traceinsn)) {
	    if (!str0_copy(&key,0,&traceinsn)) return -1;
	    if (!str0_concatc(&key,0,":")) return -1;
	    if (!str0_concat(&key,0,&tracevar)) return -1;
	    if (!str0_copy(&key,0,&traceinsn)) return -1;
	    if (!str0_concatc(&key,0,"=")) return -1;
	    if (!str0_concat(&key,0,&tracevar)) return -1;
	    if (!critbit0_insert(&blockvisible,0,&key)) return -1;
	    if (!critbit0_contains(&blockkilled,&key)) {
	      if (!str0_copy(&key,0,&traceinsn)) return -1;
	      if (!str0_concatc(&key,0,":")) return -1;
	      switch(critbit0_firstprefixed(&blockleaps,0,&next,&key)) {
	        case 0:
		  break;
	        case 1:
	          if (!str0_copy(&key,0,&next)) return -1;
	          if (!str0_concatc(&key,0,"=")) return -1;
	          if (!str0_concat(&key,0,&tracevar)) return -1;
                  if (!critbit0_insert(&totrace,0,&key)) return -1;
	          if (!str0_copy(&key,0,&traceinsn)) return -1;
	          if (!str0_concatc(&key,0,"=")) return -1;
	          if (!str0_concat(&key,0,&tracevar)) return -1;
	          if (!str0_concatc(&key,0,":")) return -1;
	          if (!str0_concat(&key,0,&next)) return -1;
	          if (!str0_concatc(&key,0,"=")) return -1;
	          if (!str0_concat(&key,0,&tracevar)) return -1;
	          if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
	          if (!str0_copy(&key,0,&next)) return -1;
	          if (!str0_concatc(&key,0,"=")) return -1;
	          if (!str0_concat(&key,0,&tracevar)) return -1;
	          if (!str0_concatc(&key,0,":")) return -1;
	          if (!str0_concat(&key,0,&traceinsn)) return -1;
	          if (!str0_concatc(&key,0,"=")) return -1;
	          if (!str0_concat(&key,0,&tracevar)) return -1;
	          if (!critbit0_insert(&assignusegraph,0,&key)) return -1;
		  break;
	        default:
		  return -1;
	      }
	    }
	  }
	}
	break;
      case 0:
	break;
      default:
	return -1;
    }

    if (!str0_concatc(&x,0,":")) return -1;
    if (critbit0_allprefixed(&usegraph,0,&y,&x,addtotrace) < 1) return -1;
  }
}


str0 value;

int tracevalue(void)
{
  if (critbit0_contains(&traced,&x)) return 1;
  if (!critbit0_insert(&totrace,0,&x)) return -1;

  for (;;) {
    switch(critbit0_firstprefixed(&totrace,0,&x,&empty)) {
      case 1: critbit0_delete(&totrace,0,&x); break;
      case 0: if (!str0_increment(&value,0)) return -1; return 1;
      default: return -1;
    }
    if (critbit0_contains(&blockvisible,&x))
      if (!critbit0_contains(&blocklive,&x))
	continue;
    if (!critbit0_contains(&traced,&x)) {
      if (!critbit0_insert(&traced,0,&x)) return -1;
      if (!str0_copy(&key,0,&x)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&value)) return -1;
      if (!critbit0_insert(&values,0,&key)) return -1;
      if (!str0_copy(&key,0,&x)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (critbit0_allprefixed(&assignusegraph,0,&y,&key,addtotrace) < 1) return -1;
    }
  }
}


int allocate_state;

int recordconflict(void)
{
  if (!str0_copy(&key,0,&z)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&y)) return -1;
  if (!critbit0_insert(&conflicts,0,&key)) return -1;
  if (!str0_copy(&key,0,&y)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&z)) return -1;
  if (!critbit0_insert(&conflicts,0,&key)) return -1;
  return 1;
}

int followconflict(void)
{
  if (!str0_concatc(&z,0,":")) return -1;
  if (!str0_concat(&z,0,&u)) return -1;
  if (!critbit0_insert(&avoidregs,0,&z)) return -1;
  return 1;
}

int preallocate2_state;

int preallocate2(void)
{
  switch(preallocate2_state) {
    case 0:
      if (!str0_copy(&key,0,&reglimitprefix)) return -1;
      if (!str0_concat(&key,0,&y)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      switch(critbit0_firstprefixed(&values,0,&v,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      preallocate2_state = 1; return 1;
    case 1:
      if (!str0_copy(&key,0,&v)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&y)) return -1;
      if (critbit0_contains(&avoidregs,&key)) return 0;
      switch(critbit0_insert(&allocated,0,&v)) {
        case 2: break;
        case 1: return 0;
        default: return -1;
      }
      if (!critbit0_insert(&allocations,0,&key)) return -1;
      if (!str0_copy(&key,0,&v)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_copy(&u,0,&y)) return -1;
      if (critbit0_allprefixed(&conflicts,0,&z,&key,followconflict) < 1) return -1;
      break;
  }
  return 0;
}

int preallocate(void)
{
  preallocate2_state = 0;
  if (str0_colon(&y,0,&x,preallocate2) < 0) return -1;
  return 1;
}


int allocate_alive(void)
{
  if (!str0_copy(&key,0,&x)) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (critbit0_firstprefixed(&values,0,&y,&key) < 1) return -1;
  if (critbit0_contains(&livevalues,&y)) return 1;

  if (!str0_copy(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (critbit0_firstprefixed(&vtypes,0,&type,&key) < 1) return -1;

  if (str0_equalc(&type,"flags")) return 1; /* XXX */

  if (allocate_state == 1) {
    if (!str0_copy(&key,0,&type)) return -1;
    if (!str0_concatc(&key,0,":")) return -1;
    if (critbit0_allprefixed(&livevaluetypes,0,&z,&key,recordconflict) < 1) return -1;
  }

  if (!critbit0_insert(&livevalues,0,&y)) return -1;

  if (!str0_copy(&key,0,&type)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&y)) return -1;
  if (!critbit0_insert(&livevaluetypes,0,&key)) return -1;

  if (allocate_state == 2) {
    switch(critbit0_insert(&allocated,0,&y)) {
      case 2:
	break;
      case 1:
	if (!str0_copy(&key,0,&y)) return -1;
	if (!str0_concatc(&key,0,":")) return -1;
	if (critbit0_firstprefixed(&allocations,0,&z,&key) < 1) return -1;
	if (critbit0_contains(&liveregs,&z)) {
	  if (!str0_copyc(&outputline,0,"error:internal register misassignment:")) return -1;
	  if (!str0_concat(&outputline,0,&z)) return -1;
	  str0_putline(&outputline);
	}
	if (!critbit0_insert(&liveregs,0,&z)) return -1;
	if (str0_equalc(&type,"float80")) {
	  if (!str0_copy(&key,0,&v)) return -1;
	  if (!str0_concatc(&key,0,"#")) return -1;
	  if (!str0_concat(&key,0,&y)) return -1;
	  if (!str0_concatc(&key,0,"=")) return -1;
	  if (!str0_concat(&key,0,&z)) return -1;
	  if (!critbit0_insert(&livefloat80regs,0,&key)) return -1;
	}
	return 1;
      default:
	return -1;
    }
    if (!str0_copyc(&r,0,"1")) return -1;
    for (;;) {
      if (!str0_copy(&key,0,&type)) return -1;
      if (!str0_concatc(&key,0,"#")) return -1;
      if (!str0_concat(&key,0,&r)) return -1;
      if (!critbit0_contains(&liveregs,&key)) {
        if (!str0_copy(&key,0,&y)) return -1;
        if (!str0_concatc(&key,0,":")) return -1;
        if (!str0_concat(&key,0,&type)) return -1;
        if (!str0_concatc(&key,0,"#")) return -1;
        if (!str0_concat(&key,0,&r)) return -1;
        if (!critbit0_contains(&avoidregs,&key)) break;
      }
      if (!str0_increment(&r,0)) return -1;
    }
    if (!str0_copy(&key,0,&type)) return -1;
    if (!str0_concatc(&key,0,"#")) return -1;
    if (!str0_concat(&key,0,&r)) return -1;
    if (!critbit0_insert(&liveregs,0,&key)) return -1;
    if (str0_equalc(&type,"float80")) {
      if (!str0_copy(&z,0,&v)) return -1;
      if (!str0_concatc(&z,0,"#")) return -1;
      if (!str0_concat(&z,0,&y)) return -1;
      if (!str0_concatc(&z,0,"=")) return -1;
      if (!str0_concat(&z,0,&key)) return -1;
      if (!critbit0_insert(&livefloat80regs,0,&z)) return -1;
    }
    if (!str0_copy(&key,0,&y)) return -1;
    if (!str0_concatc(&key,0,":")) return -1;
    if (!str0_concat(&key,0,&type)) return -1;
    if (!str0_concatc(&key,0,"#")) return -1;
    if (!str0_concat(&key,0,&r)) return -1;
    if (!critbit0_insert(&allocations,0,&key)) return -1;
    if (!str0_copy(&key,0,&y)) return -1;
    if (!str0_concatc(&key,0,":")) return -1;
    if (!str0_copy(&u,0,&type)) return -1;
    if (!str0_concatc(&u,0,"#")) return -1;
    if (!str0_concat(&u,0,&r)) return -1;
    if (critbit0_allprefixed(&conflicts,0,&z,&key,followconflict) < 1) return -1;
    return 1;
  }

  return 1;
}

int allocate_dead(void)
{
  if (!str0_copy(&key,0,&x)) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (critbit0_firstprefixed(&values,0,&y,&key) < 1) return -1;
  if (!critbit0_contains(&livevalues,&y)) return 1;
  if (!critbit0_delete(&livevalues,0,&y)) return -1;

  if (!str0_copy(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (critbit0_firstprefixed(&vtypes,0,&z,&key) < 1) return -1;
  if (!str0_copy(&key,0,&z)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&y)) return -1;
  if (!critbit0_delete(&livevaluetypes,0,&key)) return -1;

  if (allocate_state == 2) {
    if (!str0_copy(&key,0,&y)) return -1;
    if (!str0_concatc(&key,0,":")) return -1;
    if (critbit0_firstprefixed(&allocations,0,&z,&key) == 1) { /* always true */
      critbit0_delete(&liveregs,0,&z);
      if (!str0_copy(&key,0,&v)) return -1;
      if (!str0_concatc(&key,0,"#")) return -1;
      if (!str0_concat(&key,0,&y)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      if (!str0_concat(&key,0,&z)) return -1;
      critbit0_delete(&livefloat80regs,0,&key);
    }
  }

  return 1;
}

int recordfloat80(void)
{
  if (!str0_copy(&key,0,&n)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!critbit0_insert(&float80after,0,&key)) return -1;
  return 1;
}

int allocate(void)
{
  if (critbit0_contains(&traced,&n)) {
    if (!str0_copyc(&outputline,0,"error:falling through entry:")) return -1;
    if (!str0_concat(&outputline,0,&n)) return -1;
    str0_putline(&outputline);
    return 1;
  }

  for (;;) {
    if (!critbit0_contains(&traced,&n)) {
      if (!critbit0_insert(&traced,0,&n)) return -1;

      if (critbit0_contains(&blockbegins,&n)) {
	critbit0_clear(&livevalues,0);
	critbit0_clear(&livevaluetypes,0);
	critbit0_clear(&liveregs,0);
	critbit0_clear(&livefloat80regs,0);
      }

      if (!str0_copy(&x,0,&n)) return -1;
      if (!str0_concatc(&x,0,"=")) return -1;
      if (critbit0_allprefixed(&blocklive,0,&v,&x,allocate_alive) < 0) return -1;

      if (!str0_copy(&x,0,&n)) return -1;
      if (!str0_concatc(&x,0,"<")) return -1;
      if (critbit0_allprefixed(&reads,0,&v,&x,allocate_alive) < 0) return -1;

      if (!str0_copy(&x,0,&n)) return -1;
      if (!str0_concatc(&x,0,"<")) return -1;
      if (critbit0_allprefixed(&finalreads,0,&v,&x,allocate_dead) < 0) return -1;

      if (!str0_copy(&x,0,&n)) return -1;
      if (!str0_concatc(&x,0,">")) return -1;
      if (critbit0_allprefixed(&assignments,0,&v,&x,allocate_alive) < 0) return -1;

      if (critbit0_allprefixed(&livefloat80regs,0,&v,&empty,recordfloat80) < 0) return -1;

      if (!str0_copy(&next,0,&n)) return -1;
      if (!str0_increment(&next,0)) return -1;
  
      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (critbit0_allprefixed(&nextinsn,0,&y,&key,addtotrace) < 0) return -1;
  
      if (!str0_copy(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&next)) return -1;
      if (critbit0_contains(&nextinsn,&key)) {
        if (!str0_copy(&n,0,&next)) return -1;
        continue;
      }
    }

    switch(critbit0_firstprefixed(&totrace,0,&n,&empty)) {
      case 1: break;
      case 0: return 1;
      default: return -1;
    }
    critbit0_delete(&totrace,0,&n);
  }
}


int convertword_state;

int convertword(int alpha)
{
  switch(convertword_state) {
    case 0: case 2:
      if (str0_equalc(&inputword,"inplace")) { convertword_state = 2; return 1; }
      if (!str0_equalc(&inputword,">") && !str0_equalc(&inputword,"<")) return 0;
      if (!str0_copy(&key,0,&n)) return -1;
      if (convertword_state == 2) {
        if (!str0_concatc(&key,0,"<")) return -1;
      } else {
        if (!str0_concat(&key,0,&inputword)) return -1;
      }
      convertword_state = 10;
      return 1;
    case 10:
      if (!str0_concat(&key,0,&inputword)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      switch(critbit0_firstprefixed(&values,0,&x,&key)) {
	case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!str0_copy(&key,0,&x)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      switch(critbit0_firstprefixed(&allocations,0,&y,&key)) {
	case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!str0_concatc(&outputline,0,"#")) return -1;
      if (!str0_concat(&outputline,0,&x)) return -1;
      if (!str0_concatc(&outputline,0,"=")) return -1;
      if (!str0_concat(&outputline,0,&y)) return -1;
      return 0;
  }
}

int convert_state;

int convert(void)
{
  switch(convert_state) {
    case 0:
      if (!str0_concat(&outputline,0,&inputfield)) return -1;
      convert_state = 1; return 1;
    case 1:
      if (!str0_concatc(&outputline,0,":")) return -1;
      if (!str0_concat(&outputline,0,&inputfield)) return -1;
      convert_state = 2; return 1;
    case 2:
      if (!str0_concatc(&outputline,0,":")) return -1;
      if (!str0_concat(&outputline,0,&inputfield)) return -1;
      convertword_state = 0;
      if (str0_word(&inputword,0,&inputfield,convertword) < 0) return -1;
      return 1;
  }
}

int printkill(void)
{
  if (!str0_concatc(&outputline,0,"kill>")) return -1;
  if (!str0_concat(&outputline,0,&v)) return -1;
  if (!str0_concatc(&outputline,0,":")) return -1;
  return 1;
}

int printreg_write;

int printreg(void)
{
  if (!str0_concatc(&outputline,0,printreg_write ? ">" : "live<")) return -1;
  if (!str0_concat(&outputline,0,&v)) return -1;
  if (!str0_copy(&key,0,&n)) return -1;
  if (!str0_concatc(&key,0,printreg_write ? ">" : "=")) return -1;
  if (!str0_concat(&key,0,&v)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  switch(critbit0_firstprefixed(&values,0,&x,&key)) {
    case 1: break;
    case 0: goto done;
    default: return -1;
  }
  if (!str0_concatc(&outputline,0,"#")) return -1;
  if (!str0_concat(&outputline,0,&x)) return -1;
  if (!str0_copy(&key,0,&x)) return -1;
  if (!str0_concatc(&key,0,":")) return -1;
  switch(critbit0_firstprefixed(&allocations,0,&y,&key)) {
    case 1: break;
    case 0: goto done;
    default: return -1;
  }
  if (!str0_concatc(&outputline,0,"=")) return -1;
  if (!str0_concat(&outputline,0,&y)) return -1;
  done:
  if (!str0_concatc(&outputline,0,":")) return -1;
  return 1;
}

int printlivevalues(void)
{
  if (!str0_concatc(&outputline,0,"<")) return -1;
  if (!str0_concat(&outputline,0,&v)) return -1;
  if (!str0_concatc(&outputline,0,":")) return -1;
  return 1;
}

void doit(void)
{
  int64 i;

  if (critbit0_allprefixed(&inputlines,0,&inputline,&empty,findvars) < 1) nomem();
  if (critbit0_allprefixed(&inputlines,0,&inputline,&empty,finduses) < 1) nomem();
  if (critbit0_allprefixed(&inputlines,0,&inputline,&empty,findentries) < 1) nomem();
  if (critbit0_allprefixed(&inputlines,0,&inputline,&empty,findblocks) < 1) nomem();
  traceassignment_state = 0;
  if (critbit0_allprefixed(&assignments,0,&v,&empty,traceassignment) < 1) nomem();
  traceassignment_state = 1;
  if (critbit0_allprefixed(&reads,0,&v,&empty,traceassignment) < 1) nomem();
  traceassignment_state = 2;
  if (critbit0_allprefixed(&blockvisible,0,&v,&empty,traceassignment) < 1) nomem();
  critbit0_clear(&traced,0);
  critbit0_clear(&totrace,0);
  if (!str0_copyc(&value,0,"0")) nomem();
  if (critbit0_allprefixed(&assignments,0,&x,&empty,tracevalue) < 1) nomem();
  if (critbit0_allprefixed(&reads,0,&x,&empty,tracevalue) < 1) nomem();
  if (critbit0_allprefixed(&blocklive,0,&x,&empty,tracevalue) < 1) nomem();

  allocate_state = 1;
  critbit0_clear(&traced,0);
  critbit0_clear(&totrace,0);
  if (critbit0_allprefixed(&entries,0,&n,&empty,allocate) < 1) nomem();

  for (i = 0;i < inputlinesnum;++i) {
    if (!str0_uint64(&reglimitprefix,0,i)) nomem();
    if (!str0_concatc(&reglimitprefix,0,"<")) nomem();
    if (critbit0_allprefixed(&reglimitok,0,&x,&reglimitprefix,preallocate) < 1) nomem();
    if (!str0_uint64(&reglimitprefix,0,i)) nomem();
    if (!str0_concatc(&reglimitprefix,0,">")) nomem();
    if (critbit0_allprefixed(&reglimitok,0,&x,&reglimitprefix,preallocate) < 1) nomem();
  }

  allocate_state = 2;
  critbit0_clear(&traced,0);
  critbit0_clear(&totrace,0);
  if (critbit0_allprefixed(&entries,0,&n,&empty,allocate) < 1) nomem();

  for (i = 0;i < inputlinesnum;++i) {
    if (!str0_uint64(&n,0,i)) nomem();
    if (!str0_copy(&key,0,&n)) nomem();
    if (!str0_concatc(&key,0,":")) nomem();
    if (critbit0_firstprefixed(&inputlines,0,&inputline,&key) < 1) nomem();
    str0_free(&outputline,0);
    convert_state = 0;
    if (str0_colon(&inputfield,0,&inputline,convert) < 1) nomem();
    printreg_write = 0;
    if (!str0_copy(&key,0,&n)) nomem();
    if (!str0_concatc(&key,0,"=")) nomem();
    if (critbit0_allprefixed(&blocklive,0,&v,&key,printreg) < 1) nomem();
    if (critbit0_contains(&entries,&n)) {
      printreg_write = 1;
      if (critbit0_allprefixed(&inputregs,0,&v,&empty,printreg) < 1) nomem();
    }
    if (!str0_copy(&key,0,&n)) nomem();
    if (!str0_concatc(&key,0,"<")) nomem();
    if (critbit0_allprefixed(&finalreads,0,&v,&key,printkill) < 1) nomem();
    str0_putline(&outputline);

    if (!str0_copyc(&outputline,0,"op:livefloat80:")) nomem();
    if (!str0_copy(&key,0,&n)) nomem();
    if (!str0_concatc(&key,0,":")) nomem();
    if (critbit0_allprefixed(&float80after,0,&v,&key,printlivevalues) < 1) nomem();
    str0_putline(&outputline);
  }
}

int main(int argc,char **argv)
{
  for (;;)
    switch(str0_getline(&inputline,0)) {
      case 1: case 2:
	if (!str0_uint64(&key,0,inputlinesnum)) nomem();
	if (!str0_concatc(&key,0,":")) nomem();
	if (!str0_concat(&key,0,&inputline)) nomem();
        if (!critbit0_insert(&inputlines,0,&key)) nomem();
	++inputlinesnum;
        break;
      case 0:
	doit();
	return 0;
      default:
	return 111; /* XXX: louder? */
    }
}
