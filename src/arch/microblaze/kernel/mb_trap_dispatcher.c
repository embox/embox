/**
 * @file
 *
 * @date 17.03.2010
 * @author Anton Bondarev
 */
#include <types.h>
#include <hal/traps_core.h>
#include <test/testtraps_core.h>

trap_handler_t sotftrap_handler[MAX_SOFTTRAP_NUMBER];
trap_handler_t hwtrap_handler[MAX_HWTRAP_NUMBER];

void mb_exception_dispatcher(uint32_t number, void *data) {
	if (NULL == sotftrap_handler)
		return;
	sotftrap_handler[number](number, 0);
}

void mb_hwtrap_despatcher(uint32_t number, void *data) {

}
