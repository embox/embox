/**
 * @file
 * @brief Definitions for the 'struct sk_buff' memory handlers.
 *
 * @date 20.10.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef NET_SKBUFF_H_
#define NET_SKBUFF_H_

/* FIXME include this */
//#include <net/icmp.h>
//#include <net/if_arp.h>
//#include <net/if_ether.h>
//#include <net/ip.h>
//#include <net/netdevice.h>
//#include <net/sock.h>
//#include <net/udp.h>
#include <types.h>

struct skb_timeval {
	uint32_t off_sec;
	uint32_t off_usec;
};

/* Packet types */

#define PACKET_HOST             0   /* To us */
#define PACKET_BROADCAST        1   /* To all */
#define PACKET_MULTICAST        2   /* To group */
#define PACKET_OTHERHOST        3   /* To someone else */
#define PACKET_OUTGOING         4   /* Outgoing of any type */
/* These ones are invisible by user level */
#define PACKET_LOOPBACK         5   /* MC/BRD frame looped back */
#define PACKET_FASTROUTE        6   /* Fastrouted frame */

typedef struct sk_buff {        /* Socket buffer */
	/* These two members must be first. */
	struct sk_buff *next;       /* Next buffer in list */
	struct sk_buff *prev;       /* Previous buffer in list */
	struct sock *sk;            /* Socket we are owned by */
	struct net_device *dev;     /* Device we arrived on/are leaving by */
#if 0
	struct skb_timeval tstamp;  /* Time we arrived */
#endif
	__be16 protocol;            /* Packet protocol from driver */
	uint8_t pkt_type;           /* Packet class */
	char cb[52];                /* Control buffer (used to store layer-specific info e.g. ip options) */
	unsigned int len;           /* Length of actual data */
	union {                     /* Transport layer header */
		struct tcphdr *th;
		struct udphdr *uh;
		struct icmphdr *icmph;
#if 0
		igmphdr *igmph;
		iphdr *ipiph;
		ipv6hdr *ipv6h;
#endif
		unsigned char *raw;
	} h;
	union {                     /* Network layer header */
		struct iphdr *iph;
#if 0
		ipv6hdr *ipv6h;
#endif
		struct arphdr *arph;
		unsigned char *raw;
	} nh;
	union {                     /* Link layer header */
		struct ethhdr *ethh;
		unsigned char *raw;
	} mac;
	uint16_t links;             /* Amount pointer, which link to itself */
#if 0
	void (*destructor)(struct sk_buff *skb);
#endif
	unsigned char *data;
#if 0
	unsigned char tries;
#endif
} sk_buff_t;

typedef struct sk_buff_head {
	struct sk_buff *next;
	struct sk_buff *prev;
} sk_buff_head_t;

#if 0
#define SKB_LIST_HEAD_INIT(name) { (sk_buff_t *)(&name), (sk_buff_t *)(&name), 0, 0}

#define SKB_LIST_HEAD(name) \
		struct sk_buff_head name = SKB_LIST_HEAD_INIT(name)
#endif

#if 0
/**
 * function must called if we want use this functionality.
 * It init queue free packet
 */
extern void skb_init(void);
#endif

/**
 * Allocate one instance of structure sk_buff. With pointed size and flags.
 * @return If size is more then mtu (now it is defined by macros
 * CONFIG_ETHERNET_V2_FRAME_SIZE) function will return queue of sk_buff.
 * Else function return single sk_buff.
 * Function return NULL if function can't allocate demanded buffer
 */
extern struct sk_buff * alloc_skb(unsigned int size, gfp_t priority);

/**
 * Free skb allocated by alloc_skb
 */
extern void kfree_skb(struct sk_buff *skb);

#if 0
extern struct sk_buff * alloc_skb_clone(sk_buff_t *skb, gfp_t priority);
#endif

/**
 * sk_buff clone it used as we want to queue sk_buff in several queue
 */
extern struct sk_buff * skb_clone(sk_buff_t *skb, gfp_t priority);

/**
 * Create copy of skb
 */
extern struct sk_buff * skb_copy(const struct sk_buff *skb, gfp_t priority);

/**
 * Allocate one instance of structure sk_buff_head.
 */
extern struct sk_buff_head * alloc_skb_queue(void);

/**
 * Free sk_buff_head structure with his elements
 */
extern void skb_queue_purge(struct sk_buff_head *queue);

/**
 * Get struct sk_buff from the head of the list.
 */
extern struct sk_buff * skb_dequeue(struct sk_buff_head *list);

/**
 * Move newsk to tail of the list.
 */
extern void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk);

/**
 * Receive struct sk_buff from receive queue of the sock
 */
extern struct sk_buff * skb_recv_datagram(struct sock *sk, unsigned flags,
					 int noblock, int *err);

#endif /* NET_SKBUFF_H_ */
