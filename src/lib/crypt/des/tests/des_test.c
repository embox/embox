/**
 * @file
 *
 * @data Sep 19, 2022
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <lib/crypt/des.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("lib/crypt/des test");

//#define KEY_1     "0123456789ABCDEF"
//#define DATA_1    "0000000000000000"
//#define DECRYPT_1 "14AAD7F4DBB4E094"
//#define ENCRYPT_1 "D5D44FF720683D0D"

static const uint8_t key_1[] = {0x01,0x23, 0x45, 0x67, 0x89, 0xAB,0xCD,0xEF};
static const uint8_t data_1[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static const uint8_t decript_1[] = {0x14,0xAA,0xD7,0xF4,0xDB,0xB4,0xE0,0x94};
static const uint8_t encript_1[] = {0xD5,0xD4,0x4F,0xF7,0x20,0x68,0x3D,0x0D};

#define KEY_1     (void*)key_1
#define DATA_1    (void*)data_1
#define DECRYPT_1 (void*)decript_1
#define ENCRYPT_1 (void*)encript_1

TEST_CASE("des encrypt") {
	uint8_t result[sizeof(ENCRYPT_1)];

	des_encrypt(DATA_1, KEY_1, result);

	test_assert_mem_equal(result, ENCRYPT_1, sizeof(result));
}

TEST_CASE("des decrypt") {
	uint8_t result[sizeof(DECRYPT_1)];

	des_decrypt(DATA_1, KEY_1, result);

	test_assert_mem_equal(result, DECRYPT_1, sizeof(result));
}
