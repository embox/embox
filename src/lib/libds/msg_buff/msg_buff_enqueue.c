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
