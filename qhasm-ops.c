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
%use str0_equalc;
%use str0_getline;
%use str0_increment;
%use str0_putline;
%use str0_uint64;
%use str0_word;
*/

#include <unistd.h>
#include "str0.h"
#include "critbit0.h"

void nomem(void)
{
  _exit(111); /* XXX: louder? */
}

critbit0_tree vtype;
critbit0_tree op;
critbit0_tree stacktypes;
critbit0_tree isio;
critbit0_tree inputregs;
critbit0_tree outputregs;
critbit0_tree callerregs;
critbit0_tree caller;

str0 stackargusescaller; int flagstackargusescaller;
str0 stackarguses;

str0 empty;
str0 xtype;
int readingfromuser = 0;
str0 inputline;
str0 inputfield;
str0 inputword;
str0 key;
str0 result;
str0 outputline;
str0 output;
str0 v;

critbit0_tree opflags;
str0 opflag;
critbit0_tree requiredflags;
str0 requiredflag;
str0 inputlinewithoutflags;
str0 tallyflags_result;


int addwordtokey(int alpha)
{
  if (alpha < 0) return 1;
  if (alpha) if (!str0_concatc(&key,0,"x")) return -1;
  if (!alpha) if (!str0_concat(&key,0,&inputword)) return -1;
  return 1;
}

int syntaxline_state;

int syntaxline_field(void)
{
  static str0 type;
  static str0 n;

  switch(syntaxline_state) {
    case 0:
      if (str0_length(&inputfield) > 0) {
        if (!str0_copyc(&key,0,"op")) return -1;
        if (str0_word(&inputword,0,&inputfield,addwordtokey) < 0) return -1;
	if (!str0_concatc(&key,0,"_")) return -1;
	if (!str0_concat(&key,0,&tallyflags_result)) return -1;
	if (!str0_concatc(&key,0,"0:")) return -1;
	if (!str0_concat(&key,0,&inputline)) return -1;
	if (!critbit0_insert(&op,0,&key)) return -1;
        break;
      }
      syntaxline_state = 1;
      return 1;
    case 1:
      if (str0_equalc(&inputfield,"")) { readingfromuser = 1; break; }
      if (str0_equalc(&inputfield,"flag")) { syntaxline_state = 20; return 1; }
      if (str0_equalc(&inputfield,"caller")) { syntaxline_state = 30; return 1; }
      if (str0_equalc(&inputfield,"stackbytes")) { syntaxline_state = 40; return 1; }
      if (str0_equalc(&inputfield,"stackargusescaller")) { syntaxline_state = 50; return 1; }
      if (str0_equalc(&inputfield,"leftbytes")) goto printasm;
      if (str0_equalc(&inputfield,"rightbytes")) goto printasm;
      if (str0_equalc(&inputfield,"name")) goto printasm;
      if (str0_equalc(&inputfield,"stackname")) goto printasm;
      if (str0_equalc(&inputfield,"stack256name")) goto printasm;
      if (str0_equalc(&inputfield,"stack512name")) goto printasm;
      if (str0_equalc(&inputfield,"stackargname")) goto printasm;
      if (str0_equalc(&inputfield,"stackalign")) goto printasm;
      break;
    case 20:
      if (!str0_copyc(&outputline,0,"type:?")) return -1;
      if (!str0_concat(&outputline,0,&inputfield)) return -1;
      if (!str0_concatc(&outputline,0,":flags:")) return -1;
      str0_putline(&outputline);
      break;
    case 30:
      if (!str0_copy(&type,0,&inputfield)) return -1;
      if (!str0_copyc(&n,0,"0")) return -1;
      syntaxline_state = 31; return 1;
    case 31:
      if (!str0_length(&inputfield)) return 1;
      if (!str0_increment(&n,0)) return -1;
      if (!str0_copy(&key,0,&type)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&n)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      if (!str0_concat(&key,0,&inputfield)) return -1;
      if (!critbit0_insert(&caller,0,&key)) return -1;
      return 1;
    case 40:
      if (!critbit0_insert(&stacktypes,0,&inputfield)) return -1;
      goto printasm;
    case 50:
      if (!str0_copy(&stackargusescaller,0,&inputfield)) return -1;
      flagstackargusescaller = 1;
      break;
  }
  return 0;

printasm:
  if (!str0_copyc(&outputline,0,"asm")) return -1;
  if (!str0_concat(&outputline,0,&inputline)) return -1;
  str0_putline(&outputline);
  return 0;
}

