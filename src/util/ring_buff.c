/**
 * @file
 * @brief Nonblock ring buffer realization
 *
 * @date 21.10.2011
 * @author Anton Kozlov
 */

#include <stddef.h>
#include <string.h>
#include <util/ring_buff.h>

int __c_buf_add(struct c_buf *buf, void *elem, size_t elem_size) {
	char *buf_pool = buf->buf;
	if (buf->size >= buf->len) {
		return -1;
	}
	buf->size++;
	memcpy(buf_pool + (buf->beg * elem_size), (const void *) elem,
			elem_size);
	buf->beg = (buf->beg + 1) % buf->len;
	return 0;
}

int __c_buf_get(struct c_buf *buf, void *elem, size_t elem_size) {
	char *buf_pool = buf->buf;
	void *ret = buf_pool + (buf->end * elem_size);
	if (buf->size == 0) {
		return -1;
	}
	buf->size--;
	buf->end = (buf->end + 1) % buf->len;
	memcpy(elem, ret, elem_size);
	return 0;
}

int c_buf_init(struct c_buf *buf, int count, void *storage) {
	buf->len  = count;
	buf->size = 0;
	buf->beg  = 0;
	buf->end  = 0;
	buf->buf  = storage;
	return 0;
}


