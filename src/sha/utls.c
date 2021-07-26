#include "sha.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

/* size of buffer to read file */
#define FBUFFERSIZE 4096

/**
 * Calculate the SHA1 hash value of a specified file
 * input:		filename
 * output:	sha1dgest
 * return:	sha Error Code
 */
int SHA1FHash(const char *filename, uint8_t *sha1dgest)
{
	int							fd, r;
	int							aflag;
	struct stat			sta;
	uint8_t					fbuffer[FBUFFERSIZE];
	SHA1Context			ctx;

	if ((fd = open(filename, O_RDONLY)) == -1) {
		fprintf(stderr, "open %s error:%s\n", filename, strerror(errno));
		return shaBadParam;
	}
	if (fstat(fd, &sta) == -1) {
		fprintf(stderr, "stat %s error:%s\n", filename, strerror(errno));
		close(fd);
		return shaBadParam;
	}
	/*
	if (!S_ISREG(sta.st_mode)) {					
		fprintf(stderr, "%s is not regular file\n", filename);
		close(fd);
		return shaBadParam;
	}
	*/
	SHA1Reset(&ctx);											/* init context */
	while (1) {
		r = read(fd, fbuffer, FBUFFERSIZE);
		if (r > 0) {
			aflag = SHA1Input(&ctx, fbuffer, r);
		} else if (r == 0) {								/* EOF break loop */
			break;
		} else {
			fprintf(stderr, "read %s error:%s\n", filename, strerror(errno));
			close(fd);
			return shaBadParam;
		}
	}
	if (aflag == shaInputTooLong) {				/* input too long */
		fprintf(stderr, "fatal error:%s too long\n", filename);
		close(fd);
		return shaInputTooLong;
	}

	close(fd);
	return SHA1Result(&ctx, sha1dgest);
}

/* SHA224 edition */
int SHA224FHash(const char *filename, uint8_t *sha224dgest)
{
	int 						fd, r;
	int 						aflag;
	struct stat			sta;
	uint8_t					fbuffer[FBUFFERSIZE];
	SHA224Context		ctx;

	if ((fd = open(filename, O_RDONLY)) == -1) {
		fprintf(stderr, "open %s error:%s\n", filename, strerror(errno));
		return shaBadParam;
	}
	if (fstat(fd, &sta) == -1) {
		fprintf(stderr, "stat %s error:%s\n", filename, strerror(errno));
		close(fd);
		return shaBadParam;
	}
	if (!S_ISREG(sta.st_mode)) {					/* work on regular file only */
		fprintf(stderr, "%s is not regular file\n", filename);
		close(fd);
		return shaBadParam;
	}

	SHA224Reset(&ctx);										/* init context */
	while (1) {
		r = read(fd, fbuffer, FBUFFERSIZE);
		if (r > 0) {
			aflag = SHA224Input(&ctx, fbuffer, r);
		} else if (r == 0) {								/* EOF break loop */
			break;
		} else {
			fprintf(stderr, "read %s error:%s\n", filename, strerror(errno));
			close(fd);
			return shaBadParam;
		}
	}
	if (aflag == shaInputTooLong) {				/* input too long */
		fprintf(stderr, "fatal error:%s too long\n", filename);
		close(fd);
		return shaInputTooLong;
	}

	close(fd);
	return SHA224Result(&ctx, sha224dgest);
}

/* SHA256 edition */
int SHA256FHash(const char *filename, uint8_t *sha256dgest)
{
	int 						fd, r;
	int 						aflag;
	struct stat 		sta;
	uint8_t 				fbuffer[FBUFFERSIZE];
	SHA256Context		ctx;

	if ((fd = open(filename, O_RDONLY)) == -1) {
		fprintf(stderr, "open %s error:%s\n", filename, strerror(errno));
		return shaBadParam;
	}
	if (fstat(fd, &sta) == -1) {
		fprintf(stderr, "stat %s error:%s\n", filename, strerror(errno));
		close(fd);
		return shaBadParam;
	}
	if (!S_ISREG(sta.st_mode)) {					/* work on regular file only */
		fprintf(stderr, "%s is not regular file\n", filename);
		close(fd);
		return shaBadParam;
	}

	SHA256Reset(&ctx);										/* init context */
	while (1) {
		r = read(fd, fbuffer, FBUFFERSIZE);
		if (r > 0) {
			aflag = SHA256Input(&ctx, fbuffer, r);
		} else if (r == 0) {								/* EOF break loop */
			break;
		} else {
			fprintf(stderr, "read %s error:%s\n", filename, strerror(errno));
			close(fd);
			return shaBadParam;
		}
	}
	if (aflag == shaInputTooLong) {				/* input too long */
		fprintf(stderr, "fatal error:%s too long\n", filename);
		close(fd);
		return shaInputTooLong;
	}

	close(fd);
	return SHA256Result(&ctx, sha256dgest);
}

