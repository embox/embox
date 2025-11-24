#include <stm32f4discovery.conf.h>

CONFIG {
	/* uarts */
	uarts[2].status = ENABLED;
	uarts[6].status = ENABLED;

	/* spis */
	spis[1].status  = ENABLED;
	spis[2].status  = DISABLED; /* Overlaps some Ethernet pins */
}
