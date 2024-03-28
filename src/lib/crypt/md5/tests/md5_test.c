#include <stdint.h>

#include <lib/crypt/md5.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("lib/crypt/md5 test");

//data_1	"Hello, World!"
//hash_1	65a8e27d8879283831b664bd8b7f0ad4

const md5_byte_t data_1[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 
							  'W', 'o', 'r', 'l', 'd', '!'};
const md5_byte_t hash_1[] = {0x65, 0xA8, 0xE2, 0x7D, 0x88, 0x79, 0x28, 0x38,
						0x31, 0xB6, 0x64, 0xBD, 0x8B, 0x7f, 0x0A, 0xD4};
						
TEST_CASE("test 1") {
	md5_byte_t result[16];

	md5_count(data_1, sizeof(data_1), result);

	test_assert_mem_equal(result, hash_1, 16);
}

//data_2	"Many hands make light work."
//hash_2	7588e618a496a4474acbc6b28cd52c42

const md5_byte_t data_2[] = {'M','a','n','y',' ','h','a','n','d','s',' ',
							  'm','a','k','e',' ','l','i','g','h','t',' ',
							  'w','o','r','k','.'};
const md5_byte_t hash_2[] = {0x75, 0x88, 0xE6, 0x18, 0xA4, 0x96, 0xA4, 0x47,
						0x4A, 0xCB, 0xC6, 0xB2, 0x8C, 0xD5, 0x2C, 0x42};
						
TEST_CASE("test 2") {
	md5_byte_t result[16];

	md5_count(data_2, sizeof(data_2), result);

	test_assert_mem_equal(result, hash_2, 16);пше 
}
