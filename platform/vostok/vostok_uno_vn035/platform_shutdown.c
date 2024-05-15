#include <K1921VK035.h>
#include <system_K1921VK035.h>

#include <hal/platform.h>

void platform_shutdown(shutdown_mode_t mode) {
	switch (mode) {
	case SHUTDOWN_MODE_HALT:
	case SHUTDOWN_MODE_REBOOT:
	case SHUTDOWN_MODE_ABORT:
	default:
		NVIC_SystemReset();
		break;
	}

	/* NOTREACHED */
	while (1) {}
}
