#include "bigNumber.h"
#include "apeu.h"

int main(int argc, char *argv[])
{
    N128 p128, a128;
    N256 p256;
    N64 p64;
    int cnt;
    size_t len;
    pid_t pid;

    if (argc != 2) {
        printf("Usage: %s [bits width of number]\n", argv[0]);
        exit(0);
    }
    
    len = atoi(argv[1]);
    len >>= 3;

    printf("generating prime number...\n");
    if(0 == (pid = fork())) {
        //cnt = primeGend(&p256, 0, &len);
        //printf("generated a %lu bits prime number\n%s\n",len << 3, dToString(&p256));
        cnt = primeGenw(&p128, 0, &len);
        printf("generated a %lu bits prime number\n%s\n",len << 3, wToString(&p128));
        exit(0);
    }
    if(0 == (pid = fork())) {
        //cnt = primeGend(&p256, 0, &len);
        //printf("generated a %lu bits prime number\n%s\n",len << 3, dToString(&p256));
        cnt = primeGenw(&p128, 0, &len);
        printf("generated a %lu bits prime number\n%s\n",len << 3, wToString(&p128));
        exit(0);
    }

    WaitUntilECHILD(NULL);
    exit(0);
}