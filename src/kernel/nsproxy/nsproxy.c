#include <kernel/nsproxy.h>

struct nsproxy init_nsproxy = {
	.net_ns = { .p = &init_net_ns_s }
};
