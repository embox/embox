/**
 * @file 
 * @brief AT Command Parser Unit Tests 
 * 
 * @date July 05, 2025
 * @author Peize Li
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <at/at_parser.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("AT Parser Tests");

TEST_CASE("Parse command types") {
	char name[32];
	at_cmd_type_t type;
	char params[64];
	int ret;

	/* EXEC type */
	ret = at_parse_cmd_type("AT", name, sizeof(name), &type, params,
	    sizeof(params));
	test_assert_equal(ret, 0);
	test_assert_str_equal(name, "AT");
	test_assert_equal(type, AT_CMD_EXEC);
	test_assert_str_equal(params, "");

	/* SET type */
	ret = at_parse_cmd_type("AT+CWMODE=3", name, sizeof(name), &type, params,
	    sizeof(params));
	test_assert_equal(ret, 0);
	test_assert_str_equal(name, "AT+CWMODE");
	test_assert_equal(type, AT_CMD_SET);
	test_assert_str_equal(params, "3");

	/* READ type */
	ret = at_parse_cmd_type("AT+CWMODE?", name, sizeof(name), &type, params,
	    sizeof(params));
	test_assert_equal(ret, 0);
	test_assert_str_equal(name, "AT+CWMODE");
	test_assert_equal(type, AT_CMD_READ);

	/* TEST type */
	ret = at_parse_cmd_type("AT+CWMODE=?", name, sizeof(name), &type, params,
	    sizeof(params));
	test_assert_equal(ret, 0);
	test_assert_str_equal(name, "AT+CWMODE");
	test_assert_equal(type, AT_CMD_TEST);
}

TEST_CASE("Parse command with spaces") {
	char name[32];
	at_cmd_type_t type;
	char params[64];

	/* Leading spaces */
	int ret = at_parse_cmd_type("  AT+TEST", name, sizeof(name), &type, params,
	    sizeof(params));
	test_assert_equal(ret, 0);
	test_assert_str_equal(name, "AT+TEST");
	test_assert_equal(type, AT_CMD_EXEC);

	/* Spaces around = */
	ret = at_parse_cmd_type("AT+TEST = 123", name, sizeof(name), &type, params,
	    sizeof(params));
	test_assert_equal(ret, 0);
	test_assert_str_equal(name, "AT+TEST");
	test_assert_equal(type, AT_CMD_SET);
	test_assert_str_equal(params, " 123");
}

TEST_CASE("Get response types") {
	test_assert_equal(at_get_response_type("OK"), AT_RESP_OK);
	test_assert_equal(at_get_response_type("ERROR"), AT_RESP_ERROR);
	test_assert_equal(at_get_response_type("+CME ERROR: 10"), AT_RESP_ERROR);
	test_assert_equal(at_get_response_type("+CMS ERROR: 305"), AT_RESP_ERROR);
	test_assert_equal(at_get_response_type(">"), AT_RESP_PROMPT);
	test_assert_equal(at_get_response_type("SEND OK"), AT_RESP_SEND_OK);
	test_assert_equal(at_get_response_type("SEND FAIL"), AT_RESP_SEND_FAIL);
	test_assert_equal(at_get_response_type("+CWMODE:1"), AT_RESP_DATA);
	test_assert_equal(at_get_response_type("Random text"), AT_RESP_UNKNOWN);
}

TEST_CASE("Parse responses") {
	at_response_t resp;
	int ret;

	/* OK response */
	ret = at_parse_response("OK", &resp);
	test_assert_equal(ret, 0);
	test_assert_equal(resp.type, AT_RESP_OK);

	/* Data response with colon */
	ret = at_parse_response("+CWMODE:3", &resp);
	test_assert_equal(ret, 0);
	test_assert_equal(resp.type, AT_RESP_DATA);
	test_assert_str_equal(resp.cmd, "+CWMODE");
	test_assert_str_equal(resp.data, "3");

	/* Data response without colon */
	ret = at_parse_response("+READY", &resp);
	test_assert_equal(ret, 0);
	test_assert_equal(resp.type, AT_RESP_DATA);
	test_assert_str_equal(resp.cmd, "+READY");
	test_assert_str_equal(resp.data, "");

	/* CME ERROR */
	ret = at_parse_response("+CME ERROR: 10", &resp);
	test_assert_equal(ret, 0);
	test_assert_equal(resp.type, AT_RESP_ERROR);
	test_assert_equal(resp.error_code, 10);
}

