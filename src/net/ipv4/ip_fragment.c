/**
 * @date 25.11.2011
 */

#include <net/ip_fragment.h>
#include <net/skbuff.h>
#include <mem/objalloc.h>

OBJALLOC_DEF(__dgram_bufs, struct dgram_buf, MAX_BUFS_CNT);
//OBJALLOC_DEF(__holes, struct hole, MAX_HOLES_CNT);

struct slist __dgram_buf_list = SLIST_INIT(&__dgram_buf_list);

struct frag {
	int		begin;
	int		end;
};

static inline dgram_buf_t* ip_find(struct iphdr *iph) {
	struct dgram_buf *buf;

	dgram_buf_foreach_ptr(buf) {
		if(buf->daddr == iph->saddr &&
			buf->saddr == iph->daddr &&
			buf->protocol == iph->proto &&
			buf->id == iph->id) {
			return buf;
		}
	}

	return NULL;
}

static void put_frag_in_hole (struct hole *hole, struct frag *fragment) {

}

static void ip_frag_dgram_buf (struct dgram_buf *buf, struct sk_buff *skb) {
	struct frag fragment;
	struct iphdr *iph;
	struct hole *hole;
	struct list_link *hole_link;

	iph = skb->nh.iph;
	fragment.begin = iph->frag_off * 8;
	fragment.end = fragment.begin + (iph->ttl - (iph->ihl) * 4);

	list_for_each(hole_link, &buf->holes){
		hole = list_entry (hole_link, struct hole, next_hole);
		if ((hole->begin <= fragment.begin) && (hole->end >= fragment.end)) {
			put_frag_in_hole(hole, &fragment);
		}
	}

	put_frag_in_hole(hole, &fragment);
}

struct sk_buff *ip_defrag(struct sk_buff *skb) {
	struct dgram_buf *buf;

	if ((buf = ip_find(skb->nh.iph)) != NULL) {
		ip_frag_dgram_buf(buf, skb);
	} else {
		buf = (dgram_buf_t *) objalloc(&__dgram_bufs);
		list_link_init(&buf->holes);
		slist_add_first(buf, &__dgram_buf_list, next_buf);
	}

	return skb;
}

