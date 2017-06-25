/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.11.2014
 */

/*
 * Copyright © 2008-2010 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>

int main(int argc, char *argv[]) {
	const char *ip = "127.0.0.1";
	unsigned short port = 1502;
	modbus_t *ctx;
	char bit_value;
	unsigned int bit_n;
	bool input_err;
	int opt;

	while (-1 != (opt = getopt(argc, argv, "a:p:"))) {
		switch (opt) {
		case 'a':
			ip = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		}
	}


	input_err = true;
	if (argc - optind == 2) {
		bool op_set, op_clr;
		if ((op_set = (0 == strcmp(argv[optind], "set")))
				|| (op_clr = (0 == strcmp(argv[optind], "clr")))) {
			bit_value = op_set ? ON : OFF;
			bit_n = atoi(argv[optind + 1]);
			input_err = false;
		}
	}

	if (input_err) {
		fprintf(stderr, "invalid invocation\n"
			"Usage: %s [-a ADDR] [-p PORT] [set|clr] BIT_NUM\n", argv[0]);
		return -1;
	}

	ctx = modbus_new_tcp(ip, port);
	if (ctx == NULL) {
		fprintf(stderr, "Unable to allocate libmodbus context\n");
		return -1;
	}

	modbus_set_debug(ctx, TRUE);
	modbus_set_error_recovery(ctx,
			MODBUS_ERROR_RECOVERY_LINK |
			MODBUS_ERROR_RECOVERY_PROTOCOL);

	if (modbus_connect(ctx) == -1) {
		fprintf(stderr, "Connection failed: %s\n",
				modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
	}


	if (1 == modbus_write_bit(ctx, bit_n, bit_value)) {
		printf("OK\n");
	} else {
		printf("FAILED\n");
	}

	/* Close the connection */
	modbus_close(ctx);
	modbus_free(ctx);

	return 0;
}
