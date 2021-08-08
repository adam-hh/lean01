#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <openssl/opensslconf.h>
#include <openssl/evp.h>
#include <crypto/evp.h>
#include <openssl/rsa.h>

int main(int argc, char *argv[])
{
  EVP_PKEY_CTX  *kctx;
  EVP_PKEY      *params;
  EVP_PKEY      *key;

  params = EVP_PKEY_new();
  key    = EVP_PKEY_new();
  if(params == NULL)
  {
    if(!(kctx = EVP_PKEY_CTX_new(params, NULL))) goto err; 
  }
  else
  {
    /* Create context for the key generation */
    if(!(kctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL))) goto err;
  }

  if(!EVP_PKEY_keygen_init(kctx)) goto err; 

  /* RSA keys set the key length during key generation rather than parameter generation! */
  if(EVP_PKEY_RSA == EVP_PKEY_RSA)
  {
    if(!EVP_PKEY_CTX_set_rsa_keygen_bits(kctx, 2048)) goto err;
  }

  /* Generate the key */
  if (!EVP_PKEY_keygen(kctx, &key)) 
    goto err;
  else {
    printf("sucess \n");
    RSA_print_fp(stdout, key->pkey.rsa, 0);
    exit(0);
  }

  err:
  printf("error ocured.\n");
}
