/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.11.22
 */

#include <compiler.h>

#include <arm/exception.h>
#include <util/log.h>

void _NORETURN arm_fiq_handler(excpt_context_t *ctx) {
	log_raw(LOG_EMERG, "\nUnresolvable fiq exception!\n");
	arm_show_excpt_context(ctx);
	while (1) {};
}