int tallyflags2_state;

int tallyflags2(int alpha)
{
  switch(tallyflags2_state) {
    case 0:
      if (!str0_equalc(&inputword,">")) return 0;
      tallyflags2_state = 1; return 1;
    case 1:
      if (!str0_equalc(&inputword,"?")) return 0;
      if (!str0_concatc(&tallyflags_result,0,"1")) return -1;
      return 0;
  }
}

int tallyflags(void)
{
  tallyflags2_state = 0;
  if (str0_word(&inputword,0,&inputfield,tallyflags2) < 0) return -1;
  return 1;
}

void syntaxline(void)
{
  if (!str0_copyc(&tallyflags_result,0,"")) nomem();
  if (str0_colon(&inputfield,0,&inputline,tallyflags) < 0) nomem();
  syntaxline_state = 0;
  if (str0_colon(&inputfield,0,&inputline,syntaxline_field) < 0) nomem();
}


critbit0_tree vartypes;
critbit0_tree vars;
critbit0_tree labels;
critbit0_tree opwords;
critbit0_tree vmap;
critbit0_tree vmap2;
int fallthrough;
str0 jumpto;
str0 reglimittype;
str0 reglimit; int flagreglimit;

str0 tryop_op;

str0 tryop2_field;
int tryop2_state;

str0 tryop3_word;
str0 tryop3_variable;
int tryop3_state;

uint64 tryop4_state;

str0 tryop5_word;
uint64 tryop5_state;

str0 tryop6_field;
int tryop6_state;

str0 tryop7_word;
str0 tryop7_variable;
int tryop7_state;

str0 tryop8_field;
int tryop8_state;

str0 tryop9_word;
str0 tryop9_variable;
int tryop9_state;

int tryop3(int alpha)
{
  if (alpha < 0) return 1;
  /* XXX: should do this parsing only once, stick into a long-term tree */
  switch(tryop3_state) {
    case 0:
      if (str0_equalc(&tryop3_word,">")) { tryop3_state = 5; return 1; }
      if (str0_equalc(&tryop3_word,"<")) { tryop3_state = 10; return 1; }
      if (str0_equalc(&tryop3_word,"var")) { tryop3_state = 30; return 1; }
      if (str0_equalc(&tryop3_word,"#")) { tryop3_state = 51; return 1; }
      if (str0_equalc(&tryop3_word,"enter")) { tryop3_state = 50; return 1; }
      if (str0_equalc(&tryop3_word,"input")) { tryop3_state = 70; return 1; }
      if (str0_equalc(&tryop3_word,"output")) { tryop3_state = 70; return 1; }
      if (str0_equalc(&tryop3_word,"caller")) { tryop3_state = 70; return 1; }
      break;
    case 5: /* ...v=type..., with v old; or ...?flag */
      if (str0_equalc(&tryop3_word,"?")) { tryop3_state = 20; return 1; }
      if (!str0_copy(&tryop3_variable,0,&tryop3_word)) return -1;
      tryop3_state = 11; return 1;
    case 10: /* ...v=type..., with v old */
      if (!str0_copy(&tryop3_variable,0,&tryop3_word)) return -1;
      tryop3_state = 11; return 1;
    case 11:
      if (!str0_equalc(&tryop3_word,"=")) return 0;
      tryop3_state = 12; return 1;
    case 12:
      if (!str0_concatc(&tryop3_variable,0,"=")) return -1;
      if (!str0_concat(&tryop3_variable,0,&tryop3_word)) return -1;
      if (!critbit0_insert(&vartypes,0,&tryop3_variable)) return -1;
      break;
    case 20:
      if (!str0_concat(&opflag,0,&tryop3_word)) return -1;
      return 1;
    case 30: /* .../v=type..., with v new */
      if (!str0_equalc(&tryop3_word,"/")) return 0;
      tryop3_state = 31; return 1;
    case 31:
      if (!str0_copy(&tryop3_variable,0,&tryop3_word)) return -1;
      tryop3_state = 32; return 1;
    case 32:
      if (!str0_equalc(&tryop3_word,"=")) return 0;
      tryop3_state = 33; return 1;
    case 33:
      if (!critbit0_insert(&labels,0,&tryop3_variable)) return -1;
      break;
    case 50: /* .../n..., with n new */
      if (!str0_equalc(&tryop3_word,"/")) return 0;
      tryop3_state = 51; return 1;
    case 51:
      if (!critbit0_insert(&labels,0,&tryop3_word)) return -1;
      break;
    case 70: /* .../n..., with n old or new */
      if (!str0_equalc(&tryop3_word,"/")) return 0;
      tryop3_state = 71; return 1;
    case 71:
      if (!critbit0_insert(&vars,0,&tryop3_word)) return -1;
      break;
  }
  return 0;
}

