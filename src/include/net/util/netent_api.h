/**
 * @file
 * @brief Useful functions for manipulations with a netent structure
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#ifndef NET_UTIL_NETENT_API_H_
#define NET_UTIL_NETENT_API_H_

#include <netdb.h>
#include <stdint.h>

extern struct netent * netent_create(void);
extern int netent_set_name(struct netent *ne, const char *name);
extern int netent_add_alias(struct netent *ne, const char *alias);
extern int netent_set_addr_info(struct netent *ne, int addrtype);
extern int netent_set_net(struct netent *ne, uint32_t net);

#endif /* NET_UTIL_NETENT_API_H_ */
