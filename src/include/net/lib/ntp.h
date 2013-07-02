/**
 * @file
 * @brief Simple Network Time Protocol (SNTP) Version 4
 *
 * @date 13.07.12
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_NTP_H_
#define NET_LIB_NTP_H_

#include <hal/arch.h>
#include <linux/types.h>
#include <time.h>

#define NTP_PORT 123

/**
 * Leap Indicator
 */
#define NTP_LI_NO_WARNING 0 /* no warning */
#define NTP_LI_OVERFLOW   1 /* 61 sec in last minute of mouth */
#define NTP_LI_UNDERFLOW  2 /* 59 sec in last minute of mouth */
#define NTP_LI_CLK_UNSYNC 3 /* alarm condition (clock not synchronized) */

/**
 * Version Number
 */
#define NTP_VERSION 4 /* NTP V4 */

/**
 * Mode
 */
#define NTP_MOD_CLIENT           3 /* client */
#define NTP_MOD_SERVER           4 /* server */
#define NTP_MOD_BROADCAST        5 /* broadcast */
#define NTP_MOD_BROADCAST_CLIENT 6 /* client broadcast */

/**
 * Stratum
 */
#define NTP_STRATUM_UNSPEC    0 /* kiss-o'-death message */
#define NTP_STRATUM_PRIMARY   1 /* primary reference (e.g.,
								  synchronized by radio clock */
#define NTP_STRATUM_SECONDARY 2 /* 2-15: secondary reference
								  (synchronized by NTP or SNTP) */
#define NTP_STRATUM_UNSYNC    16 /* 16-255: reserved */

/**
 * Poll limits
 */
#define NTP_POLL_MIN 4
#define NTP_POLL_MAX 17

struct ntp_data_s {
	__be16 sec;
	__be16 frac;
};

struct ntp_data_l {
	__be32 sec;
	__be32 frac;
};

struct ntphdr {
#if defined(__LITTLE_ENDIAN)
	__u8 mode:3,
		version:3,
		leap:2;
#elif defined(__BIG_ENDIAN)
	__u8 leap:2,                 /* leap indicator */
		version:3,               /* version number */
		mode:3;                  /* mode */
#endif
	__u8 stratum;                /* stratum */
	__u8 poll;                   /* poll exponent */
	__s8 precision;              /* precision exponent */
	struct ntp_data_s rootdelay; /* root delay */
	struct ntp_data_s rootdisp;  /* root dispersion */
	char refid[4];               /* reference ID */
	struct ntp_data_l reftime;   /* reference timestamp */
	struct ntp_data_l org;       /* origin timestamp */
	struct ntp_data_l rec;       /* receive timestamp */
	struct ntp_data_l xmt;       /* transmit timestamp */
} __attribute__((packed));

extern int ntp_build(struct ntphdr *ntph, int leap, int mode,
		int stratum, int poll, int precision,
		struct ntp_data_l *ref, struct ntp_data_l *org,
		struct ntp_data_l *rec, struct ntp_data_l *xmt);
extern int ntp_data_l_to_timespec(const struct ntp_data_l *ndl,
		struct timespec *out_ts);
extern int ntp_timespec_to_data_l(const struct timespec *ts,
		struct ntp_data_l *out_ndl);
extern int ntp_mode_client(const struct ntphdr *ntph);
extern int ntp_mode_server(const struct ntphdr *ntph);
extern int ntp_valid_stratum(const struct ntphdr *ntph);
extern const char * ntp_stratum_error(const struct ntphdr *ntph);
extern int ntp_delay(const struct ntphdr *ntp,
		struct timespec *out_ts);
extern int ntp_offset(const struct ntphdr *ntp,
		struct timespec *out_ts);
extern int ntp_time(const struct ntphdr *ntph,
		struct timespec *out_ts);

#endif /* NET_LIB_NTP_H_ */
