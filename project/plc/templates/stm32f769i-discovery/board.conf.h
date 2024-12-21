#include <stm32f769i_discovery.conf.h>

CONFIG {
	uarts[1].status = ENABLED;
	uarts[2].status = DISABLED;
	uarts[6].status = ENABLED;

	leds[0].status = ENABLED;
	leds[1].status = ENABLED;
}
