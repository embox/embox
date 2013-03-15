/**
 * @file
 * @brief Ring buffer interface
 *
 * @date 21.10.11
 * @author Anton Kozlov
 */

#ifndef UTIL_RING_BUFF_H_
#define UTIL_RING_BUFF_H_

#include <stddef.h>

struct ring_buff {
	size_t capacity;      /**< capacity of buffer in elements */
	size_t cnt;           /**< element counter in buffer*/
	int p_write;       /**< pointer to next write element*/
	int p_read;        /**< pointer to next read element*/
	void *storage;     /**< storage */
	size_t elem_size;     /**< size of element */
};

/**
 * @brief Enqueue elements from @a elem array of lenght @a cnt. It cannot enqueue more
 * than buffer can hold.
 *
 * @param buf
 * @param elem
 * @param cnt
 *
 * @return Count of enqueued elements
 */
extern int ring_buff_enqueue(struct ring_buff *buf, void *elem, int cnt);

/**
 * @brief Enqueue elements from @a elem array of length @a cnt, possibly overwriting
 * oldest elements.
 *
 * @param buf
 * @param elem
 * @param cnt
 *
 * @return Count of enqueued elements
 */
extern int ring_buff_enqueueover(struct ring_buff *buf, void *elem, int cnt);

/**
 * @brief Dequeue @a cnt elements from @a buf to array @a elem
 *
 * @param buf
 * @param elem
 * @param cnt
 *
 * @return Number of dequeued elements
 */
extern int ring_buff_dequeue(struct ring_buff *buf, void *elem, int cnt);

/**
 * @brief Alloc space in @a buf for @a n elements.
 *
 * @param buf
 * @param n
 * @param ret
 *
 * @return Number of elements that @a *ret can hold
 */
extern int ring_buff_alloc(struct ring_buff *buf, int n, void **ret);

/**
 * @brief Alloc space in @a buf, possible overwriting old elements
 *
 * @param buf
 * @param n
 * @param ret
 *
 * @return Number of elements that @a *ret can hold
 */
extern int ring_buff_allocover(struct ring_buff *buf, int n, void **ret);

extern int ring_buff_init(struct ring_buff *buf, size_t elem_size, int count, void *storage);

extern int ring_buff_get_cnt(struct ring_buff *buf);

extern int ring_buff_get_space(struct ring_buff *buf);

#define RING_BUFFER_DEF(name, elem_type, req_len) \
	static elem_type name##_storage[req_len];     \
	static struct ring_buff name = {              \
		.elem_size = sizeof(elem_type),           \
		.capacity = req_len,                           \
		.cnt = 0,                                \
		.p_write = 0,                                 \
		.p_read = 0,                                 \
		.storage = (void *) name##_storage            \
	}

#endif /* UTIL_RING_BUFF_H_ */
