/**
 * @file
 * @brief AT Client Unit Tests
 *
 * @date July 10, 2025
 * @author Peize Li
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <at/at_client.h>
#include <at/emulator/at_emulator.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("AT Client Tests");

/* PTY support */
extern int ppty(int ptyfds[2]);

/* URC test variables */
static int g_urc_count = 0;
static char g_last_urc[128];

/* URC test callback */
static void test_urc_handler(const char *line, void *arg) {
	g_urc_count++;
	strncpy(g_last_urc, line, sizeof(g_last_urc) - 1);
}

/* Emulator thread data */
struct emulator_thread_data {
	int fd;
	at_emulator_t *emulator;
	volatile int running;
};

/* Emulator thread function */
static void *emulator_thread_func(void *arg) {
	struct emulator_thread_data *data = (struct emulator_thread_data *)arg;
	char buffer[256];

	while (data->running) {
		int n = read(data->fd, buffer, sizeof(buffer));
		if (n > 0) {
			/* Process character by character */
			for (int i = 0; i < n; i++) {
				const char *response = at_emulator_process(data->emulator,
				    &buffer[i], 1);
				if (response && *response) {
					write(data->fd, response, strlen(response));
				}
			}
		}
		else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
			break;
		}
		usleep(1000);
	}
	return NULL;
}

TEST_CASE("Basic init and close with PTY") {
	at_client_t client;
	int ptyfds[2];

	test_assert_zero(ppty(ptyfds));

	test_assert_zero(at_client_init_fd(&client, ptyfds[0]));
	test_assert_equal(client.fd, ptyfds[0]);
	test_assert_false(client.owns_fd); /* fd was provided externally */

	at_client_close(&client);
	test_assert_equal(client.fd, -1);

	close(ptyfds[0]);
	close(ptyfds[1]);
}

TEST_CASE("Init with device path") {
	at_client_t client;
	int ptyfds[2];
	char master_path[32];

	test_assert_zero(ppty(ptyfds));

	/* Get the device path for master PTY */
	snprintf(master_path, sizeof(master_path), "/dev/pts/%d", ptyfds[0]);

	/* Close the raw fd since at_client_init will open it */
	close(ptyfds[0]);

	if (at_client_init(&client, master_path) == 0) {
		test_assert(client.fd >= 0);
		test_assert_true(client.owns_fd); /* fd was opened by client */
		at_client_close(&client);
	}

	close(ptyfds[1]);
}

TEST_CASE("Parameter validation") {
	at_client_t client;
	char buf[128];
	at_result_t result;

	test_assert_equal(at_client_init(NULL, "/dev/null"), -1);
	test_assert_equal(at_client_init(&client, NULL), -1);
	test_assert_equal(at_client_init_fd(NULL, 0), -1);
	test_assert_equal(at_client_init_fd(&client, -1), -1);

	/* Initialize with invalid fd to test send validation */
	client.fd = -1;

	result = at_client_send(NULL, "AT", buf, sizeof(buf), 1000);
	test_assert_equal(result, AT_INVALID_PARAM);

	result = at_client_send(&client, NULL, buf, sizeof(buf), 1000);
	test_assert_equal(result, AT_INVALID_PARAM);

	result = at_client_send(&client, "AT", buf, sizeof(buf), 1000);
	test_assert_equal(result, AT_INVALID_PARAM); /* Invalid fd */
}

TEST_CASE("Basic AT command test") {
	at_client_t client;
	int ptyfds[2];
	pthread_t emu_thread;
	struct emulator_thread_data emu_data;
	char buf[128];
	at_result_t result;

	test_assert_zero(ppty(ptyfds));

	/* Only set non-blocking on emulator side */
	fcntl(ptyfds[1], F_SETFL, O_NONBLOCK);

	emu_data.emulator = at_emulator_create();
	test_assert_not_null(emu_data.emulator);

	emu_data.fd = ptyfds[1];
	emu_data.running = 1;
	test_assert_zero(
	    pthread_create(&emu_thread, NULL, emulator_thread_func, &emu_data));

	/* Client will configure its side appropriately */
	test_assert_zero(at_client_init_fd(&client, ptyfds[0]));

	/* Small delay to ensure emulator is ready */
	usleep(10000);

	result = at_client_send(&client, "AT", buf, sizeof(buf), 1000);
	test_assert_equal(result, AT_OK);

	emu_data.running = 0;
	pthread_join(emu_thread, NULL);
	at_emulator_destroy(emu_data.emulator);
	at_client_close(&client);
	close(ptyfds[0]);
	close(ptyfds[1]);
}

