/**
 * @file
 * @brief
 *
 * @author Dmitry Pilyuk
 * @date 23.01.25
 */

#include <stdio.h>
#include <stdlib.h>

#include <plc/modbus_plc.h>

#include "toml.h"

server_settings_t *get_server_settings() {
	char errbuf[200];
	FILE *f = fopen("modbus.toml");
	if (!f) {
		fprintf(stderr, "ERROR: No such file\n");
		exit(1);
	}

	toml_table_t *tbl = toml_parse_file(fp, errbuf, sizeof(errbuf));
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
