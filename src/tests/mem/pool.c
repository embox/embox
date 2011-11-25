/**
 * @file
 *
 * @brief
 *
 * @date 25.11.2011
 * @author Anton Bondarev
 */

#include <embox/test.h>

#include <mem/misc/pool.h>

#define OBJECTS_QUANTITY 0x10

struct test_obj {
	int a;
	char b;
};

//POOL_DEF(pool_name, struct test_obj, OBJECTS_QUANTITY);

EMBOX_TEST_SUITE("fixed size pool test");

TEST_CASE("single object allocation") {

}

TEST_CASE("test object freeing") {
	//struct test_obj* objs[OBJECTS_QUANTITY];

}



