/**
 * @file
 * @brief NTP
 *
 * @date 13.07.2012
 * @author Alexander Kalmuk
 */

#ifndef NET_NTP_H_
#define NET_NTP_H_

#include <linux/types.h>
#include <stdint.h>
#include <kernel/time/time.h>
#include <net/skbuff.h>
#include <sys/socket.h>

struct s_ntpdata {
	__u16 sec;
	__u16 fraction;
};

struct l_ntpdata {
	__u32 sec;
	__u32 fraction;
};

struct sockaddr_in;

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

#define NTP_SERVER_PORT 123

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
#define NTP_BROADCAST_CLIENT 6

/* Stratum */
#define NTP_SERVER_UNSPEC    0
#define NTP_SERVER_PRIMARY   1
#define NTP_SERVER_SECONDARY 2 /* 2-15 */
#define NTP_SERVER_UNSYNC    16
/* RFC 5905: 17-255 reserved*/

/* Kiss-o'-Death codes (KoD), that marked with "MUST" in RFC */
#define NTP_DENY    100
#define NTP_RSTR    101
#define NTP_RATE    102
#define NTP_IGNORED 103

/* Use it if you send request to SNTP server or won't to
 * specify complex NTP options. See RFC 4030, client operations. */
extern int ntp_client_xmit(int sock, struct sockaddr_in *dst);
extern int ntp_receive(struct sock *sk, struct sk_buff *skb);
extern struct timespec ntp_delay(struct ntphdr *ntp);
extern int ntp_start(void);
extern int ntp_stop(void);
void ntp_server_set(uint32_t s_addr);

static inline struct timespec ntp_to_timespec(struct l_ntpdata ld) {
	struct timespec ts;
	ts.tv_sec = ld.sec;
	ts.tv_nsec = (ld.fraction / 1000) * 232;
	return ts;
}

static inline struct l_ntpdata timespec_to_ntp(struct timespec ts) {
	struct l_ntpdata ld;
	ld.sec = ts.tv_sec;
	ld.fraction = (ts.tv_nsec / 232) * 1000;
	return ld;
}

static inline __u8 get_mode(struct ntphdr *ntp) {
	return (ntp->status & 7);
}

static inline __u8 get_leap(struct ntphdr *ntp) {
	return (ntp->status & 0xC0);
}

static inline __u8 get_version(struct ntphdr *ntp) {
	return (ntp->status & 0x38);
}

#endif /* NET_NTP_H_ */
