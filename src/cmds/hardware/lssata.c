/**
 * @file
 * @brief show list sata devices
 *
 * @date 27.04.14
 * @author Kirill Smirnov
 */
#include <stdio.h>
#include <unistd.h>
#include <embox/cmd.h>
#include <drivers/ahci.h>

struct lssata_cb {
	int nhosts;
	int full;
};

struct hba_reg {
	uint8_t offset;
	char name[100];
};

static struct hba_reg generic_hba_regs[] = {
	{0x0,  "HOST CAPABILITIES"},
	{0x4,  "GLOBAL HOST CONTROL"},
	{0x8,  "INTERRUPT STATUS"},
	{0xc,  "PORTS"},
	{0x10, "VERSION"},
	{0x14, "CMD COMPLETION COAL CONTROL"},
	{0x18, "CMD COMPLETION COAL PORTS"},
	{0x1c, "ENCLOSURE MGMT LOCATION"},
	{0x20, "ENCLOSURE MGMT CONTROL"},
	{0x24, "HOST CAPABILITIES EXTENDED"},
	{0x28, "OS HANDOFF CONTROL/STATUS"},
	{0, ""},
};

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: lssata [-f] [-h]\n");
	printf("\t[-f]      - full info\n");
	printf("\t[-h]      - print this help\n");
}

static void print_error(void) {
	printf("Wrong parameters\n");
}

static int sata_callback(struct ahci_hba* hba, void *cookie) {
	struct lssata_cb *c = (struct lssata_cb*)cookie;
	int i = 0;

	printf("hba at %p:\n", (void*)hba->base_addr);
	while (generic_hba_regs[i].name[0] != 0)
	{
		uint32_t val = *(volatile uint32_t*)(hba->base_addr + generic_hba_regs[i].offset);
		printf("%s (%x): %x\n", generic_hba_regs[i].name, generic_hba_regs[i].offset, val);
	}
	c->nhosts++;
	return 0;
}

static int exec(int argc, char **argv) {
	int opt;
	int full = 0;
	struct lssata_cb cookie;

	if (argc == 1) {
		full = 0;
	} else {
		getopt_init();
		while (-1 != (opt = getopt(argc, argv, "hf"))) {
			switch (opt) {
			case 'f':
				full = 1;
				break;
			case '?':
			case 'h':
				print_usage();
				return 0;
			default:
				print_error();
				return 0;
			}
		}
	}/*else have some parameters*/

	cookie.nhosts = 0;
	cookie.full = full;
	enumerate_ahci_hba(sata_callback, &cookie);

	printf("Total: %d sata hosts\n", cookie.nhosts);
	return 0;
}
