/**
 * @brief
 *
 * @date 6.07.2011
 * @author Alexandr Kalmuk
 */

#include <stdio.h>

#include <embox/test.h>
#include <framework/mod/self.h>
#include <framework/mod/api.h>

EMBOX_TEST_SUITE("simple test for member injection");

static int init_flag = 0;

static int init(struct mod_member *info) {
	*((int *) info->data) = 1;
	return 0;
}

static const struct mod_member_ops ops = {
		.init = &init,
};

MOD_MEMBER_BIND(&ops, &init_flag);

TEST_CASE("test for macro MOD_MEMBER_BIND") {
	test_assert_equal(init_flag, 1);
}
