/**
 * @file
 * @brief Async work interface.
 *
 * @date 15.03.13
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_WORK_H_
#define KERNEL_WORK_H_

#include <util/dlist.h>

struct work;

struct work_queue {
	struct dlist_head list;
};

struct work {
	struct dlist_head link;
	unsigned int state;

	struct work_queue *wq;
	int (*handler)(struct work *);
};

#define WORK_DISABLED 0x1

extern void work_init(struct work *w, int (*handler)(struct work *),
		unsigned int flags);

extern void work_queue_init(struct work_queue *wq);


extern void work_post(struct work *w , struct work_queue *wq);
extern void work_cancel(struct work *w);

extern void work_disable(struct work *w);
extern void work_enable(struct work *w);

extern void work_queue_run(struct work_queue *wq);


/*
extern unsigned int work_pending(struct work *);
extern unsigned int work_pending_reset(struct work *);
*/

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
