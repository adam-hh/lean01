#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "sha.h"
#include "ajsmn.h"
#include "base64.h"
#include "aes.h"
#include "bitArray.h"

static void initialize_aes_sbox(uint8_t sbox[256]);
static void initialize_aes_rcon(uint8_t rcon[16]);
uint8_t SMatrixMap(uint8_t in);
uint8_t SInvMatrixMap(uint8_t in);

int main(int argc, char *argv[])
{
	/*
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
	*/
	AESKeySet kset, pmkset;
	uint8_t ukey[AESKey128 >> 3] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
	uint8_t ukey192[AESKey192 >> 3] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
		0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x15, 0x15, 0x16, 0x17
	};
	uint8_t ukey256[AESKey256 >> 3] = {
		0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
		0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
	};

	uint8_t plain[] = {0x32, 0x88, 0x31, 0xe0, 0x43, 0x5a, 0x31, 0x37, 0xf6, 0x30, 0x98, 0x07, 0xa8, 0x8d, 0xa2, 0x34};
	uint8_t plaina[] = {0x12, 0x34, 0x56, 0x78, 0x01, 0x02, 0x03, 0x04, 0x11, 0x12, 0x13, 0x14, 0x34, 0xa2, 0x8d, 0xa8};
	//AES_BLOCK plainb = {0x4154e80b, 0x21df136b, 0x901a971a, 0xb5763506};
	uint8_t plainb[] = {0x2b,0x36,0x8a,0x61,0x24,0xde,0x12,0x6e,0x95,0x1b,0x96,0x1f,0x29,0x59,0x1a,0x9a};
	uint8_t cypher[16] = {0};
	uint8_t *ptr;

	AESSetKey(ukey, AESKey128, &kset);
	//AESSetKey(ukey192, AESKey192, &kset);
	//AESSetKey(ukey256, AESKey256, &kset);
	
	/*
	ptr = (uint8_t *)kset.key;
	for (int i = 0; i < ((kset.rounds + 1) << 4); i++) {
		printf("%02x%c", ptr[i], (i + 1) % 4 == 0 ? ' ' : 0x01);
	}
	putchar('\n');
	*/

	
	AESEncrpto((uint32_t *)plaina, (uint32_t *)cypher, &kset);

	printf("plaina: ");
	for (int i = 0; i < sizeof(plaina); i++) {
		printf("%02x%c", plaina[i], i == 15 ? '\n' : ',');
	}
	printf("cypher: ");
	for (int i = 0; i < sizeof(cypher); i++) {
		printf("%02x%c", cypher[i], i == 15 ? '\n' : ',');
	}

	AESEncrpto((uint32_t *)plainb, (uint32_t *)cypher, &kset);
	printf("plainb: ");
	for (int i = 0; i < sizeof(plaina); i++) {
		printf("%02x%c", plainb[i], i == 15 ? '\n' : ',');
	}
	printf("cypher: ");
	for (int i = 0; i < sizeof(cypher); i++) {
		printf("%02x%c", cypher[i], i == 15 ? '\n' : ',');
	}

	printf("plaina XOR plainb:\n");
	for (int j = 0; j < 16; j++) {
		printf("0x%02x%c", plaina[j] ^ plainb[j], (j == 15) ? '\n' : ',');
	}
	/*
	AESDecrpto(cypher, plain, &kset);
	ptr = (uint8_t *)plain;
	printf("plain: ");
	for (int i = 0; i < sizeof(plain); i++) {
		printf("%02x%c", ptr[i], (i + 1) % 4 == 0 ? ' ' : 0x01);
	}

	ptr = (uint8_t *)cypher;
	printf("\ncypher: ");
	for (int i = 0; i < sizeof(cypher); i++) {
		printf("%02x%c", ptr[i], (i + 1) % 4 == 0 ? ' ' : 0x01);
	}
	putchar('\n');
	*/
	/*
	printf("S_Box:\n");
	for (int m = 0; m < 256; m++) {
		printf("%02x%c", SMatrixMap(m), (m + 1) % 16 == 0 ? '\n' : ' ');
	}
	printf("S_INV_Box:\n");
	for (int m = 0; m < 256; m++) {
		printf("%02x%c", SInvMatrixMap(m), (m + 1) % 16 == 0 ? '\n' : ' ');
	}
	*/
}

#define ROTL8(x,shift) ((uint8_t) ((x) << (shift)) | ((x) >> (8 - (shift))))

static void initialize_aes_sbox(uint8_t sbox[256])
{
	uint8_t p = 1, q = 1;
	
	/* loop invariant: p * q == 1 in the Galois field */
	do {
		/* multiply p by 3 */
		p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);

		/* divide q by 3 (equals multiplication by 0xf6) */
		q ^= q << 1;
		q ^= q << 2;
		q ^= q << 4;
		q ^= q & 0x80 ? 0x09 : 0;

		/* compute the affine transformation */
		uint8_t xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);

		sbox[p] = xformed ^ 0x63;
	} while (p != 1);

	/* 0 is a special case since it has no inverse */
	sbox[0] = 0x63;
}
static void initialize_aes_rcon(uint8_t rcon[16])
{
	rcon[0] = 0;
	rcon[1] = 0x01;
	for (int i = 2; i < 16; i++) {
		rcon[i] = (rcon[i -1] & 0x80) ? (rcon[i -1] << 1) ^ 0x1b : (rcon[i -1] << 1);
	}
}