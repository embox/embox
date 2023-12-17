/**
 * @brief Software interrupt handler
 *
 * @date 15.12.22
 * @author Aleksey Zhmulin
 */
#include <arm/exception.h>
#include <util/log.h>

void arm_swi_handler(excpt_context_t *ctx) {
	log_notice("SWI handler not implemented");
}
