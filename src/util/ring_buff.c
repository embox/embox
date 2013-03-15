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

int ring_buff_get_cnt(struct ring_buff *buf) {
	return buf->cnt;
}

int ring_buff_get_space(struct ring_buff *buf) {
	return buf->capacity - buf->cnt;
}

int ring_buff_allocover(struct ring_buff *buf, int n, void **ret) {
	int m = buf->capacity - buf->p_write;

	if (n < m) {
		m = n;
	}

	*ret = buf->storage + (buf->p_write * buf->elem_size);

	buf->p_write = (buf->p_write + m) % buf->capacity;
	buf->cnt += m;

	if (buf->cnt > buf->capacity) {
		buf->p_read = (buf->p_read + (buf->cnt - buf->capacity))
			% buf->capacity;
		buf->cnt = buf->capacity;
	}

	return m;
}

int ring_buff_alloc(struct ring_buff *buf, int n, void **ret) {
	int m = buf->capacity - buf->cnt;

	return ring_buff_allocover(buf, n < m ? n : m, ret);
}

static int enqueue_fn(int (*alloc)(struct ring_buff *, int n, void **ret),
		struct ring_buff *buf, void *elem, int cnt ) {
	void *write_to;
	int tcnt = cnt, t1, t2;

	if (0 >= (t1 = alloc(buf, tcnt, &write_to))) {
		return t1;
	}

	memcpy(write_to, elem, t1 * buf->elem_size);

	tcnt -= t1;

	if (tcnt == 0) {
		return cnt;
	}

	if (0 >= (t2 = alloc(buf, tcnt, &write_to))) {
		return t2;
	}

	memcpy(&write_to, elem + t1 * buf->elem_size, t2 * buf->elem_size);

	tcnt -= t2;

	return cnt - tcnt;
}

int ring_buff_enqueueover(struct ring_buff *buf, void *elem, int cnt) {
	return enqueue_fn(ring_buff_allocover, buf, elem, cnt);
}

int ring_buff_enqueue(struct ring_buff *buf, void *elem, int cnt) {
	return enqueue_fn(ring_buff_alloc, buf, elem, cnt);
}


int ring_buff_dequeue(struct ring_buff *buf, void *elem, int cnt) {
	int rest = buf->capacity - buf->p_read;
	void *read_from = (char*)buf->storage + (buf->p_read * buf->elem_size);

	if (buf->cnt == 0) {
		return 0;
	}

	if (buf->cnt < cnt) {
		cnt = buf->cnt;
	}

	buf->cnt -= cnt;

	if (cnt > rest) {
		memcpy(elem, read_from, buf->elem_size * rest);
		memcpy((char *)elem + buf->elem_size * rest, buf->storage, buf->elem_size * (cnt - rest));
		buf->p_read = cnt - rest;
	} else {
		memcpy(elem, read_from, buf->elem_size * cnt);
		buf->p_read = (buf->p_read + cnt) % buf->capacity;
	}

	return cnt;
}

int ring_buff_init(struct ring_buff *buf, size_t elem_size, int count, void *storage) {
	buf->capacity = count;
	buf->cnt = 0;
	buf->p_write = 0;
	buf->p_read = 0;
	buf->storage = storage;
	buf->elem_size = elem_size;

	return 0;
}
