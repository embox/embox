#include <stdio.h>
#include <net/net_namespace.h>
#include <net/netdevice.h>
#include <framework/cmd/api.h>

int make_ns_set_dev(const char *ns, struct net_device *v) {
	if (!(setns_by_name(ns) == 0 || unshare_by_name(ns) == 0)) {
		printf("failed to set ns\n");
		return -1;
	}
	netns_increment_ref_cnt(get_net_ns()); /* make ns persistent */
	dev_net_set(v, get_net_ns());
	printf("eth dev name: %s\n", v->name);
	return 0;
}

int main(int argc, char **argv) {
	struct net_device *v1, *v2, *ret;

	ret = veth_alloc(&v1, &v2);
	if (ret == NULL)
		return -1;

	if (make_ns_set_dev(argv[1], v1) != 0)
		return -1;
	return make_ns_set_dev(argv[2], v2);
}
