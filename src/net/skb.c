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

#include <util/math.h>

#include <net/skbuff.h>
#include <hal/ipl.h>
#include <mem/misc/pool.h>
#include <mem/sysmalloc.h>

#include <linux/compiler.h>
#include <framework/mod/options.h>
#include <linux/list.h>
#include <util/member.h>
#include <util/binalign.h>

#include <sys/uio.h>

#define MODOPS_AMOUNT_SKB       OPTION_GET(NUMBER, amount_skb)
#define MODOPS_AMOUNT_SKB_DATA  OPTION_GET(NUMBER, amount_skb_data)
#define MODOPS_DATA_SIZE        OPTION_GET(NUMBER, data_size)
#define MODOPS_DATA_ALIGN       OPTION_GET(NUMBER, data_align)
#define MODOPS_DATA_PADTO       OPTION_GET(NUMBER, data_padto)
#define MODOPS_IP_ALIGN         OPTION_GET(BOOLEAN, ip_align)
#define MODOPS_AMOUNT_SKB_EXTRA OPTION_GET(NUMBER, amount_skb_extra)
#define MODOPS_EXTRA_SIZE       OPTION_GET(NUMBER, extra_size)
#define MODOPS_EXTRA_ALIGN      OPTION_GET(NUMBER, extra_align)
#define MODOPS_EXTRA_PADTO      OPTION_GET(NUMBER, extra_padto)

#define DATA_PAD_SIZE \
	PAD_SIZE(IP_ALIGN_SIZE + MODOPS_DATA_SIZE, MODOPS_DATA_PADTO)
#define DATA_ATTR \
	__attribute__((aligned(MODOPS_DATA_ALIGN)))
#define IP_ALIGN_SIZE \
	(MODOPS_IP_ALIGN ? 2 : 0)

#define EXTRA_PAD_SIZE \
	PAD_SIZE(MODOPS_EXTRA_SIZE, MODOPS_EXTRA_PADTO)
#define EXTRA_ATTR \
	__attribute__((aligned(MODOPS_EXTRA_ALIGN)))

#define PAD_SIZE(obj_size, padto) \
	(((padto) - (obj_size) % (padto)) % (padto))

#define SKB_DATA_SIZE(size) \
	IP_ALIGN_SIZE + MODOPS_DATA_SIZE + (size) + sizeof(size_t)

#define SKB_DEBUG 0
#if SKB_DEBUG
#include <kernel/printk.h>
#define DBG(x) x
#else
#define DBG(x)
#endif

struct sk_buff_data_fixed {
	size_t links;

	char __ip_align[IP_ALIGN_SIZE];
	unsigned char data[MODOPS_DATA_SIZE];
	char __data_pad[DATA_PAD_SIZE];
};

struct sk_buff_data {
	size_t links;

	char __data[];
} DATA_ATTR;

struct sk_buff_extra {
	unsigned char extra[MODOPS_EXTRA_SIZE];
	char __extra_pad[EXTRA_PAD_SIZE];
} EXTRA_ATTR;

POOL_DEF(skb_pool, struct sk_buff, MODOPS_AMOUNT_SKB);
POOL_DEF(skb_data_pool, struct sk_buff_data_fixed, MODOPS_AMOUNT_SKB_DATA);
POOL_DEF(skb_extra_pool, struct sk_buff_extra, MODOPS_AMOUNT_SKB_EXTRA);

static void *skb_get_data_pointner(struct sk_buff_data *skb_data) {
	return skb_data->__data + IP_ALIGN_SIZE;
}

static struct sk_buff_data * skb_data_alloc_dynamic(size_t size) {
	ipl_t sp;
	struct sk_buff_data *skb_data;

	sp = ipl_save();
	{
		skb_data = (struct sk_buff_data *) sysmalloc(SKB_DATA_SIZE(size));
	}
	ipl_restore(sp);

	if (skb_data == NULL) {
		DBG(printk("skb_data_alloc: error: no memory\n"));
		return NULL; /* error: no memory */
	}

	skb_data->links = 1;

	return skb_data;
}

size_t skb_max_size(void) {
	return MODOPS_DATA_SIZE;
}

size_t skb_max_extra_size(void) {
	return member_sizeof(struct sk_buff_extra, extra);
}

