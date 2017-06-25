/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.07.2014
 */

#ifndef SOCKET_PACKET_H_
#define SOCKET_PACKET_H_

struct sk_buff;
extern void sock_packet_add(struct sk_buff *skb, unsigned short protocol);

#endif /* SOCKET_PACKET_H_ */

