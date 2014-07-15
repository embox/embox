/**
 * @file
 * @brief
 *
 * @date 25.10.12
 * @author Vita Loginova
 */

#include <embox/test.h>
#include <url_parser.h>
#include <net/util/request_parser.h>

EMBOX_TEST_SUITE("network/parser/request_parser test");

TEST_CASE("Parse \"GET http://wsdf-365.ru/first_folder/second/index.html#frag01 HTTP/1.1\"") {
	char *test_get = "GET http://wsdf-365.ru/first_folder/second/index.html#frag01 HTTP/1.1";
	http_request parsed_request;

	test_assert_zero(parse_http(test_get, &parsed_request));
	test_assert_str_equal(parsed_request.method, "GET");
	test_assert_str_equal(parsed_request.proto, "HTTP/1.1");
	test_assert_str_equal(parsed_request.parsed_url->host, "wsdf-365.ru");
	test_assert_str_equal(parsed_request.parsed_url->path, "first_folder/second/index.html");
	test_assert_str_equal(parsed_request.parsed_url->fragment, "frag01");
	free_http_request(&parsed_request);
}

TEST_CASE("Parse \"HEAD /first_folder/second/index.php?p1+p2&p3=54; HTTP/1.1\r\nHost: 10.10.111.1:80\r\nConnection: Close\"") {
	char *test_get = "HEAD /first_folder/second/index.php?p1+p2&p3=54; HTTP/1.1\r\nHost: 10.10.111.1:80\r\nConnection: Close";
	http_request parsed_request;

	test_assert_zero(parse_http(test_get, &parsed_request));
	test_assert_str_equal(parsed_request.method, "HEAD");
	test_assert_str_equal(parsed_request.proto, "HTTP/1.1");
	test_assert_str_equal(parsed_request.parsed_url->host, "10.10.111.1");
	test_assert_str_equal(parsed_request.parsed_url->path, "first_folder/second/index.php");
	test_assert_str_equal(parsed_request.parsed_url->query, "p1+p2&p3=54;");
	test_assert_str_equal(parsed_request.parsed_url->port, "80");
	test_assert_str_equal(parsed_request.connection, "Close");
	free_http_request(&parsed_request);
}

TEST_CASE("Parse \"head /first_folder/second/index.php?p1+p2&p3=54; HTTP/1.1\r\nHost: 10.10.111.1:80\"") {
	char *test_get = "head /first_folder/second/index.php?p1+p2&p3=54; HTTP/1.1\r\nHost: 10.10.111.1:80";
	http_request parsed_request;

	test_assert_true(0 > parse_http(test_get, &parsed_request));
}

TEST_CASE("Parse \"HEAD  HTTP/1.1\nHost: 10.10.111.1:80\"") {
	char *test_get = "HEAD  HTTP/1.1\nHost: 10.10.111.1:80";
	http_request parsed_request;

	test_assert_true(0 > parse_http(test_get, &parsed_request));
}
