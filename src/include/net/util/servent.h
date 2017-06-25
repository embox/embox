/**
 * @file
 * @brief Useful functions for manipulations with a servent structure
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#ifndef NET_UTIL_SERVENT_API_H_
#define NET_UTIL_SERVENT_API_H_

#include <netdb.h>

extern struct servent * servent_create(void);
extern int servent_set_name(struct servent *se, const char *name);
extern int servent_add_alias(struct servent *se, const char *alias);
extern int servent_set_port(struct servent *se, int port);
extern int servent_set_proto(struct servent *se, const char *proto);

extern struct servent * servent_make(const char *name, int port,
		const char *proto);

#endif /* NET_UTIL_SERVENT_API_H_ */
