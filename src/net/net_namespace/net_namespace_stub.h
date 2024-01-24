#ifndef NET_NAMESPACE_H_
#define NET_NAMESPACE_H_

#include <lib/libds/dlist.h>
#include <limits.h>

typedef struct net_namespace {
} net_namespace_t;

typedef struct net_namespace_ptr {
} net_namespace_p;

#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
extern net_namespace_p init_net_ns;

extern net_namespace_p get_net_ns();
extern int setns(const char *name);
extern int unshare_by_name(const char *name);

#define assign_net_ns(net_ns1, net_ns2)
#define cmp_net_ns(net_ns1, net_ns2) 1

#define fill_net_ns_from_sk(net_ns, sk, out_skb)
#endif
#endif /* NET_NAMESPACE_H_ */
