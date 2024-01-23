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
