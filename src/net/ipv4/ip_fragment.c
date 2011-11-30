/**
 * @brief IP Fragmentation
 * @date 25.11.2011
 * @author Alexander Kalmuk
 */

#include <net/ip_fragment.h>
#include <net/skbuff.h>
#include <mem/objalloc.h>
#include <string.h>
#include <kernel/timer.h>
#include <net/udp.h>

/**
 * Datagram receive buffer
 */
struct dgram_buf {
	struct sk_buff *next_skbuff;
	struct sk_buff *prev_skbuff;
	struct list_head next_buf; /* linked list pointers	*/
	in_addr_t         saddr;
	in_addr_t         daddr;
	uint16_t          id;
	uint8_t           protocol;
	int               meat;
	int               len; /* total length of original datagram	*/
	struct sys_timer  *timer;
};

static LIST_HEAD(__dgram_buf_list);
OBJALLOC_DEF(__dgram_bufs, struct dgram_buf, MAX_BUFS_CNT);

#define dgram_buf_foreach(dgram_buf) \
	list_for_each_entry(dgram_buf, &__dgram_buf_list, next_buf)

#define skbuff_for_each(skb, buf) \
	for(skb = buf->next_skbuff;   \
	   ((struct list_head*)skb)!=(struct list_head*)buf; \
	   skb = skb->next)

static struct dgram_buf *buf_create(struct iphdr *iph);
static void buf_delete(struct dgram_buf *buf);
static void ip_frag_dgram_buf(struct dgram_buf *buf, struct sk_buff *skb);
static struct sk_buff *build_packet(struct dgram_buf *buf);

static inline struct dgram_buf *ip_find(struct iphdr *iph) {
	struct dgram_buf *buf;

	dgram_buf_foreach(buf) {
		if (buf->daddr == iph->saddr
			&& buf->saddr == iph->daddr
			&& buf->protocol == iph->proto
			&& buf->id == iph->id) {
			return buf;
		}
	}

	return NULL;
}

static void ip_frag_dgram_buf(struct dgram_buf *buf, struct sk_buff *skb) {
	struct sk_buff *tmp;
	int was_added = 0;
	int offset, tmp_offset;
	int data_len, end;

	offset = ntohs(skb->nh.iph->frag_off);
	offset &= IP_OFFSET;
	offset <<= 3;
	data_len = skb->len - (skb->h.raw - skb->data) - UDP_HEADER_SIZE;
	end = offset + data_len;

	skbuff_for_each(tmp, buf) {
		tmp_offset = ntohs(tmp->nh.iph->frag_off);
		tmp_offset &= IP_OFFSET;
		tmp_offset <<= 3;
		if (offset < tmp_offset) {
			list_add((struct list_head *) skb, (struct list_head *) tmp->prev);
			was_added = 1;
			break;
		}
	}

	if (!was_added) {
		list_add_tail((struct list_head *) skb, (struct list_head *) buf);
	}

	buf->meat += data_len;
	if (end > buf->len) {
		buf->len = end;
	}
}

//TODO there are two all data coping now. May be it is possible to do only one.
static struct sk_buff *build_packet(struct dgram_buf *buf) {
	struct sk_buff *skb;
	struct sk_buff *tmp;
	int offset = 0;
	int ihlen;

	tmp = buf->next_skbuff;
	//TODO
	ihlen = (tmp->h.raw - tmp->data) + UDP_HEADER_SIZE;
	skb = alloc_skb(buf->len + ihlen, 0);
	memcpy(skb->data, tmp->data, ihlen);

	skb->h.raw = skb->data + (tmp->h.raw - tmp->data);
	skb->nh.raw = skb->data + (tmp->nh.raw - tmp->data);
	skb->mac.raw = skb->data + (tmp->mac.raw - tmp->data);

	skb->protocol = tmp->protocol;
	skb->dev = tmp->dev;
	skb->len = buf->len + ihlen;

	while(!list_empty((struct list_head *)buf)) {
		memcpy(skb->data + ihlen + offset, tmp->data + ihlen, tmp->len - ihlen);
		offset += tmp->len - ihlen;
		list_del((struct list_head *)tmp);
		kfree_skb(tmp);
		tmp = buf->next_skbuff;
	}

	buf_delete(buf);

	return skb;
}

static struct dgram_buf *buf_create(struct iphdr *iph) {
	struct dgram_buf *buf;
	//sys_timer_t *timer;

	buf = (struct dgram_buf*) objalloc(&__dgram_bufs);
	if (!buf)
		return NULL;

	INIT_LIST_HEAD((struct list_head *)buf);
	INIT_LIST_HEAD(&buf->next_buf);
	list_add_tail(&buf->next_buf, &__dgram_buf_list);
	buf->protocol = iph->proto;
	buf->id = iph->id;
	buf->saddr = iph->saddr;
	buf->daddr = iph->daddr;
	buf->len = 0;
	buf->meat = 0;

	return buf;
}

static void buf_delete(struct dgram_buf *buf) {
	list_del(&buf->next_buf);
	pool_free(&__dgram_bufs, (void*)buf);
}

struct sk_buff *ip_defrag(struct sk_buff *skb) {
	struct dgram_buf *buf;
	int mf_flag;

	if ((buf = ip_find(skb->nh.iph)) == NULL) {
		buf = buf_create(skb->nh.iph);
	}

	ip_frag_dgram_buf(buf, skb);

	mf_flag = skb->nh.iph->frag_off & IP_MF;
	if (!mf_flag && buf->meat == buf->len) {
		return build_packet(buf);
	}

	return NULL;
}
