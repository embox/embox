/**
 * @file
 * @brief Definitions for network databaes operations
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#ifndef NET_NETDB_H_
#define NET_NETDB_H_


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


/**
 * P R I V A T E   M E M B E R S
 */

extern struct hostent * hostent_create(void);
extern int hostent_set_name(struct hostent *he, char *name);
extern int hostent_add_alias(struct hostent *he, char *alias);
extern int hostent_set_addr_info(struct hostent *he, int addrtype, int addrlen);
extern int hostent_add_addr(struct hostent *he, char *addr);

#endif /* NET_NETDB_H_ */
