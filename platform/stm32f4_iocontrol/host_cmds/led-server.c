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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>

#define LED_BITS 16

static void update_leds(uint8_t leds[], size_t leds_n) {
	int i;
	for (i = 0; i < leds_n; i++) {
		fprintf(stderr, "led(%03d)=%d\n", i, !!leds[i]);
	}
}

int main(int argc, char*argv[]) {
	int socket;
	modbus_t *ctx;
	modbus_mapping_t *mb_mapping;
	uint8_t *query;
	const char *ip = "0.0.0.0";
	unsigned short port = 1502;
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

	ctx = modbus_new_tcp(ip, port);
	query = malloc(MODBUS_TCP_MAX_ADU_LENGTH);

	modbus_set_debug(ctx, TRUE);

	mb_mapping = modbus_mapping_new(LED_BITS, 0, 0, 0);
	if (mb_mapping == NULL) {
		fprintf(stderr, "Failed to allocate the mapping: %s\n",
				modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
	}

	socket = modbus_tcp_listen(ctx, 1);
	for (;;) {
		if (-1 == modbus_tcp_accept(ctx, &socket)) {
			break;
		}

		for (;;) {
			int query_len;

			query_len = modbus_receive(ctx, query);
			if (-1 == query_len) {
				/* Connection closed by the client or error */
				break;
			}

			if (-1 == modbus_reply(ctx, query, query_len, mb_mapping)) {
				break;
			}

			update_leds(mb_mapping->tab_bits, mb_mapping->nb_bits);
		}
	}
	printf("exiting: %s\n", modbus_strerror(errno));

	close(socket);
	modbus_mapping_free(mb_mapping);
	free(query);
	modbus_free(ctx);

	return 0;
}
