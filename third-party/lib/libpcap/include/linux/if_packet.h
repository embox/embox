#ifndef _LIBPCAP_LINUX_IF_PACKET_H_
#define _LIBPCAP_LINUX_IF_PACKET_H_

#include <netpacket/packet.h>

#define TP_STATUS_KERNEL		      0

#define PACKET_ADD_MEMBERSHIP		1

#define PACKET_MR_PROMISC	1
#define PACKET_OUTGOING		4		/* Outgoing of any type */
#define PACKET_RX_RING			5
#define PACKET_STATISTICS		6
#define PACKET_VERSION			10
#define PACKET_HDRLEN			11
#define PACKET_RESERVE			12

enum tpacket_versions {
	TPACKET_V1,
	TPACKET_V2
};

struct tpacket2_hdr {
	__u32		tp_status;
	__u32		tp_len;
	__u32		tp_snaplen;
	__u16		tp_mac;
	__u16		tp_net;
	__u32		tp_sec;
	__u32		tp_nsec;
	__u16		tp_vlan_tci;
	__u16		tp_vlan_tpid;
	__u8		tp_padding[4];
};

#define TPACKET_ALIGNMENT 16
#define TPACKET_ALIGN(x)	(((x)+TPACKET_ALIGNMENT-1)&~(TPACKET_ALIGNMENT-1))
#define TPACKET2_HDRLEN		(TPACKET_ALIGN(sizeof(struct tpacket2_hdr)) + sizeof(struct sockaddr_ll))

struct tpacket_stats {
	unsigned int    tp_packets;
	unsigned int    tp_drops;
};

struct tpacket_req {
	unsigned int	tp_block_size;	/* Minimal size of contiguous block */
	unsigned int	tp_block_nr;	/* Number of blocks */
	unsigned int	tp_frame_size;	/* Size of frame */
	unsigned int	tp_frame_nr;	/* Total number of frames */
};

struct packet_mreq {
	int		mr_ifindex;
	unsigned short	mr_type;
	unsigned short	mr_alen;
	unsigned char	mr_address[8];
};

#endif /*_LIBPCAP_LINUX_IF_PACKET_H_*/
