/**
 * @file irq.c
 *
 * @brief Common interrupting-related handling routines
 *
 * @author Eldar Abusalimov
 * @author Anton Bondarev
 * @author Alexey Fomin
 * @author Alexandr Batyukov
 */

#include <common.h>
#include <hal/irq_ctrl.h>
#include <asm/traps.h>
#include <kernel/irq.h>
#include <kernel/interrupt.h>
#include <asm/sys.h>
#include <string.h>
#include <autoconf.h>

static IRQ_INFO irq_infos[MAX_IRQ_NUMBER];

void irq_dispatch(uint32_t irq_number) {
	if ((array_len(irq_infos) < irq_number) || (NULL
			== irq_infos[irq_number].handler)) {
		return;
	}

	irq_infos[irq_number].handler(irq_number, irq_infos[irq_number].dev_id,
			NULL);
#if CONFIG_SOFT_IRQ
	irq_exit();/*soft irq and other*/
#endif
}

int irq_init_handlers(void) {
	irqc_init();
	return 0;
}

int request_irq(unsigned int irq_number, IRQ_HANDLER handler,
		unsigned long flags, const char *dev_name, void *dev_id) {
	if (irq_number != (irq_number & 0xF)) {
		return -1;
	}
	if ((NULL == handler) || (NULL != irq_infos[irq_number].handler)) {
		return -1;
	}
	irq_infos[irq_number].handler  = handler;
	irq_infos[irq_number].irq_num  = irq_number;
	irq_infos[irq_number].dev_id   = dev_id;
	irq_infos[irq_number].dev_name = dev_name;
	irq_infos[irq_number].flags    = flags;
	irq_infos[irq_number].enabled  = true;

	irqc_enable_irq(irq_number);
	return 0;
}

void free_irq(unsigned int irq, void *dev_id) {
	//TODO not realize now
}

//TODO may be it needs only for testing? and move it
bool irq_set_info(IRQ_INFO *irq_info) {
	IRQ_INFO old_irq_info;
	unsigned long psr;
	if (irq_info->irq_num != (irq_info->irq_num & (MAX_IRQ_NUMBER - 1))) {
		return false;
	}
	if (irq_info->enabled && irq_info->handler == NULL) {
		return false;
	}
	psr = local_irq_save();
	if (irq_info->enabled) {
		irqc_enable_irq(irq_info->irq_num);
	} else {
		irqc_disable_irq(irq_info->irq_num);
	}
	memcpy(&old_irq_info, &irq_infos[irq_info->irq_num], sizeof(IRQ_INFO));
	old_irq_info.irq_num = irq_info->irq_num;
	irqc_clear(irq_info->irq_num);
	memcpy(&irq_infos[irq_info->irq_num], irq_info, sizeof(IRQ_INFO));
	memcpy(irq_info, &old_irq_info, sizeof(IRQ_INFO));
	local_irq_restore(psr);
	return true;
}
