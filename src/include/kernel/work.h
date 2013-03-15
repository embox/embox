/**
 * @file
 * @brief Stub API for asynch kernel workers.
 *
 * @date 15.03.13
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_WORK_H_
#define KERNEL_WORK_H_

/* XXX Stubs below. */
struct work;

static inline struct work *work_init(struct work *w, void (*worker)(struct work *))
	{ return NULL; }

static inline void work_post(struct work *w) {}
static inline int work_pending(struct work *w) { return 0; }
static inline int work_pending_reset(struct work *w) { return 0; }

static inline void work_disable(struct work *w) {}
static inline void work_enable(struct work *w) {}

/* XXX Stubs above. */

struct work {
	int (*handler)(struct work *);
};

extern struct work *work_init(struct work *, void (*worker)(struct work *));

extern void work_post(struct work *);
extern int work_pending(struct work *);
extern int work_pending_reset(struct work *);

extern void work_disable(struct work *);
extern void work_enable(struct work *);

/**
 * Evaluate a given @a expr inside a block with the specified @a work disabled.
 */
#define WORK_DISABLED_DO(work, expr) \
	({                             \
		typeof(expr) __ret;        \
		typeof(work) __w = (work); \
		work_disable(__w);         \
		__ret = (expr);            \
		work_enable(__w);          \
		__ret;                     \
	})


#endif /* KERNEL_WORK_H_ */
