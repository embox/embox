/**
 * @file
 * @brief Neighbor Discovery for IP version 6 (IPv6)
 *     (extends for Internet Control Message Protocol Version 6)
 * @details RFC 4861
 *
 * @date 25.10.13
 * @author Ilia Vaprol
 */

#ifndef NET_L3_NDP_H_
#define NET_L3_NDP_H_

#include <endian.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Prototypes
 */
struct net_device;

/**
 * NDP Types
 */
enum ndp_type {
	NDP_ROUTER_SOLICIT   = 133, /* Router Solicitation Message */
	NDP_ROUTER_ADVERT    = 134, /* Router Advertisement Message */
	NDP_NEIGHBOR_SOLICIT = 135, /* Neighbor Solicitation Message */
	NDP_NEIGHBOR_ADVERT  = 136, /* Neighbor Advertisement Message */
	NDP_REDIRECT         = 137, /* Redirect Message */
};

/**
 * NDP Codes
 */
enum ndp_code {
	__ndp_code_unused
	/* Router Solicitation Message */
		/* 0 - always (MUST) */
	/* Router Advertisement Message */
		/* 0 - always (MUST) */
	/* Neighbor Solicitation Message */
		/* 0 - always (MUST) */
	/* Neighbor Advertisement Message */
		/* 0 - always (MUST) */
	/* Redirect Message */
		/* 0 - always (MUST) */
};

/**
 * NDP Body for Router Solicitation Message
 */
struct ndpbody_router_solicit {
	__be32 zero;     /* Reserved */
	__u32 options[]; /* Options:
					     - Source link-layer address */
} __attribute__((packed));

/**
 * NDP Body for Router Advertisement Message
 */
struct ndpbody_router_advert {
	__u8 cur_hop_limit; /* Default hop_limit for outgoing
						   IPv6 packets */
#if __BYTE_ORDER == __LITTLE_ENDIAN
	__u8 zero:6,        /* Reserved */
		other:1,        /* Other configuration flag */
		managed:1;      /* Managed address configuration flag */
#elif __BYTE_ORDER == __BIG_ENDIAN
	__u8 managed:1,
		other:1,
		zero:6;
#endif
	__be16 life_time;    /* Router lifetime */
	__be32 reachbl_time; /* Reachable Time */
	__be32 retrans_time; /* Retransmission time (milliseconds) */
	__u32 options[];     /* Options:
						     - Source link-layer address
						     - MTU
						     - Prefix Information */
} __attribute__((packed));

/**
 * NDP Body for Neighbor Solicitation Message
 */
struct ndpbody_neighbor_solicit {
	__be32 zero;            /* Reserved */
	struct in6_addr target; /* Target Address */
	__u32 options[];        /* Options:
							    - Source link-layer address */
} __attribute__((packed));

/**
 * NDP Body for Neighbor Advertisement Message
 */
struct ndpbody_neighbor_advert {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	__u32 zero1:5,         /* Reserved */
		override:1,        /* Override flag */
		solicited:1,       /* Solicited flag */
		router:1,          /* Router flag */
		zero2:24;          /* Reserved */
#elif __BYTE_ORDER == __BIG_ENDIAN
	__u32 router:1,
		solicited:1,
		override:1,
		zero1:5,
		zero2:24;
#endif
	struct in6_addr target; /* Target Address */
	__u32 options[];        /* Options:
							    - Target link-layer address */
} __attribute__((packed));

/**
 * NDP Body for Redirect Message
 */
struct ndpbody_redirect {
	__be32 zero;            /* Reserved */
	struct in6_addr target; /* Target Address */
	struct in6_addr dest;   /* Destination Address */
	__u32 options[];        /* Options:
							    - Target link-layer address
								- Redirected Header */
} __attribute__((packed));

/**
 * NDP Options Header
 */
struct ndpoptionshdr {
	__u8 type; /* Option type */
	__u8 len;  /* Option length */
} __attribute__((packed));

/**
 * NDP Options Types
 */
enum ndp_options_type {
	NDP_SOURCE_LL_ADDR  = 1, /* Source Link-Layer Address */
	NDP_TARGET_LL_ADDR  = 2, /* Target Link-Layer Address */
	NDP_PREFIX_INFO     = 3, /* Prefix Information */
	NDP_REDIRECTED_HDR  = 4, /* Redirected Header */
	NDP_MTU             = 5, /* MTU */
};

/**
 * NDP Options Lengths
 */
enum ndp_options_len {
	__ndp_options_len_unused
	/* Source/Target Link-layer Address */
		/* The length of the option in units of 8 octets */
	/* Prefix Information */
		/* 4 - always (MUST) */
	/* Redirected Header */
		/* The length of the option in units of 8 octets */
	/* MTU */
		/* 1 - always (MUST) */
};

/**
 * NDP Options for Source/Target Link-layer Address
 */
struct ndpoptions_ll_addr {
	struct ndpoptionshdr hdr; /* Option header */
	__u8 ll_addr[];           /* The variable length
								 link-layer address */
} __attribute__((packed));

/**
 * NDP Options for Prefix Information
 */
struct ndpoptions_prefix_info {
	struct ndpoptionshdr hdr; /* Option header */
	__u8 prefix_len;          /* The number of leading bits in the
								 prefix that are valid */
#if __BYTE_ORDER == __LITTLE_ENDIAN
	__u8 zero1:6,             /* Reserved */
		autonom:1,            /* Autonomous address-configuration
								 flag */
		onlink:1;             /* On-link flag */
#elif __BYTE_ORDER == __BIG_ENDIAN
	__u8 onlink:1,
		autonom:1,
		zero1:6;
#endif
	__be32 valid_lifetime;    /* Valid Lifetime */
	__be32 prefer_lifetime;   /* Preferred Lifetime */
	__be32 zero2;             /* Reserved */
	struct in6_addr prefix;   /* An IPv6 address or a prefix of an
								 IPv6 address */
} __attribute__((packed));

/**
 * NDP Options for Redirected Header
 */
struct ndpoptions_redirected_hdr {
	struct ndpoptionshdr hdr; /* Option header */
	__u16 zero[3];            /* Reserved */
	__u64 msg[];              /* IP header + data */
} __attribute__((packed));

/**
 * NDP Options for MTU
 */
struct ndpoptions_mtu {
	struct ndpoptionshdr hdr; /* Option header */
	__be16 zero;              /* Reserved */
	__be32 mtu;               /* The recommended MTU
								 for the link */
} __attribute__((packed));

/**
 * NDP Send
 */
extern int ndp_send(uint8_t type, uint8_t code, const void *body,
		size_t body_sz, struct net_device *dev);

#endif /* NET_L3_NDP_H_ */
