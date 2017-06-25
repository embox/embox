/**
 * @file
 * @brief Ring buffer interface
 *
 * @date 21.10.11
 * @author Anton Kozlov
 * @author Vita Loginova
 */

#ifndef UTIL_RING_BUFF_H_
#define UTIL_RING_BUFF_H_

#include <stddef.h>
#include <util/ring.h>

/**
 * Since ring from <util/ring.h> is used, one element is always reserved
 * to distinguish between empty and full state.
 */
struct ring_buff {
	struct ring ring;
	size_t capacity;   /**< capacity of buffer in elements */
	void  *storage;    /**< storage */
	size_t elem_size;  /**< size of element */
};

#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * @brief Enqueue elements from @a from_buf array of lenght @a cnt.
 * It cannot enqueue more than buffer can hold.
 *
 * @param buf
 * @param from_buf
 * @param cnt
 *
 * @return Count of enqueued elements
 */
extern int ring_buff_enqueue(struct ring_buff *buf, void *from_buf, int cnt);

/**
 * @brief Dequeue @a cnt elements from @a buf to array @a elem
 *
 * @param buf
 * @param into_buf
 * @param cnt
 *
 * @return Number of dequeued elements
 */
extern int ring_buff_dequeue(struct ring_buff *buf, void *into_buf, int cnt);

/**
 * @brief Allocate space for @a cnt or less elements without wrapping.
 *
 * @param buf
 * @param cnt
 * @param ret Pointer at the first element in the array
 *
 * @return Number of allocated elements without wrapping
 */
extern int ring_buff_alloc(struct ring_buff *buf, int cnt, void **ret);

extern int ring_buff_init(struct ring_buff *buf, size_t elem_size,
	int count, void *storage);

extern int ring_buff_get_cnt(struct ring_buff *buf);

extern int ring_buff_get_space(struct ring_buff *buf);

__END_DECLS

#define RING_BUFFER_DEF(name, elem_type, req_len) \
	static elem_type name##_storage[req_len];     \
	static struct ring_buff name = {              \
		.ring = {                                 \
			.head = 0,                            \
			.tail = 0                             \
		},                                        \
		.elem_size = sizeof(elem_type),           \
		.capacity = req_len,                      \
		.storage = (void *) name##_storage        \
	}

#endif /* UTIL_RING_BUFF_H_ */
