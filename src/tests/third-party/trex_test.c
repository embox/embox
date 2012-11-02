/**
 * @file
 *
 * @brief
 *
 * @date 2.11.2012
 * @author Vita Loginova
 */

#include <embox/test.h>
#include <lib/trex.h>
#include <stdlib.h>

int match_string(char *pattern, char *request);

EMBOX_TEST_SUITE("third-party/trex test");

TEST_CASE("Trex test") {
test_assert_true(match_string("#[-\\w_]*", "#ab-sF"));
test_assert_false(match_string("#[-\\w_]*", "ab-sF"));

test_assert_true(match_string("^\\a{2,}", "abcd"));
test_assert_false(match_string("^\\a{2,}", "1abcd"));

test_assert_true(
		match_string("Simple\\s[a-zA-Z]+\\stest", "Simple simple test"));
test_assert_false(
		match_string("Simple\\s[a-zA-Z]+\\stest", "Simpl simple test"));
}

int match_string(char *pattern, char *request) {
const TRexChar *error = NULL;
TRex *trex;
trex = trex_compile(_TREXC(pattern), &error);
if (trex) {
	const TRexChar *begin, *end;
	if (trex_search(trex, request, &begin, &end)) {
		trex_free(trex);
		return 1;
	} else {
		trex_free(trex);
		return 0;
	}
}
return 0;
}
