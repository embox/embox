/**
 * @file
 * @brief TODO
 *
 * @date 25.03.11
 * @author Eldar Abusalimov
 */

#include <hal/cpu.h>

#include <kernel/spinlock.h>
#include <kernel/panic.h>
#include <kernel/printk.h>

#ifndef NDEBUG
/*# error "Compiling assert.c for NDEBUG configuration"*/

#include "assert_impl.h"

#ifdef CONFIG_ASSERTION_MESSAGE_BUFF_SZ
# define MESSAGE_BUFF_SZ CONFIG_ASSERTION_MESSAGE_BUFF_SZ
#else
# define MESSAGE_BUFF_SZ 128
#endif

char __assertion_message_buff[MESSAGE_BUFF_SZ];

static spinlock_t assert_lock = SPIN_STATIC_UNLOCKED;

void __assertion_handle_failure(const struct __assertion_point *point) {
	spin_lock_ipl_disable(&assert_lock);

	printk("\n"
			"\n  ______"
			"\n |  ____|"
			"\n | |___   _ __ ___"
			"\n |  ___| | \'_ ` _ \\"
			"\n | |____.| | | | | |  __  __  __"
			"\n |______||_| |_| |_| /./ /./ /./"
			"\n\n"

			"  ASSERTION  FAILED   on CPU %d\n"
			LOCATION_FUNC_FMT("\t", "\n") "\n"
			"%s\n",

			cpu_get_id(),
			LOCATION_FUNC_ARGS(&point->location),
			point->expression);

	if (*__assertion_message_buff)
		printk("\n\t(%s)\n", __assertion_message_buff);

	whereami();

	spin_unlock(&assert_lock);  /* leave IRQs off */

	arch_shutdown(ARCH_SHUTDOWN_MODE_ABORT);
	/* NOTREACHED */
}

#endif
