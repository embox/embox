/**
 * @file
 * @brief Internet Control Message Protocol (ICMPv4)
 * @details RFC 792
 *
 * @date 14.03.09
 * @author Alexander Batyukov
 * @author Nikolay Korotky
 * @author Vladimir Sokolov
 * @author Ilia Vaprol
 */

#ifndef NET_L3_ICMPV4_H_
#define NET_L3_ICMPV4_H_

#include <linux/types.h>
#include <net/l3/ndp.h>
#include <net/skbuff.h>

/**
 * Prototypes
 */
struct sk_buff;

/**
 * ICMPv4 Types
 */
enum icmp_type {
	/* ICMPv4: Error Messages */
	ICMP_DEST_UNREACH      = 3,  /* Destination Unreachable
									Message */
	ICMP_SOURCE_QUENCH     = 4,  /* Source Quench Message */
	ICMP_REDIRECT          = 5,  /* Redirect Message */
	ICMP_TIME_EXCEED       = 11, /* Time Exceeded Message */
	ICMP_PARAM_PROB        = 12, /* Parameter Problem Message */

	/* ICMPv4: Informational Messages */
	ICMP_ECHO_REPLY        = 0,  /* Echo Reply Message */
	ICMP_ECHO_REQUEST      = 8,  /* Echo Request Message */
	ICMP_TIMESTAMP_REQUEST = 13, /* Timestamp Request Message */
	ICMP_TIMESTAMP_REPLY   = 14, /* Timestamp Reply Message */
	ICMP_INFO_REQUEST      = 15, /* Information Request Message */
	ICMP_INFO_REPLY        = 16, /* Information Reply Message */
};

/**
 * ICMPv4 Error Message Type */
#define ICMP_TYPE_ERROR(type) \
	(((type) == ICMP_DEST_UNREACH)        \
		|| ((type) == ICMP_SOURCE_QUENCH) \
		|| ((type) == ICMP_REDIRECT)      \
		|| ((type) == ICMP_TIME_EXCEED)   \
		|| ((type) == ICMP_PARAM_PROB))

/**
 * ICMPv4 Codes
 */
enum icmp_code {
	/* Echo Reply Message */
		/* 0 - always (MUST) */
	/* Destination Unreachable Message */
	ICMP_NET_UNREACH      = 0, /* Network unreachable */
	ICMP_HOST_UNREACH     = 1, /* Address unreachable */
	ICMP_PROT_UNREACH     = 2, /* Protocol unreachable */
	ICMP_PORT_UNREACH     = 3, /* Port unreachable */
	ICMP_FRAG_NEEDED      = 4, /* Fragmentation needed and DF
								  set */
	ICMP_SR_FAILED        = 5, /* Source route failed */
	__ICMP_DEST_UNREACH_MAX,
	/* Source Quench Message */
		/* 0 - always (MUST) */
	/* Redirect Message */
	ICMP_NET_REDIRECT     = 0, /* Redirect datagrams for the
								  network */
	ICMP_HOST_REDIRECT    = 1, /* Redirect datagrams for the
								  host */
	ICMP_NETTOS_REDIRECT  = 2, /* Redirect datagrams for the TOS
								  and network */
	ICMP_HOSTTOS_REDIRECT = 3, /* Redirect datagrams for the TOS
								  and network */
	__ICMP_REDIRECT_MAX,
	/* Echo Request Message */
		/* 0 - always (MUST) */
	/* Time Exceeded Message */
	ICMP_TTL_EXCEED       = 0, /* Time to live exceeded in
								  transit */
	ICMP_FRAG_TIME        = 1, /* Fragment reassembly time
								  exceeded */
	__ICMP_TIME_EXCEED_MAX,
	/* Parameter Problem Message */
	ICMP_PTR_ERROR        = 0, /* Pointer indicates the error */
	ICMP_PTR_UNUSED       = 1, /* Otherwise */
	__ICMP_PARAM_PROB_MAX,
	/* Timestamp Request Message */
		/* 0 - always (MUST) */
	/* Timestamp Reply Message */
		/* 0 - always (MUST) */
	/* Information Request Message */
		/* 0 - always (MUST) */
	/* Information Reply Message */
		/* 0 - always (MUST) */
};

/**
 * ICMPv4 Body for Echo Request/Reply Message
 */
struct icmpbody_echo {
	__be16 id;   /* An identifier of the message sequence */
	__be16 seq;  /* A sequence number of the message */
	__u8 data[]; /* Zero or more octets of arbitrary data */
} __attribute__((packed));

/**
 * ICMPv4 Body for Destination Unreachable Message
 */
