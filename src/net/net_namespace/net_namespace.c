
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <net/inetdevice.h>
#include <net/net_namespace.h>
#include <net/netdevice.h>
#include <kernel/task.h>
#include <mem/misc/pool.h>

#include <fs/inode.h>
#include <fs/file_desc.h>
#include <fs/dentry.h>

#include <embox/unit.h>

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

net_namespace_p net_ns_lookup(const char *name) {
	struct net_namespace *net_ns;
	net_namespace_p net_ns_p;

	dlist_foreach_entry_safe(net_ns, net_ns_list, lnk) {
		if (strcmp(name, net_ns->name) == 0) {
			assign_net_ns_p_and_ret(net_ns_p, net_ns);
		}
	}

	assign_net_ns_p_and_ret(net_ns_p, NULL);
}

net_namespace_p net_ns_lookup_by_inode(struct inode *inode) {
	struct net_namespace *net_ns;
	net_namespace_p net_ns_p;

	dlist_foreach_entry_safe(net_ns, net_ns_list, lnk) {
		if (net_ns->inode == inode) {
			assign_net_ns_p_and_ret(net_ns_p, net_ns);
		}
	}

	assign_net_ns_p_and_ret(net_ns_p, NULL);
}

static void netns_create_inode(net_namespace_p ns) {
	struct lookup lookup = {0, 0};

	dvfs_lookup("/proc", &lookup);
	ns.p->inode = inode_new(lookup.item->d_sb);
	ns.p->inode->i_mode = S_IFREG;
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
	net_ns_p.p = net_ns;
	memcpy(net_ns->name, name, strlen(name) + 1);

	ipl = ipl_save();
	dlist_add_prev_entry(net_ns, net_ns_list, lnk);
	ipl_restore(ipl);

	netns_create_inode(net_ns_p);

	net_ns->ref_cnt = 0;
	assign_net_ns_p_and_ret(net_ns_p, net_ns);
}

void free_net_ns(net_namespace_p netns) {
	ipl_t ipl;
	inode_del(netns.p->inode);
	ipl = ipl_save();
	dlist_del_init_entry(netns.p, lnk);
	pool_free((struct pool*)&net_ns_pool, netns.p);
	ipl_restore(ipl);
}

void netns_decrement_ref_cnt(net_namespace_p netns) {
	if (netns.p == NULL || netns.p == &init_net_ns_s)
		return;
	(netns.p->ref_cnt)--;
	if (netns.p->ref_cnt <= 0) {
		free_net_ns(netns);
	}
}

void netns_increment_ref_cnt(net_namespace_p netns) {
	if (netns.p == NULL)
		return;
	(netns.p->ref_cnt)++;
}

//User space:
int setns_by_name(const char *name) {
	struct task * task = task_self();
	char *init_name;

	if (task->nsproxy.net_ns.p != NULL) {
		init_name = task->nsproxy.net_ns.p->name;
		if (strcmp(init_name, name) != 0 &&
		    net_ns_lookup(init_name).p != NULL) {
			netns_decrement_ref_cnt(task->nsproxy.net_ns);
		}
	}

	if ((assign_net_ns(task->nsproxy.net_ns,
			   net_ns_lookup(name))) == NULL)
		return -1;

	netns_increment_ref_cnt(task->nsproxy.net_ns);

	return 0;
}

int unshare_by_name(const char *name) {
	net_namespace_p net_ns;

	net_ns = alloc_net_ns(name);
	if (net_ns.p == NULL) {
		return -1;
	}

	return setns_by_name(name);
}

int unshare() {
	char netns_name[NAME_MAX + 1];
	int i = 0;

	do {
		sprintf(netns_name, "netns%d", i++);
	} while ((net_ns_lookup(netns_name)).p != NULL);

	return unshare_by_name(netns_name);
}

#include <kernel/task.h>

#include <kernel/task/resource/idesc_table.h>
#include <kernel/task/resource/idesc.h>

#include <kernel/task/resource/index_descriptor.h>

int setns(int fd) {
	struct idesc *fd_idesc;
	struct net_namespace *net_ns;

	fd_idesc = index_descriptor_get(fd);

	dlist_foreach_entry_safe(net_ns, net_ns_list, lnk) {
		if (((struct file_desc *)fd_idesc)->f_inode == net_ns->inode) {
			netns_decrement_ref_cnt((task_self())->nsproxy.net_ns);
			(task_self())->nsproxy.net_ns.p = net_ns;
			netns_increment_ref_cnt((task_self())->nsproxy.net_ns);
			return 0;
		}
	}

	return -1;
}

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
	init_net_ns_s.ref_cnt = 1;
	return 0;
}
