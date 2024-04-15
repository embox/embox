#include <stdint.h>

#include <lib/crypt/b64.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("lib/crypt/b64 test");


//data_1	"Hello_world!"
//encoded_1	"SGVsbG8sIFdvcmxkIQ=="
static const char data_1[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 
							  'W', 'o', 'r', 'l', 'd', '!'};
static const char encoded_1[] = {'S','G','V','s','b','G','8','s','I','F','d',
								 'v','c','m','x','k','I','Q','=','='};


TEST_CASE("test 1  - encoding") {
	char result[sizeof(encoded_1)];
	size_t res_size;

	b64_encode(data_1, sizeof(data_1), result, sizeof(result), &res_size);

	test_assert(res_size == sizeof(encoded_1));
	test_assert_mem_equal(result, encoded_1, res_size);
}

TEST_CASE("test 1 - decoding") {
	char result[sizeof(data_1)];
	size_t res_size;

	b64_decode(encoded_1, sizeof(encoded_1), result, sizeof(result), &res_size);
	
	test_assert(res_size == sizeof(data_1));
	test_assert_mem_equal(result, data_1, res_size);
}


//data_2	"Many hands make light work."
//encoded_2	"TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"

static const char data_2[] = {'M','a','n','y',' ','h','a','n','d','s',' ',
							  'm','a','k','e',' ','l','i','g','h','t',' ',
							  'w','o','r','k','.'};
static const char encoded_2[] = {'T','W','F','u','e','S','B','o','Y','W','5','k',
								  'c','y','B','t','Y','W','t','l','I','G','x','p',
								  'Z','2','h','0','I','H','d','v','c','m','s','u'};


TEST_CASE("test 2  - encoding") {
	char result[sizeof(encoded_2)];
	size_t res_size;

	b64_encode(data_2, sizeof(data_2), result, sizeof(result), &res_size);

	test_assert(res_size == sizeof(encoded_2));
	test_assert_mem_equal(result, encoded_2, res_size);
}

TEST_CASE("test 2 - decoding") {
	char result[sizeof(data_2)];
	size_t res_size;

	b64_decode(encoded_2, sizeof(encoded_2), result, sizeof(result), &res_size);
	
	test_assert(res_size == sizeof(data_2));
	test_assert_mem_equal(result, data_2, res_size);
}
