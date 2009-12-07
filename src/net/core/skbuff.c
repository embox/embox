/**
 * @file skbuff.c
 *
 * @details this module realize interface with structure sk_buff
 * structures sk_buff are presented queue free packet.
 * @date 20.10.2009
 * @author Anton Bondarev
 */
#include <types.h>
#include <string.h> /*for memcpy*/
#include <common.h>
#include <kernel/module.h>
#include <lib/list.h>
#include <net/skbuff.h>
#include <asm/spin_lock.h>
#include <net/net_pack_manager.h>

//TODO this define must place in autoconf
#define QUANTITY_SKB 0x100

//TODO this define must place in autoconf
#define QUANTITY_SKB_QUEUE 0x100

static struct sk_buff sk_buff_pool[QUANTITY_SKB];
static LIST_HEAD(head_free_skb);

static struct sk_buff_head sk_queue_pool[QUANTITY_SKB_QUEUE];
static LIST_HEAD(head_free_queue);

struct sk_buff_head *alloc_skb_queue(){
	struct sk_buff_head *queue;
	unsigned long sp = spin_lock();
	if(list_empty(&head_free_queue)){
		spin_unlock(sp);
		return NULL;
	}
	/* in free queue we held structure sk_buff_head but this pointer has sk_buff * type
	 * we must assignment it
	 */
	queue = (struct sk_buff_head *)(&head_free_queue)->next;
	list_del((&head_free_queue)->next);
	spin_unlock(sp);
	return queue;
}

/**
 * function must called if we want use this functionality.
 * It init queue free packet
 */
static void __init init_skb(){
	int i;
	for (i = 1; i < array_len(sk_buff_pool); i ++){
		kfree_skb(&sk_buff_pool[i]);
	}
	for (i = 1; i < array_len(sk_queue_pool); i ++){
		list_add((struct list_head *)&sk_queue_pool[i], &head_free_queue);
	}
}

struct sk_buff *alloc_skb(unsigned int size, gfp_t priority){
	struct sk_buff *skb;
//TODO only one packet now

	unsigned long sp = spin_lock();
	if(list_empty(&head_free_skb)){
		spin_unlock(sp);
		return NULL;
	}
	skb = (struct sk_buff *)(&head_free_skb)->next;
	list_del((&head_free_skb)->next);
	spin_unlock(sp);

	if (NULL == (skb->data = net_buff_alloc())) {
		kfree_skb(skb);
		return NULL;
	}
	skb ->len = size;
	return skb;
}

void kfree_skb(struct sk_buff *skb){
	if (NULL == skb){
		return;
	}
	net_buff_free(skb->data);
	unsigned long sp = spin_lock();
	list_move_tail((struct list_head *)skb, (struct list_head *)&head_free_skb);
	spin_unlock(sp);
}

void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk){
	if (NULL == list || NULL == newsk)
		return;

	unsigned long sp = spin_lock();
	list_move_tail((struct list_head *)newsk, (struct list_head *)list);
	spin_unlock(sp);

}

void skb_queue_purge(struct sk_buff_head *queue){
	if (NULL == queue){
		return;
	}
	struct list_head *skb;
	unsigned long sp = spin_lock();
	/*free all sk_buff*/
	list_for_each(skb, (struct list_head *)queue){
		net_buff_free(((struct sk_buff *)skb)->data);
		list_move_tail(skb, (struct list_head *)queue);
	}
	/*free queue*/
	list_add((struct list_head *)queue, &head_free_queue);
	spin_unlock(sp);
}

/**
 * split buffer to skb queue
 *
 */
//TODO buff_to_skb not realize now
struct sk_buff *buff_to_skb(unsigned char *buff){
	return NULL;
}

struct sk_buff *alloc_skb_fclone(struct sk_buff *skb, gfp_t priority) {

}

struct sk_buff *skb_copy(const struct sk_buff *skb, gfp_t priority){
	struct sk_buff *new_pack;
	if ((NULL == skb) || (NULL == (new_pack = alloc_skb(skb->len, priority)))){
		return NULL;
	}
	memcpy (skb->data, new_pack->data, skb->len);

