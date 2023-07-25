/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 17.07.23
 */
#include <stddef.h>

#include <util/msg_buff.h>
#include <util/ring_buff.h>

void msg_buff_init(struct msg_buff *buf, void *storage, size_t size) {
	ring_buff_init(&buf->rbuf, 1, size, storage);
}

size_t msg_buff_enqueue(struct msg_buff *buf, const void *src, size_t nbyte) {
	size_t nwritten;

	if (!nbyte || (ring_buff_get_space(&buf->rbuf) < nbyte + sizeof(size_t))) {
		nwritten = 0;
	}
	else {
		ring_buff_enqueue(&buf->rbuf, &nbyte, sizeof(size_t));
		nwritten = ring_buff_enqueue(&buf->rbuf, (void *)src, nbyte);
	}

	return nwritten;
}

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

int msg_buff_empty(struct msg_buff *buf) {
	return (0 == ring_buff_get_cnt(&buf->rbuf));
}
