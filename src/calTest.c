#include "apeu.h"
#include "bigNumber.h"
#include "ajsmn.h"
#include "RSAkey.h"
#include "tools.h"

static char JSON_STRING[4096];
static struct rsakey1024 rsa1024 = {{0}, {0}, {0}, {0}, {0}, {0}};
static struct rsakey2048 rsa2048 = {{0}, {0}, {0}, {0}, {0}, {0}};

int main(int argc, char *argv[])
{
    char pwd[1024] = {0};
    int fdUrandom;
    pid_t pid;

    jsmn_parser jparser;
    jsmntok_t tokarrary[128];
    jsmntok_t *atok;
    int i, r, cnt;
    N128 a128, b128, c128, p128, x128, y128, z128, *ptr128;
    N256 a256, b256, c256, p256, x256, y256, z256, *ptr256;
    N64 a64, b64, c64, p64;
    uint32_t *pa, *pb, *pc, *pp;
    /*
    if(getcwd(pwd, 1024) != NULL) {
        printf("pwd is %s\n", pwd);
    }
    */
    
    FILE *fpLogger, *fpTestdata;
    if(NULL == (fpLogger = fopen("logger", "a"))) {
        fprintf(stderr, "fopen logger error\n");
        exit(1);
    }
    if(NULL == (fpTestdata = fopen("testdata.json", "r"))) {
        fprintf(stderr, "fopen testdata.json error\n");
        exit(1);
    }

    fdUrandom = open("/dev/urandom", O_RDONLY);
    if(fdUrandom == -1) {
        fprintf(stderr, "open error on /dev/urandom\n");
        exit(1);
    }

    wzero(&a128);  dzero(&a256);
    wzero(&b128);  dzero(&b256);
    wzero(&c128);  dzero(&c256);
    wzero(&p128);  dzero(&p256);
    pa = (uint32_t *)(a128.val);
    pb = (uint32_t *)(b128.val);
    pc = (uint32_t *)(c128.val);
    pp = (uint32_t *)(p128.val);

    if(fread(JSON_STRING, 1, sizeof(JSON_STRING), fpTestdata) <= 0) {
        fprintf(stderr, "fread testdata.json error\n");
        exit(1);
    }
    if(!feof(fpTestdata)) {
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

    if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "a")) != NULL) {
        if (atok->end - atok->start > sizeof(a128.val) - 1)
            memcpy(a128.val, JSON_STRING + atok->start, sizeof(a128.val) - 1);
        else
            memcpy(a128.val, JSON_STRING + atok->start, atok->end - atok->start);
    }
    /*
    if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "b")) != NULL) {
        ptr128 = stringTow(JSON_STRING + atok->start, atok->end - atok->start);
        copyw(&b128, ptr128);
    }
    */
    if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "p1024")) != NULL) {
        ptr128 = stringTow(JSON_STRING + atok->start, atok->end - atok->start);
        copyw(&(rsa1024.p), ptr128);
    }
    if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "q1024")) != NULL) {
        ptr128 = stringTow(JSON_STRING + atok->start, atok->end - atok->start);
        copyw(&(rsa1024.q), ptr128);
    }
    if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "publicKey1024")) != NULL) {
        ptr128 = stringTow(JSON_STRING + atok->start, atok->end - atok->start);
        copyw(&(rsa1024.publicKey), ptr128);
    }
    /*
    if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "mode1024")) != NULL) {
        ptr128 = stringTow(JSON_STRING + atok->start, atok->end - atok->start);
        copyw(&(rsa1024.mode), ptr128);
    }
    if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "fai1024")) != NULL) {
        ptr128 = stringTow(JSON_STRING + atok->start, atok->end - atok->start);
        copyw(&(rsa1024.fai), ptr128);
    }
    if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "privateKey1024")) != NULL) {
        ptr128 = stringTow(JSON_STRING + atok->start, atok->end - atok->start);
        copyw(&(rsa1024.privateKey), ptr128);
    }
    */
    if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "p2048")) != NULL) {
        ptr256 = stringTod(JSON_STRING + atok->start, atok->end - atok->start);
        copyd(&(rsa2048.p), ptr256);
    }
    if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "q2048")) != NULL) {
        ptr256 = stringTod(JSON_STRING + atok->start, atok->end - atok->start);
        copyd(&(rsa2048.q), ptr256);
    }
    if ((atok = jsonfetch(JSON_STRING, tokarrary, r, "publicKey2048")) != NULL) {
        ptr256 = stringTod(JSON_STRING + atok->start, atok->end - atok->start);
        copyd(&(rsa2048.publicKey), ptr256);
    }
  
    genPrivatekey1024(&rsa1024);
    genPrivateKey2048(&rsa2048);
    /*
    printf("rsa1024.mode : \n%s\n", wToString(&(rsa1024.mode)));
    printf("rsa1024.fai : \n%s\n", wToString(&(rsa1024.fai)));
    printf("rsa1024.publicKey : \n%s\n", wToString(&(rsa1024.publicKey)));
    printf("rsa1024.p : \n%s\n", wToString(&(rsa1024.p)));
    printf("rsa1024.q : \n%s\n", wToString(&(rsa1024.q)));
    printf("rsa1024.privateKey : \n%s\n", wToString(&(rsa1024.privateKey)));
    */
    printf("rsa2048.mode : \n%s\n", dToString(&(rsa2048.mode)));
    printf("rsa2048.fai : \n%s\n", dToString(&(rsa2048.fai)));
    printf("rsa2048.publicKey : \n%s\n", dToString(&(rsa2048.publicKey)));
    printf("rsa2048.p : \n%s\n", dToString(&(rsa2048.p)));
    printf("rsa2048.q : \n%s\n", dToString(&(rsa2048.q)));
    printf("rsa2048.privateKey : \n%s\n", dToString(&(rsa2048.privateKey)));
    a256 = castwtd(&a128);

    printf("plain text a \n%s\n", a128.val);
    //printf("plain text a \n%s\n", a256.val);

    //cryptw(&a128, &rsa1024, &z128);
    cryptd(&a256, &rsa2048, &z256);
    printf("cypher text a \n%s\n", z256.val);

    //descryptw(&z128, &rsa1024, &y128);
    descryptd(&z256, &rsa2048, &y256);
    printf("decrypted text a \n%s\n", y256.val);
    

    //loggerStart(fpLogger);

    //loggerEnd(fpLogger);

    fclose(fpLogger);
    fclose(fpTestdata);
    close(fdUrandom);
    exit(0);
}