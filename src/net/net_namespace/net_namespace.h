#ifndef NET_NAMESPACE_H_
#define NET_NAMESPACE_H_

#include <limits.h>
#include <lib/libds/dlist.h>

#define NET_NAMESPACE_ENABLED  (1)

typedef struct net_namespace {
	struct dlist_head lnk;
	char name[NAME_MAX + 1];
	struct inode *inode;
	int ref_cnt;
} net_namespace_t;

typedef struct net_namespace_pointer {
	struct net_namespace *p;
} net_namespace_p;

extern struct net_namespace init_net_ns_s;
extern net_namespace_p init_net_ns;

extern net_namespace_p get_net_ns();
extern int setns_by_name(const char *name);
extern int unshare_by_name(const char *name);
extern int unshare();

#define assign_net_ns(net_ns1, net_ns2) ((net_ns1).p = (net_ns2).p)
#define cmp_net_ns(net_ns1, net_ns2) ((net_ns1).p == (net_ns2).p)
extern int setns(int fd);
extern void netns_decrement_ref_cnt(net_namespace_p netns);
extern void netns_increment_ref_cnt(net_namespace_p netns);

#define fill_net_ns_from_sk(net_ns, sk, out_skb)					\
	(net_ns).p = sk != NULL ? (sk->net_ns).p : ((*out_skb)->dev->net_ns).p;		\
	if ((sk != NULL) && (*out_skb != NULL)) {					\
		(net_ns).p = ((sk->net_ns).p != NULL) ? (sk->net_ns).p : ((*out_skb)->dev->net_ns).p; \
	}

#endif /* NET_NAMESPACE_H_ */
