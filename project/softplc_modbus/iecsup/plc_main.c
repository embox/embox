/**
 * @file
 * @brief
 *
 * @author Dmitry Pilyuk
 * @date 21.12.24
 */

#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <kernel/thread/thread_sched_wait.h>
#include <kernel/time/time.h>
#include <kernel/time/timer.h>

#include "iec_std_lib.h"
#include "MB_0.h"

/**
 * Functions and variables provied by generated C softPLC
 */
extern unsigned long long common_ticktime__; // ns
extern unsigned long greatest_tick_count__;  // tick

extern void config_init__(void);
extern void config_run__(unsigned long tick);

/**
 * Functions and variables to export to generated C softPLC
 */
TIME __CURRENT_TIME;
BOOL __DEBUG;

static unsigned long tick_counter;

void modbus_servers_init() {
	for (int i = 0; i < NUMBER_OF_SERVER_NODES; i++) {
		server_nodes[i].ctx = modbus_new_tcp(server_nodes[i].ip_adress,
		    server_nodes[i].port);
		modbus_set_debug(server_nodes[i].ctx, FALSE);
		// tab_bits
		if (server_nodes[i].mem_area.nb_bits == 0) {
			server_nodes[i].mem_area.tab_bits = NULL;
		}
		else {
			server_nodes[i].mem_area.tab_bits = (uint8_t *)malloc(
			    server_nodes[i].mem_area.nb_bits * sizeof(uint8_t));
			if (server_nodes[i].mem_area.tab_bits == NULL) {
				exit(1);
			}
			memset(server_nodes[i].mem_area.tab_bits, 0,
			    server_nodes[i].mem_area.nb_bits * sizeof(uint8_t));
		}

		// tab_input_bits
		if (server_nodes[i].mem_area.nb_input_bits == 0) {
			server_nodes[i].mem_area.tab_input_bits = NULL;
		}
		else {
			server_nodes[i].mem_area.tab_input_bits = (uint8_t *)malloc(
			    server_nodes[i].mem_area.nb_input_bits * sizeof(uint8_t));
			if (server_nodes[i].mem_area.tab_input_bits == NULL) {
				free(server_nodes[i].mem_area.tab_bits);
				exit(1);
			}
			memset(server_nodes[i].mem_area.tab_input_bits, 0,
			    server_nodes[i].mem_area.nb_input_bits * sizeof(uint8_t));
		}

		// tab_input_registers
		if (server_nodes[i].mem_area.nb_input_registers == 0) {
			server_nodes[i].mem_area.tab_input_registers = NULL;
		}
		else {
			server_nodes[i].mem_area.tab_input_registers = (uint16_t *)malloc(
			    server_nodes[i].mem_area.nb_input_registers * sizeof(uint16_t));
			if (server_nodes[i].mem_area.tab_input_registers == NULL) {
				free(server_nodes[i].mem_area.tab_bits);
				free(server_nodes[i].mem_area.tab_input_bits);
				exit(1);
			}
			memset(server_nodes[i].mem_area.tab_input_registers, 0,
			    server_nodes[i].mem_area.nb_input_registers * sizeof(uint16_t));
		}

		// tab_input_registers
		if (server_nodes[i].mem_area.nb_registers == 0) {
			server_nodes[i].mem_area.tab_registers = NULL;
		}
		else {
			server_nodes[i].mem_area.tab_registers = (uint16_t *)malloc(
			    server_nodes[i].mem_area.nb_registers * sizeof(uint16_t));
			if (server_nodes[i].mem_area.tab_registers == NULL) {
				free(server_nodes[i].mem_area.tab_bits);
				free(server_nodes[i].mem_area.tab_input_bits);
				free(server_nodes[i].mem_area.tab_input_registers);
				exit(1);
			}
			memset(server_nodes[i].mem_area.tab_registers, 0,
			    server_nodes[i].mem_area.nb_registers * sizeof(uint16_t));
		}
	}
	LOC_VARS_INIT
}

static void *__mb_server_thread(void *_server_node) {
	server_node_t *node = _server_node;
	int listen_socket = -1;
	int client_socket = -1;
	int rc;
	listen_socket = modbus_tcp_listen(node->ctx, 1);

	for (;;) {
		client_socket = modbus_tcp_accept(node->ctx, &listen_socket);
		if (-1 == client_socket) {
			break;
		}

		for (;;) {
			uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

			rc = modbus_receive(node->ctx, query);
			if (rc > 0) {
				modbus_reply(node->ctx, query, rc, &(node->mem_area));
			}
			else if (rc == -1) {
				/* Connection closed by the client or error */
				break;
			}
		}

		close(client_socket);
	}
	printf("exiting: %s\n", modbus_strerror(errno));

	close(listen_socket);

	fprintf(stderr,
	    "Modbus plugin: Modbus server died unexpectedly!\n"); /* should never occur */
	return NULL;
}

static void plc_handler(sys_timer_t *timer, void *param) {
	struct timespec ts;

	tick_counter += 1;
	if (greatest_tick_count__) {
		tick_counter %= greatest_tick_count__;
	}

	clock_gettime(CLOCK_REALTIME, &ts);

	__CURRENT_TIME.tv_sec = ts.tv_sec;
	__CURRENT_TIME.tv_nsec = ts.tv_nsec;

	config_run__(tick_counter);
}

int main(int argc, char const *argv[]) {
	sys_timer_t *timer;
	uint32_t ticktime_ms;
	int err;
	pthread_t threads[NUMBER_OF_SERVER_NODES];

	modbus_servers_init();
	config_init__();

	for (int i = 0; i < NUMBER_OF_SERVER_NODES; i++) {
		pthread_create(&threads[i], NULL, __mb_server_thread,
		    (void *)&(server_nodes[i]));
	}
	tick_counter = 0;
	ticktime_ms = common_ticktime__ / NSEC_PER_MSEC;

	err = timer_set(&timer, TIMER_PERIODIC, ticktime_ms, plc_handler, NULL);

	if (!err) {
		err = SCHED_WAIT(0);
	}

	for (int i = 0; i < NUMBER_OF_SERVER_NODES; i++) {
		pthread_join(threads[i], NULL);
	}

	return err;
}
