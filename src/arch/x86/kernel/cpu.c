/**
 * @file
 * @brief
 *
 * @date 26.12.2012
 * @author Anton Bulychev
 */

#include <module/embox/driver/interrupt/lapic.h>

#include <kernel/cpu.h>

unsigned int cpu_get_id(void) {
	return lapic_id();
}
