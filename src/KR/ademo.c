#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include "primitiveStack.h"
#include "singleList.h"
#include "ajsmn.h"

static char *types[] = {
  "JSMN_UNDEFINED",
  "JSMN_OBJECT",
  "JSMN_ARRAY",
  "JSMN_STRING",
  "JSMN_PRIMITIVE",
  "JSMN_KEY"
};

int main(int argc, char *argv[])
{
  char rbuf[10240];
  FILE *fp;
  int n;
  jsmntok_t *tokens, *tok;

  tokens = NULL;
  if ((fp = fopen(argv[1], "r")) == NULL)
    err_sys("error: fopen %s fail", argv[1]);
  n = fread(rbuf, sizeof(char), sizeof(rbuf) - 1, fp);
  rbuf[n] = '\0';
  n = jsmn_Parse(rbuf, sizeof(rbuf), &tokens);
  if ((tok = jsmn_NextTok(rbuf, argv[2], tokens, n)))
    printf("%.*s\n", tok->end - tok->start, &rbuf[tok->start]);
  else
    printf("%s not found\n", argv[2]);
  if (n > 0)
    free(tokens);
}