void * skb_data_cast_in(struct sk_buff_data *skb_data) {
	assert(skb_data != NULL);
	return skb_get_data_pointner(skb_data);
}

struct sk_buff_data * skb_data_cast_out(void *data) {
	assert(data != NULL);
	return member_cast_out(data - IP_ALIGN_SIZE, struct sk_buff_data, __data);
}

void * skb_extra_cast_in(struct sk_buff_extra *skb_extra) {
	assert(skb_extra != NULL);
	return &skb_extra->extra[0];
}

struct sk_buff_extra * skb_extra_cast_out(void *extra) {
	assert(extra != NULL);
	return member_cast_out(extra, struct sk_buff_extra, extra);
}

struct sk_buff_data * skb_data_alloc(void) {
	ipl_t sp;
	struct sk_buff_data *skb_data;

	sp = ipl_save();
	{
		skb_data = pool_alloc(&skb_data_pool);
	}
	ipl_restore(sp);

	if (skb_data == NULL) {
		DBG(printk("skb_data_alloc: error: no memory\n"));
		return NULL; /* error: no memory */
	}

	skb_data->links = 1;

	return skb_data;
}

struct sk_buff_data * skb_data_clone(struct sk_buff_data *skb_data) {
	ipl_t sp;

	assert(skb_data != NULL);

	sp = ipl_save();
	{
		++skb_data->links;
	}
	ipl_restore(sp);

	return skb_data;
}

int skb_data_cloned(const struct sk_buff_data *skb_data) {
	return skb_data->links != 1;
}

void skb_data_free(struct sk_buff_data *skb_data) {
	ipl_t sp;

	assert(skb_data != NULL);

	sp = ipl_save();
	{
		if (--skb_data->links == 0) {
			if (pool_belong(&skb_data_pool, skb_data)) {
				pool_free(&skb_data_pool, skb_data);
			}
			else {
				sysfree(skb_data);
			}
		}
	}
	ipl_restore(sp);
}

struct sk_buff_extra * skb_extra_alloc(void) {
	ipl_t sp;
	struct sk_buff_extra *skb_extra;

	sp = ipl_save();
	{
		skb_extra = pool_alloc(&skb_extra_pool);
	}
	ipl_restore(sp);

	if (skb_extra == NULL) {
		DBG(printk("skb_extra_alloc: error: no memory\n"));
		return NULL; /* error: no memory */
	}

	return skb_extra;
}

void skb_extra_free(struct sk_buff_extra *skb_extra) {
	ipl_t sp;

	sp = ipl_save();
	{
		pool_free(&skb_extra_pool, skb_extra);
	}
	ipl_restore(sp);
}

struct sk_buff * skb_wrap(size_t size, struct sk_buff_data *skb_data) {
	ipl_t sp;
	struct sk_buff *skb;

	assert(size != 0);
	assert(skb_data != NULL);

	// TODO move it
//	if (size > skb_max_size()) {
//		DBG(printk("skb_wrap: error: size is too big\n"));
//		return NULL; /* error: invalid argument */
//	}

	sp = ipl_save();
	{
		skb = pool_alloc(&skb_pool);
	}
	ipl_restore(sp);

	gettimeofday(&skb->tstamp, NULL);

	if (skb == NULL) {
		DBG(printk("skb_wrap: error: no memory\n"));
		return NULL; /* error: no memory */
	}

	INIT_LIST_HEAD((struct list_head * )skb);
	skb->dev = NULL;
	skb->len = size;
	skb->nh.raw = skb->h.raw = NULL;
	skb->data = skb_data;
	skb->mac.raw = skb_get_data_pointner(skb_data);
	skb->p_data = skb->p_data_end = NULL;

	return skb;
}

struct sk_buff * skb_alloc(size_t size) {
	struct sk_buff *skb;
	struct sk_buff_data *skb_data;

	if (skb_data_pool.obj_size >= size) {
		skb_data = skb_data_alloc();
	} else {
		skb_data = skb_data_alloc_dynamic(size);
	}
	if (skb_data == NULL) {
		return NULL; /* error: no memory */
	}

	skb = skb_wrap(size, skb_data);
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
		pool_free(&skb_pool, skb);
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

	decloned_data = skb_data_alloc();
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
