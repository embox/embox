/**
 * @brief Fast interrupt handler
 *
 * @date 15.12.22
 * @author Aleksey Zhmulin
 */
#include <compiler.h>

#include <util/log.h>

#include "exception.h"

void _NORETURN aarch64_fiq_handler(struct excpt_context *ctx) {
	log_raw(LOG_EMERG, "\nUnexpected fiq interrupt!\n");
	aarch64_print_excpt_context(ctx);
	while (1) {};
}
