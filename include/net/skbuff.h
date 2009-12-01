/**
 * @file sk_buff.h
 *
 * @date 20.10.2009
 * @author Anton Bondarev
 */
#ifndef SKBUFF_H_
#define SKBUFF_H_

struct skb_timeval {
	uint32_t off_sec;
	uint32_t off_usec;
};

/**
 *      struct sk_buff - socket buffer
 *      @next: Next buffer in list
 *      @prev: Previous buffer in list
 *      @sk: Socket we are owned by
 *      @netdev: Device we arrived on/are leaving by
 *      @tstamp: Time we arrived
 *      @protocol: Packet protocol from driver
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


extern struct sk_buff *alloc_skb(unsigned int size, gfp_t priority);

extern void kfree_skb(struct sk_buff *skb);
extern struct sk_buff *alloc_skb_fclone(struct sk_buff *skb, gfp_t priority);
/**
 * sk_buff clone it used as we want to queue sk_buff in several queue
 */
extern struct sk_buff *skb_clone(struct sk_buff *skb, gfp_t priority);
extern struct sk_buff *skb_copy(const struct sk_buff *skb, gfp_t priority);

#endif /* SKBUFF_H_ */
