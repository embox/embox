/**
 * @file
 * @brief ESP32-C3 AT Emulator Header
 * 
 * @date July 31, 2025
 * @author Peize Li
 */

#ifndef ESP32C3_EMULATOR_H
#define ESP32C3_EMULATOR_H

#include <at/emulator/at_emulator.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/time/timer.h>

/**
 * @brief ESP32-C3 WiFi state
 */
typedef enum {
	ESP32_WIFI_IDLE = 0,
	ESP32_WIFI_CONNECTING,
	ESP32_WIFI_CONNECTED,
	ESP32_WIFI_GOT_IP,
	ESP32_WIFI_DISCONNECTED
} esp32_wifi_state_t;

/**
 * @brief ESP32-C3 emulator specific data
 */
typedef struct {
	/* WiFi state machine */
	esp32_wifi_state_t wifi_state;
	char current_ssid[33];
	char current_password[65];
	char ip_addr[16];
	char gateway[16];
	char netmask[16];

	/* WiFi mode: 1=STA, 2=AP, 3=STA+AP */
	int wifi_mode;

	/* Connection timers - separated for proper management */
	struct sys_timer conn_timer; /* WiFi connection timer */
	struct sys_timer ip_timer;   /* IP acquisition timer */
	bool timer_active;           /* Connection timer active */
	bool ip_timer_active;        /* IP timer active */

	/* Timer synchronization mutex */
	struct mutex timer_mutex; /* Protects timer operations */

	/* State machine mutex */
	struct mutex state_mutex; /* Protects wifi_state transitions */

	/* TCP/IP state */
	struct {
		bool connected;
		char remote_ip[16];
		int remote_port;
		int mux; /* Connection multiplexing ID */
	} tcp_state;

	/* WiFi scan results */
	struct {
		char ssid[33];
		int rssi;
		int channel;
		int enc; /* Encryption type */
	} scan_results[5];
	int scan_count;

	/* Parent emulator reference */
	at_emulator_t *base_emu;

	/* Special flags */
	bool rst_pending; /* "READY" message pending after reset */
	int sleep_mode;

} esp32c3_emu_data_t;

/**
 * @brief Create ESP32-C3 emulator
 * 
 * Creates an AT emulator instance configured for ESP32-C3 WiFi module.
 * The emulator simulates WiFi connection, IP acquisition, and basic TCP/IP.
 * 
 * @return Configured AT emulator instance, NULL on failure
 */
at_emulator_t *esp32c3_emulator_create(void);

/**
 * @brief Destroy ESP32-C3 emulator
 * 
 * Cleans up all resources including active timers and allocated memory.
 * 
 * @param emu Emulator instance to destroy
 */
void esp32c3_emulator_destroy(at_emulator_t *emu);

#endif /* ESP32C3_EMULATOR_H */