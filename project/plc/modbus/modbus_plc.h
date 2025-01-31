/**
 * @file
 * @brief
 *
 * @author Dmitry Pilyuk
 * @date 23.01.25
 */

#include "modbus.h"

typedef struct _server_node_t {
	char ip_adress[12];
	uint16_t port;
	modbus_mapping_t *mem_area;
	modbus_t *ctx;
} server_node_t;

typedef struct _server_settings_t {
	char host[12];
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


int modbus_start(void);

int modbus_stop(void);