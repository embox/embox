#include <errno.h>
#include <net/net_namespace.h>

#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
int setns(const char *name) {
	return -ENOTSUP;
}

int unshare_by_name(const char *name) {
	return -ENOTSUP;
}

net_namespace_p empty_net_ns_p_obj;
net_namespace_p get_net_ns() {
	return empty_net_ns_p_obj;
}
#endif
