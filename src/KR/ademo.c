#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include "primitiveStack.h"
#include "primitiveSorting.h"
#include "singleList.h"

DEFINE_PRIMSORTING_OF(char)
DEFINE_PRIMLIST_OF(char)
int main(int argc, char *argv[])
{
  /*
  int fd[2];
  FILE *fp0, *fp1;
  pid_t pid;
  struct pollfd pl[2];

  if (pipe(fd) < 0)
    err_sys("error: pipe error");
  fp0 = fdopen(fd[0], "r");
  fp1 = fdopen(fd[1], "w");

  if ((pid = fork()) < 0)
    err_sys("error: fork error");
  if (pid == 0) {
    fclose(fp1);
    reverPoland(fp0, stdout);
    exit(0);
  } else {
    fclose(fp0);
    postToinfix(stdin, fp1);
    exit(0);
  }
  */
  int fdrandom;
  char data[1024];
  if ((fdrandom = open("/dev/urandom", O_RDONLY)) == -1)
    err_sys("open /dev/urandom error");
  if (read(fdrandom, data, sizeof(data)) != sizeof(data))
    err_quit("read random error");
  //shellSort_char(data, sizeof(data));
  qsort_char(data, 0, 1023);

  for (int i = 0; i < sizeof(data); i++) {
    printf("%4d%c", data[i], (i + 1) % 20 == 0 ? '\n' : ',');
  }
  putchar('\n');
  LIST_OF(char) *cl;
  cl = listCreateList_char();
}