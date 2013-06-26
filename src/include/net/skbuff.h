/**
 * @file
 * @brief Definitions for the 'struct sk_buff' memory handlers.
 *
 * @date 20.10.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 * @author Vladimir Sokolov
 */

#ifndef NET_SKBUFF_H_
#define NET_SKBUFF_H_

/* FIXME include this */
//#include <net/if_arp.h>
//#include <net/netdevice.h>
//#include <net/l3/ipv4/ip.h>
//#include <net/l3/icmpv4.h>
//#include <net/sock.h>
//#include <net/l4/udp.h>
//#include <net/l4/tcp.h>
#include <stdint.h>
#include <sys/types.h>

/* Prototypes */
struct sk_buff;
struct sock;
struct net_device;
struct tcphdr;
struct udphdr;
struct icmphdr;
struct icmp6hdr;
struct iphdr;
struct ip6hdr;
struct arpghdr;
struct ethhdr;


#if 0
struct skb_timeval {
	uint32_t off_sec;
	uint32_t off_usec;
};
#endif

#define SK_BUF_EXTRA_HEADROOM	50	/* Requires if someone wants to enlarge packet from head */

typedef struct sk_buff_head {
	struct sk_buff *next;       /* Next buffer in list */
	struct sk_buff *prev;       /* Previous buffer in list */
} sk_buff_head_t;

typedef struct sk_buff {        /* Socket buffer */
	/* This member must be first. */
	struct sk_buff_head lnk;    /* Pointers to next and previous packages */

	struct sock *sk;            /* Socket we are owned by */
	struct net_device *dev;     /* Device we arrived on/are leaving by */

		/* Packet protocol from driver or protocol to put into Eth header during assembling.
		 * We should get rid of it. It's almost useless during packet
		 * receiving and there is a special field in LL header
		 * for packet assembling.
		 */
	uint16_t protocol;

		/* Control buffer (used to store layer-specific info e.g. ip options)
		 * Nowdays it's used only in ip options, so it's a good idea to
		 * remove this field
		 */
	char cb[52];

		/* Length of actual data, from LL header till the end */
	unsigned int len;

	union {                     /* Transport layer header */
		struct tcphdr *th;
		struct udphdr *uh;
		struct icmphdr *icmph;
		struct icmp6hdr *icmp6h;
		unsigned char *raw;
	} h;

		/* Network layer header.
		 * Usually iph contains IP_MIN_HEADER_SIZE except some cases:
		 *	when we process incoming IP packet with options
		 *	when we use RAW sockets for sending (still not fixed in design)
		 */
	union {
		struct iphdr *iph;
		struct ip6hdr *ip6h;
		struct arpghdr *arpgh;
		unsigned char *raw;
	} nh;

		/* In current implementation our stack allocates
		 * ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE + ...
		 * for outgoing packets.
		 * So it's assumed to have eth header at the start
		 */
	union {                     /* Link layer header */
		struct ethhdr *ethh;
		unsigned char *raw;
	} mac;

		/* Pointer for buffer used to store all skb content.
		 * Used by operations with pool, so it MUST NOT be changed
		 */
	unsigned char *head;

		/* After processing by (incoming) stack packet is used by
		 * socket structures. Socket (== User) may consume only a part
		 * of data. Taken data ends with p_data
		 * Note:
		 *	Quoting from man recvfrom "If a message is too long to fit in
		 *	the supplied buffer, excess bytes may be discarded depending
		 *	on the type of socket the message is received from"
		 * So the presence of this field isn't mandatory.
		 */
	unsigned char *p_data;

} sk_buff_t;


/**
 * Allocate one instance of structure sk_buff. With pointed size and flags.
 * @return If size is more then mtu (now it is defined by module option
 * skb_buff_size) function will return queue of sk_buff. Else function
 * return single sk_buff.
 * Function return NULL if function can't allocate demanded buffer
 * TODO make skb_queue if `size` more than mtu
 */
extern struct sk_buff * skb_alloc(unsigned int size);

/**
 * Free skb allocated by skb_alloc
 */
extern void skb_free(struct sk_buff *skb);

/**
 *	skb_checkcopy_expand	-	check, copy and expand sk_buff
 *	@skb: buffer to check, copy
 *	@headroom: required amount of free bytes at head
 *	@tailroom: required amount of free bytes at tail
 *
 *	Make a copy of both an &sk_buff and its data and while doing so
 *	allocate additional space. Do nothing if we already have such amount
 *	of free space and this sbk and data are completely ours.
 *
 *	Returns NULL on failure or the pointer to the buffer
 *	on success.
 *
 *	In current implementation there is no "extra-large packets pool"
 *	to allocate data from. So no allocation if new size doesn't fit.
 */
extern struct sk_buff *skb_checkcopy_expand(struct sk_buff *skb,
				int headroom, int tailroom);

/**
 *	skb_shifthead	-	shift pointers to headers in the head of the skb structure
 *	@skb: buffer to process
 */
extern void skb_shifthead(struct sk_buff *skb, int headshift);

#if 0
/**
 * buff_to_skb parse buffer with size 'size' and write it to skb structure
 */
extern struct sk_buff * buff_to_skb(unsigned char *buff, unsigned int size);
#endif

#define SKB_SHARE_NO   0 /* make full copy of skb */
#define SKB_SHARE_DATA 1 /* make skb copy with shared data */
#define SKB_SHARE_ALL  2 /* make shared skb */

/**
 * sk_buff duplicate it used as we want to queue sk_buff in several queue
 * In current implementation we don't have shared area for packets data,
 * so copy and clone are the same.
 */
extern struct sk_buff *skb_share(struct sk_buff *skb, int share);

/**
 * Create copy of skb
 * In current implementation we don't have shared area for packets data,
 * so copy and clone are the same.
 */

/**
 * Allocate one instance of structure sk_buff_head.
 */
extern struct sk_buff_head * skb_queue_alloc(void);

/**
 * Initialize sk_buff_head
 */
extern void skb_queue_init(struct sk_buff_head *queue);

/**
 * Cleanup skb_queue
 */
extern void skb_queue_purge(struct sk_buff_head *queue);

/**
 * Free sk_buff_head structure with his elements
 */
extern void skb_queue_free(struct sk_buff_head *queue);

/**
 * Add skb to queue
 */
extern void skb_queue_push(struct sk_buff_head *queue, struct sk_buff *skb);

/**
 * Get first sk_buff from queue without removing
 */
extern struct sk_buff * skb_queue_front(struct sk_buff_head *queue);

/**
 * Get first sk_buff from queue
 */
extern struct sk_buff * skb_queue_pop(struct sk_buff_head *queue);

#include <net/if_arp.h>
#include <net/if_ether.h>

#endif /* NET_SKBUFF_H_ */
