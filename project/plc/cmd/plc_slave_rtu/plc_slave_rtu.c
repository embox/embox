/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 16.03.23
 */

#include <errno.h>
#include <fcntl.h>
#include <modbus.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <framework/mod/options.h>
#include <plc/core.h>

#define DEBUG_MODE OPTION_GET(BOOLEAN, debug_mode)

#define NB_BITS       OPTION_GET(NUMBER, nb_bits)
#define NB_REGS       OPTION_GET(NUMBER, nb_regs)
#define NB_INPUT_BITS OPTION_GET(NUMBER, nb_input_bits)
#define NB_INPUT_REGS OPTION_GET(NUMBER, nb_input_regs)
#define SLAVE_ID      OPTION_GET(NUMBER, slave_id)

static modbus_mapping_t *mb_mapping;

static void print_help_msg(const char *cmd) {
	fprintf(stderr, "Usage: %s [TTY_DEV]\n", cmd);
}

static void print_modbus_err(const char *cmd, const char *func) {
	const char *msg;

	msg = errno ? modbus_strerror(errno) : "Unknown error";
	fprintf(stderr, "%s: %s(): %s\n", cmd, func, msg);
}

static void update_mb_mapping(const struct plc_slave_var *var) {
	switch (var->type) {
	case PLC_SLAVE_VAR_QX:
		if (var->addr < NB_BITS) {
			mb_mapping->tab_bits[var->addr] = *(uint8_t *)var->data;
		}
		break;

	case PLC_SLAVE_VAR_IX:
		if (var->addr < NB_INPUT_BITS) {
			mb_mapping->tab_input_bits[var->addr] = *(uint8_t *)var->data;
		}
		break;

	case PLC_SLAVE_VAR_IW:
		if (var->addr < NB_INPUT_REGS) {
			mb_mapping->tab_input_registers[var->addr] = *(uint16_t *)var->data;
		}
		break;

	case PLC_SLAVE_VAR_MW:
		if (var->addr < NB_REGS) {
			mb_mapping->tab_registers[var->addr] = *(uint16_t *)var->data;
		}
		break;
	}
}

static void update_slave_var(const struct plc_slave_var *var) {
	switch (var->type) {
	case PLC_SLAVE_VAR_QX:
		if (var->addr < NB_BITS) {
			*(uint8_t *)var->data = mb_mapping->tab_bits[var->addr];
		}
		break;

	case PLC_SLAVE_VAR_MW:
		if (var->addr < NB_REGS) {
			*(uint16_t *)var->data = mb_mapping->tab_registers[var->addr];
		}
		break;
	}
}

int main(int argc, char **argv) {
	uint8_t request[MODBUS_TCP_MAX_ADU_LENGTH];
	modbus_t *ctx;
	int request_len;

	if (argc != 2) {
		print_help_msg(argv[0]);
		return -1;
	}

	/**
	 * The baud rate parameter does not matter.
	 * Use "stty" command to change baud rate.
	 */
	ctx = modbus_new_rtu(argv[1], 38400, 'N', 8, 1);
	if (!ctx) {
		print_modbus_err(argv[0], "modbus_new_rtu");
		return -1;
	}

	modbus_set_slave(ctx, SLAVE_ID);
	modbus_set_debug(ctx, DEBUG_MODE);

	mb_mapping = modbus_mapping_new(NB_BITS, NB_INPUT_BITS, NB_REGS,
	    NB_INPUT_REGS);
	if (!mb_mapping) {
		print_modbus_err(argv[0], "modbus_mapping_new");
		modbus_free(ctx);
		return -1;
	}

	for (;;) {
		if (-1 == modbus_connect(ctx)) {
			print_modbus_err(argv[0], "modbus_connect");
			sleep(1);
			continue;
		}

		for (;;) {
			request_len = modbus_receive(ctx, request);
			if (-1 == request_len) {
				print_modbus_err(argv[0], "modbus_receive");
				sleep(1);
				break;
			}

			plc_slave_handle(update_mb_mapping);

			if (-1 == modbus_reply(ctx, request, request_len, mb_mapping)) {
				print_modbus_err(argv[0], "modbus_reply");
				sleep(1);
				break;
			}

			plc_slave_handle(update_slave_var);
		}

		modbus_close(ctx);
	}

	modbus_mapping_free(mb_mapping);
	modbus_free(ctx);

	return 0;
}
