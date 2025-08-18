/**
 * @file
 * @brief ESP32-C3 Emulator Unit Tests 
 *
 * @date July 31, 2025
 * @author Peize Li
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <wifi/esp32c3/emulator/esp32c3_emulator.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("ESP32-C3 Emulator Tests");

/* Helper to wait for URC with timeout */
static const char *wait_for_urc(at_emulator_t *emu, int timeout_ms) {
	int elapsed = 0;
	const char *urc;

	while (elapsed < timeout_ms) {
		urc = at_emulator_poll(emu);
		if (urc) {
			return urc;
		}
		usleep(10000); /* 10ms */
		elapsed += 10;
	}
	return NULL;
}

TEST_CASE("Basic ESP32-C3 commands") {
	at_emulator_t *emu = esp32c3_emulator_create();
	const char *resp;

	test_assert_not_null(emu);

	/* Test basic AT command */
	resp = at_emulator_send_cmd(emu, "AT");
	test_assert(strstr(resp, "OK") != NULL);

	/* Test device info */
	resp = at_emulator_send_cmd(emu, "ATI");
	test_assert(strstr(resp, "ESP32-C3_Emulator") != NULL);
	test_assert(strstr(resp, "Embox") != NULL);

	/* Test WiFi mode query */
	resp = at_emulator_send_cmd(emu, "AT+CWMODE?");
	test_assert(strstr(resp, "+CWMODE:1") != NULL); /* Default STA mode */

	esp32c3_emulator_destroy(emu);
}

TEST_CASE("WiFi mode configuration") {
	at_emulator_t *emu = esp32c3_emulator_create();
	const char *resp;

	/* Set AP mode */
	resp = at_emulator_send_cmd(emu, "AT+CWMODE=2");
	test_assert(strstr(resp, "OK") != NULL);

	/* Verify mode changed */
	resp = at_emulator_send_cmd(emu, "AT+CWMODE?");
	test_assert(strstr(resp, "+CWMODE:2") != NULL);

	/* Test invalid mode */
	resp = at_emulator_send_cmd(emu, "AT+CWMODE=4");
	test_assert(strstr(resp, "ERROR") != NULL);

	/* Test mode range */
	resp = at_emulator_send_cmd(emu, "AT+CWMODE=?");
	test_assert(strstr(resp, "+CWMODE:(1-3)") != NULL);

	esp32c3_emulator_destroy(emu);
}

TEST_CASE("Reset command and ready URC") {
	at_emulator_t *emu = esp32c3_emulator_create();
	const char *resp;
	const char *urc;

	/* Send reset command */
	resp = at_emulator_send_cmd(emu, "AT+RST");
	test_assert(strstr(resp, "OK") != NULL);

	/* Wait for "READY" URC */
	urc = wait_for_urc(emu, 100);
	test_assert_not_null(urc);
	test_assert(strstr(urc, "READY") != NULL);

	esp32c3_emulator_destroy(emu);
}

TEST_CASE("WiFi connection simulation") {
	at_emulator_t *emu = esp32c3_emulator_create();
	const char *resp;
	const char *urc;

	/* Connect to existing AP from scan list */
	resp = at_emulator_send_cmd(emu, "AT+CWJAP=\"TestAP_1\",\"password123\"");
	test_assert(strstr(resp, "OK") != NULL);

	/* Wait for WIFI CONNECTED URC */
	urc = wait_for_urc(emu, 3000);
	test_assert_not_null(urc);
	test_assert(strstr(urc, "WIFI CONNECTED") != NULL);

	/* Wait for WIFI GOT IP URC */
	urc = wait_for_urc(emu, 1000);
	test_assert_not_null(urc);
	test_assert(strstr(urc, "WIFI GOT IP") != NULL);

	/* Query connection status */
	resp = at_emulator_send_cmd(emu, "AT+CWJAP?");
	test_assert(strstr(resp, "+CWJAP:\"TestAP_1\"") != NULL);

	/* Query IP address */
	resp = at_emulator_send_cmd(emu, "AT+CIPSTA?");
	test_assert(strstr(resp, "192.168.1.100") != NULL);

	esp32c3_emulator_destroy(emu);
}

TEST_CASE("WiFi disconnection") {
	at_emulator_t *emu = esp32c3_emulator_create();
	const char *resp;
	const char *urc;

	/* First connect to existing AP */
	resp = at_emulator_send_cmd(emu, "AT+CWJAP=\"TestAP_2\",\"password\"");
	test_assert(strstr(resp, "OK") != NULL);

	/* Wait for connection */
	wait_for_urc(emu, 3000); /* WIFI CONNECTED */
	wait_for_urc(emu, 1000); /* WIFI GOT IP */

	/* Disconnect */
	resp = at_emulator_send_cmd(emu, "AT+CWQAP");
	test_assert(strstr(resp, "OK") != NULL);

	/* Wait for disconnect URC */
	urc = wait_for_urc(emu, 100);
	test_assert_not_null(urc);
	test_assert(strstr(urc, "WIFI DISCONNECT") != NULL);

	/* Verify disconnected state */
	resp = at_emulator_send_cmd(emu, "AT+CWJAP?");
	test_assert(resp != NULL); /* Should return empty response or just OK */

	/* IP should be 0.0.0.0 */
	resp = at_emulator_send_cmd(emu, "AT+CIPSTA?");
	test_assert(strstr(resp, "0.0.0.0") != NULL);

	esp32c3_emulator_destroy(emu);
}

