#include "sha.h"
#include "ajsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    char *plain = "The quick brown fox jumps over the lazy dog";
    SHA1Context ctx;
    SHA224Context ctx224;
    SHA256Context ctx256;
    uint8_t shadgset[SHA1HashSize], shadgset224[SHA224HashSize], shadgset256[SHA256HashSize];
	uint8_t shadgset384[SHA384HashSize], shadgset512[SHA512HashSize];
    uint8_t shadgset512224[SHA512224HashSize], shadgset512256[SHA512256HashSize];
    static char JSON_STRING[4096];
    static char plaintext[4096];
    jsmn_parser jparser;
    jsmntok_t tokarrary[128];
    jsmntok_t *atok;
    char *optf;
		int i, r;
		FILE *fpTestdata;

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
    /*
	SHA1ArryHash(plaintext, strlen(plaintext), shadgset);
	SHA224ArryHash(plaintext, strlen(plaintext), shadgset224);
	SHA256ArryHash(plaintext, strlen(plaintext), shadgset256);
	SHA384ArryHash(plaintext, strlen(plaintext), shadgset384);
	SHA512ArryHash(plaintext, strlen(plaintext), shadgset512);
    SHA512_224ArryHash(plaintext, strlen(plaintext), shadgset512224);
    SHA512_256ArryHash(plaintext, strlen(plaintext), shadgset512256);
    */


	SHA1FHash(optf, shadgset);
	SHA224FHash(optf, shadgset224);
	SHA256FHash(optf, shadgset256);
    SHA384FHash(optf, shadgset384);
    SHA512FHash(optf, shadgset512);
    SHA512_224FHash(optf, shadgset512224);
    SHA512_256FHash(optf, shadgset512256);

    for(i = 0; i < sizeof(shadgset); i++) {
        printf("%02x%c", shadgset[i], (i == SHA1HashSize - 1) ? '\n' : ' ');
    }
    for(i = 0; i < sizeof(shadgset224); i++) {
        printf("%02x%c", shadgset224[i], (i == SHA224HashSize - 1) ? '\n' : ' ');
    }
    for(i = 0; i < sizeof(shadgset256); i++) {
        printf("%02x%c", shadgset256[i], (i == SHA256HashSize - 1) ? '\n' : ' ');
    }
		for(i = 0; i < sizeof(shadgset384); i++) {
        printf("%02x%c", shadgset384[i], (i == SHA384HashSize - 1) ? '\n' : ' ');
    }
	for(i = 0; i < sizeof(shadgset512); i++) {
        printf("%02x%c", shadgset512[i], (i == SHA512HashSize - 1) ? '\n' : ' ');
    }
    for(i = 0; i < sizeof(shadgset512224); i++) {
        printf("%02x%c", shadgset512224[i], (i == SHA512224HashSize - 1) ? '\n' : ' ');
    }
    for(i = 0; i < sizeof(shadgset512256); i++) {
        printf("%02x%c", shadgset512256[i], (i == SHA512256HashSize - 1) ? '\n' : ' ');
    }
}