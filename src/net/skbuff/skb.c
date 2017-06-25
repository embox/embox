/**
* @file
* @details sk_buff management API implementation.
* @date 20.10.09
*
* @author Anton Bondarev
* @author Ilia Vaprol
* @author Vladimir Sokolov
*/

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <sys/uio.h>

#include <util/math.h>
#include <util/log.h>
#include <util/member.h>
#include <util/binalign.h>

#include <hal/ipl.h>

#include <mem/misc/pool.h>

#include <linux/list.h>

#include <net/skbuff.h>

#include <framework/mod/options.h>

#define MODOPS_AMOUNT_SKB       OPTION_GET(NUMBER, amount_skb)
POOL_DEF(skb_pool, struct sk_buff, MODOPS_AMOUNT_SKB);

struct sk_buff * skb_wrap(size_t size, struct sk_buff_data *skb_data) {
	return skb_wrap_local(size, skb_data, &skb_pool);
}

struct sk_buff * skb_wrap_local(size_t size, struct sk_buff_data *skb_data,
		struct pool *pl) {
	ipl_t sp;
	struct sk_buff *skb;

	assert(pl != NULL);
	assert(size != 0);
	assert(skb_data != NULL);

	// TODO move it
//	if (size > skb_max_size()) {
//		log_error("skb_wrap: error: size is too big\n");
//		return NULL; /* error: invalid argument */
//	}

	sp = ipl_save();
	{
		skb = pool_alloc(pl);
	}
	ipl_restore(sp);

	if (skb == NULL) {
		log_error("skb_wrap: error: no memory\n");
		return NULL; /* error: no memory */
	}

	gettimeofday(&skb->tstamp, NULL);

	INIT_LIST_HEAD((struct list_head * )skb);
	skb->dev = NULL;
	skb->len = size;
	skb->nh.raw = skb->h.raw = NULL;
	skb->data = skb_data;
	skb->mac.raw = skb_get_data_pointner(skb_data);
	skb->p_data = skb->p_data_end = NULL;
	skb->pl = pl;

	return skb;
}

struct sk_buff * skb_alloc(size_t size) {
	return skb_alloc_local(size, &skb_pool);
}

struct sk_buff * skb_alloc_local(size_t size, struct pool *pl) {
	struct sk_buff *skb;
	struct sk_buff_data *skb_data;

	skb_data = skb_data_alloc(size);

	if (skb_data == NULL) {
		return NULL; /* error: no memory */
	}

	skb = skb_wrap_local(size, skb_data, pl);
	if (skb == NULL) {
		skb_data_free(skb_data);
		return NULL; /* error: no memory */
	}
	return skb;
}

struct sk_buff * skb_realloc(size_t size, struct sk_buff *skb) {
	if (skb == NULL) {
		return skb_alloc(size);
	}

	list_del_init((struct list_head *) skb);
	skb->dev = NULL;
	skb->len = size;
	skb->mac.raw = skb_get_data_pointner(skb->data);
	skb->nh.raw = skb->h.raw = NULL;

	return skb;
}

void skb_free(struct sk_buff *skb) {
	ipl_t sp;

	if (!skb) {
		return;
	}

	skb_data_free(skb->data);

	sp = ipl_save();
	{
		assert((skb->lnk.prev != NULL) && (skb->lnk.next != NULL));
		list_del((struct list_head *) skb);
		pool_free(skb->pl, skb);
	}
	ipl_restore(sp);
}

static void skb_copy_ref(struct sk_buff *to, const struct sk_buff *from) {
	ptrdiff_t offset;

	assert((to != NULL) && (to->data != NULL) && (from != NULL)
			&& (from->data != NULL));

	to->dev = from->dev;
	offset = skb_get_data_pointner(to->data)
			- skb_get_data_pointner(from->data);
	if (from->mac.raw != NULL) {
		to->mac.raw = from->mac.raw + offset;
	}
	if (from->nh.raw != NULL) {
		to->nh.raw = from->nh.raw + offset;
	}
	if (from->h.raw != NULL) {
		to->h.raw = from->h.raw + offset;
	}
	to->p_data = to->p_data_end = NULL;
}

