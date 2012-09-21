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

extern size_t ring_buff_enqueue(struct ring_buff *buf, void *elem, size_t cnt);

extern size_t ring_buff_dequeue(struct ring_buff *buf, void *elem, size_t cnt);

extern int ring_buff_init(struct ring_buff *buf, size_t elem_size, size_t count, void *storage);

extern size_t ring_buff_get_cnt(struct ring_buff *buf);

extern size_t ring_buff_get_space(struct ring_buff *buf);

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
