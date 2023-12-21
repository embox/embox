/**
 * @brief
 *
 * @date 20.12.23
 * @author Aleksey Zhmulin
 */
#include <hal/arch.h>
#include <hal/platform.h>

void platform_idle(void) {
	arch_idle();
}
