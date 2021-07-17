#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "sha.h"
#include "ajsmn.h"
#include "base64.h"

int main(int argc, char *argv[])
{
	char *plain = "The quick brown fox jumps over the lazy dog";
	static char JSON_STRING[4096];
	static char plaintext[4096];
	jsmn_parser jparser;
	jsmntok_t tokarrary[128];
	jsmntok_t *atok;
	char *optf;
	int i, r;
	FILE *fpTestdata;
	B64Encoder b64enc;
	B64Decoder b64dec;

	opterr = 0;
	switch(getopt(argc, argv, "f:")) {
	case 'f' :
		optf = optarg;
		break;
	default :
		printf("usage: %s -f filename\n", argv[0]);
		exit(0);
	}

	if(NULL == (fpTestdata = fopen("testdata.json", "r"))) {
		fprintf(stderr, "fopen testdata.json error\n");
		exit(1);
	}
	if(fread(JSON_STRING, 1, sizeof(JSON_STRING) - 1, fpTestdata) <= 0) {
		fprintf(stderr, "fread testdata.json error\n");
		exit(1);
	}

	jsmn_init(&jparser);
	r = jsmn_parse(&jparser, JSON_STRING, strlen(JSON_STRING), tokarrary, sizeof(tokarrary) / sizeof(tokarrary[0]));

	if(r < 0) {
		fprintf(stderr, "Failed to parse JSON: %d\n", r);
		exit(1);
	}

	if((r < 1) || (tokarrary[0].type != JSMN_OBJECT)) {
		fprintf(stderr, "Object expected\n");
		exit(1);
	}

	if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "c")) != NULL) {
		memcpy(plaintext, JSON_STRING + atok->start, atok->end -  atok->start);
		plaintext[atok->end - atok->start] = 0;
	}
	B64EncInit(&b64enc);
	B64DecInit(&b64dec);
	B64StandardEncode(&b64enc, plaintext, strlen(plaintext));
	printf("plaintext:\n%s\nbase64:\n%s\n", plaintext, b64enc.arry);

	B64StandardDecode(&b64enc, &b64dec);
	printf("base64:\n%s\ndecoded:\n%.*s\n", b64enc.arry, b64dec.len, (char *)(b64dec.arry));

	exit(0);
}