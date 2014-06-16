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
	int nports;
	int hexdump;
	int describe;
};

struct hba_port_reg {
	uint8_t offset;
	char name[100];
};

static struct hba_port_reg generic_hba_regs[] = {
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

static struct hba_port_reg generic_port_regs[] = {
	{0x0,   "COMMAND LIST BASE ADDR"},
	{0x4,   "COMMAND LIST BASE ADDR UPPER"},
	{0x8,   "FIS BASE ADDR"},
	{0xc,   "FIS BASE ADDR UPPER"},
	{0x10,  "IRQ STATUS"},
	{0x14,  "IRQ ENABLE"},
	{0x18,  "COMMAND AND STATUS"},
	{0x20,  "TASK FILE DATA"},
	{0x24,  "SIGNATURE"},
	{0x28,  "SATA STATUS"},
	{0x2c,  "SATA CONTROL"},
	{0x30,  "SATA ERROR"},
	{0x34,  "SATA ACTIVE"},
	{0x38,  "COMMAND ISSUE"},
	{0x3c,  "SATA NOTIFICATION"},
	{0x40,  "FIS SWITCHING CONTROL"},
	{0, ""},
};

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: lssata [-x] [-d] [-h]\n");
	printf("\t[-x] - dump regs\n");
	printf("\t[-d] - describe dumped regs (implies -x)\n");
	printf("\t[-h] - print this help\n");
}

static void print_error(void) {
	printf("Wrong parameters\n");
}

static void dump_port_regs(struct ahci_hba* hba, int port)
{
	int i = 0;
	uintptr_t offs = hba->base_addr + 0x100 + 0x80 * i;

	for (i = 0; i < 0x80; i += 4)
	{
		uint32_t val = *(volatile uint32_t*)(offs + i);
		if ((i/4) % 4 == 0)
			printf("%02X: ", i);
		printf("%08X ", val);
		if ((i/4) % 4 == 3)
			printf("\n");
	}
	printf("\n");
}

static void dump_port_regs_descr(struct ahci_hba* hba, int port)
{
	int i = 0;
	uintptr_t offs = hba->base_addr + 0x100 + 0x80 * i;

	while (generic_port_regs[i].name[0] != 0)
	{
		uint32_t val = *(volatile uint32_t*)(offs + generic_port_regs[i].offset);
		printf("%s (%x): %x\n", generic_port_regs[i].name, generic_port_regs[i].offset, val);
		i++;
	}
	printf("\n");
}

static void dump_ports(struct ahci_hba *hba, struct lssata_cb *c)
{
	int i;

	for (i = 0; i < hba->nports; i++)
	{
		printf("port #%d:\n", i);
		if (c->hexdump)
		{
			if (c->describe)
				dump_port_regs_descr(hba, i);
			else
				dump_port_regs(hba, i);
		}
	}
}

static void dump_sata_regs(struct ahci_hba* hba)
{
	int i = 0;

	for (i = 0; i < 0x100; i += 4)
	{
		uint32_t val = *(volatile uint32_t*)(hba->base_addr + i);
		if ((i/4) % 4 == 0)
			printf("%02X: ", i);
		printf("%08X ", val);
		if ((i/4) % 4 == 3)
			printf("\n");
	}
	printf("\n");
}

static void dump_sata_regs_descr(struct ahci_hba* hba)
{
	int i = 0;

	while (generic_hba_regs[i].name[0] != 0)
	{
		uint32_t val = *(volatile uint32_t*)(hba->base_addr + generic_hba_regs[i].offset);
		printf("%s (%x): %x\n", generic_hba_regs[i].name, generic_hba_regs[i].offset, val);
		i++;
	}
	printf("\n");
}

static int sata_callback(struct ahci_hba* hba, void *cookie) {
	struct lssata_cb *c = (struct lssata_cb*)cookie;

	printf("hba at %p:\n", (void*)hba->base_addr);
	if (c->hexdump)
	{
		if (c->describe)
			dump_sata_regs_descr(hba);
		else
			dump_sata_regs(hba);
	}

	dump_ports(hba, c);

	c->nhosts++;
	c->nports += hba->nports;
	return 0;
}

static int exec(int argc, char **argv) {
	int opt;
	int hexdump = 0;
	int describe = 0;
	struct lssata_cb cookie;

	if (argc > 1) {
		getopt_init();
		while (-1 != (opt = getopt(argc, argv, "xdh"))) {
			switch (opt) {
			case 'x':
				hexdump = 1;
				break;
			case 'd':
				hexdump = 1;
				describe = 1;
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
	}

	cookie.nhosts = 0;
	cookie.nports = 0;
	cookie.hexdump = hexdump;
	cookie.describe = describe;
	enumerate_ahci_hba(sata_callback, &cookie);

	printf("Total: %d sata hosts\n", cookie.nhosts);
	return 0;
}
