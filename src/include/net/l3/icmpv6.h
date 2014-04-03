/**
 * @file
 * @brief Internet Control Message Protocol (ICMPv6)
 * for the Internet Protocol Version 6 (IPv6) Specification
 * @details RFC 4443
 *
 * @date 17.05.13
 * @author Ilia Vaprol
 */

#ifndef NET_L3_ICMPV6_H_
#define NET_L3_ICMPV6_H_

#include <linux/types.h>
#include <net/l3/ndp.h>
#include <net/skbuff.h>

/**
 * Prototypes
 */
struct sk_buff;

/**
 * ICMPv6 Types
 */
enum icmp6_type {
	/* ICMPv6: Error Messages */
	ICMP6_DEST_UNREACH = 1, /* Destination Unreachable Message */
	ICMP6_PACK_TOO_BIG = 2, /* Packet Too Big Message */
	ICMP6_TIME_EXCEED  = 3, /* Time Exceeded Message */
	ICMP6_PARAM_PROB   = 4, /* Parameter Problem Message */

	/* ICMPv6: Informational Messages */
	ICMP6_ECHO_REQUEST = 128, /* Echo Request Message */
	ICMP6_ECHO_REPLY   = 129, /* Echo Reply Message */
};

/**
 * ICMPv6 Codes
 */
enum icmp6_code {
	/* Destination Unreachable Message */
	ICMP6_NET_UNREACH   = 0, /* No route to destination */
	ICMP6_ADMIN_PROHIBT = 1, /* Communication with destination
								administratively prohibited */
	ICMP6_NOT_NEIGHBOUR = 2, /* Beyond scope of source address */
	ICMP6_HOST_UNREACH  = 3, /* Address unreachable */
	ICMP6_PORT_UNREACH  = 4, /* Port unreachable */
	/* Packet Too Big Message */
		/* 0 - unused (SHOULD) */
	/* Time Exceeded Message */
	ICMP6_HOP_LIMIT     = 0, /* Hop limit exceeded in transit */
	ICMP6_FRAG_TIME     = 1, /* Fragment reassembly time
								exceeded */
	/* Parameter Problem Message */
	ICMP6_HEADER_ERROR  = 0, /* Erroneous header field
								encountered */
	ICMP6_NEXT_ERROR    = 1, /* Unrecognized Next Header type
								encountered */
	ICMP6_OPTION_ERROR  = 2, /* Unrecognized IPv6 option
								encountered */
	/* Echo Request Message */
		/* 0 - always (MUST) */
	/* Echo Reply Message */
		/* 0 - always (MUST) */
};

/**
 * ICMPv6 Body for Destination Unreachable Message
 */
struct icmp6body_dest_unreach {
	__be32 zero; /* Unused */
	__u8 msg[];  /* As much of invoking packet as possible
					without the ICMPv6 packet exceeding the
					minimum IPv6 MTU */
} __attribute__((packed));

/**
 * ICMPv6 Body for Packet Too Big Message
 */
struct icmp6body_pack_too_big {
	__be32 mtu; /* The Maximum Transmission Unit of the
				   next-hop link */
	__u8 msg[]; /* As much of invoking packet as possible
				   without the ICMPv6 packet exceeding the
				   minimum IPv6 MTU */
} __attribute__((packed));

/**
 * ICMPv6 Body for Time Exceeded Message
 */
struct icmp6body_time_exceed {
	__be32 zero; /* Unused */
	__u8 msg[];  /* As much of invoking packet as possible
					without the ICMPv6 packet exceeding the
					minimum IPv6 MTU */
} __attribute__((packed));

/**
 * ICMPv6 Body for Parameter Problem Message
 */
struct icmp6body_param_prob {
	__be32 ptr; /* Pointer */
	__u8 msg[]; /* As much of invoking packet as possible
				   without the ICMPv6 packet exceeding the
				   minimum IPv6 MTU */
} __attribute__((packed));

/**
 * ICMPv6 Body for Echo Request/Reply Message
 */
struct icmp6body_echo {
	__be16 id;   /* An identifier of the message sequence */
	__be16 seq;  /* A sequence number of the message */
	__u8 data[]; /* Zero or more octets of arbitrary data */
} __attribute__((packed));

/**
 * ICMPv6 Header Structure
 */
typedef struct icmp6hdr {
	__u8   type;  /* Message type */
	__u8   code;  /* Message code */
	__be16 check; /* Message checksum */
	union {       /* Message body */
		/* ICMPv6 Bodies: */
		struct icmp6body_dest_unreach dest_unreach;
		struct icmp6body_pack_too_big pack_too_big;
		struct icmp6body_time_exceed time_exceed;
		struct icmp6body_echo echo;

		/* NDP Bodies: */
		struct ndpbody_router_solicit router_solicit;
		struct ndpbody_router_advert router_advert;
		struct ndpbody_neighbor_solicit neighbor_solicit;
		struct ndpbody_neighbor_advert neighbor_advert;
	} __attribute__((packed)) body[];
} __attribute__((packed)) icmp6hdr_t;

#define ICMP6_MIN_HEADER_SIZE (sizeof(struct icmp6hdr))

static inline struct icmp6hdr * icmp6_hdr(
		const struct sk_buff *skb) {
	return skb->h.icmp6h;
}

/**
 * ICMPv6 Send
 */
extern int icmp6_send(struct sk_buff *skb, uint8_t type,
		uint8_t code, const void *body, size_t body_sz);

#endif /* NET_L3_ICMPV6_H_ */
