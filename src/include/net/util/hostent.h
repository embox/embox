/**
 * @file
 * @brief Useful functions for manipulations with a hostent structure
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#ifndef NET_UTIL_HOSTENT_API_H_
#define NET_UTIL_HOSTENT_API_H_

#include <netdb.h>

extern struct hostent * hostent_create(void);
extern int hostent_set_name(struct hostent *he, const char *name);
extern int hostent_add_alias(struct hostent *he, const char *alias);
extern int hostent_set_addr_info(struct hostent *he, int addrtype, int addrlen);
extern int hostent_add_addr(struct hostent *he, const void *addr);

extern struct hostent * hostent_make(const char *name,
		int addrtype, int addrlen, const void *addr);

#endif /* NET_UTIL_HOSTENT_API_H_ */
