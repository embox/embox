/**
 * @brief System error exception handler
 *
 * @date 15.12.22
 * @author Aleksey Zhmulin
 */
#include <compiler.h>

#include <util/log.h>

#include "exception.h"

void _NORETURN aarch64_serror_handler(struct excpt_context *ctx) {
	log_raw(LOG_EMERG, "\nSError exception!\n");
	aarch64_print_excpt_context(ctx);
	while (1) {};
}
