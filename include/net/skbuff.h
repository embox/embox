/**
 * @file sk_buff.h
 *
 * @brief Definitions for the 'struct sk_buff' memory handlers.
 * @date 20.10.2009
 * @author Anton Bondarev
 */
#ifndef SKBUFF_H_
#define SKBUFF_H_

#include "types.h"

struct skb_timeval {
	uint32_t off_sec;
	uint32_t off_usec;
};

/* Packet types */

#define PACKET_HOST             0               /* To us                */
#define PACKET_BROADCAST        1               /* To all               */
#define PACKET_MULTICAST        2               /* To group             */
#define PACKET_OTHERHOST        3               /* To someone else      */
#define PACKET_OUTGOING         4               /* Outgoing of any type */
/* These ones are invisible by user level */
#define PACKET_LOOPBACK         5               /* MC/BRD frame looped back */
#define PACKET_FASTROUTE        6               /* Fastrouted frame     */

/**
 *      struct sk_buff - socket buffer
 *      @next: Next buffer in list
 *      @prev: Previous buffer in list
 *      @sk: Socket we are owned by
 *      @netdev: Device we arrived on/are leaving by
 *      @tstamp: Time we arrived
 *      @protocol: Packet protocol from driver
 *      @pkt_type: Packet class
 *      @len: Length of actual data
 *      @h: Transport layer header
 *      @nh: Network layer header
 *      @mac: Link layer header
 */
typedef struct sk_buff {
	/* These two members must be first. */
	struct sk_buff *next;
	struct sk_buff *prev;

	struct sock        *sk;
        struct net_device  *netdev;
    	struct skb_timeval tstamp;
#if 0
        void                    *ifdev;
#endif

        __be16                  protocol;
        uint8_t                 pkt_type;
        unsigned int            len;
        union {
                //tcphdr        *th;
                struct _udphdr  *uh;
                struct icmphdr *icmph;
                //igmphdr       *igmph;
                //iphdr         *ipiph;
                //ipv6hdr       *ipv6h;
    	        unsigned char   *raw;
        } h;
        union {
                struct iphdr   *iph;
        	//ipv6hdr       *ipv6h;
                struct arphdr  *arph;
                unsigned char   *raw;
        } nh;
        union {
                struct ethhdr  *ethh;
                unsigned char   *raw;
        } mac;
        void (*destructor)(struct sk_buff *skb);
        unsigned char *data;

}sk_buff_t;

typedef struct sk_buff_head {
	struct sk_buff *next;
	struct sk_buff *prev;
	__u32		qlen;
	spinlock_t	lock;
}sk_buff_head_t;

/**
 * allocate one instance of structure sk_buff. With pointed size and flags.
 * @return If size is more then mtu (now it is defined by macros ETHERNET_V2_FRAME_SIZE)
 * function will return queue of sk_buff. Else function return single sk_buff.
 * Function return NULL if function can't allocate demanded buffer
 */
extern struct sk_buff *alloc_skb(unsigned int size, gfp_t priority);

extern void kfree_skb(struct sk_buff *skb);

extern struct sk_buff *alloc_skb_fclone(struct sk_buff *skb, gfp_t priority);

/**
 * sk_buff clone it used as we want to queue sk_buff in several queue
 */
extern struct sk_buff *skb_clone(struct sk_buff *skb, gfp_t priority);

extern struct sk_buff *skb_copy(const struct sk_buff *skb, gfp_t priority);

#endif /* SKBUFF_H_ */
