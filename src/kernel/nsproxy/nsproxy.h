#ifndef NSPROXY_H_
#define NSPROXY_H_

#include <net/net_namespace.h>

typedef struct nsproxy {
	net_namespace_p net_ns;
} nsproxy_t;

extern struct nsproxy init_nsproxy;

#define set_task_proxy(tsk, parent) \
	if (parent) { \
		tsk->nsproxy = parent->nsproxy; \
	} else { \
		tsk->nsproxy = init_nsproxy; \
	}

#endif /* NSPROXY_H_ */
