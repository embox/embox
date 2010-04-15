/**
 * @file
 * @brief Definitions for the 'struct sk_buff' memory handlers.
 *
 * @date 20.10.2009
 * @author Anton Bondarev
 */
#ifndef SKBUFF_H_
#define SKBUFF_H_

#include <types.h>

struct skb_timeval {
	uint32_t off_sec;
	uint32_t off_usec;
};

/* Packet types */

#define PACKET_HOST             0   /* To us                */
#define PACKET_BROADCAST        1   /* To all               */
#define PACKET_MULTICAST        2   /* To group             */
#define PACKET_OTHERHOST        3   /* To someone else      */
#define PACKET_OUTGOING         4   /* Outgoing of any type */
/* These ones are invisible by user level */
#define PACKET_LOOPBACK         5   /* MC/BRD frame looped back */
#define PACKET_FASTROUTE        6   /* Fastrouted frame     */

/**
 * struct sk_buff - socket buffer
 * @next: Next buffer in list
 * @prev: Previous buffer in list
 * @sk: Socket we are owned by
 * @netdev: Device we arrived on/are leaving by
 * @tstamp: Time we arrived
 * @protocol: Packet protocol from driver
 * @pkt_type: Packet class
 * @cb: control buffer, used to store layer-specific info e.g. ip options
 * @len: Length of actual data
 * @h: Transport layer header
 * @nh: Network layer header
 * @mac: Link layer header
 */
typedef struct sk_buff {
	/* These two members must be first. */
	struct sk_buff *next;
	struct sk_buff *prev;
	struct sock *sk;
	struct net_device *dev;
	struct skb_timeval tstamp;
	__be16 protocol;
	uint8_t pkt_type;
	char cb[52];
	unsigned int len;
	union {
#if 0
		tcphdr *th;
#endif
		struct udphdr *uh;
		struct icmphdr *icmph;
#if 0
		igmphdr *igmph;
		iphdr *ipiph;
		ipv6hdr *ipv6h;
#endif
		unsigned char *raw;
	} h;
	union {
		struct iphdr *iph;
#if 0
		ipv6hdr *ipv6h;
#endif
		struct arphdr *arph;
		unsigned char *raw;
	} nh;
	union {
		struct ethhdr *ethh;
		unsigned char *raw;
	} mac;
	void (*destructor)(struct sk_buff *skb);
	unsigned char *data;
	unsigned char tries;
} sk_buff_t;

typedef struct sk_buff_head {
	sk_buff_t *next;
	sk_buff_t *prev;
	uint32_t qlen;
	spinlock_t lock;
} sk_buff_head_t;

#define SKB_LIST_HEAD_INIT(name) { (sk_buff_t *)(&name), (sk_buff_t *)(&name), 0, 0}

#define SKB_LIST_HEAD(name) \
		struct sk_buff_head name = SKB_LIST_HEAD_INIT(name)

/**
 * function must called if we want use this functionality.
 * It init queue free packet
 */
extern void skb_init(void);

/**
 * allocate one instance of structure sk_buff. With pointed size and flags.
 * @return If size is more then mtu (now it is defined by macros
 * CONFIG_ETHERNET_V2_FRAME_SIZE) function will return queue of sk_buff.
 * Else function return single sk_buff.
 * Function return NULL if function can't allocate demanded buffer
 */
extern struct sk_buff *alloc_skb(unsigned int size, gfp_t priority);

extern void kfree_skb(sk_buff_t *skb);

extern struct sk_buff *alloc_skb_fclone(sk_buff_t *skb, gfp_t priority);

/**
 * sk_buff clone it used as we want to queue sk_buff in several queue
 */
extern struct sk_buff *skb_clone(sk_buff_t *skb, gfp_t priority);

extern struct sk_buff *skb_copy(const sk_buff_t *skb, gfp_t priority);

/**
 * allocate one instance of structure sk_buff_head.
 */
extern struct sk_buff_head *alloc_skb_queue(int qlen);

/**
 * Empty a list.
 * return head of queue allocated in alloc_skb_queue to the free queue pool
 * and give back all contained sk_buff in this list
 */
extern void skb_queue_purge(sk_buff_head_t *queue);

/**
 * Peek an sk_buff.
 * Returns NULL for an empty list or a pointer to the head element.
 */
extern sk_buff_t *skb_peek(struct sk_buff_head *list_);

/**
 * Remove sk_buff from list.
 */
extern void skb_unlink(sk_buff_t *skb, struct sk_buff_head *list);

/**
 * Remove the head of the list.
 */
extern sk_buff_t *skb_dequeue(struct sk_buff_head *list);

extern void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk);

extern struct sk_buff *skb_recv_datagram(struct sock *sk, unsigned flags,
					 int noblock, int *err);

#endif /* SKBUFF_H_ */
