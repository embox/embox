/**
 * @file
 * @brief
 *
 * @date 31.10.12
 * @author Vita Loginova
 */

#include <embox/test.h>
#include <url_parser.h>

EMBOX_TEST_SUITE("network/parser/url_parser test");

TEST_CASE("parse URL \"/rfc/rfc1808.txt?param1=1&param2=65\"") {
	struct parsed_url *parsed_url;

	parsed_url = parse_url("/rfc/rfc1808.txt?param1=1&param2=65");
	test_assert_not_null(parsed_url);
	test_assert_str_equal(parsed_url->path, "rfc/rfc1808.txt");
	test_assert_str_equal(parsed_url->query, "param1=1&param2=65");
	parsed_url_free(parsed_url);
}

TEST_CASE("parse URL \"http://ru.wikipedia.org:1080/wiki/URI#fragment1\"") {
	struct parsed_url *parsed_url;

	parsed_url = parse_url("http://ru.wikipedia.org:1080/wiki/URI#fragment1");
	test_assert_not_null(parsed_url);
	test_assert_str_equal(parsed_url->host, "ru.wikipedia.org");
	test_assert_str_equal(parsed_url->path, "wiki/URI");
	test_assert_str_equal(parsed_url->fragment, "fragment1");
	test_assert_str_equal(parsed_url->port, "1080");
	parsed_url_free(parsed_url);
}

TEST_CASE("parse URL \"192.0.2.16:80\"") {
	struct parsed_url *parsed_url;

	parsed_url = parse_url("192.0.2.16:80");
	test_assert_not_null(parsed_url);
	test_assert_str_equal(parsed_url->host, "192.0.2.16");
	test_assert_str_equal(parsed_url->port, "80");
	parsed_url_free(parsed_url);
}
