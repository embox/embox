/**
 * @file
 * @brief Implementation of ARP Cache
 *
 * @date 12.08.2011
 * @author Ilia Vaprol
 */

#include <embox/unit.h>
#include <errno.h>
#include <kernel/timer.h>
#include <lib/list.h>
#include <mem/misc/pool.h>
#include <net/neighbour.h>
#include <stddef.h>
#include <string.h>

EMBOX_UNIT_INIT(unit_init);

/**
 * Wrapper for the neighbour entity
 */
struct neighbour_head {
	struct list_head lnk; /* pointer to next and previous element of the cache */
	uint32_t ctime;       /* time of life */
	struct neighbour n;   /* cache entity */
};

/**
 * Declaration of neighbour cache
 */
POOL_DEF(neighbour_pool, struct neighbour_head, CONFIG_ARP_CACHE_SIZE);

/**
 * List of valid entity
 */
static LIST_HEAD(used_neighbours_list);
//static LIST_HEAD(wait_neighbours_list); // TODO

static struct sys_timer *neighbour_refresh_timer;

/**
 * This function check expires of entity from neighbour cache
 */
static void neighbour_refresh(struct sys_timer *timer, void *param) {
	struct neighbour *entity;
	struct list_head *tmp;
	union {
		struct neighbour_head *nh;
		struct list_head *lh;
		void *v;
	} ptr;


	list_for_each_safe(ptr.lh, tmp, &used_neighbours_list) {
		entity = &ptr.nh->n;
		if (entity->flags == ATF_COM) {
			ptr.nh->ctime += NEIGHBOUR_CHECK_INTERVAL;
			if (ptr.nh->ctime >= NEIGHBOUR_TIMEOUT) {
				list_del(ptr.lh);
				pool_free(&neighbour_pool, ptr.v);
			}
		}
	}
}

uint8_t * neighbour_lookup(struct in_device *if_handler, in_addr_t ip_addr) {
	struct neighbour *entity;
	union {
		struct neighbour_head *nh;
		struct list_head *lh;
	} ptr;

	if (if_handler == NULL) {
		return NULL;
	}

	list_for_each(ptr.lh, &used_neighbours_list) {
		entity = &ptr.nh->n;
		if ((entity->ip_addr == ip_addr)
				&& (entity->if_handler == if_handler)) {
			return entity->hw_addr;
		}
	}

	return NULL;
}

int neighbour_add(struct in_device *if_handler, in_addr_t ip_addr,
		uint8_t *hw_addr, uint8_t flags) {
	struct neighbour *entity;
	union {
		struct neighbour_head *nh;
		struct list_head *lh;
		void *v;
	} ptr;

	if ((if_handler == NULL) || (hw_addr == NULL)) {
		return -EINVAL;
	}

	list_for_each(ptr.lh, &used_neighbours_list) {
		entity = &ptr.nh->n;
		if ((entity->ip_addr == ip_addr)
				&& (entity->if_handler == if_handler)) {
			break;
		}
	}

	if (ptr.lh == &used_neighbours_list) { /* if not found then alloc new */
		ptr.v = pool_alloc(&neighbour_pool);

		list_add_tail(ptr.lh, &used_neighbours_list);

		if (ptr.v == NULL) {
			return -ENOMEM;
		}
		entity = &ptr.nh->n;
	}

	ptr.nh->ctime = 0;
	memcpy(entity->hw_addr, hw_addr, ETH_ALEN);
	entity->if_handler = if_handler;
	entity->ip_addr = ip_addr;
	entity->flags = flags;


	return ENOERR;
}

int neighbour_delete(struct in_device *if_handler, in_addr_t ip_addr) {
	struct list_head *tmp;
	struct neighbour *entity;
	union {
		struct neighbour_head *nh;
		struct list_head *lh;
		void *v;
	} ptr;

	if (if_handler == NULL) {
		return -EINVAL;
	}

	list_for_each_safe(ptr.lh, tmp, &used_neighbours_list) {
		entity = &ptr.nh->n;
		if ((entity->ip_addr == ip_addr)
				&& (entity->if_handler == if_handler)) {
			list_del(ptr.lh);
			pool_free(&neighbour_pool, ptr.v);
			return ENOERR;
		}
	}

	return ENOERR; /* not found */
}

struct neighbour * neighbour_get_first(void) {
	union {
		struct neighbour_head *nh;
		struct list_head *lh;
	} ptr;

	if (list_empty(&used_neighbours_list)) {
		return NULL;
	}

	ptr.lh = used_neighbours_list.next;

	return &ptr.nh->n;
}

int neighbour_get_next(struct neighbour **pentity) {
	union {
		struct neighbour_head *nh;
		struct list_head *lh;
		unsigned char *b;
	} ptr;

	if ((pentity == NULL) || (*pentity == NULL)) {
		return -EINVAL;
	}

	ptr.b = (unsigned char *)*pentity - offsetof(struct neighbour_head, n);
	ptr.lh = ptr.lh->next; /* get next */
	*pentity = (ptr.lh != &used_neighbours_list) ? &ptr.nh->n : NULL; /* if end => NULL */

	return ENOERR;
}

static int unit_init(void) {
	int res;

	res = timer_set(&neighbour_refresh_timer, NEIGHBOUR_CHECK_INTERVAL, neighbour_refresh, NULL);
	if (res < 0) {
		return res;
	}
	return ENOERR;
}
