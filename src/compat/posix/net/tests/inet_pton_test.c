/*
 * @file inet_pton_test.c
 * @brief embox test suite to test inet_pton function behavior
 *
 * @date 30.10.2023
 *
 */

#include <framework/test/self.h>
#include <framework/test/assert.h>
#include <kernel/printk.h>

#include "inet_pton_test.h"

EMBOX_TEST_SUITE("inet_pton behaviour testsuite");

TEST_CASE("inet_pton af=AF_INET") {
	int af = AF_INET;
	int res;
	struct in_addr v4_in;

	for (int i = 0; i < V4_TESTS; i++) {
		v4_in.s_addr = 0;
		res = inet_pton(af, v4_test_ref[i].test_string, &v4_in);
		test_assert_equal(res, v4_test_ref[i].pass);
		test_assert_equal(v4_in.s_addr, v4_test_ref[i].true_val.s_addr);
		printk(".");
	}
}

TEST_CASE("inet_pton af=AF_INET6") {
	int af = AF_INET6;
	int res;
	struct in6_addr v6_in;

	for (int i = 0; i < V6_TESTS; i++) {
		v6_in.s6_addr32[0] = 0;
		v6_in.s6_addr32[1] = 0;
		v6_in.s6_addr32[2] = 0;
		v6_in.s6_addr32[3] = 0;
		res = inet_pton(af, v6_test_ref[i].test_string, &v6_in);
		test_assert_equal(res, v6_test_ref[i].pass);
		test_assert_equal(v6_in.s6_addr32[0],
		    v6_test_ref[i].true_val.s6_addr32[0]);
		test_assert_equal(v6_in.s6_addr32[1],
		    v6_test_ref[i].true_val.s6_addr32[1]);
		test_assert_equal(v6_in.s6_addr32[2],
		    v6_test_ref[i].true_val.s6_addr32[2]);
		test_assert_equal(v6_in.s6_addr32[3],
		    v6_test_ref[i].true_val.s6_addr32[3]);
		printk(".");
	}
}
