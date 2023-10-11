/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.08.23
 */
#ifndef SRC_LIB_BREAKPOINT_H_
#define SRC_LIB_BREAKPOINT_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/cdefs.h>

#include <util/dlist.h>

#include <module/embox/lib/breakpoint.h>

#define BPT_TYPE_SOFT  0
#define BPT_TYPE_HARD  1
#define BPT_TYPE_WATCH 2

#define MAX_BPT_TYPES 3

#define BPT_OPS_NAME(type) __##type##_ops

#define SW_BREAKPOINT_OPS_DEF(...) \
	const struct bpt_ops BPT_OPS_NAME(BPT_TYPE_SOFT) = __VA_ARGS__

#define HW_BREAKPOINT_OPS_DEF(...) \
	const struct bpt_ops BPT_OPS_NAME(BPT_TYPE_HARD) = __VA_ARGS__

#define WATCHPOINT_OPS_DEF(...) \
	const struct bpt_ops BPT_OPS_NAME(BPT_TYPE_WATCH) = __VA_ARGS__

typedef void (*bpt_handler_t)(struct bpt_context *ctx);

struct bpt {
	struct dlist_head list_item;
	void *addr;
	union {
		bpt_instr_t orig;
		short num;
	};
};

struct bpt_ops {
	bool (*prepare)(struct bpt *bpt);
	void (*cleanup)(struct bpt *bpt);
	void (*set)(struct bpt *bpt);
	void (*remove)(struct bpt *bpt);
	void (*enable)(void);
	void (*disable)(void);
};

struct bpt_env {
	struct dlist_head bpt_list[MAX_BPT_TYPES];
	bpt_handler_t handler;
	bool bpts_enabled;
};

__BEGIN_DECLS

extern void bpt_env_init(struct bpt_env *env, bpt_handler_t handler,
    bool enable);
extern void bpt_env_save(struct bpt_env *env);
extern void bpt_env_restore(struct bpt_env *env);

extern bool bpt_set(int type, void *addr);
extern bool bpt_remove(int type, void *addr);
extern void bpt_remove_all(void);
extern void bpt_enable_all(void);
extern void bpt_disable_all(void);

__END_DECLS

#endif /* SRC_LIB_BREAKPOINT_H_ */
