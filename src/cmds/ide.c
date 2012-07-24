/**
 * @file
 * @brief
 *
 * @date 20.07.2012
 * @author Andrey Gazukin
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>
#include <drivers/ata.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: ide \n");
}

static void print_drive (ide_ata_slot_t *ide) {
	for(int i  = 0; i < 4; i++) {
		printf("\nide%02d:", i);
		if(NULL == ide->ide_bus[i].dev_ide_ata) {
			printf(" none");
		}
		else {
			printf(" %s", ide->ide_bus[i].dev_ide_ata->identification.sn);
			printf(" %s",
				ide->ide_bus[i].dev_ide_ata->identification.model_numb);
		}
	}

}


static int exec(int argc, char **argv) {
	int opt;

	getopt_init();
	while (-1 != (opt = getopt(argc - 1, argv, "ah"))) {
		switch(opt) {
		case 'a':
			break;
		case 'h':
			print_usage();
			return 0;
		default:
			return -1;
		}
	}

	print_drive(detection_drive());
	return 0;
}
