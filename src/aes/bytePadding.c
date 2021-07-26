/**
 * Reference : https://en.wikipedia.org/wiki/Padding_(cryptography)
 */
#include "aes.h"
#include "bitArray.h"

/**
 * ANSI X9.23
 * In ANSI X9.23, between 1 and 8 bytes are always added as padding. 
 * The block is padded with random bytes (although many implementations use 00) 
 * and the last byte of the block is set to the number of bytes added.
 */
int padANSIX923(void *src, size_t packetLen, size_t realLen)
{
  uint8_t ptr;

  ptr = (uint8_t *)src;
  for (size_t i = realLen; i < packetLen - 1; i++) {
    ptr[i] = 0;
  }
}







/**
 * ISO 10126
 * ISO 10126 (withdrawn, 2007[7][8]) specifies that the padding should be done at 
 * the end of that last block with random bytes, and the padding boundary should be 
 * specified by the last byte.
 */









/**
 * PKCS#5 and PKCS#7
 * PKCS#7 is described in RFC 5652
 * Padding is in whole bytes. The value of each added byte is the number of bytes that are added, 
 * i.e. N bytes, each of value N are added. The number of bytes added will depend on the block boundary 
 * to which the message needs to be extended.
 */






/**
 * ISO/IEC 7816-4
 * ISO/IEC 7816-4:2005[9] is identical to the bit padding scheme, applied to a plain text of N bytes. 
 * This means in practice that the first byte is a mandatory byte valued '80' (Hexadecimal) followed, 
 * if needed, by 0 to N − 1 bytes set to '00', until the end of the block is reached. 
 * ISO/IEC 7816-4 itself is a communication standard for smart cards containing a file system, 
 * and in itself does not contain any cryptographic specifications.
 */