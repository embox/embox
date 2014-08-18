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
//#include <net/netdevice.h>
//#include <net/l3/ipv4/ip.h>
//#include <net/l3/icmpv4.h>
//#include <net/l4/udp.h>
//#include <net/l4/tcp.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/time.h>

/* Prototypes */
struct sk_buff;
struct sk_buff_data;
struct sk_buff_extra;
struct net_device;
struct tcphdr;
struct udphdr;
struct icmphdr;
struct icmp6hdr;
struct iphdr;
struct ip6hdr;
struct arphdr;
struct ethhdr;
struct iovec;

typedef struct sk_buff_head {
	struct sk_buff *next;       /* Next buffer in list */
	struct sk_buff *prev;       /* Previous buffer in list */
} sk_buff_head_t;

typedef struct sk_buff {        /* Socket buffer */
	/* This member must be first. */
	struct sk_buff_head lnk;    /* Pointers to next and previous packages */

	struct net_device *dev;     /* Device we arrived on/are leaving by */

		/* Control buffer (used to store layer-specific info e.g. ip options)
		 * Nowdays it's used only in ip options, so it's a good idea to
		 * remove this field
		 */
	char cb[52];

		/* Length of actual data, from LL header till the end */
	size_t len;

		/* Transport layer header */
	union {
		struct tcphdr *th;
		struct udphdr *uh;
		struct icmphdr *icmph;
		struct icmp6hdr *icmp6h;
		unsigned char *raw;
	} h;

		/* Network layer header */
	union {
		struct iphdr *iph;
		struct ip6hdr *ip6h;
		struct arphdr *arph;
		unsigned char *raw;
	} nh;

		/* Link layer header */
	union {
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

	struct timeval tstamp;
} sk_buff_t;

extern size_t skb_max_size(void);
extern size_t skb_max_extra_size(void);

extern void * skb_data_cast_in(struct sk_buff_data *skb_data);
extern struct sk_buff_data * skb_data_cast_out(void *data);
extern void * skb_extra_cast_in(struct sk_buff_extra *skb_extra);
extern struct sk_buff_extra * skb_extra_cast_out(void *extra);

extern struct sk_buff_data * skb_data_alloc(void);
extern struct sk_buff_data * skb_data_clone(
		struct sk_buff_data *skb_data);
extern int skb_data_cloned(const struct sk_buff_data *skb_data);
extern void skb_data_free(struct sk_buff_data *skb_data);

extern struct sk_buff_extra * skb_extra_alloc(void);
extern void skb_extra_free(struct sk_buff_extra *skb_extra);

/**
 * Wrap sk_buff_data into sk_buff structure
 */
extern struct sk_buff * skb_wrap(size_t size,
		struct sk_buff_data *skb_data);

/**
 * Allocate one instance of structure sk_buff. With pointed size and flags.
 * @return If size is more then mtu (now it is defined by module option
 * skb_buff_size) function will return queue of sk_buff. Else function
 * return single sk_buff.
 * Function return NULL if function can't allocate demanded buffer
 * TODO make skb_queue if `size` more than mtu
 */
extern struct sk_buff * skb_alloc(size_t size);

extern struct sk_buff * skb_realloc(size_t size,
		struct sk_buff *skb);

/**
 * Free skb allocated by skb_alloc
 */
extern void skb_free(struct sk_buff *skb);

/**
 * Perform right shift on skb data
 * @skb: buffer to process
 * @count : buffer to process
 */
extern void skb_rshift(struct sk_buff *skb, size_t count);

/**
 * Make a full sk_buff copy
 */
extern struct sk_buff * skb_copy(const struct sk_buff *skb);

/**
 * Make a copy of sk_buff with shared packet data
 */
extern struct sk_buff * skb_clone(const struct sk_buff *skb);

/**
 * Make sk_buff without shared packet data
 */
extern struct sk_buff * skb_declone(struct sk_buff *skb);

/**
 * Write sk_buff data to buffers pointed by iovec.
 *
 * @param skb
 * @param iov
 * @param iovlen
 *
 * @return Written data count in bytes.
 */
extern int skb_write_iovec(const void *buf, int buflen, struct iovec *iov, int iovlen);

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

#include <net/netdevice.h>

#endif /* NET_SKBUFF_H_ */
