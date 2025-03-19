/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.12.24
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <plc/core.h>

static void print_help_msg(const char *cmd) {
	fprintf(stderr, "Usage: %s [CONFIG]\n", cmd);
}

static void plc_var_info(int type, void *data, const char **format, int *val) {
	switch (type) {
	case PLC_PHYS_VAR_QX:
	case PLC_SLAVE_VAR_QX:
		*format = "QX";
		*val = *(uint8_t *)data;
		break;

	case PLC_PHYS_VAR_IX:
	case PLC_SLAVE_VAR_IX:
		*format = "IX";
		*val = *(uint8_t *)data;
		break;

	case PLC_PHYS_VAR_IW:
	case PLC_SLAVE_VAR_IW:
		*format = "IW";
		*val = *(uint16_t *)data;
		break;

	case PLC_PHYS_VAR_QW:
		*format = "QW";
		*val = *(uint16_t *)data;
		break;

	case PLC_SLAVE_VAR_MW:
		*format = "MW";
		*val = *(uint16_t *)data;
		break;
	}
}

static void print_plc_vars(const struct plc_config *config) {
	const struct plc_slave_var *slave_var;
	const struct plc_phys_var *phys_var;
	const char *format;
	int val;

	plc_phys_var_foreach(phys_var, config) {
		plc_var_info(phys_var->type, phys_var->data, &format, &val);
		printf("\t%%%s%d.%d.%d = %d\n", format, phys_var->port, phys_var->pin,
		    phys_var->alt, val);
	}

	plc_slave_var_foreach(slave_var, config) {
		plc_var_info(slave_var->type, slave_var->data, &format, &val);
		printf("\t%%%s%d = %d\n", format, slave_var->addr, val);
	}
}

int main(int argc, char **argv) {
	const struct plc_config *config;

	if (argc == 1) {
		printf("Configs:\n");
		plc_config_foreach(config) {
			printf("\t%s\n", config->name);
		}
		return 0;
	}
	else if (argc == 2) {
		plc_config_foreach(config) {
			if (!strcmp(config->name, argv[1])) {
				print_plc_vars(config);
				return 0;
			}
		}
	}

	print_help_msg(argv[0]);
	return -1;
}
