#include <stm32l152c_discovery.conf.h>

CONFIG {
	uarts[1].status = ENABLED;
	uarts[2].status = DISABLED;
	uarts[3].status = DISABLED;
	spis[1].status = ENABLED;
	i2cs[1].status = DISABLED;
	i2cs[2].status = DISABLED;
}
