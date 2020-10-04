/**
 * @file
 *
 * @brif It's similar like uapi/linux/netlink.h (linux/rtnetlink.h)
 *
 * @date Oct 4, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_NET_SOCKET_UAPI_LINUX_NETLINK_H_
#define SRC_NET_SOCKET_UAPI_LINUX_NETLINK_H_

#include <stdint.h>
#include <sys/socket.h>

#define NETLINK_ROUTE       0   /* Routing/device hook */

struct sockaddr_nl {
	sa_family_t nl_family; /* AF_NETLINK */
	unsigned short nl_pad; /* zero */
	uint32_t nl_pid;       /* port ID	*/
	uint32_t nl_groups;    /* multicast groups mask */
};

struct nlmsghdr {
	uint32_t   nlmsg_len;   /* Length of message including header */
	uint16_t   nlmsg_type;  /* Message content */
	uint16_t   nlmsg_flags; /* Additional flags */
	uint32_t   nlmsg_seq;   /* Sequence number */
	uint32_t   nlmsg_pid;   /* Sending process port ID */
};

#define NLMSG_ALIGNTO            4U
#define NLMSG_ALIGN(len)         ( ((len)+NLMSG_ALIGNTO-1) & ~(NLMSG_ALIGNTO-1) )
#define NLMSG_HDRLEN             ((int) NLMSG_ALIGN(sizeof(struct nlmsghdr)))
#define NLMSG_LENGTH(len)        ((len) + NLMSG_HDRLEN)
#define NLMSG_SPACE(len)         NLMSG_ALIGN(NLMSG_LENGTH(len))
#define NLMSG_DATA(nlh)          ((void*)(((char*)nlh) + NLMSG_LENGTH(0)))
#define NLMSG_NEXT(nlh,len)      ((len) -= NLMSG_ALIGN((nlh)->nlmsg_len), \
					(struct nlmsghdr*)(((char*)(nlh)) + NLMSG_ALIGN((nlh)->nlmsg_len)))
#define NLMSG_OK(nlh,len)        ((len) >= (int)sizeof(struct nlmsghdr) && \
					(nlh)->nlmsg_len >= sizeof(struct nlmsghdr) && \
					(nlh)->nlmsg_len <= (len))
#define NLMSG_PAYLOAD(nlh,len)   ((nlh)->nlmsg_len - NLMSG_SPACE((len)))

#define NLMSG_NOOP               0x1 /* Nothing. */
#define NLMSG_ERROR              0x2 /* Error   */
#define NLMSG_DONE               0x3 /* End of a dump */
#define NLMSG_OVERRUN            0x4 /* Data lost		*/

#define NLMSG_MIN_TYPE           0x10 /* < 0x10: reserved control messages */

#endif /* SRC_NET_SOCKET_UAPI_LINUX_NETLINK_H_ */
