/*
 * zeromq_embox_compat.h
 *
 *  Created on: 25 mars 2013
 *      Author: fsulima
 */

#ifndef ZEROMQ_EMBOX_COMPAT_H_
#define ZEROMQ_EMBOX_COMPAT_H_

/*
 *	IPv6 address structure
 */

struct in6_addr {
	union {
		unsigned char		u6_addr8[16];
		unsigned short		u6_addr16[8];
		unsigned long 		u6_addr32[4];
	} in6_u;
#define s6_addr			in6_u.u6_addr8
#define s6_addr16		in6_u.u6_addr16
#define s6_addr32		in6_u.u6_addr32
};

/* IPv6 Wildcard Address (::) and Loopback Address (::1) defined in RFC2553
 * NOTE: Be aware the IN6ADDR_* constants and in6addr_* externals are defined
 * in network byte order, not in host byte order as are the IPv4 equivalents
 */

struct sockaddr_in6 {
	unsigned short int	sin6_family;    /* AF_INET6 */
	unsigned short sin6_port;      /* Transport layer port # */
	unsigned long			sin6_flowinfo;  /* IPv6 flow information */
	struct in6_addr		sin6_addr;      /* IPv6 address */
	unsigned long			sin6_scope_id;  /* scope id (new in RFC2553) */
};

#ifdef __cplusplus
extern "C" {
#endif
	extern const struct sockaddr_in6 in6addr_any;
#ifdef __cplusplus
}
#endif

struct sockaddr_un {
    unsigned short sun_family;  /* AF_UNIX */
    char sun_path[108];
};

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TCP_NODELACK TCP_NODELAY

struct sockaddr_storage {
	union {
		struct sockaddr_in sin;
		struct sockaddr_in6 sin6;
		struct sockaddr_un sun;
	} addr;
};

static inline char *tempnam(const char *dir, const char *pfx) {
	DPRINT();
	return NULL;
}

#define EPROTO          71      /* Protocol error */

static inline int socketpair(int domain, int type, int protocol, int sv[2]) {
	DPRINT();
	errno = EOPNOTSUPP;
	return -1;
}

typedef struct addrinfo {
  int             ai_flags;
  int             ai_family;
  int             ai_socktype;
  int             ai_protocol;
  size_t          ai_addrlen;
  char            *ai_canonname;
  struct sockaddr  *ai_addr;
  struct addrinfo  *ai_next;
};

#define AI_PASSIVE 0x100
#define AI_NUMERICHOST 0x200

#define EAI_MEMORY 1
#define EAI_FAIL 2

static inline
int getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res) {
	DPRINT();
	return EAI_FAIL;
}

static inline
void freeaddrinfo(struct addrinfo *res) {
	DPRINT();
	return;
}

#define NI_MAXHOST 1
#define NI_NUMERICHOST 2

static inline
int getnameinfo(const struct sockaddr *sa, socklen_t salen,
                       char *host, size_t hostlen,
                       char *serv, size_t servlen, int flags) {
	DPRINT();
	return EAI_FAIL;
}

#endif /* ZEROMQ_EMBOX_COMPAT_H_ */
