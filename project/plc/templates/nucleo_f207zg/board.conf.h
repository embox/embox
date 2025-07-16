#include <nucleo_f207zg.conf.h>

CONFIG {
	uarts[1].status = ENABLED;
	uarts[2].status = DISABLED;
	uarts[3].status = ENABLED;

	spis[1].status = ENABLED;

	leds[0].status = ENABLED;
	leds[1].status = ENABLED;
	leds[2].status = ENABLED;
}
