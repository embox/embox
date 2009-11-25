/**
 * \file irq.c
 *
 * \brief Common interrupting-related handling routines
 *
 * \author Eldar Abusalimov
 * \author Anton Bondarev
 * \author Alexey Fomin
 * \author Alexandr Batyukov
 */

#include "types.h"
#include "common.h"
#include "hal/irq_ctrl.h"
#include "asm/traps.h"
#include "kernel/irq.h"
#include "asm/sys.h"
#include "string.h"

static IRQ_HANDLER irq_handlers[MAX_IRQ_NUMBER];
void irq_dispatch(uint32_t irq_number){
	if ((array_len(irq_handlers)) < irq_number){
		return;
	}
	if (NULL == irq_handlers[irq_number]){
		return;
	}
	irq_handlers[irq_number]();
}

int irq_init_handlers() {
	irqc_init();
	return 0;
}

BOOL irq_set_info(IRQ_INFO *irq_info) {
	IRQ_INFO old_irq_info;
	unsigned long psr;
	// check IRQ number
	if (irq_info->irq_num != irq_info->irq_num & 0xF) {
		return FALSE;
	}
	if (irq_info->enabled && irq_info->handler == NULL) {
		return FALSE;
	}

	psr = local_irq_save();
	  old_irq_info.irq_num = irq_info->irq_num;
	  old_irq_info.handler = irq_handlers[irq_info->irq_num];
	  old_irq_info.enabled = irqc_get_status(irq_info->irq_num);

	  irq_handlers[irq_info->irq_num] = irq_info->handler;
	  if (irq_info->enabled) {
		  irqc_enable_irq(irq_info->irq_num);
	  } else {
		  irqc_disable_irq(irq_info->irq_num);
	  }
	  memcpy(irq_info, &old_irq_info, sizeof(IRQ_INFO));
	  irqc_clear(irq_info->irq_num);
	local_irq_restore(psr);

	return TRUE;
}

int request_irq(BYTE irq_number, IRQ_HANDLER handler) {
	// check IRQ number
	if (irq_number != irq_number & 0xF) {
		return -1;
	}
	if ((NULL == handler) || (NULL != irq_handlers[irq_number])){
		return -1;
	}
	//TODO may be clear pending bit?
	irq_handlers[irq_number] = handler;
	irqc_enable_irq(irq_number);
	return 0;
}

#if 0
void irq_set_handler(BYTE irq_number, IRQ_HANDLER pfunc) {
	// check IRQ number
	if (irq_number != irq_number & 0xF) {
		return;
	}

	//	IRQ_HANDLER old = user_trap_handlers[IRQ_TRAP_TYPE(irq_number)];
	user_trap_handlers[IRQ_TRAP_TYPE(irq_number)] = pfunc;
	if (pfunc != NULL) {
		LOG_DEBUG("set irq=%d\n", irq_number);
		irqc_enable_irq(irq_number);
	} else {
		irqc_disable_irq(irq_number);
	}
}

IRQ_HANDLER irq_get_handler(BYTE irq_number) {
	return user_trap_handlers[IRQ_TRAP_TYPE(irq_number)];
}
#endif