	/*fix references during copy net_pack*/
	if (NULL != skb->h.raw) {
		new_pack->h.raw = new_pack->data + (skb->h.raw - skb->data);
	}
	if (NULL != skb->nh.raw) {
		new_pack->nh.raw = new_pack->data + (skb->nh.raw - skb->data);
	}
	if (NULL != skb->mac.raw) {
		new_pack->mac.raw = new_pack->data + (skb->mac.raw - skb->data);
	}
	return new_pack;
}

//TODO delete this function from here
#if 0
void packet_dump(sk_buff_t *pack) {
	char ip[15], mac[18];
        TRACE("--------dump-----------------\n");
        TRACE("protocol=0x%X\n", pack->protocol);
        TRACE("len=%d\n", pack->len);
        TRACE("mac.mach.type=%d\n", pack->mac.ethh->h_proto);
        macaddr_print(mac, pack->mac.ethh->h_source);
        TRACE("mac.ethh.src_addr=%s\n", mac);
        macaddr_print(mac, pack->mac.ethh->h_dest);
        TRACE("mac.ethh.dst_addr=%s\n", mac);
        if (pack->protocol == ETH_P_ARP) {
        TRACE("nh.arph.htype=%d\n", pack->nh.arph->htype);
        TRACE("nh.arph.ptype=%d\n", pack->nh.arph->ptype);
        TRACE("nh.arph.hlen=%d\n", pack->nh.arph->hlen);
        TRACE("nh.arph.plen=%d\n", pack->nh.arph->plen);
        TRACE("nh.arph.oper=%d\n", pack->nh.arph->oper);
        macaddr_print(mac, pack->nh.arph->sha);
        TRACE("nh.arph.sha=%s\n", mac);
        struct in_addr spa, tpa;
        spa.s_addr = pack->nh.arph->spa;
        TRACE("nh.arph.spa=%s\n", inet_ntoa(spa));
        macaddr_print(mac, pack->nh.arph->tha);
        TRACE("nh.arph.tha=%s\n", mac);
        tpa.s_addr = pack->nh.arph->tpa;
        TRACE("nh.arph.tpa=%s\n", inet_ntoa(tpa));
        } else if (pack->protocol == ETH_P_IP) {
                TRACE("nh.iph.ihl=%d\n", pack->nh.iph->ihl);
                TRACE("nh.iph.version=%d\n", pack->nh.iph->version);
                TRACE("nh.iph.tos=%d\n", pack->nh.iph->tos);
                TRACE("nh.iph.tot_len=%d\n", pack->nh.iph->tot_len);
                TRACE("nh.iph.id=%d\n", pack->nh.iph->id);
                TRACE("nh.iph.frag_off=%d\n", pack->nh.iph->frag_off);
                TRACE("nh.iph.ttl=%d\n", pack->nh.iph->ttl);
                TRACE("nh.iph.proto=0x%X\n", pack->nh.iph->proto);
                TRACE("nh.iph.check=%d\n", pack->nh.iph->check);
                struct in_addr saddr, daddr;
                saddr.s_addr = pack->nh.iph->saddr;
                daddr.s_addr = pack->nh.iph->daddr;
                TRACE("nh.iph.saddr=%s\n", inet_ntoa(saddr));
                TRACE("nh.iph.daddr=%s\n", inet_ntoa(daddr));
                if (pack->nh.iph->proto == ICMP_PROTO_TYPE) {
                        TRACE("h.icmph.type=%d\n", pack->h.icmph->type);
                        TRACE("h.icmph.code=%d\n", pack->h.icmph->code);
                        TRACE("h.icmph.checksum=%d\n", pack->h.icmph->checksum);
                } else if (pack->nh.iph->proto == UDP_PROTO_TYPE) {
                        LOG_DEBUG("h.uh.source=%d\n", pack->h.uh->source);
                        LOG_DEBUG("h.uh.dest=%d\n", pack->h.uh->dest);
                        LOG_DEBUG("h.uh.len=%d\n", pack->h.uh->len);
                        LOG_DEBUG("h.uh.check=%d\n", pack->h.uh->check);
        	}
        }
        TRACE("---------------end-----------------\n");
}
#endif
