#include <gen_board_conf.h>

struct led_conf leds[] = {
	[0] = {
		.status = ENABLED,
		.name = "VD2",
		.port = VAL("", GPIO_PORT_D),
		.pin = VAL("", 4),
		.level = VAL("", GPIO_PIN_LOW),
	},
	[1] = {
		.status = ENABLED,
		.name = "VD5",
		.port = VAL("", GPIO_PORT_D),
		.pin = VAL("", 5),
		.level = VAL("", GPIO_PIN_LOW),
	},
};

EXPORT_CONFIG(LED(leds))
