/**
 * @file
 *
 * @date    19.06.2020
 * @author  Alexander Kalmuk
 */

#ifndef ARCH_ARM_ARMMLIB_EXCEPTION_H_
#define ARCH_ARM_ARMMLIB_EXCEPTION_H_

#include <util/macro.h>

#ifndef __ASSEMBLER__
#include <module/embox/kernel/irq_static_impl.h>
#endif

#define PENDSV_IRQ       14
#define SYSTICK_IRQ      15

#define STATIC_EXC_ATTACH(_exc_nr, _hnd, _data) \
	STATIC_IRQ_HANDLER_DEF(_exc_nr, EXC_HANDLER_NAME(_exc_nr), _hnd, _data)

#define EXC_HANDLER_NAME(_exc_nr) \
	MACRO_CONCAT(exc_handler_, _exc_nr)

#endif /* ARCH_ARM_ARMMLIB_EXCEPTION_H_ */
