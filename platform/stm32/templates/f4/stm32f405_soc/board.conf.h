#include <stm32f405_soc.conf.h>

CONFIG {
	uarts[1].status = ENABLED;
	uarts[2].status = DISABLED;
	uarts[3].status = DISABLED;
	uarts[4].status = DISABLED;
	uarts[5].status = DISABLED;
	uarts[6].status = DISABLED;
	
	spis[1].status = ENABLED;
	spis[2].status = DISABLED;
}
