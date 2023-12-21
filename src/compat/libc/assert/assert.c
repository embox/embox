/**
 * @file
 * @brief TODO
 *
 * @date 25.03.11
 * @author Eldar Abusalimov
 */

#include <framework/mod/options.h>
#include <hal/cpu.h>
#include <hal/platform.h>
#include <kernel/cpu/cpudata.h>
#include <kernel/panic.h>
#include <kernel/printk.h>
#include <kernel/spinlock.h>

#include "assert_impl.h"

#define BANNER_PRINT OPTION_GET(BOOLEAN, banner_print)

#ifndef NDEBUG
/*# error "Compiling assert.c for NDEBUG configuration"*/

char __assertion_message_buff[ASSERT_MESSAGE_BUFF_SZ];
static spinlock_t assert_lock = SPIN_STATIC_UNLOCKED;
static char assert_recursive_lock __cpudata__ = 0;

#if BANNER_PRINT
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
#endif

void __assertion_handle_failure(const struct __assertion_point *point) {
	if (cpudata_var(assert_recursive_lock)) {
		printk("\nrecursion detected on CPU %d\n", cpu_get_id());
		goto out;
	}
	cpudata_var(assert_recursive_lock) = 1;

	spin_lock_ipl_disable(&assert_lock);

#if BANNER_PRINT
	print_oops();
#endif
	printk(" ASSERTION FAILED on CPU %d\n" LOCATION_FUNC_FMT("\t", "\n") "\n"
	                                                                     "%s\n",
	    cpu_get_id(), LOCATION_FUNC_ARGS(&point->location), point->expression);

	if (*__assertion_message_buff)
		printk("\n\t(%s)\n", __assertion_message_buff);

	whereami();

	spin_unlock(&assert_lock); /* leave IRQs off */

out:
	platform_shutdown(SHUTDOWN_MODE_ABORT);
	/* NOTREACHED */
}

#endif