TEST_CASE("URC detection") {
	/* Common URCs */
	test_assert(at_is_urc("+CREG: 0,1"));
	test_assert(at_is_urc("+CGREG: 0,1"));
	test_assert(at_is_urc("+CEREG: 0,1,\"0001\",\"01A2D803\",7"));
	test_assert(at_is_urc("+CSQ: 20,99"));
	test_assert(at_is_urc("+CMTI: \"SM\",1"));
	test_assert(at_is_urc("+CMT: "
	                      "\"+1234567890\",\"\",\"20/01/01,00:00:00+00\""));
	test_assert(at_is_urc("RING"));
	test_assert(at_is_urc("+CLIP: \"1234567890\",129"));
	test_assert(at_is_urc("NO CARRIER"));
	test_assert(at_is_urc("CONNECT 115200"));
	test_assert(at_is_urc("WIFI CONNECTED"));
	test_assert(at_is_urc("WIFI GOT IP"));
	test_assert(at_is_urc("+IPD,10:"));

	/* Non-URCs */
	test_assert(!at_is_urc("OK"));
	test_assert(!at_is_urc("ERROR"));
	test_assert(!at_is_urc("AT+CWMODE=3"));
	test_assert(!at_is_urc("Random text"));
}

TEST_CASE("Parse IPD format") {
	int link_id;
	size_t length;
	const char *data_start;
	int ret;

	/* Format: +IPD,<link_id>,<length>:data */
	ret = at_parse_ipd("+IPD,2,15:Hello, World!", &link_id, &length, &data_start);
	test_assert_equal(ret, 0);
	test_assert_equal(link_id, 2);
	test_assert_equal(length, 15);
	test_assert_str_equal(data_start, "Hello, World!");

	/* Format: +IPD,<length>:data */
	ret = at_parse_ipd("+IPD,10:0123456789", &link_id, &length, &data_start);
	test_assert_equal(ret, 0);
	test_assert_equal(link_id, 0);
	test_assert_equal(length, 10);
	test_assert_str_equal(data_start, "0123456789");

	/* Invalid formats */
	ret = at_parse_ipd("NOT_IPD", &link_id, &length, &data_start);
	test_assert_equal(ret, -1);

	ret = at_parse_ipd("+IPD,abc:data", &link_id, &length, &data_start);
	test_assert_equal(ret, -1);

	ret = at_parse_ipd("+IPD,10", &link_id, &length, &data_start);
	test_assert_equal(ret, -1);
}

TEST_CASE("Extract quoted strings") {
	char output[64];
	int ret;

	/* Simple quoted string */
	ret = at_extract_quoted_string("\"Hello World\"", output, sizeof(output));
	test_assert_equal(ret, 0);
	test_assert_str_equal(output, "Hello World");

	/* Quoted string in context */
	ret = at_extract_quoted_string("+CWJAP:\"MySSID\",\"aa:bb:cc:dd:ee:ff\",1,-"
	                               "70",
	    output, sizeof(output));
	test_assert_equal(ret, 0);
	test_assert_str_equal(output, "MySSID");

	/* Empty quoted string */
	ret = at_extract_quoted_string("\"\"", output, sizeof(output));
	test_assert_equal(ret, 0);
	test_assert_str_equal(output, "");

	/* No quotes */
	ret = at_extract_quoted_string("No quotes here", output, sizeof(output));
	test_assert_equal(ret, -1);

	/* Unclosed quote */
	ret = at_extract_quoted_string("\"Unclosed", output, sizeof(output));
	test_assert_equal(ret, -1);
}

TEST_CASE("Parse comma-separated parameters") {
	char *argv[10];
	int argc;

	/* Simple parameters */
	argc = at_parse_params("1,2,3", argv, 10);
	test_assert_equal(argc, 3);
	test_assert_str_equal(argv[0], "1");
	test_assert_str_equal(argv[1], "2");
	test_assert_str_equal(argv[2], "3");

	/* Parameters with quotes */
	argc = at_parse_params("\"SSID\",\"password\",1,-70", argv, 10);
	test_assert_equal(argc, 4);
	test_assert_str_equal(argv[0], "\"SSID\"");
	test_assert_str_equal(argv[1], "\"password\"");
	test_assert_str_equal(argv[2], "1");
	test_assert_str_equal(argv[3], "-70");

	/* Quoted string with comma */
	argc = at_parse_params("\"Hello, World\",123", argv, 10);
	test_assert_equal(argc, 2);
	test_assert_str_equal(argv[0], "\"Hello, World\"");
	test_assert_str_equal(argv[1], "123");

	/* Empty parameters */
	argc = at_parse_params(",,", argv, 10);
	test_assert_equal(argc, 3);
	test_assert_str_equal(argv[0], "");
	test_assert_str_equal(argv[1], "");
	test_assert_str_equal(argv[2], "");

	/* Max args limit */
	argc = at_parse_params("1,2,3,4,5", argv, 3);
	test_assert_equal(argc, 3);
}

