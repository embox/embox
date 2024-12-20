/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 20.10.24
 */

#include <time.h>
#include <unistd.h>

#include <kernel/thread/thread_sched_wait.h>
#include <kernel/time/time.h>
#include <kernel/time/timer.h>

#include "iec_std_lib.h"

#include <pthread.h>

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
		server_nodes[i].ctx = modbus_new_tcp(server_nodes[i].ip_adress, server_nodes[i].port);
		modbus_set_debug(server_nodes[i].ctx, FALSE);
		// tab_bits
		if (server_nodes[i].mem_area.nb_bits == 0) {
			server_nodes[i].mem_area.tab_bits = NULL;
		} else {
			server_nodes[i].mem_area.tab_bits = (uint8_t *) malloc(server_nodes[i].mem_area.nb_bits * sizeof(uint8_t));
			if (server_nodes[i].mem_area.tab_bits == NULL) {
				exit(1);
			}
			memset(server_nodes[i].mem_area.tab_bits, 0, server_nodes[i].mem_area.nb_bits * sizeof(uint8_t));
		}

		// tab_input_bits
		if (server_nodes[i].mem_area.nb_input_bits == 0) {
			server_nodes[i].mem_area.tab_input_bits = NULL;
		} else {
			server_nodes[i].mem_area.tab_input_bits = (uint8_t *) malloc(server_nodes[i].mem_area.nb_input_bits * sizeof(uint8_t));
			if (server_nodes[i].mem_area.tab_input_bits == NULL) {
				free(server_nodes[i].mem_area.tab_bits);
				exit(1);
			}
			memset(server_nodes[i].mem_area.tab_input_bits, 0, server_nodes[i].mem_area.nb_input_bits * sizeof(uint8_t));
		}

		// tab_input_registers
		if (server_nodes[i].mem_area.nb_input_registers == 0) {
			server_nodes[i].mem_area.tab_input_registers = NULL;
		} else {
			server_nodes[i].mem_area.tab_input_registers = (uint16_t *) malloc(server_nodes[i].mem_area.nb_input_registers * sizeof(uint16_t));
			if (server_nodes[i].mem_area.tab_input_registers == NULL) {
				free(server_nodes[i].mem_area.tab_bits);
				free(server_nodes[i].mem_area.tab_input_bits);
				exit(1);
			}
			memset(server_nodes[i].mem_area.tab_input_registers, 0, server_nodes[i].mem_area.nb_input_registers * sizeof(uint16_t));
		}

		// tab_input_registers
		if (server_nodes[i].mem_area.nb_registers == 0) {
			server_nodes[i].mem_area.tab_registers = NULL;
		} else {
			server_nodes[i].mem_area.tab_registers = (uint16_t *) malloc(server_nodes[i].mem_area.nb_registers * sizeof(uint16_t));
			if (server_nodes[i].mem_area.tab_registers == NULL) {
				free(server_nodes[i].mem_area.tab_bits);
				free(server_nodes[i].mem_area.tab_input_bits);
				free(server_nodes[i].mem_area.tab_input_registers);
				exit(1);
			}
			memset(server_nodes[i].mem_area.tab_registers, 0, server_nodes[i].mem_area.nb_registers * sizeof(uint16_t));
		}
	}
	LOC_VARS_INIT
}

static void *__mb_server_thread(void *_server_node)  {
	int header_len;
	int listen_socket, client_socket;
	uint8_t *query;
	server_node_t *server_node = _server_node;
	header_len = modbus_get_header_length(server_node->ctx);
	query = malloc(MODBUS_TCP_MAX_ADU_LENGTH);
	// Enable thread cancelation. Enabled is default, but set it anyway to be safe.
	// pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	listen_socket = modbus_tcp_listen(server_node->ctx, 1);
	for (;;) {
		client_socket = modbus_tcp_accept(server_node->ctx, &listen_socket);
		if (-1 == client_socket) {
			break;
		}

		for (;;) {
			int query_len;

			query_len = modbus_receive(server_node->ctx, query);
			if (-1 == query_len) {
				/* Connection closed by the client or error */
				break;
			}

			if (query[header_len - 1] != MODBUS_TCP_SLAVE) {
				continue;
			}


			if (-1 == modbus_reply(server_node->ctx, query, query_len, &(server_node->mem_area))) {
				break;
			}

		}

		close(client_socket);
	}
	printf("exiting: %s\n", modbus_strerror(errno));

	close(listen_socket);

	free(query);
	
	fprintf(stderr, "Modbus plugin: Modbus server died unexpectedly!\n"); /* should never occur */
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

	config_init__();
	modbus_servers_init();

	for (int i = 0; i < NUMBER_OF_SERVER_NODES; i++) {
		pthread_create(&threads[i], NULL, __mb_server_thread, (void *)&(server_nodes[i]));
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
