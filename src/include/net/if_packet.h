/**
 * @file
 * @brief
 *
 * @date 09.05.13
 * @author Ilia Vaprol
 */

#ifndef NET_IF_PACKET_H_
#define NET_IF_PACKET_H_

/**
 * Prototypes
 */
struct sk_buff;

/**
 * Packet types
 */
enum {
	PACKET_HOST,      /* To us */
	PACKET_BROADCAST, /* To all */
	PACKET_MULTICAST, /* To group */
	PACKET_OTHERHOST, /* To someone else */
	PACKET_OUTGOING,  /* From us */
	PACKET_LOOPBACK   /* We are in loopback */
};

/**
 * Determine the paket type
 */
extern int pkt_type(const struct sk_buff *skb);

#endif /* NET_IF_PACKET_H_ */