struct icmpbody_dest_unreach {
	__be16 zero; /* Unused */
	__be16 mtu;  /* The Maximum Transmission Unit of the
					next-hop link */
	__u8 msg[];  /* The internet header plus the first 64 bits
					of the original datagram's data */
} __attribute__((packed));

/**
 * ICMPv4 Body for Source Quench Message
 */
struct icmpbody_source_quench {
	__be32 zero; /* Unused */
	__u8 msg[];  /* The internet header plus the first 64 bits
					of the original datagram's data */
} __attribute__((packed));

/**
 * ICMPv4 Body for Redirect Message
 */
struct icmpbody_redirect {
	struct in_addr gateway; /* Address of the gateway to which
							   traffic for the network specified
							   in the internet destination network
							   field of the original datagram's
							   data should be sent */
	__u8 msg[];             /* The internet header plus the first
							   64 bits of the original datagram's
							   data */
} __attribute__((packed));

/**
 * ICMPv4 Body for Time Exceeded Message
 */
struct icmpbody_time_exceed {
	__be32 zero; /* Unused */
	__u8 msg[];  /* The internet header plus the first 64 bits
					of the original datagram's data */
} __attribute__((packed));

/**
 * ICMPv4 Body for Parameter Problem Message
 */
struct icmpbody_param_prob {
	__u8 ptr;     /* Pointer */
	__u8 zero1;   /* Unused */
	__be16 zero2; /* Unused */
	__u8 msg[];   /* The internet header plus the first 64 bits
					 of the original datagram's data */
} __attribute__((packed));

/**
 * ICMPv4 Body for Timestamp Request/Reply Message
 */
struct icmpbody_timestamp {
	__be16 id;    /* An identifier of the message sequence */
	__be16 seq;   /* A sequence number of the message */
	__be32 orig;  /* The originate timestamp */
	__be32 recv;  /* The receive timestamp */
	__be32 trans; /* The transmit timestamp */
} __attribute__((packed));

/**
 * ICMPv4 Body for Information Request/Reply Message
 */
struct icmpbody_info {
	__be16 id;    /* An identifier of the message sequence */
	__be16 seq;   /* A sequence number of the message */
} __attribute__((packed));

/**
 * ICMPv4 Header Structure
 */
typedef struct icmphdr {
	__u8   type;  /* Message type */
	__u8   code;  /* Message code */
	__be16 check; /* Message checksum */
	union {       /* Message body */
		/* ICMPv4 Bodies: */
		struct icmpbody_echo echo;
		struct icmpbody_dest_unreach dest_unreach;
		struct icmpbody_source_quench source_quench;
		struct icmpbody_redirect redirect;
		struct icmpbody_time_exceed time_exceed;
		struct icmpbody_param_prob param_prob;
		struct icmpbody_timestamp timestamp;
		struct icmpbody_info info;
	} __attribute__((packed)) body[];
} __attribute__((packed)) icmphdr_t;

#define ICMP_MIN_HEADER_SIZE (sizeof(struct icmphdr))

static inline struct icmphdr * icmp_hdr(
		const struct sk_buff *skb) {
	return skb->h.icmph;
}

/**
 * ICMPv4 Discard - send ICMP packet relative to to packet
 * Extra arguments:
 * Type                Code                   Extra
 * ICMP_DEST_UNREACH   ICMP_NET_UNREACH       -
 *                     ICMP_HOST_UNREACH      -
 *                     ICMP_PROT_UNREACH      -
 *                     ICMP_PORT_UNREACH      -
 *                     ICMP_FRAG_NEEDED       uint16_t
 *                     ICMP_SR_FAILED         -
 * ICMP_SOURCE_QUENCH  -                      -
 * ICMP_REDIRECT       ICMP_NET_REDIRECT      struct in_addr *
 *                     ICMP_HOST_REDIRECT     struct in_addr *
 *                     ICMP_NETTOS_REDIRECT   struct in_addr *
 *                     ICMP_HOSTTOS_REDIRECT  struct in_addr *
 * ICMP_TIME_EXCEED    ICMP_TTL_EXCEED        -
 *                     ICMP_FRAG_TIME         -
 * ICMP_PARAM_PROB     ICMP_PTR_ERROR         uint8_t
 *                     ICMP_PTR_UNUSED        -
 */
extern int icmp_discard(struct sk_buff *skb, uint8_t type,
		uint8_t code, .../* extra */);

/**
 * ICMPv4 Discard Constants
 */
#define ICMP_DISCARD_MIN_SIZE 8
#define ICMP_DISCARD_MAX_SIZE \
	(576 /* See RCF 1812 4.3.2.3 */ \
		- (IP_MIN_HEADER_SIZE + ICMP_MIN_HEADER_SIZE))

#endif /* NET_L3_ICMPV4_H_ */
