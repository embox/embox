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

extern int async_ring_buff_enque(struct async_ring_buff *buf, void *elem, size_t cnt);

extern int async_ring_buff_deque(struct async_ring_buff *buf, void *elem, size_t cnt);

extern int async_ring_buff_init(struct async_ring_buff *buf, size_t elem_size, int count, void *storage);

extern int async_ring_buff_empty(struct async_ring_buff *buf);

extern int async_ring_buff_available_space(struct async_ring_buff *buf);

#endif /* UTIL_ASYNC_RING_BUFF_H_ */
