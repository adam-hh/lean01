#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "base64.h"

int main(int argc, char *argv[])
{
  B64Encoder b64enc;
  B64Decoder b64dec;
  char    *optc, *optin, *optout;
	int     opt;

  opterr = 0;
  optc = optin = optout = NULL;
  while ((opt = getopt(argc, argv, "c:i:o::")) != -1) {
    switch(opt) {
    case 'c' :
      optc = optarg;
      break;
    case 'i' :
      optin = optarg;
      break;
    case 'o' :
      optout = optarg;
      break;
    default :
      printf("usage: %s -c <enc/dec> -i <fileinput> [-o fileoutput]\n", argv[0]);
      exit(0);
    }
  }
  if ((optin == NULL) || (optc == NULL) || (optind != argc)) {
    printf("usage: %s -c <enc/dec> -i <fileinput> [-ofileoutput]\n", argv[0]);
    exit(0);
  }

  B64EncInit(&b64enc);
  B64DecInit(&b64dec);
  if ((strlen(optc) == 3) && !strcmp(optc, "enc")) {
    if (B64StandardEncodeFile(&b64enc, optin, optout) != b64Success) {
      fprintf(stderr, "failed to encode %s\n", optin);
      exit(0);
    }
    printf("%s base64(standard) code:\n%s\n", optin, b64enc.arry);
  } else if ((strlen(optc) == 3) && !strcmp(optc, "dec")) {
    if (B64StandardDecodeFile(&b64dec, optin, optout) != b64Success) {
      fprintf(stderr, "failed to decode %s\n", optin);
      exit(0);
    }
    printf("success to decode %s\n", optin);
  } else {
    printf("unknow opition %s\n", optc);
  }

  B64EncReset(&b64enc);
  B64DecReset(&b64dec);
  exit(0);
}