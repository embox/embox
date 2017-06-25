/**
 * @file
 * @brief Definitions for network database operations
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_NETDB_H_
#define COMPAT_POSIX_NETDB_H_

#include <netinet/in.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/socket.h>

__BEGIN_DECLS

/**
 * Host name entity
 */
struct hostent {
	char *h_name;       /* Official name of the host */
	char **h_aliases;   /* Alias list */
	int h_addrtype;     /* Host address type */
	int h_length;       /* Length of address */
	char **h_addr_list; /* List of addresses */
#define h_addr h_addr_list[0]
};

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
 * Error return value for networt host database operations
 */
extern int h_errno;

/**
 * Network host database function errors
 */
#define HOST_NOT_FOUND  1  /* Authoritative answer Host not
							  found */
#define NO_DATA         2  /* Valid name, no data record of
							  requested type */
#define NO_ADDRESS NO_DATA
#define NO_RECOVERY     3  /* Non recoverable errors */
#define TRY_AGAIN       4  /* Non-authoritative host not found */

/**
 * Manipulation with network host database error codes
 */
extern void herror(const char *msg);
extern const char * hstrerror(int error_code);

/**
 * Network name entity
 */
struct netent {
	char *n_name;     /* Official name of the host */
	char **n_aliases; /* Alias list */
	int n_addrtype;   /* Network address type */
	uint32_t n_net;   /* Network number (HOST) */
};

/**
 * Network database functions
 */
extern void setnetent(int stayopen);
extern struct netent * getnetent(void);
extern void endnetent(void);
extern struct netent * getnetbyaddr(uint32_t net, int type);
extern struct netent * getnetbyname(const char *name);

/**
 * Protocol name entity
 */
struct protoent {
	char *p_name;     /* Official name of the protocol */
	char **p_aliases; /* Alias list */
	int p_proto;      /* The protocol number */
};

/**
 * Network protocol database functions
 */
extern void setprotoent(int stayopen);
extern struct protoent * getprotoent(void);
extern void endprotoent(void);
extern struct protoent * getprotobyname(const char *name);
extern struct protoent * getprotobynumber(int proto);

/**
 * Service name entity
 */
struct servent {
	char *s_name;     /* Official name of the service */
	char **s_aliases; /* Alias list */
	int s_port;       /* The port number (BE) */
	char *s_proto;    /* The name of the protocol */
};

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

/**
 * Address information entity
 */
struct addrinfo {
	int ai_flags;             /* Input flags */
	int ai_family;            /* Socket family */
	int ai_socktype;          /* Socket type */
	int ai_protocol;          /* Socket protocol */
	socklen_t ai_addrlen;     /* Length of socket address */
	struct sockaddr *ai_addr; /* Socket address */
	char *ai_canonname;       /* Service's canonical name */
	struct addrinfo *ai_next; /* Next entity */
};

/**
 * Address information entity flags
 */
#define AI_PASSIVE     0x01 /* Socket address is intended for
							   bind() */
#define AI_CANONNAME   0x02 /* Request for canonical name */
#define AI_NUMERICHOST 0x04 /* Return numeric host address as
							   name */
#define AI_NUMERICSERV 0x08 /* Inhibit service name resolution */
#define AI_V4MAPPED    0x10 /* If no IPv6 addresses are found,
							   query for IPv4 addresses and
							   return them to the caller as
							   IPv4-mapped IPv6 addresses */
#define AI_ALL         0x20 /* Query for both IPv4 and IPv6
							   addresses */
#define AI_ADDRCONFIG  0x40 /* Query for IPv4 addresses only when
							   an IPv4 address is configured;
							   query for IPv6 addresses only when
							   an IPv6 address is configured */

/**
 * Address information functions
 */
extern int getaddrinfo(const char *nodename, const char *servname,
		const struct addrinfo *hints, struct addrinfo **res);
extern void freeaddrinfo(struct addrinfo *ai);

/**
 * Address name information function flags
 */
#define NI_NOFQDN       0x01 /* Only the nodename portion of the
								FQDN is returned for local hosts */
#define NI_NUMERICHOST  0x02 /* The numeric form of the node's
								address is returned instead of its
								name */
#define NI_NAMEREQD     0x04 /* Return an error if the node's name
								cannot be located in the
								database */
#define NI_NUMERICSERV  0x08 /* The numeric form of the service
								address is returned instead of its
								name */
#define NI_NUMERICSCOPE 0x10 /* For IPv6 addresses, the numeric
								form of the scope identifier is
								returned instead of its name */
#define NI_DGRAM        0x20 /* Indicates that the service is a
								datagram service */

/**
 * Network names limits
 */
#define NI_MAXHOST 1025
#define NI_MAXSERV 32

/**
 * Address information function errors
 */
#define EAI_AGAIN     1 /* The name could not be resolved at this
						   time. Future attempts may succeed */
#define EAI_BADFLAGS  2 /* The flags had an invalid value */
#define EAI_FAIL      3 /* A non-recoverable error occurred */
#define EAI_FAMILY    4 /* The address family was not recognized
						   or the address length was invalid for
						   the specified family */
#define EAI_MEMORY    5 /* There was a memory allocation failure */
#define EAI_NONAME    6 /* The name does not resolve for the
						   supplied parameters. NI_NAMEREQD is set
						   and the host's name cannot be located,
						   or both nodename and servname were
						   null */
#define EAI_SERVICE   7 /* The service passed was not recognized
						   for the specified socket type */
#define EAI_SOCKTYPE  8 /* The intended socket type was not
						   recognized */
#define EAI_SYSTEM    9 /* A system error occurred. The error code
						   can be found in errno */
#define EAI_OVERFLOW 10 /* An argument buffer overflowed */
#define EAI_NODATA   11 /* No address associated with nodename */

/**
 * Manipulation with address information error codes
 */
extern const char * gai_strerror(int error_code);

/**
 * Address name information functions
 */
extern
int getnameinfo(const struct sockaddr *sa, socklen_t salen,
		char *node, size_t nodelen, char *serv,
                size_t servlen, int flags);

__END_DECLS

#endif /* COMPAT_POSIX_NETDB_H_ */
