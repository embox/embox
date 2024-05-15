#include <assert.h>

#include <K1921VK035.h>
#include <system_K1921VK035.h>

#include <hal/platform.h>
#include <hal/system.h>

// static_assert(SYS_CLOCK == 100000000);

void platform_init(void) {
	SystemInit();
	SystemCoreClockUpdate();
}
