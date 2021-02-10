#include <nucleo_l476rg.conf.h>

CONFIG {
	/* uarts */
	uarts[1].status = DISABLED;
	uarts[2].status = ENABLED;

	/* spis */
	spis[1].status = ENABLED;
	spis[2].status = DISABLED;
}
