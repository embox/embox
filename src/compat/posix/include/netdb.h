/**
 * @file
 * @brief Definitions for network database operations
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_NETDB_H_
#define COMPAT_POSIX_NETDB_H_

#include <sys/cdefs.h>
#include <stdint.h>
#include <netinet/in.h>

__BEGIN_DECLS

/**
 * Possible error codes
 */
#define NETDB_SUCCESS   0  /* ok */
#define HOST_NOT_FOUND  1  /* authoritative answer Host not found */
#define TRY_AGAIN       2  /* non-authoritative host not found */
#define NO_RECOVERY     3  /* non recoverable errors */
#define NO_DATA         4  /* valid name, no data record of requested type */
#define NO_ADDRESS NO_DATA

/**
 * Error return value for networt database operations
 */
extern int h_errno;

/**
 * Host name entity
 */
struct hostent {
	char *h_name;       /* official name of the host */
	char **h_aliases;   /* alias list */
	int h_addrtype;     /* host address type */
	int h_length;       /* length of address */
	char **h_addr_list; /* list of addresses */
};

/**
 * Network name entity
 */
struct netent {
	char *n_name;     /* official name of the host */
	char **n_aliases; /* alias list */
	int n_addrtype;   /* network address type */
	uint32_t n_net;   /* network number (host byte order) */
};

/**
 * Protocol name entity
 */
struct protoent {
	char *p_name;     /* official name of the protocol */
	char **p_aliases; /* alias list */
	int p_proto;      /* the protocol number */
};

/**
 * Service name entity
 */
struct servent {
	char *s_name;     /* official name of the service */
	char **s_aliases; /* alias list */
	int s_port;       /* the port number (network byte order) */
	char *s_proto;    /* the name of the protocol */
};

/**
 * Manipulation with error codes
 */
extern void herror(const char *msg);
extern const char * hstrerror(int err);

/**
 * Network host database functions
 */
extern void sethostent(int stayopen);
extern struct hostent * gethostent(void);
extern void endhostent(void);
extern struct hostent * gethostbyaddr(const void *addr,
		socklen_t len, int type);
extern struct hostent * gethostbyname(const char *name);

/**
 * Network database functions
 */
extern void setnetent(int stayopen);
extern struct netent * getnetent(void);
extern void endnetent(void);
extern struct netent * getnetbyaddr(uint32_t net, int type);
extern struct netent * getnetbyname(const char *name);

/**
 * Network protocol database functions
 */
extern void setprotoent(int stayopen);
extern struct protoent * getprotoent(void);
extern void endprotoent(void);
extern struct protoent * getprotobyname(const char *name);
extern struct protoent * getprotobynumber(int proto);

/**
 * Network services database functions
 */
extern void setservent(int stayopen);
extern struct servent * getservent(void);
extern void endservent(void);
extern struct servent * getservbyname(const char *name,
		const char *proto);
extern struct servent * getservbyport(int port,
		const char *proto);

__END_DECLS

#endif /* COMPAT_POSIX_NETDB_H_ */
