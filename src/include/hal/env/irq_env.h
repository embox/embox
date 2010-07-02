/**
 * @file
 *
 * @date 24.06.2010
 * @author Anton Bondarev
 */

#ifndef IRQ_ENV_H_
#define IRQ_ENV_H_

#include <kernel/irq.h>

struct irq_action {
	irq_handler_t handler;
	irq_flags_t flags;
	void *dev_id;
	const char *dev_name;
	unsigned int count_handled;
	unsigned int count_unhandled;
};

struct irq_entry {
	struct irq_action *action;
	unsigned int count;
};

typedef struct irq_env {
	struct irq_entry irq_table[IRQ_NRS_TOTAL];
}irq_env_t;

extern void irq_save_env(irq_env_t *env);

extern void irq_restore_env(irq_env_t *env);

extern void irq_set_env(irq_env_t *env);

extern irq_env_t *irq_get_env(void);

#endif /* IRQ_ENV_H_ */
