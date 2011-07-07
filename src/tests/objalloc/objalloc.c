/**
 * @brief
 *
 * @date 6.07.2011
 * @author Alexandr Kalmuk
 */

#include <stdio.h>
#include <embox/test.h>
#include <mem/objalloc.h>
#include <framework/mod/members.h>
#include __impl_x(framework/mod/types.h)
#include __impl_x(framework/mod/decls.h)
#include <framework/mod/api.h>

#define OBJECT_SIZE 4
#define OBJECT_NUMBER 16

EMBOX_TEST_SUITE("simple test for macros MOD_MEMBERS_BIND and OBJALLOC_DEF");

static int flag = 0;

struct data_storage {
	void * data;
};

static int init(struct mod_members_info * info) {
	flag = 1;
	return 0;
}

static const struct mod_members_ops __init = {
		.init = &init,
};

static int data_st = 1;

static struct data_storage data_example = {
	.data = (void *) &data_st,
};

MOD_MEMBERS_BIND(&__init, &data_example);



TEST_CASE("test for macro MOD_MEMBERS_BIND") {
	test_assert_equal(flag, 1);
}

OBJALLOC_DEF(allocator, OBJECT_SIZE , OBJECT_NUMBER);

TEST_CASE("test for macro OBJALLOC_DEF") {
	void *obj = objalloc(&allocator);
	test_assert_not_equal(obj,NULL);
	objfree(&allocator,obj);
}
