/**
 * @file show_packet.h
 * @brief Show packet content
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-09-21
 */

#ifndef SHOW_PACKET_H
#define SHOW_PACKET_H

#include <util/log.h>
#include <stdint.h>


#define STR_BYTES 16


static inline void show_packet(uint8_t *raw, int size, char *title) {
	if (!&mod_logger)
		return;

	if (mod_logger.logging.level >= LOG_DEBUG - 1) {

		log_debug("PACKET(%d) %s:\n", size, title);
		int rows = (size + STR_BYTES - 1) / STR_BYTES;
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < STR_BYTES; j++) {
				int pos = i * STR_BYTES + j;
				if (pos < size) {
					log_debug(" %02hhX", *(raw + pos));
				}
				else {
					log_debug("   ");
				}
			}
			log_debug("   ");
			for (int j = 0; j < STR_BYTES; j++) {
				int pos = i * STR_BYTES + j;
				if (pos < size) {
					char c = (char) *(raw + pos);
					if (c < 33 || c > 126)
						c = '.';

					log_debug("%c", c);
				}
			}
			log_debug("\n");
		}
	}
}

#endif /* SHOW_PACKET_H */
