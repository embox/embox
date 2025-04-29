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

#define IPPROTO_IPIP IPPROTO_IP
#define IPPROTO_GRE  IPPROTO_RAW

#define AF_LINK (PD_STUB("AF_LINK"), 0)

#define MCL_CURRENT ((void)PD_STUB("MCL_CURRENT"), 0x01)
#define MCL_FUTURE  (PD_STUB("MCL_FUTURE"), 0x02)

#define IFT_ETHER 0
struct sockaddr_dl {
	int sdl_type;
};
#define LLADDR(sdl) sdl

struct iovec;

static inline char *strsignal(int sig) {
	PD_STUB_USE();
	return "i-know-but-won't-tell-you-signal";
}

#include <stdio.h>
static inline FILE *fropen(void *cookie, int (*readfn)(void *, char *, int)) {
	return funopen(cookie, readfn, NULL, NULL, NULL);
}

#include <sys/types.h>

#endif /* PACKETDRILL_STUBS_H_ */

