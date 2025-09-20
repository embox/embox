/**
 * @file
 * @brief SHA-1 algorithm test suite
 *
 * @date Sept 03, 2025
 * @author Peize Li
 */

#include <stdint.h>

#include <embox/test.h>
#include <lib/crypt/sha1.h>

EMBOX_TEST_SUITE("lib/crypt/sha1 test");

/* Test 1: Empty string */
/* data: "" */
/* hash: da39a3ee5e6b4b0d3255bfef95601890afd80709 */
const uint8_t hash_empty[] = {0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x32, 0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09};

TEST_CASE("empty string") {
	uint8_t result[SHA1_DIGEST_SIZE];

	sha1_count((const uint8_t *)"", 0, result);

	test_assert_mem_equal(result, hash_empty, SHA1_DIGEST_SIZE);
}

/* Test 2: "abc" */
/* hash: a9993e364706816aba3e25717850c26c9cd0d89d */
const uint8_t data_abc[] = {'a', 'b', 'c'};
const uint8_t hash_abc[] = {0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
    0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0, 0xd8, 0x9d};

TEST_CASE("abc") {
	uint8_t result[SHA1_DIGEST_SIZE];

	sha1_count(data_abc, sizeof(data_abc), result);

	test_assert_mem_equal(result, hash_abc, SHA1_DIGEST_SIZE);
}

/* Test 3: "The quick brown fox jumps over the lazy dog" */
/* hash: 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12 */
const uint8_t data_fox[] = {'T', 'h', 'e', ' ', 'q', 'u', 'i', 'c', 'k', ' ',
    'b', 'r', 'o', 'w', 'n', ' ', 'f', 'o', 'x', ' ', 'j', 'u', 'm', 'p', 's',
    ' ', 'o', 'v', 'e', 'r', ' ', 't', 'h', 'e', ' ', 'l', 'a', 'z', 'y', ' ',
    'd', 'o', 'g'};
const uint8_t hash_fox[] = {0x2f, 0xd4, 0xe1, 0xc6, 0x7a, 0x2d, 0x28, 0xfc,
    0xed, 0x84, 0x9e, 0xe1, 0xbb, 0x76, 0xe7, 0x39, 0x1b, 0x93, 0xeb, 0x12};

TEST_CASE("quick brown fox") {
	uint8_t result[SHA1_DIGEST_SIZE];

	sha1_count(data_fox, sizeof(data_fox), result);

	test_assert_mem_equal(result, hash_fox, SHA1_DIGEST_SIZE);
}

/* Test 4: Multi-update test */
/* data: "Hello, World!" */
/* hash: 0a0a9f2a6772942557ab5355d76af442f8f65e01 */
const uint8_t data_hello[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r',
    'l', 'd', '!'};
const uint8_t hash_hello[] = {0x0a, 0x0a, 0x9f, 0x2a, 0x67, 0x72, 0x94, 0x25,
    0x57, 0xab, 0x53, 0x55, 0xd7, 0x6a, 0xf4, 0x42, 0xf8, 0xf6, 0x5e, 0x01};

TEST_CASE("multi-update") {
	sha1_state_t ctx;
	uint8_t result[SHA1_DIGEST_SIZE];

	sha1_init(&ctx);
	sha1_update(&ctx, data_hello, 7);     /* "Hello, " */
	sha1_update(&ctx, data_hello + 7, 6); /* "World!" */
	sha1_final(&ctx, result);

	test_assert_mem_equal(result, hash_hello, SHA1_DIGEST_SIZE);
}

/* Test 5: 55-byte input (boundary case for padding) */
/* data: 55 'a' characters */
/* hash: c1c8bbdc22796e28c0e15163d20899b65621d65a */
const uint8_t data_55[] = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
    'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
    'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
    'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'};
const uint8_t hash_55[] = {0xc1, 0xc8, 0xbb, 0xdc, 0x22, 0x79, 0x6e, 0x28, 0xc0,
    0xe1, 0x51, 0x63, 0xd2, 0x08, 0x99, 0xb6, 0x56, 0x21, 0xd6, 0x5a};

TEST_CASE("55 bytes padding boundary") {
	uint8_t result[SHA1_DIGEST_SIZE];

	sha1_count(data_55, sizeof(data_55), result);

	test_assert_mem_equal(result, hash_55, SHA1_DIGEST_SIZE);
}

/* Test 6: 56-byte input (another boundary case) */
/* data: 56 'b' characters */
/* hash: 049adea016c8a8e3b7a9054aeeaa8643453bebd9 */
const uint8_t data_56[] = {'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
    'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
    'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
    'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'};
const uint8_t hash_56[] = {0x04, 0x9a, 0xde, 0xa0, 0x16, 0xc8, 0xa8, 0xe3, 0xb7,
    0xa9, 0x05, 0x4a, 0xee, 0xaa, 0x86, 0x43, 0x45, 0x3b, 0xeb, 0xd9};

TEST_CASE("56 bytes padding boundary") {
	uint8_t result[SHA1_DIGEST_SIZE];

	sha1_count(data_56, sizeof(data_56), result);

	test_assert_mem_equal(result, hash_56, SHA1_DIGEST_SIZE);
}

/* Test 7: Exactly 64 bytes (one block) */
/* data: 64 'c' characters */
/* hash: b29fcb5f41614b4ec81c87e4614ffe8b9ace1d4e */
const uint8_t data_64[] = {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c'};
const uint8_t hash_64[] = {0xb2, 0x9f, 0xcb, 0x5f, 0x41, 0x61, 0x4b, 0x4e, 0xc8,
    0x1c, 0x87, 0xe4, 0x61, 0x4f, 0xfe, 0x8b, 0x9a, 0xce, 0x1d, 0x4e};

TEST_CASE("64 bytes exactly one block") {
	uint8_t result[SHA1_DIGEST_SIZE];

	sha1_count(data_64, sizeof(data_64), result);

	test_assert_mem_equal(result, hash_64, SHA1_DIGEST_SIZE);
}
