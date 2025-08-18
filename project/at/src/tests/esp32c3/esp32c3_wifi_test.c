/**
 * @file
 * @brief ESP32-C3 WiFi Driver Unit Tests 
 *
 * @date Aug 15, 2025
 * @author Peize Li
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include <embox/test.h>
#include <wifi/esp32c3/emulator/esp32c3_emulator.h>
#include <wifi/esp32c3/esp32c3_wifi.h>

EMBOX_TEST_SUITE("ESP32-C3 WiFi Driver Tests");

extern int ppty(int ptyfds[2]);

/* Shared test resources */
static int g_ptyfds[2] = {-1, -1};
static at_emulator_t *g_emu = NULL;
static pthread_t g_emu_thread;
static volatile int g_running = 0;
static pthread_mutex_t g_init_lock = PTHREAD_MUTEX_INITIALIZER;
static int g_init_count = 0;

static void *emu_thread_func(void *arg) {
	(void)arg;
	char buf[256];

	while (g_running) {
		int n = read(g_ptyfds[1], buf, sizeof(buf));
		if (n > 0) {
			for (int i = 0; i < n; i++) {
				const char *resp = at_emulator_process(g_emu, &buf[i], 1);
				if (resp && *resp) {
					write(g_ptyfds[1], resp, strlen(resp));
				}
			}
		}

		const char *urc = at_emulator_poll(g_emu);
		if (urc && *urc) {
			write(g_ptyfds[1], urc, strlen(urc));
		}

		usleep(50);
	}
	return NULL;
}

/* Lazy initialization */
static int ensure_env_ready(void) {
	pthread_mutex_lock(&g_init_lock);

	if (g_init_count++ == 0) {
		/* First test - initialize */
		if (ppty(g_ptyfds) != 0) {
			pthread_mutex_unlock(&g_init_lock);
			return -1;
		}

		fcntl(g_ptyfds[1], F_SETFL, O_NONBLOCK);

		g_emu = esp32c3_emulator_create();
		if (!g_emu) {
			close(g_ptyfds[0]);
			close(g_ptyfds[1]);
			pthread_mutex_unlock(&g_init_lock);
			return -1;
		}

		g_running = 1;
		pthread_create(&g_emu_thread, NULL, emu_thread_func, NULL);
		usleep(1000); /* 1ms for emulator startup */
	}

	pthread_mutex_unlock(&g_init_lock);
	return 0;
}

TEST_CASE("Quick init and cleanup") {
	struct wiphy *wiphy;

	test_assert_zero(ensure_env_ready());

	wiphy = esp32c3_wifi_init_fd("wlan0", g_ptyfds[0]);
	test_assert_not_null(wiphy);
	esp32c3_wifi_cleanup(wiphy);
}

TEST_CASE("AT command basic") {
	struct wiphy *wiphy;
	char resp[64];

	test_assert_zero(ensure_env_ready());

	wiphy = esp32c3_wifi_init_fd("wlan0", g_ptyfds[0]);
	test_assert_not_null(wiphy);

	test_assert_zero(esp32c3_send_at_command(wiphy, "AT", resp, sizeof(resp), 100));

	esp32c3_wifi_cleanup(wiphy);
}

TEST_CASE("Mode switching") {
	struct wiphy *wiphy;

	test_assert_zero(ensure_env_ready());

	wiphy = esp32c3_wifi_init_fd("wlan0", g_ptyfds[0]);
	test_assert_not_null(wiphy);

	test_assert_zero(esp32c3_set_wifi_mode(wiphy, 1));
	test_assert_zero(esp32c3_set_wifi_mode(wiphy, 2));

	esp32c3_wifi_cleanup(wiphy);
}

TEST_CASE("Status check") {
	struct wiphy *wiphy;
	struct esp32c3_status status;

	test_assert_zero(ensure_env_ready());

	wiphy = esp32c3_wifi_init_fd("wlan0", g_ptyfds[0]);
	test_assert_not_null(wiphy);

	test_assert_zero(esp32c3_get_status(wiphy, &status));
	test_assert_equal(status.connected, false);

	esp32c3_wifi_cleanup(wiphy);
}

TEST_CASE("Null guards") {
	/* No environment needed for NULL tests */
	esp32c3_wifi_cleanup(NULL);
	test_assert_not_zero(esp32c3_set_wifi_mode(NULL, 0));
	test_assert_not_zero(esp32c3_send_at_command(NULL, "AT", NULL, 0, 0));
}

/* Cleanup at program exit */
__attribute__((destructor)) static void cleanup_test_env(void) {
	if (g_running) {
		g_running = 0;
		pthread_join(g_emu_thread, NULL);
	}

	if (g_emu) {
		esp32c3_emulator_destroy(g_emu);
		g_emu = NULL;
	}

	if (g_ptyfds[0] >= 0) {
		close(g_ptyfds[0]);
		close(g_ptyfds[1]);
		g_ptyfds[0] = -1;
		g_ptyfds[1] = -1;
	}
}