int tryop2(void)
{
  if (tryop2_state == 0) {
    tryop2_state = 1; return 1;
  }
  if (tryop2_state == 1) {
    if (!str0_copy(&tryop_op,0,&tryop2_field)) return -1;
    tryop2_state = 2; return 1;
  }
  if (!str0_copyc(&opflag,0,">?")) return -1;
  tryop3_state = 0;
  if (str0_word(&tryop3_word,0,&tryop2_field,tryop3) < 0) return -1;
  if (!str0_equalc(&opflag,">?")) {
    if (!critbit0_insert(&opflags,0,&opflag)) return -1;
  }
  return 1;
}

int tryop4(int alpha)
{
  if (alpha < 0) return 1;
  if (!str0_uint64(&key,0,tryop4_state)) return -1;
  if (!str0_concatc(&key,0,"_")) return -1;
  if (!str0_concat(&key,0,&inputword)) return -1;
  if (!critbit0_insert(&opwords,0,&key)) return -1;
  ++tryop4_state;
  return 1;
}

int tryop5(int alpha)
{
  int flagvmap2 = 1;

  if (alpha < 0) return 1;
  if (!str0_uint64(&key,0,tryop5_state)) return -1;
  if (!str0_concatc(&key,0,"_")) return -1;
  if (critbit0_firstprefixed(&opwords,0,&tryop5_word,&key) != 1) return -1;

  if (!str0_copy(&key,0,&inputword)) return -1;
  if (!str0_concatc(&key,0,"=")) return -1;

  switch (critbit0_firstprefixed(&vtype,0,&xtype,&key)) {
    case 1:
      if (critbit0_contains(&vars,&tryop5_word)) break;
      if (!str0_copy(&key,0,&tryop5_word)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      if (!str0_concat(&key,0,&xtype)) return -1;
      if (critbit0_contains(&vartypes,&key)) break;
      return 0;
    case 0:
      if (critbit0_contains(&labels,&tryop5_word)) break;
      if (str0_equal(&inputword,&tryop5_word)) { flagvmap2 = 0; break; }
      return 0;
    default:
      return -1;
  }

  if (!str0_copy(&key,0,&tryop5_word)) return -1;
  if (!str0_concatc(&key,0,"=")) return -1;
  if (!str0_concat(&key,0,&inputword)) return -1;
  if (!critbit0_insert(&vmap,0,&key)) return -1;

  if (flagvmap2)
    if (!critbit0_insert(&vmap2,0,&key)) return -1;

  ++tryop5_state;
  return 1;
}

int tryop7(int alpha)
{
  int flagstack;
  static str0 n;
  static str0 type;

  if (alpha < 0) return 1;
  switch(tryop7_state) {
    case 0:
      if (str0_equalc(&tryop7_word,"inplace")) { tryop7_state = 5; return 1; }
      if (str0_equalc(&tryop7_word,">")) { tryop7_state = 10; return 1; }
      if (str0_equalc(&tryop7_word,"<")) { tryop7_state = 20; return 1; }
      if (str0_equalc(&tryop7_word,"var")) { tryop7_state = 30; return 1; }
      if (str0_equalc(&tryop7_word,"caller")) { tryop7_state = 50; return 1; }
      if (str0_equalc(&tryop7_word,"input")) { tryop7_state = 60; return 1; }
      if (str0_equalc(&tryop7_word,"output")) { tryop7_state = 70; return 1; }
      if (str0_equalc(&tryop7_word,"enter")) { tryop7_state = 80; return 1; }
      if (str0_equalc(&tryop7_word,"label")) { tryop7_state = 90; return 1; }
      if (str0_equalc(&tryop7_word,"jump")) { tryop7_state = 100; return 1; }
      if (str0_equalc(&tryop7_word,"asm")) goto copy;
      if (str0_equalc(&tryop7_word,"fpstack")) goto copy;
      if (str0_equalc(&tryop7_word,"nofallthrough")) { fallthrough = 0; return 0; }
      if (str0_equalc(&tryop7_word,"leave")) {
        if (!str0_copyc(&outputline,0,"leave:")) return -1;
	str0_putline(&outputline);
        return 0;
      }
      break;
    case 5:
      if (!str0_equalc(&tryop7_word,">")) break;
      tryop7_state = 6; return 1;
    case 6:
      if (!str0_copy(&key,0,&tryop7_word)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vmap,0,&tryop7_variable,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!str0_concatc(&output,0,"inplace>")) return -1;
      if (!str0_concat(&output,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&output,0,":")) return -1;
      tryop7_state = 11; return 1;
    case 10:
      if (str0_equalc(&tryop7_word,"?")) {
	if (!critbit0_contains(&requiredflags,&tryop6_field)) {
	  if (!str0_concatc(&output,0,"kill")) return -1;
	}
        goto copy;
      }
      if (!str0_copy(&key,0,&tryop7_word)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vmap,0,&tryop7_variable,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!str0_concatc(&output,0,">")) return -1;
      if (!str0_concat(&output,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&output,0,":")) return -1;
      tryop7_state = 11; return 1;
    case 20:
      if (str0_equalc(&tryop7_word,"?")) goto copy;
      if (!str0_copy(&key,0,&tryop7_word)) return 0;
      if (!str0_concatc(&key,0,"=")) return 0;
      switch (critbit0_firstprefixed(&vmap,0,&tryop7_variable,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!str0_concatc(&output,0,"<")) return -1;
      if (!str0_concat(&output,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&output,0,":")) return -1;
      tryop7_state = 21; return 1;
    case 21: case 11:
      if (!str0_equalc(&tryop7_word,"=")) break;
      tryop7_state += 1; return 1;
    case 22: case 12:
      if (flagstackargusescaller)
        if (critbit0_contains(&isio,&tryop7_variable)) {
          if (!str0_concatc(&output,0,"<")) return -1;
          if (!str0_concat(&output,0,&stackarguses)) return -1;
          if (!str0_concatc(&output,0,":")) return -1;
	}
      if (!str0_copy(&reglimittype,0,&tryop7_word)) return -1;
      if (!str0_copyc(&reglimit,0,"reglimit:")) return -1;
      if (!str0_concat(&reglimit,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&reglimit,0,":")) return -1;
      tryop7_state += 1; return 1;
    case 23: case 13:
      if (!str0_equalc(&tryop7_word,"#")) break;
      tryop7_state += 1; return 1;
    case 24:
      flagreglimit = 1;
      if (!str0_concatc(&reglimit,0,"=")) return -1;
      if (!str0_concat(&reglimit,0,&reglimittype)) return -1;
      if (!str0_concatc(&reglimit,0,"#")) return -1;
      if (!str0_concat(&reglimit,0,&tryop7_word)) return -1;
      if (!str0_concatc(&reglimit,0,":")) return -1;
    case 14:
      tryop7_state += 1; return 1;
    case 25: case 15:
      if (!str0_equalc(&tryop7_word,",")) break;
      tryop7_state -= 1; return 1;
    case 30:
      if (!str0_equalc(&tryop7_word,"/")) break;
      tryop7_state = 31; return 1;
    case 31:
      if (!str0_copy(&key,0,&tryop7_word)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vmap,0,&tryop7_variable,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      tryop7_state = 32; return 1;
    case 32:
      if (!str0_equalc(&tryop7_word,"=")) break;
      tryop7_state = 33; return 1;
    case 33:
      if (!str0_copyc(&key,0,"type:")) return -1;
      if (!str0_concat(&key,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&tryop7_word)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      str0_putline(&key);
      if (!str0_copy(&key,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      if (!str0_concat(&key,0,&tryop7_word)) return -1;
      if (!critbit0_insert(&vtype,0,&key)) return -1;
      break;
    case 50:
      if (!str0_equalc(&tryop7_word,"/")) break;
      tryop7_state = 51; return 1;
    case 51:
      if (!str0_copy(&key,0,&tryop7_word)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vmap,0,&tryop7_variable,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!str0_copy(&key,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vtype,0,&type,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!str0_copy(&key,0,&type)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      switch (critbit0_firstprefixed(&callerregs,0,&n,&key)) {
        case 1:
          if (!str0_copy(&key,0,&type)) return -1;
          if (!str0_concatc(&key,0,":")) return -1;
          if (!str0_concat(&key,0,&n)) return -1;
          if (!critbit0_delete(&callerregs,0,&key)) return -1;
	  break;
	case 0:
	  if (!str0_copyc(&n,0,"0")) return -1;
	  break;
	default: return -1;
      }
      if (!str0_increment(&n,0)) return -1;
      if (!str0_copy(&key,0,&type)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&n)) return -1;
      if (!critbit0_insert(&callerregs,0,&key)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&caller,0,&n,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!str0_copy(&key,0,&type)) return -1;
      if (!str0_concatc(&key,0,"#")) return -1;
      if (!str0_concat(&key,0,&n)) return -1;
      if (!str0_copyc(&outputline,0,"inputreg:")) return -1;
      if (!str0_concat(&outputline,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&outputline,0,":")) return -1;
      if (!str0_concat(&outputline,0,&key)) return -1;
      if (!str0_concatc(&outputline,0,":")) return -1;
      str0_putline(&outputline);
      if (!str0_copyc(&outputline,0,"outputreg:")) return -1;
      if (!str0_concat(&outputline,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&outputline,0,":")) return -1;
      if (!str0_concat(&outputline,0,&key)) return -1;
      if (!str0_concatc(&outputline,0,":")) return -1;
      str0_putline(&outputline);
      if (str0_equal(&key,&stackargusescaller))
        if (!str0_copy(&stackarguses,0,&tryop7_variable)) return -1;
      break;
    case 60:
      if (!str0_equalc(&tryop7_word,"/")) break;
      tryop7_state = 61; return 1;
    case 61:
      if (!str0_copy(&key,0,&tryop7_word)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vmap,0,&tryop7_variable,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!critbit0_insert(&isio,0,&tryop7_variable)) return -1;
      if (!str0_copy(&key,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vtype,0,&type,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      flagstack = critbit0_contains(&stacktypes,&type);
      if (!str0_copy(&key,0,&type)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      switch (critbit0_firstprefixed(&inputregs,0,&n,&key)) {
        case 1:
          if (!str0_copy(&key,0,&type)) return -1;
          if (!str0_concatc(&key,0,":")) return -1;
          if (!str0_concat(&key,0,&n)) return -1;
          if (!critbit0_delete(&inputregs,0,&key)) return -1;
	  break;
	case 0:
	  if (!str0_copyc(&n,0,"0")) return -1;
	  break;
	default: return -1;
      }
      if (!str0_increment(&n,0)) return -1;
      if (!str0_copy(&key,0,&type)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&n)) return -1;
      if (!critbit0_insert(&inputregs,0,&key)) return -1;
      if (!str0_copyc(&outputline,0,"inputreg:")) return -1;
      if (!str0_concat(&outputline,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&outputline,0,":")) return -1;
      if (!str0_concat(&outputline,0,&type)) return -1;
      if (!str0_concatc(&outputline,0,"#")) return -1;
      if (flagstack) if (!str0_concatc(&outputline,0,"-")) return -1;
      if (!str0_concat(&outputline,0,&n)) return -1;
      if (!str0_concatc(&outputline,0,":")) return -1;
      str0_putline(&outputline);
      break;
    case 70:
      if (!str0_equalc(&tryop7_word,"/")) break;
      tryop7_state = 71; return 1;
    case 71:
      if (!str0_copy(&key,0,&tryop7_word)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vmap,0,&tryop7_variable,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!critbit0_insert(&isio,0,&tryop7_variable)) return -1;
      if (!str0_copy(&key,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vtype,0,&type,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      flagstack = critbit0_contains(&stacktypes,&type);
      if (!str0_copy(&key,0,&type)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      switch (critbit0_firstprefixed(&outputregs,0,&n,&key)) {
        case 1:
          if (!str0_copy(&key,0,&type)) return -1;
          if (!str0_concatc(&key,0,":")) return -1;
          if (!str0_concat(&key,0,&n)) return -1;
          if (!critbit0_delete(&outputregs,0,&key)) return -1;
	  break;
	case 0:
	  if (!str0_copyc(&n,0,"0")) return -1;
	  break;
	default: return -1;
      }
      if (!str0_increment(&n,0)) return -1;
      if (!str0_copy(&key,0,&type)) return -1;
      if (!str0_concatc(&key,0,":")) return -1;
      if (!str0_concat(&key,0,&n)) return -1;
      if (!critbit0_insert(&outputregs,0,&key)) return -1;
      if (!str0_copyc(&outputline,0,"outputreg:")) return -1;
      if (!str0_concat(&outputline,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&outputline,0,":")) return -1;
      if (!str0_concat(&outputline,0,&type)) return -1;
      if (!str0_concatc(&outputline,0,"#")) return -1;
      if (flagstack) if (!str0_concatc(&outputline,0,"-")) return -1;
      if (!str0_concat(&outputline,0,&n)) return -1;
      if (!str0_concatc(&outputline,0,":")) return -1;
      str0_putline(&outputline);
      break;
    case 80:
      if (!str0_equalc(&tryop7_word,"/")) break;
      tryop7_state = 81; return 1;
    case 81:
      if (!str0_copy(&key,0,&tryop7_word)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vmap,0,&tryop7_variable,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!str0_copyc(&outputline,0,"enter:")) return -1;
      if (!str0_concat(&outputline,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&outputline,0,":")) return -1;
      str0_putline(&outputline);
      return 0;
    case 90:
      if (!str0_equalc(&tryop7_word,"/")) break;
      tryop7_state = 91; return 1;
    case 91:
      if (!str0_copy(&key,0,&tryop7_word)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vmap,0,&tryop7_variable,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!str0_copyc(&outputline,0,"label:")) return -1;
      if (!str0_concat(&outputline,0,&tryop7_variable)) return -1;
      if (!str0_concatc(&outputline,0,":")) return -1;
      str0_putline(&outputline);
      return 0;
    case 100:
      if (!str0_equalc(&tryop7_word,"/")) break;
      tryop7_state = 101; return 1;
    case 101:
      if (!str0_copy(&key,0,&tryop7_word)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vmap,0,&tryop7_variable,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      if (!str0_copy(&jumpto,0,&tryop7_variable)) return -1;
      return 0;
  }
  return 0;

  copy:
  if (!str0_concat(&output,0,&tryop6_field)) return -1;
  if (!str0_concatc(&output,0,":")) return -1;
  return 0;
}

int tryop6(void)
{
  if (tryop6_state == 0) { tryop6_state = 1; return 1; }
  if (tryop6_state == 1) { tryop6_state = 2; return 1; }
  tryop7_state = 0;
  str0_free(&reglimittype,0);
  str0_free(&reglimit,0);
  flagreglimit = 0;
  if (str0_word(&tryop7_word,0,&tryop6_field,tryop7) < 0) return -1;
  if (flagreglimit) str0_putline(&reglimit);
  return 1;
}

int tryop9(int alpha)
{
  int flagstack;
  static str0 n;
  static str0 type;

  if (alpha < 0) return 1;
  switch(tryop9_state) {
    case 0:
      if (str0_equalc(&tryop9_word,">")) { tryop9_state = 10; return 1; }
      break;
    case 10:
      if (str0_equalc(&tryop9_word,"?")) break;
      if (!str0_copy(&key,0,&tryop9_word)) return -1;
      if (!str0_concatc(&key,0,"=")) return -1;
      switch (critbit0_firstprefixed(&vmap,0,&tryop9_variable,&key)) {
        case 1: break;
	case 0: return 0;
	default: return -1;
      }
      tryop9_state = 11; return 1;
    case 11:
      if (!str0_equalc(&tryop9_word,"=")) break;
      tryop9_state += 1; return 1;
    case 12:
      if (!str0_copy(&reglimittype,0,&tryop9_word)) return -1;
      if (!str0_copyc(&reglimit,0,"reglimit:")) return -1;
      if (!str0_concat(&reglimit,0,&tryop9_variable)) return -1;
      tryop9_state += 1; return 1;
    case 13:
      if (!str0_equalc(&tryop9_word,"#")) break;
      tryop9_state += 1; return 1;
    case 14:
      flagreglimit = 1;
      if (!str0_concatc(&reglimit,0,":=")) return -1;
      if (!str0_concat(&reglimit,0,&reglimittype)) return -1;
      if (!str0_concatc(&reglimit,0,"#")) return -1;
      if (!str0_concat(&reglimit,0,&tryop9_word)) return -1;
      if (!str0_concatc(&reglimit,0,":")) return -1;
      tryop9_state += 1; return 1;
    case 15:
      if (!str0_equalc(&tryop9_word,",")) break;
      tryop9_state -= 1; return 1;
  }
  return 0;
}

int tryop8(void)
{
  if (tryop8_state == 0) { tryop8_state = 1; return 1; }
  if (tryop8_state == 1) { tryop8_state = 2; return 1; }
  tryop9_state = 0;
  str0_free(&reglimittype,0);
  str0_free(&reglimit,0);
  flagreglimit = 0;
  if (str0_word(&tryop9_word,0,&tryop8_field,tryop9) < 0) return -1;
  if (flagreglimit) str0_putline(&reglimit);
  return 1;
}

int checkflag(void)
{
  return critbit0_contains(&opflags,&requiredflag);
}

int printvmap(void)
{
  if (!str0_concatc(&output,0,"vmap/")) return -1;
  if (!str0_concat(&output,0,&v)) return -1;
  if (!str0_concatc(&output,0,":")) return -1;
  return 1;
}

int tryop(void)
{
  critbit0_clear(&vartypes,0);
  critbit0_clear(&vars,0);
  critbit0_clear(&labels,0);
  critbit0_clear(&opwords,0);
  critbit0_clear(&vmap,0);
  critbit0_clear(&vmap2,0);
  critbit0_clear(&opflags,0);
  str0_free(&tryop_op,0);
  str0_free(&jumpto,0);
  if (!str0_copyc(&output,0,":")) return -1;
  fallthrough = 1;

  tryop2_state = 0;
  if (str0_colon(&tryop2_field,0,&result,tryop2) < 0) return -1;

  switch(critbit0_allprefixed(&requiredflags,0,&requiredflag,&empty,checkflag)) {
    case 1: break;
    case 0: return 1;
    default: return -1;
  }

  tryop4_state = 0;
  tryop5_state = 0;
  if (str0_word(&inputword,0,&tryop_op,tryop4) < 0) return -1;
  if (str0_word(&inputword,0,&inputlinewithoutflags,tryop5) < 0) return -1;
  if (tryop4_state != tryop5_state) return 1;

  tryop6_state = 0;
  if (str0_colon(&tryop6_field,0,&result,tryop6) < 0) return -1;

  if (critbit0_allprefixed(&vmap2,0,&v,&empty,printvmap) < 0) return -1;

  if (str0_length(&jumpto) > 0) {
    if (!str0_copyc(&outputline,0,fallthrough ? "maybegoto:" : "goto:")) return -1;
    if (!str0_concat(&outputline,0,&jumpto)) return -1;
    if (!str0_concat(&outputline,0,&output)) return -1;
    str0_putline(&outputline);
  }

  if (!str0_copyc(&outputline,0,"op:")) return -1;
  if (!str0_concat(&outputline,0,&inputline)) return -1;
  if (!str0_concat(&outputline,0,&output)) return -1;
  str0_putline(&outputline);

  tryop8_state = 0;
  if (str0_colon(&tryop8_field,0,&result,tryop8) < 0) return -1;

  return 0;
}

int addwordtokeyandflags(int alpha)
{
  if (!str0_concat(&inputlinewithoutflags,0,&inputword)) return -1;
  if (alpha < 0) return 1;

  if (str0_equalc(&inputword,"?")) {
    if (!critbit0_insert(&requiredflags,0,&requiredflag)) return -1;
    if (!str0_copyc(&inputlinewithoutflags,0,"")) return -1;
    if (!str0_copyc(&requiredflag,0,">?")) return -1;
    if (!str0_copyc(&key,0,"op")) return -1;
    return 1;
  }

  if (!str0_concat(&requiredflag,0,&inputword)) return -1;

  if (alpha) {
    if (!str0_concatc(&key,0,"x")) return -1;
  } else {
    if (!str0_concat(&key,0,&inputword)) return -1;
  }

  return 1;
}

void userline(void)
{
  critbit0_clear(&requiredflags,0);
  if (!str0_copyc(&inputlinewithoutflags,0,"")) nomem();
  if (!str0_copyc(&requiredflag,0,">?")) nomem();
  if (!str0_copyc(&key,0,"op")) nomem();

  if (str0_word(&inputword,0,&inputline,addwordtokeyandflags) < 0) nomem();
  if (!str0_concatc(&key,0,"_")) nomem();

  if (str0_equalc(&key,"op_")) return;
  switch (critbit0_allprefixed(&op,0,&result,&key,tryop)) {
    case 1:
      if (!str0_copyc(&outputline,0,"error:unknown instruction:")) nomem();
      if (!str0_concat(&outputline,0,&inputline)) nomem();
      str0_putline(&outputline);
      break;
    case 0:
      break;
    default:
      nomem();
  }
}


int main(int argc,char **argv)
{
  for (;;) {
    switch(str0_getline(&inputline,0)) {
      case 1: case 2:
        if (readingfromuser) userline();
	else syntaxline();
	break;
      case 0: 
        return 0;
      default:
        return 111; /* XXX: louder? */
    }
  }
}
