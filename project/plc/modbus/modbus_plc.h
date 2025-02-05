/**
 * @file
 * @brief
 *
 * @author Dmitry Pilyuk
 * @date 05.02.25
 */

#include "modbus.h"

typedef struct _server_node_t {
	char host[12];
	uint16_t port;
	modbus_mapping_t *mb_mapping;
	modbus_t *ctx;
	uint16_t base_addr;
	uint16_t mb_addr;
	uint16_t addr_bits;
	uint16_t addr_input_bits;
	uint16_t addr_input_registers;
	uint16_t addr_registers;

} server_node_t;

int modbus_server(void);