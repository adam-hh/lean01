#include "sha.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    uint8_t shadgset[SHA1HashSize], shadgset224[SHA224HashSize], shadgset256[SHA256HashSize];
	uint8_t shadgset384[SHA384HashSize], shadgset512[SHA512HashSize];
    uint8_t shadgset512224[SHA512224HashSize], shadgset512256[SHA512256HashSize];
    char    *opto, *optf;
	int     opt, select, i;

	opterr = 0;
    opto = optf = NULL;
    while((opt = getopt(argc, argv, "o:f:")) != -1) {
        switch(opt) {
        case 'o' :
            opto = optarg;
            break;
        case 'f' :
            optf = optarg;
            break;
        default :
            printf("usage: %s -o <option:1/224/256/512/512/224/512/256> -f <filename>\n", argv[0]);
            exit(0);
	    }
    }
    if ((opto == NULL) || (optf == NULL) || (optind != argc)) {
        printf("usage: %s -o <option:1/224/256/512/512/224/512/256> -f <filename>\n", argv[0]);
        exit(0);
    }

    select = 0;
    if ((strlen(opto) == 1) && !strcmp(opto, "1")) {
        select = 1;
    } else if ((strlen(opto) == 3) && !strcmp(opto, "224")) {
        select = 224;
    } else if ((strlen(opto) == 3) && !strcmp(opto, "256")) {
        select = 256;
    } else if ((strlen(opto) == 3) && !strcmp(opto, "384")) {
        select = 384;
    } else if ((strlen(opto) == 3) && !strcmp(opto, "512")) {
        select = 512;
    } else if ((strlen(opto) == 7) && !strcmp(opto, "512/224")) {
        select = 512224;
    } else if ((strlen(opto) == 7) && !strcmp(opto, "512/256")) {
        select = 512256;
    }

    switch(select) {
    case 1 :
        SHA1FHash(optf, shadgset);
        printf("SHA1:");
        for (i = 0; i < sizeof(shadgset); i++) {
            printf("%02x", shadgset[i]);
        }
        putchar('\n');
        break;
    case 224 :
        SHA224FHash(optf, shadgset224);
        printf("SHA224:");
        for (i = 0; i < sizeof(shadgset224); i++) {
            printf("%02x", shadgset224[i]);
        }
        putchar('\n');
        break;
    case 256 :
        SHA256FHash(optf, shadgset256);
        printf("SHA256:");
        for (i = 0; i < sizeof(shadgset256); i++) {
            printf("%02x", shadgset256[i]);
        }
        putchar('\n');
        break;
    case 384 :
        SHA384FHash(optf, shadgset384);
        printf("SHA384:");
        for (i = 0; i < sizeof(shadgset384); i++) {
            printf("%02x", shadgset384[i]);
        }
        putchar('\n');
        break;
    case 512 :
        SHA512FHash(optf, shadgset512);
        printf("SHA512:");
        for (i = 0; i < sizeof(shadgset512); i++) {
            printf("%02x", shadgset512[i]);
        }
        putchar('\n');
        break;
    case 512224 :
        SHA512_224FHash(optf, shadgset512224);
        printf("SHA512/224:");
        for (i = 0; i < sizeof(shadgset512224); i++) {
            printf("%02x", shadgset512224[i]);
        }
        putchar('\n');
        break;
    case 512256 :
        SHA512_256FHash(optf, shadgset512256);
        printf("SHA512/256:");
        for (i = 0; i < sizeof(shadgset512256); i++) {
            printf("%02x", shadgset512256[i]);
        }
        putchar('\n');
        break;
    default :
        printf("unknown option %s\n", opto);
        break;
    }
    exit(0);
}