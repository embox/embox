/**
 * @file
 *
 * @date Sep 19, 2022
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
static const uint8_t decrypt_1[] = {0x14,0xAA,0xD7,0xF4,0xDB,0xB4,0xE0,0x94};
static const uint8_t encrypt_1[] = {0xD5,0xD4,0x4F,0xF7,0x20,0x68,0x3D,0x0D};

#define KEY_1     (void*)key_1
#define DATA_1    (void*)data_1
#define DECRYPT_1 (void*)decrypt_1
#define ENCRYPT_1 (void*)encrypt_1

TEST_CASE("des encrypt 1") {
	uint8_t result[sizeof(encrypt_1)];

	des_encrypt(DATA_1, KEY_1, result);

	test_assert_mem_equal(result, ENCRYPT_1, sizeof(result));
}

TEST_CASE("des decrypt 1") {
	uint8_t result[sizeof(decrypt_1)];

	des_decrypt(DATA_1, KEY_1, result);

	test_assert_mem_equal(result, DECRYPT_1, sizeof(result));
}

//KEY_2     "FEDCBA9876543210"
//DATA_2    "0101010101010101"
//DECRYPT_2 "4A40D9A842FDE668"
//ENCRYPT_2 "AA132F32258A3C61"

static const uint8_t key_2[] = {0xFE,0xDC, 0xBA, 0x98, 0x76, 0x54,0x32,0x10};
static const uint8_t data_2[] = {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
static const uint8_t decrypt_2[] = {0x4A,0x40,0xD9,0xA8,0x42,0xFD,0xE6,0x68};
static const uint8_t encrypt_2[] = {0xAA,0x13,0x2F,0x32,0x25,0x8A,0x3C,0x61};

#define KEY_2     (void*)key_2
#define DATA_2    (void*)data_2
#define DECRYPT_2 (void*)decrypt_2
#define ENCRYPT_2 (void*)encrypt_2

TEST_CASE("des encrypt 2") {
	uint8_t result[sizeof(encrypt_2)];

	des_encrypt(DATA_2, KEY_2, result);

	test_assert_mem_equal(result, ENCRYPT_2, sizeof(result));
}

TEST_CASE("des decrypt 2") {
	uint8_t result[sizeof(decrypt_2)];

	des_decrypt(DATA_2, KEY_2, result);

	test_assert_mem_equal(result, DECRYPT_2, sizeof(result));
}
