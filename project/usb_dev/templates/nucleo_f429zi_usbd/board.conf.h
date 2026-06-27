#include <nucleo_f429zi.conf.h>

CONFIG {
	/* uarts */
	uarts[2].status = DISABLED;
	uarts[3].status = ENABLED;
	uarts[6].status = ENABLED;

	/* spis */
	spis[1].status  = ENABLED;
	spis[2].status  = DISABLED;
}
