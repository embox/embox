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
struct sk_buff_data;
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

typedef struct sk_buff_head {
	struct sk_buff *next;       /* Next buffer in list */
	struct sk_buff *prev;       /* Previous buffer in list */
} sk_buff_head_t;

typedef struct sk_buff {        /* Socket buffer */
	/* This member must be first. */
	struct sk_buff_head lnk;    /* Pointers to next and previous packages */

	struct sock *sk;            /* Socket we are owned by */
	struct net_device *dev;     /* Device we arrived on/are leaving by */

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
	struct sk_buff_data *data;

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
	unsigned char *p_data_end;

} sk_buff_t;

extern struct sk_buff_data * skb_data_alloc(void);
extern struct sk_buff_data * skb_data_clone(struct sk_buff_data *skb_data);
extern void skb_data_free(struct sk_buff_data *skb_data);

extern unsigned char * skb_data_get_extra_hdr(struct sk_buff_data *skb_data);
extern unsigned char * skb_data_get_data(struct sk_buff_data *skb_data);

extern unsigned int skb_max_extra_hdr_size(void);
extern unsigned int skb_max_size(void);
extern unsigned int skb_avail(struct sk_buff *skb);

/**
 * Wrap sk_buff_data into sk_buff structure
 */
extern struct sk_buff * skb_wrap(unsigned int size,
		unsigned int offset, struct sk_buff_data *skb_data);

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
 *	skb_rshift	-	perform right shift on skb data
 *	@skb: buffer to process
 */
extern void skb_rshift(struct sk_buff *skb, unsigned int count);

/**
 * Make a full sk_buff copy
 */
extern struct sk_buff * skb_copy(struct sk_buff *skb);

/**
 * Make a copy of sk_buff with shared packet data
 */
extern struct sk_buff * skb_clone(struct sk_buff *skb);

/**
 * Create copy of skb
 * In current implementation we don't have shared area for packets data,
 * so copy and clone are the same.
 */

/**
 * Initialize sk_buff_head
 */
extern void skb_queue_init(struct sk_buff_head *queue);

/**
 * Cleanup skb_queue
 */
extern void skb_queue_purge(struct sk_buff_head *queue);

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
