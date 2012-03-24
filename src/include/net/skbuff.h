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
//#include <net/if_arp.h>
//#include <net/if_ether.h>
//#include <net/netdevice.h>
//#include <net/ip.h>
//#include <net/icmp.h>
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


#define SK_BUF_EXTRA_HEADROOM	50	/* Requires if someone wants to enlarge packet from head */

typedef struct sk_buff {        /* Socket buffer */
	/* These two members must be first. */
	struct sk_buff *next;       /* Next buffer in list */
	struct sk_buff *prev;       /* Previous buffer in list */

	struct sock *sk;            /* Socket we are owned by */
	struct net_device *dev;     /* Device we arrived on/are leaving by */

	__be16 protocol;            /* Packet protocol from driver */
	uint8_t pkt_type;           /* Packet class */
	char cb[52];                /* Control buffer (used to store layer-specific info e.g. ip options) */
	unsigned int len;           /* Length of actual data */
	union {                     /* Transport layer header */
		struct tcphdr *th;
		struct udphdr *uh;
		struct icmphdr *icmph;
		unsigned char *raw;
	} h;
	union {                     /* Network layer header */
		struct iphdr *iph;
		struct arphdr *arph;
		unsigned char *raw;
	} nh;
	union {                     /* Link layer header */
		struct ethhdr *ethh;
		unsigned char *raw;
	} mac;
	__be16 offset;              /* Offset information for ip fragmentation*/

	unsigned char *head;		/* Pointer for buffer used to store all skb content */
	unsigned char *data;		/* Data head pointer */
	unsigned char *p_data;		/* Pointer for current processing data. See tail in Linux skb */
	char prot_info;				/* Protocol level additional data, tcp uses for state handling */
} sk_buff_t;

typedef struct sk_buff_head {
	struct sk_buff *next;
	struct sk_buff *prev;
} sk_buff_head_t;


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

/**
 *	skb_copy_expand	-	copy and expand sk_buff
 *	@skb: buffer to copy
 *	@newheadroom: add at least those amount of free bytes at head
 *	@newtailroom: add at least those amount of free bytes at tail
 *	@priority: allocation priority
 *
 *	Make a copy of both an &sk_buff and its data and while doing so
 *	allocate additional space.
 *
 *	This is used when the caller wishes to modify the data and needs a
 *	private copy of the data to alter as well as more space for new fields.
 *	Returns NULL on failure or the pointer to the buffer
 *	on success.
 */
extern struct sk_buff *skb_copy_expand(struct sk_buff *skb,
				int newheadroom, int newtailroom, gfp_t priority);

/**
 *	skb_checkcopy_expand	-	check, copy and expand sk_buff
 *	@skb: buffer to check, copy
 *	@headroom: required amount of free bytes at head
 *	@tailroom: required amount of free bytes at tail
 *	@priority: allocation priority
 *
 *	Make a copy of both an &sk_buff and its data and while doing so
 *	allocate additional space. Do nothing if we already have such amount
 *	of free space.
 *
 *	Returns NULL on failure or the pointer to the buffer
 *	on success.
 */
extern struct sk_buff *skb_checkcopy_expand(struct sk_buff *skb,
				int headroom, int tailroom, gfp_t priority);

/**
 *	skb_shifthead	-	shift pointers to headers in the head of the skb structure
 *	@skb: buffer to process
 */
extern void skb_shifthead(struct sk_buff *skb, int headshift);

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
 * Get sk_buff from list without removing it from list
 */
extern struct sk_buff * skb_peek(struct sk_buff_head *list);

/**
 * Move newsk to tail of the list.
 */
extern void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk);

/**
 * Receive struct sk_buff from receive queue of the sock
 */
extern struct sk_buff * skb_recv_datagram(struct sock *sk, unsigned flags,
					 int noblock, int *err);
/**
 * Get sk_buff from queue of sock without removing it from sock's queue
 */
extern struct sk_buff * skb_peek_datagram(struct sock *sk, unsigned flags,
					 int noblock, int *err);
#endif /* NET_SKBUFF_H_ */
