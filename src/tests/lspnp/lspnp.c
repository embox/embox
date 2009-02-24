
/*
 * lspnp.c
 *
 *  Created on: 20.02.2009
 *      Author: abatyukov
 */

#include "shell.h"
#include "types.h"
#include "lspnp.h"
#include "string.h"

char lspnp_keys[] = {
#include "lspnp_keys.inc"
};
#define AHBM 1
#define AHBSL 2
#define APB 3
#define ALL 0

static int bus_type;

void set_bus_type(const char *key_value)
{
	if (strcmp(key_value, "ahbm")) 		bus_type = AHBM;
	else if (strcmp(key_value, "ahbsl"))	bus_type = AHBSL;
	else if (strcmp(key_value, "apb"))		bus_type = APB;
	else if (strcmp(key_value, "all"))		bus_type = ALL;
}

int lspnp_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	int dev;

	keys_amount = parse_arg("lspnp", argsc, argsv, lspnp_keys, sizeof(lspnp_keys),
				keys);

	if (keys_amount < 0) {
		printf(
#include "lspnp_help.inc"
		);
		return -1;
	}

	if (get_key('h', keys, keys_amount, &key_value)) {
			printf(
	#include "lspnp_help.inc"
			);
			return 0;
		}

	if (keys_amount == 0)
	{
		bus_type = ALL;
	}

	if (get_key('b', keys, keys_amount, &key_value)) {
		set_bus_type(key_value);
	}

	switch (bus_type)
	{
	case AHBM:
		if (get_key('n', keys, keys_amount, &key_value))
		{
			if (1==sscanf(key_value,"%d", &dev)){
				print_ahbm_pnp_dev(dev);
				return 0;
			}
		}
		else {
			print_ahbm_pnp_devs();
			return 0;
		}

	case AHBSL:
		if (get_key('n', keys, keys_amount, &key_value))
		{
			if (1==sscanf(key_value,"%d", &dev)) {
				print_ahbsl_pnp_dev(dev);
				return 0;
			}
		}
		else {
			print_ahbsl_pnp_devs();
			return 0;
		}
	case APB:
		if (get_key('n', keys, keys_amount, &key_value))
		{
			if (1==sscanf(key_value,"%d", &dev)) {
				print_apb_pnp_dev(dev);
				return 0;
			}
		}
		else {
			print_apb_pnp_devs();
			return 0;
		}
	case ALL:
		if (get_key('n', keys, keys_amount, &key_value))
		{
			printf("ERROR: lspnp_shell_handler: unexpected token.\n");
			printf(
#include "lspnp_help.inc"
				);
			return 0;
		}
		else {
			print_all_pnp_devs();
			return 0;
		}
	default:
		printf("ERROR: lspnp_shell_handler: unexpected token.\n");
		printf(
#include "lspnp_help.inc"
			);
	}

	return 0;
}
