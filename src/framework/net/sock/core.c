/**
 * @file
 * @brief
 *
 * @date 05.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#include <framework/mod/member/self.h>
#include <framework/mod/self.h>
#include <framework/net/sock/api.h>
#include <kernel/printk.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>

ARRAY_SPREAD_DEF(const struct net_sock, __net_sock_registry);

static int net_sock_mod_member_init(struct mod_member *member);
static int net_sock_mod_member_fini(struct mod_member *member);

const struct mod_member_ops __net_sock_mod_member_ops = {
	.init = &net_sock_mod_member_init,
	.fini = &net_sock_mod_member_fini
};

static const char * proto2str(int proto) {
	switch (proto) {
	default:           return "unkonwn";
	case IPPROTO_IP:   return "IP";
	case IPPROTO_ICMP: return "ICMP";
	case IPPROTO_TCP:  return "TCP";
	case IPPROTO_UDP:  return "UDP";
	}
}

static int net_sock_mod_member_init(struct mod_member *member) {
	int ret;
	const struct net_sock *nsock;

	ret = 0;
	nsock = (const struct net_sock *)member->data;

	printk("\tNET: initializing socket %s.%s for %s protocol: ",
			nsock->mod->package->name, nsock->mod->name,
			proto2str(nsock->protocol));

	if (nsock->init != NULL) {
		ret = nsock->init();
	}

	if (ret == 0) {
		printk("done\n");
	}
	else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}

static int net_sock_mod_member_fini(struct mod_member *member) {
	int ret;
	const struct net_sock *nsock;

	ret = 0;
	nsock = (const struct net_sock *)member->data;

	printk("\tNET: finalizing socket %s.%s for %s protocol: ",
			nsock->mod->package->name, nsock->mod->name,
			proto2str(nsock->protocol));

	if (nsock->fini != NULL) {
		ret = nsock->fini();
	}

	if (ret == 0) {
		printk("done\n");
	}
	else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}

const struct net_sock * net_sock_lookup(int family, int type,
		int protocol) {
	const struct net_sock *nsock;

	net_sock_foreach(nsock) {
		if ((nsock->family == family)
				&& (nsock->type == type)
				&& ((nsock->protocol == protocol)
					|| ((protocol == 0) && (nsock->is_default)))) {
			return nsock;
		}
	}

	return NULL;
}
