/**
 * @file show_packet.h
 * @brief Show packet content
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-09-21
 */

#ifndef SHOW_PACKET_H
#define SHOW_PACKET_H

#include <stdint.h>

#include <framework/mod/options.h>
#include <util/log.h>

#define STR_BYTES 16

static inline void show_packet(uint8_t *raw, int size, char *title) {
	if (LOG_LEVEL == LOG_NONE) {
		return;
	}

	if (LOG_LEVEL >= LOG_DEBUG - 1) {
		log_raw(LOG_DEBUG, "PACKET(%d) %s:\n", size, title);
		int rows = (size + STR_BYTES - 1) / STR_BYTES;
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < STR_BYTES; j++) {
				int pos = i * STR_BYTES + j;
				if (pos < size) {
					log_raw(LOG_DEBUG, " %02hhX", *(raw + pos));
				}
				else {
					log_raw(LOG_DEBUG, "   ");
				}
			}
			log_raw(LOG_DEBUG, "   ");
			for (int j = 0; j < STR_BYTES; j++) {
				int pos = i * STR_BYTES + j;
				if (pos < size) {
					char c = (char)*(raw + pos);
					if (c < 33 || c > 126)
						c = '.';

					log_raw(LOG_DEBUG, "%c", c);
				}
			}
			log_raw(LOG_DEBUG, "\n");
		}
	}
}

#endif /* SHOW_PACKET_H */
