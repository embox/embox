/**
 * @file
 *
 * @date Sep 17, 2022
 * @author Anton Bondarev
 */

#ifndef SRC_LIB_CRYPT_DES_DES_H_
#define SRC_LIB_CRYPT_DES_DES_H_

#include <stdint.h>

extern void des_encrypt(uint8_t data[8], uint8_t key[8], uint8_t result[8]);
extern void des_decrypt(uint8_t data[8], uint8_t key[8], uint8_t result[8]);

#endif /* SRC_LIB_CRYPT_DES_DES_H_ */
