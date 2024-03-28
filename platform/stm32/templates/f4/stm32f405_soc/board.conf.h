#include <stm32f405_soc.conf.h>

CONFIG {
	uarts[1].status = ENABLED;
	uarts[2].status = DISABLED;
	uarts[3].status = ENABLED;

	spis[1].status = ENABLED;
}