int SHA384FHash(const char *filename, uint8_t *sha384dgest)
{
	int 						fd, r;
	int 						aflag;
	struct stat 		sta;
	uint8_t 				fbuffer[FBUFFERSIZE];
	SHA384Context		ctx;

	if ((fd = open(filename, O_RDONLY)) == -1) {
		fprintf(stderr, "open %s error:%s\n", filename, strerror(errno));
		return shaBadParam;
	}
	if (fstat(fd, &sta) == -1) {
		fprintf(stderr, "stat %s error:%s\n", filename, strerror(errno));
		close(fd);
		return shaBadParam;
	}
	if (!S_ISREG(sta.st_mode)) {					/* work on regular file only */
		fprintf(stderr, "%s is not regular file\n", filename);
		close(fd);
		return shaBadParam;
	}

	SHA384Reset(&ctx);										/* init context */
	while (1) {
		r = read(fd, fbuffer, FBUFFERSIZE);
		if (r > 0) {
			aflag = SHA384Input(&ctx, fbuffer, r);
		} else if (r == 0) {								/* EOF break loop */
			break;
		} else {
			fprintf(stderr, "read %s error:%s\n", filename, strerror(errno));
			close(fd);
			return shaBadParam;
		}
	}
	if (aflag == shaInputTooLong) {				/* input too long */
		fprintf(stderr, "fatal error:%s too long\n", filename);
		close(fd);
		return shaInputTooLong;
	}

	close(fd);
	return SHA384Result(&ctx, sha384dgest);
}

int SHA512FHash(const char *filename, uint8_t *sha512dgest)
{
	int 						fd, r;
	int 						aflag;
	struct stat 		sta;
	uint8_t 				fbuffer[FBUFFERSIZE];
	SHA512Context		ctx;

	if ((fd = open(filename, O_RDONLY)) == -1) {
		fprintf(stderr, "open %s error:%s\n", filename, strerror(errno));
		return shaBadParam;
	}
	if (fstat(fd, &sta) == -1) {
		fprintf(stderr, "stat %s error:%s\n", filename, strerror(errno));
		close(fd);
		return shaBadParam;
	}
	if (!S_ISREG(sta.st_mode)) {					/* work on regular file only */
		fprintf(stderr, "%s is not regular file\n", filename);
		close(fd);
		return shaBadParam;
	}

	SHA512Reset(&ctx);										/* init context */
	while (1) {
		r = read(fd, fbuffer, FBUFFERSIZE);
		if (r > 0) {
			aflag = SHA512Input(&ctx, fbuffer, r);
		} else if (r == 0) {								/* EOF break loop */
			break;
		} else {
			fprintf(stderr, "read %s error:%s\n", filename, strerror(errno));
			close(fd);
			return shaBadParam;
		}
	}
	if (aflag == shaInputTooLong) {				/* input too long */
		fprintf(stderr, "fatal error:%s too long\n", filename);
		close(fd);
		return shaInputTooLong;
	}

	close(fd);
	return SHA512Result(&ctx, sha512dgest);
}

int SHA512_224FHash(const char *filename, uint8_t *sha512224digest)
{
	int 						fd, r;
	int 						aflag;
	struct stat 		sta;
	uint8_t 				fbuffer[FBUFFERSIZE];
	SHA512Context		ctx;

	if ((fd = open(filename, O_RDONLY)) == -1) {
		fprintf(stderr, "open %s error:%s\n", filename, strerror(errno));
		return shaBadParam;
	}
	if (fstat(fd, &sta) == -1) {
		fprintf(stderr, "stat %s error:%s\n", filename, strerror(errno));
		close(fd);
		return shaBadParam;
	}
	if (!S_ISREG(sta.st_mode)) {					/* work on regular file only */
		fprintf(stderr, "%s is not regular file\n", filename);
		close(fd);
		return shaBadParam;
	}

	SHA512_224Reset(&ctx);								/* init context */
	while (1) {
		r = read(fd, fbuffer, FBUFFERSIZE);
		if (r > 0) {
			aflag = SHA512_224Input(&ctx, fbuffer, r);
		} else if (r == 0) {								/* EOF break loop */
			break;
		} else {
			fprintf(stderr, "read %s error:%s\n", filename, strerror(errno));
			close(fd);
			return shaBadParam;
		}
	}
	if (aflag == shaInputTooLong) {				/* input too long */
		fprintf(stderr, "fatal error:%s too long\n", filename);
		close(fd);
		return shaInputTooLong;
	}

	close(fd);
	return SHA512_224Result(&ctx, sha512224digest);
}

