/**
 * @file
 * @brief IP Fragmentation
 *
 * @date 25.11.2011
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

#include <net/l3/ipv4/ip_fragment.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/l3/icmpv4.h>
#include <net/l3/ipv4/ip.h>

#include <mem/objalloc.h>
#include <kernel/time/timer.h>
#include <kernel/time/time.h>

#include <util/math.h>
#include <lib/libds/dlist.h>
#include <util/log.h>

#include <framework/mod/options.h>

#define MAX_BUFS_CNT       OPTION_GET(NUMBER, max_uncomplete_cnt)
#define IP_FRAGMENTED_SUPP OPTION_GET(NUMBER, ip_fragmented_support)

/**
 * Datagram receive buffer
 */
struct dgram_buf {
	struct sk_buff_head fragments;
	struct dlist_head   next_buf; /* linked list pointers */
	struct buf_id {
		in_addr_t         saddr;
		in_addr_t         daddr;
		uint16_t          id;
		uint8_t           protocol;
	} buf_id;
	int               is_last_frag_received;
	int               meat;
	int               len; /* total length of original datagram */
	int               buf_ttl;
	int               is_deleted;
};

static DLIST_DEFINE(__dgram_buf_list);
static struct sys_timer ip_frag_timer;

OBJALLOC_DEF(__dgram_bufs, struct dgram_buf, MAX_BUFS_CNT);

#define df_flag(skb) (ntohs(skb->nh.iph->frag_off) & IP_DF)

#define TIMER_TICK 1000

static struct dgram_buf *ip_buf_create(struct iphdr *iph);
static void buf_delete(struct dgram_buf *buf);
static void ip_buf_add_skb(struct dgram_buf *buf, struct sk_buff *skb);
static struct sk_buff *build_packet(struct dgram_buf *buf);

static inline void buf_set_deleted(struct dgram_buf *buf) {
	buf->is_deleted = 1;
	skb_queue_purge(&buf->fragments);
}

static inline int ip_offset(struct sk_buff *skb) {
	int offset;

	offset = ntohs(skb->nh.iph->frag_off);
	offset &= IP_OFFSET;
	offset <<= 3;

	return offset;
}

static void ttl_handler(struct sys_timer *timer, void *param) {
	struct dgram_buf *buf = NULL;
	int i;

	i = 0;

	dlist_foreach_entry(buf, &__dgram_buf_list, next_buf) {
		i ++;
		if (buf->is_deleted) {
			continue;
		}

		if (buf->buf_ttl == 0) {
			/*icmp_send(buf->next_skbuff, ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, 0);*/
			buf_set_deleted(buf);
		} else {
			buf->buf_ttl -= 1;
		}
	}

	if (i == 0) {
		timer_stop(timer);
	}
}

static inline struct dgram_buf *ip_find(struct iphdr *iph) {
	struct dgram_buf *buf = NULL;

	assert(iph);

	dlist_foreach_entry(buf, &__dgram_buf_list, next_buf) {
		if (buf->is_deleted) {
			buf_delete(buf);
			continue;
		}
		if (buf->buf_id.daddr == iph->daddr
			&& buf->buf_id.saddr == iph->saddr
			&& buf->buf_id.protocol == iph->proto
			&& buf->buf_id.id == iph->id) {
			return buf;
		}
	}

	return NULL;
}

static void ip_buf_add_skb(struct dgram_buf *buf, struct sk_buff *skb) {
	int offset, data_len, end;

	assert(buf && skb);

	/* We use ttl >> 4 just to make sure time to wait is not very big.
	 * Usually, ttl is 64, so ttl >> 4 = 4 seconds */
	buf->buf_ttl = max(buf->buf_ttl, skb->nh.iph->ttl >> 4);

	offset = ip_offset(skb);

	data_len = skb->len - (skb->h.raw - skb->mac.raw);
	end = offset + data_len;

	skb_queue_push(&buf->fragments, skb);

	buf->meat += data_len;
	if (end > buf->len) {
		buf->len = end;
	}
}

static struct sk_buff *build_packet(struct dgram_buf *buf) {
	struct sk_buff *skb, *skb_iter;
	int offset = 0;
	int ihlen;

	assert(buf);

	skb_iter = skb_queue_front(&buf->fragments);
	assert(skb_iter);

	ihlen = (skb_iter->h.raw - skb_iter->mac.raw);
	skb = skb_alloc(buf->len + ihlen);
	/* Strange:
	 *	- it might return NULL, because length is too big now.
	 *	- ihlen has upper limit. So it's more wise to has such
	 *	amount of extra space in the pool (NOT shared with ICMP)
	 */
	assert(skb);

	/* copy and concatenate dat. Queue is NOT sorted by offset! */
	while((skb_iter = skb_queue_pop(&buf->fragments))) {
		offset = ip_offset(skb_iter);
		if (offset == 0) {
			memcpy(skb->mac.raw, skb_iter->mac.raw, skb_iter->len);
			/* Terrible. Some pointers might be NULL here. sk pointer is omitted */
			skb->nh.raw = skb->mac.raw + (skb_iter->nh.raw - skb_iter->mac.raw);
			skb->h.raw = skb->nh.raw + IP_HEADER_SIZE(ip_hdr(skb_iter));
			skb->nh.iph->tot_len = htons(buf->len + IP_HEADER_SIZE(ip_hdr(skb_iter)));
			skb->dev = skb_iter->dev;
		}
		memcpy(skb->mac.raw + ihlen + offset, skb_iter->mac.raw + ihlen,
				skb_iter->len - ihlen);
		skb_free(skb_iter);
	}

