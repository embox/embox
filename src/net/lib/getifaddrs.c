/**
 * @file
 * @brief
 *
 * @date 06.09.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <netinet/in.h>


#include <ifaddrs.h>

#include <framework/mod/options.h>

#include <mem/misc/pool.h>
#include <net/if.h>
#include <net/inetdevice.h>

#include <lib/libds/array.h>
#include <util/member.h>

union sockaddr_tuple {
	struct sockaddr_in in;
	struct sockaddr_in6 in6;
};

struct ifaddrs_tuple {
	struct ifaddrs ifa;
	char name[IFNAMSIZ];
	union sockaddr_tuple addr[3]; /* 0-addr, 1-netmask, 2-broadaddr or dstaddr */
};

#define MODOPS_AMOUNT_IFADDRS OPTION_GET(NUMBER, amount_ifaddrs)

POOL_DEF(ifaddrs_tuple_pool, struct ifaddrs_tuple, MODOPS_AMOUNT_IFADDRS);

static struct ifaddrs * ifa_alloc(void) {
	struct ifaddrs_tuple *ifa_t;

	ifa_t = pool_alloc(&ifaddrs_tuple_pool);
	if (ifa_t == NULL) {
		return NULL;
	}

	ifa_t->ifa.ifa_name = &ifa_t->name[0];
	ifa_t->ifa.ifa_addr = (struct sockaddr *)&ifa_t->addr[0];
	ifa_t->ifa.ifa_netmask = (struct sockaddr *)&ifa_t->addr[1];
	ifa_t->ifa.ifa_dstaddr = (struct sockaddr *)&ifa_t->addr[2];

	return &ifa_t->ifa;
}

static void ifa_free(struct ifaddrs *ifa) {
	struct ifaddrs_tuple *ifa_t;

	assert(ifa != NULL);

	ifa_t = member_cast_out(ifa, struct ifaddrs_tuple, ifa);
	pool_free(&ifaddrs_tuple_pool, ifa_t);
}

static void sa_init(struct sockaddr *sa, sa_family_t family,
		void *addr) {
	union sockaddr_tuple *sa_t;

	assert(sa != NULL);
	sa_t = (union sockaddr_tuple *)sa;

	switch (family) {
	default:
		sa->sa_family = family;
		break;
	case AF_INET:
		assert(addr != NULL);
		memset(&sa_t->in, 0, sizeof sa_t->in);
		sa_t->in.sin_family = family;
		memcpy(&sa_t->in.sin_addr, addr,
				sizeof sa_t->in.sin_addr);
		break;
	case AF_INET6:
		assert(addr != NULL);
		memset(&sa_t->in6, 0, sizeof sa_t->in6);
		sa_t->in6.sin6_family = family;
		memcpy(&sa_t->in6.sin6_addr, addr,
				sizeof sa_t->in6.sin6_addr);
		break;
	}
}

int getifaddrs(struct ifaddrs **out_ifa) {
	struct ifaddrs *ifa, **ifa_ptr;
	struct in_device *in_dev;

	ifa_ptr = &ifa;

	in_dev = inetdev_get_first();
	while (in_dev != NULL) {
		*ifa_ptr = ifa_alloc();
		if (*ifa_ptr == NULL) {
			if (ifa) {
				freeifaddrs(ifa);
			}
			return SET_ERRNO(ENOMEM);
		}

		strncpy((*ifa_ptr)->ifa_name, &in_dev->dev->name[0],
				member_sizeof(struct ifaddrs_tuple, name));
		(*ifa_ptr)->ifa_name[member_sizeof(struct ifaddrs_tuple,
				name) - 1] = '\0';
		(*ifa_ptr)->ifa_flags = in_dev->dev->flags;
		sa_init((*ifa_ptr)->ifa_addr, AF_INET,
				&in_dev->ifa_address);
		sa_init((*ifa_ptr)->ifa_netmask, AF_INET,
				&in_dev->ifa_mask);
		sa_init((*ifa_ptr)->ifa_broadaddr, AF_INET,
				&in_dev->ifa_broadcast);
		(*ifa_ptr)->ifa_data = NULL;

		ifa_ptr = &(*ifa_ptr)->ifa_next;
		in_dev = inetdev_get_next(in_dev);
	}

	*ifa_ptr = NULL;

	*out_ifa = ifa;

	return 0;
}

void freeifaddrs(struct ifaddrs *ifa) {
	struct ifaddrs *tmp;

	assert(ifa != NULL);

	do {
		tmp = ifa->ifa_next;
		ifa_free(ifa);
		ifa = tmp;
	} while (ifa != NULL);
}
