/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */

#include <util/log.h>

#include <stdint.h>
#include <sys/mman.h>

#include <embox/unit.h>
#include <hal/mmu.h>

#include <util/binalign.h>
#include <util/math.h>

int vmem_mmu_enabled(void) {
	return 0;
}

int vmem_init_context(mmu_ctx_t *ctx) {
	return 0;
}

mmu_ctx_t vmem_current_context(void) {
	return 0;
}

int vmem_create_context(mmu_ctx_t *ctx) {
	return 0;
}

void vmem_free_context(mmu_ctx_t ctx) {
}