static void skb_shift_ref(struct sk_buff *skb, ptrdiff_t offset) {
	assert((skb != NULL) && (skb->data != NULL));

	if (skb->mac.raw != NULL) {
		skb->mac.raw += offset;
	}
	if (skb->nh.raw != NULL) {
		skb->nh.raw += offset;
	}
	if (skb->h.raw != NULL) {
		skb->h.raw += offset;
	}
	skb->p_data = skb->p_data_end = NULL;
}

static void skb_copy_data(struct sk_buff_data *to_data,
		const struct sk_buff *from) {
	assert((to_data != NULL) && (from != NULL) && (from->data != NULL));
	memcpy(skb_get_data_pointner(to_data), skb_get_data_pointner(from->data),
			from->len);
}

struct sk_buff * skb_copy(const struct sk_buff *skb) {
	struct sk_buff *copied;

	assert(skb != NULL);

	copied = skb_alloc(skb->len);
	if (copied == NULL) {
		return NULL; /* error: no memory */
	}

	skb_copy_ref(copied, skb);
	skb_copy_data(copied->data, skb);

	return copied;
}

struct sk_buff * skb_clone(const struct sk_buff *skb) {
	struct sk_buff *cloned;
	struct sk_buff_data *cloned_data;

	assert(skb != NULL);

	cloned_data = skb_data_clone(skb->data);
	if (cloned_data == NULL) {
		return NULL; /* error: no memory */
	}

	cloned = skb_wrap(skb->len, cloned_data);
	if (cloned == NULL) {
		skb_data_free(cloned_data);
		return NULL; /* error: no memory */
	}

	skb_copy_ref(cloned, skb);

	return cloned;
}

struct sk_buff * skb_declone(struct sk_buff *skb) {
	struct sk_buff_data *decloned_data;

	assert(skb != NULL);

	if (!skb_data_cloned(skb->data)) {
		return skb;
	}

	decloned_data = skb_data_alloc(skb->len);
	if (decloned_data == NULL) {
		return NULL; /* error: no memory */
	}

	skb_shift_ref(skb,
			skb_get_data_pointner(decloned_data)
					- skb_get_data_pointner(skb->data));
	skb_copy_data(decloned_data, skb);

	skb_data_free(skb->data);
	skb->data = decloned_data;

	return skb;
}

void skb_rshift(struct sk_buff *skb, size_t count) {
	assert(skb != NULL);
	assert(skb->data != NULL);
	assert(count < skb_max_size());
	memmove(skb_get_data_pointner(skb->data) + count,
			skb_get_data_pointner(skb->data),
			min(skb->len, skb_max_size() - count));
	skb->len += min(count, skb_max_size() - count);
}

size_t skb_read(struct sk_buff *skb, char *buff, size_t buff_sz) {
	size_t len;

	assert(skb);
	assert(skb->p_data_end >= skb->p_data);

	len = min(buff_sz, skb->p_data_end - skb->p_data);

	memcpy(buff, skb->p_data, len);

	skb->p_data += len;

	return len;
}

int skb_iovec_buf(const struct iovec *iov, int iovlen, const void *buf, int buflen) {
	const void *const buf_e = buf + buflen;
	const void *buf_p = buf;
	int i_io = 0;

	while (buf_p < buf_e && i_io < iovlen) {
		const int to_copy = min(iov[i_io].iov_len, buf_e - buf_p);

		memcpy(iov[i_io].iov_base, buf_p, to_copy);
		buf_p += to_copy;
		++i_io;
	}

	return buf_p - buf;
}

int skb_buf_iovec(void *buf, int buflen, struct iovec *iov, int iovlen) {
	void *const buf_e = buf + buflen;
	void *buf_p = buf;
	int i_io = 0;

	while (buf_p < buf_e && i_io < iovlen) {
		const int to_copy = min(iov[i_io].iov_len, buf_e - buf_p);

		memcpy(buf_p, iov[i_io].iov_base, to_copy);
		buf_p += to_copy;
		++i_io;
	}

	return buf_p - buf;
}
