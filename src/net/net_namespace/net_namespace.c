#include <net/net_namespace.h>
#include <net/netdevice.h>
#include <kernel/task.h>
#include <mem/misc/pool.h>
#include <embox/unit.h>
#include <net/inetdevice.h>

EMBOX_UNIT_INIT(set_init_net_ns);

#define MODOPS_AMOUNT_NET_NS_OBJS OPTION_GET(NUMBER, amount_net_ns)
POOL_DEF(net_ns_pool, struct net_namespace, MODOPS_AMOUNT_NET_NS_OBJS);

static DLIST_DEFINE(net_ns_list_d);
struct dlist_head *net_ns_list = &net_ns_list_d;

struct net_namespace init_net_ns_s;
net_namespace_p init_net_ns = { .p = &init_net_ns_s };

#define assign_net_ns_p_and_ret(net_ns_p, net_ns) \
	net_ns_p.p = net_ns; \
	return net_ns_p;

static net_namespace_p net_ns_lookup(const char *name) {
	struct net_namespace *net_ns;
	net_namespace_p net_ns_p;

	dlist_foreach_entry_safe(net_ns, net_ns_list, lnk) {
		if (strcmp(name, net_ns->name) == 0) {
			assign_net_ns_p_and_ret(net_ns_p, net_ns);
		}
	}

	assign_net_ns_p_and_ret(net_ns_p, NULL);
}

static net_namespace_p alloc_net_ns(const char *name) {
	ipl_t ipl;
	struct net_namespace *net_ns;
	net_namespace_p net_ns_p;
	size_t namelen;

	if ((namelen = strlen(name)) > NAME_MAX) {
		assign_net_ns_p_and_ret(net_ns_p, NULL);
	}

	if ((net_ns_lookup(name)).p != NULL) {
		assign_net_ns_p_and_ret(net_ns_p, NULL);
	}

	net_ns = pool_alloc((struct pool*)&net_ns_pool);
	if (net_ns == NULL) {
		assign_net_ns_p_and_ret(net_ns_p, NULL);
	}
	memcpy(net_ns->name, name, strlen(name) + 1);

	ipl = ipl_save();
	dlist_add_prev_entry(net_ns, net_ns_list, lnk);
	ipl_restore(ipl);

	assign_net_ns_p_and_ret(net_ns_p, net_ns);
}

//User space:
int setns(const char *name) {
	return (assign_net_ns((task_self())->nsproxy.net_ns,
			      net_ns_lookup(name))) == NULL ? -1 : 0;
}

int unshare(const char *name) {
	net_namespace_p net_ns;

	net_ns = alloc_net_ns(name);
	if (net_ns.p == NULL) {
		return -1;
	}

	return setns(name);
}
//

static int init_net_ns_devs(void) {
	static int init = 0;

	if (init)
		return 0;
	struct in_device *iface;
	for (iface = inetdev_get_first_all(); iface != NULL;
			iface = inetdev_get_next_all(iface)) {
		dev_net_set(iface->dev, init_net_ns);
	}
	init = 1;
	return 0;
}

net_namespace_p get_net_ns() {
	init_net_ns_devs();
	return (task_self())->nsproxy.net_ns;
}

static int set_init_net_ns(void) {
	ipl_t ipl;
	ipl = ipl_save();
	dlist_add_prev_entry(&init_net_ns_s, net_ns_list, lnk);
	ipl_restore(ipl);
	memcpy(init_net_ns_s.name, "init_net_ns", strlen("init_net_ns") + 1);
	return 0;
}
