/**
 * @file
 * @brief Useful functions for manipulations with a protoent structure
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#ifndef NET_UTIL_PROTOENT_API_H_
#define NET_UTIL_PROTOENT_API_H_

#include <netdb.h>

extern struct protoent * protoent_create(void);
extern int protoent_set_name(struct protoent *pe, const char *name);
extern int protoent_add_alias(struct protoent *pe, const char *alias);
extern int protoent_set_proto(struct protoent *pe, int proto);

#endif /* NET_UTIL_PROTOENT_API_H_ */
