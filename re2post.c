
#include <string.h>
#include <stdio.h>

/*
 * Convert infix regexp re to postfix notation.
 * Insert . as explicit concatenation operator.
 * Cheesy parser, return static buffer.
 */
char*
re2post_old(char *re)
{
  int nalt, natom;
  static char buf[8000];
  char *dst;
  struct {
    int nalt;
    int natom;
  } paren[100], *p;
  
  p = paren;
  dst = buf;
  nalt = 0;
  natom = 0;
  if(strlen(re) >= sizeof buf/2)
    return NULL;
  for(; *re; re++){
    switch(*re){
      case '(':
        if(natom > 1){
          --natom;
          *dst++ = '.';
        }
        if(p >= paren+100)
          return NULL;
        p->nalt = nalt;
        p->natom = natom;
        p++;
        nalt = 0;
        natom = 0;
        break;
      case '|':
        if(natom == 0)
          return NULL;
        while(--natom > 0)
          *dst++ = '.';
        nalt++;
        break;
      case ')':
        if(p == paren)
          return NULL;
        if(natom == 0)
          return NULL;
        while(--natom > 0)
          *dst++ = '.';
        for(; nalt > 0; nalt--)
          *dst++ = '|';
        --p;
        nalt = p->nalt;
        natom = p->natom;
        natom++;
        break;
      case '*':
      case '+':
      case '?':
        if(natom == 0)
          return NULL;
        *dst++ = *re;
        break;
      default:
        if(natom > 1){
          --natom;
          *dst++ = '.';
        }
        *dst++ = *re;
        natom++;
        break;
    }
  }
  if(p != paren)
    return NULL;
  while(--natom > 0)
    *dst++ = '.';
  for(; nalt > 0; nalt--)
    *dst++ = '|';
  *dst = 0;
  return buf;
}

char* re2post_new(const char *re) {
  
  static char buf[8000];
  if (strlen(re) >= sizeof buf / 2) return NULL;
  
  struct {
    int nalt;
    int natom;
  } stack[100], *top = stack;
  
  int nalt  = 0;
  int natom = 0;
  char *dst = buf;
  for (; *re; ++re) {
    switch(*re) {
      case '(':
        if (++natom > 2) {
          *dst++ = '.';
          natom = 2;
        }
        if (top >= stack+100) return NULL;
        top->nalt  = nalt;
        top->natom = natom;
        ++top;
        nalt  = 0;
        natom = 0;
        break;
      case '|':
        if (natom == 0) return NULL;
        while (--natom > 0)
          *dst++ = '.';
        ++nalt;
        break;
      case ')':
        if (top == stack || natom == 0) return NULL;
        while (--natom > 0)
          *dst++ = '.';
        for (; nalt > 0; --nalt)
          *dst++ = '|';
        --top;
        nalt  = top->nalt;
        natom = top->natom;
        break;
      case '*':
      case '+':
      case '?':
        if (natom == 0) return NULL;
        *dst++ = *re;
        break;
      default:
        if (++natom > 2) {
          *dst++ = '.';
          natom = 2;
        }
        *dst++ = *re;
        break;
    }
  }
  if (top != stack) return NULL;
  while (--natom > 0)
    *dst++ = '.';
  for (; nalt > 0; --nalt)
    *dst++ = '|';
  *dst = 0;
  return buf;
}

char test[] = "a(bb)c";

int main() {
  printf("%s\n", re2post_old(test));
  printf("%s\n", re2post_new(test));
}
