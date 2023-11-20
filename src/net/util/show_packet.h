/**
 * @file show_packet.h
 * @brief Show packet content
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-09-21
 */

#include <stdint.h>

#include <util/log.h>

#define STR_BYTES 16

static inline void show_packet(uint8_t *raw, int size, char *title) {
	int rows;
	int pos;
	int i, j;
	char ch;

	if (log_level_self() < LOG_DEBUG) {
		return;
	}

	log_debug("PACKET(%d) %s:", size, title);

	rows = (size + STR_BYTES - 1) / STR_BYTES;

	for (i = 0; i < rows; i++) {
		log_beg(LOG_DEBUG, "\t");

		for (j = 0; j < STR_BYTES; j++) {
			pos = i * STR_BYTES + j;
			if (pos < size) {
				log_raw(LOG_DEBUG, " %02hhX", *(raw + pos));
			}
			else {
				log_raw(LOG_DEBUG, "   ");
			}
		}

		log_raw(LOG_DEBUG, "   ");

		for (j = 0; j < STR_BYTES; j++) {
			pos = i * STR_BYTES + j;
			if (pos < size) {
				ch = (char)*(raw + pos);
				if (ch < 33 || ch > 126) {
					ch = '.';
				}
				log_raw(LOG_DEBUG, "%c", ch);
			}
		}

		log_end(LOG_DEBUG, "");
	}
}
