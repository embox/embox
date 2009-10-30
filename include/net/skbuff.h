/**
 * \file sk_buff.h
 *
 * \date 20.10.2009
 * \author Anton Bondarev
 */

#ifndef SKBUFF_H_
#define SKBUFF_H_

struct skb_timeval {
    UINT32 off_sec;
    UINT32 off_usec;
};

typedef struct sk_buff {
	    struct sk_buff *next;
	    struct sk_buff *prev;

        struct net_device      *netdev;
        void                    *ifdev;
        struct sock             *sk;
        unsigned short          protocol;
        unsigned int            len;
        union {
                //tcphdr        *th;
                struct _udphdr  *uh;
                struct _icmphdr *icmph;
                //igmphdr       *igmph;
                //iphdr         *ipiph;
                //ipv6hdr       *ipv6h;
    	        unsigned char   *raw;
        } h;
        union {
                struct _iphdr   *iph;
        	//ipv6hdr       *ipv6h;
                struct _arphdr  *arph;
                unsigned char   *raw;
        } nh;
        union {
                struct _ethhdr  *ethh;
                unsigned char   *raw;
        } mac;
        void (*destructor)(struct sk_buff *skb);
        unsigned char *data;

}sk_buff_type;

struct sk_buff_head {
	struct sk_buff *next;
    struct sk_buff *prev;
};

struct sk_buff_queue {
	struct sk_buff *head;
    struct sk_buff *tail;
};

struct sk_buff *alloc_skb(unsigned int size, gfp_t priority);

void kfree_skb(struct sk_buff *skb);
struct sk_buff *alloc_skb_fclone(struct sk_buff *skb, gfp_t priority);

#endif /* SKBUFF_H_ */