TEST_CASE("Parse CME/CMS errors") {
	int error_code;
	int ret;

	/* CME ERROR */
	ret = at_parse_cme_error("+CME ERROR: 10", &error_code);
	test_assert_equal(ret, 0);
	test_assert_equal(error_code, 10);

	ret = at_parse_cme_error("+CME ERROR: 100", &error_code);
	test_assert_equal(ret, 0);
	test_assert_equal(error_code, 100);

	/* Not a CME ERROR */
	ret = at_parse_cme_error("ERROR", &error_code);
	test_assert_equal(ret, -1);

	ret = at_parse_cme_error("+CMS ERROR: 305", &error_code);
	test_assert_equal(ret, -1);
}

TEST_CASE("Parse integers") {
	int value;
	int ret;

	/* Valid integers */
	ret = at_parse_integer("123", &value);
	test_assert_equal(ret, 0);
	test_assert_equal(value, 123);

	ret = at_parse_integer("-456", &value);
	test_assert_equal(ret, 0);
	test_assert_equal(value, -456);

	ret = at_parse_integer("0", &value);
	test_assert_equal(ret, 0);
	test_assert_equal(value, 0);

	/* With trailing spaces */
	ret = at_parse_integer("789  ", &value);
	test_assert_equal(ret, 0);
	test_assert_equal(value, 789);

	/* Invalid inputs */
	ret = at_parse_integer("abc", &value);
	test_assert_equal(ret, -1);

	ret = at_parse_integer("12.34", &value);
	test_assert_equal(ret, -1);

	ret = at_parse_integer("", &value);
	test_assert_equal(ret, -1);
}

TEST_CASE("Complex response parsing") {
	at_response_t resp;
	char *argv[10];
	int argc;

	/* Parse WiFi scan result */
	at_parse_response("+CWLAP:(3,\"MyWiFi\",-70,\"aa:bb:cc:dd:ee:ff\",1)", &resp);
	test_assert_equal(resp.type, AT_RESP_DATA);
	test_assert_str_equal(resp.cmd, "+CWLAP");

	/* Extract parameters from data */
	argc = at_parse_params(resp.data, argv, 10);
	test_assert_equal(argc, 5);
	test_assert_str_equal(argv[0], "(3");
	test_assert_str_equal(argv[1], "\"MyWiFi\"");
	test_assert_str_equal(argv[2], "-70");
	test_assert_str_equal(argv[3], "\"aa:bb:cc:dd:ee:ff\"");
	test_assert_str_equal(argv[4], "1)");
}

TEST_CASE("Edge cases and boundary conditions") {
	char name[8]; /* Small buffer */
	at_cmd_type_t type;
	char params[8];
	int ret;

	/* Command name too long */
	ret = at_parse_cmd_type("AT+VERYLONGCOMMANDNAME", name, sizeof(name), &type,
	    params, sizeof(params));
	test_assert_equal(ret, -1);

	/* Parameters too long */
	ret = at_parse_cmd_type("AT+CMD=verylongparameter", name, sizeof(name),
	    &type, params, sizeof(params));
	test_assert_equal(ret, 0);
	test_assert_str_equal(params, "verylon"); /* Truncated */

	/* Empty command */
	ret = at_parse_cmd_type("", name, sizeof(name), &type, params, sizeof(params));
	test_assert_equal(ret, -1);

	/* Only spaces */
	ret = at_parse_cmd_type("   ", name, sizeof(name), &type, params,
	    sizeof(params));
	test_assert_equal(ret, -1);
}

TEST_CASE("Response type with whitespace") {
	test_assert_equal(at_get_response_type("OK  "), AT_RESP_OK);
	test_assert_equal(at_get_response_type("  OK"), AT_RESP_OK);
	test_assert_equal(at_get_response_type("  ERROR  "), AT_RESP_ERROR);
	test_assert_equal(at_get_response_type("OK\r\n"), AT_RESP_OK);
}

TEST_CASE("IPD with binary data") {
	int link_id;
	size_t length;
	const char *data_start;

	/* Binary data with null bytes */
	const char ipd_with_binary[] = "+IPD,0,5:\x00\x01\x02\x03\x04";
	int ret = at_parse_ipd(ipd_with_binary, &link_id, &length, &data_start);
	test_assert_equal(ret, 0);
	test_assert_equal(link_id, 0);
	test_assert_equal(length, 5);
	test_assert(memcmp(data_start, "\x00\x01\x02\x03\x04", 5) == 0);
}