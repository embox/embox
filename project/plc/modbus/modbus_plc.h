/**
 * @file
 * @brief
 *
 * @author Dmitry Pilyuk
 * @date 23.01.25
 */

#include "modbus.h"

typedef struct _server_node_t {
	const char *ip_adress;
	uint16_t port;
	modbus_mapping_t *mem_area;
	modbus_t *ctx;
} server_node_t;

typedef struct _server_settings_t {
	char ip_adress[12];
	uint16_t port;
	int nb_bits;
	int start_bits;
	int nb_input_bits;
	int start_input_bits;
	int nb_input_registers;
	int start_input_registers;
	int nb_registers;
	int start_registers;
} server_settings_t;


server_settings_t *get_server_settings();