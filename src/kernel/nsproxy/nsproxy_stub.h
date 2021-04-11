#ifndef NSPROXY_H_
#define NSPROXY_H_

#include <net/net_namespace.h>

typedef struct nsproxy {
} nsproxy_t;

#define set_task_proxy(tsk, parent)
#define netns_decrement_ref_cnt(tsk)

#endif /* NSPROXY_H_ */
