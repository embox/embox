#include <stm32f4ve.conf.h>

CONFIG {
	/* uarts */
	uarts[1].status = ENABLED;
	uarts[2].status = DISABLED;
	uarts[6].status = DISABLED;

	/* spis */
//	spis[1].status  = ENABLED;
//	spis[2].status  = DISABLED; /* Overlaps some Ethernet pins */
}
