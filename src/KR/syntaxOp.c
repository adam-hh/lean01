#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include "primitiveStack.h"


#define IN  1
#define OUT 0
/**
 * Word counter
 */
void wc(FILE *fp)
{
  int c, nl, nw, nc, state;

  state = OUT;
  nl = nw = nc = 0;
  while ((c = getc(fp)) != EOF) {
    ++nc;
    if (c == '\n')
      ++nl;
    if (c == ' ' || c == '\n' || c == '\t')
      state = OUT;
    else if (state == OUT) {
      state = IN;
      ++nw;
    }
  }
  printf("char count:%d\tword count:%d\tline count:%d\n", nc, nw, nl);
}

/**
 * Read by line
 * input:   rp, len
 * output:  ptrc
 * return:  actuall length of ptrc excluding tail \0
 */
size_t Getline(FILE *fp, char *ptrc, size_t len)
{
  int     c;
  size_t  i;

  for (i = 0; i < len - 1 && (c = getc(fp)) != EOF && c != '\n'; ++i) 
    ptrc[i] = c;
  if (c == '\n') {
    ptrc[i] = c;
    ++i;
  }
  ptrc[i] = '\0';
  return i;
}

#define LINELMAX 160
#define LINELMIN 12
/**
 * Cut line from file
 * input:   in, len
 * outut:   out
 * return:  void
 */
void cltline(FILE *in, FILE *out, size_t len)
{
  int     c, state;
  char    wd[LINELMAX + 1];
  size_t  cur, nw;

  if (len < LINELMIN || len > LINELMAX) {
    fprintf(stderr, "error: illega length %ld, must in [%d, %d]\n", len, LINELMIN, LINELMAX);
    return;
  }

  state = OUT;
  cur = nw = 0;
  while ((c = getc(in)) != EOF) {
    ++cur;
    switch (c) {
    case '\n' :
    case ' ':
    case '\t':
      if (state == IN) {
        wd[nw] = '\0';
        if (cur > len) {
          fprintf(out, "\n%s", wd);
          cur = nw;
          nw = 0;
        } else {
          fprintf(out, "%s", wd);
          nw = 0;
        }
      }
      putc(c, out);
      if (c == '\n')
        cur = 0;
      state = OUT;
      break;
    default :
      state = IN;
      if (nw == len) {
        wd[nw] = '\0';
        fprintf(stderr, "illegal word: %s\n", wd);
        return;
      }
      wd[nw++] = c;
    }
  }
}

DEFINE_PRIMSTACK_OF(char)
#define MAXLINESIZE 1024

/**
 * Remoe notation from c source file
 */
void rmNotation(FILE *in, FILE *out)
{
  char buf[MAXLINESIZE];
  size_t blen, i;
  STACK_OF(char) *cstack;

  cstack = primstNew_char(4);
  while((blen = Getline(in, buf, sizeof(buf))) != 0) {
    if (blen == sizeof(buf) - 1)
      err_msg("error: line too long:\n%.*s\n", blen, buf);
    for (i = 0; i < blen; i++) {
      if (buf[i] == 0x2f && (i + 1) < blen) {
        if (buf[i + 1] == 0x2f)
          goto lineNotation;
        else if (buf[i + 1] == 0x2a) {
          primstPush_char(cstack, 0x2f);
          primstPush_char(cstack, 0x2a);
          if (i) {
            fprintf(out, "%.*s", (int)i, buf);
            putc('\n', out);
          }
        }
      } else if (buf[i] == 0x2a && (i + 1) < blen)
        if (buf[i + 1] == 0x2f)
          goto segNotationRight;
    }
    if (i == blen && primstTopOfStack_char(cstack) == EmptyTOS) {
      fprintf(out, "%.*s", (int)i, buf);
      if (buf[i -1] != '\n')
        putc('\n', out);    /* incase some source file not ending with NL */
    } 
    continue;

    lineNotation:
    if (i) {
      fprintf(out, "%.*s", (int)i, buf);
      putc('\n', out);
    }
    continue;
    
    segNotationRight:
    if (primstPop_char(cstack) != 0x2a || primstPop_char(cstack) != 0x2f) {
      err_quit("notation synax error\n");
    }
    if (i + 2 < blen && buf[i + 2] != '\n') {
      fprintf(out, "%s", buf + i + 2);
    }
  }
  
  primstFree_char(cstack);
}