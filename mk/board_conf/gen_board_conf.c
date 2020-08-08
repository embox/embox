#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "gen_board_conf.h"

/* It's conf/board.conf.h */
#include <board.conf.h>

static int gen_dev_enabled(const char *dev_name) {
	char buf[128];
	char def[64];

	sprintf(def, "#define CONF_%s_ENABLED", dev_name);
	sprintf(buf, "%-50s %d", def, 1);
	printf("%s\n", buf);
}

static int gen_field_int(const char *dev_name,
		const char *prop_name, const struct field_int *f) {
	char buf[128];
	char def[64];

	if (!f->name) {
		return -1;
	}

	if (!strcmp(f->name, "")) {
		sprintf(def, "#define CONF_%s_%s",
			dev_name, prop_name);
	} else {
		sprintf(def, "#define CONF_%s_%s_%s",
			dev_name, prop_name, f->name);
	}
	sprintf(buf, "%-50s %s", def, f->val);
	printf("%s\n", buf);

	return 0;
}

static int gen_field_func(const char *dev_name,
		const char *prop_name, const struct field_int *f) {
	char buf[128];
	char def[64];

	if (!f->name) {
		return -1;
	}

	if (!strcmp(f->name, "")) {
		sprintf(def, "#define CONF_%s_%s()",
			dev_name, prop_name);
	} else {
		sprintf(def, "#define CONF_%s_%s_%s()",
			dev_name, prop_name, f->name);
	}
	sprintf(buf, "%-50s %s", def, f->val);
	printf("%s\n", buf);

	return 0;
}

static int gen_field_pin(const char *dev_name,
		const char *prop_name, const struct field_pin *f) {
	char buf[128];
	char def[64];

	if (!f->name) {
		return -1;
	}

	sprintf(def, "#define CONF_%s_%s_%s_PORT",
		dev_name, prop_name, f->name);
	sprintf(buf, "%-50s %s", def, f->port);
	printf("%s\n", buf);

	sprintf(def, "#define CONF_%s_%s_%s_NR",
		dev_name, prop_name, f->name);
	sprintf(buf, "%-50s %s", def, f->n);
	printf("%s\n", buf);

	sprintf(def, "#define CONF_%s_%s_%s_AF",
		dev_name, prop_name, f->name);
	sprintf(buf, "%-50s %s", def, f->af);
	printf("%s\n", buf);

	return 0;
}

int main() {
	int i, j;
	struct conf_item *uart_conf = &board_config[UART_IDX];
	const struct uart_conf *uart;

	config();

	printf("#ifndef BOARD_CONFIG_H_\n");
	printf("#define BOARD_CONFIG_H_\n\n");

	for (i = 0; i < uart_conf->array_size; i++) {
		uart = &((const struct uart_conf *) uart_conf->ptr)[i];;

		if (uart->status != ENABLED) {
			continue;
		}

		gen_dev_enabled(uart->name);

		for (j = 0; j < ARRAY_SIZE(uart->dev.irqs); j++) {
			if (gen_field_int(uart->name,
					"IRQ", &uart->dev.irqs[j]) != 0) {
				break;
			}
		}

		for (j = 0; j < ARRAY_SIZE(uart->dev.pins); j++) {
			if (gen_field_pin(uart->name,
					"PIN", &uart->dev.pins[j]) != 0) {
				break;
			}
		}

		for (j = 0; j < ARRAY_SIZE(uart->dev.clocks); j++) {
			if (gen_field_func(uart->name,
					"CLK_ENABLE", &uart->dev.clocks[j]) != 0) {
				break;
			}
		}

		printf("\n");
	}

	printf("#endif /* BOARD_CONFIG_H_ */\n");

	return 0;
}
