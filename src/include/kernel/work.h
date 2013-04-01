/**
 * @file
 * @brief Async work interface.
 *
 * @date 15.03.13
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_WORK_H_
#define KERNEL_WORK_H_

#include <util/slist.h>
#include <util/dlist.h>

#define WORK_F_DISABLED (0x1 << 0)  /**< Initially disabled work. */

struct work {
	struct slist_link pending_link;
	unsigned int state;
	void (*handler)(struct work *);
};

struct work_queue {
	struct dlist_head list;
};

extern struct work *work_init(struct work *, void (*worker)(struct work *),
		unsigned int flags);

extern struct work_queue *work_queue_init(struct work_queue *);

extern int work_enqueue(struct work_queue *, struct work *);
extern struct work *work_dequeue(struct work_queue *);

extern void work_cancel(struct work *);

#if 0

extern void work_disable(struct work *);
extern void work_enable(struct work *);
extern int work_disabled(struct work *);

extern void work_post(struct work *);
extern unsigned int work_pending(struct work *);
extern unsigned int work_pending_reset(struct work *);

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
#endif

#endif /* KERNEL_WORK_H_ */


