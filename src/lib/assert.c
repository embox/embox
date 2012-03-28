/**
 * @file
 * @brief TODO
 *
 * @date 25.03.11
 * @author Eldar Abusalimov
 */

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

void __assertion_handle_failure(const struct __assertion_point *point) {
	const struct location_func *loc = &point->location;

	printk("\nASSERTION FAILED at %s : %d,\n"
			"\tin function %s:\n\n"
			"%s\n",
			loc->at.file, loc->at.line, loc->func,
			point->expression);
	if (*__assertion_message_buff) {
		printk("\t(%s)\n", __assertion_message_buff);
	}
	panic("kernel debug panic");
}

#endif
