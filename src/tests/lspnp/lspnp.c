
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
#include "plug_and_play.h"

#define COMMAND_NAME "lspnp"

static char available_keys[] = {
#include "lspnp_keys.inc"
};

#define AMBA_BT_AHBM 1
#define AMBA_BT_AHBSL 2
#define AMBA_BT_APB 3
#define AMBA_BT_ALL 0

//static int bus_type;


static void show_help()
{
	printf(
	#include "lspnp_help.inc"
	);
}
typedef int (*FUNC_SHOW_BUS)(int dev_num);

static int show_ahbm(int dev_num){
	if(dev_num < 0)
	{
		print_ahbm_pnp_devs();
		return 0;
	}
	print_ahbm_pnp_dev(dev_num);
	return 0;
}
static int show_ahbsl(int dev_num){
	if(dev_num < 0)
	{
		print_ahbsl_pnp_devs();
		return 0;
	}
	print_ahbsl_pnp_dev(dev_num);
	return 0;
}
static int show_apb(int dev_num){
	if (dev_num < 0) {
		print_apb_pnp_devs();
		return 0;
	}
	print_apb_pnp_dev(dev_num);
	return 0;
}
static int show_all(int dev_num){
	show_ahbm(-1);
	show_ahbsl(-1);
	show_apb(-1);
}
static FUNC_SHOW_BUS set_bus_type(const char *key_value)
{
	if (0==strcmp(key_value, "ahbm")) 		return show_ahbm;
	else if (0==strcmp(key_value, "ahbsl"))	return  show_ahbsl;
	else if (0==strcmp(key_value, "apb"))		return  show_apb;
	else if (0==strcmp(key_value, "all"))		return  show_all;
	return NULL;
}

int lspnp_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	int dev_number = -1;

	FUNC_SHOW_BUS show_func;

	keys_amount = parse_arg(COMMAND_NAME, argsc, argsv, available_keys, sizeof(available_keys),
				keys);

	if (keys_amount < 0) {
		show_help();
		return -1;
	}

	if (get_key('h', keys, keys_amount, &key_value)) {
			show_help();
			return 0;
		}

	if (!get_key('b', keys, keys_amount, &key_value)) {
		key_value = "all";
	}
	if (NULL == (show_func = set_bus_type(key_value))){

		printf ("Parsing error: chose right bus type '-b'\n");
		show_help();
		return -1;
	}

	if (get_key('n', keys, keys_amount, &key_value))
	{
		if (show_all == show_func)
		{
			printf ("Parsing error: chose bus type '-b'\n");
			show_help();
			return -1;
		}
		if (1!=sscanf(key_value,"%d", &dev_number)) {

			printf ("Parsing error: enter validly dev_number '-b'\n");
			show_help();
			return -1;
		}
	}

	show_func(dev_number);

	return 0;
}