int SHA512_256FHash(const char *filename, uint8_t *sha512256dgest)
{
	int 						fd, r;
	int 						aflag;
	struct stat 		sta;
	uint8_t 				fbuffer[FBUFFERSIZE];
	SHA512Context		ctx;

	if ((fd = open(filename, O_RDONLY)) == -1) {
		fprintf(stderr, "open %s error:%s\n", filename, strerror(errno));
		return shaBadParam;
	}
	if (fstat(fd, &sta) == -1) {
		fprintf(stderr, "stat %s error:%s\n", filename, strerror(errno));
		close(fd);
		return shaBadParam;
	}
	if (!S_ISREG(sta.st_mode)) {					/* work on regular file only */
		fprintf(stderr, "%s is not regular file\n", filename);
		close(fd);
		return shaBadParam;
	}

	SHA512_256Reset(&ctx);								/* init context */
	while (1) {
		r = read(fd, fbuffer, FBUFFERSIZE);
		if (r > 0) {
			aflag = SHA512_256Input(&ctx, fbuffer, r);
		} else if (r == 0) {								/* EOF break loop */
			break;
		} else {
			fprintf(stderr, "read %s error:%s\n", filename, strerror(errno));
			close(fd);
			return shaBadParam;
		}
	}
	if (aflag == shaInputTooLong) {				/* input too long */
		fprintf(stderr, "fatal error:%s too long\n", filename);
		close(fd);
		return shaInputTooLong;
	}

	close(fd);
	return SHA512_256Result(&ctx, sha512256dgest);
}

/**
 * Calculate the SHA1 hash value of a specified arrary
 * input:		arr, len
 * output:	sha1dgest
 * return:	sha Error Code
 */
int SHA1ArryHash(const void *arr, unsigned int len, uint8_t *sha1dgest)
{
	SHA1Context ctx;

	SHA1Reset(&ctx);
	if (SHA1Input(&ctx, arr, len) == shaInputTooLong) {
		fprintf(stderr, "fatal error, input too long\n");
		return shaInputTooLong;
	}
	return SHA1Result(&ctx, sha1dgest);
}

/* SHA224 edition */
int SHA224ArryHash(const void *arr, unsigned int len, uint8_t *sha224dgest)
{
	SHA224Context ctx;

	SHA224Reset(&ctx);
	if (SHA224Input(&ctx, arr, len) == shaInputTooLong) {
		fprintf(stderr, "fatal error, input too long\n");
		return shaInputTooLong;
	}
	return SHA224Result(&ctx, sha224dgest);
}

/* SHA256 edition */
int SHA256ArryHash(const void *arr, unsigned int len, uint8_t *sha256dgest)
{
	SHA256Context ctx;

	SHA256Reset(&ctx);
	if (SHA256Input(&ctx, arr, len) == shaInputTooLong) {
		fprintf(stderr, "fatal error, input too long\n");
		return shaInputTooLong;
	}
	return SHA256Result(&ctx, sha256dgest);
}

int SHA384ArryHash(const void *arr, unsigned int len, uint8_t *sha384dgest)
{
	SHA384Context ctx;

	SHA384Reset(&ctx);
	if (SHA384Input(&ctx, arr, len) == shaInputTooLong) {
		fprintf(stderr, "fatal error, input too long\n");
		return shaInputTooLong;
	}
	return SHA384Result(&ctx, sha384dgest);
}

int SHA512ArryHash(const void *arr, unsigned int len, uint8_t *sha512dgest)
{
	SHA512Context ctx;

	SHA512Reset(&ctx);
	if (SHA512Input(&ctx, arr, len) == shaInputTooLong) {
		fprintf(stderr, "fatal error, input too long\n");
		return shaInputTooLong;
	}
	return SHA512Result(&ctx, sha512dgest);
}

int SHA512_224ArryHash(const void *arr, unsigned int len, uint8_t *sha512224dgest)
{
	SHA512Context ctx;

	SHA512_224Reset(&ctx);
	if (SHA512_224Input(&ctx, arr, len) == shaInputTooLong) {
		fprintf(stderr, "fatal error, input too long\n");
		return shaInputTooLong;
	}
	return SHA512_224Result(&ctx, sha512224dgest);
}

int SHA512_256ArryHash(const void *arr, unsigned int len, uint8_t *sha512256dgest)
{
	SHA512Context ctx;

	SHA512_256Reset(&ctx);
	if (SHA512_256Input(&ctx, arr, len) == shaInputTooLong) {
		fprintf(stderr, "fatal error, input too long\n");
		return shaInputTooLong;
	}
	return SHA512_256Result(&ctx, sha512256dgest);
}