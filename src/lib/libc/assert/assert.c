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

static const char oops_banner[] =
	"\n  ______"
	"\n |  ____|                                            __          __"
	"\n | |___  _ __ ___            ____  ____  ____  _____/ /   _____ / /"
	"\n |  ___|| \'_ ` _ \\          / __ \\/ __ \\/ __ \\/ ___/ /   |_____| |"
	"\n | |____| | | | | |_ _ _   / /_/ / /_/ / /_/ (__  )_/    |_____| |"
	"\n |______|_| |_| |_(_|_|_)  \\____/\\____/ .___/____(_)           | |"
	"\n                                     /_/                        \\_\\"
	"\n";

static void print_oops(void) {
	printk("\n%s", oops_banner);
}

void __assertion_handle_failure(const struct __assertion_point *point) {
	spin_lock_ipl_disable(&assert_lock);

	print_oops();
	printk(
		" ASSERTION FAILED on CPU %d\n"
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
