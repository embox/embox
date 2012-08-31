/**
 * @file
 * @brief Definitions for network database operations
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#ifndef NET_NETDB_H_
#define NET_NETDB_H_

/**
 * Error return value for networt database operations
 */
extern int h_errno;

/**
 * Possible error codes
 */
#define NETDB_SUCCESS   0  /* Ok */
#define HOST_NOT_FOUND  1  /* Authoritative Answer Host not found */
#define TRY_AGAIN       2  /* Non-Authoritative Host not found, or SERVERFAIL */
#define NO_RECOVERY     3  /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define NO_DATA         4  /* Valid name, no data record of requested type */
#define NO_ADDRESS NO_DATA

/**
 * Manipulation with error codes
 */
extern void herror(const char *msg);
extern const char * hstrerror(int err);

/**
 * Host name entity
 */
struct hostent {
	char *h_name;       /* official name of host */
	char **h_aliases;   /* alias list */
	int h_addrtype;     /* host address type */
	int h_length;       /* length of address */
	char **h_addr_list; /* list of addresses */
};

/**
 * Network host database functions
 */
extern void sethostent(int stayopen);
extern void endhostent(void);
extern struct hostent * gethostent(void);

/**
 * Converting a hostname into a network address
 */
extern struct hostent * gethostbyname(const char *hostname);

#endif /* NET_NETDB_H_ */
