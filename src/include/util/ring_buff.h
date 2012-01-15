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
	int capacity;      /**< capacity of buffer in elements */
	int cnt;           /**< element counter in buffer*/
	int p_write;       /**< pointer to next write element*/
	int p_read;        /**< pointer to next read element*/
	void *storage;     /**< storage */
	int elem_size;     /**< size of element */
};

extern int ring_buff_enque(struct ring_buff *buf, void *elem);

extern int ring_buff_deque(struct ring_buff *buf, void *elem);

extern int ring_buff_init(struct ring_buff *buf, size_t elem_size, int count, void *storage);

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
