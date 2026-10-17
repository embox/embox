#include <k1921vg1t_flight.conf.h>
//#include <niiet_k1921vg1t_dev.conf.h>

CONFIG {
	uarts[0].status = ENABLED;
	uarts[1].status = ENABLED;
	uarts[2].status = ENABLED;
	uarts[3].status = ENABLED;
	uarts[4].status = ENABLED;

	spis[0].status = ENABLED;
	spis[1].status = ENABLED;
	spis[2].status = ENABLED;
	spis[3].status = ENABLED;

	i2cs[0].status = ENABLED;
	i2cs[1].status = ENABLED;
}