TEST_CASE("Timeout test") {
	at_client_t client;
	int ptyfds[2];
	char buf[128];
	at_result_t result;

	test_assert_zero(ppty(ptyfds));

	/* Close other end to ensure timeout */
	close(ptyfds[1]);

	test_assert_zero(at_client_init_fd(&client, ptyfds[0]));

	result = at_client_send(&client, "AT", buf, sizeof(buf), 200);
	test_assert_equal(result, AT_TIMEOUT);

	at_client_close(&client);
	close(ptyfds[0]);
}

TEST_CASE("Error response test") {
	at_client_t client;
	int ptyfds[2];
	pthread_t emu_thread;
	struct emulator_thread_data emu_data;
	char buf[128];
	at_result_t result;

	test_assert_zero(ppty(ptyfds));

	fcntl(ptyfds[1], F_SETFL, O_NONBLOCK);

	emu_data.emulator = at_emulator_create();
	test_assert_not_null(emu_data.emulator);

	emu_data.fd = ptyfds[1];
	emu_data.running = 1;
	test_assert_zero(
	    pthread_create(&emu_thread, NULL, emulator_thread_func, &emu_data));

	test_assert_zero(at_client_init_fd(&client, ptyfds[0]));

	usleep(10000);

	/* Send invalid command, should return ERROR */
	result = at_client_send(&client, "AT+INVALID", buf, sizeof(buf), 1000);
	test_assert_equal(result, AT_ERROR);

	emu_data.running = 0;
	pthread_join(emu_thread, NULL);
	at_emulator_destroy(emu_data.emulator);
	at_client_close(&client);
	close(ptyfds[0]);
	close(ptyfds[1]);
}

TEST_CASE("URC handler test") {
	at_client_t client;
	int ptyfds[2];

	/* Reset URC count */
	g_urc_count = 0;
	g_last_urc[0] = '\0';

	test_assert_zero(ppty(ptyfds));

	fcntl(ptyfds[1], F_SETFL, O_NONBLOCK);

	test_assert_zero(at_client_init_fd(&client, ptyfds[0]));

	at_client_set_urc_handler(&client, test_urc_handler, NULL);

	/* Simulate URC */
	const char *urc = "+RING\r\n";
	write(ptyfds[1], urc, strlen(urc));

	usleep(10000);
	at_client_process_rx(&client);

	/* Verify URC was processed */
	test_assert_equal(g_urc_count, 1);
	test_assert_str_equal(g_last_urc, "+RING");

	at_client_close(&client);
	close(ptyfds[0]);
	close(ptyfds[1]);
}

TEST_CASE("Null buffer test") {
	at_client_t client;
	int ptyfds[2];
	pthread_t emu_thread;
	struct emulator_thread_data emu_data;
	at_result_t result;

	test_assert_zero(ppty(ptyfds));

	fcntl(ptyfds[1], F_SETFL, O_NONBLOCK);

	emu_data.emulator = at_emulator_create();
	test_assert_not_null(emu_data.emulator);

	emu_data.fd = ptyfds[1];
	emu_data.running = 1;
	test_assert_zero(
	    pthread_create(&emu_thread, NULL, emulator_thread_func, &emu_data));

	test_assert_zero(at_client_init_fd(&client, ptyfds[0]));

	usleep(10000);

	/* Should handle NULL buffer gracefully */
	result = at_client_send(&client, "AT", NULL, 0, 1000);
	test_assert_equal(result, AT_OK);

	emu_data.running = 0;
	pthread_join(emu_thread, NULL);
	at_emulator_destroy(emu_data.emulator);
	at_client_close(&client);
	close(ptyfds[0]);
	close(ptyfds[1]);
}

TEST_CASE("Sequential commands test") {
	at_client_t client;
	int ptyfds[2];
	pthread_t emu_thread;
	struct emulator_thread_data emu_data;
	char buf[128];
	at_result_t result;

	test_assert_zero(ppty(ptyfds));

	fcntl(ptyfds[1], F_SETFL, O_NONBLOCK);

	emu_data.emulator = at_emulator_create();
	test_assert_not_null(emu_data.emulator);

	emu_data.fd = ptyfds[1];
	emu_data.running = 1;
	test_assert_zero(
	    pthread_create(&emu_thread, NULL, emulator_thread_func, &emu_data));

	test_assert_zero(at_client_init_fd(&client, ptyfds[0]));

	usleep(10000);

	/* Send 3 commands sequentially */
	for (int i = 0; i < 3; i++) {
		result = at_client_send(&client, "AT", buf, sizeof(buf), 1000);
		test_assert_equal(result, AT_OK);
		usleep(50000); /* 50ms interval */
	}

	emu_data.running = 0;
	pthread_join(emu_thread, NULL);
	at_emulator_destroy(emu_data.emulator);
	at_client_close(&client);
	close(ptyfds[0]);
	close(ptyfds[1]);
}
