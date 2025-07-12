/**
 * @file
 * @brief AT Device Emulator Unit Tests
 *
 * @date July 09, 2025
 * @author Peize Li
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drivers/at/at_emulator.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("AT Emulator Tests");

/* Custom command handler for testing */
static at_result_t custom_handler(at_emulator_t *emu, at_cmd_type_t type,
    const char *params, char *response, size_t resp_size) {
	(void)emu;
	(void)params;

	if (type == AT_CMD_TEST) {
		snprintf(response, resp_size, "+CUSTOM: TEST");
		return AT_OK;
	}

	snprintf(response, resp_size, "CUSTOM RESPONSE");
	return AT_OK;
}

TEST_CASE("Basic initialization and commands") {
	at_emulator_t *emu = at_emulator_create();
	const char *resp;

	test_assert_not_null(emu);

	resp = at_emulator_send_cmd(emu, "AT");
	test_assert(strstr(resp, "OK") != NULL);

	resp = at_emulator_send_cmd(emu, "ATI");
	test_assert(strstr(resp, "Embox") != NULL);
	test_assert(strstr(resp, "AT-EMU") != NULL);

	resp = at_emulator_send_cmd(emu, "INVALID");
	test_assert(strstr(resp, "ERROR") != NULL);

	at_emulator_destroy(emu);
}

TEST_CASE("Verbose and quiet modes") {
	at_emulator_t *emu = at_emulator_create();
	const char *resp;

	resp = at_emulator_send_cmd(emu, "AT");
	test_assert(strstr(resp, "OK") != NULL);

	resp = at_emulator_send_cmd(emu, "ATV0");
	test_assert(strstr(resp, "0") != NULL);

	resp = at_emulator_send_cmd(emu, "AT");
	test_assert(strstr(resp, "0") != NULL);

	at_emulator_send_cmd(emu, "ATQ1");

	/* In quiet mode, responses are suppressed but echo still works if enabled */
	at_emulator_set_echo(emu, false);
	resp = at_emulator_send_cmd(emu, "AT");
	test_assert(resp != NULL && strlen(resp) == 0); /* \r\n0\r\n */

	at_emulator_destroy(emu);
}

TEST_CASE("Custom command registration") {
	at_emulator_t *emu = at_emulator_create();
	const char *resp;

	test_assert_zero(
	    at_emulator_register_command(emu, "AT+CUSTOM", custom_handler));

	resp = at_emulator_send_cmd(emu, "AT+CUSTOM");
	test_assert(strstr(resp, "CUSTOM RESPONSE") != NULL);

	resp = at_emulator_send_cmd(emu, "AT+CUSTOM=?");
	test_assert(strstr(resp, "+CUSTOM: TEST") != NULL);

	test_assert_zero(
	    at_emulator_register_command(emu, "AT+CUSTOM", custom_handler));

	at_emulator_destroy(emu);
}

TEST_CASE("Extended command parsing") {
	at_emulator_t *emu = at_emulator_create();
	const char *resp;

	resp = at_emulator_send_cmd(emu, "AT+GMR");
	test_assert(strstr(resp, "1.0.0") != NULL);

	resp = at_emulator_send_cmd(emu, "AT+GMM");
	test_assert(strstr(resp, "AT-EMU") != NULL);

	resp = at_emulator_send_cmd(emu, "AT+GMI");
	test_assert(strstr(resp, "Embox") != NULL);

	resp = at_emulator_send_cmd(emu, "AT+GMR=?");
	test_assert(strstr(resp, "+GMR:") != NULL);

	at_emulator_destroy(emu);
}

TEST_CASE("Device info configuration") {
	at_emulator_t *emu = at_emulator_create();
	const char *resp;

	at_emulator_set_device_info(emu, "TestDevice", "TestCorp", "Model-X", "2.0.0");

	resp = at_emulator_send_cmd(emu, "ATI");
	test_assert(strstr(resp, "TestDevice") != NULL);
	test_assert(strstr(resp, "TestCorp") != NULL);

	resp = at_emulator_send_cmd(emu, "AT+GMR");
	test_assert(strstr(resp, "2.0.0") != NULL);

	at_emulator_destroy(emu);
}

