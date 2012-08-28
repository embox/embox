/**
 * @file
 * @brief Useful functions for manipulations with a hostent structure
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#ifndef NET_NETDB_HOSTENT_API_H_
#define NET_NETDB_HOSTENT_API_H_

struct hostent;

extern struct hostent * hostent_create(void);
extern int hostent_set_name(struct hostent *he, char *name);
extern int hostent_add_alias(struct hostent *he, char *alias);
extern int hostent_set_addr_info(struct hostent *he, int addrtype, int addrlen);
extern int hostent_add_addr(struct hostent *he, char *addr);

#endif /* NET_NETDB_HOSTENT_API_H_ */