TEST_CASE("AP scan functionality") {
	at_emulator_t *emu = esp32c3_emulator_create();
	const char *resp;

	/* Scan for APs */
	resp = at_emulator_send_cmd(emu, "AT+CWLAP");
	test_assert_not_null(resp);

	/* Check for test APs */
	test_assert(strstr(resp, "TestAP_1") != NULL);
	test_assert(strstr(resp, "TestAP_2") != NULL);
	test_assert(strstr(resp, "TestAP_3") != NULL);

	/* Verify format includes RSSI and channel */
	test_assert(strstr(resp, "-45") != NULL); /* RSSI */
	test_assert(strstr(resp, ",6") != NULL);  /* Channel */

	esp32c3_emulator_destroy(emu);
}

TEST_CASE("TCP connection") {
	at_emulator_t *emu = esp32c3_emulator_create();
	const char *resp;
	const char *urc;

	/* First need to be connected to WiFi */
	at_emulator_send_cmd(emu, "AT+CWJAP=\"TestAP_1\",\"password\"");
	wait_for_urc(emu, 3000); /* WIFI CONNECTED */
	wait_for_urc(emu, 1000); /* WIFI GOT IP */

	/* Start TCP connection */
	resp = at_emulator_send_cmd(emu, "AT+CIPSTART=\"TCP\",\"192.168.1.10\","
	                                 "8080");
	test_assert(strstr(resp, "OK") != NULL);

	/* Wait for CONNECT URC */
	urc = wait_for_urc(emu, 100);
	test_assert_not_null(urc);
	test_assert(strstr(urc, "CONNECT") != NULL);

	/* Test CIPSEND */
	resp = at_emulator_send_cmd(emu, "AT+CIPSEND");
	test_assert(strstr(resp, ">") != NULL);

	esp32c3_emulator_destroy(emu);
}

TEST_CASE("Error handling") {
	at_emulator_t *emu = esp32c3_emulator_create();
	const char *resp;

	/* TCP connection without WiFi should fail */
	resp = at_emulator_send_cmd(emu, "AT+CIPSTART=\"TCP\",\"192.168.1.10\","
	                                 "8080");
	test_assert(strstr(resp, "ERROR") != NULL);

	/* Connect to non-existent AP should fail */
	resp = at_emulator_send_cmd(emu, "AT+CWJAP=\"NonExistentAP\",\"password\"");
	test_assert(strstr(resp, "ERROR") != NULL);

	/* Invalid CWJAP parameters */
	resp = at_emulator_send_cmd(emu, "AT+CWJAP=");
	test_assert(strstr(resp, "ERROR") != NULL);

	/* CIPSEND without connection */
	resp = at_emulator_send_cmd(emu, "AT+CIPSEND");
	test_assert(strstr(resp, "ERROR") != NULL);

	esp32c3_emulator_destroy(emu);
}

TEST_CASE("Multiple connect/disconnect cycles") {
	at_emulator_t *emu = esp32c3_emulator_create();
	const char *resp;

	/* Test with different APs from scan list */
	const char *test_aps[] = {"TestAP_1", "TestAP_2", "TestAP_3"};

	for (int i = 0; i < 3; i++) {
		char cmd[128];
		snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"pass\"", test_aps[i]);

		/* Connect */
		resp = at_emulator_send_cmd(emu, cmd);
		test_assert(strstr(resp, "OK") != NULL);

		/* Wait for connection */
		wait_for_urc(emu, 3000);
		wait_for_urc(emu, 1000);

		/* Disconnect */
		resp = at_emulator_send_cmd(emu, "AT+CWQAP");
		test_assert(strstr(resp, "OK") != NULL);

		/* Wait for disconnect */
		wait_for_urc(emu, 100);
	}

	esp32c3_emulator_destroy(emu);
}

TEST_CASE("Test prefix flexibility") {
	at_emulator_t *emu = esp32c3_emulator_create();
	const char *resp;
	const char *urc;

	/* Any SSID starting with "Test" should work for simulation flexibility */
	resp = at_emulator_send_cmd(emu, "AT+CWJAP=\"TestNetwork\",\"pass123\"");
	test_assert(strstr(resp, "OK") != NULL);

	/* Wait for connection */
	urc = wait_for_urc(emu, 3000);
	test_assert_not_null(urc);
	test_assert(strstr(urc, "WIFI CONNECTED") != NULL);

	wait_for_urc(emu, 1000); /* WIFI GOT IP */

	/* Query should show the connected SSID */
	resp = at_emulator_send_cmd(emu, "AT+CWJAP?");
	test_assert(strstr(resp, "TestNetwork") != NULL);

	esp32c3_emulator_destroy(emu);
}