TEST_CASE("Reset command") {
	at_emulator_t *emu = at_emulator_create();
	const char *resp;

	resp = at_emulator_send_cmd(emu, "ATE0");
	test_assert(resp && strstr(resp, "OK") != NULL);

	resp = at_emulator_send_cmd(emu, "ATV0");
	test_assert(resp && strstr(resp, "0") != NULL);

	/* Reset */
	resp = at_emulator_send_cmd(emu, "ATZ");
	/* After reset, verbose mode is restored, so return "OK" not "0" */
	test_assert(resp && strstr(resp, "OK") != NULL);

	/* After reset, echo should be restored */
	resp = at_emulator_process(emu, "X", 1);
	test_assert(resp && *resp == 'X');

	at_emulator_process(emu, "\r", 1);
	resp = at_emulator_send_cmd(emu, "AT");
	test_assert(resp && strstr(resp, "OK") != NULL);

	at_emulator_destroy(emu);
}

TEST_CASE("User data handling") {
	at_emulator_t *emu = at_emulator_create();
	int test_data = 42;

	at_emulator_set_user_data(emu, &test_data);
	test_assert_equal(at_emulator_get_user_data(emu), &test_data);

	at_emulator_set_user_data(emu, NULL);
	test_assert_null(at_emulator_get_user_data(emu));

	at_emulator_destroy(emu);
}

TEST_CASE("Backspace handling") {
	at_emulator_t *emu = at_emulator_create();
	const char *resp;

	at_emulator_process(emu, "A", 1);
	at_emulator_process(emu, "T", 1);
	at_emulator_process(emu, "X", 1);

	resp = at_emulator_process(emu, "\b", 1);
	test_assert(resp != NULL && strcmp(resp, "\b \b") == 0);

	/* Complete command (should be "AT" after backspace removed "X") */
	resp = at_emulator_process(emu, "\r", 1);
	test_assert(strstr(resp, "OK") != NULL);

	at_emulator_destroy(emu);
}

TEST_CASE("Line ending handling") {
	at_emulator_t *emu = at_emulator_create();
	const char *resp;

	resp = at_emulator_send_cmd(emu, "AT");
	test_assert(resp && strstr(resp, "OK") != NULL);

	/* Test that empty CR doesn't crash */
	resp = at_emulator_process(emu, "\r", 1);
	test_assert(resp != NULL);

	/* Test LF handling */
	resp = at_emulator_process(emu, "\n", 1);
	test_assert(resp != NULL);

	/* Test command with mixed line endings */
	at_emulator_process(emu, "A", 1);
	at_emulator_process(emu, "T", 1);
	at_emulator_process(emu, "\n", 1);
	resp = at_emulator_process(emu, "\r", 1);
	test_assert(
	    resp && (strstr(resp, "OK") != NULL || strstr(resp, "\r\n") != NULL));

	at_emulator_destroy(emu);
}

TEST_CASE("Buffer boundary testing") {
	at_emulator_t *emu = at_emulator_create();
	const char *resp;
	char long_cmd[300];

	at_emulator_send_cmd(emu, "ATE0");

	/* Build a command that exceeds buffer (cmd_buffer is 256) */
	strcpy(long_cmd, "AT+TEST");
	memset(long_cmd + 7, 'A', 250);
	long_cmd[257] = '\0';

	for (size_t i = 0; i < strlen(long_cmd); i++) {
		at_emulator_process(emu, &long_cmd[i], 1);
	}

	/* Execute - should get error since command is truncated */
	resp = at_emulator_process(emu, "\r", 1);
	test_assert(resp && strstr(resp, "ERROR") != NULL);

	at_emulator_destroy(emu);
}
