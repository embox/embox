/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 15.05.23
 */
#include <stdbool.h>
#include <stddef.h>

#include <debug/breakpoint.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <util/dlist.h>

#define BPT_COUNT OPTION_GET(NUMBER, bpt_count)

extern const struct bpt_ops BPT_OPS_NAME(BPT_TYPE_SOFT);
extern const struct bpt_ops BPT_OPS_NAME(BPT_TYPE_HARD);
extern const struct bpt_ops BPT_OPS_NAME(BPT_TYPE_WATCH);

extern void bpt_handler_set(bpt_handler_t handler);

POOL_DEF(bpt_pool, struct bpt, BPT_COUNT);

static struct bpt_env __bpt_env = {
    .bpt_list =
        {
            DLIST_INIT(__bpt_env.bpt_list[BPT_TYPE_SOFT]),
            DLIST_INIT(__bpt_env.bpt_list[BPT_TYPE_HARD]),
            DLIST_INIT(__bpt_env.bpt_list[BPT_TYPE_WATCH]),
        },
    .handler = NULL,
    .bpts_enabled = false,
};

static const struct bpt_ops *__bpt_ops[3] = {
    &BPT_OPS_NAME(BPT_TYPE_SOFT),
    &BPT_OPS_NAME(BPT_TYPE_HARD),
    &BPT_OPS_NAME(BPT_TYPE_WATCH),
};

void bpt_env_init(struct bpt_env *env, bpt_handler_t handler, bool enable) {
	struct bpt_env tmp = {
	    .bpt_list =
	        {
	            DLIST_INIT(__bpt_env.bpt_list[BPT_TYPE_SOFT]),
	            DLIST_INIT(__bpt_env.bpt_list[BPT_TYPE_HARD]),
	            DLIST_INIT(__bpt_env.bpt_list[BPT_TYPE_WATCH]),
	        },
	    .handler = handler,
	    .bpts_enabled = enable,
	};

	memcpy(env, &tmp, sizeof(struct bpt_env));
}

void bpt_env_save(struct bpt_env *env) {
	memcpy(env, &__bpt_env, sizeof(struct bpt_env));
}

void bpt_env_restore(struct bpt_env *env) {
	if (__bpt_env.bpts_enabled) {
		bpt_disable_all();
		__bpt_env.bpts_enabled = true;
	}

	memcpy(&__bpt_env, env, sizeof(struct bpt_env));
	bpt_handler_set(__bpt_env.handler);

	if (__bpt_env.bpts_enabled) {
		bpt_enable_all();
	}
}

bool bpt_set(int type, void *addr) {
	struct bpt *bpt;

	if (!__bpt_ops[type]->set || !(bpt = pool_alloc(&bpt_pool))) {
		return false;
	}

	bpt->addr = addr;

	if (__bpt_ops[type]->prepare && !__bpt_ops[type]->prepare(bpt)) {
		pool_free(&bpt_pool, bpt);
		return false;
	}

	dlist_init(&bpt->list_item);
	dlist_add_next(&bpt->list_item, &__bpt_env.bpt_list[type]);

	if (__bpt_env.bpts_enabled) {
		__bpt_ops[type]->set(bpt);
	}

	return true;
}

bool bpt_remove(int type, void *addr) {
	struct bpt *bpt;

	if (!__bpt_ops[type]->remove) {
		return false;
	}

	dlist_foreach_entry(bpt, &__bpt_env.bpt_list[type], list_item) {
		if (addr == bpt->addr) {
			if (__bpt_env.bpts_enabled) {
				__bpt_ops[type]->remove(bpt);
			}

			if (__bpt_ops[type]->cleanup) {
				__bpt_ops[type]->cleanup(bpt);
			}

			dlist_del(&bpt->list_item);
			pool_free(&bpt_pool, bpt);

			return true;
		}
	}

	return false;
}

void bpt_remove_all(void) {
	struct bpt *bpt;
	int type;

	for (type = 0; type < MAX_BPT_TYPES; type++) {
		if (!__bpt_ops[type]->remove) {
			continue;
		}

		dlist_foreach_entry(bpt, &__bpt_env.bpt_list[type], list_item) {
			if (__bpt_env.bpts_enabled) {
				__bpt_ops[type]->remove(bpt);
			}

			dlist_del(&bpt->list_item);
			pool_free(&bpt_pool, bpt);
		}
	}
}

void bpt_enable_all(void) {
	struct bpt *bpt;
	int type;

	for (type = 0; type < MAX_BPT_TYPES; type++) {
		if (!__bpt_ops[type]->set) {
			continue;
		}

		if (__bpt_ops[type]->enable) {
			__bpt_ops[type]->enable();
		}

		dlist_foreach_entry(bpt, &__bpt_env.bpt_list[type], list_item) {
			__bpt_ops[type]->set(bpt);
		}
	}

	__bpt_env.bpts_enabled = true;
}

void bpt_disable_all(void) {
	struct bpt *bpt;
	int type;

	for (type = 0; type < MAX_BPT_TYPES; type++) {
		if (!__bpt_ops[type]->remove) {
			continue;
		}

		dlist_foreach_entry(bpt, &__bpt_env.bpt_list[type], list_item) {
			__bpt_ops[type]->remove(bpt);
		}

		if (__bpt_ops[type]->disable) {
			__bpt_ops[type]->disable();
		}
	}

	__bpt_env.bpts_enabled = false;
}
