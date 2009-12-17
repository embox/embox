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
#include <asm/sys.h>
#include <string.h>

#if 0
static IRQ_HANDLER irq_handlers[MAX_IRQ_NUMBER];
#endif

static IRQ_INFO irq_infos[MAX_IRQ_NUMBER];

void irq_dispatch(uint32_t irq_number) {
//        if (array_len(irq_handlers) < irq_number ||
//    	    NULL == irq_handlers[irq_number]) {
//                return;
//        }
//		irq_handlers[irq_number]();
        irq_infos[irq_number].handler(irq_number, irq_infos[irq_number].dev_id, NULL);
}

int irq_init_handlers() {
        irqc_init();
        return 0;
}

bool irq_set_info(IRQ_INFO *irq_info) {
        IRQ_INFO old_irq_info;
        unsigned long psr;
        if (irq_info->irq_num != irq_info->irq_num & (MAX_IRQ_NUMBER - 1)) {
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
#if 0
        // check IRQ number
        if (irq_info->irq_num != irq_info->irq_num & 0xF) {
                return false;
        }
        if (irq_info->enabled && irq_info->handler == NULL) {
                return false;
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

        return true;
#endif
}

int request_irq(unsigned int irq_number, IRQ_HANDLER handler, unsigned long flags,
		const char *dev_name, void *dev_id) {

	if (irq_number != irq_number & 0xF) {
             return -1;
     }
     if ((NULL == handler) || (NULL != irq_infos[irq_number].handler)) {
             return -1;
     }
     irq_infos[irq_number].handler = handler;

     irq_infos[irq_number].irq_num;

     irq_infos[irq_number].dev_id = dev_id;
     irq_infos[irq_number].dev_name = dev_name;
     irq_infos[irq_number].flags = flags;
     irq_infos[irq_number].enabled = true;

     irqc_enable_irq(irq_number);
     return 0;
}

void free_irq(unsigned int irq, void *dev_id) {
	//TODO not realize now
}

#if 0
int request_irq(uint8_t irq_number, IRQ_HANDLER handler) {
        // check IRQ number
        if (irq_number != irq_number & 0xF) {
                return -1;
        }
        if ((NULL == handler) || (NULL != irq_handlers[irq_number])) {
                return -1;
        }
        //TODO may be clear pending bit?
        irq_handlers[irq_number] = handler;
        irqc_enable_irq(irq_number);
        return 0;
}
#endif

#if 0
void irq_set_handler(uint8_t irq_number, IRQ_HANDLER pfunc) {
        // check IRQ number
        if (irq_number != irq_number & 0xF) {
                return;
        }

        //	IRQ_HANDLER old = user_trap_handlers[IRQ_TRAP_TYPE(irq_number)];
        user_trap_handlers[IRQ_TRAP_TYPE(irq_number)] = pfunc;
        if (pfunc != NULL) {
                irqc_enable_irq(irq_number);
        } else {
                irqc_disable_irq(irq_number);
        }
}

IRQ_HANDLER irq_get_handler(uint8_t irq_number) {
        return user_trap_handlers[IRQ_TRAP_TYPE(irq_number)];
}
#endif
