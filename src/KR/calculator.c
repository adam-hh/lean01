#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "primitiveStack.h"
#include "calculator.h"

#define MAXOP   100
DEFINE_PRIMSTACK_OF(double)
DEFINE_PRIMSTACK_OF(char)

const int arithPriority[] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 9, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  3, 3, 1, 0, 0, 0, 0, 1
};

/**
 * A very primitive char stack
 */
#define UNGETCHBUFSIZE  100               /* stack capacity */
static char ungetchbuf[UNGETCHBUFSIZE];   /* stack */
int         bufp = 0;                     /* top of stack loc(init value should be 0) */

/* pop operation */
int getch(FILE *in)
{
  return (bufp > 0) ? ungetchbuf[--bufp] : getc(in);
}
/* push operation */
void ungetch(int c)
{
  if (bufp >= UNGETCHBUFSIZE)
    err_msg("ungetch: too many characters");
  else
    ungetchbuf[bufp++] = c;
}

/**
 * Get arithmetic OP
 * input:   in
 * output:  s
 * return:  arithmetic OP type
 */
int getop(FILE *in, char s[])
{
  int i, c, type;

  while ((s[0] = c = getch(in)) == ' ' || c == '\t')
    ;           /* skip blanks */
  s[1] = '\0';

  i = 0;
  switch(c) {
  case ADD :
  case SUB :
  case MLT :
  case DIV :
  case BRACKET1 :
  case BRACKET2 :
  case RET :
  case EOF :
    return c;
  case 's' :    /* try match sin */
    if ((s[++i] = c = getch(in)) != 'i' || (s[++i] = c = getch(in)) != 'n')
      goto sincheckfail;
    if ((type = getop(in, s)) == NUMBER)
      return FUNCSIN;
    else
      goto sincheckfail;

    sincheckfail:
    if (c != EOF && c != 'n') {
      s[++i] = '\0';
      ungetch(c);
    }
    return (type == RET) ? type : UNKNOWT;
  case 'P' :  /* try match pai */
    if ((s[++i] = c = getch(in)) != 'a' || (s[++i] = c = getch(in)) != 'i')
      goto paicheckfail;
    strcpy(s, "3.1415926");
    return NUMBER;

    paicheckfail:
    s[++i] = '\0';
    if (c != EOF)
      ungetch(c);
    return UNKNOWT;
  case 'p' :  /* try match pow */
    if ((s[++i] = c = getch(in)) != 'o' || (s[++i] = c = getch(in)) != 'w')
      goto powcheckfail;
    if ((type = getop(in, s)) == NUMBER)
      return FUNCPOW;
    else
      goto powcheckfail;
    
    powcheckfail:
    if (c != EOF && c != 'w') {
      s[++i] = '\0';
      ungetch(c);
    }
    return (type == RET) ? type : UNKNOWT;
  default :   /* try match number */
    while (!isdigit(c) && c != '.' && c != RET && c != EOF) {
      s[++i] = c = getch(in);
    }
    if (i) {
      s[i] = '\0';
      ungetch(c);
      return UNKNOWT;
    }

    if (isdigit(c))
      while (isdigit(s[++i] = c = getch(in)))
        ;
    if (c == '.')
      while (isdigit(s[++i] = c = getch(in)))
        ;
    s[i] = '\0';
    if (c != EOF)
      ungetch(c);
    return NUMBER;
  }
}

/**
 * Transfro arithmetic expression from postfix to infix
 * it is from regular to reverse polish
 */
void postToinfix(FILE *in, FILE *out)
{
  int     c;
  STACK_OF(char) *st;

  st = primstNew_char(16);
  while ((c = getc(in)) != EOF) {
    switch (c) {
    case ADD :
    case SUB :
    case MLT :
    case DIV :
      putc(' ', out);     /* a operator is actually a spliter of digits */
      if (primstTopOfStack_char(st) == EmptyTOS || arithPriority[c] > arithPriority[primstTop_char(st)]) {
        primstPush_char(st, c);
      }
      else {
        while (primstTopOfStack_char(st) != EmptyTOS) {
          if (primstTop_char(st) == BRACKET1)
            break;
          if (arithPriority[primstTopOfStack_char(st)] >= arithPriority[c])
            putc(primstPop_char(st), out);
          else
            break;
        }
        primstPush_char(st, c);
      }
      break;
    case BRACKET1 :
      putc(' ', out);
      primstPush_char(st, c);
      break;
    case BRACKET2 :
      putc(' ', out);
      while (primstTopOfStack_char(st) != EmptyTOS && primstTop_char(st) != BRACKET1) {
        putc(primstPop_char(st), out);
      }
      if (primstTopOfStack_char(st) == EmptyTOS || primstTop_char(st) != BRACKET1)
        err_msg("error: bracket mismatch");   /* less left bracket */
      else
        primstPop_char(st);
      break;
    case RET :
      while (primstTopOfStack_char(st) != EmptyTOS) {
        if (primstTop_char(st) != BRACKET1)
            putc(primstPop_char(st), out);
        else {
          err_msg("error: bracket mismatch");   /* less right bracket */
        primstPop_char(st);
        }
      }
      putc('\n', out);
      fflush(out);
      break;
    default :
      putc(c, out);
    }
  }

  while (primstTopOfStack_char(st) != EmptyTOS) {
    if (primstTop_char(st) != BRACKET1)
      putc(primstPop_char(st), out);
    else {
      err_msg("error: bracket mismatch");   /* less right bracket */
      primstPop_char(st);
    }
  }

  fflush(out);
  primstFree_char(st);
}

/**
 * A reverse Polish stype arithmetic expression calculation
 */
void reverPoland(FILE *in, FILE *out)
{
  int     type;
  double  op2, base;
  char    s[MAXOP];
  STACK_OF(double)  *st;

  st = primstNew_double(100);
  while ((type = getop(in, s)) != EOF) {
    switch (type) {
    case UNKNOWT :
      goto cmderror;
    case NUMBER :
      primstPush_double(st, atof(s));
      break;
    case FUNCSIN :
      primstPush_double(st, sin(atof(s)));
      break;
    case FUNCPOW :
      base = atof(s);
      type = getop(in, s);
      if (type != NUMBER) {
        fprintf(out, "error: pow parameter error\n");
        goto cmderror;
      }
      primstPush_double(st, pow(base, atof(s)));
      break;
    case ADD :
      primstPush_double(st, primstPop_double(st) + primstPop_double(st));
      break;
    case MLT :
      primstPush_double(st, primstPop_double(st) * primstPop_double(st));
      break;
    case SUB :
      op2 = primstPop_double(st);
      primstPush_double(st, primstPop_double(st) -op2);
      break;
    case DIV :
      op2 = primstPop_double(st);
      if (op2 != 0.0)
        primstPush_double(st, primstPop_double(st) / op2);
      else
        fprintf(out, "error: zero divisor\n");
      break;
    case '=' :
      break;
    case RET :
      if (primstTopOfStack_double(st) != EmptyTOS)
        fprintf(out, "= %.8g\n", primstPop_double(st));
      //else
      //  fprintf(out, "error: empty input\n");
      fflush(out);
      primstZero_double(st);
      bufp = 0;
      break;
    default :
      cmderror:
      fprintf(out, "error: unknown command %s\n", s);
      fflush(out);
      break;
    }
  }
  fflush(out);
  bufp = 0;
  primstFree_double(st);
}