/**
 * @file
 * @brief Definitions for network databaes operations
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#ifndef NET_NETDB_H_
#define NET_NETDB_H_

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