	/* recalculate length */
	skb->len = buf->len + ihlen;
	buf_set_deleted(buf);

	return skb;
}

static struct dgram_buf *ip_buf_create(struct iphdr *iph) {
	struct dgram_buf *buf;

	assert(iph);

	buf = (struct dgram_buf*) objalloc(&__dgram_bufs);
	if (!buf) {
		return NULL;
	}

	if (!timer_is_inited(&ip_frag_timer)) {
		timer_init(&ip_frag_timer, TIMER_PERIODIC, ttl_handler, NULL);
		log_debug("timer init");
	}
	timer_start(&ip_frag_timer, ms2jiffies(TIMER_TICK));

	skb_queue_init(&buf->fragments);
	dlist_head_init(&buf->next_buf);
	dlist_add_prev(&buf->next_buf, &__dgram_buf_list);

	buf->buf_id.protocol = iph->proto;
	buf->buf_id.id = iph->id;
	buf->buf_id.saddr = iph->saddr;
	buf->buf_id.daddr = iph->daddr;
	buf->len = 0;
	buf->is_last_frag_received = 0;
	buf->meat = 0;
	buf->buf_ttl = MSL;
	buf->is_deleted = 0;

	return buf;
}

static void buf_delete(struct dgram_buf *buf) {
	dlist_del(&buf->next_buf);
	objfree(&__dgram_bufs, (void*)buf);
}

static struct sk_buff *ip_frag_build(const struct sk_buff *big_skb, int frag_offset,
		int frag_size, int mf_flag) {
	struct sk_buff * frag;
	int len = big_skb->dev->hdr_len + IP_HEADER_SIZE(big_skb->nh.iph);

	if (unlikely(!(frag = skb_alloc(frag_size)))) {
		return NULL;
	}

	/* Copy IP and MAC headers */
	memcpy(frag->mac.raw, big_skb->mac.raw, len);
	/* Copy IP content */
	memcpy(frag->mac.raw + len, big_skb->mac.raw + frag_offset, frag_size - len);
	frag->nh.raw = frag->mac.raw + big_skb->dev->hdr_len;
	frag->nh.iph->frag_off = htons(
				(((frag_offset - len) >> 3) /* data offset / 8 */) | mf_flag);
	frag->nh.iph->tot_len = htons(frag_size - big_skb->dev->hdr_len);

	return frag;
}

struct sk_buff *ip_defrag(struct sk_buff *skb) {
	struct dgram_buf *buf;
	int mf_flag;

	assert(skb);

	/* if it is not complete packet */
	if (!(IP_FRAGMENTED_SUPP) || df_flag(skb)) {
		/* For some reason we don't like situation when someone used forced fragmentation */
		skb_free(skb);
		skb = (sk_buff_t *)NULL;
		return skb;
	}

	buf = ip_find(skb->nh.iph);
	if (!buf) {
		buf = ip_buf_create(skb->nh.iph);
	}
	assert(buf);

	ip_buf_add_skb(buf, skb);

	mf_flag = ntohs(skb->nh.iph->frag_off) & IP_MF;
	buf->is_last_frag_received = !mf_flag;

	if (buf->is_last_frag_received && buf->meat == buf->len) {
		return build_packet(buf);
	}

	return NULL;
}

int ip_frag(const struct sk_buff *skb, uint32_t mtu,
		struct sk_buff_head *tx_buf) {
	struct sk_buff *fragment;
	int len;
	int offset; /* offset from skb start (== mac.raw) */
	int align_MTU;

	assert(skb != NULL);
	assert(skb->dev != NULL);

	skb_queue_init(tx_buf);
	if (!IP_FRAGMENTED_SUPP) {
		skb_queue_push(tx_buf, (struct sk_buff *) skb);
		return 0;
	}

	offset = len = skb->dev->hdr_len + IP_HEADER_SIZE(skb->nh.iph);

	/* Note: correct MTU, because fragment offset must divide on 8*/
	align_MTU = mtu - (mtu - len) % 8;

	/* copy sk_buff without last fragment. All this fragments have size MTU */
	while (offset < skb->len - align_MTU + len) {
		fragment = ip_frag_build(skb, offset, align_MTU, IP_MF);
		if (!fragment) {
			skb_queue_purge(tx_buf);
			return -ENOMEM;
		}
		skb_queue_push(tx_buf, fragment);
		offset += (align_MTU - len);
	}

	/* copy last fragment */
	if (offset < skb->len) {
		fragment = ip_frag_build(skb, offset, skb->len - offset + len, 0);
		if (!fragment) {
			skb_queue_purge(tx_buf);
			return -ENOMEM;
		}
		skb_queue_push(tx_buf, fragment);
	}

	return 0;
}
