#include <embox/unit.h>
#include <drivers/ahci.h>

EMBOX_UNIT_INIT(ahci_ti8168_init);

static struct ahci_hba ti8168_hba[] = {
	{
		.base_addr = 0x4A140000,
	},
	{
		.base_addr = 0x4A0AE000,
	},
};

int ahci_ti8168_init(void) {
	register_ahci_hba(&ti8168_hba[0]);
	register_ahci_hba(&ti8168_hba[1]);
	return 0;
}
