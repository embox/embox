/**
 * @file show_packet.h
 * @brief Show packet content
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-09-21
 */

#ifndef SHOW_PACKET_H
#define SHOW_PACKET_H

#include <kernel/printk.h>
#include <util/log.h>

#define STR_BYTES 16

extern struct logger mod_logger __attribute__ ((weak));

static inline void show_packet(uint8_t *raw, int size, char *title) {
	if (!&mod_logger)
		return;

	if (&mod_logger.logging &&
		mod_logger.logging.level >= LOG_DEBUG - 1) {

		printk("\nPACKET(%d) %s:\n", size, title);
		int rows = (size + STR_BYTES - 1) / STR_BYTES;
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < STR_BYTES; j++) {
				int pos = i * STR_BYTES + j;
				if (pos < size)
					printk(" %02hhX", *(raw + pos));
				else
					printk("   ");
			}
			printk("   ");
			for (int j = 0; j < STR_BYTES; j++) {
				int pos = i * STR_BYTES + j;
				if (pos < size) {
					char c = (char) *(raw + pos);
					if (c < 33 || c > 126)
						c = '.';

					printk("%c", c);
				}
			}
			printk("\n");
		}
	}
}

#endif /* SHOW_PACKET_H */
