/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 17.07.23
 */
#include <stddef.h>

#include <lib/libds/msg_buff.h>
#include <lib/libds/ring_buff.h>

size_t msg_buff_space(struct msg_buff *buf) {
	size_t space;

	space = ring_buff_get_space(&buf->rbuf);

	if (space < sizeof(size_t)) {
		space = 0;
	}
	else {
		space -= sizeof(size_t);
	}

	return space;
}
