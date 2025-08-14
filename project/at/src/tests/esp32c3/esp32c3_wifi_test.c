/**
 * @file
 * @brief ESP32-C3 WiFi Driver Unit Tests
 *
 * @date Aug 9, 2025
 * @author Peize Li
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <wifi/esp32c3/emulator/esp32c3_emulator.h>
#include <embox/test.h>
#include <net/netdevice.h>
#include <net/wlan/wireless_ioctl.h>
#include <net/wlan/wlan_core.h>
#include <wifi/esp32c3/esp32c3_wifi.h>

EMBOX_TEST_SUITE("ESP32-C3 WiFi Driver Tests");

#define TEST_EMULATOR_STARTUP_DELAY_US   10000  /* 10ms */
#define TEST_EMULATOR_THREAD_DELAY_US    500    /* 0.5ms */
#define TEST_EMULATOR_POLL_DELAY_US      5000   /* 5ms */
#define TEST_POST_INIT_DELAY_US          10000  /* 10ms */
#define TEST_POST_SCAN_DELAY_US          5000   /* 5ms */
#define TEST_POST_CONNECT_DELAY_US       5000   /* 5ms */
#define TEST_POST_DISCONNECT_DELAY_US    10000  /* 10ms */

extern int ppty(int ptyfds[2]);

struct emulator_thread_data {
	int fd;
	at_emulator_t *emulator;
	volatile int running;
	pthread_t thread;
	pthread_t poll_thread;
};

/* Helper Thread Functions */

static void *emulator_thread_func(void *arg) {
	struct emulator_thread_data *data = (struct emulator_thread_data *)arg;
	char buffer[256];

	while (data->running) {
		int n = read(data->fd, buffer, sizeof(buffer));
		if (n > 0) {
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

static void *emulator_poll_thread(void *arg) {
	struct emulator_thread_data *data = (struct emulator_thread_data *)arg;

	while (data->running) {
		const char *urc = at_emulator_poll(data->emulator);
		if (urc && *urc) {
			write(data->fd, urc, strlen(urc));
		}
		usleep(TEST_EMULATOR_POLL_DELAY_US); /* 5ms */
	}

	return NULL;
}

/* Test Environment Setup */
static int setup_test_env(int ptyfds[2], struct emulator_thread_data *emu_data) {
	if (ppty(ptyfds) != 0) {
		return -1;
	}

	fcntl(ptyfds[1], F_SETFL, O_NONBLOCK);

	emu_data->emulator = esp32c3_emulator_create();
	if (!emu_data->emulator) {
		close(ptyfds[0]);
		close(ptyfds[1]);
		return -1;
	}

	emu_data->fd = ptyfds[1];
	emu_data->running = 1;

	pthread_create(&emu_data->thread, NULL, emulator_thread_func, emu_data);
	pthread_create(&emu_data->poll_thread, NULL, emulator_poll_thread, emu_data);

	usleep(TEST_EMULATOR_STARTUP_DELAY_US); /* Let emulator start */
	return 0;
}

/* Test Environment Cleanup */
static void cleanup_test_env(int ptyfds[2], struct emulator_thread_data *emu_data) {
	emu_data->running = 0;
	pthread_join(emu_data->thread, NULL);
	pthread_join(emu_data->poll_thread, NULL);
	esp32c3_emulator_destroy(emu_data->emulator);
	close(ptyfds[0]);
	close(ptyfds[1]);
}

TEST_CASE("Driver initialization") {
	int ptyfds[2];
	struct emulator_thread_data emu_data;
	struct net_device netdev = {0};

	test_assert_zero(setup_test_env(ptyfds, &emu_data));

	strcpy(netdev.name, "wlan0");
	test_assert_zero(esp32c3_wifi_init_fd(&netdev, ptyfds[0]));
	test_assert_equal(netdev.flags & IFF_WIRELESS, IFF_WIRELESS);
	test_assert_not_null(netdev.wireless_ops);
	test_assert_not_null(netdev.wireless_priv);

	esp32c3_wifi_cleanup(&netdev);
	cleanup_test_env(ptyfds, &emu_data);
}

TEST_CASE("Parameter validation") {
	struct net_device netdev = {0};

	test_assert_equal(esp32c3_wifi_init_fd(NULL, 0), -EINVAL);
	test_assert_equal(esp32c3_wifi_init_fd(&netdev, -1), -EINVAL);
	test_assert_equal(esp32c3_wifi_init(&netdev, NULL), -EINVAL);
}

TEST_CASE("Connect to non-existent AP") {
	int ptyfds[2];
	struct emulator_thread_data emu_data;
	struct net_device netdev = {0};

	test_assert_zero(setup_test_env(ptyfds, &emu_data));

	strcpy(netdev.name, "wlan0");
	test_assert_zero(esp32c3_wifi_init_fd(&netdev, ptyfds[0]));

	usleep(TEST_POST_CONNECT_DELAY_US);

	int ret = netdev.wireless_ops->connect(&netdev, "NonExistentAP", "password");
	test_assert_not_equal(ret, 0);

	esp32c3_wifi_cleanup(&netdev);
	cleanup_test_env(ptyfds, &emu_data);
}

TEST_CASE("Sequential operations") {
	int ptyfds[2];
	struct emulator_thread_data emu_data;
	struct net_device netdev = {0};

	test_assert_zero(setup_test_env(ptyfds, &emu_data));

	strcpy(netdev.name, "wlan0");
	test_assert_zero(esp32c3_wifi_init_fd(&netdev, ptyfds[0]));

	usleep(TEST_POST_INIT_DELAY_US);

	int scan_result = netdev.wireless_ops->scan(&netdev);
	if (scan_result == 0) {
		usleep(TEST_POST_SCAN_DELAY_US);

		int conn_result = netdev.wireless_ops->connect(&netdev, "TestAP_1", "pass");
		if (conn_result == 0) {
			usleep(TEST_POST_CONNECT_DELAY_US);
			netdev.wireless_ops->disconnect(&netdev);
			usleep(TEST_POST_DISCONNECT_DELAY_US);
		}
	}

	esp32c3_wifi_cleanup(&netdev);
	cleanup_test_env(ptyfds, &emu_data);
}

TEST_CASE("Cleanup resilience") {
	int ptyfds[2];
	struct emulator_thread_data emu_data;
	struct net_device netdev = {0};

	test_assert_zero(setup_test_env(ptyfds, &emu_data));

	strcpy(netdev.name, "wlan0");
	test_assert_zero(esp32c3_wifi_init_fd(&netdev, ptyfds[0]));

	/* Double cleanup should not crash */
	esp32c3_wifi_cleanup(&netdev);
	esp32c3_wifi_cleanup(&netdev);

	/* Cleanup on uninitialized device should not crash */
	struct net_device empty_dev = {0};
	esp32c3_wifi_cleanup(&empty_dev);

	cleanup_test_env(ptyfds, &emu_data);
}
