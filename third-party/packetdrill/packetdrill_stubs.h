/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.07.2014
 */

#ifndef PACKETDRILL_STUBS_H_
#define PACKETDRILL_STUBS_H_

#include <net/if.h>

#include <stdio.h>
#define PD_STUB(msg, ...) \
	fprintf(stderr, ">>> packetdrill: " msg "\n", ##__VA_ARGS__)

#define PD_STUB_USE(msg) \
	PD_STUB("%s" msg, __func__)

#define SIOCGIFFLAGS (PD_STUB("SIOCGIFFLAGS"), 0)
#define SIOCSIFFLAGS (PD_STUB("SIOCSIFFLAGS"), 1)

#define IPPROTO_IPIP IPPROTO_IP
#define IPPROTO_GRE  IPPROTO_RAW

#define AF_LINK (PD_STUB("AF_LINK"), 0)

#define MCL_CURRENT ((void)PD_STUB("MCL_CURRENT"), 0x01)
#define MCL_FUTURE  (PD_STUB("MCL_FUTURE"), 0x02)

struct ifreq {
	char    ifr_name[IFNAMSIZ];/* Interface name */
	union {
#if 0
		struct sockaddrifr_addr ifr_addr;
		struct sockaddrifr_dstaddr ifr_dstaddr;
		struct sockaddrifr_broadaddr ifr_broadaddr;
		struct sockaddrifr_netmask ifr_netmask;
		struct sockaddrifr_hwaddr ifr_hwaddr;
#endif
		short   ifr_flags;
		int     ifr_ifindex;
		int     ifr_metric;
		int     ifr_mtu;
#if 0
		struct ifmapifr_map ifr_map;
#endif
		char    ifr_slave[IFNAMSIZ];
		char    ifr_newname[IFNAMSIZ];
		char *  ifr_data;
	};
};

#define IFT_ETHER 0
struct sockaddr_dl {
	int sdl_type;
};
#define LLADDR(sdl) sdl

struct iovec;

extern int asprintf(char **strp, const char *fmt, ...);

static inline char *strsignal(int sig) {
	PD_STUB_USE();
	return "i-know-but-won't-tell-you-signal";
}

static inline int if_nametoindex(const char *name) {
	PD_STUB_USE();
	return 1;
}

static inline int mlockall(int flags) {
	PD_STUB_USE();
	return 0;
}

static inline int munlockall(void) {
	PD_STUB_USE();
	return 0;
}

#include <stdio.h>
static inline FILE *fropen(void *cookie, int (*readfn)(void *, char *, int)) {
	return funopen(cookie, readfn, NULL, NULL, NULL);
}

#include <sys/types.h>

#endif /* PACKETDRILL_STUBS_H_ */

