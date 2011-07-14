/**
 * @brief
 *
 * @date 6.07.2011
 * @author Alexandr Kalmuk
 */

#include <stdio.h>

#include <embox/test.h>
#include <framework/mod/member/self.h>

EMBOX_TEST_SUITE("simple test for member injection");

static int flag_member_init(struct mod_member *info) {
	*((int *) info->data) = 1;
	return 0;
}

static const struct mod_member_ops flag_ops = {
		.init = &flag_member_init,
};

static int flag = 0;

MOD_MEMBER_BIND(&flag_ops, &flag);

TEST_CASE("test for macro MOD_MEMBER_BIND") {
	test_assert_equal(flag, 1);
}

static const struct mod_member_ops null_ops;

/* No operations bound. */
MOD_MEMBER_BIND(&null_ops, NULL);

/* No operations bound. */
MOD_MEMBER_BIND(NULL, NULL);
