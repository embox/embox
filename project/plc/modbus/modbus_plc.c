/**
 * @file
 * @brief
 *
 * @author Dmitry Pilyuk
 * @date 23.01.25
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <plc/modbus_plc.h>

#include "toml.h"

// static bool modbus_started;
static pthread_t modbus_thread;
static server_node_t server_node;
static bool running;

server_settings_t *get_server_settings(void) {
	char errbuf[200];
	FILE *f = fopen("modbus.toml", "r");
	if (!f) {
		fprintf(stderr, "ERROR: No such file\n");
		exit(1);
	}

	toml_table_t *tbl = toml_parse_file(f, errbuf, sizeof(errbuf));
	if (!tbl) {
		fprintf(stderr, "ERROR: %s\n", errbuf);
		exit(1);
	}

	toml_value_t host = toml_table_string(tbl, "host");
	toml_value_t port = toml_table_int(tbl, "port");
	toml_value_t nb_bits = toml_table_int(tbl, "nb_bits");
	toml_value_t start_bits = toml_table_int(tbl, "start_bits");
	toml_value_t nb_input_bits = toml_table_int(tbl, "nb_input_bits");
	toml_value_t start_input_bits = toml_table_int(tbl, "start_input_bits");
	toml_value_t nb_input_registers = toml_table_int(tbl, "nb_input_registers");
	toml_value_t start_input_registers = toml_table_int(tbl, "start_input_"
	                                                         "registers");
	toml_value_t nb_registers = toml_table_int(tbl, "nb_registers");
	toml_value_t start_registers = toml_table_int(tbl, "start_registers");

	server_settings_t *settings = malloc(sizeof(server_settings_t));

	if (host.ok) {
		strcpy(settings->host, host.u.s);
	}
	else {}

	settings->port = (port.ok && port.u.i >= 0) ? port.u.i : 512;
	settings->nb_bits = (nb_bits.ok && nb_bits.u.i >= 0) ? nb_bits.u.i : 0;
	settings->start_bits = (start_bits.ok && start_bits.u.i >= 0)
	                           ? start_bits.u.i
	                           : 0;
	settings->nb_input_bits = (nb_input_bits.ok && nb_input_bits.u.i >= 0)
	                              ? nb_input_bits.u.i
	                              : 0;
	settings->start_input_bits = (start_input_bits.ok
	                                 && start_input_bits.u.i >= 0)
	                                 ? start_input_bits.u.i
	                                 : 0;
	settings->nb_input_registers = (nb_input_registers.ok
	                                   && nb_input_registers.u.i >= 0)
	                                   ? nb_input_registers.u.i
	                                   : 0;
	settings->start_input_registers = (start_input_registers.ok
	                                      && start_input_registers.u.i >= 0)
	                                      ? start_input_registers.u.i
	                                      : 0;
	settings->nb_registers = (nb_registers.ok && nb_registers.u.i >= 0)
	                             ? nb_registers.u.i
	                             : 0;
	settings->start_registers = (start_registers.ok && start_registers.u.i >= 0)
	                                ? start_registers.u.i
	                                : 0;
	return settings;
}

static void *__mb_server_thread(void *_server_node) {
	server_node_t *node = _server_node;
	int listen_socket = -1;
	int client_socket = -1;
	int rc;
	listen_socket = modbus_tcp_listen(node->ctx, 1);
	while(running) {
		client_socket = modbus_tcp_accept(node->ctx, &listen_socket);
		if (-1 == client_socket) {
			break;
		}
		while (running) {
			uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

			rc = modbus_receive(node->ctx, query);
			if (rc > 0) {
				modbus_reply(node->ctx, query, rc, node->mem_area);
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

int modbus_start(void) {
	server_settings_t *settings = get_server_settings();
	strcpy(server_node.ip_adress, settings->host);
	server_node.port = settings->port;
	server_node.mem_area = modbus_mapping_new_start_address(settings->start_bits,
	    settings->nb_bits, settings->start_input_bits, settings->nb_input_bits,
	    settings->start_registers, settings->nb_registers,
	    settings->start_input_registers, settings->nb_input_registers);
	server_node.ctx = modbus_new_tcp(server_node.ip_adress, server_node.port);
	modbus_set_debug(server_node.ctx, TRUE);
	free(settings);
	running = 1;
	pthread_create(&modbus_thread, NULL, __mb_server_thread,
	    (void *)&server_node);
	return 0;
}

int modbus_stop(void) {
	running = 0;
    pthread_join(modbus_thread, NULL);
    modbus_free(server_node.ctx);
    modbus_mapping_free(server_node.mem_area);
    return 0;
}