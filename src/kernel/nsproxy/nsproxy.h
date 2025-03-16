#ifndef KERNEL_NSPROXY_H_
#define KERNEL_NSPROXY_H_

#include <net/net_namespace.h>

typedef struct nsproxy {
	net_namespace_p net_ns;
} nsproxy_t;

extern struct nsproxy init_nsproxy;

#define set_task_proxy(tsk, parent)     \
	if (parent) {                       \
		tsk->nsproxy = parent->nsproxy; \
	}                                   \
	else {                              \
		tsk->nsproxy = init_nsproxy;    \
	}

extern void netns_decrement_ref_cnt(net_namespace_p netns);

#endif /* KERNEL_NSPROXY_H_ */
