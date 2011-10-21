/**
 * @file
 * @brief Circular buffer interface
 *
 * @date 21.10.2011
 * @author Anton Kozlov
 */

#ifndef UTIL_CIRCULAR_BUFFER_H_
#define UTIL_CIRCULAR_BUFFER_H_

#include <stddef.h>

struct c_buf {
	int len;
	int size;
	int beg;
	int end;
	void *buf;
};

extern int __c_buf_add(struct c_buf *buf, void *elem, size_t elem_size);

extern int __c_buf_get(struct c_buf *buf, void *elem, size_t elem_size);

extern int c_buf_init(struct c_buf *buf, int count, void *storage);

#define CIRCULAR_BUFFER_DEF(name, elem_type, len) \
	__CIRCULAR_BUFFER_DEF(name, name##_storage, elem_type, len)

#define __CIRCULAR_BUFFER_DEF(name, storage_nm, elem_type, req_len) \
	static elem_type storage_nm[req_len];\
	static struct c_buf name = {\
		.len = req_len,\
		.size = 0,\
		.beg = 0,\
		.end = 0,\
		.buf = (void *) storage_nm\
	}

#define c_buf_add(name, elem) \
	__c_buf_add(name, (void *) &elem, sizeof(elem))

#define c_buf_get(name, elem) \
	__c_buf_get(name, (void *) &elem, sizeof(elem))

#endif

