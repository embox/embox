/**
 * @file
 * @brief Async ring buffer.
 *
 * @date 18.09.2012
 * @author Alexander Kalmuk
 */
#ifndef UTIL_ASYNC_RING_BUFF_H_
#define UTIL_ASYNC_RING_BUFF_H_

#include <kernel/thread/sync/mutex.h>
#include <util/ring_buff.h>

struct async_ring_buff {
	struct ring_buff buffer;
	struct mutex mutex;
};

extern size_t async_ring_buff_enqueue(struct async_ring_buff *buf, void *elem, size_t cnt);

extern size_t async_ring_buff_dequeue(struct async_ring_buff *buf, void *elem, size_t cnt);

extern int async_ring_buff_init(struct async_ring_buff *buf, size_t elem_size, size_t count, void *storage);

extern size_t async_ring_get_cnt(struct async_ring_buff *buf);

extern size_t async_ring_buff_get_space(struct async_ring_buff *buf);

#endif /* UTIL_ASYNC_RING_BUFF_H_ */
