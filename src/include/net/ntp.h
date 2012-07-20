/*
 * @brief NTP
 *
 * @date 13.07.2012
 * @author Alexander Kalmuk
 */

#ifndef NET_NTP_H_
#define NET_NTP_H_

#include <types.h>

struct s_ntpdata {
	__u16 sec;
	__u16 fraction;
};

struct l_ntpdata {
	__u32 sec;
	__u32 fraction;
};

/* @see RFC 5905: Packet Header Format */
typedef struct ntphdr {
	__u8 status; /*< LI, VN, Mode */
	__u8 stratum;
	__u8 poll;
	__s8 precision;
	struct s_ntpdata rootdelay;
	struct s_ntpdata rootdisp; /*< root dispersion */
	__be32 refid;
	struct l_ntpdata ref_ts;
	struct l_ntpdata org_ts;
	struct l_ntpdata rec_ts;
	struct l_ntpdata xmt_ts;
} __attribute__((packed)) ntphdr_t;

/* Leap second Indicator constants (LI) */
#define NTP_LI_NO_WARNING 0x00
#define NTP_OVERFLOW      0x40 /* 61 sec in last minute of mouth */
#define NTP_UNDERFLOW     0x80 /* 59 sec in last minute of mouth */
#define NTP_CLOCK_UNSYNC  0xc0

/* Version Number (VN) */
#define NTP_V_1 0x08
#define NTP_V_2 0x10
#define NTP_V_3 0x18
#define NTP_V_4 0x20

/* TODO Mode */
#define NTP_CLIENT    3
#define NTP_SERVER    4
#define NTP_BROADCAST 5

/* Stratum */
#define NTP_SERVER_UNSPEC    0
#define NTP_SERVER_PRIMARY   1
#define NTP_SERVER_SECONDARY 2 /* 2-15 */
#define NTP_SERVER_UNSYNC    16
/* RFC 5905: 17-255 reserved*/

/* Use it if you send request to SNTP server or won't to
 * specify complex NTP options. See RFC 4030, client operations */
extern int ntp_client_xmit(int sock, struct sockaddr_in *dst);
extern int ntp_client_receive(struct sk_buff *skb, struct socket *sock);

static inline __u8 get_mode(struct ntphdr *ntp) {
	return (ntp->status & 7);
}

#endif /* NET_NTP_H_ */
