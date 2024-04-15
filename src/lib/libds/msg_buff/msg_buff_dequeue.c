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

size_t msg_buff_dequeue(struct msg_buff *buf, void *dst, size_t nbyte) {
	size_t msg_size;
	size_t nread;
	char tmp[16];

	if (msg_buff_empty(buf)) {
		nread = 0;
	}
	else {
		ring_buff_dequeue(&buf->rbuf, &msg_size, sizeof(size_t));
		if (nbyte < msg_size) {
			msg_size -= ring_buff_dequeue(&buf->rbuf, dst, nbyte);
			while (msg_size) {
				msg_size -= ring_buff_dequeue(&buf->rbuf, tmp, sizeof(tmp));
			}
			nread = 0;
		}
		else {
			nread = ring_buff_dequeue(&buf->rbuf, dst, msg_size);
		}
	}

	return nread;
}
