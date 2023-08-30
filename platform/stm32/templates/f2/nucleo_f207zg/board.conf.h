#include <nucleo_f207zg.conf.h>

CONFIG {
	uarts[1].status = ENABLED;
	uarts[2].status = DISABLED;
	uarts[3].status = ENABLED;

	spis[1].status = ENABLED;
}
